EXTRA_DIST = *.h po/*

oscd_modul_LTLIBRARIES = bd_MySQL.la
bd_MySQL_la_CXXFLAGS = -DUSE_OLD_FUNCTIONS $(MYSQL_CFLAGS)
bd_MySQL_la_LDFLAGS = -module -avoid-version -no-undefined $(MYSQL_LDFLAGS)
bd_MySQL_la_SOURCES = my_sql.cpp
bd_MySQL_la_LIBTOOLFLAGS = --tag=disable-static

if MySQLIncl
bd_MySQL_la_CXXFLAGS += -DMOD_INCL
install:
endif

I18N_mod = $(oscd_modulpref)MySQL
