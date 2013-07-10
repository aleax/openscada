EXTRA_DIST = *.h po/*

if DCONIncl
noinst_LTLIBRARIES = daq_DCON.la
daq_DAQGate_la_CXXFLAGS = -DMOD_INCL -fpic
daq_DCON_la_LIBTOOLFLAGS = --tag=disable-shared
else
oscd_modul_LTLIBRARIES = daq_DCON.la
daq_DCON_la_LIBTOOLFLAGS = --tag=disable-static
endif

daq_DCON_la_LDFLAGS = -module -avoid-version -no-undefined
daq_DCON_la_SOURCES = DCON_client.cpp

I18N_mod = $(oscd_modulpref)DCON
include ../../../../I18N.mk
