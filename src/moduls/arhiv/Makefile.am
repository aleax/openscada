# CC=gcc3
# CXX=g++3

## process this file with automake to generate Makefile.in

# CXXFLAGS= $(CXXFLAFS) -Wall -fPIC -DPIC
CXXFLAGS= -O2 -fPIC -DPIC -DOSC_DEBUG
bd_modules = arh_base.so

noinst_PROGRAMS = $(bd_modules)  

LDFLAGS= -shared -module -avoid-version
LIBS=
LDADD=
#
# Module sources
#
#mod_trend_so_OBJECTS=mod_trend.o

arh_base_so_SOURCES = base.cpp

##@(language_modules)_sources = $(language_modules).cc
