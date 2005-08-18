## process this file with automake to generate Makefile.in

EXTRA_DIST = *.h

AM_CXXFLAGS= -I$(top_srcdir)/src -fPIC -DPIC
AM_LDFLAGS= -shared -module

oscd_modul_PROGRAMS = ui_WebDbg.so
ui_WebDbg_so_SOURCES = web_dbg.cpp

