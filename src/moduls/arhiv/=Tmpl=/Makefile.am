EXTRA_DIST = *.h po/*

INCLUDES = -I$(top_srcdir)/src

oscd_modul_LTLIBRARIES = tr_Tmpl.la
tr_Tmpl_la_LDFLAGS = -module -avoid-version -no-undefined
tr_Tmpl_la_SOURCES = mod_tmpl.cpp
tr_Tmpl_la_LIBTOOLFLAGS = --tag=disable-static

I18N_mod = $(oscd_modulpref)Tmpl
