SUBDIRS = ${BldInclMods} . ${BldMods}

EXTRA_DIST = *.h po/*

oscadaincludedir=$(includedir)/openscada
oscadainclude_HEADERS = *.h

lib_LTLIBRARIES = liboscada.la
liboscada_la_SOURCES = tsys.cpp resalloc.cpp tarchives.cpp tarchval.cpp tbds.cpp tvariant.cpp tconfig.cpp \
    tcontroller.cpp tsecurity.cpp tmess.cpp tsubsys.cpp tprotocols.cpp ttransports.cpp \
    tspecials.cpp ttypedaq.cpp tprmtmpl.cpp tvalue.cpp terror.cpp tuis.cpp tmodschedul.cpp \
    telem.cpp tparamcontr.cpp tmodule.cpp xml.cpp tcntrnode.cpp tdaqs.cpp tfunction.cpp
liboscada_la_CXXFLAGS = $(PTHREAD_CFLAGS)
liboscada_la_LIBADD = $(PTHREAD_LIBS) $(LIB_CORE)

I18N_mod = $(PACKAGE)
I18N_add1 = -LShell -kGETTEXT ../data/openscada_start.in
include ../I18N.mk
