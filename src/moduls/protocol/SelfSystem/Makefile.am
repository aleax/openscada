EXTRA_DIST = *.h po/*

INCLUDES = -I$(top_srcdir)/src

if SelfSystemIncl
noinst_LTLIBRARIES = prot_SelfSystem.la
INCLUDES += -DMOD_INCL
else
oscd_modul_LTLIBRARIES = prot_SelfSystem.la
endif
prot_SelfSystem_la_LDFLAGS = -module -avoid-version -no-undefined
prot_SelfSystem_la_SOURCES = self.cpp
prot_SelfSystem_la_LIBTOOLFLAGS = --tag=disable-static

I18N_mod = $(oscd_modulpref)SelfSystem
