#CC=gcc3
#CXX=g++3

SUBDIRS = moduls

localedir = $(datadir)/locale
INCLUDES = -I../../intl -DLOCALEDIR=\"$(localedir)\"

#AM_CXXFLAGS= -O3 -DOSC_DEBUG

#EXTRA_LTLIBRARIES = OpenScada.la
lib_LTLIBRARIES =  liboscada.la     
liboscada = tsys.cpp thd.cpp tkernel.cpp tarhives.cpp tbds.cpp tconfig.cpp tcontroller.cpp tsequrity.cpp \
	tmessage.cpp tgrpmodule.cpp tparam.cpp tprotocols.cpp ttransports.cpp ttiparam.cpp \
	tspecials.cpp tcontrollers.cpp ttipcontroller.cpp tparams.cpp tvalue.cpp terror.cpp \
	tuis.cpp tmodschedul.cpp tconfigelem.cpp tparamcontr.cpp tvalueelem.cpp tmodule.cpp xml.cpp tcontr.cpp
liboscada_la_SOURCES = $(liboscada)
liboscada_la_LDFLAGS = -module -avoid-version -rdynamic -ldl -lrt -lexpat
#OpenScada_la_CXXFLAGS = 
#OpenScada_la_CXXFLAGS = -fPIC -DPIC -DOSC_DEBUG -D_REENTRANT

bin_PROGRAMS = OpenScada_din OpenScada_st 
OpenScada_din_SOURCES = main.cpp 
OpenScada_din_LDFLAGS = $(EXTRA_LDFLAGS) -Wl,-rpath,./ -loscada
#---------- Static linced -----------------------------------------------
OpenScada_st_SOURCES = main.cpp 
OpenScada_st_LDFLAGS = -export-dynamic -static -Wl,--whole-archive -loscada -z muldefs


# compl_serv_LDADD = @INTLLIBS@ @ALLOCA@

#noinst_LTLIBRARIES = server_lib.la
#server_lib_la_SOURCES = server_lib.cc

