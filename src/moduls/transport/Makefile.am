## process this file with automake to generate Makefile.in

# CXXFLAGS= $(CXXFLAFS) -Wall -fPIC -DPIC
CXXFLAGS= -Wall -O2 -fPIC -DPIC -DOSC_DEBUG
bd_modules = test_transport.so socket.so

noinst_PROGRAMS = $(bd_modules)  

LDFLAGS= -shared -module -avoid-version
LIBS=
LDADD=
#
# Module sources
#
#mod_trend_so_OBJECTS=mod_trend.o

test_transport_so_SOURCES = test_transport.cpp
socket_so_SOURCES = socket.cpp

##@(language_modules)_sources = $(language_modules).cc
