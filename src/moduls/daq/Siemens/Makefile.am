EXTRA_DIST = *.h po/*

oscd_modul_LTLIBRARIES = daq_Siemens.la
daq_Siemens_la_CFLAGS = -DLINUX -DDAVE_LITTLE_ENDIAN
daq_Siemens_la_CXXFLAGS = $(daq_Siemens_la_CFLAGS)
daq_Siemens_la_LDFLAGS = -module -avoid-version $(top_builddir)/src/liboscada.la
daq_Siemens_la_SOURCES = siemens.cpp cif_api.c nodave.c openSocket.c setport.c
daq_Siemens_la_LIBTOOLFLAGS = --tag=disable-static

I18N_mod = $(oscd_modulpref)Siemens
include ../../../../I18N.mk
