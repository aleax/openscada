SUBDIRS = po

EXTRA_DIST = *.h libdscud5.a

AM_CXXFLAGS= -I$(top_srcdir)/src -fPIC -DPIC
AM_LDFLAGS= -shared -module

oscd_modul_PROGRAMS = daq_DiamondBoards.so
daq_DiamondBoards_so_SOURCES = diamond.cpp
daq_DiamondBoards_so_LDADD = -L. -ldscud5

