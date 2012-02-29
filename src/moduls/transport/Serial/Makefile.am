EXTRA_DIST = *.h po/*

oscd_modul_LTLIBRARIES = tr_Serial.la
tr_Serial_la_LDFLAGS = -module -avoid-version -no-undefined
tr_Serial_la_SOURCES = mod_serial.cpp
tr_Serial_la_LIBTOOLFLAGS = --tag=disable-static

if SerialIncl
tr_Serial_la_CXXFLAGS = -DMOD_INCL
install:
endif

I18N_mod = $(oscd_modulpref)Serial
