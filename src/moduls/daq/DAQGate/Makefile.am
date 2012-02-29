EXTRA_DIST = *.h po/*

oscd_modul_LTLIBRARIES = daq_DAQGate.la
daq_DAQGate_la_LDFLAGS = -module -avoid-version -no-undefined
daq_DAQGate_la_SOURCES = daq_gate.cpp
daq_DAQGate_la_LIBTOOLFLAGS = --tag=disable-static

if DAQGateIncl
daq_DAQGate_la_CXXFLAGS = -DMOD_INCL
install:
endif

I18N_mod = $(oscd_modulpref)DAQGate
