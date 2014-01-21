/*
 *  mms_common.h
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

#ifndef MMS_COMMON_H_
#define MMS_COMMON_H_

#define DEFAULT_MAX_SERV_OUTSTANDING_CALLING 5
#define DEFAULT_MAX_SERV_OUTSTANDING_CALLED 5
#define DEFAULT_DATA_STRUCTURE_NESTING_LEVEL 10
#define DEFAULT_MAX_PDU_SIZE 65000

typedef enum
{
    MMS_ERROR, MMS_INITIATE, MMS_CONFIRMED_REQUEST, MMS_OK, MMS_CONCLUDE
} MmsIndication;

typedef enum {

    /* generic error codes */
    MMS_ERROR_NONE = 0,
    MMS_ERROR_CONNECTION_REJECTED = 1,
    MMS_ERROR_CONNECTION_LOST = 2,
    MMS_ERROR_SERVICE_TIMEOUT = 3,
    MMS_ERROR_PARSING_RESPONSE = 4,
    MMS_ERROR_HARDWARE_FAULT = 5,
    MMS_ERROR_OTHER = 9,

    /* confirmed error PDU codes */
    MMS_ERROR_VMDSTATE_OTHER = 10,

    MMS_ERROR_APPLICATION_REFERENCE_OTHER = 20,

    MMS_ERROR_DEFINITION_OTHER = 30,
    MMS_ERROR_DEFINITION_INVALID_ADDRESS = 31,
    MMS_ERROR_DEFINITION_TYPE_UNSUPPORTED = 32,
    MMS_ERROR_DEFINITION_TYPE_INCONSISTENT = 33,
    MMS_ERROR_DEFINITION_OBJECT_UNDEFINED = 34,
    MMS_ERROR_DEFINITION_OBJECT_EXISTS = 35,
    MMS_ERROR_DEFINITION_OBJECT_ATTRIBUTE_INCONSISTENT = 36,

    MMS_ERROR_RESOURCE_OTHER = 40,

    MMS_ERROR_SERVICE_OTHER = 50,

    MMS_ERROR_SERVICE_PREEMPT_OTHER = 60,

    MMS_ERROR_TIME_RESOLUTION_OTHER = 70,

    MMS_ERROR_ACCESS_OTHER = 80,
    MMS_ERROR_ACCESS_OBJECT_NON_EXISTENT = 81,
    MMS_ERROR_ACCESS_OBJECT_ACCESS_UNSUPPORTED = 82,
    MMS_ERROR_ACCESS_OBJECT_ACCESS_DENIED = 83,
    MMS_ERROR_ACCESS_OBJECT_INVALIDATED = 84,
    MMS_ERROR_ACCESS_OBJECT_VALUE_INVALID = 85, /* for DataAccessError 11 */
    MMS_ERROR_ACCESS_TEMPORARILY_UNAVAILABLE = 86, /* for DataAccessError 2 */

    /* reject codes */
    MMS_ERROR_REJECT_OTHER = 100,
    MMS_ERROR_REJECT_UNKNOWN_PDU_TYPE = 101,
    MMS_ERROR_REJECT_INVALID_PDU = 102,
    MMS_ERROR_REJECT_UNRECOGNIZED_SERVICE = 103,
    MMS_ERROR_REJECT_UNRECOGNIZED_MODIFIER = 104,
    MMS_ERROR_REJECT_REQUEST_INVALID_ARGUMENT = 105

} MmsError;

typedef enum
{
    MMS_ARRAY,
    MMS_STRUCTURE,
    MMS_BOOLEAN,
    MMS_BIT_STRING,
    MMS_INTEGER,
    MMS_UNSIGNED,
    MMS_FLOAT,
    MMS_OCTET_STRING,
    MMS_VISIBLE_STRING,
    MMS_GENERALIZED_TIME,
    MMS_BINARY_TIME,
    MMS_BCD,
    MMS_OBJ_ID,
    MMS_STRING,
    MMS_UTC_TIME,
    MMS_DATA_ACCESS_ERROR
} MmsType;

typedef struct sMmsDomain MmsDomain;

typedef struct sMmsAccessSpecifier {
    MmsDomain* domain;
    char* variableName;
    int arrayIndex; /* -1 --> no index present / ignore index */
    char* componentName;
} MmsAccessSpecifier;

typedef struct {
	char* domainId;
	char* itemId;
	int32_t arrayIndex; /* -1 --> no index present / ignore index */
	char* componentName;
} MmsVariableAccessSpecification;

typedef struct sMmsNamedVariableList* MmsNamedVariableList;
typedef struct sMmsAccessSpecifier* MmsNamedVariableListEntry;

#endif /* MMS_COMMON_H_ */
