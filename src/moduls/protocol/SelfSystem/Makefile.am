EXTRA_DIST = *.h po/*

oscd_modul_LTLIBRARIES = prot_SelfSystem.la
prot_SelfSystem_la_LDFLAGS = -module -avoid-version -no-undefined
prot_SelfSystem_la_SOURCES = self.cpp
prot_SelfSystem_la_LIBTOOLFLAGS = --tag=disable-static

if SelfSystemIncl
prot_SelfSystem_la_CXXFLAGS = -DMOD_INCL
install:
endif

I18N_mod = $(oscd_modulpref)SelfSystem
