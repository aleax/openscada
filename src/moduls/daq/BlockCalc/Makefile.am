EXTRA_DIST = *.h po/*

if BlockCalcIncl
noinst_LTLIBRARIES = daq_BlockCalc.la
daq_BlockCalc_la_CXXFLAGS = -DMOD_INCL -fpic
daq_BlockCalc_la_LIBTOOLFLAGS = --tag=disable-shared
else
oscd_modul_LTLIBRARIES = daq_BlockCalc.la
daq_BlockCalc_la_LIBTOOLFLAGS = --tag=disable-static
endif

daq_BlockCalc_la_LDFLAGS = -module -avoid-version -no-undefined
daq_BlockCalc_la_SOURCES = virtual.cpp block.cpp


I18N_mod = $(oscd_modulpref)BlockCalc
include ../../../../I18N.mk
