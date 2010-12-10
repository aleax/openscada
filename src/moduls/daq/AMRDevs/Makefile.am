EXTRA_DIST = *.h po/*
# doc/*

INCLUDES = -I$(top_srcdir)/src

oscd_modul_LTLIBRARIES = daq_AMRDevs.la
daq_AMRDevs_la_LDFLAGS = -module -avoid-version -no-undefined
daq_AMRDevs_la_SOURCES = mod_amr.cpp da_FlowTEC.cpp da_Ergomera.cpp
#daq_AMRdevs_la_LIBADD  = -lnetsnmp -lcrypto
daq_AMRDevs_la_LIBTOOLFLAGS = --tag=disable-static

#PO files process and doc install
doc_name = $(oscd_docmodul)DAQ.AMRDevs
m_name = $(oscd_modulpref)AMRDevs

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
#	install -m 755 -d $(doc_name)
#	install -m 644 doc/*.pdf $(doc_name)

uninstall-local:
	@catalogs=`find ./po -name \*.po` ; \
	name=$(m_name).mo ; \
	for cat in $$catalogs; do \
	rm -f $$name $(oscd_localedir)/`basename $$cat .po`/LC_MESSAGES/$$name ; \
	done

clean-local:
	rm -f po/$(m_name).pot ; \
	rm -f po/*~ ;