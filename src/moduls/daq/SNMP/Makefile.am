## process this file with automake to generate Makefile.in

EXTRA_DIST = *.h po/*

INCLUDES = -I$(top_srcdir)/src

oscd_modul_LTLIBRARIES = daq_SNMP.la
daq_SNMP_la_LDFLAGS = -module -avoid-version -no-undefined
daq_SNMP_la_SOURCES = snmp_client.cpp
daq_SNMP_la_LIBADD  = -lnetsnmp -lcrypto
daq_SNMP_la_LIBTOOLFLAGS = --tag=disable-static

I18N_mod = $(oscd_modulpref)SNMP
