EXTRA_DIST = *.h po/*

if SQLiteIncl
noinst_LTLIBRARIES = bd_SQLite.la
bd_SQLite_la_CXXFLAGS = -DMOD_INCL -fpic
bd_SQLite_la_LIBTOOLFLAGS = --tag=disable-shared
else
oscd_modul_LTLIBRARIES = bd_SQLite.la
bd_SQLite_la_CXXFLAGS =
bd_SQLite_la_LIBTOOLFLAGS = --tag=disable-static
endif

bd_SQLite_la_CXXFLAGS += $(sqlite3_CFLAGS)
bd_SQLite_la_LDFLAGS = -module -avoid-version -no-undefined $(sqlite3_LIBS)
bd_SQLite_la_SOURCES = bd_sqlite.cpp

I18N_mod = $(oscd_modulpref)SQLite
include ../../../../I18N.mk
