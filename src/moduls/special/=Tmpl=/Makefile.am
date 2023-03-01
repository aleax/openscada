EXTRA_DIST = *.h po/*

# The module include-builtin to the OpenSCADA core
if TmplIncl
#  Library name of the module
noinst_LTLIBRARIES = spec_Tmpl.la
spec_Tmpl_la_CXXFLAGS = -DMOD_INCL -fpic
#  Libtool flags - typical
spec_Tmpl_la_LIBTOOLFLAGS = --tag=disable-shared
#  Flags of linking the module
spec_Tmpl_la_LDFLAGS = -module
# The module in an external shared object file - typical
else
#  Library name of the module
oscd_modul_LTLIBRARIES = spec_Tmpl.la
spec_Tmpl_la_CXXFLAGS =
#  Libtool flags - typical
spec_Tmpl_la_LIBTOOLFLAGS = --tag=disable-static
#  Flags of linking the module
spec_Tmpl_la_LDFLAGS = -module -avoid-version $(top_builddir)/src/liboscada.la
endif

# Compiler flags
spec_Tmpl_la_CXXFLAGS += $(tmpl_CFLAGS)
# Linker flags
spec_Tmpl_la_LDFLAGS += $(tmpl_LDLAGS)
# Source files list
spec_Tmpl_la_SOURCES = module.cpp

# The module name for translation files
I18N_mod = $(oscd_modulpref)Tmpl
include ../../../../I18N.mk
