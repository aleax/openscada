## process this file with automake to generate Makefile.in

# CXXFLAGS= $(CXXFLAFS) -Wall -fPIC -DPIC
CXXFLAGS= -Wall -O2 -fPIC -DPIC -DOSC_DEBUG
bd_modules = spec_test_bd.so spec_test_kernel.so spec_test_transp.so

noinst_PROGRAMS = $(bd_modules)  

LDFLAGS= -shared -module -avoid-version
LIBS=
LDADD=
#
# Module sources
#
#mod_trend_so_OBJECTS=mod_trend.o

spec_test_bd_so_SOURCES     = test_bd.cpp 
spec_test_kernel_so_SOURCES = test_kernel.cpp
spec_test_transp_so_SOURCES = test_transp.cpp

##@(language_modules)_sources = $(language_modules).cc
