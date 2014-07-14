EXTRA_DIST = *.h po/*

if UserProtocolIncl
noinst_LTLIBRARIES = prot_UserProtocol.la
prot_UserProtocol_la_CXXFLAGS = -DMOD_INCL -fpic
prot_UserProtocol_la_LIBTOOLFLAGS = --tag=disable-shared
prot_UserProtocol_la_LDFLAGS = -module
else
oscd_modul_LTLIBRARIES = prot_UserProtocol.la
prot_UserProtocol_la_LIBTOOLFLAGS = --tag=disable-static
prot_UserProtocol_la_LDFLAGS = -module -avoid-version $(top_builddir)/src/liboscada.la
endif

prot_UserProtocol_la_SOURCES = user_prt.cpp

I18N_mod = $(oscd_modulpref)UserProtocol
include ../../../../I18N.mk
