## process this file with automake to generate Makefile.in
SUBDIRS = po

EXTRA_DIST = *.h

AM_CXXFLAGS= -I$(top_srcdir)/src -fPIC -DPIC
AM_LDFLAGS= -module -shared

oscd_modul_PROGRAMS = arh_BaseArh.so 
arh_BaseArh_so_SOURCES = base.cpp mess.cpp val.cpp


