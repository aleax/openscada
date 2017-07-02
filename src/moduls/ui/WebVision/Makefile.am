EXTRA_DIST = WebVisionVCA.html WebVisionVCA.js *.h po/*

oscd_modul_LTLIBRARIES = ui_WebVision.la
ui_WebVision_la_CXXFLAGS = $(gdlib_CFLAGS)
ui_WebVision_la_LDFLAGS = -module -avoid-version -no-undefined $(gdlib_LIBS) $(LIB_FFTW3)
ui_WebVision_la_SOURCES = web_vision.cpp vca_sess.cpp VCAjs.cpp VCAhtml.cpp
ui_WebVision_la_LIBTOOLFLAGS = --tag=disable-static

if WebVisionIncl
ui_WebVision_la_CXXFLAGS += -DMOD_INCL
install:
endif

VCAjs.cpp: WebVisionVCA.js
	@lc=`cat $< | wc -l`; let lc=lc+1; \
	il=1 ; \
	echo "const char *`echo "$<" | sed 's/\./_/g'`=" > $@ ; \
	while [ $$il -le $$lc ] ; do \
	  l="   \"`cat $< | sed -n $${il}p | sed 's/\\\\/\\\\\\\\/g' | sed 's/"/\\\\"/g'`\\n\""; \
	  if [ $$il -eq $$lc ]; then l="$$l;"; fi; \
	  echo "$$l" >> $@ ; \
	  let il=il+1 ; \
	done

VCAhtml.cpp: WebVisionVCA.html
	@lc=`cat $< | wc -l`; let lc=lc+1; \
	il=1 ; \
	echo "const char *`echo "$<" | sed 's/\./_/g'`=" > $@ ; \
	while [ $$il -le $$lc ] ; do \
	  l="   \"`cat $< | sed -n $${il}p | sed 's/\\\\/\\\\\\\\/g' | sed 's/"/\\\\"/g'`\\n\""; \
	  if [ $$il -eq $$lc ]; then l="$$l;"; fi; \
	  echo "$$l" >> $@ ; \
	  let il=il+1 ; \
	done

I18N_mod = $(oscd_modulpref)WebVision
include ../../../../I18N.mk

clean-local:
	rm -f VCAjs.cpp VCAhtml.cpp
