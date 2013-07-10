EXTRA_DIST = *.h po/*

if FireBirdIncl
noinst_LTLIBRARIES = bd_FireBird.la
bd_FireBird_la_CXXFLAGS = -DMOD_INCL -fpic
bd_FireBird_la_LIBTOOLFLAGS = --tag=disable-shared
else
oscd_modul_LTLIBRARIES = bd_FireBird.la
bd_FireBird_la_CXXFLAGS =
bd_FireBird_la_LIBTOOLFLAGS = --tag=disable-static
endif

bd_FireBird_la_CXXFLAGS += $(FIREBIRD_CFLAGS)
bd_FireBird_la_LDFLAGS = -module -avoid-version -no-undefined $(FIREBIRD_LDFLAGS)
bd_FireBird_la_SOURCES = firebird.cpp

I18N_mod = $(oscd_modulpref)FireBird
include ../../../../I18N.mk
