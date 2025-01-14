# ===========================================================================
#          http://www.nongnu.org/autoconf-archive/ax_lib_mysql.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_LIB_MYSQL([MINIMUM-VERSION])
#
# DESCRIPTION
#
#   This macro provides tests of availability of MySQL client library of
#   particular version or newer.
#
#   AX_LIB_MYSQL macro takes only one argument which is optional. If there
#   is no required version passed, then macro does not run version test.
#
#   The --with-mysql option takes one of three possible values:
#
#   no - do not check for MySQL client library
#
#   yes - do check for MySQL library in standard locations (mysql_config
#   should be in the PATH)
#
#   path - complete path to mysql_config utility, use this option if
#   mysql_config can't be found in the PATH
#
#   This macro calls:
#
#     PKG_CHECK_MODULES([mysqlclient], [mysqlclient > 1.0.0])
#     AC_SUBST(MYSQL_CFLAGS)
#     AC_SUBST(MYSQL_LDFLAGS)
#     AC_SUBST(MYSQL_VERSION)
#
#   And sets:
#
#     HAVE_MYSQL
#
# LICENSE
#
#   Copyright (c) 2025 Roman Savochenko <roman@oscada.org>
#   Copyright (c) 2008 Mateusz Loskot <mateusz@loskot.net>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved.

AC_DEFUN([AX_LIB_MYSQL],
[
    AC_ARG_WITH([mysql],
        AS_HELP_STRING([--with-mysql=@<:@ARG@:>@],
            [use MySQL client library @<:@default=yes@:>@, optionally specify path to mysql_config]
        ),
        [
        if test "$withval" = "no"; then
            want_mysql="no"
        elif test "$withval" = "yes"; then
            want_mysql="yes"
        else
            want_mysql="yes"
            MYSQL_CONFIG="$withval"
        fi
        ], [want_mysql="yes"]
    )

    MYSQL_CFLAGS=""
    MYSQL_LDFLAGS=""
    MYSQL_VERSION=""

    if test "$want_mysql" != "no" -a -z "$withval" -o "$withval" = yes; then
	mysql_version_req=ifelse([$1], [], [], [ > $1])

	PKG_CHECK_MODULES([mysqlclient], [mysqlclient$mysql_version_req],[
	    MYSQL_CFLAGS="$mysqlclient_CFLAGS"
	    MYSQL_LDFLAGS="$mysqlclient_LIBS"
	    MYSQL_VERSION="$($PKG_CONFIG --modversion mysqlclient)"

	    AC_DEFINE([HAVE_MYSQL], [1], [Define to 1 if MySQL libraries are available])
	    found_mysql="yes"
	], AC_MSG_NOTICE([[mysqlclient.pc is not found - trying old configuration file.]]))
    fi

    dnl
    dnl Check MySQL libraries
    dnl

    if test "$want_mysql" = "yes" -a -z "$MYSQL_VERSION"; then

        if test -z "$MYSQL_CONFIG" -o test; then
            AC_PATH_PROGS([MYSQL_CONFIG], [mysql_config mariadb_config], [no])
        fi

        if test "$MYSQL_CONFIG" != "no"; then
            AC_MSG_CHECKING([for MySQL libraries])

            MYSQL_CFLAGS="`$MYSQL_CONFIG --cflags`"
            MYSQL_LDFLAGS="`$MYSQL_CONFIG --libs`"

            MYSQL_VERSION=`$MYSQL_CONFIG --version`

            AC_DEFINE([HAVE_MYSQL], [1], [Define to 1 if MySQL libraries are available])

            found_mysql="yes"
            AC_MSG_RESULT([yes])
        else
            found_mysql="no"
            AC_MSG_RESULT([no])
        fi
    fi

    dnl
    dnl Check if required version of MySQL is available
    dnl
    mysql_version_req=ifelse([$1], [], [], [$1])

    if test "$found_mysql" = "yes" -a -n "$mysql_version_req" -a -z "$mysqlclient_CFLAGS"; then

        AC_MSG_CHECKING([if MySQL version is >= $mysql_version_req])

        dnl Decompose required version string of MySQL
        dnl and calculate its number representation
        mysql_version_req_major=`expr $mysql_version_req : '\([[0-9]]*\)'`
        mysql_version_req_minor=`expr $mysql_version_req : '[[0-9]]*\.\([[0-9]]*\)'`
        mysql_version_req_micro=`expr $mysql_version_req : '[[0-9]]*\.[[0-9]]*\.\([[0-9]]*\)'`
        if test "x$mysql_version_req_micro" = "x"; then
            mysql_version_req_micro="0"
        fi

        mysql_version_req_number=`expr $mysql_version_req_major \* 1000000 \
                                   \+ $mysql_version_req_minor \* 1000 \
                                   \+ $mysql_version_req_micro`

        dnl Decompose version string of installed MySQL
        dnl and calculate its number representation
        mysql_version_major=`expr $MYSQL_VERSION : '\([[0-9]]*\)'`
        mysql_version_minor=`expr $MYSQL_VERSION : '[[0-9]]*\.\([[0-9]]*\)'`
        mysql_version_micro=`expr $MYSQL_VERSION : '[[0-9]]*\.[[0-9]]*\.\([[0-9]]*\)'`
        if test "x$mysql_version_micro" = "x"; then
            mysql_version_micro="0"
        fi

        mysql_version_number=`expr $mysql_version_major \* 1000000 \
                                   \+ $mysql_version_minor \* 1000 \
                                   \+ $mysql_version_micro`

        mysql_version_check=`expr $mysql_version_number \>\= $mysql_version_req_number`
        if test "$mysql_version_check" = "1"; then
            AC_MSG_RESULT([yes])
        else
            AC_MSG_RESULT([no])
        fi
    fi

    AC_SUBST([MYSQL_VERSION])
    AC_SUBST([MYSQL_CFLAGS])
    AC_SUBST([MYSQL_LDFLAGS])
])
# ===========================================================================
#         http://www.nongnu.org/autoconf-archive/ax_lib_firebird.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_LIB_FIREBIRD([MINIMUM-VERSION])
#
# DESCRIPTION
#
#   Test for the Firebird client library of a particular version (or newer).
#   This macro takes only one optional argument, the required version of
#   Firebird library. If required version is not passed, then 1.5.0 is used
#   in test of existance of Firebird client library.
#
#   For more information about Firebird API versioning check: API Identifies
#   Client Version http://www.firebirdsql.org/rlsnotes20/rnfbtwo-apiods.html
#
#   If no intallation prefix to the installed Firebird library is given the
#   macro searches under /usr, /usr/local, and /opt.
#
#   This macro calls:
#
#     AC_SUBST(FIREBIRD_CFLAGS)
#     AC_SUBST(FIREBIRD_LDFLAGS)
#     AC_SUBST(FIREBIRD_VERSION)
#
#   And sets:
#
#     HAVE_FIREBIRD
#
# LICENSE
#
#   Copyright (c) 2008 Mateusz Loskot <mateusz@loskot.net>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved.

AC_DEFUN([AX_LIB_FIREBIRD],
[
    AC_ARG_WITH([firebird],
	AS_HELP_STRING(
	    [--with-firebird=@<:@ARG@:>@],
	    [use Firebird client library @<:@default=yes@:>@, optionally specify the prefix for firebird library]
	),
	[
	    if test "$withval" = "no"; then WANT_FIREBIRD="no";
	    elif test "$withval" = "yes"; then WANT_FIREBIRD="yes"; ac_firebird_path="";
	    else WANT_FIREBIRD="yes"; ac_firebird_path="$withval"; fi
	], [WANT_FIREBIRD="yes"]
    )

    FIREBIRD_CFLAGS=""
    FIREBIRD_LDFLAGS=""
    FIREBIRD_VERSION=""

    if test "x$WANT_FIREBIRD" = "xyes"; then

	ac_firebird_header="ibase.h"

	firebird_version_req=ifelse([$1], [], [3.0.0], [$1])
	firebird_version_req_shorten=`expr $firebird_version_req : '\([[0-9]]*\.[[0-9]]*\)'`
	firebird_version_req_major=`expr $firebird_version_req : '\([[0-9]]*\)'`
	firebird_version_req_minor=`expr $firebird_version_req : '[[0-9]]*\.\([[0-9]]*\)'`
	firebird_version_req_micro=`expr $firebird_version_req : '[[0-9]]*\.[[0-9]]*\.\([[0-9]]*\)'`
	if test "x$firebird_version_req_micro" = "x" ; then firebird_version_req_micro="0"; fi

	dnl FB_API_VER represents the version of Firebird as follows:
	dnl - Any version of Interbase, or Firebird 1.0.x:      10
	dnl - Firebird 1.5.x:                                   15
	dnl - Firebird 2.0.x:                                   20

	firebird_version_req_number=`expr $firebird_version_req_major \+ $firebird_version_req_minor`

	AC_MSG_CHECKING([for Firebird client library >= $firebird_version_req])

	if test "$ac_firebird_path" != ""; then
	    ac_firebird_ldflags="-L$ac_firebird_path/lib"
	    ac_firebird_cppflags="-I$ac_firebird_path/include -I$ac_firebird_path/include/firebird"
	    ac_firebird_header_path="$ac_firebird_path/include"
	    if test -r "$ac_firebird_path/include/firebird/$ac_firebird_header"; then ac_firebird_header_path="$ac_firebird_path/include/firebird"; fi
	else
	    for ac_firebird_path_tmp in /usr /usr/local /opt ; do
		if test -r "$ac_firebird_path_tmp/include/$ac_firebird_header" -o -r "$ac_firebird_path_tmp/include/firebird/$ac_firebird_header"; then
		    ac_firebird_path=$ac_firebird_path_tmp
		    ac_firebird_cppflags="-I$ac_firebird_path_tmp/include -I$ac_firebird_path_tmp/include/firebird"
		    if test $ac_firebird_path_tmp = /opt; then ac_firebird_ldflags="-L$ac_firebird_path_tmp/lib"; fi
		    ac_firebird_header_path=$ac_firebird_path/include
		    if test -r "$ac_firebird_path/include/firebird/$ac_firebird_header"; then ac_firebird_header_path="$ac_firebird_path/include/firebird"; fi
		    break;
		fi
	    done
	fi

	ac_firebird_header_path="$ac_firebird_header_path/$ac_firebird_header"

	if test ! -f "$ac_firebird_header_path"; then
	    AC_MSG_RESULT([no])
	    success="no"
	else

	    ac_firebird_ldflags="$ac_firebird_ldflags -lfbclient -lpthread"

	    saved_CPPFLAGS="$CPPFLAGS"
	    CPPFLAGS="$CPPFLAGS $ac_firebird_cppflags"

	    AC_LANG_PUSH(C++)
	    AC_COMPILE_IFELSE(
		[
		AC_LANG_PROGRAM([[@%:@include <ibase.h>]],
		    [[
#if (FB_API_VER >= $firebird_version_req_number)
// Everything is okay
#else
#  error Firebird version is too old
#endif
		    ]]
		)
		],
		[
		AC_MSG_RESULT([yes])
		success="yes"
		],
		[
		AC_MSG_RESULT([not found])
		succees="no"
		]
	    )
	    AC_LANG_POP([C++])

	    CPPFLAGS="$saved_CPPFLAGS"

	    if test "$success" = "yes"; then

		FIREBIRD_CFLAGS="$ac_firebird_cppflags"
		FIREBIRD_LDFLAGS="$ac_firebird_ldflags"

		dnl Retrieve Firebird release version

		ac_firebird_version=`cat $ac_firebird_header_path | \
				     grep '#define.*FB_API_VER.*' | \
				     sed -e 's/.* //'`

		if test -n "$ac_firebird_version"; then
		    ac_firebird_version_major=`expr $ac_firebird_version \/ 10`
		    ac_firebird_version_minor=`expr $ac_firebird_version \% 10`

		    FIREBIRD_VERSION="$ac_firebird_version_major.$ac_firebird_version_minor.x"
		else
		    AC_MSG_WARN([Could not find FB_API_VER macro in $ac_firebird_header to get Firebird version.])
		fi

		AC_SUBST(FIREBIRD_CFLAGS)
		AC_SUBST(FIREBIRD_LDFLAGS)
		AC_SUBST(FIREBIRD_VERSION)
		#AC_DEFINE(HAVE_FIREBIRD)
	    fi
	fi
    fi
])

# ===========================================================================
#     http://www.gnu.org/software/autoconf-archive/ax_lib_postgresql.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_LIB_POSTGRESQL([MINIMUM-VERSION])
#
# DESCRIPTION
#
#   This macro provides tests of availability of PostgreSQL 'libpq' library
#   of particular version or newer.
#
#   AX_LIB_POSTGRESQL macro takes only one argument which is optional. If
#   there is no required version passed, then macro does not run version
#   test.
#
#   The --with-postgresql option takes one of three possible values:
#
#   no - do not check for PostgreSQL client library
#
#   yes - do check for PostgreSQL library in standard locations (pg_config
#   should be in the PATH)
#
#   path - complete path to pg_config utility, use this option if pg_config
#   can't be found in the PATH
#
#   This macro calls:
#
#     AC_SUBST(POSTGRESQL_CFLAGS)
#     AC_SUBST(POSTGRESQL_LDFLAGS)
#     AC_SUBST(POSTGRESQL_VERSION)
#
#   And sets:
#
#     HAVE_POSTGRESQL
#
# LICENSE
#
#   Copyright (c) 2008 Mateusz Loskot <mateusz@loskot.net>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

AC_DEFUN([AX_LIB_POSTGRESQL],
[
    AC_ARG_WITH([postgresql],
        AS_HELP_STRING([--with-postgresql=@<:@ARG@:>@],
            [use PostgreSQL library @<:@default=yes@:>@, optionally specify path to pg_config]
        ),
        [
        if test "$withval" = "no"; then
            want_postgresql="no"
        elif test "$withval" = "yes"; then
            want_postgresql="yes"
        else
            want_postgresql="yes"
            PG_CONFIG="$withval"
        fi
        ],
        [want_postgresql="yes"]
    )

    POSTGRESQL_CFLAGS="$($PKG_CONFIG --cflags libpq)"
    POSTGRESQL_LDFLAGS="$($PKG_CONFIG --libs libpq)"
    POSTGRESQL_VERSION="$($PKG_CONFIG --modversion libpq)"

    dnl
    dnl Check PostgreSQL libraries (libpq)
    dnl
    AC_MSG_CHECKING([for PostgreSQL libraries])

    if test "x$POSTGRESQL_CFLAGS" != "x"; then
        found_postgresql="yes"
        AC_MSG_RESULT([yes])
    elif test "$want_postgresql" = "yes"; then

        if test -z "$PG_CONFIG" -o test; then
            AC_PATH_PROG([PG_CONFIG], [pg_config], [])
        fi

        if test ! -x "$PG_CONFIG"; then
#            AC_MSG_ERROR([$PG_CONFIG does not exist or it is not an exectuable file])
            PG_CONFIG="no"
            found_postgresql="no"
        fi

        if test "$PG_CONFIG" != "no"; then
            POSTGRESQL_CFLAGS="-I`$PG_CONFIG --includedir`"
            POSTGRESQL_LDFLAGS="-L`$PG_CONFIG --libdir` -lpq"
            POSTGRESQL_VERSION=`$PG_CONFIG --version | sed -e 's#PostgreSQL ##'`

            AC_DEFINE([HAVE_POSTGRESQL], [1],
                [Define to 1 if PostgreSQL libraries are available])

            found_postgresql="yes"
            AC_MSG_RESULT([yes])
        else
            found_postgresql="no"
            AC_MSG_RESULT([no])
        fi
    fi

    dnl
    dnl Check if required version of PostgreSQL is available
    dnl


    postgresql_version_req=ifelse([$1], [], [], [$1])

    if test "$found_postgresql" = "yes" -a -n "$postgresql_version_req"; then

        AC_MSG_CHECKING([if PostgreSQL version is >= $postgresql_version_req])

        dnl Decompose required version string of PostgreSQL
        dnl and calculate its number representation
        postgresql_version_req_major=`expr $postgresql_version_req : '\([[0-9]]*\)'`
        postgresql_version_req_minor=`expr $postgresql_version_req : '[[0-9]]*\.\([[0-9]]*\)'`
        postgresql_version_req_micro=`expr $postgresql_version_req : '[[0-9]]*\.[[0-9]]*\.\([[0-9]]*\)'`
        if test "x$postgresql_version_req_micro" = "x"; then
            postgresql_version_req_micro="0"
        fi

        postgresql_version_req_number=`expr $postgresql_version_req_major \* 1000000 \
                                   \+ $postgresql_version_req_minor \* 1000 \
                                   \+ $postgresql_version_req_micro`

        dnl Decompose version string of installed PostgreSQL
        dnl and calculate its number representation
        postgresql_version_major=`expr $POSTGRESQL_VERSION : '\([[0-9]]*\)'`
        postgresql_version_minor=`expr $POSTGRESQL_VERSION : '[[0-9]]*\.\([[0-9]]*\)'`
        postgresql_version_micro=`expr $POSTGRESQL_VERSION : '[[0-9]]*\.[[0-9]]*\.\([[0-9]]*\)'`
        if test "x$postgresql_version_micro" = "x"; then
            postgresql_version_micro="0"
        fi

        postgresql_version_number=`expr $postgresql_version_major \* 1000000 \
                                   \+ $postgresql_version_minor \* 1000 \
                                   \+ $postgresql_version_micro`

        postgresql_version_check=`expr $postgresql_version_number \>\= $postgresql_version_req_number`
        if test "$postgresql_version_check" = "1"; then
            AC_MSG_RESULT([yes])
        else
            AC_MSG_RESULT([no])
        fi
    fi

    AC_SUBST([POSTGRESQL_VERSION])
    AC_SUBST([POSTGRESQL_CFLAGS])
    AC_SUBST([POSTGRESQL_LDFLAGS])
])

# ===========================================================================
#     http://oscada.org
# ===========================================================================
#
# SYNOPSIS
#
#   AX_LIB_IODBC([MINIMUM-VERSION])
#
# DESCRIPTION
#
#   This macro provides tests of availability of iODBC 'libiodbc' library
#   of particular version or newer.
#
#   AX_LIB_IODBC macro takes only one argument which is optional. If
#   there is no required version passed, then macro does not run version
#   test.
#
#   The --with-iodbc option takes one of three possible values:
#     no - do not check for iODBC client library
#     yes - do check for iODBC library in standard locations (iodbc-config should be in the PATH)
#     path - complete path to iodbc-config utility, use this option if iodbc-config can't be found in the PATH
#
#   This macro calls:
#
#     AC_SUBST(IODBC_CFLAGS)
#     AC_SUBST(IODBC_LDFLAGS)
#     AC_SUBST(IODBC_VERSION)
#
#   And sets:
#
#     HAVE_IODBC
#
# LICENSE
#
#   Copyright (c) 2015 Roman Savochenko <roman@oscada.org>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

AC_DEFUN([AX_LIB_IODBC],
[
    AC_ARG_WITH([iodbc],
	AS_HELP_STRING([--with-iodbc=@<:@ARG@:>@],
	    [use iODBC library @<:@default=yes@:>@, optionally specify path to iodbc-config]
	), [
	    if test "$withval" = "no"; then want_iodbc="no";
	    elif test "$withval" = "yes"; then want_iodbc="yes";
	    else want_iodbc="yes"; IODBC_CONFIG="$withval"; fi
	], [want_iodbc="yes"]
    )

    IODBC_CFLAGS=""
    IODBC_LDFLAGS=""
    IODBC_VERSION=""

    dnl
    dnl Check iODBC libraries (libiodbc)
    dnl

    if test "$want_iodbc" = "yes"; then
	if test -z "$IODBC_CONFIG" -o test; then
	    AC_PATH_PROG([IODBC_CONFIG], [iodbc-config], [])
	fi

	if test ! -x "$IODBC_CONFIG"; then
	    IODBC_CONFIG="no"
	    found_iodbc="no"
	fi

	if test "$IODBC_CONFIG" != "no"; then
	    AC_MSG_CHECKING([for iODBC libraries])

	    IODBC_CFLAGS=`$IODBC_CONFIG --cflags`
	    IODBC_LDFLAGS=`$IODBC_CONFIG --libs`
	    IODBC_VERSION=`$IODBC_CONFIG --odbcversion`

	    AC_DEFINE([HAVE_IODBC], [1], [Define to 1 if iODBC libraries are available])

	    found_iodbc="yes"
	    AC_MSG_RESULT([yes])
	else
	    found_iodbc="no"
	    AC_MSG_RESULT([no])
	fi
    fi

    dnl
    dnl Check if required version of iODBC is available
    dnl
    iodbc_version_req=ifelse([$1], [], [], [$1])
    if test "$found_iodbc" = "yes" -a -n "$iodbc_version_req"; then
	AC_MSG_CHECKING([if iODBC version is >= $iodbc_version_req])
	if test "`expr $IODBC_VERSION \>\= $iodbc_version_req`"; then
	    AC_MSG_RESULT([yes])
	else
	    AC_MSG_RESULT([no])
	fi
    fi

    AC_SUBST([IODBC_VERSION])
    AC_SUBST([IODBC_CFLAGS])
    AC_SUBST([IODBC_LDFLAGS])
])

# ===========================================================================
#     http://oscada.org
# ===========================================================================
#
# SYNOPSIS
#
#   AX_LIB_SENSORS()
#
# DESCRIPTION
#
#   This macro provides tests of availability Sensors library from LmSensors.
#
#   This macro calls:
#
#     AC_CHECK_HEADERS(sensors/sensors.h)
#     AC_SUBST(LIB_SENSORS)
#
#   And sets:
#
#     SensorsUse=true
#
# LICENSE
#
#   Copyright (c) 2011 Roman Savochenko <roman@oscada.org>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.
AC_DEFUN([AX_LIB_SENSORS],
[
    AC_CHECK_HEADERS([sensors/sensors.h], [
	AC_CHECK_LIB(sensors, sensors_init, [
	    AC_MSG_NOTICE([LibSensors: Pass global library using])
	    LIB_SENSORS="-lsensors"
	], AC_MSG_NOTICE([Libsensors library isn't found. Using disabled!]))
    ], AC_MSG_NOTICE([DAQ.System: Libsensors headers aren't found. Using disabled!]))
    AC_SUBST(LIB_SENSORS)
    SensorsUse=true
])

# ===========================================================================
#     http://oscada.org
# ===========================================================================
#
# SYNOPSIS
#
#   AX_LIB_GD()
#
# DESCRIPTION
#
#   This macro provides tests of availability Graphical GD library.
#
#   This macro calls:
#
#     AC_CHECK_HEADERS(gd.h)
#     AC_CHECK_LIB(gd)
#     AC_SUBST(LIB_GD)
#
#   And sets:
#
#     GDuse=true
#
# LICENSE
#
#   Copyright (c) 2011 Roman Savochenko <roman@oscada.org>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.
AC_DEFUN([AX_LIB_GD], [
    if test "x${GDuse}" = "x"; then
	save_LIBS="$LIBS"
	PKG_CHECK_MODULES(gdlib, [gdlib > 2], [], [ gdlib_LIBS="-lgd"; ])
	AC_CHECK_HEADERS([gd.h], [
	    AC_SEARCH_LIBS(gdImageCreate,[gd], [], AC_MSG_ERROR([The GD library isn't found. Install or check the GDlib package!]))
	], AC_MSG_ERROR([The headers of the GD library aren't found. Install or check the GD developing package!]))
	LIBS="$save_LIBS"
	GDuse=true
    fi
])

# ===========================================================================
#     http://oscada.org
# ===========================================================================
#
# SYNOPSIS
#
#   AX_LIB_OpenSSL()
#   AX_LIB_OpenSSL_opt()
#
# DESCRIPTION
#
#   This macro provides tests of availability Open SSL library.
#
#   This macro calls:
#
#     AC_CHECK_HEADERS([openssl/ssl.h openssl/err.h openssl/bio.h])
#     AC_CHECK_LIB(ssl)
#     AC_SUBST(LIB_OpenSSL)
#
#   And sets:
#
#     OpenSSLuse=true
#
# LICENSE
#
#   Copyright (c) 2011 Roman Savochenko <roman@oscada.org>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.
AC_DEFUN([AX_LIB_OpenSSL], [
    if test "x${OpenSSLuse}" = "x"; then
	AC_CHECK_HEADERS([openssl/ssl.h openssl/err.h openssl/bio.h openssl/md5.h], [],
	    AC_MSG_ERROR([Some OpenSSL headers aren't found. Install or check the OpenSSL developing package!]))
	AC_CHECK_LIB(crypto, BIO_new, [],
	    AC_MSG_ERROR([OpenSSL libcrypto isn't found. Install or check the OpenSSL installation!]))
	AC_CHECK_LIB(ssl, SSL_new, [],
	    AC_MSG_ERROR([OpenSSL libssl isn't found. Install or check the OpenSSL installation!]))
	LIB_OpenSSL="-lssl -lcrypto"
	AC_SUBST(LIB_OpenSSL)
	OpenSSLuse=true
    fi
])
AC_DEFUN([AX_LIB_OpenSSL_opt], [
    if test "x${OpenSSLuse}" = "x"; then
	AC_CHECK_HEADERS([openssl/ssl.h openssl/err.h openssl/bio.h openssl/md5.h],
	    AC_CHECK_LIB(crypto, BIO_new, [
		LIB_OpenSSL="-lssl -lcrypto"
		AC_SUBST(LIB_OpenSSL)
		OpenSSLuse=true
	    ], AC_MSG_NOTICE([OpenSSL library isn't found. Install or check the OpenSSL installation!])),
	    AC_MSG_NOTICE([OpenSSL headers aren't found. Using disabled for the option!]))
    fi
])

# ===========================================================================
#     http://oscada.org
# ===========================================================================
#
# SYNOPSIS
#
#   AX_LIB_FFTW3()
#
# DESCRIPTION
#
#   This macro provides tests of availability FFTW3 library.
#
#   This macro calls:
#
#     AC_CHECK_HEADERS([fftw3.h])
#     AC_CHECK_LIB([fftw3])
#     AC_SUBST(LIB_FFTW3)
#
#   And sets:
#
#     FFTW3use=true
#
# LICENSE
#
#   Copyright (c) 2011 Roman Savochenko <roman@oscada.org>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.
AC_DEFUN([AX_LIB_FFTW3], [
    if test "x${FFTW3use}" = "x"; then
	AC_CHECK_HEADERS([fftw3.h], [
	    AC_CHECK_LIB(fftw3, fftw_execute, [
		AC_MSG_NOTICE([LibFFTW3: Pass global library using])
		LIB_FFTW3="-lfftw3"
	    ], AC_MSG_NOTICE([FFTW3 library isn't found. Using disabled!]))
	], AC_MSG_NOTICE([Some FFTW3 headers aren't found. Using disabled!]))
	AC_SUBST(LIB_FFTW3)
	FFTW3use=true
    fi
])

# ===========================================================================
#     http://oscada.org
# ===========================================================================
#
# SYNOPSIS
#
#   AX_LIB_Qt()
#
# DESCRIPTION
#
#   This macro provides tests of availability Qt4 or Qt5 library.
#
#   This macro calls:
#
#     PKG_CHECK_MODULES([QtGui],[QtGui > 4.3.0])
#     PKG_CHECK_MODULES([Qt5Widgets],[Qt5Widgets > 5.1.0]
#     PKG_CHECK_MODULES([Qt5PrintSupport],[Qt5PrintSupport > 5.1.0]
#     PKG_CHECK_MODULES([Qt6Widgets],[Qt6Widgets > 6.1.0]
#     PKG_CHECK_MODULES([Qt6PrintSupport],[Qt6PrintSupport > 6.1.0]
#     AC_SUBST(Qt_MOC)
#     AC_SUBST(Qt_RCC)
#
#   And sets:
#
#     Qt_use=true
#     Qt4_use=true
#     Qt5_use=true
#     Qt6_use=true
#
# LICENSE
#
#   Copyright (c) 2011-2023 Roman Savochenko <roman@oscada.org>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.
AC_DEFUN([AX_LIB_Qt],
[
    Qt_check=4
    AC_ARG_WITH([qt5],AS_HELP_STRING([--with-qt5=@<:@ARG@:>@],[Force check and use Qt5 @<:@default=no@:>@, else Qt4 will be checked firstly]), [Qt_check=5])
    AC_ARG_WITH([qt6],AS_HELP_STRING([--with-qt6=@<:@ARG@:>@],[Force check and use Qt6 @<:@default=no@:>@, else Qt5 will be checked firstly]), [Qt_check=6])

    if test "x$Qt_use" = "x"; then
	AC_SUBST(Qt_MOC)
	AC_SUBST(Qt_RCC)

	# Qt4 detection
	if test $Qt_check = 4; then
	    PKG_CHECK_MODULES([QtGui], [QtGui > 4.3.0], [
		Qt_MOC="$($PKG_CONFIG --variable=moc_location QtGui)"
		Qt_RCC="$($PKG_CONFIG --variable=rcc_location QtGui)"
		Qt_prefix="$($PKG_CONFIG --variable=prefix QtGui)"
		Qt_use=true
		Qt4_use=true
	    ], AC_MSG_NOTICE([[Qt4 is not found.]]))
	fi

	# Qt5 detection
	if test "x${Qt_use}" = "x" -a $Qt_check != 6; then
	    PKG_CHECK_MODULES([Qt5Widgets],[Qt5Widgets > 5.1.0], [
		PKG_CHECK_MODULES([Qt5PrintSupport],[Qt5PrintSupport > 5.1.0],[],AC_MSG_NOTICE([[Qt5PrintSupport is not found - printing disabled.]]))
		QtGui_CFLAGS="$Qt5Widgets_CFLAGS $Qt5PrintSupport_CFLAGS"
		QtGui_LIBS="$Qt5Widgets_LIBS $Qt5PrintSupport_LIBS"
		Qt_MOC="$($PKG_CONFIG --variable=host_bins Qt5Core)/moc"
		Qt_RCC="$($PKG_CONFIG --variable=host_bins Qt5Core)/rcc"
		Qt_prefix="$($PKG_CONFIG --variable=prefix Qt5Core)"
		Qt_use=true
		Qt5_use=true
	    ], AC_MSG_NOTICE([[Qt5 isn't found.]]))
	fi

	# Qt6 detection
	if test "x${Qt_use}" = "x"; then
	    PKG_CHECK_MODULES([Qt6Widgets],[Qt6Widgets > 6.1.0], [
		PKG_CHECK_MODULES([Qt6PrintSupport],[Qt6PrintSupport > 6.1.0],[],AC_MSG_NOTICE([[Qt6PrintSupport is not found - printing disabled.]]))
		QtGui_CFLAGS="$Qt6Widgets_CFLAGS $Qt6PrintSupport_CFLAGS"
		QtGui_LIBS="$Qt6Widgets_LIBS $Qt6PrintSupport_LIBS"
		Qt_MOC="$($PKG_CONFIG --variable=libexecdir Qt6Gui)/moc"
		Qt_RCC="$($PKG_CONFIG --variable=libexecdir Qt6Gui)/rcc"
		Qt_prefix="$($PKG_CONFIG --variable=prefix Qt6Gui)"
		Qt_use=true
		Qt6_use=true
	    ], AC_MSG_ERROR([[Qt6 isn't found. Install development packages of the Qt4, Qt5 or Qt6 library.]]))
	fi

	# Final MOC and RCC checking
	if test "x${Qt_use}" != "x"; then
	    if test "x${Qt_MOC}" = "x" -o ! -x "${Qt_MOC}"; then Qt_MOC="${Qt_prefix}/bin/moc"; fi
	    if test "x${Qt_RCC}" = "x" -o ! -x "${Qt_RCC}"; then Qt_RCC="${Qt_prefix}/bin/rcc"; fi
	fi
    fi
])

# ===========================================================================
#     http://oscada.org
# ===========================================================================
#
# SYNOPSIS
#
#   AX_MOD_EN([ModName],[Help],[En],[InclAllow],[ActionCheck],[ActionEnable])
#
# DESCRIPTION
#
#   This macro provides OpenSCADA modules enable/disable/include and tests wrapper.
#
#   Parameters:
#     ModName - module name
#     Help - enable/disable/include option help
#     En - "enable" or "disable" default option oposite for default module state.
#     InclAllow - "incl" or empty for set module including to OpenSCADA core allow.
#     ActionIfEnable - call action on enable/include module.
#     ActionElse - call action on disable module.
#
#   This macro calls:
#
#     AC_ARG_ENABLE()
#     AS_HELP_STRING()
#     AM_CONDITIONAL()
#     AS_IF()
#
#   And sets:
#
#     enable_{ModName}=yes|incl|no
#
# LICENSE
#
#   Copyright (c) 2011 Roman Savochenko <roman@oscada.org>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

AC_DEFUN([AX_MOD_EN],
[
    AC_ARG_ENABLE([$1],AS_HELP_STRING([--$3-$1],[$2]),[ ],
    [
	if test "x$3" = "xdisable"; then
	    if test $enable_AllModuls = no || test "x$4" = "xincl" -a $enable_AllModuls = incl; then enable_$1=$enable_AllModuls;
	    else enable_$1=yes; fi
	else
	    if test $enable_AllModuls = yes || test "x$4" = "xincl" -a $enable_AllModuls = incl; then enable_$1=$enable_AllModuls;
	    else enable_$1=no; fi
	fi
    ])
    AM_CONDITIONAL([$1Incl],[test "x$4" = "xincl" -a $enable_$1 = incl])
    AS_IF([test $enable_$1 = yes || test "x$4" = "xincl" -a $enable_$1 = incl], [$5 $6])
])

AC_DEFUN([AX_MOD_DB_EN],
[
    AC_ARG_ENABLE([$1],AS_HELP_STRING([--$3-$1],[$2]),[ ],[
	    if test "x$3" = "xdisable"; then
		if test $enable_AllModuls = no || test "x$4" = "xincl" -a $enable_AllModuls = incl; then enable_$1=$enable_AllModuls;
		else enable_$1=yes; fi
	    else
		if test $enable_AllModuls = yes || test "x$4" = "xincl" -a $enable_AllModuls = incl; then enable_$1=$enable_AllModuls;
		else enable_$1=no; fi
	    fi
	])
    if test $enable_AllModuls = dist; then enable_$1=$enable_AllModuls; fi
    AM_CONDITIONAL([$1Incl],[test "x$4" = "xincl" -a $enable_$1 = incl])
    AS_IF([test $enable_$1 = yes || test $enable_$1 = dist || test "x$4" = "xincl" -a $enable_$1 = incl],[
	    AC_MSG_RESULT(Building module: DB.$1)
	    AC_CONFIG_FILES(src/moduls/bd/$1/Makefile)
	    DBSub_mod="${DBSub_mod}$1 "
	    if test $enable_$1 = incl; then
		LIB_CORE="${LIB_CORE}moduls/bd/$1/bd_$1.la "
		ModsIncl="${ModsIncl}bd_$1 "
		BldInclMods="${BldInclMods}moduls/bd/$1 "
	    else BldMods="${BldMods}moduls/bd/$1 "; fi
	    AS_IF([test $enable_$1 != dist],[$5])
	    $6
	])
])

AC_DEFUN([AX_MOD_DAQ_EN],
[
    AC_ARG_ENABLE([$1],AS_HELP_STRING([--$3-$1],[$2]),[ ],[
	    if test "x$3" = "xdisable"; then
		if test $enable_AllModuls = no || test "x$4" = "xincl" -a $enable_AllModuls = incl; then enable_$1=$enable_AllModuls;
		else enable_$1=yes; fi
	    else
		if test $enable_AllModuls = yes || test "x$4" = "xincl" -a $enable_AllModuls = incl; then enable_$1=$enable_AllModuls;
		else enable_$1=no; fi
	    fi
	])
    if test $enable_AllModuls = dist; then enable_$1=$enable_AllModuls; fi
    AM_CONDITIONAL([$1Incl],[test "x$4" = "xincl" -a $enable_$1 = incl])
    AS_IF([test $enable_$1 = yes || test $enable_$1 = dist || test "x$4" = "xincl" -a $enable_$1 = incl],[
	    AC_MSG_RESULT(Building module: DAQ.$1)
	    AC_CONFIG_FILES(src/moduls/daq/$1/Makefile)
	    DAQSub_mod="${DAQSub_mod}$1 "
	    if test $enable_$1 = incl; then
		LIB_CORE="${LIB_CORE}moduls/daq/$1/daq_$1.la "
		ModsIncl="${ModsIncl}daq_$1 "
		BldInclMods="${BldInclMods}moduls/daq/$1 "
	    else BldMods="${BldMods}moduls/daq/$1 "; fi
	    AS_IF([test $enable_$1 != dist],[$5])
	    $6
	])
])

AC_DEFUN([AX_MOD_Archive_EN],
[
    AC_ARG_ENABLE([$1],AS_HELP_STRING([--$3-$1],[$2]),[ ],[
	    if test "x$3" = "xdisable"; then
		if test $enable_AllModuls = no || test "x$4" = "xincl" -a $enable_AllModuls = incl; then enable_$1=$enable_AllModuls;
		else enable_$1=yes; fi
	    else
	        if test $enable_AllModuls = yes || test "x$4" = "xincl" -a $enable_AllModuls = incl; then enable_$1=$enable_AllModuls;
		else enable_$1=no; fi
	    fi
	])
    if test $enable_AllModuls = dist; then enable_$1=$enable_AllModuls; fi
    AM_CONDITIONAL([$1Incl],[test "x$4" = "xincl" -a $enable_$1 = incl])
    AS_IF([test $enable_$1 = yes || test $enable_$1 = dist || test "x$4" = "xincl" -a $enable_$1 = incl],[
	    AC_MSG_RESULT(Building module: Archive.$1)
	    AC_CONFIG_FILES(src/moduls/arhiv/$1/Makefile)
	    ArchSub_mod="${ArchSub_mod}$1 "
	    if test $enable_$1 = incl; then
		LIB_CORE="${LIB_CORE}moduls/arhiv/$1/arh_$1.la "
		ModsIncl="${ModsIncl}arh_$1 "
		BldInclMods="${BldInclMods}moduls/arhiv/$1 "
	    else BldMods="${BldMods}moduls/arhiv/$1 "; fi
	    AS_IF([test $enable_$1 != dist],[$5])
	    $6
	])
])

AC_DEFUN([AX_MOD_Transport_EN],
[
    AC_ARG_ENABLE([$1],AS_HELP_STRING([--$3-$1],[$2]),[ ],[
	    if test "x$3" = "xdisable"; then
		if test $enable_AllModuls = no || test "x$4" = "xincl" -a $enable_AllModuls = incl; then enable_$1=$enable_AllModuls;
		else enable_$1=yes; fi
	    else
		if test $enable_AllModuls = yes || test "x$4" = "xincl" -a $enable_AllModuls = incl; then enable_$1=$enable_AllModuls;
		else enable_$1=no; fi
	    fi
	])
    if test $enable_AllModuls = dist; then enable_$1=$enable_AllModuls; fi
    AM_CONDITIONAL([$1Incl],[test "x$4" = "xincl" -a $enable_$1 = incl])
    AS_IF([test $enable_$1 = yes || test $enable_$1 = dist || test "x$4" = "xincl" -a $enable_$1 = incl],[
	    AC_MSG_RESULT(Building module: Transport.$1)
	    AC_CONFIG_FILES(src/moduls/transport/$1/Makefile)
	    TranspSub_mod="${TranspSub_mod}$1 "
	    if test $enable_$1 = incl; then
		LIB_CORE="${LIB_CORE}moduls/transport/$1/tr_$1.la "
		ModsIncl="${ModsIncl}tr_$1 "
		BldInclMods="${BldInclMods}moduls/transport/$1 "
	    else BldMods="${BldMods}moduls/transport/$1 "; fi
	    AS_IF([test $enable_$1 != dist],[$5])
	    $6
	])
])

AC_DEFUN([AX_MOD_TrProt_EN],
[
    AC_ARG_ENABLE([$1],AS_HELP_STRING([--$3-$1],[$2]),[ ],[
	    if test "x$3" = "xdisable"; then
		if test $enable_AllModuls = no || test "x$4" = "xincl" -a $enable_AllModuls = incl; then enable_$1=$enable_AllModuls;
		else enable_$1=yes; fi
	    else
		if test $enable_AllModuls = yes || test "x$4" = "xincl" -a $enable_AllModuls = incl; then enable_$1=$enable_AllModuls;
		else enable_$1=no; fi
	    fi
	])
    if test $enable_AllModuls = dist; then enable_$1=$enable_AllModuls; fi
    AM_CONDITIONAL([$1Incl],[test "x$4" = "xincl" -a $enable_$1 = incl])
    AS_IF([test $enable_$1 = yes || test $enable_$1 = dist || test "x$4" = "xincl" -a $enable_$1 = incl],[
	    AC_MSG_RESULT(Building module: Protocol.$1)
	    AC_CONFIG_FILES(src/moduls/protocol/$1/Makefile)
	    ProtSub_mod="${ProtSub_mod}$1 "
	    if test $enable_$1 = incl; then
		LIB_CORE="${LIB_CORE}moduls/protocol/$1/prot_$1.la "
		ModsIncl="${ModsIncl}prot_$1 "
		BldInclMods="${BldInclMods}moduls/protocol/$1 "
	    else BldMods="${BldMods}moduls/protocol/$1 "; fi
	    AS_IF([test $enable_$1 != dist],[$5])
	    $6
	])
])

AC_DEFUN([AX_MOD_UI_EN],
[
    AC_ARG_ENABLE([$1],AS_HELP_STRING([--$3-$1],[$2]),[ ],[
	    if test "x$3" = "xdisable"; then
		if test $enable_AllModuls = no || test "x$4" = "xincl" -a $enable_AllModuls = incl; then enable_$1=$enable_AllModuls;
		else enable_$1=yes; fi
	    else
		if test $enable_AllModuls = yes || test "x$4" = "xincl" -a $enable_AllModuls = incl; then enable_$1=$enable_AllModuls;
		else enable_$1=no; fi
	    fi
	])
    if test $enable_AllModuls = dist; then enable_$1=$enable_AllModuls; fi
    AM_CONDITIONAL([$1Incl],[test "x$4" = "xincl" -a $enable_$1 = incl])
    AS_IF([test $enable_$1 = yes || test $enable_$1 = dist || test "x$4" = "xincl" -a $enable_$1 = incl],[
	    AC_MSG_RESULT(Building module: UI.$1)
	    AC_CONFIG_FILES(src/moduls/ui/$1/Makefile)
	    UISub_mod="${UISub_mod}$1 "
	    if test $enable_$1 = incl; then
		LIB_CORE="${LIB_CORE}moduls/ui/$1/ui_$1.la "
		ModsIncl="${ModsIncl}ui_$1 "
		BldInclMods="${BldInclMods}moduls/ui/$1 "
	    else BldMods="${BldMods}moduls/ui/$1 "; fi
	    AS_IF([test $enable_$1 != dist],[$5])
	    $6
	])
])

AC_DEFUN([AX_MOD_Special_EN],
[
    AC_ARG_ENABLE([$1],AS_HELP_STRING([--$3-$1],[$2]),[ ],[
	    if test "x$3" = "xdisable"; then
		if test $enable_AllModuls = no || test "x$4" = "xincl" -a $enable_AllModuls = incl; then enable_$1=$enable_AllModuls;
		else enable_$1=yes; fi
	    else
		if test $enable_AllModuls = yes || test "x$4" = "xincl" -a $enable_AllModuls = incl; then enable_$1=$enable_AllModuls;
		else enable_$1=no; fi
	    fi
	])
    if test $enable_AllModuls = dist; then enable_$1=$enable_AllModuls; fi
    AM_CONDITIONAL([$1Incl],[test "x$4" = "xincl" -a $enable_$1 = incl])
    AS_IF([test $enable_$1 = yes || test $enable_$1 = dist || test "x$4" = "xincl" -a $enable_$1 = incl],[
	    AC_MSG_RESULT(Building module: Special.$1)
	    AC_CONFIG_FILES(src/moduls/special/$1/Makefile)
	    SpecSub_mod="${SpecSub_mod}$1 "
	    if test $enable_$1 = incl; then
		LIB_CORE="${LIB_CORE}moduls/special/$1/spec_$1.la "
		ModsIncl="${ModsIncl}spec_$1 "
		BldInclMods="${BldInclMods}moduls/special/$1 "
	    else BldMods="${BldMods}moduls/special/$1 "; fi
	    AS_IF([test $enable_$1 != dist],[$5])
	    $6
	])
])

# ===========================================================================
#       http://www.gnu.org/software/autoconf-archive/ax_define_dir.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_DEFINE_DIR(VARNAME, DIR [, DESCRIPTION])
#
# DESCRIPTION
#
#   This macro sets VARNAME to the expansion of the DIR variable, taking
#   care of fixing up ${prefix} and such.
#
#   VARNAME is then offered as both an output variable and a C preprocessor
#   symbol.
#
#   Example:
#
#     AX_DEFINE_DIR([DATADIR], [datadir], [Where data are placed to.])
#
# LICENSE
#
#   Copyright (c) 2008 Stepan Kasal <kasal@ucw.cz>
#   Copyright (c) 2008 Andreas Schwab <schwab@suse.de>
#   Copyright (c) 2008 Guido U. Draheim <guidod@gmx.de>
#   Copyright (c) 2008 Alexandre Oliva
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.
AC_DEFUN([AX_DEFINE_DIR], [
  prefix_NONE=
  exec_prefix_NONE=
  test "x$prefix" = xNONE && prefix_NONE=yes && prefix=$ac_default_prefix
  test "x$exec_prefix" = xNONE && exec_prefix_NONE=yes && exec_prefix=$prefix
dnl In Autoconf 2.60, ${datadir} refers to ${datarootdir}, which in turn
dnl refers to ${prefix}.  Thus we have to use `eval' twice.
  eval ax_define_dir="\"[$]$2\""
  eval ax_define_dir="\"$ax_define_dir\""
  AC_SUBST($1, "$ax_define_dir")
  AC_DEFINE_UNQUOTED($1, "$ax_define_dir", [$3])
  test "$prefix_NONE" && prefix=NONE
  test "$exec_prefix_NONE" && exec_prefix=NONE
])

# ===========================================================================
#        https://www.gnu.org/software/autoconf-archive/ax_pthread.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_PTHREAD([ACTION-IF-FOUND[, ACTION-IF-NOT-FOUND]])
#
# DESCRIPTION
#
#   This macro figures out how to build C programs using POSIX threads. It
#   sets the PTHREAD_LIBS output variable to the threads library and linker
#   flags, and the PTHREAD_CFLAGS output variable to any special C compiler
#   flags that are needed. (The user can also force certain compiler
#   flags/libs to be tested by setting these environment variables.)
#
#   Also sets PTHREAD_CC to any special C compiler that is needed for
#   multi-threaded programs (defaults to the value of CC otherwise). (This
#   is necessary on AIX to use the special cc_r compiler alias.)
#
#   NOTE: You are assumed to not only compile your program with these flags,
#   but also to link with them as well. For example, you might link with
#   $PTHREAD_CC $CFLAGS $PTHREAD_CFLAGS $LDFLAGS ... $PTHREAD_LIBS $LIBS
#
#   If you are only building threaded programs, you may wish to use these
#   variables in your default LIBS, CFLAGS, and CC:
#
#     LIBS="$PTHREAD_LIBS $LIBS"
#     CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
#     CC="$PTHREAD_CC"
#
#   In addition, if the PTHREAD_CREATE_JOINABLE thread-attribute constant
#   has a nonstandard name, this macro defines PTHREAD_CREATE_JOINABLE to
#   that name (e.g. PTHREAD_CREATE_UNDETACHED on AIX).
#
#   Also HAVE_PTHREAD_PRIO_INHERIT is defined if pthread is found and the
#   PTHREAD_PRIO_INHERIT symbol is defined when compiling with
#   PTHREAD_CFLAGS.
#
#   ACTION-IF-FOUND is a list of shell commands to run if a threads library
#   is found, and ACTION-IF-NOT-FOUND is a list of commands to run it if it
#   is not found. If ACTION-IF-FOUND is not specified, the default action
#   will define HAVE_PTHREAD.
#
#   Please let the authors know if this macro fails on any platform, or if
#   you have any other suggestions or comments. This macro was based on work
#   by SGJ on autoconf scripts for FFTW (http://www.fftw.org/) (with help
#   from M. Frigo), as well as ac_pthread and hb_pthread macros posted by
#   Alejandro Forero Cuervo to the autoconf macro repository. We are also
#   grateful for the helpful feedback of numerous users.
#
#   Updated for Autoconf 2.68 by Daniel Richard G.
#
# LICENSE
#
#   Copyright (c) 2008 Steven G. Johnson <stevenj@alum.mit.edu>
#   Copyright (c) 2011 Daniel Richard G. <skunk@iSKUNK.ORG>
#
#   This program is free software: you can redistribute it and/or modify it
#   under the terms of the GNU General Public License as published by the
#   Free Software Foundation, either version 3 of the License, or (at your
#   option) any later version.
#
#   This program is distributed in the hope that it will be useful, but
#   WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
#   Public License for more details.
#
#   You should have received a copy of the GNU General Public License along
#   with this program. If not, see <https://www.gnu.org/licenses/>.
#
#   As a special exception, the respective Autoconf Macro's copyright owner
#   gives unlimited permission to copy, distribute and modify the configure
#   scripts that are the output of Autoconf when processing the Macro. You
#   need not follow the terms of the GNU General Public License when using
#   or distributing such scripts, even though portions of the text of the
#   Macro appear in them. The GNU General Public License (GPL) does govern
#   all other use of the material that constitutes the Autoconf Macro.
#
#   This special exception to the GPL applies to versions of the Autoconf
#   Macro released by the Autoconf Archive. When you make and distribute a
#   modified version of the Autoconf Macro, you may extend this special
#   exception to the GPL to apply to your modified version as well.

AU_ALIAS([ACX_PTHREAD], [AX_PTHREAD])
AC_DEFUN([AX_PTHREAD], [
AC_REQUIRE([AC_CANONICAL_HOST])
AC_REQUIRE([AC_PROG_CC])
AC_REQUIRE([AC_PROG_SED])
AC_LANG_PUSH([C])
ax_pthread_ok=no

# We used to check for pthread.h first, but this fails if pthread.h
# requires special compiler flags (e.g. on Tru64 or Sequent).
# It gets checked for in the link test anyway.

# First of all, check if the user has set any of the PTHREAD_LIBS,
# etcetera environment variables, and if threads linking works using
# them:
if test "x$PTHREAD_CFLAGS$PTHREAD_LIBS" != "x"; then
        ax_pthread_save_CC="$CC"
        ax_pthread_save_CFLAGS="$CFLAGS"
        ax_pthread_save_LIBS="$LIBS"
        AS_IF([test "x$PTHREAD_CC" != "x"], [CC="$PTHREAD_CC"])
        CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
        LIBS="$PTHREAD_LIBS $LIBS"
        AC_MSG_CHECKING([for pthread_join using $CC $PTHREAD_CFLAGS $PTHREAD_LIBS])
        AC_LINK_IFELSE([AC_LANG_CALL([], [pthread_join])], [ax_pthread_ok=yes])
        AC_MSG_RESULT([$ax_pthread_ok])
        if test "x$ax_pthread_ok" = "xno"; then
                PTHREAD_LIBS=""
                PTHREAD_CFLAGS=""
        fi
        CC="$ax_pthread_save_CC"
        CFLAGS="$ax_pthread_save_CFLAGS"
        LIBS="$ax_pthread_save_LIBS"
fi

# We must check for the threads library under a number of different
# names; the ordering is very important because some systems
# (e.g. DEC) have both -lpthread and -lpthreads, where one of the
# libraries is broken (non-POSIX).

# Create a list of thread flags to try.  Items starting with a "-" are
# C compiler flags, and other items are library names, except for "none"
# which indicates that we try without any flags at all, and "pthread-config"
# which is a program returning the flags for the Pth emulation library.

ax_pthread_flags="pthreads none -Kthread -pthread -pthreads -mthreads pthread --thread-safe -mt pthread-config"

# The ordering *is* (sometimes) important.  Some notes on the
# individual items follow:

# pthreads: AIX (must check this before -lpthread)
# none: in case threads are in libc; should be tried before -Kthread and
#       other compiler flags to prevent continual compiler warnings
# -Kthread: Sequent (threads in libc, but -Kthread needed for pthread.h)
# -pthread: Linux/gcc (kernel threads), BSD/gcc (userland threads), Tru64
#           (Note: HP C rejects this with "bad form for `-t' option")
# -pthreads: Solaris/gcc (Note: HP C also rejects)
# -mt: Sun Workshop C (may only link SunOS threads [-lthread], but it
#      doesn't hurt to check since this sometimes defines pthreads and
#      -D_REENTRANT too), HP C (must be checked before -lpthread, which
#      is present but should not be used directly; and before -mthreads,
#      because the compiler interprets this as "-mt" + "-hreads")
# -mthreads: Mingw32/gcc, Lynx/gcc
# pthread: Linux, etcetera
# --thread-safe: KAI C++
# pthread-config: use pthread-config program (for GNU Pth library)

case $host_os in

        freebsd*)

        # -kthread: FreeBSD kernel threads (preferred to -pthread since SMP-able)
        # lthread: LinuxThreads port on FreeBSD (also preferred to -pthread)

        ax_pthread_flags="-kthread lthread $ax_pthread_flags"
        ;;

        hpux*)

        # From the cc(1) man page: "[-mt] Sets various -D flags to enable
        # multi-threading and also sets -lpthread."

        ax_pthread_flags="-mt -pthread pthread $ax_pthread_flags"
        ;;

        openedition*)

        # IBM z/OS requires a feature-test macro to be defined in order to
        # enable POSIX threads at all, so give the user a hint if this is
        # not set. (We don't define these ourselves, as they can affect
        # other portions of the system API in unpredictable ways.)

        AC_EGREP_CPP([AX_PTHREAD_ZOS_MISSING],
            [
#            if !defined(_OPEN_THREADS) && !defined(_UNIX03_THREADS)
             AX_PTHREAD_ZOS_MISSING
#            endif
            ],
            [AC_MSG_WARN([IBM z/OS requires -D_OPEN_THREADS or -D_UNIX03_THREADS to enable pthreads support.])])
        ;;

        solaris*)

        # On Solaris (at least, for some versions), libc contains stubbed
        # (non-functional) versions of the pthreads routines, so link-based
        # tests will erroneously succeed. (N.B.: The stubs are missing
        # pthread_cleanup_push, or rather a function called by this macro,
        # so we could check for that, but who knows whether they'll stub
        # that too in a future libc.)  So we'll check first for the
        # standard Solaris way of linking pthreads (-mt -lpthread).

        ax_pthread_flags="-mt,pthread pthread $ax_pthread_flags"
        ;;
esac

# GCC generally uses -pthread, or -pthreads on some platforms (e.g. SPARC)

AS_IF([test "x$GCC" = "xyes"],
      [ax_pthread_flags="-pthread -pthreads $ax_pthread_flags"])

# The presence of a feature test macro requesting re-entrant function
# definitions is, on some systems, a strong hint that pthreads support is
# correctly enabled

case $host_os in
        darwin* | hpux* | linux* | osf* | solaris*)
        ax_pthread_check_macro="_REENTRANT"
        ;;

        aix*)
        ax_pthread_check_macro="_THREAD_SAFE"
        ;;

        *)
        ax_pthread_check_macro="--"
        ;;
esac
AS_IF([test "x$ax_pthread_check_macro" = "x--"],
      [ax_pthread_check_cond=0],
      [ax_pthread_check_cond="!defined($ax_pthread_check_macro)"])

# Are we compiling with Clang?

AC_CACHE_CHECK([whether $CC is Clang],
    [ax_cv_PTHREAD_CLANG],
    [ax_cv_PTHREAD_CLANG=no
     # Note that Autoconf sets GCC=yes for Clang as well as GCC
     if test "x$GCC" = "xyes"; then
        AC_EGREP_CPP([AX_PTHREAD_CC_IS_CLANG],
            [/* Note: Clang 2.7 lacks __clang_[a-z]+__ */
#            if defined(__clang__) && defined(__llvm__)
             AX_PTHREAD_CC_IS_CLANG
#            endif
            ],
            [ax_cv_PTHREAD_CLANG=yes])
     fi
    ])
ax_pthread_clang="$ax_cv_PTHREAD_CLANG"

ax_pthread_clang_warning=no

# Clang needs special handling, because older versions handle the -pthread
# option in a rather... idiosyncratic way

if test "x$ax_pthread_clang" = "xyes"; then

        # Clang takes -pthread; it has never supported any other flag

        # (Note 1: This will need to be revisited if a system that Clang
        # supports has POSIX threads in a separate library.  This tends not
        # to be the way of modern systems, but it's conceivable.)

        # (Note 2: On some systems, notably Darwin, -pthread is not needed
        # to get POSIX threads support; the API is always present and
        # active.  We could reasonably leave PTHREAD_CFLAGS empty.  But
        # -pthread does define _REENTRANT, and while the Darwin headers
        # ignore this macro, third-party headers might not.)

        PTHREAD_CFLAGS="-pthread"
        PTHREAD_LIBS=

        ax_pthread_ok=yes

        # However, older versions of Clang make a point of warning the user
        # that, in an invocation where only linking and no compilation is
        # taking place, the -pthread option has no effect ("argument unused
        # during compilation").  They expect -pthread to be passed in only
        # when source code is being compiled.
        #
        # Problem is, this is at odds with the way Automake and most other
        # C build frameworks function, which is that the same flags used in
        # compilation (CFLAGS) are also used in linking.  Many systems
        # supported by AX_PTHREAD require exactly this for POSIX threads
        # support, and in fact it is often not straightforward to specify a
        # flag that is used only in the compilation phase and not in
        # linking.  Such a scenario is extremely rare in practice.
        #
        # Even though use of the -pthread flag in linking would only print
        # a warning, this can be a nuisance for well-run software projects
        # that build with -Werror.  So if the active version of Clang has
        # this misfeature, we search for an option to squash it.

        AC_CACHE_CHECK([whether Clang needs flag to prevent "argument unused" warning when linking with -pthread],
            [ax_cv_PTHREAD_CLANG_NO_WARN_FLAG],
            [ax_cv_PTHREAD_CLANG_NO_WARN_FLAG=unknown
             # Create an alternate version of $ac_link that compiles and
             # links in two steps (.c -> .o, .o -> exe) instead of one
             # (.c -> exe), because the warning occurs only in the second
             # step
             ax_pthread_save_ac_link="$ac_link"
             ax_pthread_sed='s/conftest\.\$ac_ext/conftest.$ac_objext/g'
             ax_pthread_link_step=`$as_echo "$ac_link" | sed "$ax_pthread_sed"`
             ax_pthread_2step_ac_link="($ac_compile) && (echo ==== >&5) && ($ax_pthread_link_step)"
             ax_pthread_save_CFLAGS="$CFLAGS"
             for ax_pthread_try in '' -Qunused-arguments -Wno-unused-command-line-argument unknown; do
                AS_IF([test "x$ax_pthread_try" = "xunknown"], [break])
                CFLAGS="-Werror -Wunknown-warning-option $ax_pthread_try -pthread $ax_pthread_save_CFLAGS"
                ac_link="$ax_pthread_save_ac_link"
                AC_LINK_IFELSE([AC_LANG_SOURCE([[int main(void){return 0;}]])],
                    [ac_link="$ax_pthread_2step_ac_link"
                     AC_LINK_IFELSE([AC_LANG_SOURCE([[int main(void){return 0;}]])],
                         [break])
                    ])
             done
             ac_link="$ax_pthread_save_ac_link"
             CFLAGS="$ax_pthread_save_CFLAGS"
             AS_IF([test "x$ax_pthread_try" = "x"], [ax_pthread_try=no])
             ax_cv_PTHREAD_CLANG_NO_WARN_FLAG="$ax_pthread_try"
            ])

        case "$ax_cv_PTHREAD_CLANG_NO_WARN_FLAG" in
                no | unknown) ;;
                *) PTHREAD_CFLAGS="$ax_cv_PTHREAD_CLANG_NO_WARN_FLAG $PTHREAD_CFLAGS" ;;
        esac

fi # $ax_pthread_clang = yes

if test "x$ax_pthread_ok" = "xno"; then
for ax_pthread_try_flag in $ax_pthread_flags; do

        case $ax_pthread_try_flag in
                none)
                AC_MSG_CHECKING([whether pthreads work without any flags])
                ;;

                -mt,pthread)
                AC_MSG_CHECKING([whether pthreads work with -mt -lpthread])
                PTHREAD_CFLAGS="-mt"
                PTHREAD_LIBS="-lpthread"
                ;;

                -*)
                AC_MSG_CHECKING([whether pthreads work with $ax_pthread_try_flag])
                PTHREAD_CFLAGS="$ax_pthread_try_flag"
                ;;

                pthread-config)
                AC_CHECK_PROG([ax_pthread_config], [pthread-config], [yes], [no])
                AS_IF([test "x$ax_pthread_config" = "xno"], [continue])
                PTHREAD_CFLAGS="`pthread-config --cflags`"
                PTHREAD_LIBS="`pthread-config --ldflags` `pthread-config --libs`"
                ;;

                *)
                AC_MSG_CHECKING([for the pthreads library -l$ax_pthread_try_flag])
                PTHREAD_LIBS="-l$ax_pthread_try_flag"
                ;;
        esac

        ax_pthread_save_CFLAGS="$CFLAGS"
        ax_pthread_save_LIBS="$LIBS"
        CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
        LIBS="$PTHREAD_LIBS $LIBS"

        # Check for various functions.  We must include pthread.h,
        # since some functions may be macros.  (On the Sequent, we
        # need a special flag -Kthread to make this header compile.)
        # We check for pthread_join because it is in -lpthread on IRIX
        # while pthread_create is in libc.  We check for pthread_attr_init
        # due to DEC craziness with -lpthreads.  We check for
        # pthread_cleanup_push because it is one of the few pthread
        # functions on Solaris that doesn't have a non-functional libc stub.
        # We try pthread_create on general principles.

        AC_LINK_IFELSE([AC_LANG_PROGRAM([#include <pthread.h>
#                       if $ax_pthread_check_cond
#                        error "$ax_pthread_check_macro must be defined"
#                       endif
                        static void routine(void *a) { a = 0; }
                        static void *start_routine(void *a) { return a; }],
                       [pthread_t th; pthread_attr_t attr;
                        pthread_create(&th, 0, start_routine, 0);
                        pthread_join(th, 0);
                        pthread_attr_init(&attr);
                        pthread_cleanup_push(routine, 0);
                        pthread_cleanup_pop(0) /* ; */])],
            [ax_pthread_ok=yes],
            [])

        CFLAGS="$ax_pthread_save_CFLAGS"
        LIBS="$ax_pthread_save_LIBS"

        AC_MSG_RESULT([$ax_pthread_ok])
        AS_IF([test "x$ax_pthread_ok" = "xyes"], [break])

        PTHREAD_LIBS=""
        PTHREAD_CFLAGS=""
done
fi

# Various other checks:
if test "x$ax_pthread_ok" = "xyes"; then
        ax_pthread_save_CFLAGS="$CFLAGS"
        ax_pthread_save_LIBS="$LIBS"
        CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
        LIBS="$PTHREAD_LIBS $LIBS"

        # Detect AIX lossage: JOINABLE attribute is called UNDETACHED.
        AC_CACHE_CHECK([for joinable pthread attribute],
            [ax_cv_PTHREAD_JOINABLE_ATTR],
            [ax_cv_PTHREAD_JOINABLE_ATTR=unknown
             for ax_pthread_attr in PTHREAD_CREATE_JOINABLE PTHREAD_CREATE_UNDETACHED; do
                 AC_LINK_IFELSE([AC_LANG_PROGRAM([#include <pthread.h>],
                                                 [int attr = $ax_pthread_attr; return attr /* ; */])],
                                [ax_cv_PTHREAD_JOINABLE_ATTR=$ax_pthread_attr; break],
                                [])
             done
            ])
        AS_IF([test "x$ax_cv_PTHREAD_JOINABLE_ATTR" != "xunknown" && \
               test "x$ax_cv_PTHREAD_JOINABLE_ATTR" != "xPTHREAD_CREATE_JOINABLE" && \
               test "x$ax_pthread_joinable_attr_defined" != "xyes"],
              [AC_DEFINE_UNQUOTED([PTHREAD_CREATE_JOINABLE],
                                  [$ax_cv_PTHREAD_JOINABLE_ATTR],
                                  [Define to necessary symbol if this constant
                                   uses a non-standard name on your system.])
               ax_pthread_joinable_attr_defined=yes
              ])

        AC_CACHE_CHECK([whether more special flags are required for pthreads],
            [ax_cv_PTHREAD_SPECIAL_FLAGS],
            [ax_cv_PTHREAD_SPECIAL_FLAGS=no
             case $host_os in
             solaris*)
             ax_cv_PTHREAD_SPECIAL_FLAGS="-D_POSIX_PTHREAD_SEMANTICS"
             ;;
             esac
            ])
        AS_IF([test "x$ax_cv_PTHREAD_SPECIAL_FLAGS" != "xno" && \
               test "x$ax_pthread_special_flags_added" != "xyes"],
              [PTHREAD_CFLAGS="$ax_cv_PTHREAD_SPECIAL_FLAGS $PTHREAD_CFLAGS"
               ax_pthread_special_flags_added=yes])

        AC_CACHE_CHECK([for PTHREAD_PRIO_INHERIT],
            [ax_cv_PTHREAD_PRIO_INHERIT],
            [AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <pthread.h>]],
                                             [[int i = PTHREAD_PRIO_INHERIT;]])],
                            [ax_cv_PTHREAD_PRIO_INHERIT=yes],
                            [ax_cv_PTHREAD_PRIO_INHERIT=no])
            ])
        AS_IF([test "x$ax_cv_PTHREAD_PRIO_INHERIT" = "xyes" && \
               test "x$ax_pthread_prio_inherit_defined" != "xyes"],
              [AC_DEFINE([HAVE_PTHREAD_PRIO_INHERIT], [1], [Have PTHREAD_PRIO_INHERIT.])
               ax_pthread_prio_inherit_defined=yes
              ])

        CFLAGS="$ax_pthread_save_CFLAGS"
        LIBS="$ax_pthread_save_LIBS"

        # More AIX lossage: compile with *_r variant
        if test "x$GCC" != "xyes"; then
            case $host_os in
                aix*)
                AS_CASE(["x/$CC"],
                    [x*/c89|x*/c89_128|x*/c99|x*/c99_128|x*/cc|x*/cc128|x*/xlc|x*/xlc_v6|x*/xlc128|x*/xlc128_v6],
                    [#handle absolute path differently from PATH based program lookup
                     AS_CASE(["x$CC"],
                         [x/*],
                         [AS_IF([AS_EXECUTABLE_P([${CC}_r])],[PTHREAD_CC="${CC}_r"])],
                         [AC_CHECK_PROGS([PTHREAD_CC],[${CC}_r],[$CC])])])
                ;;
            esac
        fi
fi

test -n "$PTHREAD_CC" || PTHREAD_CC="$CC"

AC_SUBST([PTHREAD_LIBS])
AC_SUBST([PTHREAD_CFLAGS])
AC_SUBST([PTHREAD_CC])

# Finally, execute ACTION-IF-FOUND/ACTION-IF-NOT-FOUND:
if test "x$ax_pthread_ok" = "xyes"; then
        ifelse([$1],,[AC_DEFINE([HAVE_PTHREAD],[1],[Define if you have POSIX threads libraries and header files.])],[$1])
        :
else
        ax_pthread_ok=no
        $2
fi
AC_LANG_POP
])dnl AX_PTHREAD
