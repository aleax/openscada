EXTRA_DIST = *.h po/*

oscd_modul_LTLIBRARIES = spec_FLibSYS.la
spec_FLibSYS_la_LDFLAGS = -module -avoid-version -no-undefined
spec_FLibSYS_la_SOURCES = statfunc.cpp xmlfnc.cpp varchfnc.cpp
spec_FLibSYS_la_LIBADD = $(LIB_FFTW3)
spec_FLibSYS_la_LIBTOOLFLAGS = --tag=disable-static

if FLibSYSIncl
spec_FLibSYS_la_CXXFLAGS = -DMOD_INCL
install:
endif

I18N_mod = $(oscd_modulpref)FLibSYS
