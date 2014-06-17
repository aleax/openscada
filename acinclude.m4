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
#   Copyright (c) 2008 Mateusz Loskot <mateusz@loskot.net>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved.

AC_DEFUN([AX_LIB_MYSQL],
[
    AC_ARG_WITH([mysql],
        AC_HELP_STRING([--with-mysql=@<:@ARG@:>@],
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
        ],
        [want_mysql="yes"]
    )

    MYSQL_CFLAGS=""
    MYSQL_LDFLAGS=""
    MYSQL_VERSION=""

    dnl
    dnl Check MySQL libraries (libpq)
    dnl

    if test "$want_mysql" = "yes"; then

        if test -z "$MYSQL_CONFIG" -o test; then
            AC_PATH_PROG([MYSQL_CONFIG], [mysql_config], [no])
        fi

        if test "$MYSQL_CONFIG" != "no"; then
            AC_MSG_CHECKING([for MySQL libraries])

            MYSQL_CFLAGS="`$MYSQL_CONFIG --cflags`"
            MYSQL_LDFLAGS="`$MYSQL_CONFIG --libs`"

            MYSQL_VERSION=`$MYSQL_CONFIG --version`

            AC_DEFINE([HAVE_MYSQL], [1],
                [Define to 1 if MySQL libraries are available])

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

    if test "$found_mysql" = "yes" -a -n "$mysql_version_req"; then

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
        AC_HELP_STRING(
            [--with-firebird=@<:@ARG@:>@],
            [use Firebird client library @<:@default=yes@:>@, optionally specify the prefix for firebird library]
        ),
        [
        if test "$withval" = "no"; then
            WANT_FIREBIRD="no"
        elif test "$withval" = "yes"; then
            WANT_FIREBIRD="yes"
            ac_firebird_path=""
        else
            WANT_FIREBIRD="yes"
            ac_firebird_path="$withval"
        fi
        ],
        [WANT_FIREBIRD="yes"]
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
        if test "x$firebird_version_req_micro" = "x" ; then
            firebird_version_req_micro="0"
        fi

        dnl FB_API_VER represents the version of Firebird as follows:
        dnl - Any version of Interbase, or Firebird 1.0.x:      10
        dnl - Firebird 1.5.x:                                   15
        dnl - Firebird 2.0.x:                                   20

        firebird_version_req_number=`expr $firebird_version_req_major \+ $firebird_version_req_minor`

        AC_MSG_CHECKING([for Firebird client library >= $firebird_version_req])

        if test "$ac_firebird_path" != ""; then
            ac_firebird_ldflags="-L$ac_firebird_path/lib"
            ac_firebird_cppflags="-I$ac_firebird_path/include"
        else
            for ac_firebird_path_tmp in /usr /usr/local /opt ; do
                if test -f "$ac_firebird_path_tmp/include/$ac_firebird_header" \
                    && test -r "$ac_firebird_path_tmp/include/$ac_firebird_header"; then
                    ac_firebird_path=$ac_firebird_path_tmp
                    ac_firebird_cppflags="-I$ac_firebird_path_tmp/include"
                    ac_firebird_ldflags="-L$ac_firebird_path_tmp/lib"
                    break;
                fi
            done
        fi

        ac_firebird_header_path="$ac_firebird_path/include/$ac_firebird_header"

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
#                AC_DEFINE(HAVE_FIREBIRD)
            fi
        fi
    fi
])
# ===========================================================================
#             http://autoconf-archive.cryp.to/ax_lib_sqlite3.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_LIB_SQLITE3([MINIMUM-VERSION])
#
# DESCRIPTION
#
#   Test for the SQLite 3 library of a particular version (or newer)
#
#   This macro takes only one optional argument, required version of SQLite
#   3 library. If required version is not passed, 3.0.0 is used in the test
#   of existance of SQLite 3.
#
#   If no intallation prefix to the installed SQLite library is given the
#   macro searches under /usr, /usr/local, and /opt.
#
#   This macro calls:
#
#     AC_SUBST(SQLITE3_CFLAGS)
#     AC_SUBST(SQLITE3_LDFLAGS)
#     AC_SUBST(SQLITE3_VERSION)
#
#   And sets:
#
#     HAVE_SQLITE3
#
# LICENSE
#
#   Copyright (c) 2008 Mateusz Loskot <mateusz@loskot.net>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved.

AC_DEFUN([AX_LIB_SQLITE3],
[
    AC_ARG_WITH([sqlite3],
        AC_HELP_STRING(
            [--with-sqlite3=@<:@ARG@:>@],
            [use SQLite 3 library @<:@default=yes@:>@, optionally specify the prefix for sqlite3 library]
        ),
        [
        if test "$withval" = "no"; then
            WANT_SQLITE3="no"
        elif test "$withval" = "yes"; then
            WANT_SQLITE3="yes"
            ac_sqlite3_path=""
        else
            WANT_SQLITE3="yes"
            ac_sqlite3_path="$withval"
        fi
        ],
        [WANT_SQLITE3="yes"]
    )

    SQLITE3_CFLAGS=""
    SQLITE3_LDFLAGS=""
    SQLITE3_VERSION=""

    if test "x$WANT_SQLITE3" = "xyes"; then

        ac_sqlite3_header="sqlite3.h"

        sqlite3_version_req=ifelse([$1], [], [3.0.0], [$1])
        sqlite3_version_req_shorten=`expr $sqlite3_version_req : '\([[0-9]]*\.[[0-9]]*\)'`
        sqlite3_version_req_major=`expr $sqlite3_version_req : '\([[0-9]]*\)'`
        sqlite3_version_req_minor=`expr $sqlite3_version_req : '[[0-9]]*\.\([[0-9]]*\)'`
        sqlite3_version_req_micro=`expr $sqlite3_version_req : '[[0-9]]*\.[[0-9]]*\.\([[0-9]]*\)'`
        if test "x$sqlite3_version_req_micro" = "x" ; then
            sqlite3_version_req_micro="0"
        fi

        sqlite3_version_req_number=`expr $sqlite3_version_req_major \* 1000000 \
                                   \+ $sqlite3_version_req_minor \* 1000 \
                                   \+ $sqlite3_version_req_micro`

        AC_MSG_CHECKING([for SQLite3 library >= $sqlite3_version_req])

        if test "$ac_sqlite3_path" != ""; then
            ac_sqlite3_ldflags="-L$ac_sqlite3_path/lib"
            ac_sqlite3_cppflags="-I$ac_sqlite3_path/include"
        else
            for ac_sqlite3_path_tmp in /usr /usr/local /opt ; do
                if test -f "$ac_sqlite3_path_tmp/include/$ac_sqlite3_header" \
                    && test -r "$ac_sqlite3_path_tmp/include/$ac_sqlite3_header"; then
                    ac_sqlite3_path=$ac_sqlite3_path_tmp
                    ac_sqlite3_cppflags="-I$ac_sqlite3_path_tmp/include"
                    ac_sqlite3_ldflags="-L$ac_sqlite3_path_tmp/lib"
                    break;
                fi
            done
        fi

        ac_sqlite3_ldflags="$ac_sqlite3_ldflags -lsqlite3"

        saved_CPPFLAGS="$CPPFLAGS"
        CPPFLAGS="$CPPFLAGS $ac_sqlite3_cppflags"

        AC_LANG_PUSH(C++)
        AC_COMPILE_IFELSE(
            [
            AC_LANG_PROGRAM([[@%:@include <sqlite3.h>]],
                [[
#if (SQLITE_VERSION_NUMBER >= $sqlite3_version_req_number)
// Everything is okay
#else
#  error SQLite version is too old
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

            SQLITE3_CFLAGS="$ac_sqlite3_cppflags"
            SQLITE3_LDFLAGS="$ac_sqlite3_ldflags"

            ac_sqlite3_header_path="$ac_sqlite3_path/include/$ac_sqlite3_header"

            dnl Retrieve SQLite release version
            if test "x$ac_sqlite3_header_path" != "x"; then
                ac_sqlite3_version=`cat $ac_sqlite3_header_path \
                    | grep '#define.*SQLITE_VERSION.*\"' | sed -e 's/.* "//' \
                        | sed -e 's/"//'`
                if test $ac_sqlite3_version != ""; then
                    SQLITE3_VERSION=$ac_sqlite3_version
                else
                    AC_MSG_WARN([Can not find SQLITE_VERSION macro in sqlite3.h header to retrieve SQLite version!])
                fi
            fi

            AC_SUBST(SQLITE3_CFLAGS)
            AC_SUBST(SQLITE3_LDFLAGS)
            AC_SUBST(SQLITE3_VERSION)
            AC_DEFINE([HAVE_SQLITE3], [], [Have the SQLITE3 library])
        fi
    fi
])
# ===========================================================================
#              http://autoconf-archive.cryp.to/bnv_have_qt.html
# ===========================================================================
#
# SYNOPSIS
#
#   BNV_HAVE_QT [--with-Qt-dir=DIR] [--with-Qt-lib-dir=DIR] [--with-Qt-lib=LIB]
#   BNV_HAVE_QT [--with-Qt-include-dir=DIR] [--with-Qt-bin-dir=DIR] [--with-Qt-lib-dir=DIR] [--with-Qt-lib=LIB]
#
# DESCRIPTION
#
#   Searches common directories for Qt include files, libraries and Qt
#   binary utilities. The macro supports several different versions of the
#   Qt framework being installed on the same machine. Without options, the
#   macro is designed to look for the latest library, i.e., the highest
#   definition of QT_VERSION in qglobal.h. By use of one or more options a
#   different library may be selected. There are two different sets of
#   options. Both sets contain the option --with-Qt-lib=LIB which can be
#   used to force the use of a particular version of the library file when
#   more than one are available. LIB must be in the form as it would appear
#   behind the "-l" option to the compiler. Examples for LIB would be
#   "qt-mt" for the multi-threaded version and "qt" for the regular version.
#   In addition to this, the first set consists of an option
#   --with-Qt-dir=DIR which can be used when the installation conforms to
#   Trolltech's standard installation, which means that header files are in
#   DIR/include, binary utilities are in DIR/bin and the library is in
#   DIR/lib. The second set of options can be used to indicate individual
#   locations for the header files, the binary utilities and the library
#   file, in addition to the specific version of the library file.
#
#   The following shell variable is set to either "yes" or "no":
#
#     have_qt
#
#   Additionally, the following variables are exported:
#
#     QT_CXXFLAGS
#     QT_LIBS
#     QT_MOC
#     QT_UIC
#     QT_LRELEASE
#     QT_LUPDATE
#     QT_DIR
#
#   which respectively contain an "-I" flag pointing to the Qt include
#   directory (and "-DQT_THREAD_SUPPORT" when LIB is "qt-mt"), link flags
#   necessary to link with Qt and X, the name of the meta object compiler
#   and the user interface compiler both with full path, and finaly the
#   variable QTDIR as Trolltech likes to see it defined (if possible).
#
#   Example lines for Makefile.in:
#
#     CXXFLAGS = @QT_CXXFLAGS@
#     MOC      = @QT_MOC@
#
#   After the variables have been set, a trial compile and link is performed
#   to check the correct functioning of the meta object compiler. This test
#   may fail when the different detected elements stem from different
#   releases of the Qt framework. In that case, an error message is emitted
#   and configure stops.
#
#   No common variables such as $LIBS or $CFLAGS are polluted.
#
#   Options:
#
#   --with-Qt-dir=DIR: DIR is equal to $QTDIR if you have followed the
#   installation instructions of Trolltech. Header files are in DIR/include,
#   binary utilities are in DIR/bin and the library is in DIR/lib.
#
#   --with-Qt-include-dir=DIR: Qt header files are in DIR.
#
#   --with-Qt-bin-dir=DIR: Qt utilities such as moc and uic are in DIR.
#
#   --with-Qt-lib-dir=DIR: The Qt library is in DIR.
#
#   --with-Qt-lib=LIB: Use -lLIB to link with the Qt library.
#
#   If some option "=no" or, equivalently, a --without-Qt-* version is given
#   in stead of a --with-Qt-*, "have_qt" is set to "no" and the other
#   variables are set to the empty string.
#
# LICENSE
#
#   Copyright (c) 2008 Bastiaan Veelo <Bastiaan@Veelo.net>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved.

dnl Copyright (C) 2001, 2002, 2003, 2005, 2006, 2007 Bastiaan Veelo

dnl THANKS! This code includes bug fixes and contributions made by:
dnl Tim McClarren,
dnl Dennis R. Weilert,
dnl Qingning Huo,
dnl Brian Mingus,
dnl Jens Hannemann,
dnl Pavel Roskin,
dnl Scott J. Bertin.
dnl Pierre Hebert

dnl ChangeLog
dnl 2008-04-12  * Add QT_LRELEASE and QT_LUPDATE (due to Pierre Hebert)
dnl 2008-04-12  * Hide output of ls and fix an m4 quoting problem (due to Scott J. Bertin).
dnl 2008-04-12  * Check compiler return value instead of parsing the error stream,
dnl               which detected warnings as false negatives (due to Jens Hannemann).
dnl 2008-04-12  * Spelling of "Success".
dnl             * Fixed unsave test for $bnv_qt_lib without quotes.
dnl             * Put dnl in front of all comments.
dnl             * Changed -l$bnv_qt_lib_dir into -L$bnv_qt_lib_dir (all due to Pavel Roskin).
dnl 2008-04-12  * Support for 64bit architectures.
dnl             * Updated documentation.
dnl 2008-04-12: * Fix "cat: bnv_qt_test.c: No such file or directory" (due to Jens Hannemann).
dnl             * Hide output of failing ls.
dnl 2008-04-12: * Check in /Developer on Mac OS X; Check in $QTDIR (due to Brian Mingus).

dnl Calls BNV_PATH_QT_DIRECT (contained in this file) as a subroutine.
AC_DEFUN([BNV_HAVE_QT],
[
  AC_REQUIRE([AC_PROG_CXX])
  AC_REQUIRE([AC_PATH_X])
  AC_REQUIRE([AC_PATH_XTRA])

  AC_MSG_CHECKING(for Qt)

  AC_ARG_WITH([Qt-dir],
    [  --with-Qt-dir=DIR       DIR is equal to $QTDIR if you have followed the
                          installation instructions of Trolltech. Header
                          files are in DIR/include, binary utilities are
                          in DIR/bin. The library is in DIR/lib, unless
			  --with-Qt-lib-dir is also set.])
  AC_ARG_WITH([Qt-include-dir],
    [  --with-Qt-include-dir=DIR
                          Qt header files are in DIR])
  AC_ARG_WITH([Qt-bin-dir],
    [  --with-Qt-bin-dir=DIR   Qt utilities such as moc and uic are in DIR])
  AC_ARG_WITH([Qt-lib-dir],
    [  --with-Qt-lib-dir=DIR   The Qt library is in DIR])
  AC_ARG_WITH([Qt-lib],
    [  --with-Qt-lib=LIB       Use -lLIB to link with the Qt library])
  if test x"$with_Qt_dir" = x"no" ||
     test x"$with_Qt_include-dir" = x"no" ||
     test x"$with_Qt_bin_dir" = x"no" ||
     test x"$with_Qt_lib_dir" = x"no" ||
     test x"$with_Qt_lib" = x"no"; then
    # user disabled Qt. Leave cache alone.
    have_qt="User disabled Qt."
  else
    # "yes" is a bogus option
    if test x"$with_Qt_dir" = xyes; then
      with_Qt_dir=
    fi
    if test x"$with_Qt_include_dir" = xyes; then
      with_Qt_include_dir=
    fi
    if test x"$with_Qt_bin_dir" = xyes; then
      with_Qt_bin_dir=
    fi
    if test x"$with_Qt_lib_dir" = xyes; then
      with_Qt_lib_dir=
    fi
    if test x"$with_Qt_lib" = xyes; then
      with_Qt_lib=
    fi
    # No Qt unless we discover otherwise
    have_qt=no
    # Check whether we are requested to link with a specific version
    if test x"$with_Qt_lib" != x; then
      bnv_qt_lib="$with_Qt_lib"
    fi
    # Check whether we were supplied with an answer already
    if test x"$with_Qt_dir" != x; then
      have_qt=yes
      bnv_qt_dir="$with_Qt_dir"
      bnv_qt_include_dir="$with_Qt_dir/include"
      bnv_qt_bin_dir="$with_Qt_dir/bin"
      bnv_qt_lib_dir="$with_Qt_dir/lib"
      # Only search for the lib if the user did not define one already
      if test x"$bnv_qt_lib" = x; then
        bnv_qt_lib="`ls $bnv_qt_lib_dir/libqt* | sed -n 1p |
                     sed s@$bnv_qt_lib_dir/lib@@ | [sed s@[.].*@@]`"
      fi
      bnv_qt_LIBS="-L$bnv_qt_lib_dir -l$bnv_qt_lib $X_PRE_LIBS $X_LIBS -lX11 -lXext -lXmu -lXt -lXi $X_EXTRA_LIBS"
    else
      # Use cached value or do search, starting with suggestions from
      # the command line
      AC_CACHE_VAL(bnv_cv_have_qt,
      [
        # We are not given a solution and there is no cached value.
        bnv_qt_dir=NO
        bnv_qt_include_dir=NO
        bnv_qt_lib_dir=NO
        if test x"$bnv_qt_lib" = x; then
          bnv_qt_lib=NO
        fi
        BNV_PATH_QT_DIRECT
        if test "$bnv_qt_dir" = NO ||
           test "$bnv_qt_include_dir" = NO ||
           test "$bnv_qt_lib_dir" = NO ||
           test "$bnv_qt_lib" = NO; then
          # Problem with finding complete Qt.  Cache the known absence of Qt.
          bnv_cv_have_qt="have_qt=no"
        else
          # Record where we found Qt for the cache.
          bnv_cv_have_qt="have_qt=yes                  \
                       bnv_qt_dir=$bnv_qt_dir          \
               bnv_qt_include_dir=$bnv_qt_include_dir  \
                   bnv_qt_bin_dir=$bnv_qt_bin_dir      \
                      bnv_qt_LIBS=\"$bnv_qt_LIBS\""
        fi
      ])dnl
      eval "$bnv_cv_have_qt"
    fi # all $bnv_qt_* are set
  fi   # $have_qt reflects the system status
  if test x"$have_qt" = xyes; then
    QT_CXXFLAGS="-I$bnv_qt_include_dir"
    if test x"$bnv_qt_lib" = xqt-mt; then
        QT_CXXFLAGS="$QT_CXXFLAGS -DQT_THREAD_SUPPORT"
    fi
    QT_DIR="$bnv_qt_dir"
    QT_LIBS="$bnv_qt_LIBS"
    # If bnv_qt_dir is defined, utilities are expected to be in the
    # bin subdirectory
    if test x"$bnv_qt_dir" != x; then
        if test -x "$bnv_qt_dir/bin/uic"; then
          QT_UIC="$bnv_qt_dir/bin/uic"
        else
          # Old versions of Qt don't have uic
          QT_UIC=
        fi
      QT_MOC="$bnv_qt_dir/bin/moc"
      QT_LRELEASE="$bnv_qt_dir/bin/lrelease"
      QT_LUPDATE="$bnv_qt_dir/bin/lupdate"
    else
      # Or maybe we are told where to look for the utilities
      if test x"$bnv_qt_bin_dir" != x; then
        if test -x "$bnv_qt_bin_dir/uic"; then
          QT_UIC="$bnv_qt_bin_dir/uic"
        else
          # Old versions of Qt don't have uic
          QT_UIC=
        fi
        QT_MOC="$bnv_qt_bin_dir/moc"
        QT_LRELEASE="$bnv_qt_bin_dir/lrelease"
        QT_LUPDATE="$bnv_qt_bin_dir/lupdate"
      else
      # Last possibility is that they are in $PATH
        QT_UIC="`which uic`"
        QT_MOC="`which moc`"
        QT_LRELEASE="`which lrelease`"
        QT_LUPDATE="`which lupdate`"
      fi
    fi
    # All variables are defined, report the result
    AC_MSG_RESULT([$have_qt:
    QT_CXXFLAGS=$QT_CXXFLAGS
    QT_DIR=$QT_DIR
    QT_LIBS=$QT_LIBS
    QT_UIC=$QT_UIC
    QT_MOC=$QT_MOC
    QT_LRELEASE=$QT_LRELEASE
    QT_LUPDATE=$QT_LUPDATE])
  else
    # Qt was not found
    QT_CXXFLAGS=
    QT_DIR=
    QT_LIBS=
    QT_UIC=
    QT_MOC=
    QT_LRELEASE=
    QT_LUPDATE=
    AC_MSG_RESULT($have_qt)
  fi
  AC_SUBST(QT_CXXFLAGS)
  AC_SUBST(QT_DIR)
  AC_SUBST(QT_LIBS)
  AC_SUBST(QT_UIC)
  AC_SUBST(QT_MOC)
  AC_SUBST(QT_LRELEASE)
  AC_SUBST(QT_LUPDATE)

  #### Being paranoid:
  if test x"$have_qt" = xyes; then
    AC_MSG_CHECKING(correct functioning of Qt installation)
    AC_CACHE_VAL(bnv_cv_qt_test_result,
    [
      cat > bnv_qt_test.h << EOF
#include <qobject.h>
class Test : public QObject
{
Q_OBJECT
public:
  Test() {}
  ~Test() {}
public slots:
  void receive() {}
signals:
  void send();
};
EOF

      cat > bnv_qt_main.$ac_ext << EOF
#include "bnv_qt_test.h"
#include <qapplication.h>
int main( int argc, char **argv )
{
  QApplication app( argc, argv );
  Test t;
  QObject::connect( &t, SIGNAL(send()), &t, SLOT(receive()) );
}
EOF

      bnv_cv_qt_test_result="failure"
      bnv_try_1="$QT_MOC bnv_qt_test.h -o moc_bnv_qt_test.$ac_ext >/dev/null 2>/dev/null"
      AC_TRY_EVAL(bnv_try_1)
      if test x"$ac_status" != x0; then
        echo "$bnv_err_1" >&AC_FD_CC
        echo "configure: could not run $QT_MOC on:" >&AC_FD_CC
        cat bnv_qt_test.h >&AC_FD_CC
      else
        bnv_try_2="$CXX $QT_CXXFLAGS -c $CXXFLAGS -o moc_bnv_qt_test.o moc_bnv_qt_test.$ac_ext >/dev/null 2>/dev/null"
        AC_TRY_EVAL(bnv_try_2)
        if test x"$ac_status" != x0; then
          echo "$bnv_err_2" >&AC_FD_CC
          echo "configure: could not compile:" >&AC_FD_CC
          cat moc_bnv_qt_test.$ac_ext >&AC_FD_CC
        else
          bnv_try_3="$CXX $QT_CXXFLAGS -c $CXXFLAGS -o bnv_qt_main.o bnv_qt_main.$ac_ext >/dev/null 2>/dev/null"
          AC_TRY_EVAL(bnv_try_3)
          if test x"$ac_status" != x0; then
            echo "$bnv_err_3" >&AC_FD_CC
            echo "configure: could not compile:" >&AC_FD_CC
            cat bnv_qt_main.$ac_ext >&AC_FD_CC
          else
            bnv_try_4="$CXX $QT_LIBS $LIBS -o bnv_qt_main bnv_qt_main.o moc_bnv_qt_test.o >/dev/null 2>/dev/null"
            AC_TRY_EVAL(bnv_try_4)
            if test x"$ac_status" != x0; then
              echo "$bnv_err_4" >&AC_FD_CC
            else
              bnv_cv_qt_test_result="success"
            fi
          fi
        fi
      fi
    ])dnl AC_CACHE_VAL bnv_cv_qt_test_result
    AC_MSG_RESULT([$bnv_cv_qt_test_result]);
    if test x"$bnv_cv_qt_test_result" = "xfailure"; then
      AC_MSG_ERROR([Failed to find matching components of a complete
                  Qt installation. Try using more options,
                  see ./configure --help.])
    fi

    rm -f bnv_qt_test.h moc_bnv_qt_test.$ac_ext moc_bnv_qt_test.o \
          bnv_qt_main.$ac_ext bnv_qt_main.o bnv_qt_main
  fi
])

dnl Internal subroutine of BNV_HAVE_QT
dnl Set bnv_qt_dir bnv_qt_include_dir bnv_qt_bin_dir bnv_qt_lib_dir bnv_qt_lib
AC_DEFUN([BNV_PATH_QT_DIRECT],
[
  ## Binary utilities ##
  if test x"$with_Qt_bin_dir" != x; then
    bnv_qt_bin_dir=$with_Qt_bin_dir
  fi
  ## Look for header files ##
  if test x"$with_Qt_include_dir" != x; then
    bnv_qt_include_dir="$with_Qt_include_dir"
  else
    # The following header file is expected to define QT_VERSION.
    qt_direct_test_header=qglobal.h
    # Look for the header file in a standard set of common directories.
    bnv_include_path_list="
      /usr/include
      `ls -dr ${QTDIR}/include 2>/dev/null`
      `ls -dr /usr/include/qt* 2>/dev/null`
      `ls -dr /usr/lib/qt*/include 2>/dev/null`
      `ls -dr /usr/local/qt*/include 2>/dev/null`
      `ls -dr /opt/qt*/include 2>/dev/null`
      `ls -dr /Developer/qt*/include 2>/dev/null`
    "
    for bnv_dir in $bnv_include_path_list; do
      if test -r "$bnv_dir/$qt_direct_test_header"; then
        bnv_dirs="$bnv_dirs $bnv_dir"
      fi
    done

    echo $bnv_dirs

    # Now look for the newest in this list
    bnv_prev_ver=0
    for bnv_dir in $bnv_dirs; do
      bnv_this_ver=`egrep -w '#define QT_VERSION' $bnv_dir/$qt_direct_test_header | sed s/'#define QT_VERSION'//`
      if expr $bnv_this_ver '>' $bnv_prev_ver > /dev/null; then
        bnv_qt_include_dir=$bnv_dir
        bnv_prev_ver=$bnv_this_ver
      fi
    done
  fi dnl Found header files.



  # Are these headers located in a traditional Trolltech installation?
  # That would be $bnv_qt_include_dir stripped from its last element:
  bnv_possible_qt_dir=`dirname $bnv_qt_include_dir`
  if (test -x $bnv_possible_qt_dir/bin/moc) &&
     ((ls $bnv_possible_qt_dir/lib/libqt* > /dev/null 2>/dev/null) ||
      (ls $bnv_possible_qt_dir/lib64/libqt* > /dev/null 2>/dev/null)); then
    # Then the rest is a piece of cake
    bnv_qt_dir=$bnv_possible_qt_dir
    bnv_qt_bin_dir="$bnv_qt_dir/bin"
    if test x"$with_Qt_lib_dir" != x; then
      bnv_qt_lib_dir="$with_Qt_lib_dir"
    else
      if (test -d $bnv_qt_dir/lib64); then
	bnv_qt_lib_dir="$bnv_qt_dir/lib64"
      else
	bnv_qt_lib_dir="$bnv_qt_dir/lib"
      fi
    fi
    # Only look for lib if the user did not supply it already
    if test x"$bnv_qt_lib" = xNO; then
      bnv_qt_lib="`ls $bnv_qt_lib_dir/libqt* | sed -n 1p |
                   sed s@$bnv_qt_lib_dir/lib@@ | [sed s@[.].*@@]`"
    fi
    bnv_qt_LIBS="-L$bnv_qt_lib_dir -l$bnv_qt_lib $X_PRE_LIBS $X_LIBS -lX11 -lXext -lXmu -lXt -lXi $X_EXTRA_LIBS"
  else
    # There is no valid definition for $QTDIR as Trolltech likes to see it
    bnv_qt_dir=
    ## Look for Qt library ##
    if test x"$with_Qt_lib_dir" != x; then
      bnv_qt_lib_dir="$with_Qt_lib_dir"
      # Only look for lib if the user did not supply it already
      if test x"$bnv_qt_lib" = xNO; then
        bnv_qt_lib="`ls $bnv_qt_lib_dir/libqt* | sed -n 1p |
                     sed s@$bnv_qt_lib_dir/lib@@ | [sed s@[.].*@@]`"
      fi
      bnv_qt_LIBS="-L$bnv_qt_lib_dir -l$bnv_qt_lib $X_PRE_LIBS $X_LIBS -lX11 -lXext -lXmu -lXt -lXi $X_EXTRA_LIBS"
    else
      # Normally, when there is no traditional Trolltech installation,
      # the library is installed in a place where the linker finds it
      # automatically.
      # If the user did not define the library name, try with qt
      if test x"$bnv_qt_lib" = xNO; then
        bnv_qt_lib=qt
      fi
      qt_direct_test_header=qapplication.h
      qt_direct_test_main="
        int argc;
        char ** argv;
        QApplication app(argc,argv);
      "
      # See if we find the library without any special options.
      # Don't add top $LIBS permanently yet
      bnv_save_LIBS="$LIBS"
      LIBS="-l$bnv_qt_lib $X_PRE_LIBS $X_LIBS -lX11 -lXext -lXmu -lXt -lXi $X_EXTRA_LIBS"
      bnv_qt_LIBS="$LIBS"
      bnv_save_CXXFLAGS="$CXXFLAGS"
      CXXFLAGS="-I$bnv_qt_include_dir"
      AC_TRY_LINK([#include <$qt_direct_test_header>],
        $qt_direct_test_main,
      [
        # Success.
        # We can link with no special library directory.
        bnv_qt_lib_dir=
      ], [
        # That did not work. Try the multi-threaded version
        echo "Non-critical error, please neglect the above." >&AC_FD_CC
        bnv_qt_lib=qt-mt
        LIBS="-l$bnv_qt_lib $X_PRE_LIBS $X_LIBS -lX11 -lXext -lXmu -lXt -lXi $X_EXTRA_LIBS"
        AC_TRY_LINK([#include <$qt_direct_test_header>],
          $qt_direct_test_main,
        [
          # Success.
          # We can link with no special library directory.
          bnv_qt_lib_dir=
        ], [
          # That did not work. Try the OpenGL version
          echo "Non-critical error, please neglect the above." >&AC_FD_CC
          bnv_qt_lib=qt-gl
          LIBS="-l$bnv_qt_lib $X_PRE_LIBS $X_LIBS -lX11 -lXext -lXmu -lXt -lXi $X_EXTRA_LIBS"
          AC_TRY_LINK([#include <$qt_direct_test_header>],
            $qt_direct_test_main,
          [
            # Success.
            # We can link with no special library directory.
            bnv_qt_lib_dir=
          ], [
            # That did not work. Maybe a library version I don't know about?
            echo "Non-critical error, please neglect the above." >&AC_FD_CC
            # Look for some Qt lib in a standard set of common directories.
            bnv_dir_list="
              `echo $bnv_qt_includes | sed ss/includess`
              /lib
	      /usr/lib64
              /usr/lib
	      /usr/local/lib64
              /usr/local/lib
	      /opt/lib64
              /opt/lib
              `ls -dr /usr/lib64/qt* 2>/dev/null`
              `ls -dr /usr/lib64/qt*/lib64 2>/dev/null`
              `ls -dr /usr/lib/qt* 2>/dev/null`
              `ls -dr /usr/local/qt* 2>/dev/null`
              `ls -dr /opt/qt* 2>/dev/null`
            "
            for bnv_dir in $bnv_dir_list; do
              if ls $bnv_dir/libqt* >/dev/null 2>/dev/null; then
                # Gamble that it's the first one...
                bnv_qt_lib="`ls $bnv_dir/libqt* | sed -n 1p |
                            sed s@$bnv_dir/lib@@ | sed s/[[.]].*//`"
                bnv_qt_lib_dir="$bnv_dir"
                break
              fi
            done
            # Try with that one
            LIBS="-l$bnv_qt_lib $X_PRE_LIBS $X_LIBS -lX11 -lXext -lXmu -lXt -lXi $X_EXTRA_LIBS"
            AC_TRY_LINK([#include <$qt_direct_test_header>],
              $qt_direct_test_main,
            [
              # Success.
              # We can link with no special library directory.
              bnv_qt_lib_dir=
            ], [
              # Leave bnv_qt_lib_dir defined
            ])
          ])
        ])
      ])
      if test x"$bnv_qt_lib_dir" != x; then
        bnv_qt_LIBS="-L$bnv_qt_lib_dir $LIBS"
      else
        bnv_qt_LIBS="$LIBS"
      fi
      LIBS="$bnv_save_LIBS"
      CXXFLAGS="$bnv_save_CXXFLAGS"
    fi dnl $with_Qt_lib_dir was not given
  fi dnl Done setting up for non-traditional Trolltech installation
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

    POSTGRESQL_CFLAGS=""
    POSTGRESQL_LDFLAGS=""
    POSTGRESQL_VERSION=""

    dnl
    dnl Check PostgreSQL libraries (libpq)
    dnl

    if test "$want_postgresql" = "yes"; then

        if test -z "$PG_CONFIG" -o test; then
            AC_PATH_PROG([PG_CONFIG], [pg_config], [])
        fi

        if test ! -x "$PG_CONFIG"; then
            AC_MSG_ERROR([$PG_CONFIG does not exist or it is not an exectuable file])
            PG_CONFIG="no"
            found_postgresql="no"
        fi

        if test "$PG_CONFIG" != "no"; then
            AC_MSG_CHECKING([for PostgreSQL libraries])

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
#   Copyright (c) 2011 Roman Savochenko <rom_as@oscada.org>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.
AC_DEFUN([AX_LIB_SENSORS],
[
    AC_CHECK_HEADERS([sensors/sensors.h],
	[
	    AC_CHECK_LIB([sensors],[sensors_init],
		[AC_MSG_NOTICE([LibSensors: Pass global library using])
		LIB_SENSORS="-lsensors"],[AC_MSG_NOTICE(Libsensors library not found. Using is disabled!)])
	],[AC_MSG_NOTICE(DAQ.System: Libsensors headers not found. Using is disabled!)])
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
#   Copyright (c) 2011 Roman Savochenko <rom_as@oscada.org>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.
AC_DEFUN([AX_LIB_GD],
[
    if test "x${GDuse}" = "x"; then
	AC_CHECK_HEADERS([gd.h],[],
	    [AC_MSG_ERROR(GD library headers not found. Install or check GD developing package!)])
	AC_CHECK_LIB([gd],[gdImageCreate],[AC_MSG_NOTICE([LibGD: Pass global library using])],
	    [AC_MSG_ERROR(GD library not found. Install or check GDlib package!)])
	LIB_GD="-lgd"
	AC_SUBST(LIB_GD)
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
#
# DESCRIPTION
#
#   This macro provides tests of availability Open SSL library.
#
#   This macro calls:
#
#     AC_CHECK_HEADERS([openssl/ssl.h openssl/err.h openssl/bio.h])
#     AC_CHECK_LIB([ssl])
#     AC_SUBST(LIB_OpenSSL)
#
#   And sets:
#
#     OpenSSLuse=true
#
# LICENSE
#
#   Copyright (c) 2011 Roman Savochenko <rom_as@oscada.org>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.
AC_DEFUN([AX_LIB_OpenSSL],
[
    if test "x${OpenSSLuse}" = "x"; then
	AC_CHECK_HEADERS([openssl/ssl.h openssl/err.h openssl/bio.h],[],
	    [AC_MSG_ERROR(Transport.SSL: Some OpenSSL headers not found. Install or check OpenSSL developing package!)])
	AC_CHECK_LIB([ssl],[SSL_library_init],[AC_MSG_NOTICE([LibSSL: Pass global library using])],
	    [AC_MSG_ERROR(Transport.SSL: OpenSSL library not found. Install or check OpenSSL installation!)])
	LIB_OpenSSL="-lssl -lcrypto"
	AC_SUBST(LIB_OpenSSL)
	OpenSSLuse=true
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
#   Copyright (c) 2011 Roman Savochenko <rom_as@oscada.org>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.
AC_DEFUN([AX_LIB_FFTW3],
[
    if test "x${FFTW3use}" = "x"; then
	AC_CHECK_HEADERS([fftw3.h],
        [
            AC_CHECK_LIB([fftw3],[fftw_execute],
                [AC_MSG_NOTICE([LibFFTW3: Pass global library using])
                LIB_FFTW3="-lfftw3"],[AC_MSG_NOTICE(FFTW3 library not found. Using is disabled!)])
        ],[AC_MSG_NOTICE(Some FFTW3 headers not found. Using is disabled!)])
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
#     AC_SUBST(Qt_MOC)
#     AC_SUBST(Qt_RCC)
#
#   And sets:
#
#     Qt_use=true
#
# LICENSE
#
#   Copyright (c) 2011-2014 Roman Savochenko <rom_as@oscada.org>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.
AC_DEFUN([AX_LIB_Qt],
[
    if test "x${Qt_use}" = "x"; then
	QtGui=QtGui
	PKG_CHECK_MODULES([QtGui],[QtGui > 4.3.0],[],
	[
	    PKG_CHECK_MODULES([Qt5Widgets],[Qt5Widgets > 5.1.0],[],[AC_MSG_ERROR(QT4 or Qt5 library QtGui not found! Install Qt4 or Qt5 library development package.)])
	    PKG_CHECK_MODULES([Qt5PrintSupport],[Qt5PrintSupport > 5.1.0],[],[AC_MSG_ERROR(Qt5 library Qt5PrintSupport not found! Install Qt5 library development package.)])
	    QtGui_CFLAGS="$Qt5Widgets_CFLAGS $Qt5PrintSupport_CFLAGS"
	    QtGui_LIBS="$Qt5Widgets_LIBS $Qt5PrintSupport_LIBS"
	    QtGui=Qt5Widgets
	    Qt5_use=true
	])
	AC_SUBST(Qt_MOC)
	AC_SUBST(Qt_RCC)
	Qt_MOC="$(pkg-config --variable=moc_location ${QtGui})"
	Qt_RCC="$(pkg-config --variable=rcc_location ${QtGui})";
	if test "x${Qt_MOC}" = "x"; then Qt_MOC="$(pkg-config --variable=prefix ${QtGui})/bin/moc"; fi
	if test "x${Qt_RCC}" = "x"; then Qt_RCC="$(pkg-config --variable=prefix ${QtGui})/bin/rcc"; fi
	Qt_use=true
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
#     AC_HELP_STRING()
#     AM_CONDITIONAL()
#     AS_IF()
#
#   And sets:
#
#     enable_{ModName}=yes|incl|no
#
# LICENSE
#
#   Copyright (c) 2011 Roman Savochenko <rom_as@oscada.org>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

AC_DEFUN([AX_MOD_EN],
[
    AC_ARG_ENABLE([$1],AC_HELP_STRING([--$3-$1],[$2]),[ ],
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
    AC_ARG_ENABLE([$1],AC_HELP_STRING([--$3-$1],[$2]),[ ],[
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
	    AC_MSG_RESULT(Build module: DB.$1)
	    AC_CONFIG_FILES(src/moduls/bd/$1/Makefile)
	    DBSub_mod="${DBSub_mod}$1 "
	    if test $enable_$1 = incl; then
		LIB_CORE="${LIB_CORE} moduls/bd/$1/bd_$1.la "
		ModsIncl="${ModsIncl}bd_$1 "
		BldInclMods="${BldInclMods}moduls/bd/$1 "
	    else BldMods="${BldMods}moduls/bd/$1 "; fi
	    AS_IF([test $enable_$1 != dist],[$5])
	    $6
	])
])

AC_DEFUN([AX_MOD_DAQ_EN],
[
    AC_ARG_ENABLE([$1],AC_HELP_STRING([--$3-$1],[$2]),[ ],[
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
	    AC_MSG_RESULT(Build module: DAQ.$1)
	    AC_CONFIG_FILES(src/moduls/daq/$1/Makefile)
	    DAQSub_mod="${DAQSub_mod}$1 "
	    if test $enable_$1 = incl; then
		LIB_CORE="${LIB_CORE} moduls/daq/$1/daq_$1.la "
		ModsIncl="${ModsIncl}daq_$1 "
		BldInclMods="${BldInclMods}moduls/daq/$1 "
	    else BldMods="${BldMods}moduls/daq/$1 "; fi
	    AS_IF([test $enable_$1 != dist],[$5])
	    $6
	])
])

AC_DEFUN([AX_MOD_Archive_EN],
[
    AC_ARG_ENABLE([$1],AC_HELP_STRING([--$3-$1],[$2]),[ ],[
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
	    AC_MSG_RESULT(Build module: Archive.$1)
	    AC_CONFIG_FILES(src/moduls/arhiv/$1/Makefile)
	    ArchSub_mod="${ArchSub_mod}$1 "
	    if test $enable_$1 = incl; then
		LIB_CORE="${LIB_CORE} moduls/arhiv/$1/arh_$1.la "
		ModsIncl="${ModsIncl}arh_$1 "
		BldInclMods="${BldInclMods}moduls/arhiv/$1 "
	    else BldMods="${BldMods}moduls/arhiv/$1 "; fi
	    AS_IF([test $enable_$1 != dist],[$5])
	    $6
	])
])

AC_DEFUN([AX_MOD_Transport_EN],
[
    AC_ARG_ENABLE([$1],AC_HELP_STRING([--$3-$1],[$2]),[ ],[
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
	    AC_MSG_RESULT(Build module: Transport.$1)
	    AC_CONFIG_FILES(src/moduls/transport/$1/Makefile)
	    TranspSub_mod="${TranspSub_mod}$1 "
	    if test $enable_$1 = incl; then
		LIB_CORE="${LIB_CORE} moduls/transport/$1/tr_$1.la "
		ModsIncl="${ModsIncl}tr_$1 "
		BldInclMods="${BldInclMods}moduls/transport/$1 "
	    else BldMods="${BldMods}moduls/transport/$1 "; fi
	    AS_IF([test $enable_$1 != dist],[$5])
	    $6
	])
])

AC_DEFUN([AX_MOD_TrProt_EN],
[
    AC_ARG_ENABLE([$1],AC_HELP_STRING([--$3-$1],[$2]),[ ],[
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
	    AC_MSG_RESULT(Build module: Protocol.$1)
	    AC_CONFIG_FILES(src/moduls/protocol/$1/Makefile)
	    ProtSub_mod="${ProtSub_mod}$1 "
	    if test $enable_$1 = incl; then
		LIB_CORE="${LIB_CORE} moduls/protocol/$1/prot_$1.la "
		ModsIncl="${ModsIncl}prot_$1 "
		BldInclMods="${BldInclMods}moduls/protocol/$1 "
	    else BldMods="${BldMods}moduls/protocol/$1 "; fi
	    AS_IF([test $enable_$1 != dist],[$5])
	    $6
	])
])

AC_DEFUN([AX_MOD_UI_EN],
[
    AC_ARG_ENABLE([$1],AC_HELP_STRING([--$3-$1],[$2]),[ ],[
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
	    AC_MSG_RESULT(Build module: UI.$1)
	    AC_CONFIG_FILES(src/moduls/ui/$1/Makefile)
	    UISub_mod="${UISub_mod}$1 "
	    if test $enable_$1 = incl; then
		LIB_CORE="${LIB_CORE} moduls/ui/$1/ui_$1.la "
		ModsIncl="${ModsIncl}ui_$1 "
		BldInclMods="${BldInclMods}moduls/ui/$1 "
	    else BldMods="${BldMods}moduls/ui/$1 "; fi
	    AS_IF([test $enable_$1 != dist],[$5])
	    $6
	])
])

AC_DEFUN([AX_MOD_Special_EN],
[
    AC_ARG_ENABLE([$1],AC_HELP_STRING([--$3-$1],[$2]),[ ],[
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
	    AC_MSG_RESULT(Build module: Special.$1)
	    AC_CONFIG_FILES(src/moduls/special/$1/Makefile)
	    SpecSub_mod="${SpecSub_mod}$1 "
	    if test $enable_$1 = incl; then
		LIB_CORE="${LIB_CORE} moduls/special/$1/spec_$1.la "
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
