EXTRA_DIST = *.h libdscud5.a po/*

INCLUDES = -I$(top_srcdir)/src

oscd_modul_LTLIBRARIES = daq_DiamondBoards.la
daq_DiamondBoards_la_LDFLAGS = -module -avoid-version -no-undefined
daq_DiamondBoards_la_SOURCES = diamond.cpp
daq_DiamondBoards_la_LIBADD  = -L./ -ldscud5
daq_DiamondBoards_la_LIBTOOLFLAGS = --tag=disable-static

I18N_mod = $(oscd_modulpref)DiamondBoards
