## process this file with automake to generate Makefile.in

EXTRA_DIST = *.h qtstarter.qrc images/* po/* doc/*

INCLUDES = -I$(top_srcdir)/src -I$(QT4_INCL) -I$(QT4_INCL)/QtCore -I$(QT4_INCL)/QtGui -D_REENTRANT -DQT_NO_DEBUG -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED

oscd_modul_LTLIBRARIES = ui_QTStarter.la
ui_QTStarter_la_LDFLAGS = -module -avoid-version -no-undefined
ui_QTStarter_la_SOURCES = tuimod.cpp moc_tuimod.cpp qrc_qtstarter.cpp
ui_QTStarter_la_LIBADD = -L$(QT4_LIB) -lQtCore -lQtGui

qrc_qtstarter.cpp: qtstarter.qrc
	$(QT4_RCC) -name QTStarter qtstarter.qrc -o qrc_qtstarter.cpp
moc_tuimod.cpp: tuimod.h
	$(QT4_MOC) tuimod.h > moc_tuimod.cpp

#PO files process and doc install
doc_name = $(oscd_docmodul)UI.QTStarter
m_name = $(oscd_modulpref)QTStarter

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
	install -m 755 -d $(doc_name)
	install -m 644 doc/*.pdf $(doc_name)

uninstall-local:
	@catalogs=`find ./po -name \*.po` ; \
	name=$(m_name).mo ; \
	for cat in $$catalogs; do \
	rm -f $$name $(oscd_localedir)/`basename $$cat .po`/LC_MESSAGES/$$name ; \
	done

clean-local:
	rm -f qrc_qtstarter.cpp moc_tuimod.cpp po/$(m_name).pot po/*~
