#CC=gcc3
#CXX=g++3

## process this file with automake to generate Makefile.in

AM_CXXFLAGS= -fPIC -DPIC -DUSE_OLD_FUNCTIONS
bd_modules = bd_my_sql.so bd_direct_dbf.so
noinst_PROGRAMS = $(bd_modules)  
AM_LDFLAGS= -shared -module -avoid-version
LIBS=
LDADD=

bd_direct_dbf_so_SOURCES = direct_dbf.cpp dbf.cpp

bd_my_sql_so_SOURCES    = my_sql.cpp
bd_my_sql_so_LDADD      = -L/usr/lib/mysql -lmysqlclient

