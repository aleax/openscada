QTINC=$(oldincludedir)/qt4

## process this file with automake to generate Makefile.in
EXTRA_DIST = *.h qtcfg.qrc images/* po/* doc/*

AM_CXXFLAGS= -I$(QTINC) -I$(QTINC)/QtCore -I$(QTINC)/QtGui -I$(top_srcdir)/src  -fPIC -DPIC -DQT_EDITION=QT_EDITION_DESKTOP \
    -DQT_USE_APPROXIMATE_CURSORS -D_LARGEFILE64_SOURCE -D_LARGEFILE_SOURCE -DQT_GUI_LIB -DQT_CORE_LIB
AM_LDFLAGS= -shared -module

oscd_modul_PROGRAMS = ui_QTCfg.so
ui_QTCfg_so_SOURCES = tuimod.cpp qrc_qtcfg.cpp qtcfg.cpp selfwidg.cpp moc_qtcfg.cpp moc_selfwidg.cpp
ui_QTCfg_so_LDADD = -lQtGui

qrc_qtcfg.cpp: qtcfg.qrc
	$(QT4_RCC) -name QTCfg qtcfg.qrc -o qrc_qtcfg.cpp
moc_qtcfg.cpp: qtcfg.h
	$(QT4_MOC) qtcfg.h > moc_qtcfg.cpp
moc_selfwidg.cpp: selfwidg.h
	$(QT4_MOC) selfwidg.h > moc_selfwidg.cpp


#PO files process and doc install
fm_name = Mod_UI_QTCfg
m_name = $(oscd_modulpref)QTCfg

messages:
	@catalogs=`find ./po -name \*.po` ; \
	name=po/$(m_name).pot ; \
	xgettext -o $$name -C -a *.cpp *.h &> /dev/null ; \
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
	install -m 755 -d $(oscd_docdir)/$(fm_name)
	install -m 644 doc/*.pdf $(oscd_docdir)/$(fm_name)

uninstall-local:
	@catalogs=`find ./po -name \*.po` ; \
	name=$(m_name).mo ; \
	for cat in $$catalogs; do \
	rm -f $$name $(oscd_localedir)/`basename $$cat .po`/LC_MESSAGES/$$name ; \
	done

clean-local:
	rm -f qrc_qtcfg.cpp moc_qtcfg.cpp moc_selfwidg.cpp po/$(m_name).pot po/*~
