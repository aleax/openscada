EXTRA_DIST = *.h po/*

if DAQGateIncl
noinst_LTLIBRARIES = daq_DAQGate.la
daq_DAQGate_la_CXXFLAGS = -DMOD_INCL -fpic
daq_DAQGate_la_LIBTOOLFLAGS = --tag=disable-shared
daq_DAQGate_la_LDFLAGS = -module
else
oscd_modul_LTLIBRARIES = daq_DAQGate.la
daq_DAQGate_la_LIBTOOLFLAGS = --tag=disable-static
daq_DAQGate_la_LDFLAGS = -module -avoid-version $(top_builddir)/src/liboscada.la
endif

daq_DAQGate_la_SOURCES = daq_gate.cpp

I18N_mod = $(oscd_modulpref)DAQGate
include ../../../../I18N.mk
