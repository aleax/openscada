/*
 *  ied_server.c
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

#include "iec61850_server.h"
#include "mms_mapping.h"
#include "control.h"
#include "stack_config.h"
#include "ied_server_private.h"

#ifndef DEBUG_IED_SERVER
#define DEBUG_IED_SERVER 0
#endif

struct sIedServer
{
    IedModel* model;
    MmsDevice* mmsDevice;
    MmsServer mmsServer;
    IsoServer isoServer;
    MmsMapping* mmsMapping;
    LinkedList clientConnections;
};

static void
createControlObjects(IedServer self, MmsDomain* domain, char* lnName, MmsVariableSpecification* typeSpec)
{
    MmsMapping* mapping = self->mmsMapping;

    if (typeSpec->type == MMS_STRUCTURE) {
        int coCount = typeSpec->typeSpec.structure.elementCount;
        int i;
        for (i = 0; i < coCount; i++) {
            MmsVariableSpecification* coSpec = typeSpec->typeSpec.structure.elements[i];

            int coElementCount = coSpec->typeSpec.structure.elementCount;

            ControlObject* controlObject = ControlObject_create(self, domain, lnName, coSpec->name);

            MmsValue* structure = MmsValue_newDefaultValue(coSpec);

            ControlObject_setMmsValue(controlObject, structure);

            ControlObject_setTypeSpec(controlObject, coSpec);

            int j;
            for (j = 0; j < coElementCount; j++) {
                MmsVariableSpecification* coElementSpec = coSpec->typeSpec.structure.elements[j];

                if (strcmp(coElementSpec->name, "Oper") == 0) {
                    MmsValue* operVal = MmsValue_getElement(structure, j);
                    ControlObject_setOper(controlObject, operVal);
                }
                else if (strcmp(coElementSpec->name, "Cancel") == 0) {
                    MmsValue* cancelVal = MmsValue_getElement(structure, j);
                    ControlObject_setCancel(controlObject, cancelVal);
                }
                else if (strcmp(coElementSpec->name, "SBOw") == 0) {
                    MmsValue* sbowVal = MmsValue_getElement(structure, j);
                    ControlObject_setSBOw(controlObject, sbowVal);
                }
                else if (!(strcmp(coElementSpec->name, "SBO") == 0)) {
                    if (DEBUG_IED_SERVER)
                        printf("createControlObjects: Unknown element in CO: %s!\n", coElementSpec->name);
                }
            }

            MmsMapping_addControlObject(mapping, controlObject);
        }
    }
}

static void
createMmsServerCache(IedServer self)
{

    int domain = 0;

    for (domain = 0; domain < self->mmsDevice->domainCount; domain++) {

        /* Install all top level MMS named variables (=Logical nodes) in the MMS server cache */
        MmsDomain* logicalDevice = self->mmsDevice->domains[domain];

        int i;

        for (i = 0; i < logicalDevice->namedVariablesCount; i++) {
            char* lnName = logicalDevice->namedVariables[i]->name;

            if (DEBUG_IED_SERVER)
                printf("Insert into cache %s - %s\n", logicalDevice->domainName, lnName);

            int fcCount = logicalDevice->namedVariables[i]->typeSpec.structure.elementCount;
            int j;

            for (j = 0; j < fcCount; j++) {
                MmsVariableSpecification* fcSpec = logicalDevice->namedVariables[i]->typeSpec.structure.elements[j];

                char* fcName = fcSpec->name;

                if (strcmp(fcName, "CO") == 0) {
                    int controlCount = fcSpec->typeSpec.structure.elementCount;

                    createControlObjects(self, logicalDevice, lnName, fcSpec);
                }
                else if ((strcmp(fcName, "BR") != 0) && (strcmp(fcName, "RP") != 0)
                        && (strcmp(fcName, "GO") != 0))
                        {

                    char* variableName = createString(3, lnName, "$", fcName);

                    MmsValue* defaultValue = MmsValue_newDefaultValue(fcSpec);

                    if (DEBUG_IED_SERVER)
                        printf("Insert into cache %s - %s\n", logicalDevice->domainName, variableName);

                    MmsServer_insertIntoCache(self->mmsServer, logicalDevice, variableName, defaultValue);

                    free(variableName);
                }
            }
        }
    }
}

static void
installDefaultValuesForDataAttribute(IedServer self, DataAttribute* dataAttribute,
        char* objectReference, int position)
{
    sprintf(objectReference + position, ".%s", dataAttribute->name);

    char mmsVariableName[255]; //TODO check for optimal size

    MmsValue* value = dataAttribute->mmsValue;

    MmsMapping_createMmsVariableNameFromObjectReference(objectReference, dataAttribute->fc, mmsVariableName);

    char domainName[100]; //TODO check for optimal size

    MmsMapping_getMmsDomainFromObjectReference(objectReference, domainName);

    MmsDomain* domain = MmsDevice_getDomain(self->mmsDevice, domainName);

    if (domain == NULL) {
        if (DEBUG_IED_SERVER) printf("Error domain (%s) not found!\n", domainName);
        return;
    }

    MmsValue* cacheValue = MmsServer_getValueFromCache(self->mmsServer, domain, mmsVariableName);

    dataAttribute->mmsValue = cacheValue;

    if (value != NULL) {
        MmsValue_update(cacheValue, value);
        MmsValue_delete(value);
    }

    int childPosition = strlen(objectReference);
    DataAttribute* subDataAttribute = (DataAttribute*) dataAttribute->firstChild;

    while (subDataAttribute != NULL) {
        installDefaultValuesForDataAttribute(self, subDataAttribute, objectReference, childPosition);

        subDataAttribute = (DataAttribute*) subDataAttribute->sibling;
    }
}

static void
installDefaultValuesForDataObject(IedServer self, DataObject* dataObject,
        char* objectReference, int position)
{
    if (dataObject->elementCount > 0) {
        //TODO check if this should be implemented

        if (DEBUG_IED_SERVER)
            printf("IED_SERVER: DataObject is an array. Skip installing default values in cache\n");
        return;
    }


    sprintf(objectReference + position, ".%s", dataObject->name);

    ModelNode* childNode = dataObject->firstChild;

    int childPosition = strlen(objectReference);

    while (childNode != NULL) {
        if (childNode->modelType == DataObjectModelType) {
            installDefaultValuesForDataObject(self, (DataObject*) childNode, objectReference, childPosition);
        }
        else if (childNode->modelType == DataAttributeModelType) {
            installDefaultValuesForDataAttribute(self, (DataAttribute*) childNode, objectReference, childPosition);
        }

        childNode = childNode->sibling;
    }
}

static void
installDefaultValuesInCache(IedServer self)
{
    IedModel* model = self->model;

    char objectReference[255]; // TODO check for optimal buffer size;

    LogicalDevice* logicalDevice = model->firstChild;

    while (logicalDevice != NULL) {
        sprintf(objectReference, "%s", logicalDevice->name);

        LogicalNode* logicalNode = logicalDevice->firstChild;

        char* nodeReference = objectReference + strlen(objectReference);

        while (logicalNode != NULL) {
            sprintf(nodeReference, "/%s", logicalNode->name);

            DataObject* dataObject = (DataObject*) logicalNode->firstChild;

            int refPosition = strlen(objectReference);

            while (dataObject != NULL) {
                installDefaultValuesForDataObject(self, dataObject, objectReference, refPosition);

                dataObject = (DataObject*) dataObject->sibling;
            }

            logicalNode = (LogicalNode*) logicalNode->sibling;
        }

        logicalDevice = logicalDevice->sibling;
    }
}

static void
updateDataSetsWithCachedValues(IedServer self)
{
    DataSet** dataSets = self->model->dataSets;

    int i = 0;
    while (dataSets[i] != NULL) {

        int fcdaCount = dataSets[i]->elementCount;

        int j = 0;

        for (j = 0; j < fcdaCount; j++) {

            MmsDomain* domain = MmsDevice_getDomain(self->mmsDevice, dataSets[i]->fcda[j]->logicalDeviceName);

            MmsValue* value = MmsServer_getValueFromCache(self->mmsServer, domain, dataSets[i]->fcda[j]->variableName);

            if (value == NULL) {
                if (DEBUG_IED_SERVER) {
                    printf("LD: %s dataset: %s : error cannot get value from cache for %s -> %s!\n",
                            dataSets[i]->logicalDeviceName, dataSets[i]->name,
                            dataSets[i]->fcda[j]->logicalDeviceName,
                            dataSets[i]->fcda[j]->variableName);
                }
            }
            else
                dataSets[i]->fcda[j]->value = value;

        }

        i++;
    }
}

IedServer
IedServer_create(IedModel* iedModel)
{
    IedServer self = (IedServer) calloc(1, sizeof(struct sIedServer));

    self->model = iedModel;

    self->mmsMapping = MmsMapping_create(iedModel);

    self->mmsDevice = MmsMapping_getMmsDeviceModel(self->mmsMapping);

    self->isoServer = IsoServer_create();

    self->mmsServer = MmsServer_create(self->isoServer, self->mmsDevice);

    MmsMapping_setMmsServer(self->mmsMapping, self->mmsServer);

    MmsMapping_installHandlers(self->mmsMapping);

    MmsMapping_setIedServer(self->mmsMapping, self);

    createMmsServerCache(self);

    iedModel->initializer();

    installDefaultValuesInCache(self); /* This will also connect cached MmsValues to DataAttributes */

    updateDataSetsWithCachedValues(self);

    self->clientConnections = LinkedList_create();

    return self;
}

void
IedServer_destroy(IedServer self)
{
    //TODO wait for control and other threads to finish??

    MmsServer_destroy(self->mmsServer);
    IsoServer_destroy(self->isoServer);
    MmsMapping_destroy(self->mmsMapping);

    LinkedList_destroyDeep(self->clientConnections, (LinkedListValueDeleteFunction) private_ClientConnection_destroy);
    free(self);
}

MmsServer
IedServer_getMmsServer(IedServer self)
{
    return self->mmsServer;
}

IsoServer
IedServer_getIsoServer(IedServer self)
{
    return self->isoServer;
}

void
IedServer_start(IedServer self, int tcpPort)
{
    MmsServer_startListening(self->mmsServer, tcpPort);
    MmsMapping_startEventWorkerThread(self->mmsMapping);
}

bool
IedServer_isRunning(IedServer self)
{
    if (IsoServer_getState(self->isoServer) == ISO_SVR_STATE_RUNNING)
        return true;
    else
        return false;
}

void
IedServer_stop(IedServer self)
{
    MmsServer_stopListening(self->mmsServer);

    //TODO waiting for all client connection threads to terminate
}

void
IedServer_lockDataModel(IedServer self)
{
    MmsServer_lockModel(self->mmsServer);
}

void
IedServer_unlockDataModel(IedServer self)
{
    MmsServer_unlockModel(self->mmsServer);
}

MmsDomain*
IedServer_getDomain(IedServer self, char* logicalDeviceName)
{
    return MmsDevice_getDomain(self->mmsDevice, logicalDeviceName);
}

MmsValue*
IedServer_getValue(IedServer self, MmsDomain* domain, char* mmsItemId)
{
    return MmsServer_getValueFromCache(self->mmsServer, domain, mmsItemId);
}

static ControlObject*
lookupControlObject(IedServer self, DataObject* node)
{
    char objectReference[129];

    ModelNode_getObjectReference((ModelNode*) node, objectReference);

    char* separator = strchr(objectReference, '/');

    *separator = 0;

    MmsDomain* domain = MmsDevice_getDomain(self->mmsDevice, objectReference);

    char* lnName = separator + 1;

    separator = strchr(lnName, '.');

    *separator = 0;

    char* objectName = separator + 1;

    separator = strchr(objectName, '.');

    if (separator != NULL)
        *separator = 0;

    ControlObject* controlObject = MmsMapping_getControlObject(self->mmsMapping, domain,
            lnName, objectName);

    return controlObject;
}

void
IedServer_setControlHandler(
        IedServer self,
        DataObject* node,
        ControlHandler listener,
        void* parameter)
{
    ControlObject* controlObject = lookupControlObject(self, node);

    if (controlObject != NULL)
        ControlObject_installListener(controlObject, listener, parameter);
}

void
IedServer_setPerformCheckHandler(IedServer self, DataObject* node, ControlPerformCheckHandler handler, void* parameter)
{
    ControlObject* controlObject = lookupControlObject(self, node);

    if (controlObject != NULL)
        ControlObject_installCheckHandler(controlObject, handler, parameter);
}

void
IedServer_setWaitForExecutionHandler(IedServer self, DataObject* node, ControlWaitForExecutionHandler handler,
        void* parameter)
{
    ControlObject* controlObject = lookupControlObject(self, node);

    if (controlObject != NULL)
        ControlObject_installWaitForExecutionHandler(controlObject, handler, parameter);
}

MmsValue*
IedServer_getAttributeValue(IedServer self, DataAttribute* dataAttribute)
{
    return dataAttribute->mmsValue;
}

void
IedServer_updateAttributeValue(IedServer self, DataAttribute* dataAttribute, MmsValue* value)
{
    if (MmsValue_equals(dataAttribute->mmsValue, value)) {
        MmsMapping_triggerReportObservers(self->mmsMapping, dataAttribute->mmsValue,
                REPORT_CONTROL_VALUE_UPDATE);
    }
    else {
        MmsValue_update(dataAttribute->mmsValue, value);
        MmsMapping_triggerReportObservers(self->mmsMapping, dataAttribute->mmsValue,
                REPORT_CONTROL_VALUE_CHANGED);

#if CONFIG_INCLUDE_GOOSE_SUPPORT == 1
        MmsMapping_triggerGooseObservers(self->mmsMapping, dataAttribute->mmsValue);
#endif

    }

}

void
IedServer_attributeQualityChanged(IedServer self, DataAttribute* dataAttribute)
{
    MmsMapping_triggerReportObservers(self->mmsMapping, dataAttribute->mmsValue,
            REPORT_CONTROL_QUALITY_CHANGED);
}

void
IedServer_enableGoosePublishing(IedServer self)
{
    MmsMapping_enableGoosePublishing(self->mmsMapping);
}

void
IedServer_observeDataAttribute(IedServer self, DataAttribute* dataAttribute,
        AttributeChangedHandler handler)
{
    MmsMapping_addObservedAttribute(self->mmsMapping, dataAttribute, handler);
}

void
IedServer_setConnectionIndicationHandler(IedServer self, IedConnectionIndicationHandler handler, void* parameter)
{
    MmsMapping_setConnectionIndicationHandler(self->mmsMapping, handler, parameter);
}

ClientConnection
private_IedServer_getClientConnectionByHandle(IedServer self, void* serverConnectionHandle)
{
    LinkedList element = LinkedList_getNext(self->clientConnections);
    ClientConnection matchingConnection = NULL;

    while (element != NULL) {
        ClientConnection connection = (ClientConnection) element->data;

        if (private_ClientConnection_getServerConnectionHandle(connection) == serverConnectionHandle) {
            matchingConnection = connection;
            break;
        }

        element = LinkedList_getNext(element);
    }

    return matchingConnection;
}

void
private_IedServer_addNewClientConnection(IedServer self, ClientConnection newClientConnection)
{
    LinkedList_add(self->clientConnections, (void*) newClientConnection);
}

void
private_IedServer_removeClientConnection(IedServer self, ClientConnection clientConnection)
{
    LinkedList_remove(self->clientConnections, clientConnection);
}


