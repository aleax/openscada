QTDIR=/usr/lib/qt4/
QTINC=$(QTDIR)include/

#SUBDIRS = po

EXTRA_DIST = *.h vision.qrc images/*

AM_CXXFLAGS= -I$(QTINC) -I$(QTINC)/QtCore -I$(QTINC)/QtGui -I$(top_srcdir)/src -fPIC -DPIC \
    -DQT_EDITION=QT_EDITION_DESKTOP -DQT_USE_APPROXIMATE_CURSORS -D_LARGEFILE64_SOURCE -D_LARGEFILE_SOURCE -DQT_GUI_LIB -DQT_CORE_LIB
AM_LDFLAGS= -shared -module

oscd_modul_PROGRAMS = ui_Vision.so
ui_Vision_so_SOURCES = tvision.cpp qrc_vision.cpp vis_widgs.cpp vis_shapes.cpp\
     moc_vis_widgs.cpp vis_devel.cpp moc_vis_devel.cpp vis_devel_widgs.cpp moc_vis_devel_widgs.cpp
ui_Vision_so_LDADD = -lQtGui

qrc_vision.cpp: vision.qrc
	$(QTDIR)/bin/rcc -name Vision vision.qrc -o qrc_vision.cpp
moc_vis_devel.cpp: vis_devel.h
	$(QTDIR)/bin/moc vis_devel.h > moc_vis_devel.cpp
moc_vis_devel_widgs.cpp: vis_devel_widgs.h
	$(QTDIR)/bin/moc vis_devel_widgs.h > moc_vis_devel_widgs.cpp
moc_vis_widgs.cpp: vis_widgs.h
	$(QTDIR)/bin/moc vis_widgs.h > moc_vis_widgs.cpp

clean-local:
	rm -f qrc_vision.cpp moc_vis_widgs.cpp moc_vis_devel.cpp moc_vis_devel_widgs.cpp
