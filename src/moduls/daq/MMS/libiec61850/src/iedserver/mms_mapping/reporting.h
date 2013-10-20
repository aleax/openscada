/*
 *  reporting.h
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

#ifndef REPORTING_H_
#define REPORTING_H_

typedef struct {
    char* name;
    MmsDomain* domain;
    MmsValue* rcbValues;
    MmsValue* inclusionField;
    MmsValue* confRev;
    MmsValue* timeOfEntry;
    DataSet* dataSet;
    bool isDynamicDataSet;
    bool enabled;
    bool reserved;
    bool bufferd;
    bool gi;
    bool triggered;
    uint16_t sqNum;
    uint32_t intgPd;
    uint32_t bufTm;
    uint64_t nextIntgReportTime;
    uint64_t reportTime;
    uint64_t reservationTimeout;
    MmsServerConnection* clientConnection;
    ReportInclusionFlag* inclusionFlags;
    int triggerOps;
} ReportControl;

ReportControl*
ReportControl_create(bool buffered);

void
ReportControl_destroy(ReportControl* self);

void
ReportControl_valueUpdated(ReportControl* self, int dataSetEntryIndex, ReportInclusionFlag flag);

MmsValue*
ReportControl_getRCBValue(ReportControl* rc, char* elementName);

MmsVariableSpecification*
Reporting_createMmsBufferedRCBs(MmsMapping* self, MmsDomain* domain,
        LogicalNode* logicalNode, int reportsCount);

MmsVariableSpecification*
Reporting_createMmsUnbufferedRCBs(MmsMapping* self, MmsDomain* domain,
        LogicalNode* logicalNode, int reportsCount);

MmsDataAccessError
Reporting_RCBWriteAccessHandler(MmsMapping* self, ReportControl* rc, char* elementName, MmsValue* value,
        MmsServerConnection* connection);


void
Reporting_processReportEvents(MmsMapping* self, uint64_t currentTimeInMs);

#endif /* REPORTING_H_ */
