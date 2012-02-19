EXTRA_DIST = *.h po/*

INCLUDES = -I$(top_srcdir)/src

if LogicLevIncl
noinst_LTLIBRARIES = daq_LogicLev.la
INCLUDES += -DMOD_INCL
else
oscd_modul_LTLIBRARIES = daq_LogicLev.la
endif
daq_LogicLev_la_LDFLAGS = -module -avoid-version -no-undefined
daq_LogicLev_la_SOURCES = logiclev.cpp
daq_LogicLev_la_LIBTOOLFLAGS = --tag=disable-static

I18N_mod = $(oscd_modulpref)LogicLev
