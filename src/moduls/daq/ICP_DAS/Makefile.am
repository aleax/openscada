## process this file with automake to generate Makefile.in

EXTRA_DIST = *.h po/* doc/*

INCLUDES = -I$(top_srcdir)/src -I./include

oscd_modul_LTLIBRARIES = daq_ICP_DAS.la
daq_ICP_DAS_la_LDFLAGS = -module -avoid-version -no-undefined
daq_ICP_DAS_la_SOURCES = ICP_module.cpp
daq_ICP_DAS_la_LIBADD  = -L./ -li8k

#PO files process and doc install
doc_name = $(oscd_docmodul)DAQ.ICP_DAS
m_name = $(oscd_modulpref)ICP_DAS

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