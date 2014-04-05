AC_PREREQ([2.50])

# After LTS relese append third stabilisation level/number to the version
AC_INIT([OpenSCADA],[0.9],[rom_as@oscada.org, rom_as@fromru.com])

# Increase major for next Work version.
# Increase minor after significant changes into API of the Work version
# Full freeze for LTS stabilisation levels/updates, after the release
oscd_LIBREVISION="3:0:0"

AC_PREFIX_DEFAULT([/usr])
oscd_modulpref="oscd_"

AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE(AC_PACKAGE_TARNAME, AC_PACKAGE_VERSION)
AM_CONFIG_HEADER(src/config.h)

AC_LANG(C++)

# Perform program name transformation
#AC_ARG_PROGRAM

# Enable options
AC_ENABLE_SHARED(yes)
AC_ENABLE_STATIC(yes)
AC_LIBTOOL_DLOPEN

# AM_GNU_GETTEXT

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

if test "$ac_cv_prog_cc_g" = "no" -o "$ac_cv_prog_cxx_g" = "no"; then
    AC_MSG_ERROR([No C or C++ compiler present. Install GCC and G++.]);
fi

AC_CHECK_PROG(have_pkg_config, pkg-config, yes, no)
if test x"$have_pkg_config" = "xno"; then
    AC_MSG_ERROR(pkg-config is required to configure this program)
fi
PKG_PROG_PKG_CONFIG

AM_SANITY_CHECK

# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([time.h signal.h semaphore.h fcntl.h math.h errno.h syslog.h unistd.h dirent.h stdio.h stdarg.h stdlib.h dlfcn.h locale.h langinfo.h sys/types.h sys/stat.h sys/time.h sys/utsname.h pthread.h],[],
    [AC_MSG_ERROR(Some GLibC headers not found. Install or check GLibC developing package!)])
AC_CHECK_HEADERS([string vector map exception typeinfo deque],[],
    [AC_MSG_ERROR([Some standard template libs' STL headers not found. Install or check STL-StdC++ developing package!])])
AC_CHECK_HEADERS([iconv.h],[ ],[AC_MSG_NOTICE(iconv.h header not found. Using is disabled!)])
AC_CHECK_HEADERS([libintl.h],[ ],[AC_MSG_NOTICE(libintl.h header not found. Using is disabled!)])

#Gettext utilites check
if test $ac_cv_header_libintl_h = yes; then
    AC_CHECK_PROG(XGETTEXT, xgettext, xgettext, false)
    AC_CHECK_PROG(MSGMERGE, msgmerge, msgmerge, false)
    AC_CHECK_PROG(MSGFMT, msgfmt, msgfmt, false)
    if test $XGETTEXT = false -o $MSGMERGE = false -o $MSGFMT = false; then
	AC_MSG_ERROR([Some gettext I18N utilite no present! Install gettext utilites package.]);
    fi
    AC_CHECK_LIB([intl],[bindtextdomain],[LIB_CORE="${LIB_CORE}-lintl "])
fi

if test $ac_cv_header_iconv_h = yes; then
    AC_CHECK_LIB([iconv],[libiconv_open],[LIB_CORE="${LIB_CORE}-liconv "])
fi

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_OFF_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM

AC_SUBST(LIB_CORE)

ix86=no;
if test $target_cpu = "i386" -o $target_cpu = "i486" -o $target_cpu = "i586" -o $target_cpu = "i686" -o \
    $target_cpu = "k6" -o $target_cpu = "athlon" -o $target_cpu = "pentium2" -o $target_cpu = "pentium3" -o \
    $target_cpu = "pentium4"; then ix86=yes;
fi

AM_CONDITIONAL([ResourcesAllow],[test -d data])

# Checks for library functions.
AC_FUNC_CLOSEDIR_VOID
AC_FUNC_MKTIME
AC_FUNC_SELECT_ARGTYPES
AC_TYPE_SIGNAL
AC_FUNC_STAT
#AC_CHECK_FUNCS([getcwd gethostbyname inet_ntoa memmove memset mkdir nl_langinfo pow select setenv setlocale socket strdup strerror strtol uname])
AC_ISC_POSIX

oscd_moduldir=\${libdir}/${PACKAGE}
oscd_datadir=\${datadir}/${PACKAGE}
oscd_moddir_full=${libdir}/${PACKAGE}
oscd_datadir_full=${datadir}/${PACKAGE}
AC_SUBST(oscd_LIBREVISION)
AC_SUBST(oscd_moduldir)
AC_SUBST(oscd_modulpref)
AC_SUBST(oscd_datadir)
AX_DEFINE_DIR([bindir_full],[bindir],[Binaries location for configurations])
AX_DEFINE_DIR([sysconfdir_full],[sysconfdir],[Configure location for configurations])
AX_DEFINE_DIR([libdir_full],[libdir],[Libraries location for configurations])
AX_DEFINE_DIR([datadir_full],[datadir],[Data location for configurations])
AX_DEFINE_DIR([localedir_full],[localedir],[I18N locales location for configurations])
AX_DEFINE_DIR([oscd_datadir_full],[oscd_datadir_full],[OpenSCADA data location for configurations])
AX_DEFINE_DIR([oscd_moddir_full],[oscd_moddir_full],[OpenSCADA modules location for configurations])

AC_CONFIG_FILES(Makefile openscada.pc
    src/Makefile src_call/Makefile src/moduls/Makefile
    src/moduls/arhiv/Makefile src/moduls/bd/Makefile src/moduls/protocol/Makefile
    src/moduls/special/Makefile src/moduls/daq/Makefile src/moduls/transport/Makefile
    src/moduls/ui/Makefile)

if test -d data; then
    AC_CONFIG_FILES(data/openscada.desktop data/openscada_start data/oscada.xml
	data/ModelsDB/AGLKS/openscada_AGLKS.desktop data/ModelsDB/Boiler/openscada_Boiler.desktop)
fi

#====== Generic functions enable/disable ======
AC_ARG_ENABLE(CrossCompile,AC_HELP_STRING([--enable-CrossCompile], [enable crosscompile build]),[ ],
[ if test $host = $build; then enable_CrossCompile=no; else enable_CrossCompile=yes; fi])
AC_ARG_ENABLE(CoreLibStatic,AC_HELP_STRING([--enable-CoreLibStatic], [enable OpenSCADA core lib no shared, only static.]),
[ liboscada_la_LIBTOOLFLAGS="--tag=disable-shared"; AC_SUBST(liboscada_la_LIBTOOLFLAGS) ],[ enable_CoreLibStatic=no; ])
AC_ARG_ENABLE(LibGD,AC_HELP_STRING([--disable-LibGD], [disable using library LibGD in core]),[ ],[ enable_LibGD=yes; ])
AC_ARG_ENABLE(AllModuls,AC_HELP_STRING([--enable-AllModuls], [all modules enable(yes)/including(incl)/for dist(dist)/disable(no) for next separated selection]),
[ ],[ enable_AllModuls=individual; ])

AC_MSG_RESULT(========== Core libraries check =============)
# Check for libraries
if test $enable_CrossCompile = no; then
    AC_FUNC_MALLOC
    AC_FUNC_REALLOC
fi
AC_CHECK_LIB([m],[sin],[AC_MSG_NOTICE([LibM: Pass global library using])],
    [AC_MSG_ERROR(Math library not found. Install or check GLibC core package!)])
AC_CHECK_LIB([dl],[dlopen],[AC_MSG_NOTICE([LibDl: Pass global library using])],
    [AC_MSG_ERROR(Dynamik linking library not found. Install or check GLibC core package!)])
#AC_CHECK_LIB([rt],[pthread_create],[AC_MSG_NOTICE(Pass global library using)],
#    [AC_MSG_ERROR(Pthread library not found. Install or check GLibC core package!)])
AC_CHECK_LIB([crypt],[crypt],[AC_MSG_NOTICE([LibCrypt: Pass global library using])],
    [AC_MSG_ERROR(Crypt library not found. Install or check GLibC core package!)])
AC_CHECK_LIB([z],[deflate],[AC_MSG_NOTICE([LibZ: Pass global library using])],
    [AC_MSG_ERROR(Z library not found. Install or check libz package!)])

#> Check for GD library use in OpenSCADA core
if test $enable_LibGD = yes; then
    AC_CHECK_HEADERS([gd.h],
	[
	    AC_CHECK_LIB([gd],[gdImageCreate],
	    [
		AC_DEFINE(HAVE_GD_CORE,1,[LibGD: Pass library using into core])
		LIB_CORE="${LIB_CORE}-lgd "
	    ],[AC_MSG_ERROR(GD library not found. Install or check GDlib package!)])
	],
	[AC_MSG_ERROR(GD library headers not found. Install or check GD developing package!)])
fi

#> Check for PCRE library use in OpenSCADA core
CXXFLAGS="$CXXFLAGS $(pkg-config --cflags libpcre)"
AC_CHECK_HEADERS([pcre.h],[],
    [AC_MSG_ERROR(PCRE header not found. Install or check PCRE developing package!)])
AC_CHECK_LIB([pcre],[pcre_compile],[AC_MSG_NOTICE([PCRE: Pass global library using])],
    [AC_MSG_ERROR(PCRE library not found. Install or check PCRE package!)])

AC_MSG_RESULT(========== Build modules check =============)
#> DB modules
AC_SUBST(DBSub_mod)
AX_MOD_DB_EN(DBF,[disable or enable[=incl] build module DB.DBF],disable,incl)
AX_MOD_DB_EN(MySQL,[disable or enable[=incl] build module DB.MySQL],disable,incl,[
    AX_LIB_MYSQL
    if test "$found_mysql" != "yes"; then AC_MSG_ERROR(DB.MySQL: MySQL development resources not found. Install or check MySQL development package!); fi
])
AX_MOD_DB_EN(SQLite,[disable or enable[=incl] build module DB.SQLite],disable,incl,[
    PKG_CHECK_MODULES([sqlite3],[sqlite3 > 3],
	[AC_CHECK_LIB([sqlite3],[sqlite3_open],[],[AC_MSG_ERROR(SQLite3 library is not usable!)])],
	[AC_MSG_ERROR(DB.SQLite: SQLite3 development resources not found. Install or check SQLite3 development package!)])
])
AX_MOD_DB_EN(FireBird,[disable or enable[=incl] build module DB.FireBird],disable,incl,[
    AX_LIB_FIREBIRD([2.0.0])
    if test "$success" != "yes"; then AC_MSG_ERROR(DB.FireBird: FireBird development resources not found. Install or check FireBird development package!); fi
])
AX_MOD_DB_EN(PostgreSQL,[disable or enable[=incl] build module DB.PostgreSQL],disable,incl,[
    AX_LIB_POSTGRESQL([8.3.0])
    if test "$found_postgresql" != "yes"; then AC_MSG_ERROR(DB.PostgreSQL: PostgreSQL development resources not found. Install or check PostgreSQL development package!); fi
])
#> DAQ modules
AC_SUBST(DAQSub_mod)
AX_MOD_DAQ_EN(System,[disable or enable[=incl] build module DAQ.System],disable,incl,[
    AX_LIB_SENSORS()
])
AX_MOD_DAQ_EN(BlockCalc,[disable or enable[=incl] build module DAQ.BlockCalc],disable,incl)
AX_MOD_DAQ_EN(JavaLikeCalc,[disable or enable[=incl] build module DAQ.BlockCalc],disable,incl,[
    AC_PROG_YACC
    AC_CHECK_PROG(YACC_OK, bison, true, false)
    if test $YACC_OK = false; then
	AC_MSG_ERROR([No syntax generator present! Install YACC like programm bison.])
    fi
])
if test $ix86 != yes; then enable_DiamondBoards=no; fi
AX_MOD_DAQ_EN(DiamondBoards,[enable build module DAQ.DiamondBoards],enable)
AX_MOD_DAQ_EN(LogicLev,[disable or enable[=incl] build module DAQ.LogicLev],disable,incl)
AX_MOD_DAQ_EN(SNMP,[disable build module DAQ.SNMP],disable,[],[
    AC_CHECK_HEADERS([net-snmp/net-snmp-config.h],[],
	[AC_MSG_ERROR(DAQ.SNMP: Net SNMP library headers not found. Install or check of Net SNMP developing package!)])
#    AC_CHECK_LIB([netsnmp],[snmp_sess_init],[AC_MSG_NOTICE(Pass global library using)],
#	[AC_MSG_ERROR(DAQ.SNMP: Net SNMP API library not found. Install or check Net SNMP library package!)],
#	[crypto])
])
AX_MOD_DAQ_EN(Siemens,[disable build module DAQ.Siemens],disable,[],
[
    AC_CHECK_HEADERS([ctype.h],[],
	[AC_MSG_ERROR(DAQ.Siemens: Some GLibC network headers not found. Install or check GLibC developing package!)])
    AC_CHECK_HEADERS([linux/kd.h],[],
	[AC_MSG_ERROR(DAQ.Siemens: Linux kernel headers not found. Install or check Linux kernel headers package!)])
])
AX_MOD_DAQ_EN(ModBus,[disable or enable[=incl] build module DAQ.ModBus],disable,incl)
AX_MOD_DAQ_EN(DCON,[disable or enable[=incl] build module DAQ.DCON],disable,incl)
AX_MOD_DAQ_EN(DAQGate,[disable or enable[=incl] build module DAQ.DAQGate],disable,incl)
AX_MOD_DAQ_EN(SoundCard,[disable build module DAQ.SoundCard],disable,[],[
    AC_CHECK_HEADERS([portaudio.h],[],
	[AC_MSG_ERROR(DAQ.SoundCard: Some Portaudio headers not found. Install or check Portaudio developing package!)])
    AC_CHECK_LIB([portaudio],[Pa_Initialize],[AC_MSG_NOTICE([LibPortaudio: Pass global library using])],
	[AC_MSG_ERROR(Portaudio library not found. Install or check Portaudio library package!)])
])
if test $ix86 != yes -a $target_cpu != "arm"; then enable_ICP_DAS=no; fi
AX_MOD_DAQ_EN(ICP_DAS,[enable build module DAQ.ICP_DAS],enable,[],[
    ICP_DAS_CFLAGS="-I./include"
    ICP_DAS_LIBS="-L./ -li8k"
    if test $target_cpu = "arm"; then ICP_DAS_LIBS="-L./ -li8k_arm"; ICP_DAS_CFLAGS="-I./include_arm"; fi
    AC_SUBST(ICP_DAS_CFLAGS)
    AC_SUBST(ICP_DAS_LIBS)
])
AX_MOD_DAQ_EN(OPC_UA,[disable build module DAQ.OPC_UA],disable,[],
    [AX_LIB_OpenSSL()],
    [AC_CONFIG_FILES(src/moduls/daq/OPC_UA/libOPC_UA/Makefile)])
AX_MOD_DAQ_EN(AMRDevs,[disable build module DAQ.AMRDevs],disable)
AX_MOD_DAQ_EN(BFN,[enable build module DAQ.BFN],enable)
AX_MOD_DAQ_EN(Comedi,[enable build module DAQ.Comedi],enable,[],[
    PKG_CHECK_MODULES([comedilib],[comedilib > 0.7.0],[],
	[AC_MSG_ERROR(Comedilib not found. Install or check Comedilib library package!)])
])
AX_MOD_DAQ_EN(SMH2Gi,[enable build module DAQ.BFN],enable)
AX_MOD_DAQ_EN(OneWire,[enable build module DAQ.OneWire],enable,incl,[
    OneWire_CFLAGS="-I${oldincludedir}/owfs"
    OneWire_LDLAGS="-lowcapi"
    AC_SUBST(OneWire_CFLAGS)
    AC_SUBST(OneWire_LDLAGS)
    AC_CHECK_LIB([owcapi],[OW_init],[],[AC_MSG_ERROR(OWCAPI library is not usable!)])
])
AX_MOD_DAQ_EN(MMS,[enable build module DAQ.MMS],enable,[],[],
    [AC_CONFIG_FILES(src/moduls/daq/MMS/libMMS/Makefile)])
AX_MOD_DAQ_EN(EPICS,[enable build module DAQ.EPICS],enable,[],[],[])
#> Archive modules
AC_SUBST(ArchSub_mod)
AX_MOD_Archive_EN(FSArch,[disable or enable[=incl] build module Archive.FSArch],disable,incl)
AX_MOD_Archive_EN(DBArch,[disable or enable[=incl] build module Archive.DBArch],disable,incl)
#> Transport modules
AC_SUBST(TranspSub_mod)
AX_MOD_Transport_EN(Sockets,[disable or enable[=incl] build module Transport.Sockets],disable,incl,[
    AC_CHECK_HEADERS([sys/un.h sys/socket.h sys/select.h arpa/inet.h netinet/in.h netdb.h],[],
	[AC_MSG_ERROR(Transport.Sockets: Some GLibC network headers not found. Install or check GLibC developing package!)])
])
AX_MOD_Transport_EN(SSL,[disable or enable[=incl] build module Transport.SSL],disable,incl,[
    AX_LIB_OpenSSL()
])
AX_MOD_Transport_EN(Serial,[disable or enable[=incl] build module Transport.Serial],disable,incl)
#> Transport protocol modules
AC_SUBST(ProtSub_mod)
AX_MOD_TrProt_EN(HTTP,[disable or enable[=incl] build module Protocol.HTTP],disable,incl)
AX_MOD_TrProt_EN(SelfSystem,[disable or enable[=incl] build module Protocol.SelfSystem],disable,incl)
AX_MOD_TrProt_EN(UserProtocol,[disable or enable[=incl] build module Protocol.UserProtocol],disable,incl)
#> UI modules
AC_SUBST(UISub_mod)
AX_MOD_UI_EN(VCAEngine,[disable or enable[=incl] build module UI.VCAEngine],disable,incl)
AX_MOD_UI_EN(Vision,[disable or enable[=incl] build module UI.Vision],disable,incl,[
    AX_LIB_FFTW3()
    AX_LIB_Qt()

    AC_ARG_ENABLE(WebKit,AC_HELP_STRING([--disable-WebKit], [disable using WebKit library]),[ ],[
	AS_IF([test "x${Qt5_use}" = "x"],[
	    PKG_CHECK_MODULES([QtWebKit],[QtWebKit > 4.4.0],
		[AC_DEFINE(HAVE_WEBKIT,1,[WebKit web browser engine allow.])],
		[AC_MSG_NOTICE(WebKit not found. Using is disabled!)])
	],[
	    PKG_CHECK_MODULES([Qt5WebKitWidgets],[Qt5WebKitWidgets > 5.1.0],
		[
		    AC_DEFINE(HAVE_WEBKIT,1,[WebKit web browser engine allow.])
		    QtWebKit_CFLAGS=$Qt5WebKitWidgets_CFLAGS
		    QtWebKit_LIBS=$Qt5WebKitWidgets_LIBS
		],
		[AC_MSG_NOTICE(WebKit not found. Using is disabled!)])
	])
    ])

    AS_IF([test "x${Qt5_use}" = "x"],[
	AC_ARG_ENABLE(Phonon,AC_HELP_STRING([--disable-Phonon], [disable using Phonon library]),[ ],[
	    PKG_CHECK_MODULES([phonon],[phonon > 4.3.0],
		[
		    AC_DEFINE(HAVE_PHONON,1,[Phonon media player engine allow.])
		    SAVE_CXXFLAGS="$CXXFLAGS"
		    CXXFLAGS="$CXXFLAGS $phonon_CFLAGS"
		    AC_CHECK_HEADERS([phonon/VideoPlayer],[],[])
		    CXXFLAGS="$SAVE_CXXFLAGS"
		],
		[AC_MSG_NOTICE(Phonon not found. Using is disabled!)])])
    ])
])
AX_MOD_UI_EN(QTStarter,[disable or enable[=incl] build module UI.QTStarter],disable,incl,[
    AX_LIB_Qt()
])
AX_MOD_UI_EN(QTCfg,[disable or enable[=incl] build module UI.QTCfg],disable,incl,[
    AX_LIB_Qt()
])
AX_MOD_UI_EN(WebCfg,[disable or enable[=incl] build module UI.WebCfg],disable,incl)
AX_MOD_UI_EN(WebCfgD,[disable or enable[=incl] build module UI.WebCfgD],disable,incl,[
    AX_LIB_GD()
])
AX_MOD_UI_EN(WebVision,[disable or enable[=incl] build module UI.WebVision],disable,incl,[
    AX_LIB_GD()
    AX_LIB_FFTW3()
])
AX_MOD_UI_EN(WebUser,[disable or enable[=incl] build module UI.WebUser],disable,incl)
#> Special modules
AC_SUBST(SpecSub_mod)
AX_MOD_Special_EN(SystemTests,[disable build module Special.SystemTests],disable)
AX_MOD_Special_EN(FLibComplex1,[disable or enable[=incl] build module Special.FLibComplex1],disable,incl)
AX_MOD_Special_EN(FLibMath,[disable or enable[=incl] build module Special.FLibMath],disable,incl)
AX_MOD_Special_EN(FLibSYS,[disable or enable[=incl] build module Special.FLibSYS],disable,incl,[
    AX_LIB_FFTW3()
])
AC_MSG_RESULT(===========================================)

AC_DEFINE_UNQUOTED(MODS_INCL, "${ModsIncl}",[Included to core modules list.])

# Clear libs list after PKG_CHECK_MODULES
LIBS=""

AC_OUTPUT

AC_MSG_RESULT([============ Final configuration results ==============])
AC_MSG_RESULT([* Status:])
AC_MSG_RESULT([  - Enable(yes)/including(incl)/for dist(dist)/disable(no) all modules = $enable_AllModuls])
AC_MSG_RESULT([  - Crosscompile build                                          = $enable_CrossCompile])
AC_MSG_RESULT([  - Core lib build only static                                  = $enable_CoreLibStatic])
AC_MSG_RESULT([* Generic features:])
AC_MSG_RESULT([  - Strings charset encode support (by iconv)                   = $ac_cv_header_iconv_h])
AC_MSG_RESULT([  - Interfaces internationalisation (I18N) support (by LibIntl) = $ac_cv_header_libintl_h])
AC_MSG_RESULT([  - Graphical library (LibGD2) use by core                      = $enable_LibGD])
AC_MSG_RESULT([  - Subsystem modules build:])
AC_MSG_RESULT([    "DB":                   $DBSub_mod])
AC_MSG_RESULT([    "DAQ":                  $DAQSub_mod])
AC_MSG_RESULT([    "Archive":              $ArchSub_mod])
AC_MSG_RESULT([    "Transport":            $TranspSub_mod])
AC_MSG_RESULT([    "Transport's protocol": $ProtSub_mod])
AC_MSG_RESULT([    "UI":                   $UISub_mod])
AC_MSG_RESULT([    "Special":              $SpecSub_mod])
AC_MSG_RESULT([  - Modules included to OpenSCADA core: $ModsIncl])
AC_MSG_RESULT([* Modules' features:])
AC_MSG_RESULT([  - FFTW3 for signal spectrum purchase: $LIB_FFTW3])
AC_MSG_RESULT([  - Linux sensors library use:          $LIB_SENSORS])
AC_MSG_RESULT([  - WebKit web-browser engine:          $QtWebKit_LIBS])
AC_MSG_RESULT([  - Media play engine:                  $phonon_LIBS])
