## process this file with automake to generate Makefile.in
SUBDIRS = po

EXTRA_DIST = *.h xpm/*

AM_CXXFLAGS= -I$(QTDIR)/include -I$(top_srcdir)/src  -fPIC -DPIC
AM_LDFLAGS= -shared -module

oscd_modul_PROGRAMS = ui_QTStarter.so
ui_QTStarter_so_SOURCES = tuimod.cpp moc_tuimod.cpp
ui_QTStarter_so_LDADD = -L$(QTDIR)/lib -L$(QTDIR)/lib64 -lqt-mt

moc_tuimod.cpp: tuimod.h
	$(QTDIR)/bin/moc tuimod.h > moc_tuimod.cpp

clean-generic:
	rm -f moc_tuimod.cpp
