EXTRA_DIST = *.h po/*

if DBFIncl
noinst_LTLIBRARIES = bd_DBF.la
bd_DBF_la_CXXFLAGS = -DMOD_INCL -fpic
bd_DBF_la_LIBTOOLFLAGS = --tag=disable-shared
bd_DBF_la_LDFLAGS = -module
else
oscd_modul_LTLIBRARIES = bd_DBF.la
bd_DBF_la_LIBTOOLFLAGS = --tag=disable-static
bd_DBF_la_LDFLAGS = -module -avoid-version $(top_builddir)/src/liboscada.la
endif

bd_DBF_la_SOURCES = dbf_mod.cpp dbf.cpp

I18N_mod = $(oscd_modulpref)DBF
include ../../../../I18N.mk
