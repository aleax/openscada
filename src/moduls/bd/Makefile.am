#CC=gcc3
#CXX=g++3

## process this file with automake to generate Makefile.in

#CXXFLAGS= -Wall -fPIC -DPIC -Ddebug -D_GLIBCPP_DEPRECATED
CXXFLAGS= -Wall -O2 -fPIC -DPIC -D_GLIBCPP_DEPRECATED -DOSC_DEBUG
bd_modules = bd_my_sql.so bd_direct_dbf.so
noinst_PROGRAMS = $(bd_modules)  
LDFLAGS= -shared -module -avoid-version
LIBS=
LDADD=
#
# Module sources
#
#mod_trend_so_OBJECTS=mod_trend.o

bd_direct_dbf_so_SOURCES = direct_dbf.cpp dbf.cpp
# direct_bd_so_LDADD   = -ljpeg ../gener/tmodule.a

bd_my_sql_so_SOURCES    = my_sql.cpp
bd_my_sql_so_LDADD      = -L/usr/lib/mysql -lmysqlclient

##@(language_modules)_sources = $(language_modules).cc
