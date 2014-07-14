EXTRA_DIST = *.h po/*

if FLibSYSIncl
noinst_LTLIBRARIES = spec_FLibSYS.la
spec_FLibSYS_la_CXXFLAGS = -DMOD_INCL -fpic
spec_FLibSYS_la_LIBTOOLFLAGS = --tag=disable-shared
spec_FLibSYS_la_LDFLAGS = -module
else
oscd_modul_LTLIBRARIES = spec_FLibSYS.la
spec_FLibSYS_la_LIBTOOLFLAGS = --tag=disable-static
spec_FLibSYS_la_LDFLAGS = -module -avoid-version $(top_builddir)/src/liboscada.la
endif

spec_FLibSYS_la_SOURCES = statfunc.cpp xmlfnc.cpp varchfnc.cpp io.cpp gd.cpp
spec_FLibSYS_la_LIBADD = $(LIB_FFTW3)


I18N_mod = $(oscd_modulpref)FLibSYS
include ../../../../I18N.mk
