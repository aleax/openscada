/*
 *  cotp.c
 *
 *  ISO 8073 Connection Oriented Transport Protocol over TCP (RFC1006)
 *
 *  Partial implementation of the ISO 8073 COTP protocol for MMS.
 *
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

#include "stack_config.h"
#include "cotp.h"
#include "byte_stream.h"
#include "byte_buffer.h"
#include "buffer_chain.h"

#define COTP_RFC1006_HEADER_SIZE 4

#define COTP_DATA_HEADER_SIZE 3

#ifdef CONFIG_COTP_MAX_TPDU_SIZE
#define COTP_MAX_TPDU_SIZE CONFIG_COTP_MAX_TPDU_SIZE
#else
#define COTP_MAX_TPDU_SIZE 8192
#endif

#ifndef DEBUG_COTP
#define DEBUG_COTP 0
#endif

static int
addPayloadToBuffer(CotpConnection* self, int rfc1006Length);

static CotpIndication
writeOptions(CotpConnection* self)
{
    if (self->options.tpdu_size != -1) {

        if (DEBUG_COTP)
            printf("COTP: send TPDU size: %i\n", CotpConnection_getTpduSize(self));

        if (ByteStream_writeUint8(self->stream, 0xc0) == -1)
            return ERROR;
        if (ByteStream_writeUint8(self->stream, 1) == -1)
            return ERROR;
        if (ByteStream_writeUint8(self->stream, self->options.tpdu_size) == -1)
            return ERROR;
    }

    if (self->options.tsap_id_dst != -1) {
        if (ByteStream_writeUint8(self->stream, 0xc2) == -1)
            return ERROR;
        if (ByteStream_writeUint8(self->stream, 2) == -1)
            return ERROR;
        if (ByteStream_writeUint8(self->stream, (uint8_t) (self->options.tsap_id_dst / 0x100)) == -1)
            return ERROR;
        if (ByteStream_writeUint8(self->stream, (uint8_t) (self->options.tsap_id_dst & 0xff)) == -1)
            return ERROR;
    }

    if (self->options.tsap_id_src != -1) {
        if (ByteStream_writeUint8(self->stream, 0xc1) == -1)
            return ERROR;
        if (ByteStream_writeUint8(self->stream, 2) == -1)
            return ERROR;
        if (ByteStream_writeUint8(self->stream, (uint8_t) (self->options.tsap_id_src / 0x100)) == -1)
            return ERROR;
        if (ByteStream_writeUint8(self->stream, (uint8_t) (self->options.tsap_id_src & 0xff)) == -1)
            return ERROR;
    }

    return OK;
}

static int
getOptionsLength(CotpConnection* self)
{
    int optionsLength = 0;
    if (self->options.tpdu_size != -1)
        optionsLength += 3;
    if (self->options.tsap_id_dst != -1)
        optionsLength += 4;
    if (self->options.tsap_id_src != -1)
        optionsLength += 4;
    return optionsLength;
}

static inline int
getConnectionResponseLength(CotpConnection* self)
{
    return 11 + getOptionsLength(self);
}

static inline CotpIndication
writeStaticConnectResponseHeader(CotpConnection* self)
{
    if (ByteStream_writeUint8(self->stream, 6 + getOptionsLength(self)) != 1)
        return ERROR;
    if (ByteStream_writeUint8(self->stream, 0xd0) != 1)
        return ERROR;
    if (ByteStream_writeUint8(self->stream, self->srcRef / 0x100) != 1)
        return ERROR;
    if (ByteStream_writeUint8(self->stream, self->srcRef & 0xff) != 1)
        return ERROR;
    if (ByteStream_writeUint8(self->stream, self->srcRef / 0x100) != 1)
        return ERROR;
    if (ByteStream_writeUint8(self->stream, self->srcRef & 0xff) != 1)
        return ERROR;
    if (ByteStream_writeUint8(self->stream, self->protocolClass) != 1)
        return ERROR;

    return OK;
}

static CotpIndication
writeRfc1006Header(CotpConnection* self, int len)
{
    if (ByteStream_writeUint8(self->stream, (uint8_t) 0x03) != 1)
        return ERROR;
    if (ByteStream_writeUint8(self->stream, (uint8_t) 0x00) != 1)
        return ERROR;
    if (ByteStream_writeUint8(self->stream, (uint8_t) (len / 0x100)) != 1)
        return ERROR;
    if (ByteStream_writeUint8(self->stream, (uint8_t) (len & 0xff)) != 1)
        return ERROR;

    return OK;
}

static CotpIndication
writeDataTpduHeader(CotpConnection* self, int isLastUnit)
{
    if (ByteStream_writeUint8(self->stream, (uint8_t) 0x02) != 1)
        return ERROR;

    if (ByteStream_writeUint8(self->stream, (uint8_t) 0xf0) != 1)
        return ERROR;

    if (isLastUnit) {
        if (ByteStream_writeUint8(self->stream, (uint8_t) 0x80) != 1)
            return ERROR;
    }
    else {
        if (ByteStream_writeUint8(self->stream, (uint8_t) 0x00) != 1)
            return ERROR;
    }

    return OK;
}

CotpIndication
CotpConnection_sendDataMessage(CotpConnection* self, ByteBuffer* payload)
{
    int fragments = 1;

    int fragmentPayloadSize = CotpConnection_getTpduSize(self) - COTP_DATA_HEADER_SIZE;

    if (payload->size > fragmentPayloadSize) { /* Is segmentation required? */
        fragments = payload->size / fragmentPayloadSize;

        if ((payload->size % fragmentPayloadSize) != 0)
            fragments += 1;
    }

    int currentBufPos = 0;
    int currentLimit;
    int lastUnit;

    while (fragments > 0) {
        if (fragments > 1) {
            currentLimit = currentBufPos + fragmentPayloadSize;
            lastUnit = 0;
        }
        else {
            currentLimit = payload->size;
            lastUnit = 1;
        }

        if (writeRfc1006Header(self, 7 + (currentLimit - currentBufPos)) == ERROR)
            return ERROR;

        if (writeDataTpduHeader(self, lastUnit) == ERROR)
            return ERROR;

        int i;
        for (i = currentBufPos; i < currentLimit; i++) {
            if (ByteStream_writeUint8(self->stream, payload->buffer[i]) != 1)
                return ERROR;
            currentBufPos++;
        }

        if (DEBUG_COTP)
            printf("COTP: Send COTP fragment %i bufpos: %i\n", fragments, currentBufPos);
        ByteStream_sendBuffer(self->stream);

        fragments--;
    }

    return OK;
}

CotpIndication
CotpConnection_sendDataMessageBC(CotpConnection* self, BufferChain payload)
{
    int fragments = 1;

    int fragmentPayloadSize = CotpConnection_getTpduSize(self) - COTP_DATA_HEADER_SIZE;

    if (payload->length > fragmentPayloadSize) { /* Is segmentation required? */
        fragments = payload->length / fragmentPayloadSize;

        if ((payload->length % fragmentPayloadSize) != 0)
            fragments += 1;
    }

    int currentBufPos = 0;
    int currentLimit;
    int lastUnit;

    BufferChain currentChain = payload;
    int currentChainIndex = 0;

    if (DEBUG_COTP)
        printf("\nCOTP: nextBufferPart: len:%i partLen:%i\n", currentChain->length, currentChain->partLength);


    while (fragments > 0) {
        if (fragments > 1) {
            currentLimit = currentBufPos + fragmentPayloadSize;
            lastUnit = 0;
        }
        else {
            currentLimit = payload->length;
            lastUnit = 1;
        }

        if (writeRfc1006Header(self, 7 + (currentLimit - currentBufPos)) == ERROR)
            return ERROR;

        if (writeDataTpduHeader(self, lastUnit) == ERROR)
            return ERROR;

        int i;
        for (i = currentBufPos; i < currentLimit; i++) {

            if (currentChainIndex >= currentChain->partLength) {
                currentChain = currentChain->nextPart;
                if (DEBUG_COTP)
                    printf("\nCOTP: nextBufferPart: len:%i partLen:%i\n", currentChain->length, currentChain->partLength);
                currentChainIndex = 0;
            }

            if (DEBUG_COTP)
                printf("%02x ", currentChain->buffer[currentChainIndex]);

            if (ByteStream_writeUint8(self->stream, currentChain->buffer[currentChainIndex]) != 1)
                return ERROR;

            currentChainIndex++;

            currentBufPos++;
        }

        if (DEBUG_COTP)
            printf("COTP: Send COTP fragment %i bufpos: %i\n", fragments, currentBufPos);

        if (ByteStream_sendBuffer(self->stream) == -1)
            return ERROR;

        fragments--;
    }

    return OK;
}

static void
allocateWriteBuffer(CotpConnection* self)
{
    if (self->writeBuffer == NULL ) {
        self->writeBuffer = ByteBuffer_create(NULL,
                CotpConnection_getTpduSize(self) + COTP_RFC1006_HEADER_SIZE);
        ByteStream_setWriteBuffer(self->stream, self->writeBuffer);
    }
}

/* client side */
CotpIndication
CotpConnection_sendConnectionRequestMessage(CotpConnection* self)
{
    allocateWriteBuffer(self);

    int len = 22;

    if (writeRfc1006Header(self, len) == ERROR)
        return ERROR;

    /* LI */
    if (ByteStream_writeUint8(self->stream, 17) != 1)
        return ERROR;

    /* TPDU CODE */
    if (ByteStream_writeUint8(self->stream, 0xe0) != 1)
        return ERROR;

    /* DST REF */
    if (ByteStream_writeUint8(self->stream, 0x00) != 1)
        return ERROR;
    if (ByteStream_writeUint8(self->stream, 0x00) != 1)
        return ERROR;

    /* SRC REF */
    if (ByteStream_writeUint8(self->stream, 0x00) != 1)
        return ERROR;
    //if (ByteStream_writeUint8(self->stream, 0x01) != 1)
    if (ByteStream_writeUint8(self->stream, 0x02) != 1)
        return ERROR;

    /* Class */
    if (ByteStream_writeUint8(self->stream, 0x00) != 1)
        return ERROR;

    self->options.tsap_id_dst = 1;
    self->options.tsap_id_src = 0;

    CotpIndication indication = writeOptions(self);

    ByteStream_sendBuffer(self->stream);

    return indication;
}

CotpIndication
CotpConnection_sendConnectionResponseMessage(CotpConnection* self)
{
    allocateWriteBuffer(self);

    int len = getConnectionResponseLength(self);

    if (writeRfc1006Header(self, len) == ERROR)
        return ERROR;

    if (writeStaticConnectResponseHeader(self) != OK)
        return ERROR;

    if (writeOptions(self) != OK)
        return ERROR;

    if (ByteStream_sendBuffer(self->stream) == -1) {
        if (DEBUG_COTP)
            printf("COTP: Error sending buffer\n");
        return ERROR;
    }

    return OK;
}

static int
parseOptions(CotpConnection* self, int opt_len)
{
    int read_bytes = 0;
    uint8_t option_type, option_len, uint8_value;
    uint16_t uint16_value;
    int i;

    while (read_bytes < opt_len) {
        if (ByteStream_readUint8(self->stream, &option_type) == -1)
            goto cpo_error;
        if (ByteStream_readUint8(self->stream, &option_len) == -1)
            goto cpo_error;

        read_bytes += 2;

        if (DEBUG_COTP)
            printf("COTP: option: %02x len: %02x\n", option_type, option_len);

        switch (option_type) {
        case 0xc0:
			{
				if (ByteStream_readUint8(self->stream, &uint8_value) == -1)
					goto cpo_error;
				read_bytes++;

				int requestedTpduSize = (1 << uint8_value);

				if (DEBUG_COTP)
				    printf("COTP: requested TPDU size: %i\n", requestedTpduSize);

				CotpConnection_setTpduSize(self, requestedTpduSize);
			}
            break;
        case 0xc1:
            if (option_len == 2) {
                if (ByteStream_readUint16(self->stream, &uint16_value) == -1)
                    goto cpo_error;
                read_bytes += 2;
                self->options.tsap_id_src = (int32_t) uint16_value;
            } else
                goto cpo_error;
            break;
        case 0xc2:
            if (option_len == 2) {
                if (ByteStream_readUint16(self->stream, &uint16_value) == -1)
                    goto cpo_error;
                self->options.tsap_id_dst = (int32_t) uint16_value;
                read_bytes += 2;
            } else
                goto cpo_error;
            break;
        default:

            if (DEBUG_COTP)
                printf("COTP: Unknown option %02x\n", option_type);

            for (i = 0; i < opt_len; i++) {
                if (ByteStream_readUint8(self->stream, &uint8_value) == -1)
                    goto cpo_error;
            }

            read_bytes += opt_len;

            break;
        }
    }

    return 1;

    cpo_error:
    if (DEBUG_COTP)
        printf("COTP: cotp_parse_options: error parsing options!\n");
    return -1;
}

void
CotpConnection_init(CotpConnection* self, Socket socket,
        ByteBuffer* payloadBuffer)
{
    self->state = 0;
    self->socket = socket;
    self->srcRef = -1;
    self->dstRef = -1;
    self->protocolClass = -1;
	self->options.tpdu_size = 0;
	self->options.tsap_id_src = -1;
	self->options.tsap_id_dst = -1;
    self->payload = payloadBuffer;

    /* default TPDU size is maximum size */
    CotpConnection_setTpduSize(self, COTP_MAX_TPDU_SIZE);

    self->writeBuffer = NULL;

    self->stream = ByteStream_create(self->socket, NULL);
}

void
CotpConnection_destroy(CotpConnection* self)
{
    if (self->writeBuffer != NULL)
        ByteBuffer_destroy(self->writeBuffer);

    ByteStream_destroy(self->stream);
}

int inline /* in byte */
CotpConnection_getTpduSize(CotpConnection* self)
{
    return (1 << self->options.tpdu_size);
}

void
CotpConnection_setTpduSize(CotpConnection* self, int tpduSize /* in byte */)
{
    int newTpduSize = 1;

    if (tpduSize > COTP_MAX_TPDU_SIZE)
        tpduSize = COTP_MAX_TPDU_SIZE;

    while ((1 << newTpduSize) < tpduSize)
        newTpduSize++;

    if ((1 << newTpduSize) > tpduSize)
        newTpduSize--;

    self->options.tpdu_size = newTpduSize;
}

ByteBuffer*
CotpConnection_getPayload(CotpConnection* self)
{
    return self->payload;
}

int CotpConnection_getSrcRef(CotpConnection* self)
{
    return self->srcRef;
}

int CotpConnection_getDstRef(CotpConnection* self)
{
    return self->dstRef;
}

/*
 CR TPDU (from RFC 0905):

 1    2        3        4       5   6    7    8    p  p+1...end
 +--+------+---------+---------+---+---+------+-------+---------+
 |LI|CR CDT|     DST - REF     |SRC-REF|CLASS |VARIAB.|USER     |
 |  |1110  |0000 0000|0000 0000|   |   |OPTION|PART   |DATA     |
 +--+------+---------+---------+---+---+------+-------+---------+
 */

static int
parseConnectRequestTpdu(CotpConnection* self, uint8_t len)
{
    uint16_t dstRef;
    uint16_t srcRef;
    uint8_t protocolClass;

    if (ByteStream_readUint16(self->stream, &dstRef) != 2)
        return -1;
    else
        self->dstRef = dstRef;

    if (ByteStream_readUint16(self->stream, &srcRef) != 2)
        return -1;
    else
        self->srcRef = srcRef;

    if (ByteStream_readUint8(self->stream, &protocolClass) != 1)
        return -1;
    else
        self->protocolClass = protocolClass;

    return parseOptions(self, len - 6);
}

static int
parseConnectConfirmTpdu(CotpConnection* self, uint8_t len)
{
    uint16_t dstRef;
    uint16_t srcRef;
    uint8_t protocolClass;

    if (ByteStream_readUint16(self->stream, &dstRef) != 2)
        return -1;
    else
        self->srcRef = dstRef;

    if (ByteStream_readUint16(self->stream, &srcRef) != 2)
        return -1;
    else
        self->dstRef = srcRef;

    if (ByteStream_readUint8(self->stream, &protocolClass) != 1)
        return -1;
    else
        self->protocolClass = protocolClass;

    return parseOptions(self, len - 6);
}

static int
parseDataTpdu(CotpConnection* self, uint8_t len)
{
    uint8_t flowControl;

    if (len != 2)
        return -1;

    if (ByteStream_readUint8(self->stream, &flowControl) != 1)
        return -1;
    else {
        if (flowControl & 0x80)
            self->isLastDataUnit = true;
        else
            self->isLastDataUnit = false;
    }

    return 1;
}

static CotpIndication
parseRFC1006Header(CotpConnection* self, uint16_t* rfc1006_length)
{
    uint8_t b;
    if (ByteStream_readUint8(self->stream, &b) == -1)
        return ERROR;
    if (b != 3)
        return ERROR;

    if (ByteStream_readUint8(self->stream, &b) == -1)
        return ERROR;
    if (b != 0) {
        return ERROR;
    }

    if (ByteStream_readUint16(self->stream, rfc1006_length) == -1)
        return ERROR;

    return OK;
}


static CotpIndication parseIncomingMessage(CotpConnection* self);

static int
addPayloadToBuffer(CotpConnection* self, int rfc1006Length)
{
    int payloadLength = rfc1006Length - 7;

    if ((self->payload->size + payloadLength) > self->payload->maxSize)
        return 0;

    int readLength = ByteStream_readOctets(self->stream,
            self->payload->buffer + self->payload->size,
            payloadLength);

    if (readLength != payloadLength) {
        if (DEBUG_COTP)
            printf("COTP: read %i bytes should have been %i\n", readLength, payloadLength);
        return 0;
    }
    else {
        self->payload->size += payloadLength;

        if (self->isLastDataUnit == false) {
            if (parseIncomingMessage(self) == DATA_INDICATION)
                return 1;
            else
                return 0;
        }
        else
            return 1;
    }
}

static CotpIndication
parseIncomingMessage(CotpConnection* self)
{
    uint8_t len;
    uint8_t tpduType;
    uint16_t rfc1006Length;

    if (parseRFC1006Header(self, &rfc1006Length) == ERROR)
        return ERROR;

    if (ByteStream_readUint8(self->stream, &len) != 1) {
        return ERROR;
    }

    if (ByteStream_readUint8(self->stream, &tpduType) == 1) {
        switch (tpduType) {
        case 0xe0:
            if (parseConnectRequestTpdu(self, len) == 1)
                return CONNECT_INDICATION;
            else
                return ERROR;
        case 0xd0:
            self->isLastDataUnit = true;
            if (parseConnectConfirmTpdu(self, len) == 1)
                return CONNECT_INDICATION;
            else
                return ERROR;
        case 0xf0:
            if (parseDataTpdu(self, len) == 1) {
                if (addPayloadToBuffer(self, rfc1006Length) == 1)
                    return DATA_INDICATION;
                else
                    return ERROR;
            }
            else
                return ERROR;
        default:
            return ERROR;
        }
    }
    else
        return ERROR;
}

CotpIndication
CotpConnection_parseIncomingMessage(CotpConnection* self)
{
    self->payload->size = 0;

    return parseIncomingMessage(self);
}
