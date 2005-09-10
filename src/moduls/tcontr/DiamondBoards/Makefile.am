SUBDIRS = po

EXTRA_DIST = *.h libdscud5.a doc/generic.sxw

AM_CXXFLAGS= -I$(top_srcdir)/src -fPIC -DPIC
AM_LDFLAGS= -shared -module

oscd_modul_PROGRAMS = cntr_DiamondBoards.so
cntr_DiamondBoards_so_SOURCES = diamond.cpp
cntr_DiamondBoards_so_LDADD = -L. -ldscud5
