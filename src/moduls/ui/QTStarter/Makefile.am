## process this file with automake to generate Makefile.in
#SUBDIRS = po

EXTRA_DIST = *.h

AM_CXXFLAGS= -I$(QTDIR)/include -I$(top_srcdir)/src  -fPIC -DPIC
AM_LDFLAGS= -shared -module

oscd_modul_PROGRAMS = ui_QTStarter.so
ui_QTStarter_so_SOURCES = tuimod.cpp
ui_QTStarter_so_LDADD = -L$(QTDIR)/lib -lqt-mt
