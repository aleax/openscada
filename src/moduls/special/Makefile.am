## process this file with automake to generate Makefile.in

# CXXFLAGS= $(CXXFLAFS) -Wall -fPIC -DPIC
CXXFLAGS= -Wall -O2 -fPIC -DPIC -DOSC_DEBUG
bd_modules = spec_web_cfg.so spec_web_info.so spec_web_dbg.so

noinst_PROGRAMS = $(bd_modules)  

LDFLAGS= -shared -module -avoid-version
LIBS=
LDADD=
#
# Module sources
#
#mod_trend_so_OBJECTS=mod_trend.o

spec_web_cfg_so_SOURCES  = web_cfg.cpp
spec_web_info_so_SOURCES = web_info.cpp
spec_web_dbg_so_SOURCES  = web_dbg.cpp

##@(language_modules)_sources = $(language_modules).cc
