## process this file with automake to generate Makefile.in

AM_CXXFLAGS= -fPIC -DPIC
bd_modules = prot_self.so prot_http.so

noinst_PROGRAMS = $(bd_modules)  

AM_LDFLAGS= -shared -module -avoid-version
LIBS=
LDADD=

prot_self_so_SOURCES = self.cpp
prot_http_so_SOURCES = http.cpp
