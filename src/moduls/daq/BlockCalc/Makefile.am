SUBDIRS = po

EXTRA_DIST = *.h

AM_CXXFLAGS= -I$(top_srcdir)/src -fPIC -DPIC
AM_LDFLAGS= -shared -module

oscd_modul_PROGRAMS = daq_BlockCalc.so
daq_BlockCalc_so_SOURCES = virtual.cpp block.cpp

