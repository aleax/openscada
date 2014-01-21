/*
 *  ied_connection.c
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

#include "iec61850_client.h"

#include "stack_config.h"

#include "mms_client_connection.h"
#include "mms_mapping.h"

#include "thread.h"

#include "ied_connection_private.h"

struct sIedConnection
{
    MmsConnection connection;
    IedConnectionState state;
    LinkedList enabledReports;
    LinkedList logicalDevices;
    LinkedList clientControls;
    LastApplError lastApplError;
    Semaphore stateMutex;
};

typedef struct sICLogicalDevice
{
    char* name;
    LinkedList variables;
    LinkedList dataSets;
} ICLogicalDevice;

struct sClientDataSet
{
    char* dataSetReference; /* data set reference in MMS format */
    MmsValue* dataSetValues; /* MmsValue instance of type MMS_ARRAY */
};


struct sClientReport
{
    ClientDataSet dataSet;
    ReportCallbackFunction callback;
    void* callbackParameter;
    char* rcbReference;
    ReasonForInclusion* reasonForInclusion;
};

static IedClientError
mapMmsErrorToIedError(MmsError mmsError)
{
    switch (mmsError) {
    case MMS_ERROR_NONE:
        return IED_ERROR_OK;

    case MMS_ERROR_CONNECTION_LOST:
        return IED_ERROR_CONNECTION_LOST;

    case MMS_ERROR_ACCESS_OBJECT_ACCESS_DENIED:
        return IED_ERROR_ACCESS_DENIED;

    case MMS_ERROR_ACCESS_OBJECT_NON_EXISTENT:
        return IED_ERROR_OBJECT_DOES_NOT_EXIST;

    case MMS_ERROR_DEFINITION_OBJECT_EXISTS:
        return IED_ERROR_OBJECT_EXISTS;

    case MMS_ERROR_SERVICE_TIMEOUT:
        return IED_ERROR_TIMEOUT;

    default:
        return IED_ERROR_UNKNOWN;
    }
}

ICLogicalDevice*
ICLogicalDevice_create(char* name)
{
    ICLogicalDevice* self = (ICLogicalDevice*) calloc(1, sizeof(struct sICLogicalDevice));

    self->name = copyString(name);

    return self;
}

void
ICLogicalDevice_setVariableList(ICLogicalDevice* self, LinkedList variables)
{
    self->variables = variables;
}

void
ICLogicalDevice_setDataSetList(ICLogicalDevice* self, LinkedList dataSets)
{
    self->dataSets = dataSets;
}

void
ICLogicalDevice_destroy(ICLogicalDevice* self)
{
    free(self->name);

    if (self->variables != NULL)
        LinkedList_destroy(self->variables);

    if (self->dataSets != NULL)
        LinkedList_destroy(self->dataSets);

    free(self);
}

static ClientDataSet
ClientDataSet_create(char* dataSetReference)
{
    ClientDataSet self = (ClientDataSet) calloc(1, sizeof(struct sClientDataSet));

    self->dataSetReference = copyString(dataSetReference);
    StringUtils_replace(self->dataSetReference, '.', '$');

    self->dataSetValues = NULL;

    return self;
}

void
ClientDataSet_destroy(ClientDataSet self)
{
    if (self->dataSetValues != NULL)
        MmsValue_delete(self->dataSetValues);

    free(self->dataSetReference);

    free(self);
}

static void
ClientDataSet_setDataSetValues(ClientDataSet self, MmsValue* dataSetValues)
{
    self->dataSetValues = dataSetValues;
}

MmsValue*
ClientDataSet_getValues(ClientDataSet self)
{
    return self->dataSetValues;
}

char*
ClientDataSet_getReference(ClientDataSet self)
{
    return self->dataSetReference;
}

int
ClientDataSet_getDataSetSize(ClientDataSet self)
{
    if (self->dataSetValues != NULL) {
        return MmsValue_getArraySize(self->dataSetValues);
    }
    else
        return 0;
}

ClientReport
ClientReport_create(ClientDataSet dataSet)
{
    ClientReport self = (ClientReport) calloc(1, sizeof(struct sClientReport));

    self->dataSet = dataSet;

    int dataSetSize = ClientDataSet_getDataSetSize(dataSet);

    self->reasonForInclusion = (ReasonForInclusion*) calloc(dataSetSize, sizeof(ReasonForInclusion));

    return self;
}

void
ClientReport_destroy(ClientReport self)
{
    free(self->rcbReference);
    free(self->reasonForInclusion);
    free(self);
}

char*
ClientReport_getRcbReference(ClientReport self)
{
    return self->rcbReference;
}

ClientDataSet
ClientReport_getDataSet(ClientReport self)
{
    return self->dataSet;
}

ReasonForInclusion
ClientReport_getReasonForInclusion(ClientReport self, int elementIndex)
{
    return self->reasonForInclusion[elementIndex];
}

bool
doesControlObjectMatch(char* objRef, char* cntrlObj)
{
    int objRefLen = strlen(objRef);

    char* separator = strchr(cntrlObj, '$');

    if (separator == NULL)
        return false;

    int sepLen = separator - cntrlObj;

    if (sepLen >= objRefLen)
        return false;

    if (memcmp(objRef, cntrlObj, sepLen) != 0)
        return false;

    char* cntrlObjName = objRef + sepLen + 1;

    if (separator[1] != 'C')
        return false;
    if (separator[2] != 'O')
        return false;
    if (separator[3] != '$')
        return false;

    char* nextSeparator = strchr(separator + 4, '$');

    if (nextSeparator == NULL)
        return false;

    int cntrlObjNameLen = strlen(cntrlObjName);

    if (cntrlObjNameLen != nextSeparator - (separator + 4))
        return false;

    if (memcmp(cntrlObjName, separator + 4, cntrlObjNameLen) == 0)
        return true;

    return false;
}

static void
informationReportHandler(void* parameter, char* domainName,
        char* variableListName, MmsValue* value, bool isVariableListName)
{
    IedConnection self = (IedConnection) parameter;

    if (DEBUG_IED_CLIENT)
        printf("DEBUG_IED_CLIENT: received information report for %s\n", variableListName);

    if (domainName == NULL) {

        if (isVariableListName) {

            MmsValue* optFlds = MmsValue_getElement(value, 1);

            if (MmsValue_getBitStringBit(optFlds, 4) == false)
                goto cleanup_and_return;

            int datSetIndex = 2;

            if (MmsValue_getBitStringBit(optFlds, 1) == true)
                datSetIndex++;

            if (MmsValue_getBitStringBit(optFlds, 2) == true)
                datSetIndex++;

            MmsValue* datSet = MmsValue_getElement(value, datSetIndex);

            char* datSetName = MmsValue_toString(datSet);

            if (DEBUG_IED_CLIENT)
                printf("DEBUG_IED_CLIENT:  with datSet %s\n", datSetName);

            int inclusionIndex = datSetIndex + 1;

            LinkedList element = LinkedList_getNext(self->enabledReports);

            ClientReport report = NULL;

            while (element != NULL) {
                report = (ClientReport) element->data;

                ClientDataSet dataSet = report->dataSet;

                if (strcmp(datSetName, ClientDataSet_getReference(dataSet)) == 0) {
                    break;
                }

                element = LinkedList_getNext(element);
            }

            if (report == NULL)
                goto cleanup_and_return;

            if (DEBUG_IED_CLIENT)
                printf("DEBUG_IED_CLIENT: Found enabled report!\n");

            /* skip bufOvfl */
            if (MmsValue_getBitStringBit(optFlds, 6) == true)
                inclusionIndex++;

            /* skip entryId */
            if (MmsValue_getBitStringBit(optFlds, 7) == true)
                inclusionIndex++;

            /* skip confRev */
            if (MmsValue_getBitStringBit(optFlds, 8) == true)
                inclusionIndex++;

            /* skip segmentation fields */
            if (MmsValue_getBitStringBit(optFlds, 9) == true)
                inclusionIndex += 2;

            MmsValue* inclusion = MmsValue_getElement(value, inclusionIndex);

            int includedElements = MmsValue_getNumberOfSetBits(inclusion);

            if (DEBUG_IED_CLIENT)
                printf("DEBUG_IED_CLIENT: Report includes %i data set elements\n", includedElements);

            int valueIndex = inclusionIndex + 1;

            /* skip data-reference fields */
            if (MmsValue_getBitStringBit(optFlds, 5) == true)
                valueIndex += includedElements;

            int i;

            ClientDataSet dataSet = report->dataSet;
            MmsValue* dataSetValues = ClientDataSet_getValues(dataSet);

            bool hasReasonForInclusion = MmsValue_getBitStringBit(optFlds, 3);

            int reasonForInclusionIndex = valueIndex + includedElements;

            for (i = 0; i < ClientDataSet_getDataSetSize(dataSet); i++) {
                if (MmsValue_getBitStringBit(inclusion, i) == true) {

                    MmsValue* dataSetElement = MmsValue_getElement(dataSetValues, i);

                    MmsValue* newElementValue = MmsValue_getElement(value, valueIndex);

                    if (DEBUG_IED_CLIENT)
                        printf("DEBUG_IED_CLIENT:  update element value type: %i\n", MmsValue_getType(newElementValue));

                    MmsValue_update(dataSetElement, newElementValue);

                    valueIndex++;

                    if (hasReasonForInclusion) {
                        MmsValue* reasonForInclusion = MmsValue_getElement(value, reasonForInclusionIndex);


                        if (MmsValue_getBitStringBit(reasonForInclusion, 1) == true)
                            report->reasonForInclusion[i] = REASON_DATA_CHANGE;
                        else if (MmsValue_getBitStringBit(reasonForInclusion, 2) == true)
                            report->reasonForInclusion[i] = REASON_QUALITY_CHANGE;
                        else if (MmsValue_getBitStringBit(reasonForInclusion, 3) == true)
                            report->reasonForInclusion[i] = REASON_DATA_UPDATE;
                        else if (MmsValue_getBitStringBit(reasonForInclusion, 4) == true)
                            report->reasonForInclusion[i] = REASON_INTEGRITY;
                        else if (MmsValue_getBitStringBit(reasonForInclusion, 5) == true)
                            report->reasonForInclusion[i] = REASON_GI;
                    }
                    else {
                        report->reasonForInclusion[i] = REASON_UNKNOWN;
                    }
                }
                else {
                    report->reasonForInclusion[i] = REASON_NOT_INCLUDED;
                }
            }

            if (report->callback != NULL) {
                report->callback(report->callbackParameter, report);
            }
        }
        else {
            if (strcmp(variableListName, "LastApplError") == 0) {
                if (DEBUG_IED_CLIENT)
                    printf("DEBUG_IED_CLIENT: received LastApplError\n");

                MmsValue* lastAppIError = value;

                MmsValue* cntrlObj = MmsValue_getElement(lastAppIError, 0);

                MmsValue* error = MmsValue_getElement(lastAppIError, 1);
                MmsValue* origin = MmsValue_getElement(lastAppIError, 2);
                MmsValue* ctlNum = MmsValue_getElement(lastAppIError, 3);
                MmsValue* addCause = MmsValue_getElement(lastAppIError, 4);

                if (DEBUG_IED_CLIENT)
                    printf("DEBUG_IED_CLIENT:  CntrlObj: %s\n", MmsValue_toString(cntrlObj));
                if (DEBUG_IED_CLIENT)
                    printf("DEBUG_IED_CLIENT:  ctlNum: %u\n", MmsValue_toUint32(ctlNum));
                if (DEBUG_IED_CLIENT)
                    printf("DEBUG_IED_CLIENT:  addCause: %i\n", MmsValue_toInt32(addCause));
                if (DEBUG_IED_CLIENT)
                    printf("DEBUG_IED_CLIENT:  error: %i\n", MmsValue_toInt32(error));

                self->lastApplError.ctlNum = MmsValue_toUint32(ctlNum);
                self->lastApplError.addCause = MmsValue_toInt32(addCause);
                self->lastApplError.error = MmsValue_toInt32(error);

                LinkedList control = LinkedList_getNext(self->clientControls);

                while (control != NULL) {
                    ControlObjectClient object = (ControlObjectClient) control->data;

                    char* objectRef = ControlObjectClient_getObjectReference(object);

                    if (doesControlObjectMatch(objectRef, MmsValue_toString(cntrlObj)))
                        ControlObjectClient_setLastApplError(object, self->lastApplError);

                    control = LinkedList_getNext(control);
                }
            }
        }
    }

    cleanup_and_return:

    MmsValue_delete(value);
}

IedConnection
IedConnection_create()
{
    IedConnection self = (IedConnection) calloc(1, sizeof(struct sIedConnection));

    self->enabledReports = LinkedList_create();
    self->logicalDevices = NULL;
    self->clientControls = LinkedList_create();

    self->connection = MmsConnection_create();

    self->state = IED_STATE_IDLE;

    self->stateMutex = Semaphore_create(1);

    return self;
}

IedConnectionState
IedConnection_getState(IedConnection self)
{
    IedConnectionState state;

    Semaphore_wait(self->stateMutex);
    state = self->state;
    Semaphore_post(self->stateMutex);

    return state;
}

static void
IedConnection_setState(IedConnection self, IedConnectionState newState)
{
    Semaphore_wait(self->stateMutex);
    self->state = newState;
    Semaphore_post(self->stateMutex);
}

static void
connectionLostHandler(MmsConnection connection, void* parameter)
{
    IedConnection self = (IedConnection) parameter;

    IedConnection_setState(self, IED_STATE_CLOSED);

    if (DEBUG) printf("IedConnection closed!\n");
}

void
IedConnection_connect(IedConnection self, IedClientError* error, char* hostname, int tcpPort)
{
    MmsError mmsError;

    if (IedConnection_getState(self) != IED_STATE_CONNECTED) {

        MmsConnection_setConnectionLostHandler(self->connection, connectionLostHandler, (void*) self);
        MmsConnection_setInformationReportHandler(self->connection, informationReportHandler, self);

        if (MmsConnection_connect(self->connection, &mmsError, hostname, tcpPort)) {
            *error = IED_ERROR_OK;
            IedConnection_setState(self, IED_STATE_CONNECTED);
        }
        else {
            *error = mapMmsErrorToIedError(mmsError);
            MmsConnection_destroy(self->connection);
            self->connection = NULL;
        }
    }
    else
        *error = IED_ERROR_ALREADY_CONNECTED;
}

void
IedConnection_close(IedConnection self)
{
    if (IedConnection_getState(self) == IED_STATE_CONNECTED) {
        IedConnection_setState(self, IED_STATE_CLOSED);
        MmsConnection_destroy(self->connection);
        self->connection = NULL;
    }
}

void
IedConnection_destroy(IedConnection self)
{
    IedConnection_close(self);

    if (self->logicalDevices != NULL)
        LinkedList_destroyDeep(self->logicalDevices, (LinkedListValueDeleteFunction) ICLogicalDevice_destroy);

    if (self->enabledReports != NULL)
        LinkedList_destroyDeep(self->enabledReports, (LinkedListValueDeleteFunction) ClientReport_destroy);

    LinkedList_destroyStatic(self->clientControls);

    Semaphore_destroy(self->stateMutex);

    free(self);
}

MmsVariableSpecification*
IedConnection_getVariableSpecification(IedConnection self, IedClientError* error, char* objectReference,
        FunctionalConstraint fc)
{
    char* domainId;
    char* itemId;

    MmsError mmsError;
    MmsVariableSpecification* varSpec = NULL;

    domainId = MmsMapping_getMmsDomainFromObjectReference(objectReference, NULL);
    itemId = MmsMapping_createMmsVariableNameFromObjectReference(objectReference, fc, NULL);

    if ((domainId == NULL) || (itemId == NULL)) {
        *error = IED_ERROR_OBJECT_REFERENCE_INVALID;
        goto cleanup_and_exit;
    }

    varSpec =
            MmsConnection_getVariableAccessAttributes(self->connection, &mmsError, domainId, itemId);

    if (varSpec != NULL) {
        *error = IED_ERROR_OK;
    }
    else
        *error = mapMmsErrorToIedError(mmsError);

    cleanup_and_exit:
    if (domainId != NULL)
        free(domainId);
    if (itemId != NULL)
        free(itemId);

    return varSpec;
}

MmsValue*
IedConnection_readObject(IedConnection self, IedClientError* error, char* objectReference,
        FunctionalConstraint fc)
{
    char* domainId;
    char* itemId;
    MmsValue* value = NULL;

    //TODO avoid dynamic memory allocation with alloca!

    domainId = MmsMapping_getMmsDomainFromObjectReference(objectReference, NULL);
    itemId = MmsMapping_createMmsVariableNameFromObjectReference(objectReference, fc, NULL);

    if ((domainId == NULL) || (itemId == NULL)) {
        *error = IED_ERROR_OBJECT_REFERENCE_INVALID;
        goto cleanup_and_exit;
    }

    MmsError mmsError;

    value = MmsConnection_readVariable(self->connection, &mmsError, domainId, itemId);

    if (value != NULL) {
        *error = IED_ERROR_OK;
    }
    else
        *error = mapMmsErrorToIedError(mmsError);

    cleanup_and_exit:
    if (domainId != NULL)
        free(domainId);
    if (itemId != NULL)
        free(itemId);

    return value;
}

void
IedConnection_writeObject(IedConnection self, IedClientError* error, char* objectReference,
        FunctionalConstraint fc, MmsValue* value)
{
    char* domainId;
    char* itemId;

    domainId = MmsMapping_getMmsDomainFromObjectReference(objectReference, NULL);
    itemId = MmsMapping_createMmsVariableNameFromObjectReference(objectReference, fc, NULL);

    if ((domainId == NULL || itemId == NULL)) {
        *error = IED_ERROR_OBJECT_REFERENCE_INVALID;
        return;
    }

    MmsError mmsError;

    MmsIndication indication =
            MmsConnection_writeVariable(self->connection, &mmsError, domainId, itemId, value);

    if (indication == MMS_OK) {
        *error = IED_ERROR_OK;
    }
    else {
        *error = mapMmsErrorToIedError(mmsError);
    }

    free(domainId);
    free(itemId);
}

void
IedConnection_getDeviceModelFromServer(IedConnection self, IedClientError* error)
{
    MmsError mmsError = MMS_ERROR_NONE;
    *error = IED_ERROR_OK;

    LinkedList logicalDeviceNames = MmsConnection_getDomainNames(self->connection, &mmsError);

    if (logicalDeviceNames != NULL) {

        if (self->logicalDevices != NULL) {
            LinkedList_destroyDeep(self->logicalDevices, (LinkedListValueDeleteFunction) ICLogicalDevice_destroy);
            self->logicalDevices = NULL;
        }

        LinkedList logicalDevice = LinkedList_getNext(logicalDeviceNames);

        LinkedList logicalDevices = LinkedList_create();

        while (logicalDevice != NULL) {
            char* name = (char*) logicalDevice->data;

            ICLogicalDevice* icLogicalDevice = ICLogicalDevice_create(name);

            LinkedList variables = MmsConnection_getDomainVariableNames(self->connection,
                    &mmsError, name);

            if (variables != NULL)
                ICLogicalDevice_setVariableList(icLogicalDevice, variables);
            else {
                *error = mapMmsErrorToIedError(mmsError);
                break;
            }

            LinkedList dataSets = MmsConnection_getDomainVariableListNames(self->connection,
                    &mmsError, name);

            if (dataSets != NULL)
                ICLogicalDevice_setDataSetList(icLogicalDevice, dataSets);
            else {
                *error = mapMmsErrorToIedError(mmsError);
                break;
            }

            LinkedList_add(logicalDevices, icLogicalDevice);

            logicalDevice = LinkedList_getNext(logicalDevice);
        }

        self->logicalDevices = logicalDevices;

        LinkedList_destroy(logicalDeviceNames);
    }
    else {
        *error = mapMmsErrorToIedError(mmsError);
    }
}

LinkedList /*<char*>*/
IedConnection_getLogicalDeviceList(IedConnection self, IedClientError* error)
{
    *error = IED_ERROR_OK;

    if (self->logicalDevices == NULL) {
        IedConnection_getDeviceModelFromServer(self, error);

        if (*error != IED_ERROR_OK)
            return NULL;
    }

    if (self->logicalDevices != NULL) {
        LinkedList logicalDevice = LinkedList_getNext(self->logicalDevices);

        LinkedList logicalDeviceList = LinkedList_create();

        while (logicalDevice != NULL) {
            ICLogicalDevice* icLogicalDevice = (ICLogicalDevice*) logicalDevice->data;

            char* logicalDeviceName = copyString(icLogicalDevice->name);

            LinkedList_add(logicalDeviceList, logicalDeviceName);

            logicalDevice = LinkedList_getNext(logicalDevice);
        }

        *error = IED_ERROR_OK;
        return logicalDeviceList;
    }
    else {
        *error = IED_ERROR_UNKNOWN;
        return NULL;
    }
}

LinkedList /*<char*>*/
IedConnection_getLogicalDeviceDirectory(IedConnection self, IedClientError* error,
        char* logicalDeviceName)
{
    if (self->logicalDevices == NULL)
        IedConnection_getDeviceModelFromServer(self, error);

    if (self->logicalDevices == NULL)
        return NULL;

    LinkedList logicalDevice = LinkedList_getNext(self->logicalDevices);

    while (logicalDevice != NULL) {
        ICLogicalDevice* device = (ICLogicalDevice*) logicalDevice->data;

        if (strcmp(device->name, logicalDeviceName) == 0) {
            LinkedList logicalNodeNames = LinkedList_create();

            LinkedList variable = LinkedList_getNext(device->variables);

            while (variable != NULL) {
                char* variableName = (char*) variable->data;

                if (strchr(variableName, '$') == NULL)
                    LinkedList_add(logicalNodeNames, copyString((char*) variable->data));

                variable = LinkedList_getNext(variable);
            }

            return logicalNodeNames;
        }

        logicalDevice = LinkedList_getNext(logicalDevice);
    }

    *error = IED_ERROR_OBJECT_REFERENCE_INVALID;

    return NULL;
}

static bool
addToStringSet(LinkedList set, char* string)
{
    LinkedList element = set;

    while (LinkedList_getNext(element) != NULL) {
        if (strcmp((char*) LinkedList_getNext(element)->data, string) == 0)
            return false;

        element = LinkedList_getNext(element);
    }

    LinkedList_insertAfter(element, string);
    return true;
}

static void
addVariablesWithFc(char* fc, char* lnName, LinkedList variables, LinkedList lnDirectory)
{
    LinkedList variable = LinkedList_getNext(variables);

    while (variable != NULL) {
        char* variableName = (char*) variable->data;

        char* fcPos = strchr(variableName, '$');

        if (fcPos != NULL) {
            if (memcmp(fcPos + 1, fc, 2) != 0)
                goto next_element;

            int lnNameLen = fcPos - variableName;

            if (strncmp(variableName, lnName, lnNameLen) == 0) {
                char* fcEndPos = strchr(fcPos + 1, '$');

                if (fcEndPos != NULL) {
                    char* nameEndPos = strchr(fcEndPos + 1, '$');

                    if (nameEndPos == NULL)
                        addToStringSet(lnDirectory, copyString(fcEndPos + 1));
                }
            }
        }

        next_element:

        variable = LinkedList_getNext(variable);
    }
}

LinkedList /*<char*>*/
IedConnection_getLogicalNodeDirectory(IedConnection self, IedClientError* error,
        char* logicalNodeReference, ACSIClass acsiClass)
{
    if (self->logicalDevices == NULL)
        IedConnection_getDeviceModelFromServer(self, error);

    int lnRefLen = strlen(logicalNodeReference);

    char* lnRefCopy = (char*) alloca(lnRefLen + 1);

    strcpy(lnRefCopy, logicalNodeReference);

    char* ldSep = strchr(lnRefCopy, '/');

    if (ldSep == NULL) {
        *error = IED_ERROR_USER_PROVIDED_INVALID_ARGUMENT;
        return NULL;
    }

    *ldSep = 0;

    char* logicalDeviceName = lnRefCopy;

    char* logicalNodeName = ldSep + 1;

    // search for logical device

    LinkedList device = LinkedList_getNext(self->logicalDevices);

    bool deviceFound = false;

    ICLogicalDevice* ld;

    while (device != NULL) {
        ld = (ICLogicalDevice*) device->data;

        if (strcmp(logicalDeviceName, ld->name) == 0) {
            deviceFound = true;
            break;
        }

        device = LinkedList_getNext(device);
    }

    if (!deviceFound) {
        *error = IED_ERROR_OBJECT_REFERENCE_INVALID;
        return NULL;
    }

    LinkedList lnDirectory = LinkedList_create();

    switch (acsiClass) {

    case ACSI_CLASS_DATA_OBJECT:
        {
            LinkedList variable = LinkedList_getNext(ld->variables);

            while (variable != NULL) {
                char* variableName = (char*) variable->data;

                char* fcPos = strchr(variableName, '$');

                if (fcPos != NULL) {
                    if (memcmp(fcPos + 1, "RP", 2) == 0)
                        goto next_element;

                    if (memcmp(fcPos + 1, "BR", 2) == 0)
                        goto next_element;

                    if (memcmp(fcPos + 1, "GO", 2) == 0)
                        goto next_element;

                    int lnNameLen = fcPos - variableName;

                    if (strncmp(variableName, logicalNodeName, lnNameLen) == 0) {
                        char* fcEndPos = strchr(fcPos + 1, '$');

                        if (fcEndPos != NULL) {
                            char* nameEndPos = strchr(fcEndPos + 1, '$');

                            if (nameEndPos == NULL) {
                                char* dataObjectName = copyString(fcEndPos + 1);

                                if (!addToStringSet(lnDirectory, dataObjectName))
                                    free(dataObjectName);
                            }
                        }
                    }
                }

                next_element:

                variable = LinkedList_getNext(variable);
            }
        }
        break;

    case ACSI_CLASS_BRCB:
        addVariablesWithFc("BR", logicalNodeName, ld->variables, lnDirectory);
        break;

    case ACSI_CLASS_URCB:
        addVariablesWithFc("RP", logicalNodeName, ld->variables, lnDirectory);
        break;

    case ACSI_CLASS_GoCB:
        addVariablesWithFc("GO", logicalNodeName, ld->variables, lnDirectory);
        break;

    case ACSI_CLASS_DATA_SET:
        {
            LinkedList dataSet = LinkedList_getNext(ld->dataSets);

            while (dataSet != NULL) {
                char* dataSetName = (char*) dataSet->data;

                char* fcPos = strchr(dataSetName, '$');

                if (fcPos == NULL)
                    goto next_data_set_element;

                int lnNameLen = fcPos - dataSetName;

                if (strlen(logicalNodeName) != lnNameLen)
                    goto next_data_set_element;

                if (memcmp(dataSetName, logicalNodeName, lnNameLen) != 0)
                    goto next_data_set_element;

                LinkedList_add(lnDirectory, copyString(fcPos + 1));

                next_data_set_element:

                dataSet = LinkedList_getNext(dataSet);
            }
        }
        break;
    }

    *error = IED_ERROR_OK;
    return lnDirectory;
}

LinkedList /*<char*>*/
IedConnection_getLogicalNodeVariables(IedConnection self, IedClientError* error,
        char* logicalNodeReference)
{
    if (self->logicalDevices == NULL)
        IedConnection_getDeviceModelFromServer(self, error);

    int lnRefLen = strlen(logicalNodeReference);

    char* lnRefCopy = (char*) alloca(lnRefLen + 1);

    strcpy(lnRefCopy, logicalNodeReference);

    char* ldSep = strchr(lnRefCopy, '/');

    *ldSep = 0;

    char* logicalDeviceName = lnRefCopy;

    char* logicalNodeName = ldSep + 1;

    // search for logical device

    LinkedList device = LinkedList_getNext(self->logicalDevices);

    bool deviceFound = false;

    ICLogicalDevice* ld;

    while (device != NULL) {
        ld = (ICLogicalDevice*) device->data;

        if (strcmp(logicalDeviceName, ld->name) == 0) {
            deviceFound = true;
            break;
        }

        device = LinkedList_getNext(device);
    }

    if (!deviceFound) {
        *error = IED_ERROR_OBJECT_REFERENCE_INVALID;
        return NULL;
    }

    if (DEBUG_IED_CLIENT)
        printf("DEBUG_IED_CLIENT: Found LD %s search for variables of LN %s ...\n", logicalDeviceName, logicalNodeName);

    LinkedList variable = LinkedList_getNext(ld->variables);

    LinkedList lnDirectory = LinkedList_create();

    while (variable != NULL) {
        char* variableName = (char*) variable->data;

        char* fcPos = strchr(variableName, '$');

        if (fcPos != NULL) {
            int lnNameLen = fcPos - variableName;

            if (strncmp(variableName, logicalNodeName, lnNameLen) == 0) {
                LinkedList_add(lnDirectory, copyString(fcPos + 1));
            }
        }

        variable = LinkedList_getNext(variable);
    }

    *error = IED_ERROR_OK;
    return lnDirectory;
}

static LinkedList
getDataDirectory(IedConnection self, IedClientError* error,
        char* dataReference, bool withFc)
{
    if (self->logicalDevices == NULL)
        IedConnection_getDeviceModelFromServer(self, error);

    int dataRefLen = strlen(dataReference);
    ;

    char* dataRefCopy = (char*) alloca(dataRefLen + 1);

    strcpy(dataRefCopy, dataReference);

    char* ldSep = strchr(dataRefCopy, '/');

    *ldSep = 0;

    char* logicalDeviceName = dataRefCopy;

    char* logicalNodeName = ldSep + 1;

    char* logicalNodeNameEnd = strchr(logicalNodeName, '.');

    if (logicalNodeNameEnd == NULL) {
        *error = IED_ERROR_OBJECT_REFERENCE_INVALID;
        return NULL;
    }

    int logicalNodeNameLen = logicalNodeNameEnd - logicalNodeName;

    char* dataNamePart = logicalNodeNameEnd + 1;

    int dataNamePartLen = strlen(dataNamePart);

    if (dataNamePartLen < 1) {
        *error = IED_ERROR_OBJECT_REFERENCE_INVALID;
        return NULL;
    }

    StringUtils_replace(dataNamePart, '.', '$');

    // search for logical device

    LinkedList device = LinkedList_getNext(self->logicalDevices);

    bool deviceFound = false;

    ICLogicalDevice* ld;

    while (device != NULL) {
        ld = (ICLogicalDevice*) device->data;

        if (strcmp(logicalDeviceName, ld->name) == 0) {
            deviceFound = true;
            break;
        }

        device = LinkedList_getNext(device);
    }

    if (!deviceFound) {
        *error = IED_ERROR_OBJECT_REFERENCE_INVALID;
        return NULL;
    }

    LinkedList variable = LinkedList_getNext(ld->variables);

    LinkedList dataDirectory = LinkedList_create();

    while (variable != NULL) {
        char* variableName = (char*) variable->data;

        char* fcPos = strchr(variableName, '$');

        if (fcPos != NULL) {
            int lnNameLen = fcPos - variableName;

            if (logicalNodeNameLen == lnNameLen) {

                if (memcmp(variableName, logicalNodeName, lnNameLen) == 0) {

                    /* ok we are in the correct logical node */

                    /* skip FC */
                    char* fcEnd = strchr(fcPos + 1, '$');

                    if (fcEnd == NULL)
                        goto next_variable;

                    char* remainingPart = fcEnd + 1;

                    int remainingLen = strlen(remainingPart);

                    if (remainingLen <= dataNamePartLen)
                        goto next_variable;

                    if (remainingPart[dataNamePartLen] == '$') {

                        if (memcmp(dataNamePart, remainingPart, dataNamePartLen) == 0) {

                            char* subElementName = remainingPart + dataNamePartLen + 1;

                            char* subElementNameSep = strchr(subElementName, '$');

                            if (subElementNameSep != NULL)
                                goto next_variable;

                            char* elementName;

                            if (withFc) {
                                int elementNameLen = strlen(subElementName);

                                elementName = (char*) malloc(elementNameLen + 5);
                                memcpy(elementName, subElementName, elementNameLen);
                                elementName[elementNameLen] = '[';
                                elementName[elementNameLen + 1] = *(fcPos + 1);
                                elementName[elementNameLen + 2] = *(fcPos + 2);
                                elementName[elementNameLen + 3] = ']';
                                elementName[elementNameLen + 4] = 0;
                            }
                            else
                                elementName = copyString(subElementName);

                            if (!addToStringSet(dataDirectory, elementName))
                                free(elementName);
                        }
                    }
                }
            }
        }

        next_variable:

        variable = LinkedList_getNext(variable);
    }

    *error = IED_ERROR_OK;
    return dataDirectory;

}

LinkedList
IedConnection_getDataDirectory(IedConnection self, IedClientError* error,
        char* dataReference)
{
    return getDataDirectory(self, error, dataReference, false);
}

LinkedList
IedConnection_getDataDirectoryFC(IedConnection self, IedClientError* error,
        char* dataReference)
{
    return getDataDirectory(self, error, dataReference, true);
}

void
IedConnection_createDataSet(IedConnection self, IedClientError* error, char* dataSetReference,
        LinkedList /* <char*> */ dataSetElements)
{

    char* domainId;
    char* itemId;

    domainId = (char*) alloca(129);

    domainId = MmsMapping_getMmsDomainFromObjectReference(dataSetReference, domainId);

    itemId = copyString(dataSetReference + strlen(domainId) + 1);

    StringUtils_replace(itemId, '.', '$');

    MmsError mmsError;

    LinkedList dataSetEntries = LinkedList_create();

    LinkedList dataSetElement = LinkedList_getNext(dataSetElements);

    while (dataSetElement != NULL) {

        MmsVariableAccessSpecification* dataSetEntry =
                MmsMapping_ObjectReferenceToVariableAccessSpec((char*) dataSetElement->data);

        LinkedList_add(dataSetEntries, (void*) dataSetEntry);

        dataSetElement = LinkedList_getNext(dataSetElement);
    }

    MmsConnection_defineNamedVariableList(self->connection, &mmsError,
            domainId, itemId, dataSetEntries);

    /* delete list and all elements */
    LinkedList_destroyDeep(dataSetEntries, (LinkedListValueDeleteFunction) MmsVariableAccessSpecification_destroy);

    free(itemId);

    *error = mapMmsErrorToIedError(mmsError);
}

void
IedConnection_deleteDataSet(IedConnection self, IedClientError* error, char* dataSetReference)
{
    char* domainId;
    char* itemId;

    domainId = (char*) alloca(129);

    domainId = MmsMapping_getMmsDomainFromObjectReference(dataSetReference, domainId);

    itemId = copyString(dataSetReference + strlen(domainId) + 1);

    StringUtils_replace(itemId, '.', '$');

    MmsError mmsError;

    MmsConnection_deleteNamedVariableList(self->connection, &mmsError, domainId, itemId);

    free(itemId);

    *error = mapMmsErrorToIedError(mmsError);
}

LinkedList /* <char*> */
IedConnection_getDataSetDirectory(IedConnection self, IedClientError* error, char* dataSetReference, bool* isDeletable)
{
    bool deletable = false;

    LinkedList dataSetMembers = NULL;

    char* domainId;
    char* itemId;

    domainId = (char*) alloca(129);

    domainId = MmsMapping_getMmsDomainFromObjectReference(dataSetReference, domainId);

    itemId = copyString(dataSetReference + strlen(domainId) + 1);

    StringUtils_replace(itemId, '.', '$');

    MmsError mmsError;

    LinkedList entries =
            MmsConnection_readNamedVariableListDirectory(self->connection, &mmsError, domainId, itemId,
                    &deletable);

    if (mmsError == MMS_ERROR_NONE) {

        LinkedList entry = LinkedList_getNext(entries);

        dataSetMembers = LinkedList_create();

        while (entry != NULL) {
            MmsVariableAccessSpecification* varAccessSpec = (MmsVariableAccessSpecification*) entry->data;

            char* objectReference = MmsMapping_varAccessSpecToObjectReference(varAccessSpec);

            LinkedList_add(dataSetMembers, objectReference);

            entry = LinkedList_getNext(entry);
        }

        if (isDeletable != NULL)
            *isDeletable = deletable;

        LinkedList_destroyDeep(entries, (LinkedListValueDeleteFunction) MmsVariableAccessSpecification_destroy);
    }

    free(itemId);

    *error = mapMmsErrorToIedError(mmsError);

    return dataSetMembers;
}

ClientDataSet
IedConnection_readDataSetValues(IedConnection self, IedClientError* error, char* dataSetReference,
        ClientDataSet dataSet)
{
    char* domainId;
    char* itemId;

    domainId = (char*) alloca(129);

    domainId = MmsMapping_getMmsDomainFromObjectReference(dataSetReference, domainId);

    itemId = copyString(dataSetReference + strlen(domainId) + 1);

    StringUtils_replace(itemId, '.', '$');

    MmsError mmsError;

    MmsValue* dataSetVal =
            MmsConnection_readNamedVariableListValues(self->connection, &mmsError,
                    domainId, itemId, true);

    free(itemId);

    if (dataSetVal == NULL) {
        *error = mapMmsErrorToIedError(mmsError);
        goto cleanup_and_exit;
    }
    else
        *error = IED_ERROR_OK;

    if (dataSet == NULL) {
        dataSet = ClientDataSet_create(dataSetReference);
        ClientDataSet_setDataSetValues(dataSet, dataSetVal);
    }
    else {
        MmsValue* dataSetValues = ClientDataSet_getValues(dataSet);
        MmsValue_update(dataSetValues, dataSetVal);
    }

    cleanup_and_exit:
    return dataSet;
}

static void
writeReportResv(IedConnection self, IedClientError* error, char* rcbReference, bool resvValue)
{
    char* domainId;
    char* itemId;

    MmsError mmsError;

    domainId = (char*) alloca(129);
    itemId = (char*) alloca(129);

    domainId = MmsMapping_getMmsDomainFromObjectReference(rcbReference, domainId);

    strcpy(itemId, rcbReference + strlen(domainId) + 1);

    StringUtils_replace(itemId, '.', '$');

    if (DEBUG_IED_CLIENT)
        printf("DEBUG_IED_CLIENT: reserve report for (%s) (%s)\n", domainId, itemId);

    int itemIdLen = strlen(itemId);

    strcpy(itemId + itemIdLen, "$Resv");

    MmsValue* resv = MmsValue_newBoolean(resvValue);

    MmsIndication indication =
            MmsConnection_writeVariable(self->connection, &mmsError, domainId,
                    itemId, resv);

    MmsValue_delete(resv);

    if (indication != MMS_OK) {
        if (DEBUG_IED_CLIENT)
            printf("DEBUG_IED_CLIENT:  failed to write to RCB!\n");
        *error = mapMmsErrorToIedError(mmsError);
    }
    else {
        *error = *error = IED_ERROR_OK;
    }
}

void
IedConnection_reserveRCB(IedConnection self, IedClientError* error, char* rcbReference)
{
    writeReportResv(self, error, rcbReference, true);
}

void
IedConnection_releaseRCB(IedConnection self, IedClientError* error, char* rcbReference)
{
    writeReportResv(self, error, rcbReference, false);
}

ClientReportControlBlock
IedConnection_getRCBValues(IedConnection self, IedClientError* error, char* rcbReference,
        ClientReportControlBlock updateRcb)
{
    MmsError mmsError = MMS_ERROR_NONE;

    ClientReportControlBlock returnRcb = updateRcb;

    char* domainId;
    char* itemId;

    domainId = (char*) alloca(129);
    itemId = (char*) alloca(129);

    domainId = MmsMapping_getMmsDomainFromObjectReference(rcbReference, domainId);

    strcpy(itemId, rcbReference + strlen(domainId) + 1);

    StringUtils_replace(itemId, '.', '$');

    if (DEBUG_IED_CLIENT)
        printf("DEBUG_IED_CLIENT: readRCBValues for %s\n", rcbReference);

    MmsValue* rcb = MmsConnection_readVariable(self->connection, &mmsError, domainId, itemId);

    if (mmsError != MMS_ERROR_NONE) {
        *error = mapMmsErrorToIedError(mmsError);

        return NULL;
    }

    if (rcb == NULL) {
        *error = IED_ERROR_OBJECT_DOES_NOT_EXIST;
        return NULL;
    }

    if (MmsValue_getType(rcb) != MMS_STRUCTURE) {
        if (DEBUG_IED_CLIENT)
                printf("DEBUG_IED_CLIENT: getRCBValues returned wrong type!\n");

        MmsValue_delete(rcb);

        *error = IED_ERROR_UNKNOWN;
        return NULL;
    }


    if (returnRcb == NULL)
        returnRcb = ClientReportControlBlock_create(rcbReference);

    private_ClientReportControlBlock_updateValues(returnRcb, rcb);

    MmsValue_delete(rcb);

    *error = IED_ERROR_OK;

    return returnRcb;
}

void
IedConnection_setRCBValues(IedConnection self, IedClientError* error, ClientReportControlBlock rcb,
        uint32_t parametersMask, bool singleRequest)
{
    *error = IED_ERROR_OK;

    MmsError mmsError = MMS_ERROR_NONE;

    bool isBuffered = ClientReportControlBlock_isBuffered(rcb);

    char* domainId;
    char* itemId;

    domainId = (char*) alloca(129);
    itemId = (char*) alloca(129);

    char* rcbReference = ClientReportControlBlock_getObjectReference(rcb);

    domainId = MmsMapping_getMmsDomainFromObjectReference(rcbReference, domainId);

    strcpy(itemId, rcbReference + strlen(domainId) + 1);

    StringUtils_replace(itemId, '.', '$');

    if (DEBUG_IED_CLIENT)
        printf("DEBUG_IED_CLIENT: setRCBValues for %s\n", rcbReference);

    int itemIdLen = strlen(itemId);

    /* create the list of requested itemIds references */
    LinkedList itemIds = LinkedList_create();
    LinkedList values = LinkedList_create();

    /* add resv/resvTms as first element and rptEna as last element */
    if (parametersMask & RCB_ELEMENT_RESV) {
        if (isBuffered) goto error_invalid_parameter;

        strcpy(itemId + itemIdLen, "$Resv");

        LinkedList_add(itemIds, copyString(itemId));
        LinkedList_add(values, rcb->resv);
    }

    if (parametersMask & RCB_ELEMENT_RESV_TMS) {
        if (!isBuffered) goto error_invalid_parameter;

        strcpy(itemId + itemIdLen, "$ResvTms");

        LinkedList_add(itemIds, copyString(itemId));
        LinkedList_add(values, rcb->resvTms);
    }

    if (parametersMask & RCB_ELEMENT_RPT_ID) {
        strcpy(itemId + itemIdLen, "$RptID");

        LinkedList_add(itemIds, copyString(itemId));
        LinkedList_add(values, rcb->rptId);
    }

    if (parametersMask & RCB_ELEMENT_DATSET) {
         strcpy(itemId + itemIdLen, "$DatSet");

         LinkedList_add(itemIds, copyString(itemId));
         LinkedList_add(values, rcb->datSet);
     }

    if (parametersMask & RCB_ELEMENT_OPT_FLDS) {
         strcpy(itemId + itemIdLen, "$OptFlds");

         LinkedList_add(itemIds, copyString(itemId));
         LinkedList_add(values, rcb->optFlds);
    }

    if (parametersMask & RCB_ELEMENT_BUF_TM) {
         strcpy(itemId + itemIdLen, "$BufTm");

         LinkedList_add(itemIds, copyString(itemId));
         LinkedList_add(values, rcb->bufTm);
    }

    if (parametersMask & RCB_ELEMENT_TRG_OPS) {
         strcpy(itemId + itemIdLen, "$TrgOps");

         LinkedList_add(itemIds, copyString(itemId));
         LinkedList_add(values, rcb->trgOps);
    }

    if (parametersMask & RCB_ELEMENT_INTG_PD) {
         strcpy(itemId + itemIdLen, "$IntgPd");

         LinkedList_add(itemIds, copyString(itemId));
         LinkedList_add(values, rcb->intgPd);
    }

    if (parametersMask & RCB_ELEMENT_GI) {
         strcpy(itemId + itemIdLen, "$GI");

         LinkedList_add(itemIds, copyString(itemId));
         LinkedList_add(values, rcb->gi);
    }

    if (parametersMask & RCB_ELEMENT_PURGE_BUF) {
        if (!isBuffered) goto error_invalid_parameter;


        strcpy(itemId + itemIdLen, "$PurgeBuf");

        LinkedList_add(itemIds, copyString(itemId));
        LinkedList_add(values, rcb->purgeBuf);
    }

    if (parametersMask & RCB_ELEMENT_TIME_OF_ENTRY) {
        if (!isBuffered) goto error_invalid_parameter;

        strcpy(itemId + itemIdLen, "$TimeOfEntry");

        LinkedList_add(itemIds, copyString(itemId));
        LinkedList_add(values, rcb->timeOfEntry);
    }

    if (parametersMask & RCB_ELEMENT_RPT_ENA) {
         strcpy(itemId + itemIdLen, "$RptEna");

         LinkedList_add(itemIds, copyString(itemId));
         LinkedList_add(values, rcb->rptEna);
    }

    if (singleRequest) {
        LinkedList accessResults = NULL;

        MmsConnection_writeMultipleVariables(self->connection, &mmsError, domainId, itemIds, values, &accessResults);

        if (accessResults != NULL)
            LinkedList_destroyDeep(accessResults, (LinkedListValueDeleteFunction) MmsValue_delete);

        *error = mapMmsErrorToIedError(mmsError);
        goto exit_function;
    }
    else {
        LinkedList itemIdElement = LinkedList_getNext(itemIds);
        LinkedList valueElement = LinkedList_getNext(values);

        while (itemIdElement != NULL) {
            char* rcbItemId = (char*) itemIdElement->data;
            MmsValue* value = (MmsValue*) valueElement->data;

            MmsConnection_writeVariable(self->connection, &mmsError, domainId, rcbItemId, value);

            if (mmsError != MMS_ERROR_NONE)
                break;

            itemIdElement = LinkedList_getNext(itemIdElement);
            valueElement = LinkedList_getNext(valueElement);
        }

        *error = mapMmsErrorToIedError(mmsError);
        goto exit_function;
    }

error_invalid_parameter:
    *error = IED_ERROR_USER_PROVIDED_INVALID_ARGUMENT;

exit_function:
    LinkedList_destroy(itemIds);
    LinkedList_destroyStatic(values);
}

void
IedConnection_enableReporting(IedConnection self, IedClientError* error,
        char* rcbReference,
        ClientDataSet dataSet,
        int triggerOptions,
        ReportCallbackFunction callback,
        void* callbackParameter)
{
    MmsError mmsError = MMS_ERROR_NONE;

    char* domainId;
    char* itemId;

    domainId = (char*) alloca(129);
    itemId = (char*) alloca(129);

    domainId = MmsMapping_getMmsDomainFromObjectReference(rcbReference, domainId);

    strcpy(itemId, rcbReference + strlen(domainId) + 1);

    StringUtils_replace(itemId, '.', '$');

    if (DEBUG_IED_CLIENT)
        printf("DEBUG_IED_CLIENT: enable report for (%s) (%s)\n", domainId, itemId);

    int itemIdLen = strlen(itemId);

    // check if data set is matching
    strcpy(itemId + itemIdLen, "$DatSet");
    MmsValue* datSet = MmsConnection_readVariable(self->connection, &mmsError, domainId, itemId);

    if (datSet != NULL) {

        if (DEBUG_IED_CLIENT)
            printf("DEBUG_IED_CLIENT: RCB has dataset: %s\n", MmsValue_toString(datSet));

        bool matching = false;

        if (strcmp(MmsValue_toString(datSet), ClientDataSet_getReference(dataSet)) == 0) {
            if (DEBUG_IED_CLIENT)
                printf("DEBUG_IED_CLIENT:   data sets are matching!\n");
            matching = true;
        }
        else {
            if (DEBUG_IED_CLIENT)
                printf("DEBUG_IED_CLIENT:  data sets (%s) - (%s) not matching!", MmsValue_toString(datSet),
                        ClientDataSet_getReference(dataSet));
        }

        MmsValue_delete(datSet);

        if (!matching) {
            *error = IED_ERROR_ENABLE_REPORT_FAILED_DATASET_MISMATCH;
            goto cleanup_and_exit;
        }

    }
    else {
        if (DEBUG_IED_CLIENT)
            printf("DEBUG_IED_CLIENT: Error accessing RCB!\n");
        *error = IED_ERROR_ACCESS_DENIED;
        return;
    }

    // set include data set reference

    strcpy(itemId + itemIdLen, "$OptFlds");
    MmsValue* optFlds = MmsConnection_readVariable(self->connection, &mmsError,
            domainId, itemId);

    if (MmsValue_getBitStringBit(optFlds, 4) == true) {
        if (DEBUG_IED_CLIENT)
            printf("DEBUG_IED_CLIENT:  data set reference is included in report.\n");
        MmsValue_delete(optFlds);
    }
    else {
        MmsValue_setBitStringBit(optFlds, 4, true);

        MmsIndication indication =
                MmsConnection_writeVariable(self->connection, &mmsError, domainId, itemId, optFlds);

        MmsValue_delete(optFlds);

        if (indication != MMS_OK) {
            if (DEBUG_IED_CLIENT)
                printf("DEBUG_IED_CLIENT:  failed to write to RCB!\n");
            *error = mapMmsErrorToIedError(mmsError);
            goto cleanup_and_exit;
        }
    }

    // set trigger options
    if (triggerOptions != 0) {
        strcpy(itemId + itemIdLen, "$TrgOps");

        MmsValue* trgOps = MmsConnection_readVariable(self->connection, &mmsError,
                domainId, itemId);

        if (trgOps != NULL) {
            MmsValue_deleteAllBitStringBits(trgOps);

            if (triggerOptions & TRG_OPT_DATA_CHANGED)
                MmsValue_setBitStringBit(trgOps, 1, true);
            if (triggerOptions & TRG_OPT_QUALITY_CHANGED)
                MmsValue_setBitStringBit(trgOps, 2, true);
            if (triggerOptions & TRG_OPT_DATA_UPDATE)
                MmsValue_setBitStringBit(trgOps, 3, true);
            if (triggerOptions & TRG_OPT_INTEGRITY)
                MmsValue_setBitStringBit(trgOps, 4, true);
            if (triggerOptions & TRG_OPT_GI)
                MmsValue_setBitStringBit(trgOps, 5, true);

            MmsIndication indication =
                    MmsConnection_writeVariable(self->connection, &mmsError, domainId,
                            itemId, trgOps);

            MmsValue_delete(trgOps);
        }
        else {
            if (DEBUG_IED_CLIENT)
                printf("DEBUG_IED_CLIENT:  failed to read trigger options!\n");
            *error = mapMmsErrorToIedError(mmsError);
            MmsValue_delete(trgOps);
            goto cleanup_and_exit;
        }
    }

    // enable report
    strcpy(itemId + itemIdLen, "$RptEna");
    MmsValue* rptEna = MmsValue_newBoolean(true);
    MmsIndication indication =
            MmsConnection_writeVariable(self->connection, &mmsError, domainId, itemId, rptEna);

    MmsValue_delete(rptEna);

    if (indication == MMS_OK) {
        IedConnection_installReportHandler(self, rcbReference, callback, callbackParameter, dataSet);
    }
    else {
        if (DEBUG_IED_CLIENT)
            printf("DEBUG_IED_CLIENT:Failed to enable report!\n");
        *error = mapMmsErrorToIedError(mmsError);
    }

    cleanup_and_exit:
    return;
}

static ClientReport
lookupReportHandler(IedConnection self, char* rcbReference)
{
    LinkedList element = LinkedList_getNext(self->enabledReports);

    while (element != NULL) {
        ClientReport report = (ClientReport) element->data;

        if (strcmp(report->rcbReference, rcbReference) == 0)
            return report;

        element = LinkedList_getNext(element);
    }

    return NULL;
}

void
IedConnection_installReportHandler(IedConnection self, char* rcbReference, ReportCallbackFunction handler,
        void* handlerParameter, ClientDataSet dataSet)
{
    ClientReport report = lookupReportHandler(self, rcbReference);

    if (report != NULL) {
        IedConnection_uninstallReportHandler(self, rcbReference);

        if (DEBUG_IED_CLIENT)
           printf("DEBUG_IED_CLIENT: Removed existing report callback handler for %s\n", rcbReference);
    }

    report = ClientReport_create(dataSet);
    report->callback = handler;
    report->callbackParameter = handlerParameter;
    report->rcbReference = copyString(rcbReference);
    LinkedList_add(self->enabledReports, report);

    if (DEBUG_IED_CLIENT)
       printf("DEBUG_IED_CLIENT: Installed new report callback handler for %s\n", rcbReference);
}

void
IedConnection_uninstallReportHandler(IedConnection self, char* rcbReference)
{
    ClientReport report = lookupReportHandler(self, rcbReference);

    if (report != NULL) {
        LinkedList_remove(self->enabledReports, report);
        ClientReport_destroy(report);
    }
}

void
IedConnection_disableReporting(IedConnection self, IedClientError* error, char* rcbReference)
{
    char* domainId;
    char* itemId;

    domainId = (char*) alloca(129);
    itemId = (char*) alloca(129);

    domainId = MmsMapping_getMmsDomainFromObjectReference(rcbReference, domainId);

    strcpy(itemId, rcbReference + strlen(domainId) + 1);

    StringUtils_replace(itemId, '.', '$');

    if (DEBUG_IED_CLIENT)
        printf("DEBUG_IED_CLIENT: disable reporting for (%s) (%s)\n", domainId, itemId);

    int itemIdLen = strlen(itemId);

    strcpy(itemId + itemIdLen, "$RptEna");
    MmsValue* rptEna = MmsValue_newBoolean(false);

    MmsError errorCode;

    MmsIndication indication =
            MmsConnection_writeVariable(self->connection, &errorCode, domainId, itemId, rptEna);

    MmsValue_delete(rptEna);

    if (indication != MMS_OK) {
        if (DEBUG_IED_CLIENT)
            printf("DEBUG_IED_CLIENT:  failed to disable RCB!\n");

        *error = mapMmsErrorToIedError(errorCode);
    }
    else {
        IedConnection_uninstallReportHandler(self, rcbReference);

        *error = IED_ERROR_OK;
    }
}

void
IedConnection_triggerGIReport(IedConnection self, IedClientError* error, char* rcbReference)
{
    char* domainId;
    char* itemId;

    domainId = (char*) alloca(129);
    itemId = (char*) alloca(129);

    domainId = MmsMapping_getMmsDomainFromObjectReference(rcbReference,
            domainId);

    strcpy(itemId, rcbReference + strlen(domainId) + 1);

    StringUtils_replace(itemId, '.', '$');

    int itemIdLen = strlen(itemId);

    strcpy(itemId + itemIdLen, "$GI");

    MmsConnection mmsCon = IedConnection_getMmsConnection(self);

    MmsError mmsError;

    MmsValue* gi = MmsValue_newBoolean(true);

    MmsIndication indication =
            MmsConnection_writeVariable(mmsCon, &mmsError, domainId, itemId, gi);

    MmsValue_delete(gi);

    if (indication != MMS_OK) {
        if (DEBUG_IED_CLIENT)
            printf("DEBUG_IED_CLIENT: failed to trigger GI for %s!\n", rcbReference);

        *error = mapMmsErrorToIedError(mmsError);
    }
    else {
        *error = IED_ERROR_OK;
    }
}

MmsConnection
IedConnection_getMmsConnection(IedConnection self)
{
    return self->connection;
}

LastApplError
IedConnection_getLastApplError(IedConnection self)
{
    return self->lastApplError;
}

void
private_IedConnection_addControlClient(IedConnection self, ControlObjectClient control)
{
    LinkedList_add(self->clientControls, control);
}

void
private_IedConnection_removeControlClient(IedConnection self, ControlObjectClient control)
{
    LinkedList_remove(self->clientControls, control);
}

