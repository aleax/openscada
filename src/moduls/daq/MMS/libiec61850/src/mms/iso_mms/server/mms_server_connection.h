/*
 *  mms_connection.h
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

/**
 * A MmsServerConnection object is responsible for handling a client connection.
 */

#ifndef MMS_SERVER_CONNECTION_H_
#define MMS_SERVER_CONNECTION_H_

#include "libiec61850_platform_includes.h"

#include "mms_common.h"
#include "mms_device_model.h"
#include "mms_value.h"
#include "mms_server.h"
#include "iso_server.h"
#include "linked_list.h"
#include "byte_buffer.h"

MmsServerConnection*
MmsServerConnection_init(MmsServerConnection* connection, MmsServer server, IsoConnection isoCon);

void
MmsServerConnection_destroy(MmsServerConnection* connection);

bool
MmsServerConnection_addNamedVariableList(MmsServerConnection* self, MmsNamedVariableList variableList);

MmsNamedVariableList
MmsServerConnection_getNamedVariableList(MmsServerConnection* self, char* variableListName);

LinkedList
MmsServerConnection_getNamedVariableLists(MmsServerConnection* self);

void
MmsServerConnection_deleteNamedVariableList(MmsServerConnection* self, char* listName);

char*
MmsServerConnection_getClientAddress(MmsServerConnection* self);

MmsIndication
MmsServerConnection_parseMessage
(MmsServerConnection* connection, ByteBuffer* message, ByteBuffer* response);

void /* send information report for a single VMD specific variable */
MmsServerConnection_sendInformationReportSingleVariableVMDSpecific(MmsServerConnection* self,
		char* itemId, MmsValue* value);

void /* send information report for a VMD specific named variable list */
MmsServerConnection_sendInformationReportVMDSpecific(MmsServerConnection* self, char* itemId, LinkedList values);

void
MmsServerConnection_sendInformationReportListOfVariables(
        MmsServerConnection* self,
        LinkedList /* MmsVariableAccessSpecification */ variableAccessDeclarations,
        LinkedList /* MmsValue */ values
        );

void
MmsServerConnection_sendWriteResponse(MmsServerConnection* self, uint32_t invokeId, MmsDataAccessError indication);


uint32_t
MmsServerConnection_getLastInvokeId(MmsServerConnection* self);

#endif /* MMS_SERVER_CONNECTION_H_ */


