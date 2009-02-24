SUBDIRS = moduls

EXTRA_DIST = *.h po/*

INCLUDES = -DLOCALEDIR=\"$(oscd_localedir)\"

AM_CXXFLAGS =

lib_LTLIBRARIES =  liboscada.la
liboscada_la_SOURCES = tsys.cpp resalloc.cpp tarchives.cpp tarchval.cpp tbds.cpp tvariant.cpp tconfig.cpp \
    tcontroller.cpp tsecurity.cpp tmess.cpp tsubsys.cpp tprotocols.cpp ttransports.cpp \
    tspecials.cpp ttipdaq.cpp tprmtmpl.cpp tvalue.cpp terror.cpp tuis.cpp tmodschedul.cpp \
    telem.cpp tparamcontr.cpp tmodule.cpp xml.cpp tcntrnode.cpp tdaqs.cpp tfunction.cpp
liboscada_la_LIBADD = -ldl -lpthread -lrt -lexpat -lcrypt -lgd -lz
liboscada_la_LDFLAGS = -no-undefined -version-info $(oscd_LIBREVISION)

m_name = $(PACKAGE)

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

uninstall-local:
	@catalogs=`find ./po -name \*.po` ; \
	name=$(m_name).mo ; \
	for cat in $$catalogs; do \
	rm -f $$name $(oscd_localedir)/`basename $$cat .po`/LC_MESSAGES/$$name ; \
	done

clean-local:
	rm -f po/$(m_name).pot ; \
	rm -f po/*~ ;
