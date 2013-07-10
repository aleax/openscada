EXTRA_DIST = *.h po/*

if UserProtocolIncl
noinst_LTLIBRARIES = prot_UserProtocol.la
prot_UserProtocol_la_CXXFLAGS = -DMOD_INCL -fpic
prot_UserProtocol_la_LIBTOOLFLAGS = --tag=disable-shared
else
oscd_modul_LTLIBRARIES = prot_UserProtocol.la
prot_UserProtocol_la_LIBTOOLFLAGS = --tag=disable-static
endif

prot_UserProtocol_la_LDFLAGS = -module -avoid-version -no-undefined
prot_UserProtocol_la_SOURCES = user_prt.cpp

I18N_mod = $(oscd_modulpref)UserProtocol
include ../../../../I18N.mk
