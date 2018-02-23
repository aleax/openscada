#!/bin/sh

NDK_PATH=/home/roman/Android/Ndk
HOST=linux-x86_64
TARGET=arm-linux-androideabi
PLATFORM=android-19
PLATFORM_LIB_VAR=armeabi-v7a
TARGET_V=4.9
PREFIX=$NDK_PATH/toolchains/$TARGET-$TARGET_V/prebuilt/$HOST

#Soft float
# -march=armv5te -mfloat-abi=softfp -mfpu=neon -mthumb\
export CFLAGS="-O2 -ffunction-sections -funwind-tables -no-canonical-prefixes\
 -march=armv7-a -mfloat-abi=softfp -mfpu=vfp\
 -fno-builtin-memmove -fomit-frame-pointer -fno-strict-aliasing -finline-limit=64 -Wa,--noexecstack -Wformat -Werror=format-security"
export LDFLAGS="-llog -landroid -Wl,--fix-cortex-a8"
#Hard float
#export CFLAGS="-O2 -ffunction-sections -funwind-tables -no-canonical-prefixes\
# -march=armv7-a -mfpu=vfp -mhard-float\
# -fomit-frame-pointer -fno-strict-aliasing -finline-limit=64 -Wa,--noexecstack -Wformat -Werror=format-security"
#export LDFLAGS="-llog -landroid -Wl,--no-warn-mismatch -lm_hard -Wl,--fix-cortex-a8"
# -Wl,--no-undefined"

export CFLAGS="$CFLAGS --sysroot=$NDK_PATH/platforms/$PLATFORM/arch-arm/ -I$NDK_PATH/sources/crystax/include\
 -I$NDK_PATH/sources/pcre/include\
 -I$NDK_PATH/sources/openssl/include\
 -I$NDK_PATH/sources/expat/include\
 -I$NDK_PATH/sources/libpng/include\
 -I$NDK_PATH/sources/libjpeg/include\
 -I$NDK_PATH/sources/freetype/include\
 -I$NDK_PATH/sources/fontconfig/include\
 -I$NDK_PATH/sources/libgd2/include\
 -I$NDK_PATH/sources/fftw3/include\
 -I$NDK_PATH/sources/libiconv/include\
 -I$NDK_PATH/sources/gettext/include\
 -I$NDK_PATH/sources/sqlite/include"
export LDFLAGS="$LDFLAGS -L$NDK_PATH/sources/crystax/libs/$PLATFORM_LIB_VAR\
 -L$NDK_PATH/sources/pcre/libs/$PLATFORM_LIB_VAR\
 -L$NDK_PATH/sources/openssl/libs/$PLATFORM_LIB_VAR\
 -L$NDK_PATH/sources/expat/libs/$PLATFORM_LIB_VAR\
 -L$NDK_PATH/sources/libpng/libs/$PLATFORM_LIB_VAR\
 -L$NDK_PATH/sources/libjpeg/libs/$PLATFORM_LIB_VAR\
 -L$NDK_PATH/sources/freetype/libs/$PLATFORM_LIB_VAR\
 -L$NDK_PATH/sources/fontconfig/libs/$PLATFORM_LIB_VAR\
 -L$NDK_PATH/sources/libgd2/libs/$PLATFORM_LIB_VAR\
 -L$NDK_PATH/sources/fftw3/libs/$PLATFORM_LIB_VAR\
 -L$NDK_PATH/sources/libiconv/libs/$PLATFORM_LIB_VAR\
 -L$NDK_PATH/sources/gettext/libs/$PLATFORM_LIB_VAR\
 -L$NDK_PATH/sources/sqlite/libs/$PLATFORM_LIB_VAR"
export CXXFLAGS="$CFLAGS -std=c++11 -I$NDK_PATH/sources/cxx-stl/gnu-libstdc++/$TARGET_V/include\
 -I$NDK_PATH/sources/cxx-stl/gnu-libstdc++/$TARGET_V/libs/$PLATFORM_LIB_VAR/include\
 -L$NDK_PATH/sources/cxx-stl/gnu-libstdc++/$TARGET_V/libs/$PLATFORM_LIB_VAR -lgnustl_shared"
#-lgnustl_static -fexceptions -frtti"

# export PKG_CONFIG="pkg-config --static"
export PKG_CONFIG_PATH=$NDK_PATH/sources/pkgconfig
export PATH=$PREFIX/bin:$PATH

#Extended for not autotool
export CC=$TARGET-gcc
export LD=$TARGET-ld
export AS=$TARGET-as
export AR=$TARGET-ar

#Build OpenSCADA
AndroidPrjDir=$(pwd)
OpenSCADADir=$AndroidPrjDir/../..
BuildDir=$AndroidPrjDir/build_dir

cd $OpenSCADADir

# Patches apply
for iPatch in $(ls $AndroidPrjDir/patches/openscada_*.patch 2> /dev/null); do patch -N -p0 < $iPatch; done

# Same build
#make clean
./configure --host=arm-linux-androideabi \
    --prefix=/data/data/org.oscada --bindir=/data/data/org.oscada/bin --sysconfdir=/data/data/org.oscada --datarootdir=/data/data/org.oscada \
    CoreLibNmToDlOpen=liboscada.so --disable-CoreLibVersion --disable-AllModuls \
    --enable-SQLite=incl --enable-DBF=incl \
    --enable-Sockets=incl --enable-Serial=incl --enable-SSL \
    --enable-SelfSystem=incl --enable-UserProtocol=incl --enable-HTTP=incl \
    --enable-JavaLikeCalc=incl --enable-LogicLev=incl --enable-BlockCalc=incl --enable-System --enable-OPC_UA --enable-DAQGate=incl --enable-ModBus=incl --enable-DCON=incl \
    --enable-FSArch=incl --enable-DBArch=incl \
    --with-qt5=yes --enable-QtMainThrd --enable-QTStarter --enable-QTCfg --enable-Vision \
    --enable-VCAEngine --enable-WebCfg --enable-WebCfgD --enable-WebVision --enable-WebUser \
    --enable-FLibSYS=incl --enable-SystemTests --enable-FLibMath --enable-FLibComplex1
rm -R $BuildDir
DESTDIR=$BuildDir make -j4 install

#Update the Android files
dirSrc=$BuildDir/data/data/org.oscada

# Strip all libraries
arm-linux-androideabi-strip $dirSrc/lib/liboscada.so
arm-linux-androideabi-strip $dirSrc/lib/openscada/*.so

# Install the libraries with the needed prefixes
install -m 755 $dirSrc/lib/liboscada.so $AndroidPrjDir/libs/armeabi-v7a/
for fit in `ls $dirSrc/lib/openscada/*.so`; do
    install -m 755 $fit $AndroidPrjDir/libs/armeabi-v7a/liboscada_`basename $fit`
done

# Prepare assets
install -m 755 -d $AndroidPrjDir/assets/icons
cp $AndroidPrjDir/libs.xml.in $AndroidPrjDir/res/values/libs.xml

install -m 755 $dirSrc/bin/openscada-proj $AndroidPrjDir/assets/

# Install projects and standard configurations
cp $dirSrc/oscada.xml $AndroidPrjDir/assets/
cp $dirSrc/oscada_start.xml $AndroidPrjDir/assets/
install -m 755 -d $AndroidPrjDir/assets/LibsDB
cp $dirSrc/openscada/LibsDB/* $AndroidPrjDir/assets/LibsDB/
cp $dirSrc/oscada_AGLKS.xml $AndroidPrjDir/assets/
install -m 755 -d $AndroidPrjDir/assets/AGLKS
cp $dirSrc/openscada/AGLKS/* $AndroidPrjDir/assets/AGLKS/
cp $dirSrc/oscada_Boiler.xml $AndroidPrjDir/assets/
install -m 755 -d $AndroidPrjDir/assets/Boiler
cp $dirSrc/openscada/Boiler/* $AndroidPrjDir/assets/Boiler/

# Install icons
for fit in `ls $dirSrc/openscada/icons/*`; do
    install -m 644 $fit $AndroidPrjDir/assets/icons/`basename $fit`
    echo "\\t<item>icons/$(basename $fit):../openscada/icons/$(basename $fit)</item>" >> $AndroidPrjDir/res/values/libs.xml
done

# Install the translations
install -m 755 -d $AndroidPrjDir/assets/locale
cp -R $dirSrc/locale/* $AndroidPrjDir/assets/locale/
for fit in `find $AndroidPrjDir/assets/locale/ -name "*.mo"`; do
    fit1=`echo $fit | sed -n "/.*\/assets\/\(.*\)/s//\1/p"`
    echo "\\t<item>$fit1:../$fit1</item>" >> $AndroidPrjDir/res/values/libs.xml
done

echo "    </array>\\n</resources>" >> $AndroidPrjDir/res/values/libs.xml

#Make the APK package
cd $AndroidPrjDir
ant clean
ant debug
