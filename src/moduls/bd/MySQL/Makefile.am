EXTRA_DIST = *.h po/*

if MySQLIncl
noinst_LTLIBRARIES = bd_MySQL.la
bd_MySQL_la_CXXFLAGS = -DMOD_INCL -fpic
bd_MySQL_la_LIBTOOLFLAGS = --tag=disable-shared
bd_MySQL_la_LDFLAGS = -module
else
oscd_modul_LTLIBRARIES = bd_MySQL.la
bd_MySQL_la_CXXFLAGS =
bd_MySQL_la_LIBTOOLFLAGS = --tag=disable-static
bd_MySQL_la_LDFLAGS = -module -avoid-version $(top_builddir)/src/liboscada.la
endif

bd_MySQL_la_CXXFLAGS += $(MYSQL_CFLAGS)
bd_MySQL_la_LDFLAGS += $(MYSQL_LDFLAGS)
bd_MySQL_la_SOURCES = my_sql.cpp

I18N_mod = $(oscd_modulpref)MySQL
include ../../../../I18N.mk
