## process this file with automake to generate Makefile.in

AM_CXXFLAGS= -fPIC -DPIC
bd_modules = spec_test_bd.so spec_test_kernel.so spec_test_transp.so

noinst_PROGRAMS = $(bd_modules)  

AM_LDFLAGS= -shared -module -avoid-version
LIBS=
LDADD=

spec_test_bd_so_SOURCES     = test_bd.cpp 
spec_test_kernel_so_SOURCES = test_kernel.cpp
spec_test_transp_so_SOURCES = test_transp.cpp
