EXTRA_DIST = *.h po/*

if DAQGateIncl
noinst_LTLIBRARIES = daq_DAQGate.la
daq_DAQGate_la_CXXFLAGS = -DMOD_INCL -fpic
daq_DAQGate_la_LIBTOOLFLAGS = --tag=disable-shared
else
oscd_modul_LTLIBRARIES = daq_DAQGate.la
daq_DAQGate_la_LIBTOOLFLAGS = --tag=disable-static
endif

daq_DAQGate_la_LDFLAGS = -module -avoid-version -no-undefined
daq_DAQGate_la_SOURCES = daq_gate.cpp

I18N_mod = $(oscd_modulpref)DAQGate
include ../../../../I18N.mk
