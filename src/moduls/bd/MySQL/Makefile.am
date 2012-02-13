## process this file with automake to generate Makefile.in

EXTRA_DIST = *.h po/*

INCLUDES = -I$(top_srcdir)/src -DUSE_OLD_FUNCTIONS $(MYSQL_CFLAGS)

if MySQLIncl
noinst_LTLIBRARIES = bd_MySQL.la
INCLUDES += -DMOD_INCL
else
oscd_modul_LTLIBRARIES = bd_MySQL.la
endif
bd_MySQL_la_LDFLAGS = -module -avoid-version -no-undefined $(MYSQL_LDFLAGS)
bd_MySQL_la_SOURCES = my_sql.cpp
bd_MySQL_la_LIBTOOLFLAGS = --tag=disable-static

if LibIntl

#PO files process
m_name = $(oscd_modulpref)MySQL

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