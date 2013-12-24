EXTRA_DIST = *.h po/*

oscd_modul_LTLIBRARIES = daq_OPC_UA.la
daq_OPC_UA_la_LDFLAGS = -module -avoid-version -no-undefined
daq_OPC_UA_la_SOURCES = moduls.cpp mod_daq.cpp mod_prt.cpp
daq_OPC_UA_la_LIBADD  = $(LIB_OpenSSL) -LlibOPC_UA/ -lOPC_UA
daq_OPC_UA_la_LIBTOOLFLAGS = --tag=disable-static

I18N_mod = $(oscd_modulpref)OPC_UA
include ../../../../I18N.mk

SUBDIRS = libOPC_UA
