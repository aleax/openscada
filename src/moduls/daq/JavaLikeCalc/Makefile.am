EXTRA_DIST = *.h po/*

oscd_modul_LTLIBRARIES = daq_JavaLikeCalc.la
daq_JavaLikeCalc_la_LDFLAGS = -module -avoid-version -no-undefined
daq_JavaLikeCalc_la_SOURCES = virtual.cpp freelib.cpp freefunc.cpp func_analise.ypp
daq_JavaLikeCalc_la_LIBTOOLFLAGS = --tag=disable-static

if JavaLikeCalcIncl
daq_JavaLikeCalc_la_CXXFLAGS = -DMOD_INCL
install:
endif

I18N_mod = $(oscd_modulpref)JavaLikeCalc

clean-local:
	rm -f *func_analise.cpp
