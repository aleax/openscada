EXTRA_DIST = *.h po/*

INCLUDES = -I$(top_srcdir)/src

if SerialIncl
noinst_LTLIBRARIES = tr_Serial.la
INCLUDES += -DMOD_INCL
else
oscd_modul_LTLIBRARIES = tr_Serial.la
endif
tr_Serial_la_LDFLAGS = -module -avoid-version -no-undefined
tr_Serial_la_SOURCES = mod_serial.cpp
tr_Serial_la_LIBTOOLFLAGS = --tag=disable-static

I18N_mod = $(oscd_modulpref)Serial
