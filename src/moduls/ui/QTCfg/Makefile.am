## process this file with automake to generate Makefile.in
EXTRA_DIST = *.h xpm/* po/* doc/*

AM_CXXFLAGS= -I$(QTDIR)/include -I$(top_srcdir)/src  -fPIC -DPIC
AM_LDFLAGS= -shared -module

oscd_modul_PROGRAMS = ui_QTCfg.so
ui_QTCfg_so_SOURCES = tuimod.cpp qtcfg.cpp dlguser.cpp imgview.cpp moc_qtcfg.cpp moc_imgview.cpp
ui_QTCfg_so_LDADD = -L$(QTDIR)/lib -L$(QTDIR)/lib64 -lqt-mt

moc_qtcfg.cpp: qtcfg.h
	$(QTDIR)/bin/moc qtcfg.h > moc_qtcfg.cpp

moc_imgview.cpp: imgview.h
	$(QTDIR)/bin/moc imgview.h > moc_imgview.cpp


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
	rm -f moc_qtcfg.cpp moc_imgview.cpp \
	rm -f po/$(m_name).pot ; \
	rm -f po/*~ ; 
