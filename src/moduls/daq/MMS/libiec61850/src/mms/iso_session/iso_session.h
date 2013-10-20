/*
 *  ise_session.h
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

#ifndef ISE_SESSION_H_
#define ISE_SESSION_H_

#include "byte_buffer.h"

typedef struct {
	uint16_t callingSessionSelector;
	uint16_t calledSessionSelector;
	uint16_t sessionRequirement;
	uint8_t protocolOptions;
	ByteBuffer userData;
} IsoSession;

typedef enum {
	SESSION_OK,
	SESSION_ERROR,
	SESSION_CONNECT,
	SESSION_GIVE_TOKEN,
	SESSION_DATA
} IsoSessionIndication;

void
IsoSession_init(IsoSession* session);

ByteBuffer*
IsoSession_getUserData(IsoSession* session);

void
IsoSession_createDataSpdu(IsoSession* session, ByteBuffer* buffer);

void
IsoSession_createConnectSpdu(IsoSession* self, ByteBuffer* buffer, uint8_t payloadLength);

void
IsoSession_createAcceptSpdu(IsoSession* session, ByteBuffer* buffer, uint8_t payloadLength);

IsoSessionIndication
IsoSession_parseMessage(IsoSession* session, ByteBuffer* message);

#endif /* ISE_SESSION_H_ */
