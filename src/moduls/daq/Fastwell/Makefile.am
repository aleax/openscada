EXTRA_DIST = *.h fbus/libfbus.so fbus/*.h fbus/stdlib/*.h
# po/*

oscd_modul_LTLIBRARIES = daq_Fastwell.la
daq_Fastwell_la_SOURCES = module.cpp
daq_Fastwell_la_CXXFLAGS = -Ifbus
daq_Fastwell_la_LDFLAGS = -module -avoid-version $(top_builddir)/src/liboscada.la
daq_Fastwell_la_LIBADD = -Lfbus -lfbus
daq_Fastwell_la_LIBTOOLFLAGS = --tag=disable-static

I18N_mod = $(oscd_modulpref)Fastwell
include ../../../../I18N.mk
