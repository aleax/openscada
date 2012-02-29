EXTRA_DIST = *.h po/*

oscd_modul_LTLIBRARIES = prot_HTTP.la
prot_HTTP_la_LDFLAGS = -module -avoid-version -no-undefined
prot_HTTP_la_SOURCES = http.cpp
prot_HTTP_la_LIBTOOLFLAGS = --tag=disable-static

if HTTPIncl
prot_HTTP_la_CXXFLAGS = -DMOD_INCL
install:
endif

I18N_mod = $(oscd_modulpref)HTTP
