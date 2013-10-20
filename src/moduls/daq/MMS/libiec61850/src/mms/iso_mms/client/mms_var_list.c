/*
 *  mms_var_list.c
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
 *
 *  Handles MMS Named Variable lists.
 */

#include <MmsPdu.h>
#include "mms_common.h"
#include "mms_client_connection.h"
#include "byte_buffer.h"
#include "string_utilities.h"
#include "mms_client_internal.h"


void
createDefineNamedVariableListMsg(long invokeId, char* listName)
{
	MmsPdu_t* mmsPdu = mmsClient_createConfirmedRequestPdu(invokeId);

	mmsPdu->choice.confirmedRequestPdu.confirmedServiceRequest.present =
		ConfirmedServiceRequest_PR_defineNamedVariableList;

	DefineNamedVariableListRequest_t* request;

	request = &(mmsPdu->choice.confirmedRequestPdu.confirmedServiceRequest.choice.defineNamedVariableList);

	request->variableListName.choice.aaspecific.buf = (uint8_t*) copyString(listName);
	request->variableListName.choice.aaspecific.size = strlen(listName);
}
