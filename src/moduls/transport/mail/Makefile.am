## process this file with automake to generate Makefile.in

EXTRA_DIST = *.h

AM_CXXFLAGS= -I../../.. -fPIC -DPIC
AM_LDFLAGS= -shared -module -avoid-version

oscd_modul_PROGRAMS = tr_mail.so
tr_mail_so_SOURCES = mail.cpp

