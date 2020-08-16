EXTRA_DIST = *.h po/*

if DBGateIncl
noinst_LTLIBRARIES = bd_DBGate.la
bd_DBGate_la_CXXFLAGS = -DMOD_INCL -fpic
bd_DBGate_la_LIBTOOLFLAGS = --tag=disable-shared
bd_DBGate_la_LDFLAGS = -module
else
oscd_modul_LTLIBRARIES = bd_DBGate.la
bd_DBGate_la_CXXFLAGS =
bd_DBGate_la_LIBTOOLFLAGS = --tag=disable-static
bd_DBGate_la_LDFLAGS = -module -avoid-version $(top_builddir)/src/liboscada.la
endif

bd_DBGate_la_SOURCES = DBGate.cpp

I18N_mod = $(oscd_modulpref)DBGate
include ../../../../I18N.mk
