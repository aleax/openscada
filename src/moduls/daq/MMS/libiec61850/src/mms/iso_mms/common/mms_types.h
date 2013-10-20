/*
 *  mms_types.h
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

#ifndef MMS_TYPES_H_
#define MMS_TYPES_H_

#include "libiec61850_platform_includes.h"
#include "ber_integer.h"

typedef enum {
    MMS_VALUE_NO_RESPONSE,
	MMS_VALUE_OK,
	MMS_VALUE_ACCESS_DENIED,
	MMS_VALUE_VALUE_INVALID,
	MMS_VALUE_TEMPORARILY_UNAVAILABLE,
	MMS_VALUE_OBJECT_ACCESS_UNSUPPORTED
} MmsValueIndication;

typedef enum {
    DATA_ACCESS_ERROR_NO_RESPONSE = -2, /* for server internal purposes only! */
    DATA_ACCESS_ERROR_SUCCESS = -1,
    DATA_ACCESS_ERROR_OBJECT_INVALIDATED = 0,
    DATA_ACCESS_ERROR_HARDWARE_FAULT = 1,
    DATA_ACCESS_ERROR_TEMPORARILY_UNAVAILABLE = 2,
    DATA_ACCESS_ERROR_OBJECT_ACCESS_DENIED = 3,
    DATA_ACCESS_ERROR_OBJECT_UNDEFINED = 4,
    DATA_ACCESS_ERROR_INVALID_ADDRESS = 5,
    DATA_ACCESS_ERROR_TYPE_UNSUPPORTED = 6,
    DATA_ACCESS_ERROR_TYPE_INCONSISTENT = 7,
    DATA_ACCESS_ERROR_OBJECT_ATTRIBUTE_INCONSISTENT = 8,
    DATA_ACCESS_ERROR_OBJECT_ACCESS_UNSUPPORTED = 9,
    DATA_ACCESS_ERROR_OBJECT_NONE_EXISTENT = 10,
    DATA_ACCESS_ERROR_OBJECT_VALUE_INVALID = 11
} MmsDataAccessError;

/**
 * MmsValue - complex value type for MMS Client API
 */
typedef struct sMmsValue MmsValue;

struct sMmsValue {
	MmsType type;
	int deleteValue;
	union uMmsValue {
        MmsDataAccessError dataAccessError;
		struct {
			int size;
			MmsValue** components;
		} structure;
		bool boolean;
		Asn1PrimitiveValue* integer;
		Asn1PrimitiveValue* unsignedInteger;
		struct {
			uint8_t exponentWidth;
			uint8_t formatWidth;
			uint8_t* buf;
		} floatingPoint;
		struct {
			uint16_t size;
			uint16_t maxSize;
			uint8_t* buf;
		} octetString;
		struct {
			int size;     /* Number of bits */
			uint8_t* buf;
		} bitString;
		char* mmsString;
		char* visibleString;
		uint8_t utcTime[8];
		struct {
			uint8_t size;
			uint8_t buf[6];
		} binaryTime;
	} value;
};

/**
 * Type definition for MMS Named Variables
 */
typedef struct sMmsVariableSpecification MmsVariableSpecification;

struct sMmsVariableSpecification {
    MmsType type;
    char* name;
    union uMmsTypeSpecification
    {
        struct sMmsArray {
            int elementCount; /* number of array elements */
            MmsVariableSpecification* elementTypeSpec;
        } array;
        struct sMmsStructure {
            int elementCount;
            MmsVariableSpecification** elements;
        } structure;
        int boolean; /* dummy - not required */
        int integer; /* size of integer in bits */
        int unsignedInteger; /* size of integer in bits */
        struct sMmsFloat
        {
            uint8_t exponentWidth;
            uint8_t formatWidth;
        } floatingpoint;
        int bitString; /* Number of bits in bitstring */
        int octetString; /* Number of octets in octet string */
        int visibleString; /* Maximum size of string */
        int mmsString;
        int utctime; /* dummy - not required */
        int binaryTime; /* size: either 4 or 6 */
    } typeSpec;
};


#endif /* MMS_TYPES_H_ */
