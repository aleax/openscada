EXTRA_DIST = *.h po/*

oscd_modul_LTLIBRARIES = arh_FSArch.la
arh_FSArch_la_LDFLAGS = -module -avoid-version -no-undefined
arh_FSArch_la_SOURCES = base.cpp mess.cpp val.cpp
arh_FSArch_la_LIBTOOLFLAGS = --tag=disable-static

if FSArchIncl
arh_FSArch_la_CXXFLAGS = -DMOD_INCL
install:
endif

I18N_mod = $(oscd_modulpref)FSArch
