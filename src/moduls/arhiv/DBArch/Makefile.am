EXTRA_DIST = *.h po/*

INCLUDES = -I$(top_srcdir)/src

if DBArchIncl
noinst_LTLIBRARIES = arh_DBArch.la
INCLUDES += -DMOD_INCL
else
oscd_modul_LTLIBRARIES = arh_DBArch.la
endif
arh_DBArch_la_LDFLAGS = -module -avoid-version -no-undefined
arh_DBArch_la_SOURCES = arch.cpp mess.cpp val.cpp
arh_DBArch_la_LIBTOOLFLAGS = --tag=disable-static

I18N_mod = $(oscd_modulpref)DBArch
