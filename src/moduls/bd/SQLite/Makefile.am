EXTRA_DIST = *.h po/*

oscd_modul_LTLIBRARIES = bd_SQLite.la
bd_SQLite_la_CXXFLAGS = $(sqlite3_CFLAGS)
bd_SQLite_la_LDFLAGS = -module -avoid-version -no-undefined $(sqlite3_LIBS)
bd_SQLite_la_SOURCES = bd_sqlite.cpp
bd_SQLite_la_LIBTOOLFLAGS = --tag=disable-static

if SQLiteIncl
bd_SQLite_la_CXXFLAGS += -DMOD_INCL
install:
endif

I18N_mod = $(oscd_modulpref)SQLite
