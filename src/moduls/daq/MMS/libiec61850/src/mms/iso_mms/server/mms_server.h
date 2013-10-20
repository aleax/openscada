/*
 *  mms_server.h
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

#ifndef MMS_SERVER_H_
#define MMS_SERVER_H_

/** \addtogroup mms_server_api_group
 *  @{
 */

#include "mms_device_model.h"
#include "mms_value.h"

#include "iso_server.h"

typedef enum {
	MMS_SERVER_NEW_CONNECTION, MMS_SERVER_CONNECTION_CLOSED
} MmsServerEvent;

typedef struct sMmsServer* MmsServer;

typedef struct sMmsServerConnection {
	int maxServOutstandingCalling;
	int maxServOutstandingCalled;
	int dataStructureNestingLevel;
	int maxPduSize; /* local detail */
	IsoConnection isoConnection;
	MmsServer server;
	LinkedList /*<MmsNamedVariableList>*/namedVariableLists; /* aa-specific named variable lists */
	uint32_t lastInvokeId;
} MmsServerConnection;

typedef MmsValue* (*ReadVariableHandler)(void* parameter, MmsDomain* domain,
		char* variableId, MmsServerConnection* connection);

typedef MmsDataAccessError (*WriteVariableHandler)(void* parameter,
		MmsDomain* domain, char* variableId, MmsValue* value,
		MmsServerConnection* connection);

typedef void (*MmsConnectionHandler)(void* parameter,
		MmsServerConnection* connection, MmsServerEvent event);

MmsServer
MmsServer_create(IsoServer isoServer, MmsDevice* device);

void
MmsServer_installReadHandler(MmsServer self, ReadVariableHandler,
		void* parameter);

void
MmsServer_installWriteHandler(MmsServer self, WriteVariableHandler,
		void* parameter);

/**
 * A connection handler will be invoked whenever a new client connection is opened or closed
 */
void
MmsServer_installConnectionHandler(MmsServer self, MmsConnectionHandler,
		void* parameter);

MmsDevice*
MmsServer_getDevice(MmsServer self);

MmsValue*
MmsServer_getValueFromCache(MmsServer self, MmsDomain* domain, char* itemId);

bool
MmsServer_isLocked(MmsServer self);

void
MmsServer_lockModel(MmsServer self);

void
MmsServer_unlockModel(MmsServer self);

void
MmsServer_insertIntoCache(MmsServer self, MmsDomain* domain, char* itemId,
		MmsValue* value);

void
MmsServer_setDevice(MmsServer self, MmsDevice* device);

/* Start a new server thread and listen for incoming connections */
void
MmsServer_startListening(MmsServer self, int tcpPort);

/* Stop server thread an all open connection threads */
void
MmsServer_stopListening(MmsServer self);

void
MmsServer_destroy(MmsServer self);

void
MmsServer_setServerIdentity(MmsServer self, char* vendorName, char* modelName, char* revision);

char*
MmsServer_getVendorName(MmsServer self);

char*
MmsServer_getModelName(MmsServer self);

char*
MmsServer_getRevision(MmsServer self);

/**@}*/

#endif /* MMS_SERVER_H_ */
