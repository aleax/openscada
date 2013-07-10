EXTRA_DIST = *.h po/*

if SerialIncl
noinst_LTLIBRARIES = tr_Serial.la
tr_Serial_la_CXXFLAGS = -DMOD_INCL -fpic
tr_Serial_la_LIBTOOLFLAGS = --tag=disable-shared
else
oscd_modul_LTLIBRARIES = tr_Serial.la
tr_Serial_la_LIBTOOLFLAGS = --tag=disable-static
endif

tr_Serial_la_LDFLAGS = -module -avoid-version -no-undefined
tr_Serial_la_SOURCES = mod_serial.cpp

I18N_mod = $(oscd_modulpref)Serial
include ../../../../I18N.mk
