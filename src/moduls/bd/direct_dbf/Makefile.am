## process this file with automake to generate Makefile.in

EXTRA_DIST = *.h

AM_CXXFLAGS= -I../../.. -fPIC -DPIC
AM_LDFLAGS= -shared -module -avoid-version

oscd_modul_PROGRAMS = bd_direct_dbf.so
bd_direct_dbf_so_SOURCES = direct_dbf.cpp dbf.cpp

