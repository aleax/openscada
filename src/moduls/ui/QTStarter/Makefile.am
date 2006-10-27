QTDIR=/usr/lib/qt4/
QTINC=/usr/lib/qt4/include/

## process this file with automake to generate Makefile.in
EXTRA_DIST = *.h xpm/* po/* doc/*

AM_CXXFLAGS= -I$(QTINC) -I$(QTINC)/QtCore -I$(QTINC)/QtGui -I$(top_srcdir)/src  -fPIC -DPIC
AM_LDFLAGS= -shared -module

oscd_modul_PROGRAMS = ui_QTStarter.so
ui_QTStarter_so_SOURCES = tuimod.cpp moc_tuimod.cpp
ui_QTStarter_so_LDADD = -L$(QTDIR)/lib -lQtGui

moc_tuimod.cpp: tuimod.h
	$(QTDIR)/bin/moc tuimod.h > moc_tuimod.cpp


#PO files process and doc install
fm_name = Mod_Tr_QTStarter
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
	install -m 755 -d $(oscd_docdir)/$(fm_name)
	install -m 644 doc/*.pdf $(oscd_docdir)/$(fm_name)

uninstall-local:
	@catalogs=`find ./po -name \*.po` ; \
	name=$(m_name).mo ; \
	for cat in $$catalogs; do \
	rm -f $$name $(oscd_localedir)/`basename $$cat .po`/LC_MESSAGES/$$name ; \
	done

clean-local:
	rm -f moc_tuimod.cpp \
	rm -f po/$(m_name).pot ; \
	rm -f po/*~ ; 
