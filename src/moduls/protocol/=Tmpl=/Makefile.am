EXTRA_DIST = *.h po/*

INCLUDES = -I$(top_srcdir)/src

#!!! The module result library name
oscd_modul_LTLIBRARIES = prot_Tmpl.la
#!!! The module link flags
prot_Tmpl_la_LDFLAGS = -module -avoid-version -no-undefined
#!!! The module source files list
prot_Tmpl_la_SOURCES = mod_tmpl.cpp
#!!! The module depending libraryes list
#prot_Tmpl_la_LIBADD  = -lnetsnmp -lcrypto
prot_Tmpl_la_LIBTOOLFLAGS = --tag=disable-static

#!!! The module name for translation files
I18N_mod = $(oscd_modulpref)Tmpl
