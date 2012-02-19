EXTRA_DIST = *.h qtcfg.qrc images/* po/*

INCLUDES = -I$(top_srcdir)/src $(QT4_INCL)

if QTCfgIncl
noinst_LTLIBRARIES = ui_QTCfg.la
INCLUDES += -DMOD_INCL
else
oscd_modul_LTLIBRARIES = ui_QTCfg.la
endif
ui_QTCfg_la_LDFLAGS = -module -avoid-version -no-undefined
ui_QTCfg_la_SOURCES = tuimod.cpp qrc_qtcfg.cpp qtcfg.cpp selfwidg.cpp moc_qtcfg.cpp moc_selfwidg.cpp
ui_QTCfg_la_DIST = tuimod.cpp qtcfg.cpp selfwidg.cpp qtcfg.qrc
ui_QTCfg_la_LIBADD = $(QT4_LIB)
ui_QTCfg_la_LIBTOOLFLAGS = --tag=disable-static

qrc_qtcfg.cpp: qtcfg.qrc
	$(QT4_RCC) -name QTCfg qtcfg.qrc -o qrc_qtcfg.cpp
moc_qtcfg.cpp: qtcfg.h
	$(QT4_MOC) qtcfg.h > moc_qtcfg.cpp
moc_selfwidg.cpp: selfwidg.h
	$(QT4_MOC) selfwidg.h > moc_selfwidg.cpp

I18N_mod = $(oscd_modulpref)QTCfg

clean-local:
	rm -f qrc_qtcfg.cpp moc_qtcfg.cpp moc_selfwidg.cpp

DISTFILES = $(DIST_COMMON) $(ui_QTCfg_la_DIST) $(EXTRA_DIST)
