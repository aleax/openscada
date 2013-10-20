/*
 *  model.c
 *
 *  Copyright 2013 Michael Zillgith
 *
 *	This file is part of libIEC61850.
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


#include "model.h"


int
IedModel_getLogicalDeviceCount(IedModel* iedModel)
{
	if (iedModel->firstChild == NULL)
		return 0;

	LogicalDevice* logicalDevice = iedModel->firstChild;

	int ldCount = 1;

	while (logicalDevice->sibling != NULL) {
		logicalDevice = logicalDevice->sibling;
		ldCount++;
	}

	return ldCount;
}

DataSet*
IedModel_lookupDataSet(IedModel* model, char* dataSetReference  /* e.g. ied1Inverter/LLN0$dataset1 */)
{
	DataSet* dataSet = model->dataSets[0];

	char* separator = strchr(dataSetReference, '/');

	if (separator == NULL)
		return NULL;

	int ldNameLen = separator - dataSetReference;

	int i = 0;
	while (dataSet != NULL) {
		if (strncmp(dataSet->logicalDeviceName, dataSetReference, ldNameLen) == 0) {
			if (strcmp(dataSet->name, separator + 1) == 0) {
				return dataSet;
			}
		}

		i++;
		dataSet = model->dataSets[i];
	}

	return NULL;
}

bool
DataObject_hasFCData(DataObject* dataObject, FunctionalConstraint fc)
{
	ModelNode* modelNode = dataObject->firstChild;

	while (modelNode != NULL) {

		if (modelNode->modelType == DataAttributeModelType) {
			DataAttribute* dataAttribute = (DataAttribute*) modelNode;

			if (dataAttribute->fc == fc)
				return true;
		}
		else if (modelNode->modelType == DataObjectModelType) {
			DataObject* dataObject = (DataObject*) modelNode;

			if (DataObject_hasFCData(dataObject, fc))
				return true;
		}

		modelNode = modelNode->sibling;
	}

	return false;
}

bool
LogicalNode_hasFCData(LogicalNode* node, FunctionalConstraint fc)
{
	DataObject* dataObject = (DataObject*) node->firstChild;

	while (dataObject != NULL) {
		if (DataObject_hasFCData(dataObject, fc))
			return true;

		dataObject = (DataObject*) dataObject->sibling;
	}

	return false;
}

int
LogicalDevice_getLogicalNodeCount(LogicalDevice* logicalDevice)
{
	int lnCount = 0;

	LogicalNode* logicalNode = logicalDevice->firstChild;

	while (logicalNode != NULL) {
		logicalNode = (LogicalNode*) logicalNode->sibling;
		lnCount++;
	}

	return lnCount;
}

//ModelNode_getObjectReference

static int
createObjectReference(ModelNode* node, char* objectReference)
{
    ModelNodeType type;

    int bufPos;

    if (node->modelType != LogicalNodeModelType) {
        bufPos = createObjectReference(node->parent, objectReference);

        objectReference[bufPos++] = '.';
    }
    else {
        LogicalNode* lNode = (LogicalNode*) node;

        LogicalDevice* lDevice = lNode->parent;

        bufPos = 0;

        int nameLength = strlen(lDevice->name);

        int i;
        for (i = 0; i < nameLength; i++) {
            objectReference[bufPos++] = lDevice->name[i];
        }

        objectReference[bufPos++] = '/';
    }

    /* append own name */
    int nameLength = strlen(node->name);

    int i;
    for (i = 0; i < nameLength; i++) {
        objectReference[bufPos++] = node->name[i];
    }

    return bufPos;
}

char*
ModelNode_getObjectReference(ModelNode* node, char* objectReference)
{
    if (objectReference == NULL)
        objectReference = (char*) malloc(130);

    int bufPos = createObjectReference(node, objectReference);

    objectReference[bufPos] = 0;

    return objectReference;
}

int
ModelNode_getChildCount(ModelNode* modelNode) {
	int childCount = 0;

	ModelNode* child = modelNode->firstChild;

	while (child != NULL) {
		childCount++;
		child = child->sibling;
	}

	return childCount;
}

