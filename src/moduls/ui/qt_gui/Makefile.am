## process this file with automake to generate Makefile.in

EXTRA_DIST = *.h qt_gui.moc

AM_CXXFLAGS= -I$(QTDIR)/include -I../../.. -fPIC -DPIC
AM_LDFLAGS= -shared -module -avoid-version

oscd_modul_PROGRAMS = ui_qt_gui.so
ui_qt_gui_so_SOURCES = qt_gui.cpp
ui_qt_gui_so_LDADD = -L$(QTDIR)/lib -lqt-mt

