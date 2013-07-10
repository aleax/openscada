EXTRA_DIST = *.h po/*

#!!! Include module to OpenSCADA core process. It is typical!
if TmplIncl
#!!! The module result library name
noinst_LTLIBRARIES = tr_Tmpl.la
tr_Tmpl_la_CXXFLAGS = -DMOD_INCL -fpic
#!!! Libtool util's flags. It is typical!
tr_Tmpl_la_LIBTOOLFLAGS = --tag=disable-shared
else
#!!! The module result library name
oscd_modul_LTLIBRARIES = tr_Tmpl.la
tr_Tmpl_la_CXXFLAGS =
#!!! Libtool util's flags. It is typical!
tr_Tmpl_la_LIBTOOLFLAGS = --tag=disable-static
endif

#!!! The module compiler flags
tr_Tmpl_la_CXXFLAGS += $(tmpl_CFLAGS)
#!!! The module link flags
tr_Tmpl_la_LDFLAGS = -module -avoid-version -no-undefined $(tmpl_LDLAGS)
#!!! The module source files list
tr_Tmpl_la_SOURCES = module.cpp


#!!! The module name for translation files
I18N_mod = $(oscd_modulpref)Tmpl
include ../../../../I18N.mk
