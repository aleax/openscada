EXTRA_DIST = *.h po/*

INCLUDES = -I$(top_srcdir)/src

if DAQGateIncl
noinst_LTLIBRARIES = daq_DAQGate.la
INCLUDES += -DMOD_INCL
else
oscd_modul_LTLIBRARIES = daq_DAQGate.la
endif
daq_DAQGate_la_LDFLAGS = -module -avoid-version -no-undefined
daq_DAQGate_la_SOURCES = daq_gate.cpp
daq_DAQGate_la_LIBTOOLFLAGS = --tag=disable-static

I18N_mod = $(oscd_modulpref)DAQGate
