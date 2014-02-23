EXTRA_DIST = *.h
# po/*

INCLUDES = -Iepics/

oscd_modul_LTLIBRARIES = daq_EPICS.la
daq_EPICS_la_SOURCES = module.cpp
daq_EPICS_la_CXXFLAGS = $(EPICS_CFLAGS)
daq_EPICS_la_LDFLAGS = -module -avoid-version -no-undefined $(EPICS_LDLAGS) -lCom -lca
daq_EPICS_la_LIBTOOLFLAGS = --tag=disable-static

I18N_mod = $(oscd_modulpref)EPICS
include ../../../../I18N.mk
