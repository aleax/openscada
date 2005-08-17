## process this file with automake to generate Makefile.in
SUBDIRS = po

EXTRA_DIST = *.h doc/generic.sxw

AM_CXXFLAGS= -I$(top_srcdir)/src -fPIC -DPIC
AM_LDFLAGS= -shared -module

oscd_modul_PROGRAMS = bd_SQLite.so 
bd_SQLite_so_SOURCES = bd_sqlite.cpp
bd_SQLite_so_LDADD = -lsqlite3
