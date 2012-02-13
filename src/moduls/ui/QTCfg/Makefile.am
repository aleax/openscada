## process this file with automake to generate Makefile.in

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

if LibIntl

#PO files process
m_name = $(oscd_modulpref)QTCfg

messages:
	@catalogs=`find ./po -name \*.po` ; \
	name=po/$(m_name).pot ; \
	xgettext -o $$name -C --no-location --no-wrap -k_ *.cpp *.h ; \
	for cat in $$catalogs; do \
	msgmerge -U $$cat $$name ; \
	done

install-data-local:
	@catalogs=`find ./po -name \*.po` ; \
	name=$(m_name).mo ; \
	for cat in $$catalogs; do \
	mkdir -p $(oscd_localedir)/`basename $$cat .po`/LC_MESSAGES ; \
	msgfmt -vo $$name $$cat ; \
	cp $$name $(oscd_localedir)/`basename $$cat .po`/LC_MESSAGES/$$name ; \
	rm $$name ; \
	done

uninstall-local:
	@catalogs=`find ./po -name \*.po` ; \
	name=$(m_name).mo ; \
	for cat in $$catalogs; do \
	rm -f $$name $(oscd_localedir)/`basename $$cat .po`/LC_MESSAGES/$$name ; \
	done

clean-local:
	rm -f qrc_qtcfg.cpp moc_qtcfg.cpp moc_selfwidg.cpp po/$(m_name).pot po/*~

endif

DISTFILES = $(DIST_COMMON) $(ui_QTCfg_la_DIST) $(EXTRA_DIST)
