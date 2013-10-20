EXTRA_DIST = *.h po/*

INCLUDES = -Ilibiec61850/inc -Ilibiec61850/src/common \
    -Ilibiec61850/src/hal -Ilibiec61850/src/hal/socket -Ilibiec61850/src/hal/thread \
    -Ilibiec61850/src/mms/asn1 \
    -Ilibiec61850/src/mms/iso_client -Ilibiec61850/src/mms/iso_acse -Ilibiec61850/src/mms/iso_cotp -Ilibiec61850/src/mms/iso_session -Ilibiec61850/src/mms/iso_presentation \
    -Ilibiec61850/src/mms/iso_mms/common -Ilibiec61850/src/mms/iso_mms/client -Ilibiec61850/src/mms/iso_mms/asn1c -Ilibiec61850/src/mms/iso_mms/server

oscd_modul_LTLIBRARIES = daq_MMS.la
daq_MMS_la_LIBTOOLFLAGS = --tag=disable-static
daq_MMS_la_CXXFLAGS = $(MMS_CFLAGS)
daq_MMS_la_LDFLAGS = -module -avoid-version -no-undefined $(MMS_LDLAGS)
daq_MMS_la_SOURCES = module.cpp \
    libiec61850/src/common/byte_buffer.c libiec61850/src/common/linked_list.c libiec61850/src/common/string_utilities.c \
    libiec61850/src/mms/iso_cotp/cotp.c libiec61850/src/mms/iso_acse/acse.c \
    libiec61850/src/mms/iso_presentation/iso_presentation.c libiec61850/src/mms/iso_session/iso_session.c \
    libiec61850/src/mms/iso_client/impl/iso_client_connection.c \
    libiec61850/src/mms/asn1/ber_encoder.c libiec61850/src/mms/iso_mms/asn1c/ber_decoder.c libiec61850/src/mms/asn1/ber_decode.c \
    libiec61850/src/mms/asn1/ber_integer.c libiec61850/src/mms/asn1/asn1_ber_primitive_value.c \
    libiec61850/src/mms/iso_mms/common/mms_value.c libiec61850/src/mms/iso_mms/common/mms_common_msg.c libiec61850/src/mms/iso_mms/common/mms_type_spec.c \
    libiec61850/src/mms/iso_mms/client/mms_client_common.c libiec61850/src/mms/iso_mms/client/mms_client_connection.c \
    libiec61850/src/mms/iso_mms/client/mms_client_initiate.c libiec61850/src/mms/iso_mms/client/mms_client_read.c \
    libiec61850/src/mms/iso_mms/client/mms_client_get_namelist.c libiec61850/src/mms/iso_mms/client/mms_client_get_var_access.c \
    libiec61850/src/mms/iso_mms/asn1c/der_encoder.c libiec61850/src/mms/iso_mms/asn1c/ber_tlv_tag.c libiec61850/src/mms/iso_mms/asn1c/ber_tlv_length.c \
    libiec61850/src/mms/iso_mms/asn1c/ObjectName.c libiec61850/src/mms/iso_mms/asn1c/ObjectClass.c \
    libiec61850/src/mms/iso_mms/asn1c/InformationReport.c \
    libiec61850/src/mms/iso_mms/asn1c/ParameterSupportOptions.c libiec61850/src/mms/iso_mms/asn1c/RejectPDU.c  \
    libiec61850/src/mms/iso_mms/asn1c/MmsPdu.c libiec61850/src/mms/iso_mms/asn1c/constr_CHOICE.c \
    libiec61850/src/mms/iso_mms/asn1c/InitiateErrorPdu.c libiec61850/src/mms/iso_mms/asn1c/ServiceError.c  \
    libiec61850/src/mms/iso_mms/asn1c/ConfirmedErrorPDU.c libiec61850/src/mms/iso_mms/asn1c/UnconfirmedPDU.c \
    libiec61850/src/mms/iso_mms/asn1c/asn_SET_OF.c \
    libiec61850/src/mms/iso_mms/asn1c/constr_SEQUENCE.c libiec61850/src/mms/iso_mms/asn1c/constr_SET_OF.c \
    libiec61850/src/mms/iso_mms/asn1c/constr_SEQUENCE_OF.c libiec61850/src/mms/iso_mms/asn1c/DataSequence.c \
    libiec61850/src/mms/iso_mms/asn1c/AccessResult.c libiec61850/src/mms/iso_mms/asn1c/DataAccessError.c \
    libiec61850/src/mms/iso_mms/asn1c/Data.c libiec61850/src/mms/iso_mms/asn1c/ServiceSupportOptions.c \
    libiec61850/src/mms/iso_mms/asn1c/ScatteredAccessDescription.c libiec61850/src/mms/iso_mms/asn1c/AlternateAccess.c \
    libiec61850/src/mms/iso_mms/asn1c/AlternateAccessSelection.c \
    libiec61850/src/mms/iso_mms/asn1c/UnconfirmedService.c \
    libiec61850/src/mms/iso_mms/asn1c/VariableAccessSpecification.c libiec61850/src/mms/iso_mms/asn1c/VariableSpecification.c \
    libiec61850/src/mms/iso_mms/asn1c/TypeSpecification.c \
    libiec61850/src/mms/iso_mms/asn1c/Identifier.c libiec61850/src/mms/iso_mms/asn1c/Address.c \
    libiec61850/src/mms/iso_mms/asn1c/NULL.c libiec61850/src/mms/iso_mms/asn1c/BOOLEAN.c \
    libiec61850/src/mms/iso_mms/asn1c/INTEGER.c libiec61850/src/mms/iso_mms/asn1c/NativeInteger.c \
    libiec61850/src/mms/iso_mms/asn1c/Unsigned8.c libiec61850/src/mms/iso_mms/asn1c/Integer8.c \
    libiec61850/src/mms/iso_mms/asn1c/Unsigned16.c libiec61850/src/mms/iso_mms/asn1c/Integer16.c \
    libiec61850/src/mms/iso_mms/asn1c/Unsigned32.c libiec61850/src/mms/iso_mms/asn1c/Integer32.c \
    libiec61850/src/mms/iso_mms/asn1c/FloatingPoint.c \
    libiec61850/src/mms/iso_mms/asn1c/BIT_STRING.c libiec61850/src/mms/iso_mms/asn1c/VisibleString.c \
    libiec61850/src/mms/iso_mms/asn1c/OCTET_STRING.c libiec61850/src/mms/iso_mms/asn1c/MMSString.c libiec61850/src/mms/iso_mms/asn1c/UTF8String.c \
    libiec61850/src/mms/iso_mms/asn1c/GeneralizedTime.c libiec61850/src/mms/iso_mms/asn1c/TimeOfDay.c libiec61850/src/mms/iso_mms/asn1c/UtcTime.c \
    libiec61850/src/mms/iso_mms/asn1c/ListOfVariableSeq.c libiec61850/src/mms/iso_mms/asn1c/StructComponent.c \
    libiec61850/src/mms/iso_mms/asn1c/constraints.c libiec61850/src/mms/iso_mms/asn1c/asn_codecs_prim.c \
    libiec61850/src/mms/iso_mms/asn1c/ConcludeRequestPDU.c libiec61850/src/mms/iso_mms/asn1c/ConcludeResponsePDU.c \
    libiec61850/src/mms/iso_mms/asn1c/InitiateRequestPdu.c libiec61850/src/mms/iso_mms/asn1c/InitiateResponsePdu.c \
    libiec61850/src/mms/iso_mms/asn1c/ConfirmedRequestPdu.c libiec61850/src/mms/iso_mms/asn1c/ConfirmedResponsePdu.c \
    libiec61850/src/mms/iso_mms/asn1c/InitRequestDetail.c libiec61850/src/mms/iso_mms/asn1c/InitResponseDetail.c \
    libiec61850/src/mms/iso_mms/asn1c/ConfirmedServiceRequest.c libiec61850/src/mms/iso_mms/asn1c/ConfirmedServiceResponse.c \
    libiec61850/src/mms/iso_mms/asn1c/GetNameListRequest.c libiec61850/src/mms/iso_mms/asn1c/GetNameListResponse.c \
    libiec61850/src/mms/iso_mms/asn1c/ReadRequest.c libiec61850/src/mms/iso_mms/asn1c/ReadResponse.c \
    libiec61850/src/mms/iso_mms/asn1c/WriteRequest.c libiec61850/src/mms/iso_mms/asn1c/WriteResponse.c \
    libiec61850/src/mms/iso_mms/asn1c/GetVariableAccessAttributesRequest.c libiec61850/src/mms/iso_mms/asn1c/GetVariableAccessAttributesResponse.c \
    libiec61850/src/mms/iso_mms/asn1c/DefineNamedVariableListRequest.c libiec61850/src/mms/iso_mms/asn1c/DefineNamedVariableListResponse.c \
    libiec61850/src/mms/iso_mms/asn1c/GetNamedVariableListAttributesRequest.c libiec61850/src/mms/iso_mms/asn1c/GetNamedVariableListAttributesResponse.c \
    libiec61850/src/mms/iso_mms/asn1c/DeleteNamedVariableListRequest.c libiec61850/src/mms/iso_mms/asn1c/DeleteNamedVariableListResponse.c \
    libiec61850/src/hal/hal.c  libiec61850/src/hal/byte_stream.c \
    libiec61850/src/hal/thread/linux/thread_linux.c libiec61850/src/hal/socket/linux/socket_linux.c


I18N_mod = $(oscd_modulpref)MMS
include ../../../../I18N.mk
