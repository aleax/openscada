## process this file with automake to generate Makefile.in

# CXXFLAGS= $(CXXFLAFS) -Wall -fPIC -DPIC
CXXFLAGS+= -Wall -fPIC -DPIC -Ddebug
bd_modules = test_special.so

noinst_PROGRAMS = $(bd_modules)  

LDFLAGS= -shared -export-dynamic -module -avoid-version
LIBS=
LDADD= ../gener/tmodule.a
#
# Module sources
#
#mod_trend_so_OBJECTS=mod_trend.o

test_special_so_SOURCES = test_special.cpp

##@(language_modules)_sources = $(language_modules).cc
