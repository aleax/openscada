/*
 *  mms_server_connection.c
 *
 *  Copyright 2013 Michael Zillgith
 *
 *	This file is part of libIEC61850.
 *
 *	libIEC61850 is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	libIEC61850 is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with libIEC61850.  If not, see <http://www.gnu.org/licenses/>.
 *
 *	See COPYING file for the complete license text.
 *
 *
 *  MMS client connection handling code for libiec61850.
 *
 *  Handles a MMS client connection.
 */

#include "libiec61850_platform_includes.h"
#include "mms_server_internal.h"
#include "iso_server.h"
#include "ber_encoder.h"
#include "ber_decode.h"

#define MMS_SERVICE_STATUS 0x80
#define MMS_SERVICE_GET_NAME_LIST 0x40
#define MMS_SERVICE_IDENTIFY 0x20
#define MMS_SERVICE_RENAME 0x10
#define MMS_SERVICE_READ 0x08
#define MMS_SERVICE_WRITE 0x04
#define MMS_SERVICE_GET_VARIABLE_ACCESS_ATTRIBUTES 0x02
#define MMS_SERVICE_DEFINE_NAMED_VARIABLE 0x01

#define MMS_SERVICE_DEFINE_SCATTERED_ACCESS 0x80
#define MMS_SERVICE_GET_SCATTERED_ACCESS_ATTRIBUTES 0x40
#define MMS_SERVICE_DELETE_VARIABLE_ACCESS 0x20
#define MMS_SERVICE_DEFINE_NAMED_VARIABLE_LIST 0x10
#define MMS_SERVICE_GET_NAMED_VARIABLE_LIST_ATTRIBUTES 0x08
#define MMS_SERVICE_DELETE_NAMED_VARIABLE_LIST 0x04
#define MMS_SERVICE_DEFINE_NAMED_TYPE 0x02
#define MMS_SERVICE_GET_NAMED_TYPE_ATTRIBUTES 0x01


/* servicesSupported MMS bitstring */
static uint8_t servicesSupported[] =
{
		0x00
		| MMS_SERVICE_GET_NAME_LIST
		| MMS_SERVICE_IDENTIFY
		| MMS_SERVICE_READ
		| MMS_SERVICE_WRITE
		| MMS_SERVICE_GET_VARIABLE_ACCESS_ATTRIBUTES
		,
		0x00
		| MMS_SERVICE_DEFINE_NAMED_VARIABLE_LIST
		| MMS_SERVICE_DELETE_NAMED_VARIABLE_LIST
		,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00
};

/* negotiated parameter CBB */
static uint8_t parameterCBB[] =
{
		0xf1,
		0x00
};



/**********************************************************************************************
 * MMS Common support functions
 *********************************************************************************************/

void
mmsServer_writeMmsRejectPdu(uint32_t* invokeId, int reason, ByteBuffer* response) {
	MmsPdu_t* mmsPdu = (MmsPdu_t*) calloc(1, sizeof(MmsPdu_t));

	mmsPdu->present = MmsPdu_PR_rejectPDU;

	if (invokeId != NULL) {
		mmsPdu->choice.rejectPDU.originalInvokeID = (Unsigned32_t*) calloc(1, sizeof(Unsigned32_t));
		asn_long2INTEGER(mmsPdu->choice.rejectPDU.originalInvokeID, *invokeId);
	}

	if (reason == MMS_ERROR_REJECT_UNRECOGNIZED_SERVICE) {
		mmsPdu->choice.rejectPDU.rejectReason.present = RejectPDU__rejectReason_PR_confirmedRequestPDU;
		mmsPdu->choice.rejectPDU.rejectReason.choice.confirmedResponsePDU =
			RejectPDU__rejectReason__confirmedRequestPDU_unrecognizedService;
	}
	else if(reason == MMS_ERROR_REJECT_UNKNOWN_PDU_TYPE) {
		mmsPdu->choice.rejectPDU.rejectReason.present = RejectPDU__rejectReason_PR_pduError;
		asn_long2INTEGER(&mmsPdu->choice.rejectPDU.rejectReason.choice.pduError,
				RejectPDU__rejectReason__pduError_unknownPduType);
	}
	else if (reason == MMS_ERROR_REJECT_REQUEST_INVALID_ARGUMENT) {
	    mmsPdu->choice.rejectPDU.rejectReason.present = RejectPDU__rejectReason_PR_confirmedRequestPDU;
	    mmsPdu->choice.rejectPDU.rejectReason.choice.confirmedResponsePDU =
	                RejectPDU__rejectReason__confirmedRequestPDU_invalidArgument;
	}
	else {
		mmsPdu->choice.rejectPDU.rejectReason.present = RejectPDU__rejectReason_PR_confirmedRequestPDU;
		mmsPdu->choice.rejectPDU.rejectReason.choice.confirmedResponsePDU =
			RejectPDU__rejectReason__confirmedRequestPDU_other;
	}

	der_encode(&asn_DEF_MmsPdu, mmsPdu,	mmsServer_write_out, (void*) response);

	if (DEBUG) xer_fprint(stdout, &asn_DEF_MmsPdu, mmsPdu);

	asn_DEF_MmsPdu.free_struct(&asn_DEF_MmsPdu, mmsPdu, 0);
}

/**********************************************************************************************
 * MMS Initiate Service
 *********************************************************************************************/


static int
encodeInitResponseDetail(uint8_t* buffer, int bufPos, bool encode)
{
	int initResponseDetailSize = 14 + 5 + 3;

	if (encode == false)
		return initResponseDetailSize + 2;

	bufPos = BerEncoder_encodeTL(0xa4, initResponseDetailSize, buffer, bufPos);

	bufPos = BerEncoder_encodeUInt32WithTL(0x80, 1, buffer, bufPos); /* negotiated protocol version */

	bufPos = BerEncoder_encodeBitString(0x81, 11, parameterCBB, buffer, bufPos);

	bufPos = BerEncoder_encodeBitString(0x82, 85, servicesSupported, buffer, bufPos);

	return bufPos;
}


static int
createInitiateResponse(MmsServerConnection* self, ByteBuffer* writeBuffer)
{
	uint8_t* buffer = writeBuffer->buffer;
	int bufPos = 0;

	int initiateResponseLength = 0;

	initiateResponseLength += 2 + BerEncoder_UInt32determineEncodedSize(self->maxPduSize);
	initiateResponseLength += 2 + BerEncoder_UInt32determineEncodedSize(self->maxServOutstandingCalling);
	initiateResponseLength += 2 + BerEncoder_UInt32determineEncodedSize(self->maxServOutstandingCalled);
	initiateResponseLength += 2 + BerEncoder_UInt32determineEncodedSize(self->dataStructureNestingLevel);

	initiateResponseLength += encodeInitResponseDetail(NULL, 0, false);

	/* Initiate response pdu */
	bufPos = BerEncoder_encodeTL(0xa9, initiateResponseLength, buffer, bufPos);

	bufPos = BerEncoder_encodeUInt32WithTL(0x80, self->maxPduSize, buffer, bufPos);

	bufPos = BerEncoder_encodeUInt32WithTL(0x81, self->maxServOutstandingCalling, buffer, bufPos);

	bufPos = BerEncoder_encodeUInt32WithTL(0x82, self->maxServOutstandingCalled, buffer, bufPos);

	bufPos = BerEncoder_encodeUInt32WithTL(0x83, self->dataStructureNestingLevel, buffer, bufPos);

	bufPos = encodeInitResponseDetail(buffer, bufPos, true);

	writeBuffer->size = bufPos;

	return bufPos;
}

static bool
parseInitiateRequestPdu(MmsServerConnection* self, uint8_t* buffer, int bufPos, int maxBufPos)
{
	self->maxPduSize = MMS_MAXIMUM_PDU_SIZE;

	self->dataStructureNestingLevel =
					DEFAULT_DATA_STRUCTURE_NESTING_LEVEL;

	self->maxServOutstandingCalled = DEFAULT_MAX_SERV_OUTSTANDING_CALLED;

	self->maxServOutstandingCalling = DEFAULT_MAX_SERV_OUTSTANDING_CALLING;

	while (bufPos < maxBufPos) {
		uint8_t tag = buffer[bufPos++];
		int length;

		bufPos = BerDecoder_decodeLength(buffer, &length, bufPos, maxBufPos);

		if (bufPos < 0)  {
			// TODO write initiate error PDU!
			return false;
		}

		switch (tag) {
		case 0x80: /* local-detail-calling */
			self->maxPduSize = BerDecoder_decodeUint32(buffer, length, bufPos);

			if (self->maxPduSize > MMS_MAXIMUM_PDU_SIZE)
				self->maxPduSize = MMS_MAXIMUM_PDU_SIZE;

			break;

		case 0x81:  /* proposed-max-serv-outstanding-calling */
			self->maxServOutstandingCalling = BerDecoder_decodeUint32(buffer, length, bufPos);
			break;

		case 0x82:  /* proposed-max-serv-outstanding-called */
			self->maxServOutstandingCalled = BerDecoder_decodeUint32(buffer, length, bufPos);
			break;
		case 0x83: /* proposed-data-structure-nesting-level */
			self->dataStructureNestingLevel = BerDecoder_decodeUint32(buffer, length, bufPos);
			break;

		case 0xa4: /* mms-init-request-detail */
			/* we ignore this */
			break;
		}

		bufPos += length;
	}

	return true;
}

static void
handleInitiateRequestPdu (
		MmsServerConnection* self,
		uint8_t* buffer, int bufPos, int maxBufPos,
		ByteBuffer* response)
{

	if (parseInitiateRequestPdu(self, buffer, bufPos, maxBufPos))
		createInitiateResponse(self, response);
	else {
		//TODO send initiate error PDU
	}

}

/**********************************************************************************************
 * MMS Identify Service
 *********************************************************************************************/

static void
handleIdentifyRequest(
        MmsServerConnection* connection,
        int invokeId,
        ByteBuffer* response)
{
    int bufPos = 0;
    uint8_t* buffer = response->buffer;

    uint32_t invokeIdLength = BerEncoder_UInt32determineEncodedSize(invokeId);

    MmsServer mmsServer = connection->server;

    uint32_t vendorNameLength = strlen(MmsServer_getVendorName(mmsServer));
    uint32_t modelNameLength = strlen(MmsServer_getModelName(mmsServer));
    uint32_t revisionLength = strlen(MmsServer_getRevision(mmsServer));

    uint32_t identityLength = 3 +  BerEncoder_determineLengthSize(vendorNameLength)
            + BerEncoder_determineLengthSize(modelNameLength) + BerEncoder_determineLengthSize(revisionLength)
            + vendorNameLength + modelNameLength + revisionLength;

    uint32_t identifyResponseLength = invokeIdLength + 2 + 1 + BerEncoder_determineLengthSize(identityLength)
            + identityLength;

    /* Identify response pdu */
    bufPos = BerEncoder_encodeTL(0xa1, identifyResponseLength, buffer, bufPos);

    /* invokeId */
    bufPos = BerEncoder_encodeTL(0x02, invokeIdLength, buffer, bufPos);
    bufPos = BerEncoder_encodeUInt32(invokeId, buffer, bufPos);

    bufPos = BerEncoder_encodeTL(0xa2, identityLength, buffer, bufPos);
    bufPos = BerEncoder_encodeStringWithTag(0x80, MmsServer_getVendorName(mmsServer), buffer, bufPos);
    bufPos = BerEncoder_encodeStringWithTag(0x81, MmsServer_getModelName(mmsServer), buffer, bufPos);
    bufPos = BerEncoder_encodeStringWithTag(0x82, MmsServer_getRevision(mmsServer), buffer, bufPos);

    response->size = bufPos;
}

/**********************************************************************************************
 * MMS General service handling functions
 *********************************************************************************************/

static void
handleConfirmedRequestPdu(
		MmsServerConnection* self,
		uint8_t* buffer, int bufPos, int maxBufPos,
		ByteBuffer* response)
{
	uint32_t invokeId = 0;

	while (bufPos < maxBufPos) {
		uint8_t tag = buffer[bufPos++];
		int length;

		bufPos = BerDecoder_decodeLength(buffer, &length, bufPos, maxBufPos);

		if (bufPos < 0)  {
			mmsServer_writeMmsRejectPdu(&invokeId, MMS_ERROR_REJECT_UNRECOGNIZED_SERVICE, response);
			return;
		}

		if (DEBUG) printf("tag %02x size: %i\n", tag, length);

		switch(tag) {
		case 0x02: /* invoke Id */
			invokeId = BerDecoder_decodeUint32(buffer, length, bufPos);
			if (DEBUG) printf("invokeId: %i\n", invokeId);
			self->lastInvokeId = invokeId;
			break;
		case 0xa1: /* get-name-list-request */
			mmsServer_handleGetNameListRequest(self, buffer, bufPos, bufPos + length,
					invokeId, response);
			break;

		case 0x82: /* identify */
			handleIdentifyRequest(self, invokeId, response);
			break;

		case 0xa4: /* read-request */
			mmsServer_handleReadRequest(self, buffer, bufPos, bufPos + length,
					invokeId, response);
			break;
		case 0xa5: /* write-request */
			mmsServer_handleWriteRequest(self, buffer, bufPos, bufPos + length,
							invokeId, response);
			break;
		case 0xa6: /* get-variable-access-attributes-request */
			mmsServer_handleGetVariableAccessAttributesRequest(self,
					buffer, bufPos, bufPos + length,
					invokeId, response);
			break;
		case 0xab: /* define-named-variable-list */
			mmsServer_handleDefineNamedVariableListRequest(self,
					buffer, bufPos, bufPos + length,
					invokeId, response);
			break;
		case 0xac: /* get-named-variable-list-attributes-request */
			mmsServer_handleGetNamedVariableListAttributesRequest(self,
					buffer, bufPos, bufPos + length,
					invokeId, response);
			break;
		case 0xad: /* delete-named-variable-list-request */
			mmsServer_handleDeleteNamedVariableListRequest(self,
					buffer, bufPos, bufPos + length,
					invokeId, response);
			break;
		default:
			mmsServer_writeMmsRejectPdu(&invokeId, MMS_ERROR_REJECT_UNRECOGNIZED_SERVICE, response);
			return;
			break;
		}

		bufPos += length;
	}
}


static inline MmsIndication
parseMmsPdu(MmsServerConnection* self, ByteBuffer* message, ByteBuffer* response)
{
	MmsIndication retVal;

	uint8_t* buffer = message->buffer;

	if (message->size < 2)
		return MMS_ERROR;

	int bufPos = 0;

	uint8_t pduType = buffer[bufPos++];
	int pduLength;

	bufPos = BerDecoder_decodeLength(buffer, &pduLength, bufPos, message->size);

	if (bufPos < 0)
		return MMS_ERROR;

	if (DEBUG) printf("mms_server: recvd MMS-PDU type: %02x size: %u\n", pduType, pduLength);

	switch (pduType) {
	case 0xa8: /* Initiate request PDU */
		handleInitiateRequestPdu(self, buffer, bufPos, bufPos + pduLength, response);
		retVal = MMS_INITIATE;
		break;
	case 0xa0: /* Confirmed request PDU */
		handleConfirmedRequestPdu(self, buffer, bufPos, bufPos + pduLength, response);
		retVal = MMS_CONFIRMED_REQUEST;
		break;
	case 0x8b: /* Conclude request PDU */
		mmsServer_writeConcludeResponsePdu(response);
		IsoConnection_close(self->isoConnection);
		retVal = MMS_CONCLUDE;
		break;
	case 0xa4: /* Reject PDU - silently ignore */
	    if (DEBUG) printf("received reject PDU!\n");
		retVal = MMS_OK;
		break;
	default:
		mmsServer_writeMmsRejectPdu(NULL, MMS_ERROR_REJECT_UNKNOWN_PDU_TYPE, response);
		retVal = MMS_ERROR;
		break;
	}

parseMmsPdu_exit:
	return retVal;
}

static void /* will be called by IsoConnection */
messageReceived(void* parameter, ByteBuffer* message, ByteBuffer* response)
{
	MmsServerConnection* self = (MmsServerConnection*) parameter;

	MmsServerConnection_parseMessage(self,  message, response);
}

/**********************************************************************************************
 * MMS server connection public API functions
 *********************************************************************************************/

MmsServerConnection*
MmsServerConnection_init(MmsServerConnection* connection, MmsServer server, IsoConnection isoCon)
{
	MmsServerConnection* self;

	if (connection == NULL)
		self = (MmsServerConnection*) calloc(1, sizeof(MmsServerConnection));
	else
		self = connection;

	self->maxServOutstandingCalled = 0;
	self->maxServOutstandingCalling = 0;
	self->maxPduSize = MMS_MAXIMUM_PDU_SIZE;
	self->dataStructureNestingLevel = 0;
	self->server = server;
	self->isoConnection = isoCon;
	self->namedVariableLists = LinkedList_create();

	IsoConnection_installListener(isoCon, messageReceived, (void*) self);

	return self;
}

void
MmsServerConnection_destroy(MmsServerConnection* self)
{
	LinkedList_destroyDeep(self->namedVariableLists, (LinkedListValueDeleteFunction) MmsNamedVariableList_destroy);
	free(self);
}

bool
MmsServerConnection_addNamedVariableList(MmsServerConnection* self, MmsNamedVariableList variableList)
{
	//TODO check if operation is allowed!

	LinkedList_add(self->namedVariableLists, variableList);

	return true;
}

void
MmsServerConnection_deleteNamedVariableList(MmsServerConnection* self, char* listName)
{
	mmsServer_deleteVariableList(self->namedVariableLists, listName);
}

MmsNamedVariableList
MmsServerConnection_getNamedVariableList(MmsServerConnection* self, char* variableListName)
{
	//TODO remove code duplication - similar to MmsDomain_getNamedVariableList !
	MmsNamedVariableList variableList = NULL;

	LinkedList element = LinkedList_getNext(self->namedVariableLists);

	while (element != NULL) {
		MmsNamedVariableList varList = (MmsNamedVariableList) element->data;

		if (strcmp(MmsNamedVariableList_getName(varList), variableListName) == 0) {
			variableList = varList;
			break;
		}

		element = LinkedList_getNext(element);
	}

	return variableList;
}

char*
MmsServerConnection_getClientAddress(MmsServerConnection* self)
{
	return IsoConnection_getPeerAddress(self->isoConnection);
}

LinkedList
MmsServerConnection_getNamedVariableLists(MmsServerConnection* self)
{
	return self->namedVariableLists;
}

MmsIndication
MmsServerConnection_parseMessage(MmsServerConnection* self, ByteBuffer* message, ByteBuffer* response)
{
	return parseMmsPdu(self, message, response);
}

uint32_t
MmsServerConnection_getLastInvokeId(MmsServerConnection* self)
{
    return self->lastInvokeId;
}

