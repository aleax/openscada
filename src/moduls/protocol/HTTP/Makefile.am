EXTRA_DIST = *.h po/*

if HTTPIncl
noinst_LTLIBRARIES = prot_HTTP.la
prot_HTTP_la_CXXFLAGS = -DMOD_INCL -fpic
prot_HTTP_la_LIBTOOLFLAGS = --tag=disable-shared
prot_HTTP_la_LDFLAGS = -module
else
oscd_modul_LTLIBRARIES = prot_HTTP.la
prot_HTTP_la_LIBTOOLFLAGS = --tag=disable-static
prot_HTTP_la_LDFLAGS = -module -avoid-version $(top_builddir)/src/liboscada.la
endif

prot_HTTP_la_SOURCES = http.cpp

I18N_mod = $(oscd_modulpref)HTTP
include ../../../../I18N.mk
