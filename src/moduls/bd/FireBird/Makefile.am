EXTRA_DIST = *.h po/*

INCLUDES = -I$(top_srcdir)/src $(FIREBIRD_CFLAGS)

if FireBirdIncl
noinst_LTLIBRARIES = bd_FireBird.la
INCLUDES += -DMOD_INCL
else
oscd_modul_LTLIBRARIES = bd_FireBird.la
endif
bd_FireBird_la_LDFLAGS = -module -avoid-version -no-undefined $(FIREBIRD_LDFLAGS)
bd_FireBird_la_SOURCES = firebird.cpp
bd_FireBird_la_LIBTOOLFLAGS = --tag=disable-static

I18N_mod = $(oscd_modulpref)FireBird
