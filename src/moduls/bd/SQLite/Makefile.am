EXTRA_DIST = *.h po/*

INCLUDES = -I$(top_srcdir)/src $(sqlite3_CFLAGS)

if SQLiteIncl
noinst_LTLIBRARIES = bd_SQLite.la
INCLUDES += -DMOD_INCL
else
oscd_modul_LTLIBRARIES = bd_SQLite.la
endif
bd_SQLite_la_LDFLAGS = -module -avoid-version -no-undefined $(sqlite3_LIBS)
bd_SQLite_la_SOURCES = bd_sqlite.cpp
bd_SQLite_la_LIBTOOLFLAGS = --tag=disable-static

I18N_mod = $(oscd_modulpref)SQLite
