EXTRA_DIST = *.h qtcfg.qrc images/* po/*

if QTCfgIncl
noinst_LTLIBRARIES = ui_QTCfg.la
ui_QTCfg_la_CXXFLAGS = -DMOD_INCL -fpic
ui_QTCfg_la_LIBTOOLFLAGS = --tag=disable-shared
else
oscd_modul_LTLIBRARIES = ui_QTCfg.la
ui_QTCfg_la_CXXFLAGS =
ui_QTCfg_la_LIBTOOLFLAGS = --tag=disable-static
endif

ui_QTCfg_la_CXXFLAGS += $(QtGui_CFLAGS)
ui_QTCfg_la_LDFLAGS = -module -avoid-version -no-undefined
ui_QTCfg_la_SOURCES = tuimod.cpp qrc_qtcfg.cpp qtcfg.cpp selfwidg.cpp moc_qtcfg.cpp moc_selfwidg.cpp
ui_QTCfg_la_DIST = tuimod.cpp qtcfg.cpp selfwidg.cpp qtcfg.qrc
ui_QTCfg_la_LIBADD = $(QtGui_LIBS)


qrc_qtcfg.cpp: qtcfg.qrc
	$(Qt_RCC) -name QTCfg qtcfg.qrc -o qrc_qtcfg.cpp
moc_qtcfg.cpp: qtcfg.h
	$(Qt_MOC) qtcfg.h > moc_qtcfg.cpp
moc_selfwidg.cpp: selfwidg.h
	$(Qt_MOC) selfwidg.h > moc_selfwidg.cpp

I18N_mod = $(oscd_modulpref)QTCfg
include ../../../../I18N.mk

clean-local:
	rm -f qrc_qtcfg.cpp moc_qtcfg.cpp moc_selfwidg.cpp

DISTFILES = $(DIST_COMMON) $(ui_QTCfg_la_DIST) $(EXTRA_DIST)
