# CC=gcc3
# CXX=g++3

## process this file with automake to generate Makefile.in

AM_CXXFLAGS= -fPIC -DPIC
bd_modules = arh_base.so

noinst_PROGRAMS = $(bd_modules)  

AM_LDFLAGS= -shared -module -avoid-version
LIBS=
LDADD=

arh_base_so_SOURCES = base.cpp

