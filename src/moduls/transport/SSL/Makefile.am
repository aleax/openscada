EXTRA_DIST = *.h po/*

if SSLIncl
noinst_LTLIBRARIES = tr_SSL.la
tr_SSL_la_CXXFLAGS = -DMOD_INCL -fpic
tr_SSL_la_LIBTOOLFLAGS = --tag=disable-shared
else
oscd_modul_LTLIBRARIES = tr_SSL.la
tr_SSL_la_LIBTOOLFLAGS = --tag=disable-static
endif

tr_SSL_la_LDFLAGS = -module -avoid-version -no-undefined
tr_SSL_la_SOURCES = modssl.cpp
tr_SSL_la_LIBADD  = $(LIB_OpenSSL)

I18N_mod = $(oscd_modulpref)SSL
include ../../../../I18N.mk
