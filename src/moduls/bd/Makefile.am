#CC=gcc3
#CXX=g++3

## process this file with automake to generate Makefile.in

# CXXFLAGS= $(CXXFLAFS) -Wall -fPIC -DPIC
CXXFLAGS= -Wall -fPIC -DPIC -Ddebug -D_GLIBCPP_DEPRECATED
bd_modules = test_bd.so test_bd1.so my_sql.so direct_dbf.so

noinst_PROGRAMS = $(bd_modules)  

LDFLAGS= -shared -export-dynamic -module -avoid-version
LIBS=
LDADD=
#
# Module sources
#
#mod_trend_so_OBJECTS=mod_trend.o

direct_dbf_so_SOURCES = direct_dbf.cpp dbf.cpp
# direct_bd_so_LDADD   = -ljpeg ../gener/tmodule.a
my_sql_so_SOURCES    = my_sql.cpp
test_bd_so_SOURCES   = test_bd.cpp
test_bd1_so_SOURCES  = test_bd1.cpp

##@(language_modules)_sources = $(language_modules).cc
