# CC=gcc3
# CXX=g++3

## process this file with automake to generate Makefile.in

# CXXFLAGS= $(CXXFLAFS) -Wall -fPIC -DPIC
CXXFLAGS+= -Wall -fPIC -DPIC -Ddebug
bd_modules = test_bd.so test_bd1.so test_bd2.so direct_bd.so

noinst_PROGRAMS = $(bd_modules)  

LDFLAGS= -shared -export-dynamic -module -avoid-version
LIBS=
LDADD= ../gener/tmodule.a
#
# Module sources
#
#mod_trend_so_OBJECTS=mod_trend.o

direct_bd_so_SOURCES = direct_bd.cpp dbf.cpp
# direct_bd_so_LDADD   = -ljpeg ../gener/tmodule.a
test_bd_so_SOURCES   = test_bd.cpp
test_bd1_so_SOURCES  = test_bd1.cpp
test_bd2_so_SOURCES  = test_bd2.cpp

##@(language_modules)_sources = $(language_modules).cc
