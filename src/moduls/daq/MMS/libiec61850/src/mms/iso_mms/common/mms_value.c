/*
 *  MmsValue.c
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

#include "libiec61850_platform_includes.h"
#include "mms_common.h"
#include "mms_value.h"
#include "mms_type_spec.h"

#include "string_utilities.h"
#include "platform_endian.h"

void
memcpyReverseByteOrder(uint8_t* dst, uint8_t* src, int size);

static inline int
bitStringByteSize(MmsValue* value)
{
	int bitSize = value->value.bitString.size;
	return (bitSize / 8) + ((bitSize % 8) > 0);
}

static void
updateStructuredComponent(MmsValue* self, MmsValue* update)
{
	int componentCount;
	MmsValue** selfValues;
	MmsValue** updateValues;

    componentCount = self->value.structure.size;
    selfValues = self->value.structure.components;
    updateValues = update->value.structure.components;

	int i;
	for (i = 0; i < componentCount; i++) {
		MmsValue_update(selfValues[i], updateValues[i]);
	}
}

MmsValue*
MmsValue_newIntegerFromBerInteger(Asn1PrimitiveValue* berInteger)
{
	MmsValue* self = (MmsValue*) calloc(1, sizeof(MmsValue));
	self->type = MMS_INTEGER;

	self->value.integer = berInteger;

	return self;
}

MmsValue*
MmsValue_newUnsignedFromBerInteger(Asn1PrimitiveValue* berInteger)
{
	MmsValue* self = (MmsValue*) calloc(1, sizeof(MmsValue));
	self->type = MMS_UNSIGNED;

	self->value.unsignedInteger = berInteger;

	return self;
}

bool
MmsValue_equals(MmsValue* self, MmsValue* otherValue)
{
    if (self->type == otherValue->type) {
        switch (self->type) {
        case MMS_ARRAY:
        case MMS_STRUCTURE:
            if (self->value.structure.size == otherValue->value.structure.size) {
                int componentCount = self->value.structure.size;

                int i;
                for (i = 0; i < componentCount; i++) {
                    if (!MmsValue_equals(self->value.structure.components[i],
                            otherValue->value.structure.components[i]))
                        return false;
                }

                return true;
            }
            break;

        case MMS_BOOLEAN:
            if (self->value.boolean == otherValue->value.boolean)
                return true;
            break;
        case MMS_FLOAT:
            if (memcmp(self->value.floatingPoint.buf, otherValue->value.floatingPoint.buf,
                    self->value.floatingPoint.formatWidth / 8) == 0)
                return true;
            break;
        case MMS_INTEGER:
            return Asn1PrimitivaValue_compare(self->value.integer, otherValue->value.integer);
            break;
        case MMS_UNSIGNED:
            return Asn1PrimitivaValue_compare(self->value.unsignedInteger, otherValue->value.unsignedInteger);
            break;
        case MMS_UTC_TIME:
            if (memcmp(self->value.utcTime, otherValue->value.utcTime, 8) == 0)
                return true;
            break;
        case MMS_BIT_STRING:
            if (self->value.bitString.size == otherValue->value.bitString.size) {
                if (memcmp(self->value.bitString.buf, otherValue->value.bitString.buf,
                        bitStringByteSize(self)) == 0)
                    return true;

            }
            break;
        case MMS_BINARY_TIME:
            if (self->value.binaryTime.size == otherValue->value.binaryTime.size) {
                if (memcmp(self->value.binaryTime.buf, otherValue->value.binaryTime.buf,
                        self->value.binaryTime.size) == 0)
                    return true;
            }
            break;

        case MMS_OCTET_STRING:
            if (self->value.octetString.size == otherValue->value.octetString.size) {
                if (memcmp(self->value.octetString.buf, otherValue->value.octetString.buf,
                        self->value.octetString.size) == 0)
                    return true;
            }
            break;

        case MMS_VISIBLE_STRING:
            if (self->value.visibleString != NULL) {
                if (otherValue->value.visibleString != NULL) {
                    if (strcmp(self->value.visibleString, otherValue->value.visibleString) == 0)
                        return true;
                }
            }
            else {
                if (otherValue->value.visibleString == NULL)
                    return true;
            }
            break;

        case MMS_STRING:
            if (self->value.mmsString != NULL) {
                if (otherValue->value.mmsString != NULL) {
                    if (strcmp(self->value.mmsString, otherValue->value.mmsString) == 0)
                        return true;
                }
            }
            else {
                if (otherValue->value.mmsString == NULL)
                    return true;
            }
            break;
        }
        return false;
    }
    else
        return false;
}

bool
MmsValue_update(MmsValue* self, MmsValue* update)
{
	if (self->type == update->type) {
		switch (self->type) {
		case MMS_STRUCTURE:
		case MMS_ARRAY:
			updateStructuredComponent(self, update);
			break;
		case MMS_BOOLEAN:
			self->value.boolean = update->value.boolean;
			break;
		case MMS_FLOAT:
			if (self->value.floatingPoint.formatWidth == update->value.floatingPoint.formatWidth) {
				self->value.floatingPoint.exponentWidth = update->value.floatingPoint.exponentWidth;
				memcpy(self->value.floatingPoint.buf, update->value.floatingPoint.buf,
						self->value.floatingPoint.formatWidth / 8);
			}
			else return false;
			break;
		case MMS_INTEGER:
			if (BerInteger_setFromBerInteger(self->value.integer, update->value.integer))
				return true;
			else
				return false;
			break;
		case MMS_UNSIGNED:
			if (BerInteger_setFromBerInteger(self->value.unsignedInteger,
					update->value.unsignedInteger))
				return true;
			else
				return false;
			break;
		case MMS_UTC_TIME:
			memcpy(self->value.utcTime, update->value.utcTime, 8);
			break;
		case MMS_BIT_STRING:
			if (self->value.bitString.size == update->value.bitString.size)
				memcpy(self->value.bitString.buf, update->value.bitString.buf, bitStringByteSize(self));
			else return false;
			break;
		case MMS_OCTET_STRING:
			if (self->value.octetString.maxSize == update->value.octetString.maxSize) {
				memcpy(self->value.octetString.buf, update->value.octetString.buf,
						update->value.octetString.size);

				self->value.octetString.size = update->value.octetString.size;
			}
			else return false;
			break;
		case MMS_VISIBLE_STRING:
			MmsValue_setVisibleString(self, update->value.visibleString);
			break;
		case MMS_STRING:
			MmsValue_setMmsString(self, update->value.mmsString);
			break;
		case MMS_BINARY_TIME:
			self->value.binaryTime.size = update->value.binaryTime.size;
			memcpy(self->value.binaryTime.buf, update->value.binaryTime.buf,
					update->value.binaryTime.size);
			break;
		default:
			return false;
			break;
		}
		return true;
	}
	else
		return false;
}

MmsValue*
MmsValue_newDataAccessError(MmsDataAccessError accessError)
{
    MmsValue* self = (MmsValue*) calloc(1, sizeof(MmsValue));

    self->type = MMS_DATA_ACCESS_ERROR;
    self->value.dataAccessError = accessError;

    return self;
}

MmsValue*
MmsValue_newBitString(int bitSize)
{
	MmsValue* self = (MmsValue*) calloc(1, sizeof(MmsValue));;

	self->type = MMS_BIT_STRING;
	self->value.bitString.size = bitSize;
	self->value.bitString.buf = (uint8_t*) calloc(bitStringByteSize(self), 1);

	return self;
}

static int
getBitStringByteSize(MmsValue* self)
{
	int byteSize;

	if (self->value.bitString.size % 8)
		byteSize = (self->value.bitString.size / 8) + 1;
	else
		byteSize = self->value.bitString.size / 8;

	return byteSize;
}

void
MmsValue_deleteAllBitStringBits(MmsValue* self)
{
	int byteSize = getBitStringByteSize(self);

	int i;
	for (i = 0; i < byteSize; i++) {
		self->value.bitString.buf[i] = 0;
	}
}

void
MmsValue_setAllBitStringBits(MmsValue* self)
{
	int byteSize = getBitStringByteSize(self);

	int i;
	for (i = 0; i < byteSize; i++) {
		self->value.bitString.buf[i] = 0xff;
	}

    int padding = (byteSize * 8) - self->value.bitString.size;

    uint8_t paddingMask = 0;

    for (i = 0; i < padding; i++) {
        paddingMask += (1 << i);
    }

    paddingMask = ~paddingMask;

    self->value.bitString.buf[byteSize - 1] =  self->value.bitString.buf[byteSize - 1] & paddingMask;
}

int
MmsValue_getBitStringSize(MmsValue* self)
{
   return self->value.bitString.size;
}

int
MmsValue_getNumberOfSetBits(MmsValue* self)
{
    int size = MmsValue_getBitStringSize(self);
    int setBitsCount = 0;

    int byteSize = getBitStringByteSize(self);

    int i;
    for (i = 0; i < byteSize; i++) {
        uint8_t currentByte = self->value.bitString.buf[i];

        while (currentByte != 0) {
            if ((currentByte & 1) == 1)
                setBitsCount++;
            currentByte >>= 1;
        }
    }

   return setBitsCount;
}

void
MmsValue_setBitStringBit(MmsValue* self, int bitPos, bool value)
{
	if (bitPos < self->value.bitString.size) {
		int bytePos = bitPos / 8;
		int bitPosInByte = 7 - (bitPos % 8);

		int bitMask = (1 << bitPosInByte);

		if (value)
			self->value.bitString.buf[bytePos] |= bitMask;
		else
			self->value.bitString.buf[bytePos] &= (~bitMask);
	}
}

bool
MmsValue_getBitStringBit(MmsValue* self, int bitPos)
{
	if (bitPos < self->value.bitString.size) {
		int bytePos = bitPos / 8;

		int bitPosInByte = 7 - (bitPos % 8);

		int bitMask = (1 << bitPosInByte);

		if ((self->value.bitString.buf[bytePos] & bitMask) > 0)
			return true;
		else
			return false;

	}
	else return false; /* out of range bits are always zero */
}

uint32_t
MmsValue_getBitStringAsInteger(MmsValue* self)
{
    uint32_t value = 0;

    int bitPos;

    for (bitPos = 0; bitPos < self->value.bitString.size; bitPos++) {
        if (MmsValue_getBitStringBit(self, bitPos)) {
            value += (1 << bitPos);
        }
    }

    return value;
}

void
MmsValue_setBitStringFromInteger(MmsValue* self, uint32_t intValue)
{
    int bitPos;

    for (bitPos = 0; bitPos < self->value.bitString.size; bitPos++) {
        if ((intValue & 1) == 1)
            MmsValue_setBitStringBit(self, bitPos, true);
        else
            MmsValue_setBitStringBit(self, bitPos, false);

        intValue = intValue >> 1;
    }
}


MmsValue*
MmsValue_newFloat(float variable)
{
	MmsValue* value = (MmsValue*) calloc(1, sizeof(MmsValue));;

	value->type = MMS_FLOAT;
	value->value.floatingPoint.formatWidth = 32;
	value->value.floatingPoint.exponentWidth = 8;
	value->value.floatingPoint.buf = (uint8_t*) malloc(4);

	*((float*) value->value.floatingPoint.buf) = variable;

	return value;
}

void
MmsValue_setFloat(MmsValue* value, float newFloatValue)
{
	if (value->type == MMS_FLOAT) {
		if (value->value.floatingPoint.formatWidth == 32) {
			*((float*) value->value.floatingPoint.buf) = newFloatValue;
		}
		else if (value->value.floatingPoint.formatWidth == 64) {
			*((double*) value->value.floatingPoint.buf) = (double) newFloatValue;
		}
	}
}

void
MmsValue_setDouble(MmsValue* value, double newFloatValue)
{
	if (value->type == MMS_FLOAT) {
		if (value->value.floatingPoint.formatWidth == 32) {
			*((float*) value->value.floatingPoint.buf) = (float) newFloatValue;
		}
		else if (value->value.floatingPoint.formatWidth == 64) {
			*((double*) value->value.floatingPoint.buf) = newFloatValue;
		}
	}
}

MmsValue*
MmsValue_newDouble(double variable)
{
	MmsValue* value = (MmsValue*) calloc(1, sizeof(MmsValue));

	value->type = MMS_FLOAT;
	value->value.floatingPoint.formatWidth = 64;
	value->value.floatingPoint.exponentWidth = 11;
	value->value.floatingPoint.buf = (uint8_t*) malloc(8);

	*((double*) value->value.floatingPoint.buf) = variable;

	return value;
}

MmsValue*
MmsValue_newIntegerFromInt16(int16_t integer)
{
	MmsValue* value = (MmsValue*) calloc(1, sizeof(MmsValue));;

	value->type = MMS_INTEGER;
	value->value.integer = BerInteger_createFromInt32((int32_t) integer);

	return value;
}

void
MmsValue_setInt32(MmsValue* value, int32_t integer)
{
	if (value->type == MMS_INTEGER) {
		if (Asn1PrimitiveValue_getMaxSize(value->value.integer) >= 4) {
			BerInteger_setInt32(value->value.integer, integer);
		}
	}
}

void
MmsValue_setUint32(MmsValue* value, uint32_t integer)
{
    if (value->type == MMS_UNSIGNED) {
        if (Asn1PrimitiveValue_getMaxSize(value->value.integer) >= 4) {
            BerInteger_setUint32(value->value.integer, integer);
        }
    }
}


void
MmsValue_setUint16(MmsValue* value, uint16_t integer)
{
    if (value->type == MMS_UNSIGNED) {
        if (Asn1PrimitiveValue_getMaxSize(value->value.integer) >= 2) {
            BerInteger_setUint16(value->value.integer, integer);
        }
    }
}


void
MmsValue_setUint8(MmsValue* value, uint8_t integer)
{
    if (value->type == MMS_UNSIGNED) {
        if (Asn1PrimitiveValue_getMaxSize(value->value.integer) >= 1) {
            BerInteger_setUint8(value->value.integer, integer);
        }
    }

}


void
MmsValue_setBoolean(MmsValue* value, bool boolValue)
{
	value->value.boolean = boolValue;
}

bool
MmsValue_getBoolean(MmsValue* value)
{
    return value->value.boolean;
}


MmsValue*
MmsValue_setUtcTime(MmsValue* value, uint32_t timeval)
{
	uint8_t* timeArray = (uint8_t*) &timeval;
	uint8_t* valueArray = value->value.utcTime;

#ifdef ORDER_LITTLE_ENDIAN
		memcpyReverseByteOrder(valueArray, timeArray, 4);
#else
		memcpy(valueArray, timeArray, 4);
#endif

	return value;
}


MmsValue*
MmsValue_setUtcTimeMs(MmsValue* self, uint64_t timeval)
{
	uint32_t timeval32 = (timeval / 1000LL);

    uint8_t* timeArray = (uint8_t*) &timeval32;
	uint8_t* valueArray = self->value.utcTime;

#ifdef ORDER_LITTLE_ENDIAN
		memcpyReverseByteOrder(valueArray, timeArray, 4);
#else
		memcpy(valueArray, timeArray, 4);
#endif

	uint32_t remainder = (timeval % 1000LL);
	uint32_t fractionOfSecond = (remainder) * 16777 + ((remainder * 216) / 1000);

	/* encode fraction of second */
	valueArray[4] = ((fractionOfSecond >> 16) & 0xff);
	valueArray[5] = ((fractionOfSecond >> 8) & 0xff);
	valueArray[6] = (fractionOfSecond & 0xff);

	/* encode time quality */
	valueArray[7] = 0x0a; /* 10 bit sub-second time accuracy */

	return self;
}

void
MmsValue_setUtcTimeByBuffer(MmsValue* self, uint8_t* buffer)
{
    uint8_t* valueArray = self->value.utcTime;

    int i;
    for (i = 0; i < 8; i++) {
        valueArray[i] = buffer[i];
    }
}

uint64_t
MmsValue_getUtcTimeInMs(MmsValue* self)
{
    uint32_t timeval32;
    uint8_t* valueArray = self->value.utcTime;

#ifdef ORDER_LITTLE_ENDIAN
    memcpyReverseByteOrder((uint8_t*) &timeval32, valueArray, 4);
#else
    memcpy((uint8_t*) &timeval32, valueArray, 4);
#endif

    uint32_t fractionOfSecond = 0;

    fractionOfSecond = (valueArray[4] << 16);
    fractionOfSecond += (valueArray[5] << 8);
    fractionOfSecond += (valueArray[6]);

    uint32_t remainder = fractionOfSecond / 16777;

    uint64_t msVal = (timeval32 * 1000LL) + remainder;

    return (uint64_t) msVal;
}



MmsValue*
MmsValue_newIntegerFromInt32(int32_t integer)
{
	MmsValue* value = (MmsValue*) calloc(1, sizeof(MmsValue));

	value->type = MMS_INTEGER;
	value->value.integer = BerInteger_createFromInt32(integer);

	return value;
}

MmsValue*
MmsValue_newUnsignedFromUint32(uint32_t integer)
{
	MmsValue* value = (MmsValue*) calloc(1, sizeof(MmsValue));;

	value->type = MMS_UNSIGNED;
	value->value.unsignedInteger = BerInteger_createFromUint32(integer);

	return value;
}

MmsValue*
MmsValue_newIntegerFromInt64(int64_t integer)
{
	MmsValue* value = (MmsValue*) calloc(1, sizeof(MmsValue));;

	value->type = MMS_INTEGER;
	value->value.integer = BerInteger_createFromInt64(integer);

	return value;
}

/**
 * Convert signed integer to int32_t
 */
int32_t
MmsValue_toInt32(MmsValue* value)
{
	int32_t integerValue = 0;

	if (value->type == MMS_INTEGER)
		BerInteger_toInt32(value->value.integer, &integerValue);
	else if (value->type == MMS_UNSIGNED)
		BerInteger_toInt32(value->value.unsignedInteger, &integerValue);

	return integerValue;
}

uint32_t
MmsValue_toUint32(MmsValue* value)
{
	uint32_t integerValue = 0;

	if (value->type == MMS_INTEGER)
		BerInteger_toUint32(value->value.integer, &integerValue);
	else if (value->type == MMS_UNSIGNED)
		BerInteger_toUint32(value->value.unsignedInteger, &integerValue);

	return integerValue;
}

/**
 * Convert signed integer to int64_t and do sign extension if required
 */
int64_t
MmsValue_toInt64(MmsValue* value)
{
	int64_t integerValue = 0;

	if (value->type == MMS_INTEGER)
			BerInteger_toInt64(value->value.integer, &integerValue);
	else if (value->type == MMS_UNSIGNED)
			BerInteger_toInt64(value->value.unsignedInteger, &integerValue);

	return integerValue;
}

float
MmsValue_toFloat(MmsValue* value)
{
	if (value->type == MMS_FLOAT) {
		if (value->value.floatingPoint.formatWidth == 32) {
			float val;

			val = *((float*) (value->value.floatingPoint.buf));
			return val;
		}
		else if (value->value.floatingPoint.formatWidth == 64) {
			float val;
			val = *((double*) (value->value.floatingPoint.buf));
			return val;
		}
	}
	else
		printf("MmsValue_toFloat: conversion error. Wrong type!\n");

	return 0.f;
}

double
MmsValue_toDouble(MmsValue* value)
{
	if (value->type == MMS_FLOAT) {
		double val;
		if (value->value.floatingPoint.formatWidth == 32) {
			val = (double) *((float*) (value->value.floatingPoint.buf));
			return val;
		}
		if (value->value.floatingPoint.formatWidth == 64) {
			val = *((double*) (value->value.floatingPoint.buf));
			return val;
		}
	}

	return 0.f;
}



uint32_t
MmsValue_toUnixTimestamp(MmsValue* value)
{
	uint32_t timestamp;
	uint8_t* timeArray = (uint8_t*) &timestamp;

if (ORDER_LITTLE_ENDIAN) {
	timeArray[0] = value->value.utcTime[3];
	timeArray[1] = value->value.utcTime[2];
	timeArray[2] = value->value.utcTime[1];
	timeArray[3] = value->value.utcTime[0];
}
else {
    timeArray[0] = value->value.utcTime[0];
    timeArray[1] = value->value.utcTime[1];
    timeArray[2] = value->value.utcTime[2];
    timeArray[3] = value->value.utcTime[3];
}

	return timestamp;
}


// create a deep clone
MmsValue*
MmsValue_clone(MmsValue* value)
{
	MmsValue* newValue = (MmsValue*) calloc(1, sizeof(MmsValue));
	newValue->deleteValue = value->deleteValue;
	newValue->type = value->type;
	int size;

	switch(value->type) {

	case MMS_ARRAY:
	case MMS_STRUCTURE:
		{
			int componentCount = value->value.structure.size;
			newValue->value.structure.size = componentCount;
			newValue->value.structure.components = (MmsValue**) calloc(componentCount, sizeof(MmsValue*));
			int i;
			for (i = 0; i < componentCount; i++) {
				newValue->value.structure.components[i] =
						MmsValue_clone(value->value.structure.components[i]);
			}
		}
		break;

	case MMS_INTEGER:
		newValue->value.integer = Asn1PrimitiveValue_clone(value->value.integer);
		break;
	case MMS_UNSIGNED:
		newValue->value.unsignedInteger = Asn1PrimitiveValue_clone(value->value.unsignedInteger);
		break;
	case MMS_FLOAT:
		newValue->value.floatingPoint.formatWidth = value->value.floatingPoint.formatWidth;
		newValue->value.floatingPoint.exponentWidth = value->value.floatingPoint.exponentWidth;
		size = value->value.floatingPoint.formatWidth / 8;
		newValue->value.floatingPoint.buf = (uint8_t*) malloc(size);
		memcpy(newValue->value.floatingPoint.buf, value->value.floatingPoint.buf, size);
		break;
	case MMS_BIT_STRING:
		newValue->value.bitString.size = value->value.bitString.size;
		size = bitStringByteSize(value);
		newValue->value.bitString.buf = (uint8_t*) malloc(size);
		memcpy(newValue->value.bitString.buf, value->value.bitString.buf, size);
		break;
	case MMS_BOOLEAN:
		newValue->value.boolean = value->value.boolean;
		break;
	case MMS_OCTET_STRING:
		size = value->value.octetString.size;
		newValue->value.octetString.size = size;
		newValue->value.octetString.maxSize  = value->value.octetString.maxSize;
		newValue->value.octetString.buf = (uint8_t*) malloc(value->value.octetString.maxSize);
		memcpy(newValue->value.octetString.buf, value->value.octetString.buf, size);
		break;
	case MMS_VISIBLE_STRING:
		size = strlen(value->value.visibleString) + 1;
		newValue->value.visibleString = (char*) malloc(size);
		strcpy(newValue->value.visibleString, value->value.visibleString);
		break;
	case MMS_UTC_TIME:
		memcpy(newValue->value.utcTime, value->value.utcTime, 8);
		break;
	case MMS_BINARY_TIME:
	    newValue->value.binaryTime.size = value->value.binaryTime.size;
	    memcpy(newValue->value.binaryTime.buf, value->value.binaryTime.buf, 6);
	    break;
	case MMS_STRING:
	    size = strlen(value->value.mmsString) + 1;
        newValue->value.mmsString = (char*) malloc(size);
        strcpy(newValue->value.mmsString, value->value.mmsString);
	    break;
	}

	return newValue;
}

uint32_t
MmsValue_getArraySize(MmsValue* value)
{
	return value->value.structure.size;
}

void
MmsValue_deleteIfNotNull(MmsValue* value)
{
    if (value != NULL)
        MmsValue_delete(value);
}

void
MmsValue_delete(MmsValue* value)
{
    switch (value->type) {
    case MMS_INTEGER:
        Asn1PrimitiveValue_destroy(value->value.integer);
        break;
    case MMS_UNSIGNED:
        Asn1PrimitiveValue_destroy(value->value.unsignedInteger);
        break;
    case MMS_FLOAT:
        free(value->value.floatingPoint.buf);
        break;
    case MMS_BIT_STRING:
        free(value->value.bitString.buf);
        break;
    case MMS_OCTET_STRING:
        free(value->value.octetString.buf);
        break;
    case MMS_VISIBLE_STRING:
        if (value->value.visibleString != NULL)
            free(value->value.visibleString);
        break;
    case MMS_STRING:
        if (value->value.mmsString != NULL)
            free(value->value.mmsString);
        break;
    case MMS_ARRAY:
    case MMS_STRUCTURE:
        {
            int componentCount = value->value.structure.size;
            int i;

            for (i = 0; i < componentCount; i++) {
            	if (value->value.structure.components[i] != NULL)
            		MmsValue_delete(value->value.structure.components[i]);
            }
        }
        free(value->value.structure.components);
        break;
    }

	free(value);
}

MmsValue*
MmsValue_newInteger(int size /*integer size in bits*/)
{
	MmsValue* value = (MmsValue*) calloc(1, sizeof(MmsValue));
	value->type = MMS_INTEGER;

	if (size <= 32)
		value->value.integer = BerInteger_createInt32();
	else
		value->value.integer = BerInteger_createInt64();

	return value;
}

MmsValue*
MmsValue_newUnsigned(int size /*integer size in bits*/)
{
	MmsValue* value = (MmsValue*) calloc(1, sizeof(MmsValue));
	value->type = MMS_UNSIGNED;

	if (size <= 32)
		value->value.unsignedInteger = BerInteger_createInt32();
	else
		value->value.unsignedInteger = BerInteger_createInt64();

	return value;
}

MmsValue*
MmsValue_newBoolean(bool boolean)
{
	MmsValue* self = (MmsValue*) calloc(1, sizeof(MmsValue));
	self->type = MMS_BOOLEAN;
	if (boolean == true)
		self->value.boolean = 1;
	else
		self->value.boolean = 0;

	return self;
}

MmsValue*
MmsValue_newOctetString(int size, int maxSize)
{
	MmsValue* self = (MmsValue*) calloc(1, sizeof(MmsValue));
	self->type = MMS_OCTET_STRING;
	self->value.octetString.size = size;
	self->value.octetString.maxSize = maxSize;
	self->value.octetString.buf = (uint8_t*) calloc(1, maxSize);

	return self;
}

void
MmsValue_setOctetString(MmsValue* self, uint8_t* buf, int size)
{
    if (size <= self->value.octetString.maxSize) {
        memcpy(self->value.octetString.buf, buf, size);
        self->value.octetString.size = size;
    }
}

MmsValue*
MmsValue_newStructure(MmsVariableSpecification* typeSpec)
{
	MmsValue* self = (MmsValue*) calloc(1, sizeof(MmsValue));

	self->type = MMS_STRUCTURE;
	int componentCount = typeSpec->typeSpec.structure.elementCount;
	self->value.structure.size = componentCount;
	self->value.structure.components = (MmsValue**) calloc(componentCount, sizeof(MmsValue*));

	int i;
	for (i = 0; i < componentCount; i++) {
		self->value.structure.components[i] =
				MmsValue_newDefaultValue(typeSpec->typeSpec.structure.elements[i]);
	}

	return self;
}

MmsValue*
MmsValue_newDefaultValue(MmsVariableSpecification* typeSpec)
{
	MmsValue* value;

	switch (typeSpec->type) {
	case MMS_INTEGER:
		value = MmsValue_newInteger(typeSpec->typeSpec.integer);
		break;
	case MMS_UNSIGNED:
		value = MmsValue_newUnsigned(typeSpec->typeSpec.unsignedInteger);
		break;
	case MMS_FLOAT:
		value = (MmsValue*) calloc(1, sizeof(MmsValue));
		value->type = MMS_FLOAT;
		value->value.floatingPoint.exponentWidth = typeSpec->typeSpec.floatingpoint.exponentWidth;
		value->value.floatingPoint.formatWidth = typeSpec->typeSpec.floatingpoint.formatWidth;
		value->value.floatingPoint.buf = (uint8_t*) calloc(1, typeSpec->typeSpec.floatingpoint.formatWidth / 8);
		break;
	case MMS_BIT_STRING:
		value = (MmsValue*) calloc(1, sizeof(MmsValue));
		value->type = MMS_BIT_STRING;
		{
			int bitSize = abs(typeSpec->typeSpec.bitString);
			value->value.bitString.size = bitSize;
			int size = (bitSize / 8) + ((bitSize % 8) > 0);
			value->value.bitString.buf = (uint8_t*) calloc(1, size);
		}
		break;
	case MMS_OCTET_STRING:
		value = (MmsValue*) calloc(1, sizeof(MmsValue));
		value->type = MMS_OCTET_STRING;

		if (typeSpec->typeSpec.octetString < 0)
			value->value.octetString.size = 0;
		else
			value->value.octetString.size = typeSpec->typeSpec.octetString;

		value->value.octetString.maxSize = abs(typeSpec->typeSpec.octetString);
		value->value.octetString.buf = (uint8_t*) calloc(1, abs(typeSpec->typeSpec.octetString));
		break;
	case MMS_VISIBLE_STRING:
		value = MmsValue_newVisibleString(NULL);
		break;
	case MMS_BOOLEAN:
		value = MmsValue_newBoolean(false);
		break;
	case MMS_UTC_TIME:
		value = (MmsValue*) calloc(1, sizeof(MmsValue));
		value->type = MMS_UTC_TIME;
		break;
	case MMS_ARRAY:
		value = MmsValue_createArray(typeSpec->typeSpec.array.elementTypeSpec,
				typeSpec->typeSpec.array.elementCount);
		break;
	case MMS_STRUCTURE:
		value = MmsValue_newStructure(typeSpec);
		break;
	case MMS_STRING:
		value = MmsValue_newMmsString(NULL);
		break;
	case MMS_BINARY_TIME:
		if (typeSpec->typeSpec.binaryTime == 4)
			value = MmsValue_newBinaryTime(true);
		else
			value = MmsValue_newBinaryTime(false);
		break;
	}

	value->deleteValue = 0;

	return value;
}

static inline void
setVisibleStringValue(MmsValue* value, char* string)
{
	if (string != NULL)
		value->value.visibleString = copyString(string);
	else
		value->value.visibleString = NULL;
}

MmsValue*
MmsValue_newVisibleString(char* string)
{
	MmsValue* value = (MmsValue*) calloc(1, sizeof(MmsValue));
	value->type = MMS_VISIBLE_STRING;

	setVisibleStringValue(value, string);

	return value;
}

MmsValue*
MmsValue_newBinaryTime(bool timeOfDay)
{
	MmsValue* value = (MmsValue*) calloc(1, sizeof(MmsValue));
	value->type = MMS_BINARY_TIME;

	if (timeOfDay == true)
		value->value.binaryTime.size = 4;
	else
		value->value.binaryTime.size = 6;

	return value;
}

void
MmsValue_setBinaryTime(MmsValue* value, uint64_t timestamp)
{
    uint64_t mmsTime = timestamp - (441763200000LL);

    uint8_t* binaryTimeBuf = value->value.binaryTime.buf;

    if (value->value.binaryTime.size == 6) {
        uint16_t daysDiff = mmsTime / (86400000LL);
        uint8_t* daysDiffBuf = (uint8_t*) &daysDiff;

        if (ORDER_LITTLE_ENDIAN) {
            binaryTimeBuf[4] = daysDiffBuf[1];
            binaryTimeBuf[5] = daysDiffBuf[0];
        }
        else {
            binaryTimeBuf[4] = daysDiffBuf[0];
            binaryTimeBuf[5] = daysDiffBuf[1];
        }
    }

    uint32_t msSinceMidnight = mmsTime % (86400000LL);
    uint8_t* msSinceMidnightBuf = (uint8_t*) &msSinceMidnight;

    if (ORDER_LITTLE_ENDIAN) {
        binaryTimeBuf[0] = msSinceMidnightBuf[3];
        binaryTimeBuf[1] = msSinceMidnightBuf[2];
        binaryTimeBuf[2] = msSinceMidnightBuf[1];
        binaryTimeBuf[3] = msSinceMidnightBuf[0];
    }
    else {
        binaryTimeBuf[0] = msSinceMidnightBuf[0];
        binaryTimeBuf[1] = msSinceMidnightBuf[1];
        binaryTimeBuf[2] = msSinceMidnightBuf[2];
        binaryTimeBuf[3] = msSinceMidnightBuf[3];
    }
}

uint64_t
MmsValue_getBinaryTimeAsUtcMs(MmsValue* value)
{
    uint64_t timestamp = 0;

    uint8_t* binaryTimeBuf = value->value.binaryTime.buf;

    if (value->value.binaryTime.size == 6) {

        uint16_t daysDiff;

        daysDiff = binaryTimeBuf[4] * 256;
        daysDiff += binaryTimeBuf[5];

        uint64_t mmsTime;

        mmsTime = daysDiff * (86400000LL);


        timestamp = mmsTime + (441763200000LL);
    }

    uint32_t msSinceMidnight = 0;

    msSinceMidnight = binaryTimeBuf[0] << 24;
    msSinceMidnight += binaryTimeBuf[1] << 16;
    msSinceMidnight += binaryTimeBuf[2] << 8;
    msSinceMidnight += binaryTimeBuf[3];

    timestamp += msSinceMidnight;

    return timestamp;
}

MmsDataAccessError
MmsValue_getDataAccessError(MmsValue* self)
{
    return self->value.dataAccessError;
}

static inline void
setMmsStringValue(MmsValue* value, char* string)
{
	if (string != NULL)
		value->value.mmsString = copyString(string);
	else
		value->value.mmsString = NULL;
}

MmsValue*
MmsValue_newMmsString(char* string)
{
	MmsValue* value = (MmsValue*) calloc(1, sizeof(MmsValue));
	value->type = MMS_STRING;

	setMmsStringValue(value, string);

	return value;
}

void
MmsValue_setMmsString(MmsValue* value, char* string)
{
	if (value->type == MMS_STRING) {
		if (value->value.mmsString != NULL)
			free(value->value.mmsString);

		setMmsStringValue(value, string);
	}
}

MmsValue*
MmsValue_newVisibleStringFromByteArray(uint8_t* byteArray, int size)
{
	MmsValue* value = (MmsValue*) calloc(1, sizeof(MmsValue));
	value->type = MMS_VISIBLE_STRING;

	value->value.visibleString = createStringFromBuffer(byteArray, size);

	return value;
}

void
MmsValue_setVisibleString(MmsValue* value, char* string)
{
	if (value->type == MMS_VISIBLE_STRING) {
		if (value->value.visibleString != NULL)
			free(value->value.visibleString);

		setVisibleStringValue(value, string);
	}
}

char*
MmsValue_toString(MmsValue* value)
{
	if (value->type == MMS_VISIBLE_STRING)
		return value->value.visibleString;
	else if (value->type == MMS_STRING)
	    return value->value.mmsString;

	return NULL;
}

MmsValue*
MmsValue_newUtcTime(uint32_t timeval)
{
	MmsValue* value = (MmsValue*) calloc(1, sizeof(MmsValue));
	value->type = MMS_UTC_TIME;

	uint8_t* timeArray = (uint8_t*) &timeval;
	uint8_t* valueArray = value->value.utcTime;

#ifdef ORDER_LITTLE_ENDIAN
	valueArray[0] = timeArray[3];
	valueArray[1] = timeArray[2];
	valueArray[2] = timeArray[1];
	valueArray[3] = timeArray[0];
#else
	valueArray[0] = timeArray[0];
	valueArray[1] = timeArray[1];
	valueArray[2] = timeArray[2];
	valueArray[3] = timeArray[3];
#endif

	return value;
}


MmsValue*
MmsValue_newUtcTimeByMsTime(uint64_t timeval)
{
    MmsValue* value = (MmsValue*) calloc(1, sizeof(MmsValue));
    value->type = MMS_UTC_TIME;

    MmsValue_setUtcTimeMs(value, timeval);

    return value;
}

MmsValue*
MmsValue_createArray(MmsVariableSpecification* elementType, int size)
{
	MmsValue* array = (MmsValue*) calloc(1, sizeof(MmsValue));

	array->type = MMS_ARRAY;
	array->value.structure.size = size;
	array->value.structure.components = (MmsValue**) calloc(size, sizeof(MmsValue*));

	int i;
	for (i = 0; i < size; i++) {
		array->value.structure.components[i] = MmsValue_newDefaultValue(elementType);
	}

	return array;
}

MmsValue*
MmsValue_createEmtpyArray(int size)
{
	MmsValue* array = (MmsValue*) calloc(1, sizeof(MmsValue));

	array->type = MMS_ARRAY;
	array->value.structure.size = size;
	array->value.structure.components = (MmsValue**) calloc(size, sizeof(MmsValue*));

	int i;
	for (i = 0; i < size; i++) {
		array->value.structure.components[i] = NULL;
	}

	return array;
}

MmsValue*
MmsValue_createEmptyStructure(int size)
{
    MmsValue* structure = MmsValue_createEmtpyArray(size);

    structure->type = MMS_STRUCTURE;

    return structure;
}

void
MmsValue_setElement(MmsValue* complexValue, int index, MmsValue* elementValue)
{
    if ((complexValue->type != MMS_ARRAY) && (complexValue->type != MMS_STRUCTURE))
        return;

	if ((index < 0) || (index >= complexValue->value.structure.size))
		return;

	complexValue->value.structure.components[index] = elementValue;
}

MmsValue*
MmsValue_getElement(MmsValue* complexValue, int index)
{
	if ((complexValue->type != MMS_ARRAY) && (complexValue->type != MMS_STRUCTURE))
		return NULL;

	if ((index < 0) || (index >= complexValue->value.structure.size))
		return NULL;

	return complexValue->value.structure.components[index];
}

void
MmsValue_setDeletable(MmsValue* value)
{
	value->deleteValue = 1;
}

int
MmsValue_isDeletable(MmsValue* value)
{
	return value->deleteValue;
}

MmsType
MmsValue_getType(MmsValue* value)
{
	return value->type;
}

MmsValue*
MmsValue_getSubElement(MmsValue* self, MmsVariableSpecification* varSpec, char* mmsPath)
{
    return MmsVariableSpecification_getChildValue(varSpec, self, mmsPath);
}
