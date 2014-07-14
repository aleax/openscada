EXTRA_DIST = *.h po/*

if SSLIncl
noinst_LTLIBRARIES = tr_SSL.la
tr_SSL_la_CXXFLAGS = -DMOD_INCL -fpic
tr_SSL_la_LIBTOOLFLAGS = --tag=disable-shared
tr_SSL_la_LDFLAGS = -module
else
oscd_modul_LTLIBRARIES = tr_SSL.la
tr_SSL_la_LIBTOOLFLAGS = --tag=disable-static
tr_SSL_la_LDFLAGS = -module -avoid-version $(top_builddir)/src/liboscada.la
endif

tr_SSL_la_SOURCES = modssl.cpp
tr_SSL_la_LIBADD  = $(LIB_OpenSSL)

I18N_mod = $(oscd_modulpref)SSL
include ../../../../I18N.mk
