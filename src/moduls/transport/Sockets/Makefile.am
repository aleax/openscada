EXTRA_DIST = *.h po/*

oscd_modul_LTLIBRARIES = tr_Sockets.la
tr_Sockets_la_LDFLAGS = -module -avoid-version -no-undefined
tr_Sockets_la_SOURCES = socket.cpp
tr_Sockets_la_LIBTOOLFLAGS = --tag=disable-static

if SocketsIncl
tr_Sockets_la_CXXFLAGS = -DMOD_INCL
install:
endif

I18N_mod = $(oscd_modulpref)Sockets
