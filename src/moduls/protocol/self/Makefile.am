## process this file with automake to generate Makefile.in

EXTRA_DIST = *.h

AM_CXXFLAGS= -I../../.. -fPIC -DPIC
AM_LDFLAGS= -shared -module -avoid-version

oscd_modul_PROGRAMS = prot_self.so
prot_self_so_SOURCES = self.cpp

