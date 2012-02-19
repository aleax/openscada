EXTRA_DIST = *.h po/*

INCLUDES = -I$(top_srcdir)/src

if HTTPIncl
noinst_LTLIBRARIES = prot_HTTP.la
INCLUDES += -DMOD_INCL
else
oscd_modul_LTLIBRARIES = prot_HTTP.la
endif
prot_HTTP_la_LDFLAGS = -module -avoid-version -no-undefined
prot_HTTP_la_SOURCES = http.cpp
prot_HTTP_la_LIBTOOLFLAGS = --tag=disable-static

I18N_mod = $(oscd_modulpref)HTTP
