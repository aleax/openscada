## process this file with automake to generate Makefile.in

CXXFLAGS= -I$(QTDIR)/include -Wall -O2 -fPIC -DPIC -DOSC_DEBUG 
bd_modules = gui_qt_cfg.so

noinst_PROGRAMS = $(bd_modules)  

LDFLAGS= -shared -module -avoid-version
LIBS=
LDADD=
#
# Module sources
#
#mod_trend_so_OBJECTS=mod_trend.o

gui_qt_cfg_so_SOURCES  = qt_cfg.cpp
gui_qt_cfg_so_LDADD    = -L$(QTDIR)/lib -lqt

##@(language_modules)_sources = $(language_modules).cc
