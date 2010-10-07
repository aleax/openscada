## process this file with automake to generate Makefile.in

EXTRA_DIST = *.h po/* doc/*

INCLUDES = -I$(top_srcdir)/src

oscd_modul_LTLIBRARIES = daq_OPC_UA.la
daq_OPC_UA_la_LDFLAGS = -module -avoid-version -no-undefined
daq_OPC_UA_la_SOURCES = moduls.cpp mod_daq.cpp mod_prt.cpp
daq_OPC_UA_la_LIBADD  = -lcrypto
daq_OPC_UA_la_LIBTOOLFLAGS = --tag=disable-static

#PO files process and doc install
doc_name = $(oscd_docmodul)DAQ.OPC_UA
m_name = $(oscd_modulpref)OPC_UA

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