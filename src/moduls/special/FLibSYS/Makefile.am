EXTRA_DIST = *.h po/*

INCLUDES = -I$(top_srcdir)/src

if FLibSYSIncl
noinst_LTLIBRARIES = spec_FLibSYS.la
INCLUDES += -DMOD_INCL
else
oscd_modul_LTLIBRARIES = spec_FLibSYS.la
endif
spec_FLibSYS_la_LDFLAGS = -module -avoid-version -no-undefined
spec_FLibSYS_la_SOURCES = statfunc.cpp xmlfnc.cpp varchfnc.cpp
spec_FLibSYS_la_LIBADD = $(LIB_FFTW3)
spec_FLibSYS_la_LIBTOOLFLAGS = --tag=disable-static

I18N_mod = $(oscd_modulpref)FLibSYS
