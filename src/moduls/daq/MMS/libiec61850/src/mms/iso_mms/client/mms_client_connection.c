/*
 *  mms_client_connection.c
 *
 *  Copyright 2013 Michael Zillgith
 *
 *  This file is part of libIEC61850.
 *
 *  libIEC61850 is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  libIEC61850 is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with libIEC61850.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  See COPYING file for the complete license text.
 */

#include "libiec61850_platform_includes.h"

#include "mms_client_connection.h"
#include "iso_client_connection.h"
#include "mms_client_internal.h"
#include "stack_config.h"

#include <MmsPdu.h>

#include "byte_buffer.h"
#include "ber_decode.h"

#define CONFIG_MMS_CONNECTION_DEFAULT_TIMEOUT 5000
#define OUTSTANDING_CALLS 10

static void
handleUnconfirmedMmsPdu(MmsConnection self, ByteBuffer* message)
{
	if (self->reportHandler != NULL) {
	    MmsPdu_t* mmsPdu = 0; /* allow asn1c to allocate structure */

	    MmsValue* valueList = NULL;
	    MmsValue* value = NULL;

	    asn_dec_rval_t rval;

	    rval = ber_decode(NULL, &asn_DEF_MmsPdu,
	            (void**) &mmsPdu, ByteBuffer_getBuffer(message), ByteBuffer_getSize(message));

	    if (rval.code == RC_OK) {

	        if (DEBUG) xer_fprint(stdout, &asn_DEF_MmsPdu, mmsPdu);

	        if (mmsPdu->present == MmsPdu_PR_unconfirmedPDU) {

	            if (mmsPdu->choice.unconfirmedPDU.unconfirmedService.present ==
	                    UnconfirmedService_PR_informationReport)
	            {
	                char* domainId = NULL;

	                InformationReport_t* report =
	                        &(mmsPdu->choice.unconfirmedPDU.unconfirmedService.choice.informationReport);

	                if (report->variableAccessSpecification.present ==
	                        VariableAccessSpecification_PR_variableListName)
	                {
	                    if (report->variableAccessSpecification.choice.variableListName.present == ObjectName_PR_vmdspecific)
	                    {
	                        int nameSize = report->variableAccessSpecification.choice.variableListName.choice.vmdspecific.size;
	                        uint8_t* buffer = report->variableAccessSpecification.choice.variableListName.choice.vmdspecific.buf;

	                        char* variableListName = createStringFromBuffer(buffer, nameSize);


							int listSize = report->listOfAccessResult.list.count;

							MmsValue* values = mmsClient_parseListOfAccessResults(
								report->listOfAccessResult.list.array, listSize, true);

							self->reportHandler(self->reportHandlerParameter, domainId, variableListName, values, true);

							free(variableListName);
	                    }
	                    else {
	                        // Ignore domain and association specific information reports (not used by IEC 61850)
	                    }


	                }
	                else if (report->variableAccessSpecification.present ==
	                		VariableAccessSpecification_PR_listOfVariable)
	                {
	                	//TODO add code to handle reports with more than one variable

	                	int listSize = report->listOfAccessResult.list.count;
						int variableSpecSize = report->variableAccessSpecification.choice.listOfVariable.list.count;

						if (listSize != variableSpecSize) {
							if (DEBUG) printf("report contains wrong number of access results\n");
							return;
						}

						MmsValue* values = mmsClient_parseListOfAccessResults(
														report->listOfAccessResult.list.array, listSize, false);

						int i;
						for (i = 0; i < variableSpecSize; i++) {
	                		if (report->variableAccessSpecification.choice.listOfVariable.list.array[i]->variableSpecification.present ==
	                				VariableSpecification_PR_name)
	                		{
	                			if (report->variableAccessSpecification.choice.listOfVariable.list.array[i]
											 ->variableSpecification.choice.name.present == ObjectName_PR_vmdspecific) {

									int nameSize = report->variableAccessSpecification.choice.listOfVariable.list.array[i]
									                           ->variableSpecification.choice.name.choice.vmdspecific.size;

									uint8_t* buffer = report->variableAccessSpecification.choice.listOfVariable.list.array[i]
									                            ->variableSpecification.choice.name.choice.vmdspecific.buf;

									char* variableListName = createStringFromBuffer(buffer, nameSize);


									self->reportHandler(self->reportHandlerParameter, domainId, variableListName, values, false);

									free(variableListName);
	                			}

	                		}
	                	}

	                }
	                else {
	                    // Ignore
	                	printf("unrecognized information report\n");
	                }

	            }

	        }
	    }
	    else {
	        printf("handleUnconfirmedMmsPdu: error parsing PDU at %i\n", rval.consumed);
	    }

	    asn_DEF_MmsPdu.free_struct(&asn_DEF_MmsPdu, mmsPdu, 0);
	}
}

static uint32_t
getNextInvokeId(MmsConnection self)
{
    uint32_t nextInvokeId;

    Semaphore_wait(self->lastInvokeIdLock);
    self->lastInvokeId++;
    nextInvokeId = self->lastInvokeId;
    Semaphore_post(self->lastInvokeIdLock);

    return nextInvokeId;
}

static bool
checkForOutstandingCall(MmsConnection self, uint32_t invokeId)
{
    int i = 0;

    Semaphore_wait(self->outstandingCallsLock);

    for (i = 0; i < OUTSTANDING_CALLS; i++) {
        if (self->outstandingCalls[i] == invokeId) {
            Semaphore_post(self->outstandingCallsLock);
            return true;
        }
    }

    Semaphore_post(self->outstandingCallsLock);

    return false;
}

static bool
addToOutstandingCalls(MmsConnection self, uint32_t invokeId)
{
    int i = 0;

    Semaphore_wait(self->outstandingCallsLock);

    for (i = 0; i < OUTSTANDING_CALLS; i++) {
        if (self->outstandingCalls[i] == 0) {
            self->outstandingCalls[i] = invokeId;
            Semaphore_post(self->outstandingCallsLock);
            return true;
        }
    }

    Semaphore_post(self->outstandingCallsLock);

    return false;
}

static void
removeFromOutstandingCalls(MmsConnection self, uint32_t invokeId)
{
    int i = 0;

    Semaphore_wait(self->outstandingCallsLock);

    for (i = 0; i < OUTSTANDING_CALLS; i++) {
        if (self->outstandingCalls[i] == invokeId) {
            self->outstandingCalls[i] = 0;
            break;
        }
    }

    Semaphore_post(self->outstandingCallsLock);
}


static ByteBuffer*
sendRequestAndWaitForResponse(MmsConnection self, uint32_t invokeId, ByteBuffer* message)
{
    ByteBuffer* receivedMessage = NULL;

    uint64_t startTime = Hal_getTimeInMs();

    uint64_t waitUntilTime = startTime + self->requestTimeout;

    uint64_t currentTime = startTime;

    bool success = false;

    addToOutstandingCalls(self, invokeId);

    IsoClientConnection_sendMessage(self->isoClient, message);

    while (currentTime < waitUntilTime) {
        uint32_t receivedInvokeId;

        if (self->associationState == MMS_STATE_CLOSED)
            goto connection_lost;

        Semaphore_wait(self->lastResponseLock);
        receivedInvokeId = self->responseInvokeId;
        Semaphore_post(self->lastResponseLock);

        if (receivedInvokeId == invokeId) {
            receivedMessage = self->lastResponse;
            success = true;
            break;
        }

        Thread_sleep(10);

        currentTime = Hal_getTimeInMs();
    }

    if (!success) {
       if (DEBUG) printf("TIMEOUT for request %u: \n", invokeId);
       self->lastResponseError = MMS_ERROR_SERVICE_TIMEOUT;
    }

connection_lost:

    removeFromOutstandingCalls(self, invokeId);

    return receivedMessage;
}

static void
releaseResponse(MmsConnection self)
{
    IsoClientConnection_releasePayloadBuffer(self->isoClient, self->lastResponse);

    Semaphore_wait(self->lastResponseLock);
    self->responseInvokeId = 0;
    self->lastResponseError = MMS_ERROR_NONE;
    Semaphore_post(self->lastResponseLock);
}


static uint32_t
getResponseInvokeId(MmsConnection self)
{
    uint32_t currentInvokeId;

    Semaphore_wait(self->lastResponseLock);
    currentInvokeId = self->responseInvokeId;
    Semaphore_post(self->lastResponseLock);

    return currentInvokeId;
}

static void
waitUntilLastResponseHasBeenProcessed(MmsConnection self)
{
    uint32_t currentInvokeId = getResponseInvokeId(self);

    while (currentInvokeId != 0) {
        Thread_sleep(1);
        currentInvokeId = getResponseInvokeId(self);
    }
}

typedef struct sMmsServiceError {
    int errorClass;
    int errorCode;
} MmsServiceError;

static MmsError
convertServiceErrorToMmsError(MmsServiceError serviceError)
{
    MmsError mmsError;

    switch(serviceError.errorClass) {
    case 0: /* class: vmd-state */
        mmsError = MMS_ERROR_VMDSTATE_OTHER;
        break;

    case 1: /* class: application-reference */
        mmsError = MMS_ERROR_APPLICATION_REFERENCE_OTHER;
        break;

    case 2: /* class: definition */
        switch (serviceError.errorCode) {
        case 1:
            mmsError = MMS_ERROR_DEFINITION_OBJECT_UNDEFINED;
            break;
        case 2:
            mmsError = MMS_ERROR_DEFINITION_INVALID_ADDRESS;
            break;
        case 3:
            mmsError = MMS_ERROR_DEFINITION_TYPE_UNSUPPORTED;
            break;
        case 4:
            mmsError = MMS_ERROR_DEFINITION_TYPE_INCONSISTENT;
            break;
        case 5:
            mmsError = MMS_ERROR_DEFINITION_OBJECT_EXISTS;
            break;
        case 6:
            mmsError = MMS_ERROR_DEFINITION_OBJECT_ATTRIBUTE_INCONSISTENT;
            break;
        default:
            mmsError = MMS_ERROR_DEFINITION_OTHER;
            break;
        }
        break;

    case 3: /* class: resource */
        mmsError = MMS_ERROR_RESOURCE_OTHER;
        break;

    case 4: /* class: service */
        mmsError = MMS_ERROR_SERVICE_OTHER;
        break;

    case 5: /* class: service-preempt */
        mmsError = MMS_ERROR_SERVICE_PREEMPT_OTHER;
        break;

    case 6: /* class: time-resolution */
        mmsError = MMS_ERROR_TIME_RESOLUTION_OTHER;
        break;

    case 7: /* class: access */
        switch (serviceError.errorCode) {
        case 1:
            mmsError = MMS_ERROR_ACCESS_OBJECT_ACCESS_UNSUPPORTED;
            break;
        case 2:
            mmsError = MMS_ERROR_ACCESS_OBJECT_NON_EXISTENT;
            break;
        case 3:
            mmsError = MMS_ERROR_ACCESS_OBJECT_ACCESS_DENIED;
            break;
        case 4:
            mmsError = MMS_ERROR_ACCESS_OBJECT_INVALIDATED;
            break;
        default:
            mmsError = MMS_ERROR_ACCESS_OTHER;
            break;
        }
        break;

    default:
        mmsError = MMS_ERROR_OTHER;
    }

    return mmsError;
}

static int
parseServiceError(uint8_t* buffer, int bufPos, int maxLength, MmsServiceError* error)
{
    int endPos = bufPos + maxLength;
    int length;

    while (bufPos < endPos) {
       uint8_t tag = buffer[bufPos++];
       bufPos = BerDecoder_decodeLength(buffer, &length, bufPos, endPos);

       if (bufPos < 0) return -1;

       switch (tag) {
       case 0xa0: /* errorClass */ {
           uint8_t errorClassTag = buffer[bufPos++];
           bufPos = BerDecoder_decodeLength(buffer, &length, bufPos, endPos);

           if (bufPos < 0) return -1;

           error->errorClass = errorClassTag - 0x80;
           error->errorCode = BerDecoder_decodeUint32(buffer, length, bufPos);

           bufPos += length;
           }
           break;
       case 0x81: /* additionalCode */
           bufPos += length; /* ignore */
           break;
       case 0x82: /* additionalDescription */
           bufPos += length; /* ignore */
           break;
       case 0xa3: /* serviceSpecificInfo */
           bufPos += length; /* ignore */
           break;
       }
    }

    return bufPos;
}

static int
parseConfirmedErrorPDU(ByteBuffer* message, uint32_t* invokeId, MmsServiceError* serviceError)
{
    uint8_t* buffer = message->buffer;
    int maxBufPos = message->size;
    int bufPos = 0;
    int length;

    uint8_t tag = buffer[bufPos++];
    if (tag != 0xa2) goto exit_error;

    bufPos = BerDecoder_decodeLength(buffer, &length, bufPos, maxBufPos);
    if (bufPos < 0) goto exit_error;

    int endPos = bufPos + length;

    if (endPos > maxBufPos) {
        if (DEBUG) printf("parseConfirmedErrorPDU: message to short!\n");
        goto exit_error;
    }

    while (bufPos < endPos) {
        tag = buffer[bufPos++];
        bufPos = BerDecoder_decodeLength(buffer, &length, bufPos, maxBufPos);

        if (bufPos < 0) goto exit_error;

        switch (tag) {
        case 0x80: /* invoke Id */
            if (invokeId != NULL)
                *invokeId = BerDecoder_decodeUint32(buffer, length, bufPos);
            bufPos += length;
            break;
        case 0x81: /* modifierPosition */
            bufPos += length; /* ignore */
            break;
        case 0xa2: /* serviceError */
            bufPos = parseServiceError(buffer, bufPos, length, serviceError);
            if (bufPos < 0) goto exit_error;
            break;
        }
    }

    return bufPos;

exit_error:
    return -1;
}

static void
mmsIsoCallback(IsoIndication indication, void* parameter, ByteBuffer* payload)
{
	MmsConnection self = (MmsConnection) parameter;

	if (DEBUG) printf("mmsIsoCallback called with indication %i\n", indication);

	if (indication == ISO_IND_CLOSED) {
	    if (DEBUG) printf("mmsIsoCallback: Connection lost or closed by client!\n");
	    self->connectionState = MMS_CON_IDLE;
	    self->associationState = MMS_STATE_CLOSED;

	    /* Call user provided callback function */
	    if (self->connectionLostHandler != NULL)
	        self->connectionLostHandler(self, self->connectionLostHandlerParameter);

	    return;
	}

	if (indication == ISO_IND_ASSOCIATION_FAILED) {
	    if (DEBUG) printf("mmsIsoCallback: association failed!\n");
	    self->connectionState = MMS_CON_ASSOCIATION_FAILED;
	    self->associationState = MMS_STATE_CLOSED;
	    return;
	}

	if (payload != NULL) {
        if (ByteBuffer_getSize(payload) < 1) {
            IsoClientConnection_releasePayloadBuffer(self->isoClient, payload);
            return;
        }
	}

	uint8_t* buf = ByteBuffer_getBuffer(payload);

	uint8_t tag = buf[0];

	if (DEBUG) printf("MMS-PDU: %02x\n", tag);

	if (tag == 0xa9) { /* initiate response PDU */
	    if (indication == ISO_IND_ASSOCIATION_SUCCESS) {
           self->connectionState = MMS_CON_ASSOCIATED;
        }
        else {
           self->connectionState = MMS_CON_ASSOCIATION_FAILED;
        }
        self->lastResponse = payload;

        return;
	}
	else if (tag == 0xa3) { /* unconfirmed PDU */
	    handleUnconfirmedMmsPdu(self, payload);
	    IsoClientConnection_releasePayloadBuffer(self->isoClient, payload);
	}
	else if (tag == 0xa2) { /* confirmed error PDU */
	    if (DEBUG) printf("Confirmed error PDU!\n");
	    uint32_t invokeId;
	    MmsServiceError serviceError;

	    if (parseConfirmedErrorPDU(payload, &invokeId, &serviceError) < 0) {
	        if (DEBUG) printf("Error parsing confirmedErrorPDU!\n");
	    }
	    else {
            if (checkForOutstandingCall(self, invokeId)) {

                waitUntilLastResponseHasBeenProcessed(self);

                Semaphore_wait(self->lastResponseLock);
                self->lastResponseError = convertServiceErrorToMmsError(serviceError);
                self->responseInvokeId = invokeId;
                Semaphore_post(self->lastResponseLock);
            }
            else {
                if (DEBUG) printf("unexpected message from server!\n");
                IsoClientConnection_releasePayloadBuffer(self->isoClient, payload);
                return;
            }
	    }
	}
	else if (tag == 0xa1) { /* confirmed response PDU */

	    int length;
	    int bufPos = 1;

	    bufPos = BerDecoder_decodeLength(buf, &length, bufPos, payload->size);
	    if (bufPos == -1) goto exit_with_error;

	    if (buf[bufPos++] == 0x02) {
	        int invokeIdLength;

	        bufPos = BerDecoder_decodeLength(buf, &invokeIdLength, bufPos, payload->size);
	        if (bufPos == -1) goto exit_with_error;

	        uint32_t invokeId =
	                BerDecoder_decodeUint32(buf, invokeIdLength, bufPos);

	        if (DEBUG)
	            printf("mms_client_connection: rcvd confirmed resp - invokeId: %i length: %i bufLen: %i\n",
	                    invokeId, length, payload->size);

	        bufPos += invokeIdLength;

	        if (checkForOutstandingCall(self, invokeId)) {

	            waitUntilLastResponseHasBeenProcessed(self);

	            Semaphore_wait(self->lastResponseLock);
	            self->lastResponse = payload;
	            self->lastResponseBufPos = bufPos;
	            self->responseInvokeId = invokeId;
	            Semaphore_post(self->lastResponseLock);
	        }
	        else {
	            if (DEBUG) printf("unexpected message from server!\n");
                IsoClientConnection_releasePayloadBuffer(self->isoClient, payload);
                return;
	        }
	    }
	    else
	        goto exit_with_error;
	}

	return;

exit_with_error:

    if (DEBUG) printf("received malformed message from server!\n");
    IsoClientConnection_releasePayloadBuffer(self->isoClient, payload);
	return;
}

MmsConnection
MmsConnection_create()
{
	MmsConnection self = (MmsConnection) calloc(1, sizeof(struct sMmsConnection));

	self->parameters.dataStructureNestingLevel = -1;
	self->parameters.maxServOutstandingCalled = -1;
	self->parameters.maxServOutstandingCalling = -1;
	self->parameters.maxPduSize = -1;

	self->parameters.maxPduSize = MMS_MAXIMUM_PDU_SIZE;

	self->requestTimeout = CONFIG_MMS_CONNECTION_DEFAULT_TIMEOUT;

	self->lastInvokeIdLock = Semaphore_create(1);
	self->lastResponseLock = Semaphore_create(1);
	self->outstandingCallsLock = Semaphore_create(1);

	self->lastResponseError = MMS_ERROR_NONE;

	self->outstandingCalls = (uint32_t*) calloc(OUTSTANDING_CALLS, sizeof(uint32_t));

	return self;
}

void
MmsConnection_destroy(MmsConnection self)
{
	if (self->isoClient != NULL)
		IsoClientConnection_destroy(self->isoClient);

	if (self->isoConnectionParametersSelfAllocated)
		free(self->isoParameters);

	Semaphore_destroy(self->lastInvokeIdLock);
	Semaphore_destroy(self->lastResponseLock);
	Semaphore_destroy(self->outstandingCallsLock);

	free(self->outstandingCalls);

	free(self->buffer);
	free(self);
}

void
MmsConnection_setConnectionLostHandler(MmsConnection self, MmsConnectionLostHandler handler, void* handlerParameter)
{
    self->connectionLostHandler = handler;
    self->connectionLostHandlerParameter = handlerParameter;
}

void
MmsConnection_setRequestTimeout(MmsConnection self, uint32_t timeoutInMs)
{
    self->requestTimeout = timeoutInMs;
}

void
MmsConnection_setLocalDetail(MmsConnection self, int32_t localDetail)
{
	self->parameters.maxPduSize = localDetail;
}

int32_t
MmsConnection_getLocalDetail(MmsConnection self)
{
	return self->parameters.maxPduSize;
}


void
MmsConnection_setIsoConnectionParameters(MmsConnection self, IsoConnectionParameters* params)
{
	self->isoParameters = params;
}

static void
waitForConnectResponse(MmsConnection self)
{
    uint64_t startTime = Hal_getTimeInMs();

    uint64_t waitUntilTime = startTime + self->requestTimeout;

    uint64_t currentTime = startTime;

    while (currentTime < waitUntilTime) {
        uint32_t receivedInvokeId;

        if (self->connectionState != MMS_CON_WAITING)
            break;

        Thread_sleep(10);

        currentTime = Hal_getTimeInMs();
    }

}

bool
MmsConnection_connect(MmsConnection self, MmsError* mmsError, char* serverName, int serverPort)
{
	self->isoClient = IsoClientConnection_create((IsoIndicationCallback) mmsIsoCallback, (void*) self);

	if (self->isoParameters == NULL) {
		self->isoConnectionParametersSelfAllocated = 1;
		self->isoParameters = (IsoConnectionParameters*) calloc(1, sizeof(IsoConnectionParameters));
	}

	self->isoParameters->hostname = serverName;
	self->isoParameters->tcpPort = serverPort;

	if (self->parameters.maxPduSize == -1)
		self->parameters.maxPduSize = MMS_MAXIMUM_PDU_SIZE;

	self->buffer = (uint8_t*) malloc(self->parameters.maxPduSize);

	ByteBuffer payload;

	ByteBuffer_wrap(&payload, self->buffer, 0, self->parameters.maxPduSize);

	mmsClient_createInitiateRequest(self, &payload);

	self->connectionState = MMS_CON_WAITING;

	IsoClientConnection_associate(self->isoClient, self->isoParameters, &payload);

	waitForConnectResponse(self);

	if (DEBUG)
	    printf("MmsConnection_connect: received response conState: %i\n", self->connectionState);

	if (self->connectionState == MMS_CON_ASSOCIATED) {
		mmsClient_parseInitiateResponse(self);

		releaseResponse(self);

		self->associationState = MMS_STATE_CONNECTED;
	}
	else {
		self->associationState = MMS_STATE_CLOSED;
	}

	self->connectionState = MMS_CON_IDLE;

	if (DEBUG)
	        printf("MmsConnection_connect: states: con %i ass %i\n", self->connectionState, self->associationState);

	if (self->associationState == MMS_STATE_CONNECTED) {
	    *mmsError = MMS_ERROR_NONE;
		return true;
	}
	else {
	    *mmsError = MMS_ERROR_CONNECTION_REJECTED;
		return false;
	}
}


void
MmsConnection_setInformationReportHandler(MmsConnection self, MmsInformationReportHandler handler,
        void* parameter)
{
    self->reportHandler = handler;
    self->reportHandlerParameter = parameter;
}

bool
mmsClient_getNameListSingleRequest(
		LinkedList* nameList,
		MmsConnection self,
		MmsError* mmsError,
		char* domainId,
		MmsObjectClass objectClass,
		bool associationSpecific,
		char* continueAfter)
{
	ByteBuffer payload;
	ByteBuffer_wrap(&payload, self->buffer, 0, self->parameters.maxPduSize);

	*mmsError = MMS_ERROR_NONE;

	uint32_t invokeId = getNextInvokeId(self);

	if (associationSpecific)
		mmsClient_createMmsGetNameListRequestAssociationSpecific(invokeId,
				&payload, continueAfter);
	else {

		if (domainId == NULL)
			mmsClient_createMmsGetNameListRequestVMDspecific(invokeId,
					&payload, continueAfter);
		else
			mmsClient_createGetNameListRequestDomainSpecific(invokeId, domainId,
					&payload, objectClass, continueAfter);
	}

	ByteBuffer* responseMessage = sendRequestAndWaitForResponse(self, invokeId, &payload);

	bool moreFollows = false;

//    if (self->lastResponseError != MMS_ERROR_NONE)
//        *mmsError = self->lastResponseError;



    //else
    if (responseMessage != NULL)
        moreFollows = mmsClient_parseGetNameListResponse(nameList, self->lastResponse, NULL);

    releaseResponse(self);

    if (self->associationState == MMS_STATE_CLOSED)
        *mmsError = MMS_ERROR_CONNECTION_LOST;

	return moreFollows;
}


static LinkedList /* <char*> */
mmsClient_getNameList(MmsConnection self, MmsError *mmsError,
		char* domainId,
		MmsObjectClass objectClass,
		bool associationSpecific)
{
	LinkedList list = NULL;

	bool moreFollows;

	moreFollows = mmsClient_getNameListSingleRequest(&list, self, mmsError, domainId,
					 objectClass, associationSpecific, NULL);

	while ((moreFollows == true) && (list != NULL)) {
		LinkedList lastElement = LinkedList_getLastElement(list);

		char* lastIdentifier = (char*) lastElement->data;

		if (DEBUG) printf("getNameList: identifier: %s\n", lastIdentifier);

		moreFollows = mmsClient_getNameListSingleRequest(&list, self, mmsError, domainId,
							 objectClass, associationSpecific, lastIdentifier);
	}

	return list;
}

LinkedList /* <char*> */
MmsConnection_getDomainNames(MmsConnection self, MmsError* mmsError) {
	return mmsClient_getNameList(self, mmsError, NULL, MMS_NAMED_VARIABLE, false);
}

LinkedList /* <char*> */
MmsConnection_getDomainVariableNames(MmsConnection self, MmsError* mmsError, char* domainId) {
	return mmsClient_getNameList(self, mmsError, domainId, MMS_NAMED_VARIABLE, false);
}

LinkedList /* <char*> */
MmsConnection_getDomainVariableListNames(MmsConnection self, MmsError* mmsError, char* domainId) {
	return mmsClient_getNameList(self, mmsError, domainId, MMS_NAMED_VARIABLE_LIST, false);
}

LinkedList /* <char*> */
MmsConnection_getVariableListNamesAssociationSpecific(MmsConnection self, MmsError* mmsError)
{
	return mmsClient_getNameList(self, mmsError, NULL, MMS_NAMED_VARIABLE_LIST, true);
}

MmsValue*
MmsConnection_readVariable(MmsConnection self, MmsError* mmsError,
        char* domainId, char* itemId)
{
    ByteBuffer payload;
    MmsValue* value = NULL;
    ByteBuffer_wrap(&payload, self->buffer, 0, self->parameters.maxPduSize);

    *mmsError = MMS_ERROR_NONE;

    uint32_t invokeId = getNextInvokeId(self);

    mmsClient_createReadRequest(invokeId, domainId, itemId, &payload);

    ByteBuffer* responseMessage = sendRequestAndWaitForResponse(self, invokeId, &payload);

    if (self->lastResponseError != MMS_ERROR_NONE)
        *mmsError = self->lastResponseError;
    else if (responseMessage != NULL)
        value = mmsClient_parseReadResponse(self->lastResponse, NULL, false);

    releaseResponse(self);

    if (self->associationState == MMS_STATE_CLOSED)
        *mmsError = MMS_ERROR_CONNECTION_LOST;

    return value;
}

MmsValue*
MmsConnection_readArrayElements(MmsConnection self, MmsError* mmsError,
		char* domainId, char* itemId,
		uint32_t startIndex, uint32_t numberOfElements)
{
	ByteBuffer payload;
	MmsValue* value = NULL;
	ByteBuffer_wrap(&payload, self->buffer, 0, self->parameters.maxPduSize);

	*mmsError = MMS_ERROR_NONE;

	uint32_t invokeId = getNextInvokeId(self);

	mmsClient_createReadRequestAlternateAccessIndex(invokeId, domainId, itemId, startIndex,
	        numberOfElements, &payload);

	ByteBuffer* responseMessage = sendRequestAndWaitForResponse(self, invokeId, &payload);

    if (self->lastResponseError != MMS_ERROR_NONE)
        *mmsError = self->lastResponseError;
    else if (responseMessage != NULL)
        value = mmsClient_parseReadResponse(self->lastResponse, NULL, true);

    releaseResponse(self);

    if (self->associationState == MMS_STATE_CLOSED)
        *mmsError = MMS_ERROR_CONNECTION_LOST;

	return value;
}

MmsValue*
MmsConnection_readMultipleVariables(MmsConnection self, MmsError* mmsError,
		char* domainId, LinkedList /*<char*>*/ items)
{
	ByteBuffer payload;
	MmsValue* value = NULL;
	ByteBuffer_wrap(&payload, self->buffer, 0, self->parameters.maxPduSize);

	*mmsError = MMS_ERROR_NONE;

	uint32_t invokeId = getNextInvokeId(self);

	mmsClient_createReadRequestMultipleValues(invokeId, domainId, items, &payload);

    ByteBuffer* responseMessage = sendRequestAndWaitForResponse(self, invokeId, &payload);

    if (self->lastResponseError != MMS_ERROR_NONE)
        *mmsError = self->lastResponseError;
    else if (responseMessage != NULL)
        value = mmsClient_parseReadResponse(self->lastResponse, NULL, true);

    releaseResponse(self);

    if (self->associationState == MMS_STATE_CLOSED)
        *mmsError = MMS_ERROR_CONNECTION_LOST;

	return value;
}

MmsValue*
MmsConnection_readNamedVariableListValues(MmsConnection self, MmsError* mmsError,
		char* domainId, char* listName,
		bool specWithResult)
{
	ByteBuffer payload;
	ByteBuffer_wrap(&payload, self->buffer, 0, self->parameters.maxPduSize);

	MmsValue* value = NULL;

	*mmsError = MMS_ERROR_NONE;

	uint32_t invokeId = getNextInvokeId(self);

	mmsClient_createReadNamedVariableListRequest(invokeId, domainId, listName,
			&payload, specWithResult);

	ByteBuffer* responseMessage = sendRequestAndWaitForResponse(self, invokeId, &payload);

	if (self->lastResponseError != MMS_ERROR_NONE) {
	    *mmsError = self->lastResponseError;
	}
	else if (responseMessage != NULL) {
		value = mmsClient_parseReadResponse(self->lastResponse, NULL, true);
	}

	releaseResponse(self);

	if (self->associationState == MMS_STATE_CLOSED)
	    *mmsError = MMS_ERROR_CONNECTION_LOST;

	return value;
}

MmsValue*
MmsConnection_readNamedVariableListValuesAssociationSpecific(
		MmsConnection self, MmsError* mmsError,
		char* listName,
		bool specWithResult)
{
	ByteBuffer payload;
	ByteBuffer_wrap(&payload, self->buffer, 0, self->parameters.maxPduSize);

	MmsValue* value = NULL;

	*mmsError = MMS_ERROR_NONE;

	uint32_t invokeId = getNextInvokeId(self);

	mmsClient_createReadAssociationSpecificNamedVariableListRequest(invokeId, listName,
			&payload, specWithResult);

	ByteBuffer* responseMessage = sendRequestAndWaitForResponse(self, invokeId, &payload);

    if (self->lastResponseError != MMS_ERROR_NONE)
        *mmsError = self->lastResponseError;
    else if (responseMessage != NULL)
        value = mmsClient_parseReadResponse(self->lastResponse, NULL, true);

    releaseResponse(self);

    if (self->associationState == MMS_STATE_CLOSED)
        *mmsError = MMS_ERROR_CONNECTION_LOST;


	return value;
}

LinkedList /* <MmsVariableAccessSpecification*> */
MmsConnection_readNamedVariableListDirectory(MmsConnection self,  MmsError* mmsError,
		char* domainId, char* listName,	bool* deletable)
{
	ByteBuffer payload;
	ByteBuffer_wrap(&payload, self->buffer, 0, self->parameters.maxPduSize);

	*mmsError = MMS_ERROR_NONE;

	LinkedList attributes = NULL;

	uint32_t invokeId = getNextInvokeId(self);

	mmsClient_createGetNamedVariableListAttributesRequest(invokeId, &payload, domainId,
			listName);

	ByteBuffer* responseMessage = sendRequestAndWaitForResponse(self, invokeId, &payload);

    if (self->lastResponseError != MMS_ERROR_NONE)
        *mmsError = self->lastResponseError;
    else if (responseMessage != NULL)
        attributes = mmsClient_parseGetNamedVariableListAttributesResponse(self->lastResponse, NULL,
                        deletable);

    releaseResponse(self);

    if (self->associationState == MMS_STATE_CLOSED)
        *mmsError = MMS_ERROR_CONNECTION_LOST;

	return attributes;
}

MmsIndication
MmsConnection_defineNamedVariableList(MmsConnection self,  MmsError* mmsError,
		char* domainId, char* listName, LinkedList variableSpecs)
{
	ByteBuffer payload;
	ByteBuffer_wrap(&payload, self->buffer, 0, self->parameters.maxPduSize);

	*mmsError = MMS_ERROR_NONE;

	uint32_t invokeId = getNextInvokeId(self);

	mmsClient_createDefineNamedVariableListRequest(invokeId, &payload, domainId,
			listName, variableSpecs, false);

	MmsIndication indication = MMS_ERROR;

	ByteBuffer* responseMessage = sendRequestAndWaitForResponse(self, invokeId, &payload);


    if (self->lastResponseError != MMS_ERROR_NONE)
        *mmsError = self->lastResponseError;
    else if (responseMessage != NULL)
        indication = mmsClient_parseDefineNamedVariableResponse(self->lastResponse, NULL);

    releaseResponse(self);

    if (self->associationState == MMS_STATE_CLOSED)
        *mmsError = MMS_ERROR_CONNECTION_LOST;

	return indication;
}

MmsIndication
MmsConnection_defineNamedVariableListAssociationSpecific(MmsConnection self,
        MmsError* mmsError, char* listName, LinkedList variableSpecs)
{
	ByteBuffer payload;
	ByteBuffer_wrap(&payload, self->buffer, 0, self->parameters.maxPduSize);

	*mmsError = MMS_ERROR_NONE;

	uint32_t invokeId = getNextInvokeId(self);

	mmsClient_createDefineNamedVariableListRequest(invokeId, &payload, NULL,
			listName, variableSpecs, true);

	ByteBuffer* responseMessage = sendRequestAndWaitForResponse(self, invokeId, &payload);

	MmsIndication indication = MMS_ERROR;

    if (self->lastResponseError != MMS_ERROR_NONE)
        *mmsError = self->lastResponseError;
    else if (responseMessage != NULL)
        indication = mmsClient_parseDefineNamedVariableResponse(self->lastResponse, NULL);

    releaseResponse(self);

    if (self->associationState == MMS_STATE_CLOSED)
              *mmsError = MMS_ERROR_CONNECTION_LOST;

	return indication;
}

MmsIndication
MmsConnection_deleteNamedVariableList(MmsConnection self,  MmsError* mmsError,
		char* domainId, char* listName)
{
	ByteBuffer payload;
	ByteBuffer_wrap(&payload, self->buffer, 0, self->parameters.maxPduSize);

	*mmsError = MMS_ERROR_NONE;

	uint32_t invokeId = getNextInvokeId(self);

	mmsClient_createDeleteNamedVariableListRequest(invokeId, &payload, domainId, listName);

	MmsIndication indication = MMS_ERROR;

	ByteBuffer* responseMessage = sendRequestAndWaitForResponse(self, invokeId, &payload);

    if (self->lastResponseError != MMS_ERROR_NONE)
        *mmsError = self->lastResponseError;
    else if (responseMessage != NULL)
        indication = mmsClient_parseDeleteNamedVariableListResponse(self->lastResponse,NULL);

    releaseResponse(self);

    if (self->associationState == MMS_STATE_CLOSED)
        *mmsError = MMS_ERROR_CONNECTION_LOST;

	return indication;
}

MmsIndication
MmsConnection_deleteAssociationSpecificNamedVariableList(MmsConnection self,
        MmsError* mmsError, char* listName)
{
	ByteBuffer payload;
	ByteBuffer_wrap(&payload, self->buffer, 0, self->parameters.maxPduSize);

	*mmsError = MMS_ERROR_NONE;

	uint32_t invokeId = getNextInvokeId(self);

	mmsClient_createDeleteAssociationSpecificNamedVariableListRequest(
	        invokeId, &payload, listName);

	MmsIndication indication = MMS_ERROR;

	ByteBuffer* responseMessage = sendRequestAndWaitForResponse(self, invokeId, &payload);

    if (self->lastResponseError != MMS_ERROR_NONE)
        *mmsError = self->lastResponseError;
    else if (responseMessage != NULL)
        indication = mmsClient_parseDeleteNamedVariableListResponse(self->lastResponse, NULL);

    releaseResponse(self);

    if (self->associationState == MMS_STATE_CLOSED)
        *mmsError = MMS_ERROR_CONNECTION_LOST;

	return indication;
}

MmsVariableSpecification*
MmsConnection_getVariableAccessAttributes(MmsConnection self,  MmsError* mmsError,
		char* domainId, char* itemId)
{
	ByteBuffer payload;
	ByteBuffer_wrap(&payload, self->buffer, 0, self->parameters.maxPduSize);
	MmsVariableSpecification* typeSpec = NULL;

	*mmsError = MMS_ERROR_NONE;

	uint32_t invokeId = getNextInvokeId(self);

	mmsClient_createGetVariableAccessAttributesRequest(invokeId, domainId, itemId, &payload);

	ByteBuffer* responseMessage = sendRequestAndWaitForResponse(self, invokeId, &payload);

    if (self->lastResponseError != MMS_ERROR_NONE)
        *mmsError = self->lastResponseError;
    else if (responseMessage != NULL)
        typeSpec = mmsClient_parseGetVariableAccessAttributesResponse(self->lastResponse, NULL);

    releaseResponse(self);

    if (self->associationState == MMS_STATE_CLOSED)
        *mmsError = MMS_ERROR_CONNECTION_LOST;

	return typeSpec;
}

MmsServerIdentity*
MmsConnection_identify(MmsConnection self, MmsError* mmsError)
{
    ByteBuffer payload;
    ByteBuffer_wrap(&payload, self->buffer, 0, self->parameters.maxPduSize);
    MmsServerIdentity* identity = NULL;

    *mmsError = MMS_ERROR_NONE;

    uint32_t invokeId = getNextInvokeId(self);

    mmsClient_createIdentifyRequest(invokeId, &payload);

    ByteBuffer* responseMessage = sendRequestAndWaitForResponse(self, invokeId, &payload);

    if (self->lastResponseError != MMS_ERROR_NONE)
        *mmsError = self->lastResponseError;
    else if (responseMessage != NULL)
        identity = mmsClient_parseIdentifyResponse(self->lastResponse, NULL);

    releaseResponse(self);

    if (self->associationState == MMS_STATE_CLOSED)
        *mmsError = MMS_ERROR_CONNECTION_LOST;

    return identity;
}

MmsIndication
MmsConnection_writeVariable(MmsConnection self, MmsError* mmsError,
		char* domainId, char* itemId,
		MmsValue* value)
{
    MmsIndication indication = MMS_ERROR;
	ByteBuffer payload;
	ByteBuffer_wrap(&payload, self->buffer, 0, self->parameters.maxPduSize);

	*mmsError = MMS_ERROR_NONE;

	uint32_t invokeId = getNextInvokeId(self);

	mmsClient_createWriteRequest(invokeId, domainId, itemId, value, &payload);

	ByteBuffer* responseMessage = sendRequestAndWaitForResponse(self, invokeId, &payload);

    if (self->lastResponseError != MMS_ERROR_NONE)
        *mmsError = self->lastResponseError;
    else if (responseMessage != NULL)
        indication = mmsClient_parseWriteResponse(self->lastResponse);

    releaseResponse(self);

    if (self->associationState == MMS_STATE_CLOSED)
        *mmsError = MMS_ERROR_CONNECTION_LOST;

	return indication;
}

MmsIndication
MmsConnection_writeMultipleVariables(MmsConnection self, MmsError* mmsError, char* domainId,
        LinkedList /*<char*>*/ items,
        LinkedList /* <MmsValue*> */ values,
        /* OUTPUT */ LinkedList* /* <MmsValue*> */ accessResults)
{
    MmsIndication indication = MMS_ERROR;
    ByteBuffer payload;
    ByteBuffer_wrap(&payload, self->buffer, 0, self->parameters.maxPduSize);

    *mmsError = MMS_ERROR_NONE;

    uint32_t invokeId = getNextInvokeId(self);

    mmsClient_createWriteMultipleItemsRequest(invokeId, domainId, items, values, &payload);

    ByteBuffer* responseMessage = sendRequestAndWaitForResponse(self, invokeId, &payload);

    if (self->lastResponseError != MMS_ERROR_NONE)
        *mmsError = self->lastResponseError;
    else if (responseMessage != NULL) {

        int numberOfItems = LinkedList_size(items);

        indication = mmsClient_parseWriteMultipleItemsResponse(self->lastResponse, numberOfItems, accessResults);

        if (indication != MMS_OK)
            *mmsError = MMS_ERROR_PARSING_RESPONSE;

    }

    releaseResponse(self);

    if (self->associationState == MMS_STATE_CLOSED)
        *mmsError = MMS_ERROR_CONNECTION_LOST;

    return indication;
}

void
MmsServerIdentity_destroy(MmsServerIdentity* self)
{
    if (self->modelName != NULL)
        free(self->modelName);

    if (self->vendorName != NULL)
        free(self->vendorName);

    if (self->revision != NULL)
        free(self->revision);

    free(self);
}

MmsVariableAccessSpecification*
MmsVariableAccessSpecification_create(char* domainId, char* itemId)
{
	MmsVariableAccessSpecification* self = (MmsVariableAccessSpecification*) 
				malloc(sizeof(MmsVariableAccessSpecification));

	self->domainId = domainId;
	self->itemId = itemId;
	self->arrayIndex = -1;
	self->componentName = NULL;

	return self;
}

MmsVariableAccessSpecification*
MmsVariableAccessSpecification_createAlternateAccess(char* domainId, char* itemId, int32_t index,
		char* componentName)
{
	MmsVariableAccessSpecification* self = (MmsVariableAccessSpecification*) 
		malloc(sizeof(MmsVariableAccessSpecification));

	self->domainId = domainId;
	self->itemId = itemId;
	self->arrayIndex = index;
	self->componentName = componentName;

	return self;
}

void
MmsVariableAccessSpecification_destroy(MmsVariableAccessSpecification* self)
{
	if (self->domainId != NULL)
		free(self->domainId);

	if (self->itemId != NULL)
		free(self->itemId);

	if (self->componentName != NULL)
		free (self->componentName);

	free(self);
}

