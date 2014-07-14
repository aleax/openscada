EXTRA_DIST = *.h po/*

if SelfSystemIncl
noinst_LTLIBRARIES = prot_SelfSystem.la
prot_SelfSystem_la_CXXFLAGS = -DMOD_INCL -fpic
prot_SelfSystem_la_LIBTOOLFLAGS = --tag=disable-shared
prot_SelfSystem_la_LDFLAGS = -module
else
oscd_modul_LTLIBRARIES = prot_SelfSystem.la
prot_SelfSystem_la_LIBTOOLFLAGS = --tag=disable-static
prot_SelfSystem_la_LDFLAGS = -module -avoid-version $(top_builddir)/src/liboscada.la
endif

prot_SelfSystem_la_SOURCES = self.cpp


I18N_mod = $(oscd_modulpref)SelfSystem
include ../../../../I18N.mk
