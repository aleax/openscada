EXTRA_DIST = *.h po/*

if LDAPIncl
noinst_LTLIBRARIES = bd_LDAP.la
bd_LDAP_la_CXXFLAGS = -DMOD_INCL -fpic
bd_LDAP_la_LIBTOOLFLAGS = --tag=disable-shared
bd_LDAP_la_LDFLAGS = -module
else
oscd_modul_LTLIBRARIES = bd_LDAP.la
bd_LDAP_la_CXXFLAGS =
bd_LDAP_la_LIBTOOLFLAGS = --tag=disable-static
bd_LDAP_la_LDFLAGS = -module -avoid-version $(top_builddir)/src/liboscada.la
endif

bd_LDAP_la_CXXFLAGS += -DLDAP_DEPRECATED
bd_LDAP_la_LDFLAGS += -lldap
bd_LDAP_la_SOURCES = mod_ldap.cpp


I18N_mod = $(oscd_modulpref)LDAP
include ../../../../I18N.mk
