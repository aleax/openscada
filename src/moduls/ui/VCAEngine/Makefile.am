EXTRA_DIST = *.h po/*

INCLUDES = -I$(top_srcdir)/src

if VCAEngineIncl
noinst_LTLIBRARIES = ui_VCAEngine.la
INCLUDES += -DMOD_INCL
else
oscd_modul_LTLIBRARIES = ui_VCAEngine.la
endif
ui_VCAEngine_la_LDFLAGS = -module -avoid-version -no-undefined
ui_VCAEngine_la_SOURCES = vcaengine.cpp libwidg.cpp widget.cpp origwidg.cpp project.cpp session.cpp
ui_VCAEngine_la_LIBTOOLFLAGS = --tag=disable-static

I18N_mod = $(oscd_modulpref)VCAEngine
