## process this file with automake to generate Makefile.in
#SUBDIRS = po

EXTRA_DIST = *.h xpm/*

AM_CXXFLAGS= -I$(QTDIR)/include -I$(top_srcdir)/src -fPIC -DPIC
AM_LDFLAGS= -shared -module

oscd_modul_PROGRAMS = ui_Vision.so
ui_Vision_so_SOURCES = tvision.cpp tvisiondev.cpp moc_tvisiondev.cpp tframedev.cpp moc_tframedev.cpp tframeview.cpp moc_tframeview.cpp titem.cpp moc_titem.cpp titemsupervisor.cpp moc_titemsupervisor.cpp tconfiguration.cpp tvisionrun.cpp moc_tvisionrun.cpp tdbgw.cpp tvisionerror.cpp tdynamicpropdialog.cpp moc_tdynamicpropdialog.cpp
ui_Vision_so_LDADD = -L$(QTDIR)/lib -L$(QTDIR)/lib64 -lqt-mt

moc_tvisiondev.cpp: tvisiondev.h
	$(QTDIR)/bin/moc tvisiondev.h > moc_tvisiondev.cpp
moc_tvisionrun.cpp: tvisionrun.h
	$(QTDIR)/bin/moc tvisionrun.h > moc_tvisionrun.cpp
moc_tframedev.cpp: tframedev.h
	$(QTDIR)/bin/moc tframedev.h > moc_tframedev.cpp
moc_tframeview.cpp: tframeview.h
	$(QTDIR)/bin/moc tframeview.h > moc_tframeview.cpp
moc_titem.cpp: titem.h
	$(QTDIR)/bin/moc titem.h > moc_titem.cpp
moc_titemsupervisor.cpp: titemsupervisor.h
	$(QTDIR)/bin/moc titemsupervisor.h > moc_titemsupervisor.cpp
moc_tdynamicpropdialog.cpp: tdynamicpropdialog.h
	$(QTDIR)/bin/moc tdynamicpropdialog.h > moc_tdynamicpropdialog.cpp

clean-generic:
	rm -f moc_tvisiondev.cpp moc_tvisionrun.cpp moc_tframedev.cpp moc_tframeview.cpp moc_titem.cpp moc_titemsupervisor.cpp moc_tdynamicpropdialog.cpp
