#EXTRA_DIST = *.h

bin_PROGRAMS = openscada

AM_CXXFLAGS= -I../src

#---------- Dinamic linking -----------------------------------------------
openscada_SOURCES = main.cpp
openscada_LDFLAGS = $(EXTRA_LDFLAGS) -Wl,-rpath,$(libdir) -loscada -L../src/
#---------- Static linking -----------------------------------------------
#OpenScada_SOURCES = main.cpp
#OpenScada_LDFLAGS = -export-dynamic -static -loscada
