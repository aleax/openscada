EXTRA_DIST = *.h qtstarter.qrc images/* po/*

if QTStarterIncl
noinst_LTLIBRARIES = ui_QTStarter.la
ui_QTStarter_la_CXXFLAGS = -DMOD_INCL -fpic
ui_QTStarter_la_LIBTOOLFLAGS = --tag=disable-shared
ui_QTStarter_la_LDFLAGS = -module
else
oscd_modul_LTLIBRARIES = ui_QTStarter.la
ui_QTStarter_la_CXXFLAGS =
ui_QTStarter_la_LIBTOOLFLAGS = --tag=disable-static
ui_QTStarter_la_LDFLAGS = -module -avoid-version $(top_builddir)/src/liboscada.la
endif

ui_QTStarter_la_CXXFLAGS += $(QtGui_CFLAGS)
ui_QTStarter_la_SOURCES = tuimod.cpp moc_tuimod.cpp qrc_qtstarter.cpp
ui_QTStarter_la_DIST = tuimod.cpp qtstarter.qrc
ui_QTStarter_la_LIBADD = $(QtGui_LIBS)


qrc_qtstarter.cpp: qtstarter.qrc
	$(Qt_RCC) -name QTStarter qtstarter.qrc -o qrc_qtstarter.cpp
moc_tuimod.cpp: tuimod.h
	$(Qt_MOC) tuimod.h > moc_tuimod.cpp

I18N_mod = $(oscd_modulpref)QTStarter
include ../../../../I18N.mk

clean-local:
	rm -f qrc_qtstarter.cpp moc_tuimod.cpp

DISTFILES = $(DIST_COMMON) $(ui_QTStarter_la_DIST) $(EXTRA_DIST)
