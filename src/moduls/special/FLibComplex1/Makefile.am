EXTRA_DIST = *.h po/*

INCLUDES = -I$(top_srcdir)/src

if FLibComplex1Incl
noinst_LTLIBRARIES = spec_FLibComplex1.la
INCLUDES += -DMOD_INCL
else
oscd_modul_LTLIBRARIES = spec_FLibComplex1.la
endif
spec_FLibComplex1_la_LDFLAGS = -module -avoid-version -no-undefined
spec_FLibComplex1_la_SOURCES = statfunc.cpp
spec_FLibComplex1_la_LIBTOOLFLAGS = --tag=disable-static

I18N_mod = $(oscd_modulpref)FLibComplex1
