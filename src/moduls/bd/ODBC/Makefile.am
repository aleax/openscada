EXTRA_DIST = *.h 
#po/*

if ODBCIncl
noinst_LTLIBRARIES = bd_ODBC.la
bd_ODBC_la_CXXFLAGS = -DMOD_INCL -fpic
bd_ODBC_la_LIBTOOLFLAGS = --tag=disable-shared
bd_ODBC_la_LDFLAGS = -module
else
oscd_modul_LTLIBRARIES = bd_ODBC.la
bd_ODBC_la_CXXFLAGS =
bd_ODBC_la_LIBTOOLFLAGS = --tag=disable-static
bd_ODBC_la_LDFLAGS = -module -avoid-version $(top_builddir)/src/liboscada.la
endif

bd_ODBC_la_CXXFLAGS += $(IODBC_CFLAGS)
bd_ODBC_la_LDFLAGS += $(IODBC_LDFLAGS)
bd_ODBC_la_SOURCES = odbc.cpp


I18N_mod = $(oscd_modulpref)ODBC
include ../../../../I18N.mk
