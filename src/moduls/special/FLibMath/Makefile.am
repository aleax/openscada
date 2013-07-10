EXTRA_DIST = *.h po/*

if FLibMathIncl
noinst_LTLIBRARIES = spec_FLibMath.la
spec_FLibMath_la_CXXFLAGS = -DMOD_INCL -fpic
spec_FLibMath_la_LIBTOOLFLAGS = --tag=disable-shared
else
oscd_modul_LTLIBRARIES = spec_FLibMath.la
spec_FLibMath_la_LIBTOOLFLAGS = --tag=disable-static
endif

spec_FLibMath_la_LDFLAGS = -module -avoid-version -no-undefined
spec_FLibMath_la_SOURCES = statfunc.cpp


I18N_mod = $(oscd_modulpref)FLibMath
include ../../../../I18N.mk
