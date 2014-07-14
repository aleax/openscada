EXTRA_DIST = *.h po/*

if LogicLevIncl
noinst_LTLIBRARIES = daq_LogicLev.la
daq_LogicLev_la_CXXFLAGS = -DMOD_INCL -fpic
daq_LogicLev_la_LIBTOOLFLAGS = --tag=disable-shared
daq_LogicLev_la_LDFLAGS = -module
else
oscd_modul_LTLIBRARIES = daq_LogicLev.la
daq_LogicLev_la_LIBTOOLFLAGS = --tag=disable-static
daq_LogicLev_la_LDFLAGS = -module -avoid-version $(top_builddir)/src/liboscada.la
endif

daq_LogicLev_la_SOURCES = logiclev.cpp

I18N_mod = $(oscd_modulpref)LogicLev
include ../../../../I18N.mk
