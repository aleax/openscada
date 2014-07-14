EXTRA_DIST = *.h po/*

if FSArchIncl
noinst_LTLIBRARIES = arh_FSArch.la
arh_FSArch_la_CXXFLAGS = -DMOD_INCL -fpic
arh_FSArch_la_LIBTOOLFLAGS = --tag=disable-shared
arh_FSArch_la_LDFLAGS = -module
else
oscd_modul_LTLIBRARIES = arh_FSArch.la
arh_FSArch_la_LIBTOOLFLAGS = --tag=disable-static
arh_FSArch_la_LDFLAGS = -module -avoid-version $(top_builddir)/src/liboscada.la
endif

arh_FSArch_la_SOURCES = base.cpp mess.cpp val.cpp

I18N_mod = $(oscd_modulpref)FSArch
include ../../../../I18N.mk
