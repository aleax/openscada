## process this file with automake to generate Makefile.in

AM_CXXFLAGS= -I$(QTDIR)/include -fPIC -DPIC $(gtk-config --cflags)
bd_modules = ui_web_cfg.so ui_web_dbg.so ui_qt_gui.so

noinst_PROGRAMS = $(bd_modules)  

AM_LDFLAGS= -shared -module -avoid-version
LIBS=
LDADD=

ui_web_cfg_so_SOURCES  = web_cfg.cpp
ui_web_dbg_so_SOURCES  = web_dbg.cpp
ui_qt_gui_so_SOURCES   = qt_gui.cpp
ui_qt_gui_so_LDADD     = -L$(QTDIR)/lib -lqt-mt
