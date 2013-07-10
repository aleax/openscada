EXTRA_DIST = *.h po/*

oscd_modul_LTLIBRARIES = daq_Comedi.la
daq_Comedi_la_CXXFLAGS = $(comedilib_CFLAGS)
daq_Comedi_la_LDFLAGS = -module -avoid-version -no-undefined $(comedilib_LIBS)
daq_Comedi_la_SOURCES = module.cpp
daq_Comedi_la_LIBTOOLFLAGS = --tag=disable-static

I18N_mod = $(oscd_modulpref)Comedi
include ../../../../I18N.mk
