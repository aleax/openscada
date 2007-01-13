SUBDIRS = po moduls
    
EXTRA_DIST = *.h	

INCLUDES = -DLOCALEDIR=\"$(oscd_localedir)\"

AM_CXXFLAGS =

lib_LTLIBRARIES =  liboscada.la
liboscada_la_SOURCES = tsys.cpp resalloc.cpp tarchives.cpp tarchval.cpp tbds.cpp tconfig.cpp \
    tcontroller.cpp tsecurity.cpp tmess.cpp tsubsys.cpp tprotocols.cpp ttransports.cpp \
    ttiparam.cpp tspecials.cpp ttipdaq.cpp tprmtmpl.cpp \
    tvalue.cpp terror.cpp tuis.cpp tmodschedul.cpp telem.cpp tparamcontr.cpp \
    tmodule.cpp xml.cpp tcntrnode.cpp tdaqs.cpp tfunction.cpp
liboscada_la_LIBADD = -ldl -lrt -lexpat -lcrypt $(sys_lib)/libgd.a -lpng -lz
liboscada_la_LDFLAGS = -no-undefined -version-info 0:0:0

bin_PROGRAMS = openscada 
#---------- Dinamic linking -----------------------------------------------
openscada_SOURCES = main.cpp 
openscada_LDFLAGS = $(EXTRA_LDFLAGS) -Wl,-rpath,$(libdir) -loscada
#---------- Static linking -----------------------------------------------
#OpenScada_SOURCES = main.cpp 
#OpenScada_LDFLAGS = -export-dynamic -static -loscada
