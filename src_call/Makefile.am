bin_PROGRAMS = openscada
openscada_SOURCES = main.cpp
if CoreLibStatic
openscada_LDFLAGS = $(EXTRA_LDFLAGS) -rdynamic -Wl,--whole-archive,../src/.libs/liboscada.a,--no-whole-archive ../src/liboscada.la
else
openscada_LDFLAGS = $(EXTRA_LDFLAGS) -L../src/ -loscada
endif
