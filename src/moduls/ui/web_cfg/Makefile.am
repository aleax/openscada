## process this file with automake to generate Makefile.in
SUBDIRS = po

EXTRA_DIST = *.h doc/generic.sxw

AM_CXXFLAGS= -I$(top_srcdir)/src -fPIC -DPIC
AM_LDFLAGS= -shared -module

oscd_modul_PROGRAMS = ui_web_cfg.so
ui_web_cfg_so_SOURCES = web_cfg.cpp

