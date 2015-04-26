EXTRA_DIST = *.h fbus/libfbus.so fbus/*.h fbus/stdlib/*.h
# po/*

oscd_modul_LTLIBRARIES = daq_Fastwel.la
daq_Fastwel_la_SOURCES = module.cpp
daq_Fastwel_la_CXXFLAGS = -Ifbus
daq_Fastwel_la_LDFLAGS = -module -avoid-version $(top_builddir)/src/liboscada.la
daq_Fastwel_la_LIBADD = -Lfbus -lfbus
daq_Fastwel_la_LIBTOOLFLAGS = --tag=disable-static

I18N_mod = $(oscd_modulpref)Fastwel
include ../../../../I18N.mk
