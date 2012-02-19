EXTRA_DIST = *.h po/*

INCLUDES = -I$(top_srcdir)/src

if JavaLikeCalcIncl
noinst_LTLIBRARIES = daq_JavaLikeCalc.la
INCLUDES += -DMOD_INCL
else
oscd_modul_LTLIBRARIES = daq_JavaLikeCalc.la
endif
daq_JavaLikeCalc_la_LDFLAGS = -module -avoid-version -no-undefined
daq_JavaLikeCalc_la_SOURCES = virtual.cpp freelib.cpp freefunc.cpp func_analise.ypp
daq_JavaLikeCalc_la_LIBTOOLFLAGS = --tag=disable-static

I18N_mod = $(oscd_modulpref)JavaLikeCalc

clean-local:
	rm -f *func_analise.cpp
