## process this file with automake to generate Makefile.in

EXTRA_DIST = *.h vision.qrc images/* po/*

INCLUDES = -I$(top_srcdir)/src $(QT4_INCL)

if VisionIncl
noinst_LTLIBRARIES = ui_Vision.la
INCLUDES += -DMOD_INCL
else
oscd_modul_LTLIBRARIES = ui_Vision.la
endif
ui_Vision_la_LDFLAGS = -module -avoid-version -no-undefined
ui_Vision_la_SOURCES = tvision.cpp qrc_vision.cpp vis_widgs.cpp moc_vis_widgs.cpp vis_shapes.cpp moc_vis_shapes.cpp \
    vis_devel.cpp moc_vis_devel.cpp vis_devel_dlgs.cpp moc_vis_devel_dlgs.cpp vis_devel_widgs.cpp moc_vis_devel_widgs.cpp \
    vis_run.cpp moc_vis_run.cpp vis_run_widgs.cpp moc_vis_run_widgs.cpp vis_shape_elfig.cpp moc_vis_shape_elfig.cpp
ui_Vision_la_DIST = tvision.cpp vis_widgs.cpp vis_shapes.cpp vis_devel.cpp vis_devel_dlgs.cpp vis_devel_widgs.cpp \
    vis_run.cpp vis_run_widgs.cpp vis_shape_elfig.cpp vision.qrc
ui_Vision_la_LIBADD = $(QT4_LIB) $(LIB_FFTW3)
ui_Vision_la_LIBTOOLFLAGS = --tag=disable-static

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

if LibIntl

#PO files process
m_name = $(oscd_modulpref)Vision

messages:
	@catalogs=`find ./po -name \*.po` ; \
	name=po/$(m_name).pot ; \
	xgettext -o $$name -C --no-location --no-wrap -k_ *.cpp *.h ; \
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
	rm -f qrc_vision.cpp moc_vis_widgs.cpp moc_vis_devel.cpp moc_vis_devel_widgs.cpp moc_vis_devel_dlgs.cpp \
    moc_vis_run.cpp moc_vis_run_widgs.cpp moc_vis_shapes.cpp moc_vis_shape_elfig.cpp po/$(m_name).pot po/*~

endif

DISTFILES = $(DIST_COMMON) $(ui_Vision_la_DIST) $(EXTRA_DIST)
