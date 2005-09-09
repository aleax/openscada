SUBDIRS = po

EXTRA_DIST = *.h doc/generic.sxw doc/project.sxw

AM_CXXFLAGS= -I$(top_srcdir)/src -fPIC -DPIC
AM_LDFLAGS= -shared -module

oscd_modul_PROGRAMS = cntr_BlockCalc.so
cntr_BlockCalc_so_SOURCES = virtual.cpp block.cpp

