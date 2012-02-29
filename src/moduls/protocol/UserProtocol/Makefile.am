EXTRA_DIST = *.h po/*

oscd_modul_LTLIBRARIES = prot_UserProtocol.la
prot_UserProtocol_la_LDFLAGS = -module -avoid-version -no-undefined
prot_UserProtocol_la_SOURCES = user_prt.cpp
prot_UserProtocol_la_LIBTOOLFLAGS = --tag=disable-static

if UserProtocolIncl
prot_UserProtocol_la_CXXFLAGS = -DMOD_INCL
install:
endif

I18N_mod = $(oscd_modulpref)UserProtocol
