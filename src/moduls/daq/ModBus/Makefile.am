EXTRA_DIST = *.h po/*

INCLUDES = -I$(top_srcdir)/src

if ModBusIncl
noinst_LTLIBRARIES = daq_ModBus.la
INCLUDES += -DMOD_INCL
else
oscd_modul_LTLIBRARIES = daq_ModBus.la
endif
daq_ModBus_la_LDFLAGS = -module -avoid-version -no-undefined
daq_ModBus_la_SOURCES = moduls.cpp modbus_daq.cpp modbus_prt.cpp
daq_ModBus_la_LIBTOOLFLAGS = --tag=disable-static

I18N_mod = $(oscd_modulpref)ModBus
