## process this file with automake to generate Makefile.in
SUBDIRS = po

EXTRA_DIST = *.h doc/generic.sxw

AM_CXXFLAGS= -I$(top_srcdir)/src -fPIC -DPIC
AM_LDFLAGS= -shared -module

oscd_modul_PROGRAMS = bd_sqlite.so 
bd_sqlite_so_SOURCES = bd_sqlite.cpp
bd_sqlite_so_LDADD = -lsqlite3
