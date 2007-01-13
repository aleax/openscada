#SUBDIRS = po

EXTRA_DIST = *.h xpm/*

AM_CXXFLAGS= -I$(top_srcdir)/src -fPIC -DPIC 
AM_LDFLAGS= -shared -module

oscd_modul_PROGRAMS = ui_VCAEngine.so
ui_VCAEngine_so_SOURCES = vcaengine.cpp libwidg.cpp widget.cpp origwidg.cpp