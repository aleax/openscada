SUBDIRS = po

EXTRA_DIST = *.h libdscud5.a

AM_CXXFLAGS= -I$(top_srcdir)/src -fPIC -DPIC
AM_LDFLAGS= -shared -module

oscd_modul_PROGRAMS = cntr_diamond.so
cntr_diamond_so_SOURCES = diamond.cpp
cntr_diamond_so_LDADD = -L. -ldscud5
