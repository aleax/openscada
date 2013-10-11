I18N: messages

messages:
	@catalogs=`find ./po -name \*.po` ; \
	name=po/$(I18N_mod).pot ; \
	xgettext -o $$name -C --no-wrap -k_ *.{cpp,h} $(I18N_addfiles) ; \
	if test "x$(I18N_add1)" != "x"; then xgettext -o $$name -j --no-wrap $(I18N_add1); fi; \
	for cat in $$catalogs; do \
	  msgmerge -U $$cat $$name ; \
	done

install-I18N:
	@catalogs=`find ./po -name \*.po` ; \
	name=$(I18N_mod).mo ; \
	for cat in $$catalogs; do \
	  mkdir -p $(DESTDIR)$(localedir)/`basename $$cat .po`/LC_MESSAGES ; \
	  msgfmt -vo $$name $$cat ; \
	  cp $$name $(DESTDIR)$(localedir)/`basename $$cat .po`/LC_MESSAGES/$$name ; \
	  rm $$name ; \
	done

uninstall-I18N:
	@catalogs=`find ./po -name \*.po` ; \
	name=$(I18N_mod).mo ; \
	for cat in $$catalogs; do \
	  rm -f $$name $(DESTDIR)$(localedir)/`basename $$cat .po`/LC_MESSAGES/$$name ; \
	done

clean-I18N:
	rm -f po/$(I18N_mod).pot po/*~ ;

install-data-local: install-I18N
uninstall-local: uninstall-I18N
clean-local: clean-I18N
