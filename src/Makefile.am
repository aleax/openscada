SUBDIRS = moduls

bin_PROGRAMS = OpenScadaServ 
OpenScadaServ_SOURCES = main.cpp tapplication.cpp tarhive.cpp tbd.cpp tconfig.cpp tcontroller.cpp \
                        tmessage.cpp tgrpmodule.cpp tparam.cpp tprocrequest.cpp tprotocol.cpp \
			tspecial.cpp ttipcontroller.cpp tparams.cpp tmodschedul.cpp
OpenScadaServ_LDADD = -ldl -lrt  

CXXFLAGS = -rdynamic -O2 -Ddebug 
#CXXFLAGS = -rdynamic -O2
# compl_serv_LDADD = @INTLLIBS@ @ALLOCA@
localedir = $(datadir)/locale
INCLUDES = -I../../intl -DLOCALEDIR=\"$(localedir)\"

#noinst_LTLIBRARIES = server_lib.la
#server_lib_la_SOURCES = server_lib.cc

