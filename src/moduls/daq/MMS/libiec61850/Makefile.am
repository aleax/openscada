EXTRA_DIST = *.h po/*

INCLUDES = -Iinc -Isrc/common -Isrc/hal -Isrc/hal/socket -Isrc/hal/thread -Isrc/mms/asn1 \
    -Isrc/mms/iso_client -Isrc/mms/iso_acse -Isrc/mms/iso_cotp -Isrc/mms/iso_session -Isrc/mms/iso_presentation \
    -Isrc/mms/iso_mms/common -Isrc/mms/iso_mms/client -Isrc/mms/iso_mms/asn1c -Isrc/mms/iso_mms/server

noinst_LTLIBRARIES = libiec61850.la
libiec61850_la_CXXFLAGS = -fpic
libiec61850_la_SOURCES = src/common/byte_buffer.c src/common/linked_list.c src/common/string_utilities.c \
    src/mms/iso_cotp/cotp.c src/mms/iso_acse/acse.c \
    src/mms/iso_presentation/iso_presentation.c src/mms/iso_session/iso_session.c \
    src/mms/iso_client/impl/iso_client_connection.c \
    src/mms/asn1/ber_encoder.c src/mms/iso_mms/asn1c/ber_decoder.c src/mms/asn1/ber_decode.c \
    src/mms/asn1/ber_integer.c src/mms/asn1/asn1_ber_primitive_value.c \
    src/mms/iso_mms/common/mms_value.c src/mms/iso_mms/common/mms_common_msg.c src/mms/iso_mms/common/mms_type_spec.c \
    src/mms/iso_mms/client/mms_client_common.c src/mms/iso_mms/client/mms_client_connection.c \
    src/mms/iso_mms/client/mms_client_initiate.c src/mms/iso_mms/client/mms_client_read.c \
    src/mms/iso_mms/client/mms_client_get_namelist.c src/mms/iso_mms/client/mms_client_get_var_access.c \
    src/mms/iso_mms/asn1c/der_encoder.c src/mms/iso_mms/asn1c/ber_tlv_tag.c src/mms/iso_mms/asn1c/ber_tlv_length.c \
    src/mms/iso_mms/asn1c/ObjectName.c src/mms/iso_mms/asn1c/ObjectClass.c \
    src/mms/iso_mms/asn1c/InformationReport.c \
    src/mms/iso_mms/asn1c/ParameterSupportOptions.c src/mms/iso_mms/asn1c/RejectPDU.c  \
    src/mms/iso_mms/asn1c/MmsPdu.c src/mms/iso_mms/asn1c/constr_CHOICE.c \
    src/mms/iso_mms/asn1c/InitiateErrorPdu.c src/mms/iso_mms/asn1c/ServiceError.c  \
    src/mms/iso_mms/asn1c/ConfirmedErrorPDU.c src/mms/iso_mms/asn1c/UnconfirmedPDU.c \
    src/mms/iso_mms/asn1c/asn_SET_OF.c \
    src/mms/iso_mms/asn1c/constr_SEQUENCE.c src/mms/iso_mms/asn1c/constr_SET_OF.c \
    src/mms/iso_mms/asn1c/constr_SEQUENCE_OF.c src/mms/iso_mms/asn1c/DataSequence.c \
    src/mms/iso_mms/asn1c/AccessResult.c src/mms/iso_mms/asn1c/DataAccessError.c \
    src/mms/iso_mms/asn1c/Data.c src/mms/iso_mms/asn1c/ServiceSupportOptions.c \
    src/mms/iso_mms/asn1c/ScatteredAccessDescription.c src/mms/iso_mms/asn1c/AlternateAccess.c \
    src/mms/iso_mms/asn1c/AlternateAccessSelection.c \
    src/mms/iso_mms/asn1c/UnconfirmedService.c \
    src/mms/iso_mms/asn1c/VariableAccessSpecification.c src/mms/iso_mms/asn1c/VariableSpecification.c \
    src/mms/iso_mms/asn1c/TypeSpecification.c \
    src/mms/iso_mms/asn1c/Identifier.c src/mms/iso_mms/asn1c/Address.c \
    src/mms/iso_mms/asn1c/NULL.c src/mms/iso_mms/asn1c/BOOLEAN.c \
    src/mms/iso_mms/asn1c/INTEGER.c src/mms/iso_mms/asn1c/NativeInteger.c \
    src/mms/iso_mms/asn1c/Unsigned8.c src/mms/iso_mms/asn1c/Integer8.c \
    src/mms/iso_mms/asn1c/Unsigned16.c src/mms/iso_mms/asn1c/Integer16.c \
    src/mms/iso_mms/asn1c/Unsigned32.c src/mms/iso_mms/asn1c/Integer32.c \
    src/mms/iso_mms/asn1c/FloatingPoint.c \
    src/mms/iso_mms/asn1c/BIT_STRING.c src/mms/iso_mms/asn1c/VisibleString.c \
    src/mms/iso_mms/asn1c/OCTET_STRING.c src/mms/iso_mms/asn1c/MMSString.c src/mms/iso_mms/asn1c/UTF8String.c \
    src/mms/iso_mms/asn1c/GeneralizedTime.c src/mms/iso_mms/asn1c/TimeOfDay.c src/mms/iso_mms/asn1c/UtcTime.c \
    src/mms/iso_mms/asn1c/ListOfVariableSeq.c src/mms/iso_mms/asn1c/StructComponent.c \
    src/mms/iso_mms/asn1c/constraints.c src/mms/iso_mms/asn1c/asn_codecs_prim.c \
    src/mms/iso_mms/asn1c/ConcludeRequestPDU.c src/mms/iso_mms/asn1c/ConcludeResponsePDU.c \
    src/mms/iso_mms/asn1c/InitiateRequestPdu.c src/mms/iso_mms/asn1c/InitiateResponsePdu.c \
    src/mms/iso_mms/asn1c/ConfirmedRequestPdu.c src/mms/iso_mms/asn1c/ConfirmedResponsePdu.c \
    src/mms/iso_mms/asn1c/InitRequestDetail.c src/mms/iso_mms/asn1c/InitResponseDetail.c \
    src/mms/iso_mms/asn1c/ConfirmedServiceRequest.c src/mms/iso_mms/asn1c/ConfirmedServiceResponse.c \
    src/mms/iso_mms/asn1c/GetNameListRequest.c src/mms/iso_mms/asn1c/GetNameListResponse.c \
    src/mms/iso_mms/asn1c/ReadRequest.c src/mms/iso_mms/asn1c/ReadResponse.c \
    src/mms/iso_mms/asn1c/WriteRequest.c src/mms/iso_mms/asn1c/WriteResponse.c \
    src/mms/iso_mms/asn1c/GetVariableAccessAttributesRequest.c src/mms/iso_mms/asn1c/GetVariableAccessAttributesResponse.c \
    src/mms/iso_mms/asn1c/DefineNamedVariableListRequest.c src/mms/iso_mms/asn1c/DefineNamedVariableListResponse.c \
    src/mms/iso_mms/asn1c/GetNamedVariableListAttributesRequest.c src/mms/iso_mms/asn1c/GetNamedVariableListAttributesResponse.c \
    src/mms/iso_mms/asn1c/DeleteNamedVariableListRequest.c src/mms/iso_mms/asn1c/DeleteNamedVariableListResponse.c \
    src/hal/hal.c src/hal/byte_stream.c \
    src/hal/thread/linux/thread_linux.c src/hal/socket/linux/socket_linux.c
