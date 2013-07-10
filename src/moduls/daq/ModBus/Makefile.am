EXTRA_DIST = *.h po/*

if ModBusIncl
noinst_LTLIBRARIES = daq_ModBus.la
daq_ModBus_la_CXXFLAGS = -DMOD_INCL -fpic
daq_ModBus_la_LIBTOOLFLAGS = --tag=disable-shared
else
oscd_modul_LTLIBRARIES = daq_ModBus.la
daq_ModBus_la_LIBTOOLFLAGS = --tag=disable-static
endif

daq_ModBus_la_LDFLAGS = -module -avoid-version -no-undefined
daq_ModBus_la_SOURCES = moduls.cpp modbus_daq.cpp modbus_prt.cpp

I18N_mod = $(oscd_modulpref)ModBus
include ../../../../I18N.mk
