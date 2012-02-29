EXTRA_DIST = *.h po/*

oscd_modul_LTLIBRARIES = spec_FLibComplex1.la
spec_FLibComplex1_la_LDFLAGS = -module -avoid-version -no-undefined
spec_FLibComplex1_la_SOURCES = statfunc.cpp
spec_FLibComplex1_la_LIBTOOLFLAGS = --tag=disable-static

if FLibComplex1Incl
spec_FLibComplex1_la_CXXFLAGS = -DMOD_INCL
install:
endif

I18N_mod = $(oscd_modulpref)FLibComplex1
