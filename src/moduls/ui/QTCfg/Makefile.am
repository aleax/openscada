## process this file with automake to generate Makefile.in
SUBDIRS = po

EXTRA_DIST = *.h xpm/*

AM_CXXFLAGS= -I$(QTDIR)/include -I$(top_srcdir)/src  -fPIC -DPIC
AM_LDFLAGS= -shared -module

oscd_modul_PROGRAMS = ui_QTCfg.so
ui_QTCfg_so_SOURCES = tuimod.cpp qtcfg.cpp dlguser.cpp moc_qtcfg.cpp
ui_QTCfg_so_LDADD = -L$(QTDIR)/lib -lqt-mt

moc_qtcfg.cpp:
	$(QTDIR)/bin/moc qtcfg.h > moc_qtcfg.cpp

clean-generic:
	rm -f moc_qtcfg.cpp
