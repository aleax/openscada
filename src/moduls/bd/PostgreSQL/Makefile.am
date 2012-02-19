EXTRA_DIST = *.h po/*

INCLUDES = -I$(top_srcdir)/src $(POSTGRESQL_CFLAGS)

if PostgreSQLIncl
noinst_LTLIBRARIES = bd_PostgreSQL.la
INCLUDES += -DMOD_INCL
else
oscd_modul_LTLIBRARIES = bd_PostgreSQL.la
endif
bd_PostgreSQL_la_LDFLAGS = -module -avoid-version -no-undefined $(POSTGRESQL_LDFLAGS)
bd_PostgreSQL_la_SOURCES = postgre.cpp
bd_PostgreSQL_la_LIBTOOLFLAGS = --tag=disable-static

I18N_mod = $(oscd_modulpref)PostgreSQL
