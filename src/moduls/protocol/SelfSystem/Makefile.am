EXTRA_DIST = *.h po/*

if SelfSystemIncl
noinst_LTLIBRARIES = prot_SelfSystem.la
prot_SelfSystem_la_CXXFLAGS = -DMOD_INCL -fpic
prot_SelfSystem_la_LIBTOOLFLAGS = --tag=disable-shared
else
oscd_modul_LTLIBRARIES = prot_SelfSystem.la
prot_SelfSystem_la_LIBTOOLFLAGS = --tag=disable-static
endif

prot_SelfSystem_la_LDFLAGS = -module -avoid-version -no-undefined
prot_SelfSystem_la_SOURCES = self.cpp


I18N_mod = $(oscd_modulpref)SelfSystem
include ../../../../I18N.mk
