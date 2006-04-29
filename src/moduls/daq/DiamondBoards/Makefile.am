SUBDIRS = po

EXTRA_DIST = *.h 

AM_CXXFLAGS= -I$(top_srcdir)/src -fPIC -DPIC
AM_LDFLAGS= -shared -module

oscd_modul_PROGRAMS = daq_DiamondBoards.so
daq_DiamondBoards_so_SOURCES = diamond.cpp

