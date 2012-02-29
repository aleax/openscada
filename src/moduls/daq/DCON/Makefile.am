EXTRA_DIST = *.h po/*

oscd_modul_LTLIBRARIES = daq_DCON.la
daq_DCON_la_LDFLAGS = -module -avoid-version -no-undefined
daq_DCON_la_SOURCES = DCON_client.cpp
daq_DCON_la_LIBTOOLFLAGS = --tag=disable-static

if DCONIncl
daq_DAQGate_la_CXXFLAGS = -DMOD_INCL
install:
endif

I18N_mod = $(oscd_modulpref)DCON
