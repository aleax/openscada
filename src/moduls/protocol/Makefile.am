## process this file with automake to generate Makefile.in

# CXXFLAGS= $(CXXFLAFS) -Wall -fPIC -DPIC
CXXFLAGS= -Wall -O2 -fPIC -DPIC -Ddebug
bd_modules = prot_self.so prot_http.so

noinst_PROGRAMS = $(bd_modules)  

LDFLAGS= -shared -module -avoid-version
LIBS=
LDADD=
#
# Module sources
#
#mod_trend_so_OBJECTS=mod_trend.o

prot_self_so_SOURCES = self.cpp
prot_http_so_SOURCES = http.cpp

##@(language_modules)_sources = $(language_modules).cc
