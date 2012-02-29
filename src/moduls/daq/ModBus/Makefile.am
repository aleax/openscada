EXTRA_DIST = *.h po/*

oscd_modul_LTLIBRARIES = daq_ModBus.la
daq_ModBus_la_LDFLAGS = -module -avoid-version -no-undefined
daq_ModBus_la_SOURCES = moduls.cpp modbus_daq.cpp modbus_prt.cpp
daq_ModBus_la_LIBTOOLFLAGS = --tag=disable-static

if ModBusIncl
daq_ModBus_la_CXXFLAGS = -DMOD_INCL
install:
endif

I18N_mod = $(oscd_modulpref)ModBus
