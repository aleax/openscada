#CC=gcc3
#CXX=g++3

## process this file with automake to generate Makefile.in

# CXXFLAGS= $(CXXFLAFS) -Wall -fPIC -DPIC
CXXFLAGS= -Wall -fPIC -DPIC -Ddebug
contr_modules = test_tcontr.so virtual.so

noinst_PROGRAMS = $(contr_modules)  

LDFLAGS= -shared -export-dynamic -module -avoid-version
LIBS=
LDADD=-lrt
#
# Module sources
#
#mod_trend_so_OBJECTS=mod_trend.o

test_tcontr_so_SOURCES = test_tcontr.cpp
virtual_so_SOURCES = virtual.cpp

##@(language_modules)_sources = $(language_modules).cc
