/*
 *  mms_client_write.c
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

#include <MmsPdu.h>
#include "mms_common.h"
#include "mms_client_connection.h"
#include "byte_buffer.h"

#include "mms_client_internal.h"
#include "mms_common_internal.h"

#include "stack_config.h"

MmsIndication
mmsClient_parseWriteMultipleItemsResponse(ByteBuffer* message, int itemCount, LinkedList* accessResults)
{
    MmsPdu_t* mmsPdu = 0;
    MmsIndication retVal =  MMS_OK;

    asn_dec_rval_t rval;

    rval = ber_decode(NULL, &asn_DEF_MmsPdu,
            (void**) &mmsPdu, ByteBuffer_getBuffer(message), ByteBuffer_getSize(message));

    if (rval.code != RC_OK) {
        retVal = MMS_ERROR;
        goto cleanUp;
    }

    if (DEBUG) xer_fprint(stdout, &asn_DEF_MmsPdu, mmsPdu);

    if (mmsPdu->present == MmsPdu_PR_confirmedResponsePdu) {

        if (mmsPdu->choice.confirmedResponsePdu.confirmedServiceResponse.present == ConfirmedServiceResponse_PR_write)
        {
            WriteResponse_t* response =
                    &(mmsPdu->choice.confirmedResponsePdu.confirmedServiceResponse.choice.write);

            if (response->list.count == itemCount) {

                int i;

                *accessResults = LinkedList_create();

                for (i = 0; i < itemCount; i++) {

                    MmsValue* value;

                    if (response->list.array[i]->present == WriteResponse__Member_PR_success) {
                        MmsDataAccessError error;
                        value = MmsValue_newDataAccessError(DATA_ACCESS_ERROR_SUCCESS);
                    }
                    else {
                        long errorCode;

                        asn_INTEGER2long(&response->list.array[i]->choice.failure, &errorCode);

                        value = MmsValue_newDataAccessError((MmsDataAccessError) errorCode);
                    }

                    LinkedList_add(*accessResults, (void*) value);
                }
            }
            else
                retVal = MMS_ERROR;

        }
        else {
            retVal = MMS_ERROR;
        }
    }
    else {
        retVal = MMS_ERROR;
    }


 cleanUp:
    asn_DEF_MmsPdu.free_struct(&asn_DEF_MmsPdu, mmsPdu, 0);

    return retVal;
}

MmsIndication
mmsClient_parseWriteResponse(ByteBuffer* message)
{
	MmsPdu_t* mmsPdu = 0;
	MmsIndication retVal =  MMS_OK;

	asn_dec_rval_t rval;

	rval = ber_decode(NULL, &asn_DEF_MmsPdu,
			(void**) &mmsPdu, ByteBuffer_getBuffer(message), ByteBuffer_getSize(message));

	if (rval.code != RC_OK) {
		retVal = MMS_ERROR;
		goto cleanUp;
	}

	if (DEBUG) xer_fprint(stdout, &asn_DEF_MmsPdu, mmsPdu);

	if (mmsPdu->present == MmsPdu_PR_confirmedResponsePdu) {

		if (mmsPdu->choice.confirmedResponsePdu.confirmedServiceResponse.present == ConfirmedServiceResponse_PR_write)
		{
			WriteResponse_t* response =
					&(mmsPdu->choice.confirmedResponsePdu.confirmedServiceResponse.choice.write);

			if (response->list.count > 0) {
				if (response->list.array[0]->present == WriteResponse__Member_PR_success)
					retVal = MMS_OK;
				else
					retVal = MMS_ERROR;
			}
			else
				retVal = MMS_ERROR;

		}
		else {
			retVal = MMS_ERROR;
		}
	}
	else {
		retVal = MMS_ERROR;
	}

cleanUp:
	asn_DEF_MmsPdu.free_struct(&asn_DEF_MmsPdu, mmsPdu, 0);

	return retVal;
}


static VariableSpecification_t*
createNewDomainVariableSpecification(char* domainId, char* itemId)
{
	VariableSpecification_t* varSpec = (VariableSpecification_t*) calloc(1, sizeof(ListOfVariableSeq_t));
	varSpec->present = VariableSpecification_PR_name;
	varSpec->choice.name.present = ObjectName_PR_domainspecific;
	varSpec->choice.name.choice.domainspecific.domainId.buf = (uint8_t*) domainId;
	varSpec->choice.name.choice.domainspecific.domainId.size = strlen(domainId);
	varSpec->choice.name.choice.domainspecific.itemId.buf = (uint8_t*) itemId;
	varSpec->choice.name.choice.domainspecific.itemId.size = strlen(itemId);

	return varSpec;
}

static void
deleteDataElement(Data_t* dataElement)
{
    if (dataElement == NULL ) {
        printf("deleteDataElement NULL argument\n");
        return;
    }

    if (dataElement->present == Data_PR_structure) {
        int elementCount = dataElement->choice.structure->list.count;

        int i;
        for (i = 0; i < elementCount; i++) {
            deleteDataElement(dataElement->choice.structure->list.array[i]);
        }

        free(dataElement->choice.structure->list.array);
        free(dataElement->choice.structure);
    }
    else if (dataElement->present == Data_PR_array) {
        int elementCount = dataElement->choice.array->list.count;

        int i;
        for (i = 0; i < elementCount; i++) {
            deleteDataElement(dataElement->choice.array->list.array[i]);
        }

        free(dataElement->choice.array->list.array);
        free(dataElement->choice.array);
    }
    else if (dataElement->present == Data_PR_floatingpoint) {
        free(dataElement->choice.floatingpoint.buf);
    }
    else if (dataElement->present == Data_PR_utctime) {
        free(dataElement->choice.utctime.buf);
    }

    free(dataElement);
}

int
mmsClient_createWriteMultipleItemsRequest(uint32_t invokeId, char* domainId, LinkedList itemIds, LinkedList values,
        ByteBuffer* writeBuffer)
{
    MmsPdu_t* mmsPdu = mmsClient_createConfirmedRequestPdu(invokeId);

    mmsPdu->choice.confirmedRequestPdu.confirmedServiceRequest.present =
            ConfirmedServiceRequest_PR_write;
    WriteRequest_t* request =
            &(mmsPdu->choice.confirmedRequestPdu.confirmedServiceRequest.choice.write);

    int numberOfItems = LinkedList_size(itemIds);

    /* Create list of variable specifications */
    request->variableAccessSpecification.present = VariableAccessSpecification_PR_listOfVariable;
    request->variableAccessSpecification.choice.listOfVariable.list.count = numberOfItems;
    request->variableAccessSpecification.choice.listOfVariable.list.size = numberOfItems;
    request->variableAccessSpecification.choice.listOfVariable.list.array =
            (ListOfVariableSeq_t**) calloc(numberOfItems, sizeof(ListOfVariableSeq_t*));

    /* Create list of data values */
    request->listOfData.list.count = numberOfItems;
    request->listOfData.list.size = numberOfItems;
    request->listOfData.list.array = (Data_t**) calloc(numberOfItems, sizeof(struct Data*));

    int i;

    LinkedList item = LinkedList_getNext(itemIds);
    LinkedList valueElement = LinkedList_getNext(values);

    for (i = 0; i < numberOfItems; i++) {
        if (item == NULL) return -1;
        if (valueElement == NULL) return -1;

        char* itemId = (char*) item->data;
        MmsValue* value = (MmsValue*) valueElement->data;

        request->variableAccessSpecification.choice.listOfVariable.list.array[i] = (ListOfVariableSeq_t*)
                    createNewDomainVariableSpecification(domainId, itemId);

        request->listOfData.list.array[i] = mmsMsg_createBasicDataElement(value);

        item = LinkedList_getNext(item);
    }

    asn_enc_rval_t rval;

    rval = der_encode(&asn_DEF_MmsPdu, mmsPdu,
            (asn_app_consume_bytes_f*) mmsClient_write_out, (void*) writeBuffer);

    if (DEBUG) xer_fprint(stdout, &asn_DEF_MmsPdu, mmsPdu);

    /* Free ASN structure */
    request->variableAccessSpecification.choice.listOfVariable.list.count = 0;

    for (i = 0; i < numberOfItems; i++) {
        free(request->variableAccessSpecification.choice.listOfVariable.list.array[i]);
        deleteDataElement(request->listOfData.list.array[i]);

    }

    free(request->variableAccessSpecification.choice.listOfVariable.list.array);
    request->variableAccessSpecification.choice.listOfVariable.list.array = 0;

    request->listOfData.list.count = 0;
    free(request->listOfData.list.array);
    request->listOfData.list.array = 0;

    asn_DEF_MmsPdu.free_struct(&asn_DEF_MmsPdu, mmsPdu, 0);

    return rval.encoded;

}

int
mmsClient_createWriteRequest(uint32_t invokeId, char* domainId, char* itemId, MmsValue* value,
		ByteBuffer* writeBuffer)
{
	//TODO reuse code to send information report!

	MmsPdu_t* mmsPdu = mmsClient_createConfirmedRequestPdu(invokeId);

	mmsPdu->choice.confirmedRequestPdu.confirmedServiceRequest.present =
			ConfirmedServiceRequest_PR_write;
	WriteRequest_t* request =
			&(mmsPdu->choice.confirmedRequestPdu.confirmedServiceRequest.choice.write);

	/* Create list of variable specifications */
	request->variableAccessSpecification.present = VariableAccessSpecification_PR_listOfVariable;
	request->variableAccessSpecification.choice.listOfVariable.list.count = 1;
	request->variableAccessSpecification.choice.listOfVariable.list.size = 1;
	request->variableAccessSpecification.choice.listOfVariable.list.array =
			(ListOfVariableSeq_t**) calloc(1, sizeof(ListOfVariableSeq_t*));
	request->variableAccessSpecification.choice.listOfVariable.list.array[0] = (ListOfVariableSeq_t*)
			createNewDomainVariableSpecification(domainId, itemId);

	/* Create list of typed data values */
	request->listOfData.list.count = 1;
	request->listOfData.list.size = 1;
	request->listOfData.list.array = (Data_t**) calloc(1, sizeof(struct Data*));
	request->listOfData.list.array[0] = mmsMsg_createBasicDataElement(value);

	asn_enc_rval_t rval;

	rval = der_encode(&asn_DEF_MmsPdu, mmsPdu,
			(asn_app_consume_bytes_f*) mmsClient_write_out, (void*) writeBuffer);

	if (DEBUG) xer_fprint(stdout, &asn_DEF_MmsPdu, mmsPdu);

	/* Free ASN structure */
	request->variableAccessSpecification.choice.listOfVariable.list.count = 0;

	free(request->variableAccessSpecification.choice.listOfVariable.list.array[0]);
	free(request->variableAccessSpecification.choice.listOfVariable.list.array);
	request->variableAccessSpecification.choice.listOfVariable.list.array = 0;

	request->listOfData.list.count = 0;

	deleteDataElement(request->listOfData.list.array[0]);

	free(request->listOfData.list.array);
	request->listOfData.list.array = 0;

	asn_DEF_MmsPdu.free_struct(&asn_DEF_MmsPdu, mmsPdu, 0);

	return rval.encoded;
}
