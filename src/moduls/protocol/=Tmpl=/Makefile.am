## process this file with automake to generate Makefile.in

EXTRA_DIST = *.h po/* doc/*

INCLUDES = -I$(top_srcdir)/src

#!!! The module result library name
oscd_modul_LTLIBRARIES = prot_Tmpl.la
#!!! The module link flags
prot_Tmpl_la_LDFLAGS = -module -avoid-version -no-undefined
#!!! The module source files list
prot_Tmpl_la_SOURCES = mod_tmpl.cpp
#!!! The module depending libraryes list
#prot_Tmpl_la_LIBADD  = -lnetsnmp -lcrypto

#PO files process and doc install
#!!! The module documents directory
doc_name = $(oscd_docmodul)Protocol.Tmpl
#!!! The module name for translation files
m_name = $(oscd_modulpref)Tmpl

#!!! Standard code for translations and documentations processing. Don't change it!
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