EXTRA_DIST = *.h po/*

oscd_modul_LTLIBRARIES = daq_BlockCalc.la
daq_BlockCalc_la_LDFLAGS = -module -avoid-version -no-undefined
daq_BlockCalc_la_SOURCES = virtual.cpp block.cpp
daq_BlockCalc_la_LIBTOOLFLAGS = --tag=disable-static

if BlockCalcIncl
daq_BlockCalc_la_CXXFLAGS = -DMOD_INCL
install:
endif

I18N_mod = $(oscd_modulpref)BlockCalc
