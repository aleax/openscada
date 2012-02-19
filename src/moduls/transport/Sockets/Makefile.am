EXTRA_DIST = *.h po/*

INCLUDES = -I$(top_srcdir)/src

if SocketsIncl
noinst_LTLIBRARIES = tr_Sockets.la
INCLUDES += -DMOD_INCL
else
oscd_modul_LTLIBRARIES = tr_Sockets.la
endif
tr_Sockets_la_LDFLAGS = -module -avoid-version -no-undefined
tr_Sockets_la_SOURCES = socket.cpp
tr_Sockets_la_LIBTOOLFLAGS = --tag=disable-static

I18N_mod = $(oscd_modulpref)Sockets
