EXTRA_DIST = *.h po/*

oscd_modul_LTLIBRARIES = bd_FireBird.la
bd_FireBird_la_CXXFLAGS = $(FIREBIRD_CFLAGS)
bd_FireBird_la_LDFLAGS = -module -avoid-version -no-undefined $(FIREBIRD_LDFLAGS)
bd_FireBird_la_SOURCES = firebird.cpp
bd_FireBird_la_LIBTOOLFLAGS = --tag=disable-static

if FireBirdIncl
bd_FireBird_la_CXXFLAGS += -DMOD_INCL
install:
endif

I18N_mod = $(oscd_modulpref)FireBird
