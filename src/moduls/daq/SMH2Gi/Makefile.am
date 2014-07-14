EXTRA_DIST = *.h *.hpp po/* data/*

oscd_modul_LTLIBRARIES = daq_SMH2Gi.la
daq_SMH2Gi_la_CXXFLAGS = $(SMH2Gi_CFLAGS)
daq_SMH2Gi_la_LDFLAGS = -module -avoid-version $(top_builddir)/src/liboscada.la $(SMH2Gi_LDLAGS)
daq_SMH2Gi_la_SOURCES = module.cpp shm.cpp rlinifile.cpp rlsharedmemory.cpp rlwthread.cpp rlcutil.cpp rlsocket.cpp
daq_SMH2Gi_la_LIBTOOLFLAGS = --tag=disable-static

SMH2Giconfigdir = $(oscd_datadir)/SegneticsMRC
SMH2Giconfig_DATA = data/*

if SMH2GiIncl
daq_SMH2Gi_la_CXXFLAGS += -DMOD_INCL
install:
endif

I18N_mod = $(oscd_modulpref)SMH2Gi
include ../../../../I18N.mk
