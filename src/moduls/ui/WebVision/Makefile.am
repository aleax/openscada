## process this file with automake to generate Makefile.in

EXTRA_DIST = WebVisionVCA.js *.h po/* doc/*

INCLUDES = -I$(top_srcdir)/src

oscd_modul_LTLIBRARIES = ui_WebVision.la
ui_WebVision_la_LDFLAGS = -module -avoid-version -no-undefined
ui_WebVision_la_SOURCES = web_vision.cpp vca_sess.cpp VCAjs.cpp
ui_WebVision_la_LIBADD = -lfftw3
ui_WebVision_la_LIBTOOLFLAGS = --tag=disable-static

VCAjs.cpp: WebVisionVCA.js
	@lc=`cat $< | wc -l` ; \
	il=1 ; \
	echo "const char *`echo "$<" | sed 's/\./_/g'`=" > $@ ; \
	while [ $$il -le $$lc ] ; do \
	  l="   \"`cat $< | sed -n $${il}p | sed 's/\\\\/\\\\\\\\/g' | sed 's/"/\\\\"/g'`\\n\""; \
	  if [ $$il -eq $$lc ]; then l="$$l;"; fi; \
	  echo "$$l" >> $@ ; \
	  let il=il+1 ; \
	done

#PO files process and doc install
doc_name = $(oscd_docmodul)UI.WebVision
m_name = $(oscd_modulpref)WebVision

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
	rm -f VCAjs.cpp po/$(m_name).pot po/*~
