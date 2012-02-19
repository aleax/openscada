EXTRA_DIST = *.h po/*

INCLUDES = -I$(top_srcdir)/src

if UserProtocolIncl
noinst_LTLIBRARIES = prot_UserProtocol.la
INCLUDES += -DMOD_INCL
else
oscd_modul_LTLIBRARIES = prot_UserProtocol.la
endif
prot_UserProtocol_la_LDFLAGS = -module -avoid-version -no-undefined
prot_UserProtocol_la_SOURCES = user_prt.cpp
prot_UserProtocol_la_LIBTOOLFLAGS = --tag=disable-static

I18N_mod = $(oscd_modulpref)UserProtocol
