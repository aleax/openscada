EXTRA_DIST = *.h po/*

INCLUDES = -I$(top_srcdir)/src

oscd_modul_LTLIBRARIES = spec_SystemTests.la
spec_SystemTests_la_LDFLAGS = -module -avoid-version -no-undefined
spec_SystemTests_la_SOURCES = mod_tests.cpp
spec_SystemTests_la_LIBTOOLFLAGS = --tag=disable-static

I18N_mod = $(oscd_modulpref)SystemTests
