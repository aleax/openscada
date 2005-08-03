m_name = $(oscd_modulpref)diamond

EXTRA_DIST = *.po

messages:
	@catalogs=`find ./ -name \*.po` ; \
	name=$(m_name).pot ; \
	xgettext -o $$name -C -a ../*.cpp ../*.h &> /dev/null ; \
	for cat in $$catalogs; do \
	msgmerge -U $$cat $$name ; \
	done

install:
	@catalogs=`find ./ -name \*.po` ; \
	name=$(m_name).mo ; \
	for cat in $$catalogs; do \
	mkdir -p $(oscd_localedir)/`basename $$cat .po`/LC_MESSAGES ; \
	msgfmt -vo $$name $$cat ; \
	cp $$name $(oscd_localedir)/`basename $$cat .po`/LC_MESSAGES/$$name ; \
	rm $$name ; \
	done

uninstall:
	@catalogs=`find ./ -name \*.po` ; \
	name=$(m_name).mo ; \
	for cat in $$catalogs; do \
	rm -f $$name $(oscd_localedir)/`basename $$cat .po`/LC_MESSAGES/$$name ; \
	done	

clean:
	rm -f $(m_name).pot ; \
	rm -f *~ ;
