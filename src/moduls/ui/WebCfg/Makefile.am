EXTRA_DIST = *.h po/*

INCLUDES = -I$(top_srcdir)/src

if WebCfgIncl
noinst_LTLIBRARIES = ui_WebCfg.la
INCLUDES += -DMOD_INCL
else
oscd_modul_LTLIBRARIES = ui_WebCfg.la
endif
ui_WebCfg_la_LDFLAGS = -module -avoid-version -no-undefined
ui_WebCfg_la_SOURCES = web_cfg.cpp
ui_WebCfg_la_LIBTOOLFLAGS = --tag=disable-static

I18N_mod = $(oscd_modulpref)WebCfg
