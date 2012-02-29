EXTRA_DIST = *.h po/*

oscd_modul_LTLIBRARIES = daq_LogicLev.la
daq_LogicLev_la_LDFLAGS = -module -avoid-version -no-undefined
daq_LogicLev_la_SOURCES = logiclev.cpp
daq_LogicLev_la_LIBTOOLFLAGS = --tag=disable-static

if LogicLevIncl
daq_LogicLev_la_CXXFLAGS = -DMOD_INCL
install:
endif

I18N_mod = $(oscd_modulpref)LogicLev
