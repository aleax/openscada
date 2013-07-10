EXTRA_DIST = *.h
# po/*

if OneWireIncl
noinst_LTLIBRARIES = daq_OneWire.la
daq_OneWire_la_CXXFLAGS = -DMOD_INCL -fpic
daq_OneWire_la_LIBTOOLFLAGS = --tag=disable-shared
else
oscd_modul_LTLIBRARIES = daq_OneWire.la
daq_OneWire_la_CXXFLAGS =
daq_OneWire_la_LIBTOOLFLAGS = --tag=disable-static
endif

daq_OneWire_la_CXXFLAGS += $(OneWire_CFLAGS)
daq_OneWire_la_LDFLAGS = -module -avoid-version -no-undefined $(OneWire_LDLAGS)
daq_OneWire_la_SOURCES = module.cpp

I18N_mod = $(oscd_modulpref)OneWire
include ../../../../I18N.mk
