EXTRA_DIST = *.h po/*

oscd_modul_LTLIBRARIES = ui_WebCfg.la
ui_WebCfg_la_LDFLAGS = -module -avoid-version -no-undefined
ui_WebCfg_la_SOURCES = web_cfg.cpp
ui_WebCfg_la_LIBTOOLFLAGS = --tag=disable-static

if WebCfgIncl
ui_WebCfg_la_CXXFLAGS = -DMOD_INCL
install:
endif

I18N_mod = $(oscd_modulpref)WebCfg
