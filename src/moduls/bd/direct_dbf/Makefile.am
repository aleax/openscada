## process this file with automake to generate Makefile.in

EXTRA_DIST = *.h

AM_CXXFLAGS= -I$(top_srcdir)/src -fPIC -DPIC
AM_LDFLAGS= -shared -module

oscd_modul_PROGRAMS = bd_direct_dbf.so
bd_direct_dbf_so_SOURCES = direct_dbf.cpp dbf.cpp

