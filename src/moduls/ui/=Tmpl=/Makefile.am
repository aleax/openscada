EXTRA_DIST = *.h po/*

INCLUDES = -I$(top_srcdir)/src

oscd_modul_LTLIBRARIES = ui_Tmpl.la
ui_Tmpl_la_LDFLAGS = -module -avoid-version -no-undefined
ui_Tmpl_la_SOURCES = mod_tmpl.cpp
ui_Tmpl_la_LIBTOOLFLAGS = --tag=disable-static

I18N_mod = $(oscd_modulpref)Tmpl
