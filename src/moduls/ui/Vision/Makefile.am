EXTRA_DIST = *.h vision.qrc images/* po/*

oscd_modul_LTLIBRARIES = ui_Vision.la
ui_Vision_la_CXXFLAGS = $(QT4_INCL) $(phonon_CFLAGS)
ui_Vision_la_LDFLAGS = -module -avoid-version -no-undefined
ui_Vision_la_SOURCES = tvision.cpp qrc_vision.cpp vis_widgs.cpp moc_vis_widgs.cpp vis_shapes.cpp moc_vis_shapes.cpp \
    vis_devel.cpp moc_vis_devel.cpp vis_devel_dlgs.cpp moc_vis_devel_dlgs.cpp vis_devel_widgs.cpp moc_vis_devel_widgs.cpp \
    vis_run.cpp moc_vis_run.cpp vis_run_widgs.cpp moc_vis_run_widgs.cpp vis_shape_elfig.cpp moc_vis_shape_elfig.cpp
ui_Vision_la_DIST = tvision.cpp vis_widgs.cpp vis_shapes.cpp vis_devel.cpp vis_devel_dlgs.cpp vis_devel_widgs.cpp \
    vis_run.cpp vis_run_widgs.cpp vis_shape_elfig.cpp vision.qrc
ui_Vision_la_LIBADD = $(QT4_LIB) $(LIB_FFTW3) $(phonon_LIBS)
ui_Vision_la_LIBTOOLFLAGS = --tag=disable-static

if VisionIncl
ui_Vision_la_CXXFLAGS += -DMOD_INCL
install:
endif

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

I18N_mod = $(oscd_modulpref)Vision

clean-local:
	rm -f qrc_vision.cpp moc_vis_widgs.cpp moc_vis_devel.cpp moc_vis_devel_widgs.cpp moc_vis_devel_dlgs.cpp \
    moc_vis_run.cpp moc_vis_run_widgs.cpp moc_vis_shapes.cpp moc_vis_shape_elfig.cpp

DISTFILES = $(DIST_COMMON) $(ui_Vision_la_DIST) $(EXTRA_DIST)
