EXTRA_DIST = *.h po/*

if WebUserIncl
noinst_LTLIBRARIES = ui_WebUser.la
ui_WebUser_la_CXXFLAGS = -DMOD_INCL -fpic
ui_WebUser_la_LIBTOOLFLAGS = --tag=disable-shared
ui_WebUser_la_LDFLAGS = -module
else
oscd_modul_LTLIBRARIES = ui_WebUser.la
ui_WebUser_la_LIBTOOLFLAGS = --tag=disable-static
ui_WebUser_la_LDFLAGS = -module -avoid-version $(top_builddir)/src/liboscada.la
endif

ui_WebUser_la_SOURCES = web_user.cpp


I18N_mod = $(oscd_modulpref)WebUser
include ../../../../I18N.mk
