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

typedef enum eIedState
{
    IED_STATE_IDLE,
    IED_STATE_CONNECTED
} IedStateInt;

struct sIedConnection
{
    MmsConnection connection;
    IedStateInt state;
    LinkedList enabledReports;
    LinkedList logicalDevices;
    LinkedList clientControls;
    LastApplError lastApplError;
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
// lastUpdateTime;
};

static IedClientError
mapMmsErrorToIedError(MmsClientError mmsError)
{
    switch (mmsError) {
    case MMS_ERROR_NONE:
        return IED_ERROR_OK;

    case MMS_ERROR_CONNECTION_LOST:
        return IED_ERROR_NOT_CONNECTED;

    case MMS_ERROR_SERVICE_ERROR:
        return IED_ERROR_ACCESS_DENIED;

    case MMS_ERROR_TIMEOUT:
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

ClientDataSet
ClientDataSet_create(char* dataSetReference)
{
    ClientDataSet self = (ClientDataSet) calloc(1, sizeof(struct sClientDataSet));

    self->dataSetReference = copyString(dataSetReference);
    StringUtils_replace(self->dataSetReference, '.', '$');

    self->dataSetValues = NULL;

    return self;
}

void
ClientDataSet_setDataSetValues(ClientDataSet self, MmsValue* dataSetValues)
{
    self->dataSetValues = dataSetValues;
}

MmsValue*
ClientDataSet_getDataSetValues(ClientDataSet self)
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

    return self;
}

void
ClientReport_destroy(ClientReport self)
{
    free(self);
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

    // printf(" %s : %s : %s : %i\n", cntrlObjName, separator + 4, nextSeparator, cntrlObjNameLen);

    if (memcmp(cntrlObjName, separator + 4, cntrlObjNameLen) == 0)
        return true;

    return false;
}

static void
informationReportHandler(void* parameter, char* domainName,
        char* variableListName, MmsValue* value, bool isVariableListName)
{
    IedConnection self = (IedConnection) parameter;

    if (DEBUG)
        printf("received information report for %s\n", variableListName);

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

            if (DEBUG)
                printf("  with datSet %s\n", datSetName);

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

            if (DEBUG)
                printf("Found enabled report!\n");

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

            if (DEBUG)
                printf("Report includes %i data set elements\n", includedElements);

            int valueIndex = inclusionIndex + 1;

            /* skip data-reference fields */
            if (MmsValue_getBitStringBit(optFlds, 5) == true)
                valueIndex += includedElements;

            int i;

            ClientDataSet dataSet = report->dataSet;
            MmsValue* dataSetValues = ClientDataSet_getDataSetValues(dataSet);

            for (i = 0; i < ClientDataSet_getDataSetSize(dataSet); i++) {
                if (MmsValue_getBitStringBit(inclusion, i) == true) {

                    MmsValue* dataSetElement = MmsValue_getElement(dataSetValues, i);

                    MmsValue* newElementValue = MmsValue_getElement(value, valueIndex);

                    if (DEBUG)
                        printf("  update element value type: %i\n", MmsValue_getType(newElementValue));

                    MmsValue_update(dataSetElement, newElementValue);

                    valueIndex++;
                }
            }

            // TODO update inclusion fields

            if (report->callback != NULL) {
                report->callback(report->callbackParameter, report);
            }
        }
        else {
            if (strcmp(variableListName, "LastApplError") == 0) {
                if (DEBUG)
                    printf("received LastApplError\n");

                MmsValue* lastAppIError = value;

                MmsValue* cntrlObj = MmsValue_getElement(lastAppIError, 0);

                MmsValue* error = MmsValue_getElement(lastAppIError, 1);
                MmsValue* origin = MmsValue_getElement(lastAppIError, 2);
                MmsValue* ctlNum = MmsValue_getElement(lastAppIError, 3);
                MmsValue* addCause = MmsValue_getElement(lastAppIError, 4);

                if (DEBUG)
                    printf("  CntrlObj: %s\n", MmsValue_toString(cntrlObj));
                if (DEBUG)
                    printf("  ctlNum: %u\n", MmsValue_toUint32(ctlNum));
                if (DEBUG)
                    printf("  addCause: %i\n", MmsValue_toInt32(addCause));
                if (DEBUG)
                    printf("  error: %i\n", MmsValue_toInt32(error));

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

    return self;
}

void
IedConnection_connect(IedConnection self, IedClientError* error, char* hostname, int tcpPort)
{
    MmsClientError mmsError;

    self->connection = MmsConnection_create();

    MmsConnection_setInformationReportHandler(self->connection, informationReportHandler, self);

    MmsIndication indication = MmsConnection_connect(self->connection,
            &mmsError, hostname, tcpPort);

    if (indication == MMS_OK) {
        *error = IED_ERROR_OK;
        self->state = IED_STATE_CONNECTED;
    }
    else {
        *error = mapMmsErrorToIedError(mmsError);
        MmsConnection_destroy(self->connection);
        self->connection = NULL;
    }
}

void
IedConnection_close(IedConnection self)
{
    if (self->state == IED_STATE_CONNECTED) {
        self->state = IED_STATE_IDLE;
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

    free(self);
}

MmsVariableSpecification*
IedConnection_getVariableSpecification(IedConnection self, IedClientError* error, char* objectReference,
        FunctionalConstraint fc)
{
    char* domainId;
    char* itemId;

    MmsClientError mmsError;
    MmsVariableSpecification* varSpec = NULL;

    domainId = MmsMapping_getMmsDomainFromObjectReference(objectReference, NULL);
    itemId = MmsMapping_createMmsVariableNameFromObjectReference(objectReference, fc, NULL);

    if ((domainId == NULL || itemId == NULL)) {
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

    if ((domainId == NULL || itemId == NULL)) {
        *error = IED_ERROR_OBJECT_REFERENCE_INVALID;
        goto cleanup_and_exit;
    }

    MmsClientError mmsError;

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

    MmsClientError mmsError;

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
    MmsClientError mmsError;

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
        *error = IED_ERROR_NOT_CONNECTED;
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

    if (DEBUG)
        printf("Found LD %s search for variables of LN %s ...\n", logicalDeviceName, logicalNodeName);

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

ClientDataSet
IedConnection_getDataSetValues(IedConnection self, IedClientError* error, char* dataSetReference,
        ClientDataSet dataSet)
{
    char* domainId;
    char* itemId;

    domainId = (char*) alloca(129);

    domainId = MmsMapping_getMmsDomainFromObjectReference(dataSetReference, domainId);

    itemId = copyString(dataSetReference + strlen(domainId) + 1);

    StringUtils_replace(itemId, '.', '$');

    MmsClientError mmsError;

    MmsValue* dataSetVal =
            MmsConnection_readNamedVariableListValues(self->connection, &mmsError,
                    domainId, itemId, true);

    free(itemId);

    if (dataSetVal == NULL) {
        *error = mapMmsErrorToIedError(mmsError);
        goto cleanup_and_exit;
    }

    if (dataSet == NULL) {
        dataSet = ClientDataSet_create(dataSetReference);
        ClientDataSet_setDataSetValues(dataSet, dataSetVal);
    }
    else {
        MmsValue* dataSetValues = ClientDataSet_getDataSetValues(dataSet);
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

    MmsClientError mmsError;

    domainId = (char*) alloca(129);
    itemId = (char*) alloca(129);

    domainId = MmsMapping_getMmsDomainFromObjectReference(rcbReference, domainId);

    strcpy(itemId, rcbReference + strlen(domainId) + 1);

    StringUtils_replace(itemId, '.', '$');

    if (DEBUG)
        printf("reserve report for (%s) (%s)\n", domainId, itemId);

    int itemIdLen = strlen(itemId);

    strcpy(itemId + itemIdLen, "$Resv");

    MmsValue* resv = MmsValue_newBoolean(resvValue);

    MmsIndication indication =
            MmsConnection_writeVariable(self->connection, &mmsError, domainId,
                    itemId, resv);

    MmsValue_delete(resv);

    if (indication != MMS_OK) {
        if (DEBUG)
            printf("  failed to write to RCB!\n");
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

void
IedConnection_enableReporting(IedConnection self, IedClientError* error,
        char* rcbReference,
        ClientDataSet dataSet,
        int triggerOptions,
        ReportCallbackFunction callback,
        void* callbackParameter)
{
    char* domainId;
    char* itemId;

    MmsClientError mmsError;

    domainId = (char*) alloca(129);
    itemId = (char*) alloca(129);

    domainId = MmsMapping_getMmsDomainFromObjectReference(rcbReference, domainId);

    strcpy(itemId, rcbReference + strlen(domainId) + 1);

    StringUtils_replace(itemId, '.', '$');

    if (DEBUG)
        printf("enable report for (%s) (%s)\n", domainId, itemId);

    int itemIdLen = strlen(itemId);

    // check if data set is matching
    strcpy(itemId + itemIdLen, "$DatSet");
    MmsValue* datSet = MmsConnection_readVariable(self->connection, &mmsError, domainId, itemId);

    if (datSet != NULL) {

        if (DEBUG)
            printf("RCB has dataset: %s\n", MmsValue_toString(datSet));

        bool matching = false;

        if (strcmp(MmsValue_toString(datSet), ClientDataSet_getReference(dataSet)) == 0) {
            if (DEBUG)
                printf("  data sets are matching!\n");
            matching = true;
        }
        else {
            if (DEBUG)
                printf(" data sets (%s) - (%s) not matching!", MmsValue_toString(datSet),
                        ClientDataSet_getReference(dataSet));
        }

        MmsValue_delete(datSet);

        if (!matching) {
            *error = IED_ERROR_ENABLE_REPORT_FAILED_DATASET_MISMATCH;
            goto cleanup_and_exit;
        }

    }
    else {
        if (DEBUG)
            printf("Error accessing RCB!\n");
        *error = IED_ERROR_ACCESS_DENIED;
        return;
    }

    // set include data set reference

    strcpy(itemId + itemIdLen, "$OptFlds");
    MmsValue* optFlds = MmsConnection_readVariable(self->connection, &mmsError,
            domainId, itemId);

    if (MmsValue_getBitStringBit(optFlds, 4) == true) {
        if (DEBUG)
            printf("  data set reference is included in report.\n");
        MmsValue_delete(optFlds);
    }
    else {
        MmsValue_setBitStringBit(optFlds, 4, true);

        MmsIndication indication =
                MmsConnection_writeVariable(self->connection, &mmsError, domainId, itemId, optFlds);

        MmsValue_delete(optFlds);

        if (indication != MMS_OK) {
            if (DEBUG)
                printf("  failed to write to RCB!\n");
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
            if (DEBUG)
                printf("  failed to read trigger options!\n");
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
        ClientReport report = ClientReport_create(dataSet);
        report->callback = callback;
        report->callbackParameter = callbackParameter;
        LinkedList_add(self->enabledReports, report);
        if (DEBUG)
            printf("Successfully enabled report!\n");
    }
    else {
        if (DEBUG)
            printf("Failed to enable report!\n");
        *error = mapMmsErrorToIedError(mmsError);
    }

    cleanup_and_exit:
    return;
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

    if (DEBUG)
        printf("disable reporting for (%s) (%s)\n", domainId, itemId);

    int itemIdLen = strlen(itemId);

    strcpy(itemId + itemIdLen, "$RptEna");
    MmsValue* rptEna = MmsValue_newBoolean(false);

    MmsClientError errorCode;

    MmsIndication indication =
            MmsConnection_writeVariable(self->connection, &errorCode, domainId, itemId, rptEna);

    MmsValue_delete(rptEna);

    if (indication != MMS_OK) {
        if (DEBUG)
            printf("  failed to disable RCB!\n");

        *error = mapMmsErrorToIedError(errorCode);
    }
    else {
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

    MmsClientError mmsError;

    MmsValue* gi = MmsValue_newBoolean(true);

    MmsIndication indication =
            MmsConnection_writeVariable(mmsCon, &mmsError, domainId, itemId, gi);

    MmsValue_delete(gi);

    if (indication != MMS_OK) {
        if (DEBUG)
            printf("failed to trigger GI for %s!\n", rcbReference);

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
IedConnectionPrivate_addControlClient(IedConnection self, ControlObjectClient control)
{
    LinkedList_add(self->clientControls, control);
}

void
IedConnectionPrivate_removeControlClient(IedConnection self, ControlObjectClient control)
{
    LinkedList_remove(self->clientControls, control);
}

