EXTRA_DIST = *.h po/*

if LogicLevIncl
noinst_LTLIBRARIES = daq_LogicLev.la
daq_LogicLev_la_CXXFLAGS = -DMOD_INCL -fpic
daq_LogicLev_la_LIBTOOLFLAGS = --tag=disable-shared
else
oscd_modul_LTLIBRARIES = daq_LogicLev.la
daq_LogicLev_la_LIBTOOLFLAGS = --tag=disable-static
endif

daq_LogicLev_la_LDFLAGS = -module -avoid-version -no-undefined
daq_LogicLev_la_SOURCES = logiclev.cpp

I18N_mod = $(oscd_modulpref)LogicLev
include ../../../../I18N.mk
