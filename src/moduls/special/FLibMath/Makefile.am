EXTRA_DIST = *.h po/*

oscd_modul_LTLIBRARIES = spec_FLibMath.la
spec_FLibMath_la_LDFLAGS = -module -avoid-version -no-undefined
spec_FLibMath_la_SOURCES = statfunc.cpp
spec_FLibMath_la_LIBTOOLFLAGS = --tag=disable-static

if FLibMathIncl
spec_FLibMath_la_CXXFLAGS = -DMOD_INCL
install:
endif

I18N_mod = $(oscd_modulpref)FLibMath
