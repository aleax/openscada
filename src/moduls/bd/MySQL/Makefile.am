## process this file with automake to generate Makefile.in

EXTRA_DIST = *.h po/* doc/*

AM_CXXFLAGS= -I$(top_srcdir)/src -fPIC -DPIC -DUSE_OLD_FUNCTIONS
AM_LDFLAGS= -shared -module

oscd_modul_PROGRAMS = bd_MySQL.so
bd_MySQL_so_SOURCES = my_sql.cpp
bd_MySQL_so_LDADD = -lmysqlclient

#PO files process and doc install
doc_name = $(oscd_docmodul)DB.MySQL
m_name = $(oscd_modulpref)MySQL

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
	rm -f po/$(m_name).pot ; \
	rm -f po/*~ ;