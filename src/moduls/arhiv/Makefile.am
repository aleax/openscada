# CC=gcc3
# CXX=g++3

## process this file with automake to generate Makefile.in

# CXXFLAGS= $(CXXFLAFS) -Wall -fPIC -DPIC
CXXFLAGS= -Wall -O2 -fPIC -DPIC -DOSC_DEBUG
bd_modules = test_arhiv.so

noinst_PROGRAMS = $(bd_modules)  

LDFLAGS= -shared -export-dynamic -module -avoid-version
LIBS=
LDADD=
#
# Module sources
#
#mod_trend_so_OBJECTS=mod_trend.o

test_arhiv_so_SOURCES = test_arhiv.cpp

##@(language_modules)_sources = $(language_modules).cc
