## process this file with automake to generate Makefile.in
SUBDIRS = po

EXTRA_DIST = *.h doc/generic.sxw

AM_CXXFLAGS= -I$(top_srcdir)/src -fPIC -DPIC -DUSE_OLD_FUNCTIONS
AM_LDFLAGS= -shared -module

oscd_modul_PROGRAMS = bd_my_sql.so 
bd_my_sql_so_SOURCES = my_sql.cpp
bd_my_sql_so_LDADD = -L/usr/lib/mysql -lmysqlclient

