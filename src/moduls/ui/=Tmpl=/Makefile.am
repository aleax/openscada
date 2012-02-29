EXTRA_DIST = *.h po/*

#!!! The module result library name
oscd_modul_LTLIBRARIES = ui_Tmpl.la
#!!! The module compiler flags
ui_Tmpl_la_CXXFLAGS = $(tmpl_CFLAGS)
#!!! The module link flags
ui_Tmpl_la_LDFLAGS = -module -avoid-version -no-undefined $(tmpl_LDLAGS)
#!!! The module source files list
ui_Tmpl_la_SOURCES = mod_tmpl.cpp
#!!! Libtool util's flags. It is typical!
ui_Tmpl_la_LIBTOOLFLAGS = --tag=disable-static

#!!! Include module to OpenSCADA core process. It is typical!
if TmplIncl
ui_Tmpl_la_CXXFLAGS += -DMOD_INCL
install:
endif

#!!! The module name for translation files
I18N_mod = $(oscd_modulpref)Tmpl
