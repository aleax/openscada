EXTRA_DIST = *.h 
#po/*

if ODBCIncl
noinst_LTLIBRARIES = db_ODBC.la
db_ODBC_la_CXXFLAGS = -DMOD_INCL -fpic
db_ODBC_la_LIBTOOLFLAGS = --tag=disable-shared
db_ODBC_la_LDFLAGS = -module
else
oscd_modul_LTLIBRARIES = db_ODBC.la
db_ODBC_la_CXXFLAGS =
db_ODBC_la_LIBTOOLFLAGS = --tag=disable-static
db_ODBC_la_LDFLAGS = -module -avoid-version $(top_builddir)/src/liboscada.la
endif

db_ODBC_la_CXXFLAGS += $(IODBC_CFLAGS)
db_ODBC_la_LDFLAGS += $(IODBC_LDFLAGS)
db_ODBC_la_SOURCES = odbc.cpp


I18N_mod = $(oscd_modulpref)ODBC
include ../../../../I18N.mk
