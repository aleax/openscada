EXTRA_DIST = *.h po/*

oscd_modul_LTLIBRARIES = daq_MMS.la
daq_MMS_la_SOURCES = module.cpp
daq_MMS_la_CXXFLAGS = $(MMS_CFLAGS)
daq_MMS_la_LDFLAGS = -module -avoid-version $(top_builddir)/src/liboscada.la $(MMS_LDLAGS) libMMS/libMMS.la
daq_MMS_la_LIBTOOLFLAGS = --tag=disable-static

I18N_mod = $(oscd_modulpref)MMS
include ../../../../I18N.mk

SUBDIRS = libMMS
