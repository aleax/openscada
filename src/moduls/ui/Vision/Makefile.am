## process this file with automake to generate Makefile.in

EXTRA_DIST = *.h vision.qrc images/* po/* doc/*

AM_CXXFLAGS= -I$(QT4_INCL) -I$(QT4_INCL)/QtCore -I$(QT4_INCL)/QtGui -I$(top_srcdir)/src -fPIC -DPIC \
    -DQT_EDITION=QT_EDITION_DESKTOP -DQT_USE_APPROXIMATE_CURSORS -D_LARGEFILE64_SOURCE -D_LARGEFILE_SOURCE -DQT_GUI_LIB -DQT_CORE_LIB
AM_LDFLAGS= -shared -module

oscd_modul_PROGRAMS = ui_Vision.so
ui_Vision_so_SOURCES = tvision.cpp qrc_vision.cpp vis_widgs.cpp moc_vis_widgs.cpp vis_shapes.cpp moc_vis_shapes.cpp \
    vis_devel.cpp moc_vis_devel.cpp vis_devel_dlgs.cpp moc_vis_devel_dlgs.cpp vis_devel_widgs.cpp moc_vis_devel_widgs.cpp \
    vis_run.cpp moc_vis_run.cpp vis_run_widgs.cpp moc_vis_run_widgs.cpp vis_shape_elfig.cpp moc_vis_shape_elfig.cpp
ui_Vision_so_LDADD = -L$(QT4_LIB) -lQtGui

qrc_vision.cpp: vision.qrc
	$(QT4_RCC) -name Vision vision.qrc -o qrc_vision.cpp
moc_vis_devel.cpp: vis_devel.h
	$(QT4_MOC) vis_devel.h > moc_vis_devel.cpp
moc_vis_devel_widgs.cpp: vis_devel_widgs.h
	$(QT4_MOC) vis_devel_widgs.h > moc_vis_devel_widgs.cpp
moc_vis_devel_dlgs.cpp: vis_devel_dlgs.h
	$(QT4_MOC) vis_devel_dlgs.h > moc_vis_devel_dlgs.cpp
moc_vis_widgs.cpp: vis_widgs.h
	$(QT4_MOC) vis_widgs.h > moc_vis_widgs.cpp
moc_vis_run.cpp: vis_run.h
	$(QT4_MOC) vis_run.h > moc_vis_run.cpp
moc_vis_run_widgs.cpp: vis_run_widgs.h
	$(QT4_MOC) vis_run_widgs.h > moc_vis_run_widgs.cpp
moc_vis_shapes.cpp: vis_shapes.h
	$(QT4_MOC) vis_shapes.h > moc_vis_shapes.cpp
moc_vis_shape_elfig.cpp: vis_shape_elfig.h
	$(QT4_MOC) vis_shape_elfig.h > moc_vis_shape_elfig.cpp

#PO files process and doc install
doc_name = $(oscd_docmodul)UI.Vision
m_name = $(oscd_modulpref)Vision

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
	rm -f qrc_vision.cpp moc_vis_widgs.cpp moc_vis_devel.cpp moc_vis_devel_widgs.cpp moc_vis_devel_dlgs.cpp \
    moc_vis_run.cpp moc_vis_run_widgs.cpp moc_vis_shapes.cpp moc_vis_shape_elfig.cpp po/$(m_name).pot po/*~
