EXTRA_DIST = WebVisionVCA.js *.h po/*

if WebVisionIncl
noinst_LTLIBRARIES = ui_WebVision.la
ui_WebVision_la_CXXFLAGS = -DMOD_INCL -fpic
ui_WebVision_la_LIBTOOLFLAGS = --tag=disable-shared
ui_WebVision_la_LDFLAGS = -module
else
oscd_modul_LTLIBRARIES = ui_WebVision.la
ui_WebVision_la_LIBTOOLFLAGS = --tag=disable-static
ui_WebVision_la_LDFLAGS = -module -avoid-version $(top_builddir)/src/liboscada.la
endif

ui_WebVision_la_SOURCES = web_vision.cpp vca_sess.cpp VCAjs.cpp
ui_WebVision_la_LIBADD = $(LIB_GD) $(LIB_FFTW3)


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

I18N_mod = $(oscd_modulpref)WebVision
include ../../../../I18N.mk

clean-local:
	rm -f VCAjs.cpp
