/*
 *  mms_read_service.c
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

#include "mms_server_internal.h"
#include "mms_common_internal.h"

#include "mms_access_result.h"

#include "linked_list.h"

#include "ber_encoder.h"

/**********************************************************************************************
 * MMS Read Service
 *********************************************************************************************/

typedef struct sVarAccessSpec {
	bool isNamedVariableList;
	int specific; /* 0 - vmd, 1 - domain, 2 - association */

	char* itemId;
	char* domainId;
} VarAccessSpec;

static MmsValue*
addNamedVariableValue(MmsVariableSpecification* namedVariable, MmsServerConnection* connection,
		MmsDomain* domain, char* itemId, LinkedList typedValues)
{
	MmsValue* value = NULL;

	if (namedVariable->type == MMS_STRUCTURE) {

	    value = mmsServer_getValue(connection->server, domain, itemId, connection);

	    if (value != NULL) {
	        LinkedList_add(typedValues, value);
	    }
	    else {

            int componentCount = namedVariable->typeSpec.structure.elementCount;

            MmsValue* value = MmsValue_createEmptyStructure(componentCount);

            int i;

            for (i = 0; i < componentCount; i++) {
                char* newNameIdStr = createString(3, itemId, "$",
                        namedVariable->typeSpec.structure.elements[i]->name);

                MmsValue* element =
                        addNamedVariableValue(namedVariable->typeSpec.structure.elements[i],
                                connection, domain,	newNameIdStr, typedValues);

                MmsValue_setElement(value, i, element);

                free(newNameIdStr);
            }
	    }
	}
	else {
		value = mmsServer_getValue(connection->server, domain, itemId, connection);

		//TODO add checks???

		if (value != NULL)
			LinkedList_add(typedValues, value);
		else {
			if (DEBUG)
			    printf("Error getting element %s\n", itemId);
		}
	}

	return value;
}

static void
addComplexValueToResultList(MmsVariableSpecification* namedVariable,
								LinkedList typedValues, MmsServerConnection* connection,
								MmsDomain* domain, char* nameIdStr)
{
	int componentCount = namedVariable->typeSpec.structure.elementCount;

	int i;
	for (i = 0; i < componentCount; i++) {

	    char* componentName = namedVariable->typeSpec.structure.elements[i]->name;

	    int bufLen = strlen(nameIdStr) + strlen(componentName) + sizeof('$')
	            + 1 /* for string terminator */;

	    char* newNameIdStr = (char*) alloca(bufLen);

		createStringInBuffer(newNameIdStr, 3, nameIdStr, "$", componentName);


		addNamedVariableValue(namedVariable->typeSpec.structure.elements[i],
				connection, domain, newNameIdStr,typedValues);
	}
}


static void
appendValueToResultList(MmsValue* value, LinkedList values)
{

	if (value != NULL )
		LinkedList_add(values, value);
}

static void
appendErrorToResultList(LinkedList values, uint32_t errorCode) {
    MmsValue* value = MmsValue_newDataAccessError((MmsDataAccessError) errorCode);
    MmsValue_setDeletable(value);
    appendValueToResultList(value, values);
}

static void
deleteValueList(LinkedList values)
{
	LinkedList value = values;
	MmsValue* typedValue;
	while ((value = LinkedList_getNext(value)) != NULL ) {
		typedValue = (MmsValue*) (value->data);

		if (typedValue->deleteValue == 1) {
			MmsValue_delete(typedValue);
		}
	}
	LinkedList_destroyStatic(values);
}

static bool
isAccessToArrayComponent(AlternateAccess_t* alternateAccess)
{
    if (alternateAccess->list.array[0]->choice.unnamed->choice.selectAlternateAccess.
                alternateAccess != NULL)
        return true;
    else
        return false;
}

static MmsValue*
getComponentOfArrayElement(AlternateAccess_t* alternateAccess, MmsVariableSpecification* namedVariable,
        MmsValue* structuredValue)
{
    if (isAccessToArrayComponent(alternateAccess))
    {
        Identifier_t component = alternateAccess->list.array[0]->choice.unnamed->choice.selectAlternateAccess.alternateAccess
                ->list.array[0]->choice.unnamed->choice.selectAccess.choice.component;

        if (component.size > 129)
            return NULL;

        int elementCount = namedVariable->typeSpec.structure.elementCount;


        MmsVariableSpecification* structSpec = namedVariable->typeSpec.array.elementTypeSpec;

        int i;
        for (i = 0; i < elementCount; i++) {
            if (strncmp (structSpec->typeSpec.structure.elements[i]->name, (char*) component.buf,
                    component.size) == 0)
            {
                MmsValue* value = MmsValue_getElement(structuredValue, i);
                return value;
            }
        }
    }

    return NULL;
}

static void
alternateArrayAccess(MmsServerConnection* connection,
		AlternateAccess_t* alternateAccess, MmsDomain* domain,
		char* itemId, LinkedList values,
		MmsVariableSpecification* namedVariable)
{
	if (mmsServer_isIndexAccess(alternateAccess))
	{
		int lowIndex = mmsServer_getLowIndex(alternateAccess);
		int numberOfElements = mmsServer_getNumberOfElements(alternateAccess);

		if (DEBUG) printf("Alternate access index: %i elements %i\n",
				lowIndex, numberOfElements);

		int index = lowIndex;

		MmsValue* value = NULL;

		MmsValue* arrayValue = mmsServer_getValue(connection->server, domain, itemId, connection);

		if (arrayValue != NULL) {

			if (numberOfElements == 0)
			    if (isAccessToArrayComponent(alternateAccess)) {
			        if (namedVariable->typeSpec.array.elementTypeSpec->type == MMS_STRUCTURE) {
			            MmsValue* structValue = MmsValue_getElement(arrayValue, index);

			            if (structValue != NULL)
			                value = getComponentOfArrayElement(alternateAccess,
			                        namedVariable, structValue);
			        }
			    }
			    else
			        value = MmsValue_getElement(arrayValue, index);
			else {
				value = MmsValue_createEmtpyArray(numberOfElements);

				MmsValue_setDeletable(value);

				int resultIndex = 0;
				while (index < lowIndex + numberOfElements) {
					MmsValue* elementValue = NULL;

					elementValue = MmsValue_getElement(arrayValue, index);

					if (!MmsValue_isDeletable(elementValue))
						elementValue = MmsValue_clone(elementValue);

					MmsValue_setElement(value, resultIndex, elementValue);

					index++;
					resultIndex++;
				}
			}

			appendValueToResultList(value, values);

			if (MmsValue_isDeletable(arrayValue))
				MmsValue_delete(arrayValue);

		}
		else  /* access error */
			appendErrorToResultList(values, 10 /* object-non-existant*/);

	}
	else { // invalid access
		if (DEBUG) printf("Invalid alternate access\n");
        appendErrorToResultList(values, 10 /* object-non-existant*/);
	}
}

static void
addNamedVariableToResultList(MmsVariableSpecification* namedVariable, MmsDomain* domain, char* nameIdStr,
		LinkedList /*<MmsValue>*/ values, MmsServerConnection* connection, AlternateAccess_t* alternateAccess)
{
	if (namedVariable != NULL) {

		if (DEBUG) printf("MMS read: found named variable %s with search string %s\n",
				namedVariable->name, nameIdStr);

		if (namedVariable->type == MMS_STRUCTURE) {

		    MmsValue* value = mmsServer_getValue(connection->server, domain, nameIdStr, connection);

		    if (value != NULL) {
		        appendValueToResultList(value, values);
		    }
		    else {
		        //TODO signal DataAccessError to client
		        addComplexValueToResultList(namedVariable,
					values, connection, domain, nameIdStr);
		    }
		}
		else if (namedVariable->type == MMS_ARRAY) {

			if (alternateAccess != NULL) {
				alternateArrayAccess(connection, alternateAccess, domain,
						nameIdStr, values, namedVariable);
			}
			else { //getCompleteArray
			    MmsValue* value = mmsServer_getValue(connection->server, domain, nameIdStr, connection);
			    appendValueToResultList(value, values);
			}
		}
		else {
			MmsValue* value = mmsServer_getValue(connection->server, domain, nameIdStr, connection);

			if (value == NULL) {
			    if (DEBUG)
			        printf("MMS read: value of known variable is not found. Maybe illegal access to array element!\n");

			    appendErrorToResultList(values, 10 /* object-non-existant*/);
			}
			else
			    appendValueToResultList(value, values);
		}

	}
	else
		appendErrorToResultList(values, 10 /* object-non-existant*/);
}


static bool
isSpecWithResult(ReadRequest_t* read)
{
	if (read->specificationWithResult != NULL)
		if (*(read->specificationWithResult) != false)
			return true;

	return false;
}

static int
encodeVariableAccessSpecification(VarAccessSpec* accessSpec, uint8_t* buffer, int bufPos, bool encode)
{
	/* determine size */
	int varAccessSpecSize = 0;

	int itemIdLen = strlen(accessSpec->itemId);

	varAccessSpecSize += itemIdLen + BerEncoder_determineLengthSize(itemIdLen) + 1;

	if (accessSpec->domainId != NULL) {
		int domainIdLen = strlen(accessSpec->domainId);

		varAccessSpecSize += domainIdLen + BerEncoder_determineLengthSize(domainIdLen) + 1;
	}

	int specificityLength = varAccessSpecSize;

	varAccessSpecSize += 1 + BerEncoder_determineLengthSize(specificityLength);

	int variableListNameLength = varAccessSpecSize;

	varAccessSpecSize += 1 + BerEncoder_determineLengthSize(variableListNameLength);

	int varAccessSpecLength = varAccessSpecSize;

	varAccessSpecSize += 1 + BerEncoder_determineLengthSize(varAccessSpecLength);

	if (encode == false)
		return varAccessSpecSize;

	/* encode to buffer */
	bufPos = BerEncoder_encodeTL(0xa0, varAccessSpecLength, buffer, bufPos);

	if (accessSpec->isNamedVariableList == true) {

		bufPos = BerEncoder_encodeTL(0xa1, variableListNameLength, buffer, bufPos);

		if (accessSpec->specific == 0) { /* vmd-specific */
			bufPos = BerEncoder_encodeTL(0xa0, specificityLength, buffer, bufPos);
		}
		else if (accessSpec->specific == 1) { /* domain-specific */
			bufPos = BerEncoder_encodeTL(0xa1, specificityLength, buffer, bufPos);

		}
		else { /* association-specific */
			bufPos = BerEncoder_encodeTL(0xa2, specificityLength, buffer, bufPos);
		}


		if (accessSpec->domainId != NULL)
			bufPos = BerEncoder_encodeStringWithTag(0x1a, accessSpec->domainId, buffer, bufPos);

		bufPos = BerEncoder_encodeStringWithTag(0x1a, accessSpec->itemId, buffer, bufPos);
	}

	return bufPos;
}

static void
encodeReadResponse(MmsServerConnection* connection,
		uint32_t invokeId, ByteBuffer* response, LinkedList values,
		VarAccessSpec* accessSpec)
{
	int i;

	int variableCount = LinkedList_size(values);

	int varAccessSpecSize = 0;

	if (accessSpec != NULL) {
		varAccessSpecSize = encodeVariableAccessSpecification(accessSpec, NULL, 0, false);
	}

	/* determine BER encoded message sizes */
	int accessResultSize = 0;

	/* iterate values list to determine encoded size  */
	LinkedList value = LinkedList_getNext(values);

	for (i = 0; i < variableCount; i++) {

	   MmsValue* data = (MmsValue*) value->data;

	   accessResultSize += mmsServer_encodeAccessResult(data, NULL, 0, false);

		value = LinkedList_getNext(value);
	}

	int listOfAccessResultsLength = 1 +
									BerEncoder_determineLengthSize(accessResultSize) +
									accessResultSize;


	int confirmedServiceResponseContentLength = listOfAccessResultsLength  + varAccessSpecSize;

	int confirmedServiceResponseLength = 1 +
						BerEncoder_determineLengthSize(confirmedServiceResponseContentLength) +
						confirmedServiceResponseContentLength;

	int invokeIdSize = BerEncoder_UInt32determineEncodedSize(invokeId) + 2;

	int confirmedResponseContentSize = confirmedServiceResponseLength + invokeIdSize;

	int mmsPduSize = 1 + BerEncoder_determineLengthSize(confirmedResponseContentSize) +
			confirmedResponseContentSize;

	/* Check if message would fit in the MMS PDU */
	if (mmsPduSize > connection->maxPduSize) {
		if (DEBUG)
			printf("MMS read: message to large! send error PDU!\n");

		mmsServer_createConfirmedErrorPdu(invokeId, response,
					  MMS_ERROR_SERVICE_OTHER);
		return;
	}

	/* encode message */

	uint8_t* buffer = response->buffer;
	int bufPos = 0;

	/* confirmed response PDU */
	bufPos = BerEncoder_encodeTL(0xa1, confirmedResponseContentSize, buffer, bufPos);

	/* invoke id */
	bufPos = BerEncoder_encodeTL(0x02, invokeIdSize - 2, buffer, bufPos);
	bufPos = BerEncoder_encodeUInt32(invokeId, buffer, bufPos);

	/* confirmed-service-response read */
	bufPos = BerEncoder_encodeTL(0xa4, confirmedServiceResponseContentLength, buffer, bufPos);

	/* encode variable access specification */
	if (accessSpec != NULL)
		bufPos = encodeVariableAccessSpecification(accessSpec, buffer, bufPos, true);

	/* encode list of access results */
	bufPos = BerEncoder_encodeTL(0xa1, accessResultSize, buffer, bufPos);

	/* encode access results */
	value = LinkedList_getNext(values);

	for (i = 0; i < variableCount; i++) {
		MmsValue* data = (MmsValue*) value->data;

		bufPos = mmsServer_encodeAccessResult(data, buffer, bufPos, true);

		value = LinkedList_getNext(value);
	}

	response->size = bufPos;

	if (DEBUG)
		printf("MMS read: sent message for request with id %u (size = %i)\n", invokeId, bufPos);

}

static void
handleReadListOfVariablesRequest(
		MmsServerConnection* connection,
		ReadRequest_t* read,
		uint32_t invokeId,
		ByteBuffer* response)
{
	int variableCount = read->variableAccessSpecification.choice.listOfVariable.list.count;

	LinkedList /*<MmsValue>*/ values = LinkedList_create();

	if (isSpecWithResult(read)) { /* add specification to result */
		// ignore - not required for IEC 61850
	}

	MmsServer_lockModel(connection->server);

	int i;

	for (i = 0; i < variableCount; i++) {
		VariableSpecification_t varSpec =
			read->variableAccessSpecification.choice.listOfVariable.list.array[i]->variableSpecification;

		AlternateAccess_t* alternateAccess =
			read->variableAccessSpecification.choice.listOfVariable.list.array[i]->alternateAccess;

		if (varSpec.present == VariableSpecification_PR_name) {

			if (varSpec.choice.name.present == ObjectName_PR_domainspecific) {
				char* domainIdStr = mmsMsg_createStringFromAsnIdentifier(
						varSpec.choice.name.choice.domainspecific.domainId);


				char* nameIdStr = mmsMsg_createStringFromAsnIdentifier(
						varSpec.choice.name.choice.domainspecific.itemId);

				MmsDomain* domain = MmsDevice_getDomain(MmsServer_getDevice(connection->server), domainIdStr);

				if (domain == NULL) {
					if (DEBUG) printf("MMS read: domain %s not found!\n", domainIdStr);

					appendErrorToResultList(values, 10 /* object-non-existant*/);
					break;
				}

				MmsVariableSpecification* namedVariable = MmsDomain_getNamedVariable(domain, nameIdStr);

				addNamedVariableToResultList(namedVariable, domain, nameIdStr,
						values, connection, alternateAccess);

				free(domainIdStr);
				free(nameIdStr);
			}
			else {
                appendErrorToResultList(values, 10 /* object-non-existant*/);

				if (DEBUG) printf("MMS read: object name type not supported!\n");
			}
		}
		else {
			//TODO should we send a ConfirmedErrorPdu here?
			if (DEBUG) printf("MMS read: varspec type not supported!\n");
		}
	}

	encodeReadResponse(connection, invokeId, response, values, NULL);

	MmsServer_unlockModel(connection->server);

exit:
    deleteValueList(values);
}

#if MMS_DATA_SET_SERVICE == 1

static void
createNamedVariableListResponse(MmsServerConnection* connection, MmsNamedVariableList namedList,
		int invokeId, ByteBuffer* response, ReadRequest_t* read, VarAccessSpec* accessSpec)
{

	LinkedList /*<MmsValue>*/ values = LinkedList_create();
	LinkedList variables = MmsNamedVariableList_getVariableList(namedList);

	int variableCount = LinkedList_size(variables);

	MmsServer_lockModel(connection->server);

	int i;

	LinkedList variable = LinkedList_getNext(variables);

	for (i = 0; i < variableCount; i++) {

		MmsNamedVariableListEntry variableListEntry = (MmsNamedVariableListEntry) variable->data;

		MmsDomain* variableDomain = MmsNamedVariableListEntry_getDomain(variableListEntry);
		char* variableName = MmsNamedVariableListEntry_getVariableName(variableListEntry);

		MmsVariableSpecification* namedVariable = MmsDomain_getNamedVariable(variableDomain,
				variableName);

		addNamedVariableToResultList(namedVariable, variableDomain, variableName,
								values, connection, NULL);

		variable = LinkedList_getNext(variable);
	}

	if (isSpecWithResult(read)) /* add specification to result */
		encodeReadResponse(connection, invokeId, response, values, accessSpec);
	else
		encodeReadResponse(connection, invokeId, response, values, NULL);

	MmsServer_unlockModel(connection->server);

	deleteValueList(values);
}

static void
handleReadNamedVariableListRequest(
		MmsServerConnection* connection,
		ReadRequest_t* read,
		int invokeId,
		ByteBuffer* response)
{
	if (read->variableAccessSpecification.choice.variableListName.present ==
			ObjectName_PR_domainspecific)
	{
		char* domainId = mmsMsg_createStringFromAsnIdentifier(
				read->variableAccessSpecification.choice.variableListName.choice.domainspecific.domainId);

		char* listName = mmsMsg_createStringFromAsnIdentifier(
				read->variableAccessSpecification.choice.variableListName.choice.domainspecific.itemId);

		VarAccessSpec accessSpec;

		accessSpec.isNamedVariableList = true;
		accessSpec.specific = 1;
		accessSpec.domainId = domainId;
		accessSpec.itemId = listName;

		MmsDomain* domain = MmsDevice_getDomain(MmsServer_getDevice(connection->server), domainId);

		if (domain == NULL) {
			if (DEBUG) printf("MMS read: domain %s not found!\n", domainId);
			mmsServer_createConfirmedErrorPdu(invokeId, response, MMS_ERROR_ACCESS_OBJECT_NON_EXISTENT);
		}
		else {
			MmsNamedVariableList namedList = MmsDomain_getNamedVariableList(domain, listName);

			if (namedList != NULL) {
				createNamedVariableListResponse(connection, namedList, invokeId, response, read,
						&accessSpec);
			}
			else {
				if (DEBUG) printf("MMS read: named variable list %s not found!\n", listName);
				mmsServer_createConfirmedErrorPdu(invokeId, response, MMS_ERROR_ACCESS_OBJECT_NON_EXISTENT);
			}
		}

		free(domainId);
		free(listName);
	}
	else if (read->variableAccessSpecification.choice.variableListName.present ==
				ObjectName_PR_aaspecific)
	{
		char* listName = mmsMsg_createStringFromAsnIdentifier(read->variableAccessSpecification.
				choice.variableListName.choice.aaspecific);

		MmsNamedVariableList namedList = MmsServerConnection_getNamedVariableList(connection, listName);

		VarAccessSpec accessSpec;

		accessSpec.isNamedVariableList = true;
		accessSpec.specific = 2;
		accessSpec.domainId = NULL;
		accessSpec.itemId = listName;

		if (namedList == NULL)
			mmsServer_createConfirmedErrorPdu(invokeId, response, MMS_ERROR_ACCESS_OBJECT_NON_EXISTENT);
		else {
			createNamedVariableListResponse(connection, namedList, invokeId, response, read, &accessSpec);
		}

		free(listName);
	}
	else
		mmsServer_createConfirmedErrorPdu(invokeId, response, MMS_ERROR_ACCESS_OBJECT_ACCESS_UNSUPPORTED);
}

#endif /* MMS_DATA_SET_SERVICE == 1 */

void
mmsServer_handleReadRequest(
		MmsServerConnection* connection,
		uint8_t* buffer, int bufPos, int maxBufPos,
		uint32_t invokeId,
		ByteBuffer* response)
{
	ReadRequest_t* request = 0; /* allow asn1c to allocate structure */

	MmsPdu_t* mmsPdu = 0;

	asn_dec_rval_t rval; /* Decoder return value  */

	rval = ber_decode(NULL, &asn_DEF_MmsPdu, (void**) &mmsPdu, buffer, MMS_MAXIMUM_PDU_SIZE);

	request = &(mmsPdu->choice.confirmedRequestPdu.confirmedServiceRequest.choice.read);

	if (request->variableAccessSpecification.present == VariableAccessSpecification_PR_listOfVariable) {
		handleReadListOfVariablesRequest(connection, request, invokeId, response);
	}
#if MMS_DATA_SET_SERVICE == 1
	else if (request->variableAccessSpecification.present == VariableAccessSpecification_PR_variableListName) {
		handleReadNamedVariableListRequest(connection, request, invokeId, response);
	}
#endif
	else {
		mmsServer_createConfirmedErrorPdu(invokeId, response, MMS_ERROR_ACCESS_OBJECT_ACCESS_UNSUPPORTED);
	}

	asn_DEF_MmsPdu.free_struct(&asn_DEF_MmsPdu, mmsPdu, 0);
}

