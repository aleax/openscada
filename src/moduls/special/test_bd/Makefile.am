## process this file with automake to generate Makefile.in

EXTRA_DIST = *.h

AM_CXXFLAGS= -I../../.. -fPIC -DPIC
AM_LDFLAGS= -shared -module -avoid-version

oscd_modul_PROGRAMS = spec_test_bd.so
spec_test_bd_so_SOURCES = test_bd.cpp

