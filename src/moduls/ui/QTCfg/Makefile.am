## process this file with automake to generate Makefile.in
SUBDIRS = po

EXTRA_DIST = *.h xpm/*

AM_CXXFLAGS= -I$(QTDIR)/include -I$(top_srcdir)/src  -fPIC -DPIC
AM_LDFLAGS= -shared -module

oscd_modul_PROGRAMS = ui_QTCfg.so
ui_QTCfg_so_SOURCES = tuimod.cpp qtcfg.cpp dlguser.cpp imgview.cpp moc_qtcfg.cpp moc_imgview.cpp
ui_QTCfg_so_LDADD = -L$(QTDIR)/lib -L$(QTDIR)/lib64 -lqt-mt

moc_qtcfg.cpp: qtcfg.h
	$(QTDIR)/bin/moc qtcfg.h > moc_qtcfg.cpp

moc_imgview.cpp: imgview.h
	$(QTDIR)/bin/moc imgview.h > moc_imgview.cpp

clean-generic:
	rm -f moc_qtcfg.cpp moc_imgview.cpp
