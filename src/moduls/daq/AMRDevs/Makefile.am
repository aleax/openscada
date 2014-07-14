EXTRA_DIST = *.h po/*

oscd_modul_LTLIBRARIES = daq_AMRDevs.la
daq_AMRDevs_la_LDFLAGS = -module -avoid-version $(top_builddir)/src/liboscada.la
daq_AMRDevs_la_SOURCES = mod_amr.cpp da_Kontar.cpp
# da_FlowTEC.cpp da_Ergomera.cpp
daq_AMRDevs_la_LIBTOOLFLAGS = --tag=disable-static

I18N_mod = $(oscd_modulpref)AMRDevs
include ../../../../I18N.mk
