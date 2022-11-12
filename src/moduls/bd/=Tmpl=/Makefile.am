EXTRA_DIST = *.h po/*

# The module include-builtin to the OpenSCADA core
if TmplIncl
#  Library name of the module
noinst_LTLIBRARIES = db_Tmpl.la
db_Tmpl_la_CXXFLAGS = -DMOD_INCL -fpic
#  Libtool flags - typical
db_Tmpl_la_LIBTOOLFLAGS = --tag=disable-shared
#  Flags of linking the module
db_Tmpl_la_LDFLAGS = -module
# The module in an external shared object file - typical
else
#  Library name of the module
oscd_modul_LTLIBRARIES = db_Tmpl.la
db_Tmpl_la_CXXFLAGS =
#  Libtool flags - typical
db_Tmpl_la_LIBTOOLFLAGS = --tag=disable-static
#  Flags of linking the module
db_Tmpl_la_LDFLAGS = -module -avoid-version $(top_builddir)/src/liboscada.la
endif

# Compiler flags
db_Tmpl_la_CXXFLAGS += $(tmpl_CFLAGS)
# Linker flags
db_Tmpl_la_LDFLAGS += $(tmpl_LDLAGS)
# Source files list
db_Tmpl_la_SOURCES = module.cpp

# The module name for translation files
I18N_mod = $(oscd_modulpref)Tmpl
include ../../../../I18N.mk
