/*
 *  iso_server.h
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

#ifndef ISO_SERVER_H_
#define ISO_SERVER_H_

#include "socket.h"
#include "byte_buffer.h"
#include "acse.h"

/** \addtogroup mms_server_api_group
 *  @{
 */

typedef enum {
	ISO_SVR_STATE_IDLE,
	ISO_SVR_STATE_RUNNING,
	ISO_SVR_STATE_STOPPED,
	ISO_SVR_STATE_ERROR
} IsoServerState;

typedef struct sIsoServer* IsoServer;

typedef enum {
	ISO_CONNECTION_OPENED,
	ISO_CONNECTION_CLOSED
} IsoConnectionIndication;

typedef struct sIsoConnection* IsoConnection;


typedef struct sIsoServerCallbacks {
	void (*clientConnected) (IsoConnection connection);
} IsoServerCallbacks;

typedef void (*ConnectionIndicationHandler) (IsoConnectionIndication indication,
		void* parameter, IsoConnection connection);

typedef void (*MessageReceivedHandler) (void* parameter, ByteBuffer* message, ByteBuffer* response);

char*
IsoConnection_getPeerAddress(IsoConnection self);

IsoConnection
IsoConnection_create(Socket socket, IsoServer isoServer);

void
IsoConnection_close(IsoConnection self);

void
IsoConnection_installListener(IsoConnection self, MessageReceivedHandler handler,
		void* parameter);

/**
 * \brief send a message over an ISO connection
 *
 * \param handlerMode specifies if this function is used in the context of the connection handling thread
 *        (handlerMode)
 */
void
IsoConnection_sendMessage(IsoConnection self, ByteBuffer* message, bool handlerMode);

IsoServer
IsoServer_create();

void
IsoServer_setTcpPort(IsoServer self, int port);

void
IsoServer_setLocalIpAddress(IsoServer self, char* ipAddress);

IsoServerState
IsoServer_getState(IsoServer self);

void
IsoServer_setConnectionHandler(IsoServer self, ConnectionIndicationHandler handler,
		void* parameter);

void
IsoServer_setAuthenticationParameter(IsoServer self, AcseAuthenticationParameter authParameter);

AcseAuthenticationParameter
IsoServer_getAuthenticationParameter(IsoServer self);

void
IsoServer_startListening(IsoServer self);

void
IsoServer_stopListening(IsoServer self);

void
IsoServer_closeConnection(IsoServer self, IsoConnection isoConnection);

void
IsoServer_destroy(IsoServer self);

/**@}*/

#endif /* ISO_SERVER_H_ */
