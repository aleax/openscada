EXTRA_DIST = *.h po/*

if VCAEngineIncl
noinst_LTLIBRARIES = ui_VCAEngine.la
ui_VCAEngine_la_CXXFLAGS = -DMOD_INCL -fpic
ui_VCAEngine_la_LIBTOOLFLAGS = --tag=disable-shared
ui_VCAEngine_la_LDFLAGS = -module
else
oscd_modul_LTLIBRARIES = ui_VCAEngine.la
ui_VCAEngine_la_LIBTOOLFLAGS = --tag=disable-static
ui_VCAEngine_la_LDFLAGS = -module -avoid-version $(top_builddir)/src/liboscada.la
endif

ui_VCAEngine_la_SOURCES = vcaengine.cpp libwidg.cpp widget.cpp origwidg.cpp project.cpp session.cpp

I18N_mod = $(oscd_modulpref)VCAEngine
include ../../../../I18N.mk
