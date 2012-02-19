## process this file with automake to generate Makefile.in

EXTRA_DIST = *.h po/*

INCLUDES = -I$(top_srcdir)/src -DUSE_OLD_FUNCTIONS $(MYSQL_CFLAGS)

if MySQLIncl
noinst_LTLIBRARIES = bd_MySQL.la
INCLUDES += -DMOD_INCL
else
oscd_modul_LTLIBRARIES = bd_MySQL.la
endif
bd_MySQL_la_LDFLAGS = -module -avoid-version -no-undefined $(MYSQL_LDFLAGS)
bd_MySQL_la_SOURCES = my_sql.cpp
bd_MySQL_la_LIBTOOLFLAGS = --tag=disable-static

I18N_mod = $(oscd_modulpref)MySQL
