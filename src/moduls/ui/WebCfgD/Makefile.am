EXTRA_DIST = WebCfgDVCA.js WebCfgDVCA.html *.h po/*

oscd_modul_LTLIBRARIES = ui_WebCfgD.la
ui_WebCfgD_la_LDFLAGS = -module -avoid-version -no-undefined
ui_WebCfgD_la_SOURCES = web_cfg.cpp VCAjs.cpp VCAhtml.cpp
ui_WebCfgD_la_LIBADD = $(LIB_GD)
ui_WebCfgD_la_LIBTOOLFLAGS = --tag=disable-static

if WebCfgDIncl
ui_WebCfgD_la_CXXFLAGS = -DMOD_INCL
install:
endif

VCAjs.cpp: WebCfgDVCA.js
	@lc=`cat $< | wc -l` ; \
	il=1 ; \
	echo "const char *`echo "$<" | sed 's/\./_/g'`=" > $@ ; \
	while [ $$il -le $$lc ] ; do \
	  l="   \"`cat $< | sed -n $${il}p | sed 's/\\\\/\\\\\\\\/g' | sed 's/"/\\\\"/g'`\\n\""; \
	  if [ $$il -eq $$lc ]; then l="$$l;"; fi; \
	  echo "$$l" >> $@ ; \
	  let il=il+1 ; \
	done

VCAhtml.cpp: WebCfgDVCA.html
	@lc=`cat $< | wc -l` ; \
	il=1 ; \
	echo "const char *`echo "$<" | sed 's/\./_/g'`=" > $@ ; \
	while [ $$il -le $$lc ] ; do \
	  l="   \"`cat $< | sed -n $${il}p | sed 's/\\\\/\\\\\\\\/g' | sed 's/"/\\\\"/g'`\\n\""; \
	  if [ $$il -eq $$lc ]; then l="$$l;"; fi; \
	  echo "$$l" >> $@ ; \
	  let il=il+1 ; \
	done

I18N_mod = $(oscd_modulpref)WebCfgD

clean-local:
	rm -f VCAhtml.cpp VCAjs.cpp
