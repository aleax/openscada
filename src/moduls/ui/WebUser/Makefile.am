EXTRA_DIST = *.h po/*

oscd_modul_LTLIBRARIES = ui_WebUser.la
ui_WebUser_la_LDFLAGS = -module -avoid-version -no-undefined
ui_WebUser_la_SOURCES = web_user.cpp
ui_WebUser_la_LIBTOOLFLAGS = --tag=disable-static

if WebUserIncl
ui_WebUser_la_CXXFLAGS = -DMOD_INCL
install:
endif

I18N_mod = $(oscd_modulpref)WebUser
