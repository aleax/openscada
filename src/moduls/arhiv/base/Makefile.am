## process this file with automake to generate Makefile.in
SUBDIRS = po

EXTRA_DIST = *.h doc/generic.sxw

AM_CXXFLAGS= -I$(top_srcdir)/src -fPIC -DPIC
AM_LDFLAGS= -module -shared

oscd_modul_PROGRAMS = arh_base.so 
arh_base_so_SOURCES = base.cpp


