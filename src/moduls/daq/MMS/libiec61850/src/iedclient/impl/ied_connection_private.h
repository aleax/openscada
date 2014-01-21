/*
 *  ied_connection_private.h
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

#ifndef IED_CONNECTION_PRIVATE_H_
#define IED_CONNECTION_PRIVATE_H_

#ifndef DEBUG_IED_CLIENT
#define DEBUG_IED_CLIENT 0
#endif

struct sClientReportControlBlock {
    char* objectReference;
    bool isBuffered;

    MmsValue* rptId;
    MmsValue* rptEna;
    MmsValue* resv;
    MmsValue* datSet;
    MmsValue* confRev;
    MmsValue* optFlds;
    MmsValue* bufTm;
    MmsValue* sqNum;
    MmsValue* trgOps;
    MmsValue* intgPd;
    MmsValue* gi;
    MmsValue* purgeBuf;
    MmsValue* entryId;
    MmsValue* timeOfEntry;
    MmsValue* resvTms;
    MmsValue* owner;
};

void
private_IedConnection_addControlClient(IedConnection self, ControlObjectClient control);

void
private_IedConnection_removeControlClient(IedConnection self, ControlObjectClient control);

bool
private_ClientReportControlBlock_updateValues(ClientReportControlBlock self, MmsValue* values);

#endif /* IED_CONNECTION_PRIVATE_H_ */
