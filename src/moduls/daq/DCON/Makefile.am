EXTRA_DIST = *.h po/*

INCLUDES = -I$(top_srcdir)/src

if DCONIncl
noinst_LTLIBRARIES = daq_DCON.la
INCLUDES += -DMOD_INCL
else
oscd_modul_LTLIBRARIES = daq_DCON.la
endif
daq_DCON_la_LDFLAGS = -module -avoid-version -no-undefined
daq_DCON_la_SOURCES = DCON_client.cpp
daq_DCON_la_LIBTOOLFLAGS = --tag=disable-static

I18N_mod = $(oscd_modulpref)DCON
