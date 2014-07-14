EXTRA_DIST = *.h po/*

oscd_modul_LTLIBRARIES = daq_SoundCard.la
daq_SoundCard_la_LDFLAGS = -module -avoid-version $(top_builddir)/src/liboscada.la
daq_SoundCard_la_SOURCES = sound.cpp
daq_SoundCard_la_LIBADD  = -lportaudio
daq_SoundCard_la_LIBTOOLFLAGS = --tag=disable-static

I18N_mod = $(oscd_modulpref)SoundCard
include ../../../../I18N.mk
