EXTRA_DIST = *.h po/*

if DCONIncl
noinst_LTLIBRARIES = daq_DCON.la
daq_DAQGate_la_CXXFLAGS = -DMOD_INCL -fpic
daq_DCON_la_LIBTOOLFLAGS = --tag=disable-shared
daq_DCON_la_LDFLAGS = -module
else
oscd_modul_LTLIBRARIES = daq_DCON.la
daq_DCON_la_LIBTOOLFLAGS = --tag=disable-static
daq_DCON_la_LDFLAGS = -module -avoid-version $(top_builddir)/src/liboscada.la
endif

daq_DCON_la_SOURCES = DCON_client.cpp

I18N_mod = $(oscd_modulpref)DCON
include ../../../../I18N.mk
