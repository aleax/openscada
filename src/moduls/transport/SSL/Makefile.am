## process this file with automake to generate Makefile.in

EXTRA_DIST = *.h po/*

INCLUDES = -I$(top_srcdir)/src

if SSLIncl
noinst_LTLIBRARIES = tr_SSL.la
INCLUDES += -DMOD_INCL
else
oscd_modul_LTLIBRARIES = tr_SSL.la
endif
tr_SSL_la_LDFLAGS = -module -avoid-version -no-undefined
tr_SSL_la_SOURCES = modssl.cpp
tr_SSL_la_LIBADD  = $(LIB_OpenSSL)
tr_SSL_la_LIBTOOLFLAGS = --tag=disable-static

if LibIntl

#PO files process
m_name = $(oscd_modulpref)SSL

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

endif