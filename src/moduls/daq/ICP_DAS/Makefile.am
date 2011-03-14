## process this file with automake to generate Makefile.in

EXTRA_DIST = *.h po/* libi8k.a include/*.h

INCLUDES = -I$(top_srcdir)/src -I./include

oscd_modul_LTLIBRARIES = daq_ICP_DAS.la
daq_ICP_DAS_la_LDFLAGS = -module -avoid-version -no-undefined
daq_ICP_DAS_la_SOURCES = ICP_module.cpp
daq_ICP_DAS_la_LIBADD  = -L./ -li8k
daq_ICP_DAS_la_LIBTOOLFLAGS = --tag=disable-static

#PO files process
m_name = $(oscd_modulpref)ICP_DAS

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
	rm -f po/$(m_name).pot ; \
	rm -f po/*~ ;
