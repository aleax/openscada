#CC=gcc3
#CXX=g++3

AM_CXXFLAGS= -fPIC -DPIC
contr_modules = cntr_virt1.so cntr_sys.so

noinst_PROGRAMS = $(contr_modules)  

AM_LDFLAGS= -shared -module -avoid-version
LIBS=
LDADD=-lrt

cntr_virt1_so_SOURCES = virtual1.cpp
cntr_sys_so_SOURCES   = sys.cpp

