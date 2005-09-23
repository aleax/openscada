## process this file with automake to generate Makefile.in
SUBDIRS = po

EXTRA_DIST = *.h

AM_CXXFLAGS= -I$(top_srcdir)/src -fPIC -DPIC
AM_LDFLAGS= -shared -module 

oscd_modul_PROGRAMS = spec_FreeFuncLibs.so
spec_FreeFuncLibs_so_SOURCES = freefunclibs.cpp freelib.cpp freefunc.cpp func_analise.ypp

clean-generic:
	rm -f func_analise.cpp
