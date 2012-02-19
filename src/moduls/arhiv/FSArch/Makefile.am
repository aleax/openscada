EXTRA_DIST = *.h po/*

INCLUDES = -I$(top_srcdir)/src

if FSArchIncl
noinst_LTLIBRARIES = arh_FSArch.la
INCLUDES += -DMOD_INCL
else
oscd_modul_LTLIBRARIES = arh_FSArch.la
endif
arh_FSArch_la_LDFLAGS = -module -avoid-version -no-undefined
arh_FSArch_la_SOURCES = base.cpp mess.cpp val.cpp
arh_FSArch_la_LIBTOOLFLAGS = --tag=disable-static

I18N_mod = $(oscd_modulpref)FSArch
