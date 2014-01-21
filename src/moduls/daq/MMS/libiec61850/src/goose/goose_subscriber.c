/*
 *  goose_subscriber.c
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

#include "stack_config.h"
#include "goose_subscriber.h"
#include "ethernet.h"
#include "thread.h"

#include "ber_decode.h"

#include "mms_value.h"

#define ETH_BUFFER_LENGTH 1518

#define ETH_P_GOOSE 0x88b8

struct sGooseSubscriber {
    char* datSetRef;
    int dataSetLen;
    uint32_t timeAllowedToLive;
    uint32_t stNum;
    uint32_t sqNum;
    uint32_t confRev;
    MmsValue* timestamp;
    bool simulation;
    bool ndsCom;
    MmsValue* dataSetValues;
    GooseListener listener;
    void* listenerParameter;
    bool running;
    Thread receiver;
    char* interfaceId;
};

int
parseAllData(uint8_t* buffer, int allDataLength, MmsValue* dataSetValues)
{
    int bufPos = 0;
    int elementLength = 0;

    int elementIndex = 0;

    int maxIndex = MmsValue_getArraySize(dataSetValues) - 1;

    while (bufPos < allDataLength) {
        uint8_t tag = buffer[bufPos++];

        if (elementIndex > maxIndex) {
        	if (DEBUG) printf("Malformed message: too much elements!\n");
        	return 0;
        }

        MmsValue* value = MmsValue_getElement(dataSetValues, elementIndex);

        bufPos = BerDecoder_decodeLength(buffer, &elementLength, bufPos, allDataLength);

        if (bufPos + elementLength > allDataLength) {
            if (DEBUG) printf("Malformed message: sub element is to large!\n");
            return 0;
        }

        switch (tag) {
        case 0x80: /* reserved for access result */
            printf("    found reserved value (tag 0x80)!\n");
            break;
        case 0xa1: /* array */
            if (DEBUG) printf("    found array\n");
            if (MmsValue_getType(value) == MMS_ARRAY) {
            	if (!parseAllData(buffer + bufPos, elementLength, value))
            		return -1;
            }
            break;
        case 0xa2: /* structure */
            if (DEBUG) printf("    found structure\n");
            if (MmsValue_getType(value) == MMS_STRUCTURE) {
				if (!parseAllData(buffer + bufPos, elementLength, value))
					return -1;
			}
            break;
        case 0x83: /* boolean */
            if (DEBUG) printf("    found boolean\n");

            if (MmsValue_getType(value) == MMS_BOOLEAN) {
                MmsValue_setBoolean(value, BerDecoder_decodeBoolean(buffer, bufPos));
            }
            else
                if (DEBUG) printf("      message contains value of wrong type!\n");

            break;

        case 0x84: /* BIT STRING */
        	if (MmsValue_getType(value) == MMS_BIT_STRING) {
        		int padding = buffer[bufPos];
        		int bitStringLength = (8 * (elementLength - 1)) - padding;
        		if (bitStringLength == value->value.bitString.size) {
        			memcpy(value->value.bitString.buf, buffer + bufPos + 1,
        					elementLength - 1);
        		}
        		else
        			printf("bit-string is of wrong size");
        	}
        	break;
        case 0x85: /* integer */
        	if (MmsValue_getType(value) == MMS_INTEGER) {
        		if (elementLength <= value->value.integer->maxSize) {
        			value->value.integer->size = elementLength;
        			memcpy(value->value.integer->octets, buffer + bufPos, elementLength);
        		}
        	}
        	break;
        case 0x86: /* unsigned integer */
        	if (MmsValue_getType(value) == MMS_UNSIGNED) {
				if (elementLength <= value->value.unsignedInteger->maxSize) {
					value->value.integer->size = elementLength;
					memcpy(value->value.unsignedInteger->octets, buffer + bufPos, elementLength);
				}
			}
			break;
        case 0x87: /* Float */
        	if (MmsValue_getType(value) == MMS_FLOAT) {
				if (elementLength == 9) {
					MmsValue_setDouble(value, BerDecoder_decodeDouble(buffer, bufPos));
				}
				else if (elementLength == 5) {
					MmsValue_setFloat(value, BerDecoder_decodeFloat(buffer, bufPos));
				}
        	}
        	break;

        case 0x89: /* octet string */
        	if (MmsValue_getType(value) == MMS_OCTET_STRING) {
        		if (elementLength <= value->value.octetString.maxSize) {
        			value->value.octetString.size = elementLength;
        			memcpy(value->value.octetString.buf, buffer + bufPos, elementLength);
        		}
        	}
        	break;
        case 0x8a: /* visible string */
        	if (MmsValue_getType(value) == MMS_VISIBLE_STRING) {
        		if (value->value.visibleString != NULL) {
        			if (strlen(value->value.visibleString) >= elementLength) {
        				memcpy(value->value.visibleString, buffer + bufPos, elementLength);
						value->value.visibleString[elementLength] = 0;
        			}
        			else {
        				free(value->value.visibleString);
        				value->value.visibleString = (char*) malloc(elementLength + 1);
						memcpy(value->value.visibleString, buffer + bufPos, elementLength);
						value->value.visibleString[elementLength] = 0;
        			}
        		}
        		else {
        			value->value.visibleString = (char*) malloc(elementLength + 1);
        			memcpy(value->value.visibleString, buffer + bufPos, elementLength);
        			value->value.visibleString[elementLength] = 0;
        		}

        	}
        	break;
        case 0x8c: /* binary time */
        	if (MmsValue_getType(value) == MMS_BINARY_TIME) {
        		if ((elementLength == 4) || (elementLength == 6)) {
        			memcpy(value->value.binaryTime.buf, buffer + bufPos, elementLength);
        		}
        	}
        	break;
        case 0x91: /* Utctime */
            if (elementLength == 8) {
                if (MmsValue_getType(value) == MMS_UTC_TIME) {
                    MmsValue_setUtcTimeByBuffer(value, buffer + bufPos);
                }
                else
                    if (DEBUG) printf("      message contains value of wrong type!\n");
            }
            else
                if (DEBUG) printf("      UTCTime element is of wrong size!\n");
            break;
        default:
            printf("    found unkown tag %02x\n", tag);
            break;
        }

        bufPos += elementLength;

        elementIndex++;
    }

    return 1;
}

static int
parseGoosePayload(uint8_t* buffer, int apduLength, GooseSubscriber self)
{
    int bufPos = 0;
    char* gocbRef = NULL;
    char* datSet = NULL;
    uint32_t timeAllowedToLive = 0;
    uint32_t stNum = 0;
    uint32_t sqNum = 0;
    uint32_t confRev;
    bool simulation = false;
    bool ndsCom = false;
    bool isMatching = false;

    uint32_t numberOfDatSetEntries = 0;

    if (buffer[bufPos++] == 0x61) {
        int gooseLength;
        bufPos = BerDecoder_decodeLength(buffer, &gooseLength, bufPos, apduLength);

        int gooseEnd = bufPos + gooseLength;

        while (bufPos < gooseEnd) {
            int elementLength;

            uint8_t tag = buffer[bufPos++];
            bufPos = BerDecoder_decodeLength(buffer, &elementLength, bufPos, apduLength);

            if (bufPos + elementLength > apduLength) {
                if (DEBUG) printf("Malformed message: sub element is to large!\n");
                goto exit_with_fault;
            }

            if (bufPos == -1)
                goto exit_with_fault;

            switch(tag) {
            case 0x80: /* gocbRef */
                gocbRef = BerDecoder_decodeString(buffer, elementLength, bufPos, apduLength);

                if (DEBUG) printf("  Found gocbRef: %s\n", gocbRef);
                free(gocbRef);

                break;

            case 0x81: /* timeAllowedToLive */

                timeAllowedToLive = BerDecoder_decodeUint32(buffer, elementLength, bufPos);

                if (DEBUG) printf("  Found timeAllowedToLive %u\n", timeAllowedToLive);

                break;

            case 0x82:
                if (DEBUG) printf("  Found dataSet\n");

                if (self->dataSetLen == elementLength) {
                    if (memcmp(self->datSetRef, buffer + bufPos, elementLength) == 0) {
                        if (DEBUG) printf("  data set is matching!\n");
                        isMatching = true;
                    }
                    else return 0;
                }
                else
                    return 0;

                break;

            case 0x83:
                if (DEBUG) printf("  Found goId\n");
                break;

            case 0x84:
                MmsValue_setUtcTimeByBuffer(self->timestamp, buffer + bufPos);
                if (DEBUG) printf("  Found timestamp t: %llu\n", MmsValue_getUtcTimeInMs(self->timestamp));
                break;

            case 0x85:
                stNum = BerDecoder_decodeUint32(buffer, elementLength, bufPos);
                if (DEBUG) printf("  Found stNum: %u\n", stNum);
                break;

            case 0x86:
                sqNum = BerDecoder_decodeUint32(buffer, elementLength, bufPos);
                if (DEBUG) printf("  Found sqNum: %u\n", sqNum);
                break;

            case 0x87:
                simulation = BerDecoder_decodeBoolean(buffer, bufPos);
                if (DEBUG) printf("  Found simulation: %i\n", simulation);
                break;

            case 0x88:
                confRev = BerDecoder_decodeUint32(buffer, elementLength, bufPos);
                if (DEBUG) printf("  Found confRev: %u\n", confRev);
                break;

            case 0x89:
                ndsCom = BerDecoder_decodeBoolean(buffer, bufPos);
                if (DEBUG) printf("  Found ndsCom: %i\n", ndsCom);
                break;

            case 0x8a:
                numberOfDatSetEntries = BerDecoder_decodeUint32(buffer, elementLength, bufPos);
                if (DEBUG) printf("  Found number of entries: %u\n", numberOfDatSetEntries);
                break;

            case 0xab:
                if (DEBUG) printf("  Found all data with length: %i\n", elementLength);
                parseAllData(buffer + bufPos, elementLength, self->dataSetValues);
                break;

            default:
                if (DEBUG)  printf("  Unknown tag %02x\n", tag);
                break;
            }

            bufPos += elementLength;
        }

        if (isMatching) {
            self->stNum = stNum;
            self->sqNum = sqNum;
            self->timeAllowedToLive = timeAllowedToLive;
            self->confRev = confRev;
            self->ndsCom = ndsCom;
            self->simulation = simulation;

            return 1;
        }

        return 0;
    }

exit_with_fault:
    if (DEBUG) printf("Invalid goose payload\n");
    return -1;
}

int
parseGooseMessage(uint8_t* buffer, int numbytes, GooseSubscriber subscriber)
{
    int bufPos;

    if (numbytes < 22) return -1;

    // TODO check destination address

    /* skip ethernet addresses */
    bufPos = 12;
    int headerLength = 14;

    /* check for VLAN tag */
    if ((buffer[bufPos] == 0x81) && (buffer[bufPos + 1] == 0x00)) {
        bufPos += 4; /* skip VLAN tag */
        headerLength += 4;
    }

    /* check for GOOSE Ethertype */
    if (buffer[bufPos++] != 0x88)
        return -1;
    if (buffer[bufPos++] != 0xb8)
        return -1;

    uint16_t appId;

    appId = buffer[bufPos++] * 0x100;
    appId += buffer[bufPos++];

    uint16_t length;

    length = buffer[bufPos++] * 0x100;
    length += buffer[bufPos++];

    /* skip reserved fields */
    bufPos += 4;

    int apduLength = length - 8;


    if (numbytes != length + headerLength) {
        if (DEBUG)
        	printf("Invalid PDU size\n");
        return -1;
    }

    if (DEBUG) {
        printf("GOOSE message:\n----------------\n");
        printf("  APPID: %u\n", appId);
        printf("  LENGTH: %u\n", length);
        printf("  APDU length: %i\n", apduLength);
    }

    return parseGoosePayload(buffer + bufPos, apduLength, subscriber);
}

void
gooseSubscriberLoop(void* threadParameter)
{
    GooseSubscriber self = (GooseSubscriber) threadParameter;

    uint8_t* buffer = (uint8_t*) malloc(ETH_BUFFER_LENGTH);

    EthernetSocket socket;

    if (self->interfaceId == NULL)
    	socket = Ethernet_createSocket(CONFIG_ETHERNET_INTERFACE_ID, NULL);
    else
    	socket = Ethernet_createSocket(self->interfaceId, NULL);

    Ethernet_setProtocolFilter(socket, ETH_P_GOOSE);

    int running = 1;

    while (running) {

        int packetSize = Ethernet_receivePacket(socket, buffer, ETH_BUFFER_LENGTH);

        if (packetSize > 0) {
            if (parseGooseMessage(buffer, packetSize, self) == 1) {
                if (self->listener != NULL) {
                    self->listener(self, self->listenerParameter);
                }
            }
        }

        Thread_sleep(1);

        running = self->running;
    }

    free(buffer);

    Ethernet_destroySocket(socket);
}

GooseSubscriber
GooseSubscriber_create(char* datSetRef, MmsValue* dataSetValues)
{
    GooseSubscriber self = (GooseSubscriber) calloc(1, sizeof(struct sGooseSubscriber));

    self->datSetRef = copyString(datSetRef);
    self->dataSetLen = strlen(datSetRef);
    self->timestamp = MmsValue_newUtcTime(0);
    self->dataSetValues = dataSetValues;
    self->running = false;

    return self;
}

void
GooseSubscriber_setInterfaceId(GooseSubscriber self, char* interfaceId)
{
	self->interfaceId = copyString(interfaceId);
}

void
GooseSubscriber_subscribe(GooseSubscriber self)
{
    Thread thread = Thread_create((ThreadExecutionFunction) gooseSubscriberLoop, self, false);
    self->receiver = thread;
    self->running = true;
    Thread_start(thread);
}

void
GooseSubscriber_unsubscribe(GooseSubscriber self)
{
    if (self->running) {
        self->running = false;
        Thread_destroy(self->receiver);
    }
}

void
GooseSubscriber_destroy(GooseSubscriber self)
{
    GooseSubscriber_unsubscribe(self);

    free(self->datSetRef);

    MmsValue_delete(self->timestamp);

    if (self->interfaceId != NULL)
    	free(self->interfaceId);

    free(self);
}

void
GooseSubscriber_setListener(GooseSubscriber self, GooseListener listener, void* parameter)
{
    self->listener = listener;
    self->listenerParameter = parameter;
}

uint32_t
GooseSubscriber_getStNum(GooseSubscriber self)
{
    return self->stNum;
}

uint32_t
GooseSubscriber_getSqNum(GooseSubscriber self)
{
    return self->sqNum;
}

bool
GooseSubscriber_isTest(GooseSubscriber self)
{
    return self->simulation;
}

bool
GooseSubscriber_needsCommission(GooseSubscriber self)
{
    return self->ndsCom;
}

uint32_t
GooseSubscriber_getTimeAllowedToLive(GooseSubscriber self)
{
    return self->timeAllowedToLive;
}

uint64_t
GooseSubscriber_getTimestamp(GooseSubscriber self)
{
    return MmsValue_getUtcTimeInMs(self->timestamp);
}

MmsValue*
GooseSubscriber_getDataSetValues(GooseSubscriber self)
{
    return self->dataSetValues;
}








