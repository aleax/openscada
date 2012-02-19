EXTRA_DIST = *.h po/*

INCLUDES = -I$(top_srcdir)/src

if DBFIncl
noinst_LTLIBRARIES = bd_DBF.la
INCLUDES += -DMOD_INCL
else
oscd_modul_LTLIBRARIES = bd_DBF.la
endif
bd_DBF_la_LDFLAGS = -module -avoid-version -no-undefined
bd_DBF_la_SOURCES = dbf_mod.cpp dbf.cpp
bd_DBF_la_LIBTOOLFLAGS = --tag=disable-static

I18N_mod = $(oscd_modulpref)DBF
