EXTRA_DIST = *.h po/*

if WebCfgIncl
noinst_LTLIBRARIES = ui_WebCfg.la
ui_WebCfg_la_CXXFLAGS = -DMOD_INCL -fpic
ui_WebCfg_la_LIBTOOLFLAGS = --tag=disable-shared
else
oscd_modul_LTLIBRARIES = ui_WebCfg.la
ui_WebCfg_la_LIBTOOLFLAGS = --tag=disable-static
endif

ui_WebCfg_la_LDFLAGS = -module -avoid-version -no-undefined
ui_WebCfg_la_SOURCES = web_cfg.cpp


I18N_mod = $(oscd_modulpref)WebCfg
include ../../../../I18N.mk
