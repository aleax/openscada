EXTRA_DIST = *.h po/*

if FLibComplex1Incl
noinst_LTLIBRARIES = spec_FLibComplex1.la
spec_FLibComplex1_la_CXXFLAGS = -DMOD_INCL -fpic
spec_FLibComplex1_la_LIBTOOLFLAGS = --tag=disable-shared
else
oscd_modul_LTLIBRARIES = spec_FLibComplex1.la
spec_FLibComplex1_la_LIBTOOLFLAGS = --tag=disable-static
endif

spec_FLibComplex1_la_LDFLAGS = -module -avoid-version -no-undefined
spec_FLibComplex1_la_SOURCES = statfunc.cpp


I18N_mod = $(oscd_modulpref)FLibComplex1
include ../../../../I18N.mk
