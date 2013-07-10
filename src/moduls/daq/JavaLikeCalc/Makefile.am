EXTRA_DIST = *.h po/*

if JavaLikeCalcIncl
noinst_LTLIBRARIES = daq_JavaLikeCalc.la
daq_JavaLikeCalc_la_CXXFLAGS = -DMOD_INCL -fpic
daq_JavaLikeCalc_la_LIBTOOLFLAGS = --tag=disable-shared
else
oscd_modul_LTLIBRARIES = daq_JavaLikeCalc.la
daq_JavaLikeCalc_la_LIBTOOLFLAGS = --tag=disable-static
endif

daq_JavaLikeCalc_la_LDFLAGS = -module -avoid-version -no-undefined
daq_JavaLikeCalc_la_SOURCES = virtual.cpp freelib.cpp freefunc.cpp func_analise.ypp


I18N_mod = $(oscd_modulpref)JavaLikeCalc
I18N_addfiles = *.ypp
include ../../../../I18N.mk

clean-local:
	rm -f *func_analise.cpp

DISTFILES = Makefile.am Makefile.in $(DIST_SOURCES) $(EXTRA_DIST)
