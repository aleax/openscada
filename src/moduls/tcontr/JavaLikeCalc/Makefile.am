## process this file with automake to generate Makefile.in
SUBDIRS = po

EXTRA_DIST = *.h

AM_CXXFLAGS= -I$(top_srcdir)/src -fPIC -DPIC
AM_LDFLAGS= -shared -module 

oscd_modul_PROGRAMS = cntr_JavaLikeCalc.so
cntr_JavaLikeCalc_so_SOURCES = virtual.cpp freelib.cpp freefunc.cpp func_analise.ypp

clean-generic:
	rm -f func_analise.cpp
