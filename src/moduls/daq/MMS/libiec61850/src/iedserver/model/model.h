/*
 *  model.h
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

#ifndef MODEL_H_
#define MODEL_H_

#include "libiec61850_platform_includes.h"
#include "mms_value.h"
#include "iec61850_common.h"

/** \addtogroup server_api_group
 *  @{
 */

/**
 * \brief abstract base type for IEC 61850 data model nodes
 */
typedef struct sModelNode ModelNode;

/**
 * \brief IEC 61850 data model element of type data attribute
 */
typedef struct sDataAttribute DataAttribute;

/**
 * \brief IEC 61850 data model element of type data object
 */
typedef struct sDataObject DataObject;

/**
 * \brief IEC 61850 data model element of type logical node
 */
typedef struct sLogicalNode LogicalNode;

/**
 * \brief IEC 61850 data model element of type logical device
 */
typedef struct sLogicalDevice LogicalDevice;

/**
 * \brief Root node of the IEC 61850 data model. This is usually created by the model generator tool (genmodel.jar)
 */
typedef struct sIedModel IedModel;

/**@}*/

typedef struct sDataSet DataSet;
typedef struct sReportControlBlock ReportControlBlock;
typedef struct sGSEControlBlock GSEControlBlock;


typedef enum eDataAttributeType {
	BOOLEAN,/* int */
	INT8,   /* int8_t */
	INT16,  /* int16_t */
	INT32,  /* int32_t */
	INT64,  /* int64_t */
	INT128, /* no native mapping! */
	INT8U,  /* uint8_t */
	INT16U, /* uint16_t */
	INT24U, /* uint32_t */
	INT32U, /* uint32_t */
	FLOAT32, /* float */
	FLOAT64, /* double */
	ENUMERATED,
	OCTET_STRING_64,
	OCTET_STRING_6,
	OCTET_STRING_8,
	VISIBLE_STRING_32,
	VISIBLE_STRING_64,
	VISIBLE_STRING_65,
	VISIBLE_STRING_129,
	VISIBLE_STRING_255,
	UNICODE_STRING_255,
	TIMESTAMP,
	QUALITY,
	CHECK,
	CODEDENUM,
	GENERIC_BITSTRING,
	CONSTRUCTED,
	ENTRY_TIME,
	PHYCOMADDR
} DataAttributeType;

typedef enum {
	LogicalNodeModelType,
	DataObjectModelType,
	DataAttributeModelType,
} ModelNodeType;

struct sIedModel {
    char* name;
    LogicalDevice* firstChild;
    DataSet** dataSets;
    ReportControlBlock** rcbs;
    GSEControlBlock** gseCBs;
    void (*initializer) ();
};

struct sLogicalDevice {
	char* name;
	LogicalDevice* sibling;
	LogicalNode* firstChild;
	//MmsDomain* mmsDomain;
};

struct sModelNode {
	ModelNodeType modelType;
	char* name;
	ModelNode* parent;
	ModelNode* sibling;
	ModelNode* firstChild;
};

struct sLogicalNode {
	ModelNodeType modelType;
	char* name;
	LogicalDevice* parent;
	ModelNode* sibling;
	ModelNode* firstChild;
};

struct sDataObject {
	ModelNodeType modelType;
	char* name;
	ModelNode* parent;
	ModelNode* sibling;
	ModelNode* firstChild;

	int observerCount; /* Number of observers currently monitoring this node TODO remove attribute */
	int elementCount;  /* > 0 if this is an array */
};

struct sDataAttribute {
	ModelNodeType modelType;
	char* name;
	ModelNode* parent;
	ModelNode* sibling;
	ModelNode* firstChild;

	int observerCount; /* Number of observers currently monitoring this node TODO remove attribute */
	int elementCount;  /* > 0 if this is an array */

	FunctionalConstraint fc;
	DataAttributeType type;

	MmsValue* mmsValue;
};

typedef struct {
	char* logicalDeviceName;
	char* variableName;
	int index;
	char* componentName;
	MmsValue* value;
} DataSetEntry;

struct sDataSet {
	char* logicalDeviceName;
	char* name; /* eg. MMXU1$dataset1 */
	int elementCount;
	DataSetEntry** fcda;
};

struct sReportControlBlock {
	LogicalNode* parent;
	char* name;
	char* rptId;
	bool buffered;
	char* dataSetName; /* pre loaded with relative name in logical node */

	uint32_t confRef;    /* ConfRef - configuration revision */
	uint8_t trgOps;      /* TrgOps - trigger conditions */
	uint8_t options;     /* OptFlds */
	uint32_t bufferTime; /* BufTm - time to buffer events until a report is generated */
	uint32_t intPeriod;  /* IntPrd - integrity period */

	//char* owner;
};

typedef struct {
    uint8_t vlanPriority;
    uint16_t vlanId;
    uint16_t appId;
    uint8_t dstAddress[6];
} PhyComAddress;

struct sGSEControlBlock {
    LogicalNode* parent;
    char* name;
    char* appId;
    char* dataSetName; /* pre loaded with relative name in logical node */
    uint32_t confRef;  /* ConfRef - configuration revision */
    bool fixedOffs;    /* fixed offsets */
    PhyComAddress* address; /* GSE communication parameters */
};

LogicalDevice*
IedModel_getDevice(IedModel* model, char* deviceName);

/*
 *  \param dataSetReference MMS mapping object reference! e.g. ied1Inverter/LLN0$dataset1
 */
DataSet*
IedModel_lookupDataSet(IedModel* model, char* dataSetReference);

int
IedModel_getLogicalDeviceCount(IedModel* iedModel);

LogicalNode*
LogicalDevice_getLogicalNode(LogicalDevice* device, char* nodeName);

int
LogicalDevice_getLogicalNodeCount(LogicalDevice* logicalDevice);

bool
LogicalNode_hasFCData(LogicalNode* node, FunctionalConstraint fc);

bool
LogicalNode_hasBufferedReports(LogicalNode* node);

bool
LogicalNode_hasUnbufferedReports(LogicalNode* node);

bool
DataObject_hasFCData(DataObject* dataObject, FunctionalConstraint fc);

int
ModelNode_getChildCount(ModelNode* modelNode);

char*
ModelNode_getObjectReference(ModelNode* node, char* objectReference);

#endif /* MODEL_H_ */
