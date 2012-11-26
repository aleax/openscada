EXTRA_DIST = *.h 
# po/*

oscd_modul_LTLIBRARIES = daq_OneWire.la
daq_OneWire_la_CXXFLAGS = $(OneWire_CFLAGS)
daq_OneWire_la_LDFLAGS = -module -avoid-version -no-undefined $(OneWire_LDLAGS)
daq_OneWire_la_SOURCES = module.cpp
daq_OneWire_la_LIBTOOLFLAGS = --tag=disable-static

if OneWireIncl
daq_OneWire_la_CXXFLAGS += -DMOD_INCL
install:
endif

I18N_mod = $(oscd_modulpref)OneWire
