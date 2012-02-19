EXTRA_DIST = *.h po/*

INCLUDES = -I$(top_srcdir)/src

#!!! The module result library name
oscd_modul_LTLIBRARIES = daq_Tmpl.la
#!!! The module link flags
daq_Tmpl_la_LDFLAGS = -module -avoid-version -no-undefined
#!!! The module source files list
daq_Tmpl_la_SOURCES = mod_tmpl.cpp
#!!! The module depending libraryes list
#daq_Tmpl_la_LIBADD  = -lnetsnmp -lcrypto
daq_Tmpl_la_LIBTOOLFLAGS = --tag=disable-static

#!!! The module name for translation files
I18N_mod = $(oscd_modulpref)Tmpl
