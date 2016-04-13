EXTRA_DIST = *.h 
# po/*

oscd_modul_LTLIBRARIES = daq_BCM2835.la
daq_BCM2835_la_CXXFLAGS =
daq_BCM2835_la_LIBTOOLFLAGS = --tag=disable-static
daq_BCM2835_la_LDFLAGS = -module -avoid-version $(top_builddir)/src/liboscada.la

daq_BCM2835_la_CXXFLAGS += $(BCM2835_CFLAGS)
daq_BCM2835_la_LDFLAGS += $(BCM2835_LDLAGS)
daq_BCM2835_la_SOURCES = module.cpp bcm2835.c


I18N_mod = $(oscd_modulpref)BCM2835
include ../../../../I18N.mk
