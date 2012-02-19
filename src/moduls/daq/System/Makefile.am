EXTRA_DIST = *.h po/*

INCLUDES = -I$(top_srcdir)/src

if SystemIncl
noinst_LTLIBRARIES = daq_System.la
INCLUDES += -DMOD_INCL
else
oscd_modul_LTLIBRARIES = daq_System.la
endif
daq_System_la_LDFLAGS = -module -avoid-version -no-undefined
daq_System_la_SOURCES = os_contr.cpp da_cpu.cpp da_mem.cpp da_uptime.cpp da_hddtemp.cpp da_sensors.cpp da_smart.cpp da_hddstat.cpp da_netstat.cpp
daq_System_la_LIBADD  = $(LIB_SENSORS)
daq_System_la_LIBTOOLFLAGS = --tag=disable-static

I18N_mod = $(oscd_modulpref)System
