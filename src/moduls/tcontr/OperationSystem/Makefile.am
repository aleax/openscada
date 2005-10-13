SUBDIRS = po

EXTRA_DIST = *.h

AM_CXXFLAGS= -I$(top_srcdir)/src -fPIC -DPIC
AM_LDFLAGS= -shared -module

oscd_modul_PROGRAMS = cntr_OperationSystem.so
cntr_OperationSystem_so_SOURCES = os_contr.cpp da_cpu.cpp da_mem.cpp da_uptime.cpp da_smart.cpp da_sensors.cpp
