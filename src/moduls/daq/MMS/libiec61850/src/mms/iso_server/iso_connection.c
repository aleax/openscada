/*
 *  iso_connection.c
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
#include "byte_stream.h"
#include "cotp.h"
#include "iso_session.h"
#include "iso_presentation.h"
#include "acse.h"
#include "iso_server.h"
#include "socket.h"
#include "thread.h"

#define RECEIVE_BUF_SIZE MMS_MAXIMUM_PDU_SIZE + 100
#define SEND_BUF_SIZE MMS_MAXIMUM_PDU_SIZE + 100

#define ISO_CON_STATE_RUNNING 1
#define ISO_CON_STATE_STOPPED 0

struct sIsoConnection {
    uint8_t* receive_buf;
    uint8_t* send_buf_1;
    uint8_t* send_buf_2;
    MessageReceivedHandler msgRcvdHandler;
    IsoServer isoServer;
    void* msgRcvdHandlerParameter;
    Socket socket;
    int state;
    IsoSession* session;
    IsoPresentation* presentation;
    CotpConnection* cotpConnection;
    char* clientAddress;
    Thread thread;
    Semaphore conMutex;
};


static void
handleTcpConnection(IsoConnection self)
{
    CotpIndication cotpIndication;

    IsoSessionIndication sIndication;

    IsoPresentationIndication pIndication;

    AcseIndication aIndication;
    AcseConnection acseConnection;

    ByteBuffer receiveBuffer;

    ByteBuffer responseBuffer1;
    ByteBuffer responseBuffer2;

    self->cotpConnection = (CotpConnection*) calloc(1, sizeof(CotpConnection));
    CotpConnection_init(self->cotpConnection, self->socket, &receiveBuffer);

    self->session = (IsoSession*) calloc(1, sizeof(IsoSession));
    IsoSession_init(self->session);

    self->presentation = (IsoPresentation*) calloc(1, sizeof(IsoPresentation));
    IsoPresentation_init(self->presentation);

    AcseConnection_init(&acseConnection);
    AcseConnection_setAuthenticationParameter(&acseConnection,
            IsoServer_getAuthenticationParameter(self->isoServer));

    while (self->msgRcvdHandlerParameter == NULL )
        Thread_sleep(1);

    printf("IsoConnection: state = RUNNING. Start to handle connection for client %s\n",
            self->clientAddress);

    while (self->state == ISO_CON_STATE_RUNNING) {
        ByteBuffer_wrap(&receiveBuffer, self->receive_buf, 0, RECEIVE_BUF_SIZE);
        ByteBuffer_wrap(&responseBuffer1, self->send_buf_1, 0, SEND_BUF_SIZE);
        ByteBuffer_wrap(&responseBuffer2, self->send_buf_2, 0, SEND_BUF_SIZE);

        cotpIndication = CotpConnection_parseIncomingMessage(self->cotpConnection);

        switch (cotpIndication) {
        case CONNECT_INDICATION:
            if (DEBUG)
                printf("COTP connection indication\n");

            Semaphore_wait(self->conMutex);

            CotpConnection_sendConnectionResponseMessage(self->cotpConnection);

            Semaphore_post(self->conMutex);

            break;
        case DATA_INDICATION:
			{
				if (DEBUG)
					printf("COTP data indication\n");

				ByteBuffer* cotpPayload = CotpConnection_getPayload(self->cotpConnection);

				sIndication = IsoSession_parseMessage(self->session, cotpPayload);

				ByteBuffer* sessionUserData = IsoSession_getUserData(self->session);

				switch (sIndication) {
				case SESSION_CONNECT:
					if (DEBUG)
						printf("iso_connection: session connect indication\n");

					pIndication = IsoPresentation_parseConnect(self->presentation, sessionUserData);

					if (pIndication == PRESENTATION_OK) {
						if (DEBUG)
							printf("iso_connection: presentation ok\n");

						ByteBuffer* acseBuffer = &(self->presentation->nextPayload);

						aIndication = AcseConnection_parseMessage(&acseConnection, acseBuffer);

						if (aIndication == ACSE_ASSOCIATE) {
							if (DEBUG)
								printf("cotp_server: acse associate\n");

							ByteBuffer mmsRequest;

							ByteBuffer_wrap(&mmsRequest, acseConnection.userDataBuffer,
									acseConnection.userDataBufferSize, acseConnection.userDataBufferSize);

							Semaphore_wait(self->conMutex);

							self->msgRcvdHandler(self->msgRcvdHandlerParameter,
									&mmsRequest, &responseBuffer1);

							if (responseBuffer1.size > 0) {
								if (DEBUG)
									printf("iso_connection: application payload size: %i\n",
											responseBuffer1.size);

								AcseConnection_createAssociateResponseMessage(&acseConnection,
                            			ACSE_RESULT_ACCEPT, &responseBuffer2, &responseBuffer1);

								responseBuffer1.size = 0;

								IsoPresentation_createCpaMessage(self->presentation, &responseBuffer1,
										&responseBuffer2);

								responseBuffer2.size = 0;

								IsoSession_createAcceptSpdu(self->session, &responseBuffer2,
										responseBuffer1.size);

								ByteBuffer_append(&responseBuffer2, responseBuffer1.buffer,
										responseBuffer1.size);

								CotpConnection_sendDataMessage(self->cotpConnection, &responseBuffer2);
							}
							else {
								if (DEBUG)
									printf("iso_connection: association error. No response from application!\n");
							}

							Semaphore_post(self->conMutex);
						}
						else {
							if (DEBUG)
								printf("iso_connection: acse association failed\n");
							self->state = ISO_CON_STATE_STOPPED;
						}

					}
					break;
				case SESSION_DATA:
					if (DEBUG)
						printf("iso_connection: session data indication\n");

					pIndication = IsoPresentation_parseUserData(self->presentation, sessionUserData);

					if (pIndication == PRESENTATION_ERROR) {
						if (DEBUG)
							printf("cotp_server: presentation error\n");
						self->state = ISO_CON_STATE_STOPPED;
						break;
					}

					if (self->presentation->nextContextId == self->presentation->mmsContextId) {
						if (DEBUG)
							printf("iso_connection: mms message\n");

						ByteBuffer* mmsRequest = &(self->presentation->nextPayload);

						self->msgRcvdHandler(self->msgRcvdHandlerParameter,
								mmsRequest, &responseBuffer1);

						if (responseBuffer1.size > 0) {

							Semaphore_wait(self->conMutex);

							IsoPresentation_createUserData(self->presentation,
									&responseBuffer2, &responseBuffer1);

							responseBuffer1.size = 0;

							IsoSession_createDataSpdu(self->session, &responseBuffer1);

							ByteBuffer_append(&responseBuffer1, responseBuffer2.buffer,
									responseBuffer2.size);

							CotpConnection_sendDataMessage(self->cotpConnection, &responseBuffer1);

							Semaphore_post(self->conMutex);
						}
					}
					else {
                		if (DEBUG)
                			printf("iso_connection: unknown presentation layer context!");
					}

					break;
				case SESSION_ERROR:
					self->state = ISO_CON_STATE_STOPPED;
					break;
				}
			}
            break;
        case ERROR:
            if (DEBUG)
                printf("COTP protocol error\n");
            self->state = ISO_CON_STATE_STOPPED;
            break;
        default:
            if (DEBUG)
                printf("COTP Unknown Indication: %i\n", cotpIndication);
            self->state = ISO_CON_STATE_STOPPED;
            break;
        }
    }

    Socket_destroy(self->socket);

    //if (DEBUG)
    printf("IsoConnection: connection closed!\n");

    IsoServer_closeConnection(self->isoServer, self);

    free(self->session);
    free(self->presentation);

    AcseConnection_destroy(&acseConnection);

    CotpConnection_destroy(self->cotpConnection);
    free(self->cotpConnection);

    Semaphore_destroy(self->conMutex);

	free(self->receive_buf);
	free(self->send_buf_1);
	free(self->send_buf_2);
	free(self->clientAddress);
	free(self);
}

IsoConnection
IsoConnection_create(Socket socket, IsoServer isoServer)
{
	IsoConnection self = (IsoConnection) calloc(1, sizeof(struct sIsoConnection));
	self->socket = socket;
	self->receive_buf = (uint8_t*) malloc(RECEIVE_BUF_SIZE);
	self->send_buf_1 = (uint8_t*) malloc(SEND_BUF_SIZE);
	self->send_buf_2 = (uint8_t*) malloc(SEND_BUF_SIZE);
	self->msgRcvdHandler = NULL;
	self->msgRcvdHandlerParameter = NULL;
	self->isoServer = isoServer;
	self->state = ISO_CON_STATE_RUNNING;
	self->clientAddress = Socket_getPeerAddress(self->socket);

	self->thread = Thread_create((ThreadExecutionFunction) handleTcpConnection, self, true);
	self->conMutex = Semaphore_create(1);

	Thread_start(self->thread);

	if (DEBUG) printf("new iso connection thread started\n");

	return self;
}

char*
IsoConnection_getPeerAddress(IsoConnection self)
{
	return self->clientAddress;
}


void
IsoConnection_sendMessage(IsoConnection self, ByteBuffer* message)
{
    Semaphore_wait(self->conMutex);

	ByteBuffer presWriteBuffer;
	ByteBuffer sessionWriteBuffer;

	ByteBuffer_wrap(&presWriteBuffer, self->send_buf_1, 0, SEND_BUF_SIZE);
	ByteBuffer_wrap(&sessionWriteBuffer, self->send_buf_2, 0, SEND_BUF_SIZE);

	IsoPresentation_createUserData(self->presentation,
								&presWriteBuffer, message);

	IsoSession_createDataSpdu(self->session, &sessionWriteBuffer);

	ByteBuffer_append(&sessionWriteBuffer, presWriteBuffer.buffer,
								presWriteBuffer.size);

	CotpConnection_sendDataMessage(self->cotpConnection, &sessionWriteBuffer);

	Semaphore_post(self->conMutex);
}

void
IsoConnection_close(IsoConnection self)
{
	if (self->state != ISO_CON_STATE_STOPPED) {
	    self->state = ISO_CON_STATE_STOPPED;
	}
}

void
IsoConnection_installListener(IsoConnection self, MessageReceivedHandler handler,
		void* parameter)
{
	self->msgRcvdHandler = handler;
	self->msgRcvdHandlerParameter = parameter;
}


