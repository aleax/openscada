EXTRA_DIST = *.h po/*

INCLUDES = -I$(top_srcdir)/src

if WebUserIncl
noinst_LTLIBRARIES = ui_WebUser.la
INCLUDES += -DMOD_INCL
else
oscd_modul_LTLIBRARIES = ui_WebUser.la
endif
ui_WebUser_la_LDFLAGS = -module -avoid-version -no-undefined
ui_WebUser_la_SOURCES = web_user.cpp
ui_WebUser_la_LIBTOOLFLAGS = --tag=disable-static

I18N_mod = $(oscd_modulpref)WebUser
