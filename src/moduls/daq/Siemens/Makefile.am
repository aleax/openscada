EXTRA_DIST = *.h po/*

INCLUDES = -DLINUX -DDAVE_LITTLE_ENDIAN

oscd_modul_LTLIBRARIES = daq_Siemens.la
daq_Siemens_la_LDFLAGS = -module -avoid-version -no-undefined
daq_Siemens_la_SOURCES = siemens.cpp cif_api.c nodave.c openSocket.c setport.c
daq_Siemens_la_LIBTOOLFLAGS = --tag=disable-static

I18N_mod = $(oscd_modulpref)Siemens
include ../../../../I18N.mk
