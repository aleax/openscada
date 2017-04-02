EXTRA_DIST = WebCfgDVCA.js WebCfgDVCA.html *.h po/*

if WebCfgDIncl
noinst_LTLIBRARIES = ui_WebCfgD.la
ui_WebCfgD_la_CXXFLAGS = -DMOD_INCL -fpic
ui_WebCfgD_la_LIBTOOLFLAGS = --tag=disable-shared
ui_WebCfgD_la_LDFLAGS = -module
else
oscd_modul_LTLIBRARIES = ui_WebCfgD.la
ui_WebCfgD_la_CXXFLAGS =
ui_WebCfgD_la_LIBTOOLFLAGS = --tag=disable-static
ui_WebCfgD_la_LDFLAGS = -module -avoid-version $(top_builddir)/src/liboscada.la
endif

ui_WebCfgD_la_CXXFLAGS += $(gdlib_CFLAGS)
ui_WebCfgD_la_LDFLAGS += $(gdlib_LIBS)
ui_WebCfgD_la_SOURCES = web_cfg.cpp VCAjs.cpp VCAhtml.cpp

VCAjs.cpp: WebCfgDVCA.js
	@lc=`cat $< | wc -l`; let lc=lc+1; \
	il=1 ; \
	echo "const char *`echo "$<" | sed 's/\./_/g'`=" > $@ ; \
	while [ $$il -le $$lc ] ; do \
	  l="   \"`cat $< | sed -n $${il}p | sed 's/\\\\/\\\\\\\\/g' | sed 's/"/\\\\"/g'`\\n\""; \
	  if [ $$il -eq $$lc ]; then l="$$l;"; fi; \
	  echo "$$l" >> $@ ; \
	  let il=il+1 ; \
	done

VCAhtml.cpp: WebCfgDVCA.html
	@lc=`cat $< | wc -l`; let lc=lc+1; \
	il=1 ; \
	echo "const char *`echo "$<" | sed 's/\./_/g'`=" > $@ ; \
	while [ $$il -le $$lc ] ; do \
	  l="   \"`cat $< | sed -n $${il}p | sed 's/\\\\/\\\\\\\\/g' | sed 's/"/\\\\"/g'`\\n\""; \
	  if [ $$il -eq $$lc ]; then l="$$l;"; fi; \
	  echo "$$l" >> $@ ; \
	  let il=il+1 ; \
	done

I18N_mod = $(oscd_modulpref)WebCfgD
include ../../../../I18N.mk

clean-local:
	rm -f VCAhtml.cpp VCAjs.cpp
