## process this file with automake to generate Makefile.in
SUBDIRS = po

EXTRA_DIST = *.h qtcfg.moc

AM_CXXFLAGS= -I$(QTDIR)/include -I$(top_srcdir)/src  -fPIC -DPIC
AM_LDFLAGS= -shared -module

oscd_modul_PROGRAMS = ui_qtcfg.so
ui_qtcfg_so_SOURCES = tuimod.cpp qtcfg.cpp
ui_qtcfg_so_LDADD = -L$(QTDIR)/lib -lqt-mt

