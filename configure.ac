AC_PREREQ(2.50)

# After any LTS release append there the third stabilisation level/number to the version
#  Don't change the version string but it uses in whole to replace by a real one into the automatic builder.
AC_INIT(OpenSCADA, [0.9], rom_as@oscada.org)

# Increase the major for next Work version.
# Increase the minor after any significant changes into API of the Work/LTS version
oscd_LIBREVISION="3:6:0"

AC_PREFIX_DEFAULT(/usr)
oscd_modulpref="oscd_"

AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE
AM_CONFIG_HEADER(src/config.h)

AC_LANG(C++)

# Perform the program name transformation
#AC_ARG_PROGRAM

# Enable options
AC_ENABLE_SHARED(yes)
AC_ENABLE_STATIC(yes)
AC_LIBTOOL_DLOPEN

# AM_GNU_GETTEXT

# Check for the needed programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL
AM_PROG_LIBTOOL

if test "$ac_cv_prog_cc_g" = "no" -o "$ac_cv_prog_cxx_g" = "no"; then
    AC_MSG_ERROR([No C/C++ compiler present. Install GCC and G++.]);
fi

PKG_PROG_PKG_CONFIG
if test "x$PKG_CONFIG" = "x"; then
    AC_MSG_ERROR([pkg-config is required to configure this program])
fi

AM_SANITY_CHECK

# Check for the header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([time.h signal.h fcntl.h math.h errno.h unistd.h dirent.h stdio.h stdarg.h stdlib.h dlfcn.h locale.h pthread.h sys/types.h sys/stat.h sys/time.h sys/utsname.h],[],
    AC_MSG_ERROR([Some GLibC headers are not found. Install or check the GLibC developing package!]))
AC_CHECK_HEADERS([string vector map exception typeinfo deque],[],
    AC_MSG_ERROR([Some STL headers are not found. Install or check the STL of StdC++ developing package!]))
AC_CHECK_HEADERS([langinfo.h syscall.h iconv.h libintl.h], [], AC_MSG_NOTICE([Some system headers aren't found. Using disabled!]))

#Check for the gettext utilites
if test $ac_cv_header_iconv_h = yes; then
    AC_SEARCH_LIBS(libiconv_open, [iconv])
fi

if test $ac_cv_header_libintl_h = yes; then
    AC_CHECK_PROG(XGETTEXT, xgettext, xgettext, false)
    AC_CHECK_PROG(MSGMERGE, msgmerge, msgmerge, false)
    AC_CHECK_PROG(MSGFMT, msgfmt, msgfmt, false)
    if test $XGETTEXT = false -o $MSGMERGE = false -o $MSGFMT = false; then
	AC_MSG_ERROR([Some utilities of I18N gettext are not present! Install the gettext utilites package.]);
    fi
    AC_SEARCH_LIBS(libintl_bindtextdomain, [intl])
fi

# Check for typedefs, structures and the compiler characteristics.
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

AM_CONDITIONAL([ResourcesAllow], [test -d data])

# Check for functions of libraries
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
AX_DEFINE_DIR(bindir_full, bindir, [Binaries location for configurations])
AX_DEFINE_DIR(sysconfdir_full, sysconfdir, [Configure location for configurations])
AX_DEFINE_DIR(libdir_full, libdir, [Libraries location for configurations])
AX_DEFINE_DIR(datadir_full, datadir, [Data location for configurations])
AX_DEFINE_DIR(localedir_full, localedir, [I18N locales location for configurations])
AX_DEFINE_DIR(oscd_datadir_full, oscd_datadir_full, [OpenSCADA data location for configurations])
AX_DEFINE_DIR(oscd_moddir_full, oscd_moddir_full, [OpenSCADA modules location for configurations])
AC_ARG_VAR(CoreLibNmToDlOpen, [Force core library name to it open by dlopen() for included/builtin modules])
if test -n "${CoreLibNmToDlOpen}"; then
    AC_DEFINE_UNQUOTED(CoreLibNmToDlOpen, "${CoreLibNmToDlOpen}", [Force core library name to it open by dlopen() for included/builtin modules])
fi

AC_CONFIG_FILES([Makefile openscada.pc src/Makefile src_call/Makefile])

if test -d data; then
    COND_CONFIG_FILES="data/openscada.desktop data/openscada_start data/openscada-proj data/ModelsDB/AGLKS/openscada_AGLKS.desktop data/ModelsDB/Boiler/openscada_Boiler.desktop"
    AC_CONFIG_FILES([${COND_CONFIG_FILES}])
fi

#====== Generic functions enable/disable ======
AC_ARG_ENABLE(CrossCompile, AC_HELP_STRING([--enable-CrossCompile],[enable the crosscompile build]), [],
    [if test $host = $build; then enable_CrossCompile=no; else enable_CrossCompile=yes; fi])
AC_ARG_ENABLE(CoreLibStatic, AC_HELP_STRING([--enable-CoreLibStatic],[enable OpenSCADA core library to build only in static, without the shared one.]),
    [liboscada_la_LIBTOOLFLAGS="--tag=disable-shared"; AC_SUBST(liboscada_la_LIBTOOLFLAGS)], [enable_CoreLibStatic=no;])
AC_ARG_ENABLE(CoreLibVersion, AC_HELP_STRING([--disable-CoreLibVersion],[disable version for OpenSCADA core library.]), [], [enable_CoreLibVersion=yes;])
if test $enable_CoreLibVersion = yes; then liboscada_la_LDFLAGS="-version-info ${oscd_LIBREVISION}"; else liboscada_la_LDFLAGS="-avoid-version"; fi
AC_SUBST(liboscada_la_LDFLAGS)
AC_ARG_ENABLE(LibGD, AC_HELP_STRING([--enable-LibGD], [enable/force of using the library LibGD into the core or the modules]), [], [enable_LibGD=no;])
AC_ARG_ENABLE(AllModuls,
    AC_HELP_STRING([--enable-AllModuls],[all modules to enable (yes), to include/builtin (incl), for a distributive (dist) or to disable (no) for next its separated selection]),
	[],[enable_AllModuls=individual;])

AC_MSG_RESULT([========== Check for libraries of the core =============])
AX_PTHREAD
if test $enable_CrossCompile = no; then
    AC_FUNC_MALLOC
    AC_FUNC_REALLOC
fi
AC_SEARCH_LIBS(sin, [m], [], AC_MSG_ERROR([The mathematic library isn't found. Install or check the GLibC core package!]))
AC_SEARCH_LIBS(dlopen, [dl], [], AC_MSG_ERROR([The dynamic linking library isn't found. Install or check the GLibC core package!]))
# AC_SEARCH_LIBS(clock_gettime, [rt])
AC_SEARCH_LIBS(clock_nanosleep, [rt])
AC_CHECK_HEADERS([crypt.h], AC_SEARCH_LIBS(crypt,[crypt]), AC_MSG_NOTICE([crypt.h header isn't found. Using disabled!]))
AC_SEARCH_LIBS(deflate, [z], [], AC_MSG_ERROR([Z library isn't found. Install or check the libz package!]))

# Check for the PCRE library using in the OpenSCADA core
CXXFLAGS="$CXXFLAGS $($PKG_CONFIG --cflags libpcre)"
AC_CHECK_HEADERS([pcre.h], [], AC_MSG_ERROR([The PCRE header isn't found. Install or check the PCRE developing package!]))
AC_SEARCH_LIBS(pcre_compile, [pcre], [], AC_MSG_ERROR([The PCRE library isn't found. Install or check the PCRE package!]))

# Check for the GD library using into the OpenSCADA core
if test $enable_LibGD = yes; then
    AX_LIB_GD
    AC_DEFINE(HAVE_GD_FORCE, 1, [LibGD: Force the library using into the core and modules])
    LIBS="${LIBS}${gdlib_LIBS} "
fi
LIB_CORE=${LIBS}

AC_SUBST(BldMods)
AC_SUBST(BldInclMods)
AC_SUBST(DBSub_mod)
AC_SUBST(DAQSub_mod)
AC_SUBST(ArchSub_mod)
AC_SUBST(TranspSub_mod)
AC_SUBST(ProtSub_mod)
AC_SUBST(UISub_mod)
AC_SUBST(SpecSub_mod)

AC_MSG_RESULT([========== Check for building modules =============])

# DB modules
AX_MOD_DB_EN(SQLite, [disable or enable[=incl] compilation module DB.SQLite], disable, incl, [
    PKG_CHECK_MODULES(sqlite3, [sqlite3 > 3],
	AC_CHECK_LIB(sqlite3, sqlite3_open, [], AC_MSG_ERROR([SQLite3 library isn't usable!])),
	AC_MSG_ERROR([DB.SQLite: SQLite3 developing resources aren't found. Install or check the SQLite3 development package!]))
])
AX_MOD_DB_EN(MySQL, [disable or enable[=incl] compilation module DB.MySQL], disable, incl, [
    AX_LIB_MYSQL
    if test "$found_mysql" != "yes"; then AC_MSG_ERROR([DB.MySQL: MySQL developing resources aren't found. Install or check the MySQL development package!]); fi
])
AX_MOD_DB_EN(PostgreSQL, [disable or enable[=incl] compilation module DB.PostgreSQL], disable, incl, [
    AX_LIB_POSTGRESQL(8.3.0)
    if test "$found_postgresql" != "yes"; then
	AC_MSG_ERROR([DB.PostgreSQL: PostgreSQL developing resources aren't found. Install or check the PostgreSQL development package!]);
    fi
])
AX_MOD_DB_EN(FireBird, [disable or enable[=incl] compilation module DB.FireBird], disable, incl, [
    AX_LIB_FIREBIRD(2.0.0)
    if test "$success" != "yes"; then
	AC_MSG_ERROR([DB.FireBird: FireBird developing resources aren't found. Install or check the FireBird development package!]);
    fi
])
AX_MOD_DB_EN(DBF, [disable or enable[=incl] compilation module DB.DBF], disable, incl)
AX_MOD_DB_EN(ODBC, [disable or enable[=incl] compilation module DB.ODBC], enable, incl, [
    AX_LIB_IODBC(1)
    if test "$found_iodbc" != "yes"; then
	AC_MSG_ERROR([DB.ODBC: iODBC developing resources aren't found. Install or check the iODBC development package!]);
    fi
])
AX_MOD_DB_EN(LDAP, [disable or enable[=incl] compilation module DB.LDAP], enable, incl, [
    AC_CHECK_HEADERS([ldap.h], [],
	AC_MSG_ERROR([DB.LDAP: OpenLDAP developing resources aren't found. Install or check the iODBC development package!]))
])

# DAQ modules
AX_MOD_DAQ_EN(JavaLikeCalc, [disable or enable[=incl] compilation module DAQ.BlockCalc], disable, incl, [
    AC_PROG_YACC
    AC_CHECK_PROG(YACC_OK, bison, true, false)
    if test $YACC_OK = false; then
	AC_MSG_ERROR([No syntax generator present! Install a YACC program like to bison.])
    fi
])
AX_MOD_DAQ_EN(LogicLev, [disable or enable[=incl] compilation module DAQ.LogicLev], disable, incl)
AX_MOD_DAQ_EN(BlockCalc, [disable or enable[=incl] compilation module DAQ.BlockCalc], disable, incl)
AX_MOD_DAQ_EN(DAQGate, [disable or enable[=incl] compilation module DAQ.DAQGate], disable, incl)
AX_MOD_DAQ_EN(System, [disable or enable[=incl] compilation module DAQ.System], disable, incl, [
    AX_LIB_SENSORS()
])
AX_MOD_DAQ_EN(ModBus, [disable or enable[=incl] compilation module DAQ.ModBus], disable, incl)
AX_MOD_DAQ_EN(DCON, [disable or enable[=incl] compilation module DAQ.DCON], disable, incl)
AX_MOD_DAQ_EN(OPC_UA, [disable compilation module DAQ.OPC_UA], disable, incl, [
    AX_LIB_OpenSSL()
], AC_CONFIG_FILES([src/moduls/daq/OPC_UA/libOPC_UA/Makefile]))
AX_MOD_DAQ_EN(MMS, [enable compilation module DAQ.MMS], enable, [], [], [
    AC_CONFIG_FILES([src/moduls/daq/MMS/libMMS/Makefile])
])
AX_MOD_DAQ_EN(SNMP, [disable compilation module DAQ.SNMP], disable, [], [
    AC_CHECK_HEADERS([net-snmp/net-snmp-config.h], [],
	AC_MSG_ERROR([DAQ.SNMP: The Net SNMP library headers aren't found. Install or check the Net SNMP development package!]))
#    AC_CHECK_LIB([netsnmp],[snmp_sess_init],[AC_MSG_NOTICE(Pass global library using)],
#	[AC_MSG_ERROR(DAQ.SNMP: Net SNMP API library not found. Install or check Net SNMP library package!)],
#	[crypto])
])
if test $ix86 != yes -a $target_cpu != "arm"; then enable_ICP_DAS=no; fi
AX_MOD_DAQ_EN(ICP_DAS, [enable compilation module DAQ.ICP_DAS], enable, [], [
    ICP_DAS_CFLAGS="-I./include"
    ICP_DAS_LIBS="-L./ -li8k"
    if test $target_cpu = "arm"; then ICP_DAS_LIBS="-L./ -li8k_arm"; ICP_DAS_CFLAGS="-I./include_arm"; fi
    AC_SUBST(ICP_DAS_CFLAGS)
    AC_SUBST(ICP_DAS_LIBS)
])
AX_MOD_DAQ_EN(Siemens, [disable compilation module DAQ.Siemens], disable, [], [
    AC_CHECK_HEADERS([ctype.h], [],
	AC_MSG_ERROR([DAQ.Siemens: Some GLibC network headers aren't found. Install or check the GLibC development package!]))
    AC_CHECK_HEADERS([linux/kd.h], [],
	AC_MSG_ERROR([DAQ.Siemens: The Linux kernel headers aren't found. Install or check the Linux kernel headers package!]))
])
if test $ix86 != yes; then enable_DiamondBoards=no; fi
AX_MOD_DAQ_EN(DiamondBoards, [enable compilation module DAQ.DiamondBoards], enable)
AX_MOD_DAQ_EN(Comedi, [enable compilation module DAQ.Comedi], enable, [], [
    PKG_CHECK_MODULES(comedilib, [comedilib > 0.7.0], [], AC_MSG_ERROR([Comedilib isn't found. Install or check the Comedilib library package!]))
])
AX_MOD_DAQ_EN(SoundCard, [disable compilation module DAQ.SoundCard], disable, [], [
    AC_CHECK_HEADERS([portaudio.h], [],
	AC_MSG_ERROR([DAQ.SoundCard: Some Portaudio headers aren't found. Install or check the Portaudio developing package!]))
    AC_CHECK_LIB(portaudio, Pa_Initialize, [], AC_MSG_ERROR([The Portaudio library isn't found. Install or check the Portaudio library package!]))
])
AX_MOD_DAQ_EN(BFN, [enable compilation module DAQ.BFN], enable)
AX_MOD_DAQ_EN(SMH2Gi, [enable compilation module DAQ.BFN], enable)
AX_MOD_DAQ_EN(AMRDevs, [disable compilation module DAQ.AMRDevs], disable)
if test $ix86 != yes -a $target_cpu != "ix86"; then enable_Fastwel=no; fi
AX_MOD_DAQ_EN(Fastwel, [enable compilation module DAQ.Fastwel], enable)
AX_MOD_DAQ_EN(FT3, [enable compilation module DAQ.FT3], enable)
AX_MOD_DAQ_EN(BCM2835, [enable compilation module DAQ.BCM2835], enable)

# Archive modules
AX_MOD_Archive_EN(FSArch, [disable or enable[=incl] compilation module Archive.FSArch], disable, incl)
AX_MOD_Archive_EN(DBArch, [disable or enable[=incl] compilation module Archive.DBArch], disable, incl)

# Transport modules
AX_MOD_Transport_EN(Sockets, [disable or enable[=incl] compilation module Transport.Sockets], disable, incl, [
    AC_CHECK_HEADERS([linux/can.h], [], AC_MSG_NOTICE([linux/can.h header isn't found. Using disabled!]))
    AC_CHECK_HEADERS([sys/un.h sys/socket.h sys/select.h arpa/inet.h netinet/in.h netdb.h], [],
	AC_MSG_ERROR([Transport.Sockets: Some GLibC network headers aren't found. Install or check the GLibC development package!]))
])
AX_MOD_Transport_EN(Serial, [disable or enable[=incl] compilation module Transport.Serial], disable, incl, [
    AC_CHECK_HEADERS([linux/serial.h linux/i2c-dev.h linux/spi/spidev.h], [], AC_MSG_NOTICE([Some Linux headers aren't found. Using disabled!]))
])
AX_MOD_Transport_EN(SSL, [disable or enable[=incl] compilation module Transport.SSL], disable, incl, [
    AX_LIB_OpenSSL()
])

# Transport protocol modules
AX_MOD_TrProt_EN(SelfSystem, [disable or enable[=incl] compilation module Protocol.SelfSystem], disable, incl)
AX_MOD_TrProt_EN(UserProtocol, [disable or enable[=incl] compilation module Protocol.UserProtocol], disable, incl)
AX_MOD_TrProt_EN(HTTP, [disable or enable[=incl] compilation module Protocol.HTTP], disable, incl)

# UI modules
AX_MOD_UI_EN(VCAEngine, [disable or enable[=incl] compilation module UI.VCAEngine], disable, incl)
AX_MOD_UI_EN(QTStarter, [disable or enable[=incl] compilation module UI.QTStarter], disable, incl, [
    AX_LIB_Qt()

    AS_IF([test "x${Qt5_use}" = "x"], [
	PKG_CHECK_MODULES(QtSensors, [QtSensors > 1.2.0],
	    AC_DEFINE(HAVE_QTSENSORS, 1, [QtSensors for mobility devices is allowed]),
	    AC_MSG_NOTICE([QtSensors has not found. Using disabled!]))
    ], [
	PKG_CHECK_MODULES(Qt5Sensors, [Qt5Sensors > 1.2.0], [
	    AC_DEFINE(HAVE_QTSENSORS, 1, [QtSensors for mobility devices is allowed])
	    QtSensors_CFLAGS=$Qt5Sensors_CFLAGS
	    QtSensors_LIBS=$Qt5Sensors_LIBS
	], AC_MSG_NOTICE([QtSensors has not found. Using disabled!]))
    ])
])
AX_MOD_UI_EN(QTCfg, [disable or enable[=incl] compilation module UI.QTCfg], disable, incl, [
    AX_LIB_Qt()
])
AX_MOD_UI_EN(Vision, [disable or enable[=incl] compilation module UI.Vision], disable, incl, [
    AX_LIB_FFTW3()
    AX_LIB_Qt()

    AC_ARG_ENABLE(WebKit, AC_HELP_STRING([--disable-WebKit], [disable the WebKit library using]), [], [
	AS_IF([test "x${Qt5_use}" = "x"], [
	    PKG_CHECK_MODULES(QtWebKit, [QtWebKit > 4.4.0],
		AC_DEFINE(HAVE_WEBKIT, 1, [The WebKit web browser engine is allowed]),
		AC_MSG_NOTICE([WebKit isn't found. Using disabled!]))
	], [
	    PKG_CHECK_MODULES(Qt5WebKitWidgets, [Qt5WebKitWidgets > 5.1.0], [
		AC_DEFINE(HAVE_WEBKIT, 1, [The WebKit web browser engine is allowed])
		QtWebKit_CFLAGS=$Qt5WebKitWidgets_CFLAGS
		QtWebKit_LIBS=$Qt5WebKitWidgets_LIBS
	    ], AC_MSG_NOTICE([WebKit isn't found. Using disabled!]))
	])
    ])

    AC_ARG_ENABLE(Phonon, AC_HELP_STRING([--disable-Phonon], [disable the Phonon library using]), [], [
	AS_IF([test "x${Qt5_use}" = "x"], [
	    PKG_CHECK_MODULES(phonon, [phonon > 4.3.0],
		AC_DEFINE(HAVE_PHONON, 1, [The Phonon media player engine is allowed.]),
		AC_MSG_NOTICE([The Phonon media player engine isn't found. Using disabled!]))
	], [
	    PKG_CHECK_MODULES(phonon4qt5, [phonon4qt5 > 4.3.0], [
		AC_DEFINE(HAVE_PHONON, 1, [The Phonon media player engine is allowed.])
		phonon_CFLAGS=$phonon4qt5_CFLAGS
		phonon_LIBS=$phonon4qt5_LIBS
	    ], AC_MSG_NOTICE([The Phonon media player engine isn't found. Using disabled!]))
	])
	AS_IF([test "x${phonon_CFLAGS}" = "x"], [], [
	    SAVE_CXXFLAGS="$CXXFLAGS"
	    CXXFLAGS="$CXXFLAGS -fpic $QtGui_CFLAGS $phonon_CFLAGS"
	    AC_CHECK_HEADERS([phonon/VideoPlayer],[],[])
	    CXXFLAGS="$SAVE_CXXFLAGS"
	])
    ])
])
AX_MOD_UI_EN(WebCfg, [disable or enable[=incl] compilation module UI.WebCfg], disable, incl)
AX_MOD_UI_EN(WebCfgD, [disable or enable[=incl] compilation module UI.WebCfgD], disable, incl)
AX_MOD_UI_EN(WebVision, [disable or enable[=incl] compilation module UI.WebVision], disable, incl, [
    AX_LIB_GD
    AX_LIB_FFTW3
])
AX_MOD_UI_EN(WebUser, [disable or enable[=incl] compilation module UI.WebUser], disable, incl)

# Special modules
AX_MOD_Special_EN(FLibSYS, [disable or enable[=incl] compilation module Special.FLibSYS], disable, incl, [
    AX_LIB_FFTW3
    AX_LIB_OpenSSL_opt
])
AX_MOD_Special_EN(SystemTests, [disable compilation module Special.SystemTests], disable)
AX_MOD_Special_EN(FLibMath, [disable or enable[=incl] compilation module Special.FLibMath], disable, incl)
AX_MOD_Special_EN(FLibComplex1, [disable or enable[=incl] compilation module Special.FLibComplex1], disable, incl)

AC_MSG_RESULT(===========================================)

AC_DEFINE_UNQUOTED(MODS_INCL, "${ModsIncl}", [Modules list of included/builtin to the core.])

# Clear libs list after PKG_CHECK_MODULES
LIBS=""

AC_OUTPUT

AC_MSG_RESULT([============ Final configuration results ==============])
AC_MSG_RESULT([* Status:])
AC_MSG_RESULT([  - To enable (yes), to include/builtin (incl), for a distributive (dist), to disable (no) all modules = $enable_AllModuls])
AC_MSG_RESULT([  - Cross compilation                                           = $enable_CrossCompile])
AC_MSG_RESULT([  - The core library compilation only in static                 = $enable_CoreLibStatic])
AC_MSG_RESULT([* Generic features:])
AC_MSG_RESULT([  - Strings charset encode support (by iconv)                   = $ac_cv_header_iconv_h])
AC_MSG_RESULT([  - Interfaces internationalisation (I18N) support (by LibIntl) = $ac_cv_header_libintl_h])
AC_MSG_RESULT([  - LibGD forcing for the core and the modules                  = $enable_LibGD])
AC_MSG_RESULT([  - Compilation modules of the sunsystems:])
AC_MSG_RESULT([    "DB":                   $DBSub_mod])
AC_MSG_RESULT([    "DAQ":                  $DAQSub_mod])
AC_MSG_RESULT([    "Archive":              $ArchSub_mod])
AC_MSG_RESULT([    "Transport":            $TranspSub_mod])
AC_MSG_RESULT([    "Transport's protocol": $ProtSub_mod])
AC_MSG_RESULT([    "UI":                   $UISub_mod])
AC_MSG_RESULT([    "Special":              $SpecSub_mod])
AC_MSG_RESULT([  - Including/builtin modules to the OpenSCADA core: $ModsIncl])
AC_MSG_RESULT([* Features of some specific modules:])
AC_MSG_RESULT([  - FFTW3 to obtain spectrum of signal: $LIB_FFTW3])
AC_MSG_RESULT([  - Library of the Linux sensors:       $LIB_SENSORS])
AC_MSG_RESULT([  - Web-browser's engine WebKit:        $QtWebKit_LIBS])
AC_MSG_RESULT([  - Media player's engine:              $phonon_LIBS])
