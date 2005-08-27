SUBDIRS = moduls po

EXTRA_DIST = *.h	

INCLUDES = -DLOCALEDIR=\"$(oscd_localedir)\"

lib_LTLIBRARIES =  liboscada.la     
liboscada_la_SOURCES = tsys.cpp resalloc.cpp tarchives.cpp tbds.cpp tconfig.cpp \
    tcontroller.cpp tsequrity.cpp tmess.cpp tsubsys.cpp tparam.cpp tprotocols.cpp \
    ttransports.cpp ttiparam.cpp tspecials.cpp ttipcontroller.cpp tparams.cpp \
    tvalue.cpp terror.cpp tuis.cpp tmodschedul.cpp telem.cpp tparamcontr.cpp \
    tmodule.cpp xml.cpp tcntrnode.cpp tcontrollers.cpp tfunctions.cpp
liboscada_la_LIBADD = -ldl -lrt -lexpat
liboscada_la_LDFLAGS = -no-undefined -version-info 0:0:0

bin_PROGRAMS = openscada 
#---------- Dinamic linced -----------------------------------------------
openscada_SOURCES = main.cpp 
openscada_LDFLAGS = $(EXTRA_LDFLAGS) -Wl,-rpath,$(libdir) -loscada
#---------- Static linced -----------------------------------------------
#OpenScada_SOURCES = main.cpp 
#OpenScada_LDFLAGS = -export-dynamic -static -loscada

