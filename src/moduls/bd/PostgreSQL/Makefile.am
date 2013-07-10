EXTRA_DIST = *.h po/*

if PostgreSQLIncl
noinst_LTLIBRARIES = bd_PostgreSQL.la
bd_PostgreSQL_la_CXXFLAGS = -DMOD_INCL -fpic
else
oscd_modul_LTLIBRARIES = bd_PostgreSQL.la
bd_PostgreSQL_la_CXXFLAGS =
bd_PostgreSQL_la_LIBTOOLFLAGS = --tag=disable-static
endif

bd_PostgreSQL_la_CXXFLAGS += $(POSTGRESQL_CFLAGS)
bd_PostgreSQL_la_LDFLAGS = -module -avoid-version -no-undefined $(POSTGRESQL_LDFLAGS)
bd_PostgreSQL_la_SOURCES = postgre.cpp

I18N_mod = $(oscd_modulpref)PostgreSQL
include ../../../../I18N.mk
