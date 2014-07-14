EXTRA_DIST = *.h po/*

if FLibMathIncl
noinst_LTLIBRARIES = spec_FLibMath.la
spec_FLibMath_la_CXXFLAGS = -DMOD_INCL -fpic
spec_FLibMath_la_LIBTOOLFLAGS = --tag=disable-shared
spec_FLibMath_la_LDFLAGS = -module
else
oscd_modul_LTLIBRARIES = spec_FLibMath.la
spec_FLibMath_la_LIBTOOLFLAGS = --tag=disable-static
spec_FLibMath_la_LDFLAGS = -module -avoid-version $(top_builddir)/src/liboscada.la
endif

spec_FLibMath_la_SOURCES = statfunc.cpp


I18N_mod = $(oscd_modulpref)FLibMath
include ../../../../I18N.mk
