## process this file with automake to generate Makefile.in

EXTRA_DIST = *.h

AM_CXXFLAGS= -fPIC -DPIC -I../../..
AM_LDFLAGS= -shared -module -avoid-version

oscd_modul_PROGRAMS = ui_web_dbg.so
ui_web_dbg_so_SOURCES = web_dbg.cpp

