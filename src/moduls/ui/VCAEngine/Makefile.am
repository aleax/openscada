EXTRA_DIST = *.h po/*

if VCAEngineIncl
noinst_LTLIBRARIES = ui_VCAEngine.la
ui_VCAEngine_la_CXXFLAGS = -DMOD_INCL -fpic
ui_VCAEngine_la_LIBTOOLFLAGS = --tag=disable-shared
else
oscd_modul_LTLIBRARIES = ui_VCAEngine.la
ui_VCAEngine_la_LIBTOOLFLAGS = --tag=disable-static
endif

ui_VCAEngine_la_LDFLAGS = -module -avoid-version -no-undefined
ui_VCAEngine_la_SOURCES = vcaengine.cpp libwidg.cpp widget.cpp origwidg.cpp project.cpp session.cpp

I18N_mod = $(oscd_modulpref)VCAEngine
include ../../../../I18N.mk
