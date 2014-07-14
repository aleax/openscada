EXTRA_DIST = *.h po/*

#!!! Include module to OpenSCADA core process. It is typical!
if TmplIncl
#!!! The module result library name
noinst_LTLIBRARIES = arh_Tmpl.la
arh_Tmpl_la_CXXFLAGS = -DMOD_INCL -fpic
#!!! Libtool util's flags. It is typical
arh_Tmpl_la_LIBTOOLFLAGS = --tag=disable-shared
#!!! The module link flags
arh_Tmpl_la_LDFLAGS = -module
else
#!!! The module result library name
oscd_modul_LTLIBRARIES = arh_Tmpl.la
arh_Tmpl_la_CXXFLAGS =
#!!! Libtool util's flags. It is typical
arh_Tmpl_la_LIBTOOLFLAGS = --tag=disable-static
#!!! The module link flags
arh_Tmpl_la_LDFLAGS = -module -avoid-version $(top_builddir)/src/liboscada.la
endif

#!!! The module compiler flags
arh_Tmpl_la_CXXFLAGS += $(tmpl_CFLAGS)
#!!! The module link flags
arh_Tmpl_la_LDFLAGS += $(tmpl_LDLAGS)
#!!! The module source files list
arh_Tmpl_la_SOURCES = module.cpp mess.cpp val.cpp

#!!! The module name for translation files
I18N_mod = $(oscd_modulpref)Tmpl
include ../../../../I18N.mk
