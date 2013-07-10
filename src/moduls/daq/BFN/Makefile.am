EXTRA_DIST = *.h po/* data/*

oscd_modul_LTLIBRARIES = daq_BFN.la
daq_BFN_la_LDFLAGS = -module -avoid-version -no-undefined
daq_BFN_la_SOURCES = mod_BFN.cpp
daq_BFN_la_LIBTOOLFLAGS = --tag=disable-static

BFNdatadir = $(oscd_datadir)/BFN
BFNdata_DATA = data/*

I18N_mod = $(oscd_modulpref)BFN
include ../../../../I18N.mk
