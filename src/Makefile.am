#CC=gcc3
#CXX=g++3

SUBDIRS = moduls

localedir = $(datadir)/locale
INCLUDES = -I../../intl -DLOCALEDIR=\"$(localedir)\"

CXXFLAGS= -O2 -Wall

#EXTRA_LTLIBRARIES = OpenScada.la
lib_LTLIBRARIES =  OpenScada.la     
OpenScada = tsys.cpp tkernel.cpp tarhives.cpp tbds.cpp tconfig.cpp tcontroller.cpp \
	tmessage.cpp tgrpmodule.cpp tparam.cpp tprotocols.cpp ttransports.cpp ttiparam.cpp \
	tspecials.cpp tcontrollers.cpp ttipcontroller.cpp tparams.cpp tvalue.cpp terror.cpp \
	tuis.cpp tmodschedul.cpp tconfigelem.cpp tparamcontr.cpp tvalueelem.cpp tmodule.cpp 
OpenScada_la_SOURCES = $(OpenScada)
OpenScada_la_LDFLAGS = -module -avoid-version -rdynamic -ldl -lrt
OpenScada_la_CXXFLAGS = -D_GLIBCPP_DEPRECATED -DOSC_DEBUG
#OpenScada_la_CXXFLAGS = -fPIC -DPIC -D_GLIBCPP_DEPRECATED -DOSC_DEBUG -D_REENTRANT

bin_PROGRAMS = OpenScada 
OpenScada_SOURCES = main.cpp 
OpenScada_LDFLAGS = $(EXTRA_LDFLAGS) -rdynamic -ldl -lrt -Wl,-rpath,./ 
OpenScada_LDADD = OpenScada.la
#./.libs/OpenScada.a
OpenScada_CXXFLAGS = 

# compl_serv_LDADD = @INTLLIBS@ @ALLOCA@

#noinst_LTLIBRARIES = server_lib.la
#server_lib_la_SOURCES = server_lib.cc

