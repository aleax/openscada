#CC=gcc3
#CXX=g++3

SUBDIRS = moduls

bin_PROGRAMS = OpenScada 
OpenScada_SOURCES = main.cpp tapplication.cpp tarhive.cpp tbd.cpp tconfig.cpp tcontroller.cpp \
			tmessage.cpp tgrpmodule.cpp tparam.cpp tprocrequest.cpp tprotocol.cpp \
			tspecial.cpp tcontrollers.cpp ttipcontroller.cpp tparams.cpp tvalue.cpp \
			tgui.cpp tmodschedul.cpp tconfigelem.cpp tparamcontr.cpp tvalueelem.cpp tmodule.cpp
OpenScada_LDADD = -ldl -lrt  

CXXFLAGS = -rdynamic -O2 -Ddebug -D_GLIBCPP_DEPRECATED
#CXXFLAGS = -rdynamic -O2
# compl_serv_LDADD = @INTLLIBS@ @ALLOCA@
localedir = $(datadir)/locale
INCLUDES = -I../../intl -DLOCALEDIR=\"$(localedir)\"

#noinst_LTLIBRARIES = server_lib.la
#server_lib_la_SOURCES = server_lib.cc

