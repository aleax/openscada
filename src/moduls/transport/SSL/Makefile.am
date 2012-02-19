EXTRA_DIST = *.h po/*

INCLUDES = -I$(top_srcdir)/src

if SSLIncl
noinst_LTLIBRARIES = tr_SSL.la
INCLUDES += -DMOD_INCL
else
oscd_modul_LTLIBRARIES = tr_SSL.la
endif
tr_SSL_la_LDFLAGS = -module -avoid-version -no-undefined
tr_SSL_la_SOURCES = modssl.cpp
tr_SSL_la_LIBADD  = $(LIB_OpenSSL)
tr_SSL_la_LIBTOOLFLAGS = --tag=disable-static

I18N_mod = $(oscd_modulpref)SSL
