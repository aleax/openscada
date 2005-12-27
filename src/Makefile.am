SUBDIRS = po moduls
    
EXTRA_DIST = *.h	

INCLUDES = -DLOCALEDIR=\"$(oscd_localedir)\"

lib_LTLIBRARIES =  liboscada.la     
liboscada_la_SOURCES = tsys.cpp resalloc.cpp tarchives.cpp tbds.cpp tconfig.cpp \
    tcontroller.cpp tsecurity.cpp tmess.cpp tsubsys.cpp tprotocols.cpp ttransports.cpp \
    ttiparam.cpp tspecials.cpp ttipcontroller.cpp tparams.cpp tparam.cpp tprmtmpl.cpp \
    tvalue.cpp terror.cpp tuis.cpp tmodschedul.cpp telem.cpp tparamcontr.cpp \
    tmodule.cpp xml.cpp tcntrnode.cpp tcontrollers.cpp tfunctions.cpp
liboscada_la_LIBADD = -ldl -lrt -lexpat
liboscada_la_LDFLAGS = -no-undefined -version-info 0:0:0

bin_PROGRAMS = openscada 
#---------- Dinamic linked -----------------------------------------------
openscada_SOURCES = main.cpp 
openscada_LDFLAGS = $(EXTRA_LDFLAGS) -Wl,-rpath,$(libdir) -loscada
#---------- Static linked -----------------------------------------------
#OpenScada_SOURCES = main.cpp 
#OpenScada_LDFLAGS = -export-dynamic -static -loscada

