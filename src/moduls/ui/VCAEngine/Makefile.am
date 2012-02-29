EXTRA_DIST = *.h po/*

oscd_modul_LTLIBRARIES = ui_VCAEngine.la
ui_VCAEngine_la_LDFLAGS = -module -avoid-version -no-undefined
ui_VCAEngine_la_SOURCES = vcaengine.cpp libwidg.cpp widget.cpp origwidg.cpp project.cpp session.cpp
ui_VCAEngine_la_LIBTOOLFLAGS = --tag=disable-static

if VCAEngineIncl
ui_VCAEngine_la_CXXFLAGS = -DMOD_INCL
install:
endif

I18N_mod = $(oscd_modulpref)VCAEngine
