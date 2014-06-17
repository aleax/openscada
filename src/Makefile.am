SUBDIRS = ${BldInclMods} . ${BldMods}

EXTRA_DIST = *.h po/*

oscadaincludedir=$(includedir)/openscada
oscadainclude_HEADERS = *.h

lib_LTLIBRARIES = liboscada.la
liboscada_la_SOURCES = tsys.cpp resalloc.cpp tarchives.cpp tarchval.cpp tbds.cpp tvariant.cpp tconfig.cpp \
    tcontroller.cpp tsecurity.cpp tmess.cpp tsubsys.cpp tprotocols.cpp ttransports.cpp \
    tspecials.cpp ttipdaq.cpp tprmtmpl.cpp tvalue.cpp terror.cpp tuis.cpp tmodschedul.cpp \
    telem.cpp tparamcontr.cpp tmodule.cpp xml.cpp tcntrnode.cpp tdaqs.cpp tfunction.cpp
liboscada_la_LIBADD = -ldl -lpthread -lrt -lcrypt -lz -lpcre $(LIB_CORE)
liboscada_la_LDFLAGS = -version-info $(oscd_LIBREVISION)

I18N_mod = $(PACKAGE)
I18N_add1 = -LShell ../data/openscada_start.in
include ../I18N.mk
