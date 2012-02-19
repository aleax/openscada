EXTRA_DIST = *.h po/*

INCLUDES = -I$(top_srcdir)/src

if FLibMathIncl
noinst_LTLIBRARIES = spec_FLibMath.la
INCLUDES += -DMOD_INCL
else
oscd_modul_LTLIBRARIES = spec_FLibMath.la
endif
spec_FLibMath_la_LDFLAGS = -module -avoid-version -no-undefined
spec_FLibMath_la_SOURCES = statfunc.cpp
spec_FLibMath_la_LIBTOOLFLAGS = --tag=disable-static

I18N_mod = $(oscd_modulpref)FLibMath
