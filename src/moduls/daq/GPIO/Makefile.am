EXTRA_DIST = *.h po/*

oscd_modul_LTLIBRARIES = daq_GPIO.la
daq_GPIO_la_CXXFLAGS =
daq_GPIO_la_LIBTOOLFLAGS = --tag=disable-static
daq_GPIO_la_LDFLAGS = -module -avoid-version $(top_builddir)/src/liboscada.la

daq_GPIO_la_CXXFLAGS += $(GPIO_CFLAGS)
daq_GPIO_la_LDFLAGS += $(GPIO_LDLAGS)
daq_GPIO_la_SOURCES = GPIO_module.cpp da_BCM2835.cpp bcm2835.c da_SUNXI.cpp

I18N_mod = $(oscd_modulpref)GPIO
include ../../../../I18N.mk
