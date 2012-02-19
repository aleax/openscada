EXTRA_DIST = *.h po/*

INCLUDES = -I$(top_srcdir)/src

oscd_modul_LTLIBRARIES = daq_BFN.la
daq_BFN_la_LDFLAGS = -module -avoid-version -no-undefined
daq_BFN_la_SOURCES = mod_BFN.cpp
daq_BFN_la_LIBTOOLFLAGS = --tag=disable-static

I18N_mod = $(oscd_modulpref)BFN
