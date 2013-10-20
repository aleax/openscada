/*
 *  iso_session.c
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
#include "stack_config.h"
#include "iso_session.h"

static int
parseAcceptParameters(IsoSession* session, ByteBuffer* message, int startOffset, int parameterLength)
{
	uint8_t pi;
	uint8_t param_len;
	uint8_t param_val;
	uint8_t hasProtocolOptions = 0;
	uint8_t hasProtocolVersion = 0;
	int offset = startOffset;
	int maxOffset = offset + parameterLength;

	while (offset < maxOffset) {
		pi = message->buffer[offset++];
		param_len = message->buffer[offset++];

		switch (pi) {
		case 19: /* Protocol options */
			if (param_len != 1)
				return -1;
			session->protocolOptions = message->buffer[offset++];
			if (DEBUG) printf("iso_session: Param - Protocol Options: %02x\n", session->protocolOptions);
			hasProtocolOptions = 1;
			break;
		case 21: /* TSDU Maximum Size */
			if (DEBUG) printf("iso_session: Param - TODO TSDU Maximum Size\n", param_val);
			offset += 4;
			break;
		case 22: /* Version Number */
			param_val = message->buffer[offset++];
			if (DEBUG) printf("iso_session: Param - Version number: %02x\n", param_val);
			if (param_val != 2)
				return -1;
			hasProtocolVersion = 1;
			break;
		case 23: /* Initial Serial Number */
			if (DEBUG) printf("iso_session: Param - TODO Initial Serial Number\n");
			offset += param_len;
			break;
		case 26: /* Token Setting Item */
			param_val = message->buffer[offset++];
			if (DEBUG) printf("iso_session: Param - Token Setting Item: %02x\n", param_val);
			break;
		case 55: /* Second Initial Serial Number */
			if (DEBUG) printf("iso_session: Param - TODO Second Initial Serial Number\n");
			offset += param_len;
			break;
		case 56: /* Upper Limit Serial Number */
			if (DEBUG) printf("iso_session: Param - TODO Upper Limit Serial Number\n");
			offset += param_len;
			break;
		case 57: /* Large Initial Serial Number */
			if (DEBUG) printf("iso_session: Param - TODO Large Initial Serial Number\n");
			offset += param_len;
			break;
		case 58: /* Large Second Initial Serial Number */
			if (DEBUG) printf("iso_session: Param - TODO Large Second Initial Serial Number\n");
			offset += param_len;
			break;
		default:
			if (DEBUG) printf("iso_session: Param - Invalid Parameter with ID %02x\n", pi);
			break;
		}
	}

	if (hasProtocolOptions && hasProtocolVersion)
		return offset - startOffset;
	else
		return -1;
}

static IsoSessionIndication
parseSessionHeaderParameters(IsoSession* session, ByteBuffer* message, int parametersOctets)
{
	int offset = 2;
	uint8_t pgi;
	uint8_t parameterLength;
	int hasCallingSessionSelector = 0;
	int hasCalledSessionSelector = 0;
	int hasConnectAcceptItem = 0;
	int hasSessionRequirement = 0;

	while (offset < (parametersOctets + 2)) {
		pgi = message->buffer[offset++];
		parameterLength = message->buffer[offset++];

		switch (pgi) {
		case 1:		/* Connection Identifier */
			if (DEBUG) printf("iso_session: PGI - connection identifier\n");
			printf("TODO: PGI not implemented!");
			offset += parameterLength;
			break;
		case 5:		/* Connection/Accept Item */
			if (DEBUG) printf("iso_session: PGI - Connection/Accept Item\n");
			int connectAcceptLen;

			connectAcceptLen = parseAcceptParameters(session, message, offset, parameterLength);

			if (connectAcceptLen == -1)
				return SESSION_ERROR;

			offset += connectAcceptLen;
			hasConnectAcceptItem = 1;
			break;
		case 20: /* Session User Requirements */
			if (DEBUG) printf("iso_session: Parameter - Session User Req\n");
			if (parameterLength != 2)
				return SESSION_ERROR;
			hasSessionRequirement = 1;
			session->sessionRequirement = message->buffer[offset++] * 0x100;
			session->sessionRequirement += message->buffer[offset++];
			break;
		case 51: /* Calling Session Selector */
			if (DEBUG) printf("iso_session: Parameter - Calling Session Selector\n");
			if (parameterLength != 2)
				return SESSION_ERROR;
			hasCallingSessionSelector = 1;
			session->callingSessionSelector = message->buffer[offset++] * 0x100;
			session->callingSessionSelector += message->buffer[offset++];
			break;
		case 52: /* Called Session Selector */
			if (DEBUG) printf("iso_session: Parameter - Called Session Selector\n");
			if (parameterLength != 2)
				return SESSION_ERROR;
			hasCalledSessionSelector = 1;
			session->calledSessionSelector = message->buffer[offset++] * 0x100;
			session->calledSessionSelector += message->buffer[offset++];
			break;
		case 60: /* Data Overflow */
			if (DEBUG) printf("iso_session: Parameter - Data Overflow\n");
			printf("TODO: parameter not implemented!");
			offset += parameterLength;
			break;
		case 193:	/* User Data */
			if (DEBUG) printf("iso_session: PGI - user data\n");
			/* here we should return - the remaining data is for upper layers ! */

			if (hasConnectAcceptItem && hasSessionRequirement)
			{
				ByteBuffer_wrap(&session->userData, message->buffer + offset,
						message->size - offset, message->maxSize - offset);

				return SESSION_OK;
			}
			else
				return SESSION_ERROR;
		case 194:	/* Extended User Data */
			if (DEBUG) printf("iso_session: PGI - extended user data\n");
			break;
		default:
			if (DEBUG) printf("iso_session: invalid parameter/PGI\n");
			break;
		}
	}

	return SESSION_ERROR;
}

void
IsoSession_createDataSpdu(IsoSession* session, ByteBuffer* buffer)
{
	int offset = buffer->size;
	uint8_t* buf = buffer->buffer;

	buf[offset++] = 0x01; /* GiveToken SPDU */
	buf[offset++] = 0x00;
	buf[offset++] = 0x01; /* Data SPDU */
	buf[offset++] = 0x00;

	buffer->size = offset;
}


static int
encodeConnectAcceptItem(uint8_t* buf, int offset, uint8_t options)
{
	buf[offset++] = 5;
	buf[offset++] = 6;
	buf[offset++] = 0x13; /* Protocol Options */
	buf[offset++] = 1;
	buf[offset++] = options;
	buf[offset++] = 0x16; /* Version Number */
	buf[offset++] = 1;
	buf[offset++] = 2;    /* Version = 2 */

	return offset;
}

static int
encodeSessionRequirement(IsoSession* self, uint8_t* buf, int offset)
{
	buf[offset++] = 0x14;
	buf[offset++] = 2;
	buf[offset++] = (uint8_t) (self->sessionRequirement / 0x100);
	buf[offset++] = (uint8_t) (self->sessionRequirement & 0x00ff);

	return offset;
}

static int
encodeCallingSessionSelector(IsoSession* self, uint8_t* buf, int offset)
{
	buf[offset++] = 0x33;
	buf[offset++] = 2;
	buf[offset++] = (uint8_t) (self->callingSessionSelector / 0x100);
	buf[offset++] = (uint8_t) (self->callingSessionSelector & 0x00ff);

	return offset;
}

static int
encodeCalledSessionSelector(IsoSession* self, uint8_t* buf, int offset)
{
	buf[offset++] = 0x34;
	buf[offset++] = 2;
	buf[offset++] = (uint8_t) (self->calledSessionSelector / 0x100);
	buf[offset++] = (uint8_t) (self->calledSessionSelector & 0x00ff);

	return offset;
}

static int
encodeSessionUserData(IsoSession* self, uint8_t* buf, int offset, uint8_t payloadLength)
{
	buf[offset++] = 0xc1;
	buf[offset++] = payloadLength;

	return offset;
}

void
IsoSession_createConnectSpdu(IsoSession* self, ByteBuffer* buffer, uint8_t payloadLength)
{
	int offset = buffer->size;
	uint8_t* buf = buffer->buffer;
	int lengthOffset;

	buf[offset++] = 13; /* CONNECT SPDU */
	lengthOffset = offset; offset++; /* Skip byte for length - fill it later */

	offset = encodeConnectAcceptItem(buf, offset, 0);

	offset = encodeSessionRequirement(self, buf, offset);

	offset = encodeCallingSessionSelector(self, buf, offset);

	offset = encodeCalledSessionSelector(self, buf, offset);

	offset = encodeSessionUserData(self, buf, offset, payloadLength);

	int spduLength = (offset - lengthOffset - 1) + payloadLength;

	buf[lengthOffset] = spduLength;

	buffer->size = offset;
}

void
IsoSession_createAcceptSpdu(IsoSession* self, ByteBuffer* buffer, uint8_t payloadLength)
{
	int offset = buffer->size;
	uint8_t* buf = buffer->buffer;
	int lengthOffset;

	buf[offset++] = 14; /* ACCEPT SPDU */
	lengthOffset = offset; offset++;

	offset = encodeConnectAcceptItem(buf, offset, self->protocolOptions);

	offset = encodeSessionRequirement(self, buf, offset);

	offset = encodeCalledSessionSelector(self, buf, offset);

	offset = encodeSessionUserData(self, buf, offset, payloadLength);

	int spduLength = (offset - lengthOffset - 1) + payloadLength;

	buf[lengthOffset] = spduLength;

	buffer->size = offset;
}

void
IsoSession_init(IsoSession* session)
{
	memset(session, 0, sizeof(IsoSession));
	session->sessionRequirement = 0x0002; /* default = duplex functional unit */
	session->callingSessionSelector = 0x0001;
	session->calledSessionSelector = 0x0001;
}

ByteBuffer*
IsoSession_getUserData(IsoSession* session)
{
	return &session->userData;
}

IsoSessionIndication
IsoSession_parseMessage(IsoSession* session, ByteBuffer* message)
{
	uint8_t* buffer = message->buffer;
	uint8_t id;
	uint8_t length;

	if (message->size > 1) {
		id = buffer[0];
		length = buffer[1];
	}
	else
		return SESSION_ERROR;

	switch (id) {
	case 13: /* CONNECT(CN) SPDU */
		if (length != (message->size - 2))
			return SESSION_ERROR;
		if (parseSessionHeaderParameters(session, message, length) == SESSION_OK)
			return SESSION_CONNECT;
		else {
			printf("iso_session.c: error parsing connect spdu\n");
			return SESSION_ERROR;
		}
		break;
	case 14: /* ACCEPT SPDU */
		if (length != (message->size - 2))
			return SESSION_ERROR;
		if (parseSessionHeaderParameters(session, message, length) == SESSION_OK)
			return SESSION_CONNECT;
		else {
			printf("iso_session.c: error parsing accept spdu\n");
			return SESSION_ERROR;
		}

		break;
	case 1: /* Give token / data SPDU */
		if (message->size < 4)
			return SESSION_ERROR;
		if ((length == 0) && (buffer[2] == 1) && (buffer[3] == 0)) {
			ByteBuffer_wrap(&session->userData, message->buffer + 4, message->size - 4, message->maxSize - 4);

			return SESSION_DATA;
		}
		return SESSION_ERROR;
	}

	return SESSION_ERROR;
}

