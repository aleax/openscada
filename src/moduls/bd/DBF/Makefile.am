EXTRA_DIST = *.h po/*

oscd_modul_LTLIBRARIES = bd_DBF.la
bd_DBF_la_LDFLAGS = -module -avoid-version -no-undefined
bd_DBF_la_SOURCES = dbf_mod.cpp dbf.cpp
bd_DBF_la_LIBTOOLFLAGS = --tag=disable-static

if DBFIncl
bd_DBF_la_CXXFLAGS = -DMOD_INCL
install:
endif

I18N_mod = $(oscd_modulpref)DBF
