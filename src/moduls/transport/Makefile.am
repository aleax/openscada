## process this file with automake to generate Makefile.in

AM_CXXFLAGS= -fPIC -DPIC
bd_modules = tr_mail.so tr_socket.so

noinst_PROGRAMS = $(bd_modules)  

AM_LDFLAGS= -shared -module -avoid-version
LIBS=
LDADD=

tr_mail_so_SOURCES = mail.cpp
tr_socket_so_SOURCES = socket.cpp
