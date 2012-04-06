EXTRA_DIST = *.h po/*

oscd_modul_LTLIBRARIES = arh_DBArch.la
arh_DBArch_la_LDFLAGS = -module -avoid-version -no-undefined
arh_DBArch_la_SOURCES = arch.cpp mess.cpp val.cpp
arh_DBArch_la_LIBTOOLFLAGS = --tag=disable-static

if DBArchIncl
arh_DBArch_la_CXXFLAGS = -DMOD_INCL
install:
endif

I18N_mod = $(oscd_modulpref)DBArch
