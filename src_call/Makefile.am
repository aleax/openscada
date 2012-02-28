bin_PROGRAMS = openscada

AM_CXXFLAGS = -I../src

openscada_SOURCES = main.cpp
openscada_LDFLAGS = $(EXTRA_LDFLAGS) -L../src/ -loscada -export-dynamic
