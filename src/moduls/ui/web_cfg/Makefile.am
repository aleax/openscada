## process this file with automake to generate Makefile.in
SUBDIRS = po

EXTRA_DIST = *.h

AM_CXXFLAGS= -fPIC -DPIC -I../../..
AM_LDFLAGS= -shared -module -avoid-version

oscd_modul_PROGRAMS = ui_web_cfg.so
ui_web_cfg_so_SOURCES = web_cfg.cpp

