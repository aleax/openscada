SUBDIRS = moduls po

EXTRA_DIST = *.h	

INCLUDES = -DLOCALEDIR=\"$(oscd_localedir)\"

lib_LTLIBRARIES =  liboscada.la     
liboscada = tsys.cpp thd.cpp tkernel.cpp tarhives.cpp tbds.cpp tconfig.cpp tcontroller.cpp tsequrity.cpp \
	tmessage.cpp tgrpmodule.cpp tparam.cpp tprotocols.cpp ttransports.cpp ttiparam.cpp \
	tspecials.cpp tcontrollers.cpp ttipcontroller.cpp tparams.cpp tvalue.cpp terror.cpp \
	tuis.cpp tmodschedul.cpp tconfigelem.cpp tparamcontr.cpp tvalueelem.cpp tmodule.cpp xml.cpp tcontr.cpp
liboscada_la_SOURCES = $(liboscada)
liboscada_la_LDFLAGS = -module -avoid-version -rdynamic -ldl -lrt -lexpat

bin_PROGRAMS = OpenScada 
#---------- Dinamic linced -----------------------------------------------
OpenScada_SOURCES = main.cpp 
OpenScada_LDFLAGS = $(EXTRA_LDFLAGS) -Wl,-rpath,./ -Wl,-rpath,$(libdir) -loscada
#---------- Static linced -----------------------------------------------
#OpenScada_SOURCES = main.cpp 
#OpenScada_LDFLAGS = -export-dynamic -static -Wl,--whole-archive -loscada -z muldefs

