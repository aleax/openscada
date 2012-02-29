EXTRA_DIST = *.h po/*

oscd_modul_LTLIBRARIES = tr_SSL.la
tr_SSL_la_LDFLAGS = -module -avoid-version -no-undefined
tr_SSL_la_SOURCES = modssl.cpp
tr_SSL_la_LIBADD  = $(LIB_OpenSSL)
tr_SSL_la_LIBTOOLFLAGS = --tag=disable-static

if SSLIncl
tr_SSL_la_CXXFLAGS = -DMOD_INCL
install:
endif

I18N_mod = $(oscd_modulpref)SSL
