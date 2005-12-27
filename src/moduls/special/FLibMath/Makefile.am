## process this file with automake to generate Makefile.in
SUBDIRS = po

EXTRA_DIST = *.h

AM_CXXFLAGS= -I$(top_srcdir)/src -fPIC -DPIC
AM_LDFLAGS= -shared -module 

oscd_modul_PROGRAMS = spec_FLibMath.so
spec_FLibMath_so_SOURCES = statfunc.cpp

