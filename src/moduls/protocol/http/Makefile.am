## process this file with automake to generate Makefile.in
SUBDIRS = po

EXTRA_DIST = *.h doc/generic.sxw

AM_CXXFLAGS= -I$(top_srcdir)/src -fPIC -DPIC
AM_LDFLAGS= -shared -module

oscd_modul_PROGRAMS = prot_http.so
prot_http_so_SOURCES = http.cpp

