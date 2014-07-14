EXTRA_DIST = *.h po/*

if DBArchIncl
noinst_LTLIBRARIES = arh_DBArch.la
arh_DBArch_la_CXXFLAGS = -DMOD_INCL -fpic
arh_DBArch_la_LIBTOOLFLAGS = --tag=disable-shared
arh_DBArch_la_LDFLAGS = -module
else
oscd_modul_LTLIBRARIES = arh_DBArch.la
arh_DBArch_la_LIBTOOLFLAGS = --tag=disable-static
arh_DBArch_la_LDFLAGS = -module -avoid-version $(top_builddir)/src/liboscada.la
endif

arh_DBArch_la_SOURCES = arch.cpp mess.cpp val.cpp

I18N_mod = $(oscd_modulpref)DBArch
include ../../../../I18N.mk
