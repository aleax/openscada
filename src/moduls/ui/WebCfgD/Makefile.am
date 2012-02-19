EXTRA_DIST = WebCfgDVCA.js WebCfgDVCA.html *.h po/*

INCLUDES = -I$(top_srcdir)/src

if WebCfgDIncl
noinst_LTLIBRARIES = ui_WebCfgD.la
INCLUDES += -DMOD_INCL
else
oscd_modul_LTLIBRARIES = ui_WebCfgD.la
endif
ui_WebCfgD_la_LDFLAGS = -module -avoid-version -no-undefined
ui_WebCfgD_la_SOURCES = web_cfg.cpp VCAjs.cpp VCAhtml.cpp
ui_WebCfgD_la_LIBADD = -lgd
ui_WebCfgD_la_LIBTOOLFLAGS = --tag=disable-static

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
