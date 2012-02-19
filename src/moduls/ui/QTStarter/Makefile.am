EXTRA_DIST = *.h qtstarter.qrc images/* po/*

INCLUDES = -I$(top_srcdir)/src $(QT4_INCL)

if QTStarterIncl
noinst_LTLIBRARIES = ui_QTStarter.la
INCLUDES += -DMOD_INCL
else
oscd_modul_LTLIBRARIES = ui_QTStarter.la
endif
ui_QTStarter_la_LDFLAGS = -module -avoid-version -no-undefined
ui_QTStarter_la_SOURCES = tuimod.cpp moc_tuimod.cpp qrc_qtstarter.cpp
ui_QTStarter_la_DIST = tuimod.cpp qtstarter.qrc
ui_QTStarter_la_LIBADD = $(QT4_LIB)
ui_QTStarter_la_LIBTOOLFLAGS = --tag=disable-static

qrc_qtstarter.cpp: qtstarter.qrc
	$(QT4_RCC) -name QTStarter qtstarter.qrc -o qrc_qtstarter.cpp
moc_tuimod.cpp: tuimod.h
	$(QT4_MOC) tuimod.h > moc_tuimod.cpp

I18N_mod = $(oscd_modulpref)QTStarter

clean-local:
	rm -f qrc_qtstarter.cpp moc_tuimod.cpp

DISTFILES = $(DIST_COMMON) $(ui_QTStarter_la_DIST) $(EXTRA_DIST)
