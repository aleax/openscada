EXTRA_DIST = *.h po/*

if FT3Incl
noinst_LTLIBRARIES = daq_FT3.la
daq_FT3_la_CXXFLAGS = -DMOD_INCL -fpic
daq_FT3_la_LIBTOOLFLAGS = --tag=disable-shared
daq_FT3_la_LDFLAGS = -module
else
oscd_modul_LTLIBRARIES = daq_FT3.la
daq_FT3_la_LIBTOOLFLAGS = --tag=disable-static
daq_FT3_la_LDFLAGS = -module -avoid-version $(top_builddir)/src/liboscada.la
endif

daq_FT3_la_SOURCES = da.cpp mod_FT3.cpp FT3_prt.cpp BUC.cpp BVI.cpp BVTC.cpp BVT.cpp BIP.cpp PAUK.cpp ACCOUNT.cpp BTR.cpp BTE.cpp ODOR.cpp GKR.cpp GZD.cpp GNS.cpp TANK.cpp

I18N_mod = $(oscd_modulpref)FT3
include ../../../../I18N.mk
