/*
 *  iso_client_connection.c
 *
 *  Client side representation of the ISO stack (COTP, session, presentation, ACSE)
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

#include "socket.h"
#include "thread.h"
#include "cotp.h"
#include "iso_session.h"
#include "iso_presentation.h"
#include "iso_client_connection.h"
#include "acse.h"

#define STATE_IDLE 0
#define STATE_ASSOCIATED 1
#define STATE_ERROR 2

#define ISO_CLIENT_BUFFER_SIZE MMS_MAXIMUM_PDU_SIZE + 100

struct sIsoClientConnection {
	IsoIndicationCallback callback;
	void* callbackParameter;
	int state;
	Socket socket;
	CotpConnection* cotpConnection;
	IsoPresentation* presentation;
	IsoSession* session;
	uint8_t* buffer1;
	uint8_t* buffer2;
	uint8_t* cotpBuf;
	ByteBuffer* cotpBuffer;
	ByteBuffer* payloadBuffer;
	Thread thread;
};

static void*
connectionHandlingThread(void* threadParameter)
{
	IsoClientConnection self = (IsoClientConnection) threadParameter;

	IsoSessionIndication sessionIndication;
	IsoPresentationIndication presentationIndication;

	while (CotpConnection_parseIncomingMessage(self->cotpConnection) == DATA_INDICATION) {

		sessionIndication =
				IsoSession_parseMessage(self->session,
						CotpConnection_getPayload(self->cotpConnection));

		if (sessionIndication != SESSION_DATA) {
			if (DEBUG) printf("connectionHandlingThread: Invalid session message\n");
			break;
		}

		presentationIndication =
				IsoPresentation_parseUserData(self->presentation,
						IsoSession_getUserData(self->session));

		if (presentationIndication != PRESENTATION_OK) {
			if (DEBUG) printf("connectionHandlingThread: Invalid presentation message\n");
			break;
		}

		//TODO copy payload buffer to user buffer and lock user buffer

		self->callback(ISO_IND_DATA, self->callbackParameter,
				&(self->presentation->nextPayload));
	}

	self->callback(ISO_IND_CLOSED, self->callbackParameter, NULL);

	return NULL;
}

IsoClientConnection
IsoClientConnection_create(IsoIndicationCallback callback, void* callbackParameter)
{
	IsoClientConnection self = (IsoClientConnection) calloc(1, sizeof(struct sIsoClientConnection));

	self->callback = callback;
	self->callbackParameter = callbackParameter;
	self->state = STATE_IDLE;

	self->buffer1 = (uint8_t*) malloc(ISO_CLIENT_BUFFER_SIZE);
	self->buffer2 = (uint8_t*) malloc(ISO_CLIENT_BUFFER_SIZE);

	self->payloadBuffer = (ByteBuffer*) calloc(1, sizeof(ByteBuffer));

	return self;
}

void
IsoClientConnection_sendMessage(IsoClientConnection self, ByteBuffer* payload)
{
	ByteBuffer message;
	ByteBuffer_wrap(&message, self->buffer1, 0, ISO_CLIENT_BUFFER_SIZE);

	IsoSession_createDataSpdu(self->session, &message);

	IsoPresentation_createUserData(self->presentation, &message, payload);

	CotpIndication indication;

	indication = CotpConnection_sendDataMessage(self->cotpConnection, &message);

	if (indication != OK)
		printf("IsoClientConnection_sendMessage: send message failed!\n");
}

void
IsoClientConnection_close(IsoClientConnection self)
{
	if (self->socket != NULL)
		Socket_destroy(self->socket);

	self->state = STATE_IDLE;
}

void
IsoClientConnection_destroy(IsoClientConnection self)
{
	if (self->state == STATE_ASSOCIATED)
		IsoClientConnection_close(self);

	if (self->thread != NULL)
		Thread_destroy(self->thread);

	if (self->cotpBuf != NULL) free(self->cotpBuf);
	if (self->cotpBuffer != NULL) free(self->cotpBuffer);
	if (self->cotpConnection != NULL) {
		CotpConnection_destroy(self->cotpConnection);
		free(self->cotpConnection);
	}
	if (self->session != NULL) free(self->session);
	if (self->presentation != NULL) free(self->presentation);

	free(self->payloadBuffer);

	free(self->buffer1);
	free(self->buffer2);
	free(self);
}

void
IsoClientConnection_associate(IsoClientConnection self, IsoConnectionParameters* params,
		ByteBuffer* payload)
{
	Socket socket = TcpSocket_create();

	self->socket = socket;

	if (!Socket_connect(socket, params->hostname, params->tcpPort))
		goto returnError;

	self->cotpBuf = (uint8_t*) malloc(ISO_CLIENT_BUFFER_SIZE);
	self->cotpBuffer = (ByteBuffer*) calloc(1, sizeof(ByteBuffer));
	ByteBuffer_wrap(self->cotpBuffer, self->cotpBuf, 0, ISO_CLIENT_BUFFER_SIZE);

	self->cotpConnection = (CotpConnection*) calloc(1, sizeof(CotpConnection));
	CotpConnection_init(self->cotpConnection, socket, self->cotpBuffer);

	/* COTP handshake */
	CotpIndication cotpIndication =
			CotpConnection_sendConnectionRequestMessage(self->cotpConnection);

	cotpIndication = CotpConnection_parseIncomingMessage(self->cotpConnection);

	if (cotpIndication != CONNECT_INDICATION)
		goto returnError;

	/* Upper layers handshake */
	AcseConnection acse;

	ByteBuffer acseBuffer;
	ByteBuffer_wrap(&acseBuffer, self->buffer1, 0, ISO_CLIENT_BUFFER_SIZE);

	AcseConnection_init(&acse);

	if (params != NULL)
		AcseConnection_setAuthenticationParameter(&acse, params->acseAuthParameter);

	AcseConnection_createAssociateRequestMessage(&acse, &acseBuffer, payload);

	ByteBuffer presentationBuffer;
	ByteBuffer_wrap(&presentationBuffer, self->buffer2, 0, ISO_CLIENT_BUFFER_SIZE);

	self->presentation = (IsoPresentation*) calloc(1, sizeof(IsoPresentation));
	IsoPresentation_init(self->presentation);
	IsoPresentation_createConnectPdu(self->presentation, &presentationBuffer, &acseBuffer);

	ByteBuffer sessionBuffer;
	ByteBuffer_wrap(&sessionBuffer, self->buffer1, 0, ISO_CLIENT_BUFFER_SIZE);

	self->session = (IsoSession*) calloc(1, sizeof(IsoSession));
	IsoSession_init(self->session);
	IsoSession_createConnectSpdu(self->session, &sessionBuffer,
			ByteBuffer_getSize(&presentationBuffer));

	ByteBuffer_append(&sessionBuffer, ByteBuffer_getBuffer(&presentationBuffer),
			ByteBuffer_getSize(&presentationBuffer));


	CotpConnection_sendDataMessage(self->cotpConnection, &sessionBuffer);

	cotpIndication = CotpConnection_parseIncomingMessage(self->cotpConnection);

	if (cotpIndication != DATA_INDICATION)
		goto returnError;

	IsoSessionIndication sessionIndication;

	sessionIndication =
			IsoSession_parseMessage(self->session, CotpConnection_getPayload(self->cotpConnection));

	if (sessionIndication != SESSION_CONNECT) {
		if (DEBUG) printf("IsoClientConnection_associate: no session connect indication\n");
		goto returnError;
	}


	IsoPresentationIndication presentationIndication;
	presentationIndication =
			IsoPresentation_parseAcceptMessage(self->presentation, IsoSession_getUserData(self->session));

	if (presentationIndication != PRESENTATION_OK) {
		if (DEBUG) printf("IsoClientConnection_associate: no presentation ok indication\n");
		goto returnError;
	}

	AcseIndication acseIndication;

	acseIndication = AcseConnection_parseMessage(&acse, &self->presentation->nextPayload);

	if (acseIndication != ACSE_ASSOCIATE) {
		if (DEBUG) printf("IsoClientConnection_associate: no ACSE_ASSOCIATE indication\n");
		goto returnError;
	}

	ByteBuffer_wrap(self->payloadBuffer, acse.userDataBuffer, acse.userDataBufferSize, ISO_CLIENT_BUFFER_SIZE);

	self->callback(ISO_IND_ASSOCIATION_SUCCESS, self->callbackParameter, self->payloadBuffer);

	self->state = STATE_ASSOCIATED;

	AcseConnection_destroy(&acse);

	self->thread = Thread_create(connectionHandlingThread, self, false);
	Thread_start(self->thread);

	return;

returnError:
	self->callback(ISO_IND_ASSOCIATION_FAILED, self->callbackParameter, NULL);

	AcseConnection_destroy(&acse);

	self->state = STATE_ERROR;
	return;
}

void
IsoClientConnection_releasePayloadBuffer(IsoClientConnection self, ByteBuffer* buffer)
{
	//TODO implement me
}
