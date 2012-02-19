EXTRA_DIST = *.h po/*

INCLUDES = -I$(top_srcdir)/src

if BlockCalcIncl
noinst_LTLIBRARIES = daq_BlockCalc.la
INCLUDES += -DMOD_INCL
else
oscd_modul_LTLIBRARIES = daq_BlockCalc.la
endif
daq_BlockCalc_la_LDFLAGS = -module -avoid-version -no-undefined
daq_BlockCalc_la_SOURCES = virtual.cpp block.cpp
daq_BlockCalc_la_LIBTOOLFLAGS = --tag=disable-static

I18N_mod = $(oscd_modulpref)BlockCalc
