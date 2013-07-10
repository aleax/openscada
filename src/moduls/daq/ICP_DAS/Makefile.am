EXTRA_DIST = *.h po/* libi8k.a libi8k_arm.a include/*.h include_arm/*.h

oscd_modul_LTLIBRARIES = daq_ICP_DAS.la
daq_ICP_DAS_la_CXXFLAGS = $(ICP_DAS_CFLAGS)
daq_ICP_DAS_la_LDFLAGS = -module -avoid-version -no-undefined
daq_ICP_DAS_la_SOURCES = ICP_module.cpp da_LP_8x.cpp da_87x.cpp da_ISA.cpp
daq_ICP_DAS_la_LIBADD  = $(ICP_DAS_LIBS)
daq_ICP_DAS_la_LIBTOOLFLAGS = --tag=disable-static

I18N_mod = $(oscd_modulpref)ICP_DAS
include ../../../../I18N.mk
