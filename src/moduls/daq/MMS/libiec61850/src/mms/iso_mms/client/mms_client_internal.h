/*
 *  mms_msg_internal.h
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
 */

#ifndef MMS_MSG_INTERNAL_H_
#define MMS_MSG_INTERNAL_H_

#include "MmsPdu.h"
#include "linked_list.h"
#include "mms_client_connection.h"
#include "ber_decode.h"

#include "thread.h"

typedef enum {
	MMS_STATE_CLOSED,
	MMS_STATE_CONNECTING,
	MMS_STATE_CONNECTED
} AssociationState;

typedef enum {
	MMS_CON_IDLE,
	MMS_CON_WAITING,
	MMS_CON_ASSOCIATION_FAILED,
	MMS_CON_ASSOCIATED,
	MMS_CON_RESPONSE_PENDING
} ConnectionState;

/* private instance variables */
struct sMmsConnection {
    Semaphore lastInvokeIdLock;
    uint32_t lastInvokeId;

	Semaphore lastResponseLock;
    uint32_t responseInvokeId;
	ByteBuffer* lastResponse;
	uint32_t lastResponseBufPos;
	MmsError lastResponseError;

	Semaphore outstandingCallsLock;
	uint32_t* outstandingCalls;

	uint32_t requestTimeout;

	IsoClientConnection isoClient;
	AssociationState associationState;
	ConnectionState connectionState;
	uint8_t* buffer;

	MmsConnectionParameters parameters;
	IsoConnectionParameters* isoParameters;

	int isoConnectionParametersSelfAllocated;

	MmsInformationReportHandler reportHandler;
	void* reportHandlerParameter;

	MmsConnectionLostHandler connectionLostHandler;
	void* connectionLostHandlerParameter;
};


/**
 * MMS Object class enumeration type
 */
typedef enum {
	MMS_NAMED_VARIABLE,
	MMS_NAMED_VARIABLE_LIST
} MmsObjectClass;

MmsValue*
mmsClient_parseListOfAccessResults(AccessResult_t** accessResultList, int listSize, bool createArray);

uint32_t
mmsClient_getInvokeId(ConfirmedResponsePdu_t* confirmedResponse);

int
mmsClient_write_out(void *buffer, size_t size, void *app_key);

int
mmsClient_createInitiateRequest(MmsConnection self, ByteBuffer* writeBuffer);

MmsPdu_t*
mmsClient_createConfirmedRequestPdu(uint32_t invokeId);

int
mmsClient_createMmsGetNameListRequestVMDspecific(long invokeId, ByteBuffer* writeBuffer, char* continueAfter);

bool
mmsClient_parseGetNameListResponse(LinkedList* nameList, ByteBuffer* message, uint32_t* invokeId);

int
mmsClient_createGetNameListRequestDomainSpecific(long invokeId, char* domainName,
		ByteBuffer* writeBuffer, MmsObjectClass objectClass, char* continueAfter);

MmsValue*
mmsClient_parseReadResponse(ByteBuffer* message, uint32_t* invokeId, bool createArray);

int
mmsClient_createReadRequest(uint32_t invokeId, char* domainId, char* itemId, ByteBuffer* writeBuffer);

int
mmsClient_createReadRequestAlternateAccessIndex(uint32_t invokeId, char* domainId, char* itemId,
		uint32_t index, uint32_t elementCount, ByteBuffer* writeBuffer);

int
mmsClient_createReadRequestMultipleValues(uint32_t invokeId, char* domainId, LinkedList /*<char*>*/ items,
		ByteBuffer* writeBuffer);

int
mmsClient_createReadNamedVariableListRequest(uint32_t invokeId, char* domainId, char* itemId,
		ByteBuffer* writeBuffer, bool specWithResult);

int
mmsClient_createReadAssociationSpecificNamedVariableListRequest(
		uint32_t invokeId,
		char* itemId,
		ByteBuffer* writeBuffer,
		bool specWithResult);

void
mmsClient_createGetNamedVariableListAttributesRequest(uint32_t invokeId, ByteBuffer* writeBuffer,
		char* domainId, char* listNameId);

LinkedList
mmsClient_parseGetNamedVariableListAttributesResponse(ByteBuffer* message, uint32_t* invokeId,
		bool* /*OUT*/ deletable);

int
mmsClient_createGetVariableAccessAttributesRequest(
        uint32_t invokeId,
		char* domainId, char* itemId,
		ByteBuffer* writeBuffer);

MmsVariableSpecification*
mmsClient_parseGetVariableAccessAttributesResponse(ByteBuffer* message, uint32_t* invokeId);

MmsIndication
mmsClient_parseWriteResponse(ByteBuffer* message);

MmsIndication
mmsClient_parseWriteMultipleItemsResponse(ByteBuffer* message, int itemCount, LinkedList* accessResults);

int
mmsClient_createWriteRequest(uint32_t invokeId, char* domainId, char* itemId, MmsValue* value,
		ByteBuffer* writeBuffer);

int
mmsClient_createWriteMultipleItemsRequest(uint32_t invokeId, char* domainId, LinkedList itemIds, LinkedList values,
        ByteBuffer* writeBuffer);

void
mmsClient_createDefineNamedVariableListRequest(uint32_t invokeId, ByteBuffer* writeBuffer,
		char* domainId, char* listNameId, LinkedList /*<char*>*/ listOfVariables,
		bool associationSpecific);

MmsIndication
mmsClient_parseDefineNamedVariableResponse(ByteBuffer* message, uint32_t* invokeId);

void
mmsClient_createDeleteNamedVariableListRequest(long invokeId, ByteBuffer* writeBuffer,
		char* domainId, char* listNameId);

MmsIndication
mmsClient_parseDeleteNamedVariableListResponse(ByteBuffer* message, uint32_t* invokeId);

void
mmsClient_createDeleteAssociationSpecificNamedVariableListRequest(
		long invokeId,
		ByteBuffer* writeBuffer,
		char* listNameId);

void
mmsClient_createIdentifyRequest(uint32_t invokeId, ByteBuffer* request);

MmsServerIdentity*
mmsClient_parseIdentifyResponse(ByteBuffer* message, uint32_t* invokeId);

bool
mmsClient_parseInitiateResponse(MmsConnection self);

int
mmsClient_createMmsGetNameListRequestAssociationSpecific(long invokeId, ByteBuffer* writeBuffer,
		char* continueAfter);

#endif /* MMS_MSG_INTERNAL_H_ */
