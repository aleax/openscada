#CC=gcc3
#CXX=g++3

## process this file with automake to generate Makefile.in

# CXXFLAGS= $(CXXFLAFS) -Wall -fPIC -DPIC
CXXFLAGS= -O2 -fPIC -DPIC -DOSC_DEBUG
# CXXFLAGS=-Wall -O2 -fPIC -DPIC -DOSC_DEBUG -D_GLIBCPP_DEPRECATED
contr_modules = cntr_virt1.so cntr_sys.so

noinst_PROGRAMS = $(contr_modules)  

LDFLAGS= -shared -module -avoid-version
LIBS=
LDADD=-lrt
#
# Module sources
#
#mod_trend_so_OBJECTS=mod_trend.o

# cntr_test_so_SOURCES = test_tcontr.cpp
cntr_virt1_so_SOURCES = virtual1.cpp
cntr_sys_so_SOURCES   = sys.cpp

##@(language_modules)_sources = $(language_modules).cc
