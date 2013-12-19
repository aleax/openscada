EXTRA_DIST = *.h po/*

INCLUDES = -Ilibiec61850/inc -Ilibiec61850/src/common \
    -Ilibiec61850/src/hal -Ilibiec61850/src/hal/socket -Ilibiec61850/src/hal/thread \
    -Ilibiec61850/src/mms/asn1 \
    -Ilibiec61850/src/mms/iso_client -Ilibiec61850/src/mms/iso_acse -Ilibiec61850/src/mms/iso_cotp -Ilibiec61850/src/mms/iso_session -Ilibiec61850/src/mms/iso_presentation \
    -Ilibiec61850/src/mms/iso_mms/common -Ilibiec61850/src/mms/iso_mms/client -Ilibiec61850/src/mms/iso_mms/asn1c -Ilibiec61850/src/mms/iso_mms/server

oscd_modul_LTLIBRARIES = daq_MMS.la
daq_MMS_la_SOURCES = module.cpp
daq_MMS_la_CXXFLAGS = $(MMS_CFLAGS)
daq_MMS_la_LDFLAGS = -module -avoid-version -no-undefined $(MMS_LDLAGS) -Llibiec61850/ -liec61850
daq_MMS_la_LIBTOOLFLAGS = --tag=disable-static

I18N_mod = $(oscd_modulpref)MMS
include ../../../../I18N.mk

SUBDIRS = libiec61850
