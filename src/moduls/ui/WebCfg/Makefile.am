EXTRA_DIST = *.h po/*

if WebCfgIncl
noinst_LTLIBRARIES = ui_WebCfg.la
ui_WebCfg_la_CXXFLAGS = -DMOD_INCL -fpic
ui_WebCfg_la_LIBTOOLFLAGS = --tag=disable-shared
ui_WebCfg_la_LDFLAGS = -module
else
oscd_modul_LTLIBRARIES = ui_WebCfg.la
ui_WebCfg_la_LIBTOOLFLAGS = --tag=disable-static
ui_WebCfg_la_LDFLAGS = -module -avoid-version $(top_builddir)/src/liboscada.la
endif

ui_WebCfg_la_SOURCES = web_cfg.cpp


I18N_mod = $(oscd_modulpref)WebCfg
include ../../../../I18N.mk
