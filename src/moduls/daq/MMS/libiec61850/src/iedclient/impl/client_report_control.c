/*
 *  client_report_control.c
 *
 *  Implementation of the ClientReportControlBlock class
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

#include "iec61850_client.h"

#include "stack_config.h"

#include "ied_connection_private.h"

static bool
isBufferedRcb(char* objectReference)
{
    char* separator = strchr(objectReference, '.');

    if (separator == NULL)
        return false; //TODO report an error

    if (*(separator + 1) == 'B')
        return true;
    else
        return false;
}

ClientReportControlBlock
ClientReportControlBlock_create(char* objectReference)
{
    ClientReportControlBlock self = (ClientReportControlBlock) calloc(1, sizeof(struct sClientReportControlBlock));

    self->objectReference = copyString(objectReference);
    self->isBuffered = isBufferedRcb(objectReference);

    return self;
}

void
ClientReportControlBlock_destroy(ClientReportControlBlock self)
{
    free(self->objectReference);

    MmsValue_deleteIfNotNull(self->rptId);
    MmsValue_deleteIfNotNull(self->rptEna);
    MmsValue_deleteIfNotNull(self->resv);
    MmsValue_deleteIfNotNull(self->datSet);
    MmsValue_deleteIfNotNull(self->confRev);
    MmsValue_deleteIfNotNull(self->optFlds);
    MmsValue_deleteIfNotNull(self->bufTm);
    MmsValue_deleteIfNotNull(self->sqNum);
    MmsValue_deleteIfNotNull(self->trgOps);
    MmsValue_deleteIfNotNull(self->intgPd);
    MmsValue_deleteIfNotNull(self->gi);
    MmsValue_deleteIfNotNull(self->purgeBuf);
    MmsValue_deleteIfNotNull(self->entryId);
    MmsValue_deleteIfNotNull(self->timeOfEntry);
    MmsValue_deleteIfNotNull(self->resvTms);
    MmsValue_deleteIfNotNull(self->owner);

    free(self);
}

char*
ClientReportControlBlock_getObjectReference(ClientReportControlBlock self)
{
    return self->objectReference;
}

bool
ClientReportControlBlock_isBuffered(ClientReportControlBlock self)
{
    return self->isBuffered;
}

char*
ClientReportControlBlock_getRptId(ClientReportControlBlock self)
{
    if (self->rptId != NULL)
        return MmsValue_toString(self->rptId);
    else
        return NULL;
}

void
ClientReportControlBlock_setRptId(ClientReportControlBlock self, char* rptId)
{
    if (self->rptId == NULL)
        self->rptId = MmsValue_newVisibleString(rptId);
    else
        MmsValue_setVisibleString(self->rptId, rptId);
}


bool
ClientReportControlBlock_getRptEna(ClientReportControlBlock self)
{
    if (self->rptEna != NULL) {
        return MmsValue_getBoolean(self->rptEna);
    }
    else
        return false;
}

void
ClientReportControlBlock_setRptEna(ClientReportControlBlock self, bool rptEna)
{
    if (self->rptEna == NULL)
        self->rptEna = MmsValue_newBoolean(rptEna);
    else
        MmsValue_setBoolean(self->rptEna, rptEna);
}

bool
ClientReportControlBlock_getResv(ClientReportControlBlock self)
{
    if (self->resv != NULL) {
        return MmsValue_getBoolean(self->resv);
    }
    else
        return false;
}

void
ClientReportControlBlock_setResv(ClientReportControlBlock self, bool resv)
{
    if (self->resv == NULL)
        self->resv = MmsValue_newBoolean(resv);
    else
        MmsValue_setBoolean(self->resv, resv);
}

char*
ClientReportControlBlock_getDataSetReference(ClientReportControlBlock self)
{
    if (self->datSet != NULL)
        return MmsValue_toString(self->datSet);
    else
        return NULL;
}

void
ClientReportControlBlock_setDataSetReference(ClientReportControlBlock self, char* dataSetReference)
{
    if (self->datSet == NULL)
        self->datSet = MmsValue_newVisibleString(dataSetReference);
    else
        MmsValue_setVisibleString(self->datSet, dataSetReference);
}

uint32_t
ClientReportControlBlock_getConvRev(ClientReportControlBlock self)
{
    if (self->confRev != NULL)
        return MmsValue_toUint32(self->confRev);
    else
        return 0;
}

MmsValue*
ClientReportControlBlock_getOptFlds(ClientReportControlBlock self)
{
    return self->optFlds;
}

void
ClientReportControlBlock_setOptFlds(ClientReportControlBlock self, MmsValue* optFlds)
{
    if (self->optFlds != NULL) {
        MmsValue_update(self->optFlds, optFlds);
    }
    else {
        if (MmsValue_getType(optFlds) != MMS_BIT_STRING) {
            if (DEBUG_IED_CLIENT)
                printf("IED_CLIENT: ClientReportControlBlock_setOptFlds invalid argument type\n");
        }
        else
            self->optFlds = MmsValue_clone(optFlds);
    }
}

uint32_t
ClientReportControlBlock_getBufTm(ClientReportControlBlock self)
{
    if (self->bufTm != NULL)
        return MmsValue_toUint32(self->bufTm);
    else
        return 0;
}

void
ClientReportControlBlock_setBufTm(ClientReportControlBlock self, uint32_t bufTm)
{
    if (self->bufTm == NULL)
        self->bufTm = MmsValue_newUnsignedFromUint32(bufTm);
    else
        MmsValue_setUint32(self->bufTm, bufTm);
}

uint16_t
ClientReportControlBlock_getSqNum(ClientReportControlBlock self)
{
    if (self->sqNum != NULL)
        return (uint16_t) MmsValue_toUint32(self->sqNum);
    else
        return 0;
}

int
ClientReportControlBlock_getTrgOps(ClientReportControlBlock self)
{
    int triggerOps = 0;

    if (self->trgOps != NULL) {
        if (MmsValue_getBitStringBit(self->trgOps, 1))
            triggerOps += TRG_OPT_DATA_CHANGED;
        if (MmsValue_getBitStringBit(self->trgOps, 2))
            triggerOps += TRG_OPT_QUALITY_CHANGED;
        if (MmsValue_getBitStringBit(self->trgOps, 3))
            triggerOps += TRG_OPT_DATA_UPDATE;
        if (MmsValue_getBitStringBit(self->trgOps, 4))
            triggerOps += TRG_OPT_INTEGRITY;
        if (MmsValue_getBitStringBit(self->trgOps, 5))
            triggerOps += TRG_OPT_GI;
    }

    return triggerOps;
}

void
ClientReportControlBlock_setTrgOps(ClientReportControlBlock self, int trgOps)
{
    if (self->trgOps == NULL)
        self->trgOps = MmsValue_newBitString(6);

    MmsValue_setBitStringFromInteger(self->trgOps, trgOps << 1);
}

uint32_t
ClientReportControlBlock_getIntgPd(ClientReportControlBlock self)
{
    if (self->intgPd != NULL)
        return MmsValue_toUint32(self->intgPd);
    else
        return 0;
}

void
ClientReportControlBlock_setIntgPd(ClientReportControlBlock self, uint32_t intgPd)
{
    if (self->intgPd == NULL)
        self->intgPd = MmsValue_newUnsignedFromUint32(intgPd);
    else
        MmsValue_setUint32(self->intgPd, intgPd);
}

bool
ClientReportControlBlock_getGI(ClientReportControlBlock self)
{
    if (self->gi != NULL)
        return MmsValue_getBoolean(self->gi);
    else
        return false;
}

void
ClientReportControlBlock_setGI(ClientReportControlBlock self, bool gi)
{
    if (self->gi == NULL)
        self->gi = MmsValue_newBoolean(gi);
    else
        MmsValue_setBoolean(self->gi, gi);
}

bool
ClientReportControlBlock_getPurgeBuf(ClientReportControlBlock self)
{
    if (self->purgeBuf != NULL)
        return MmsValue_getBoolean(self->purgeBuf);
    else
        return false;
}

void
ClientReportControlBlock_setPurgeBuf(ClientReportControlBlock self, bool purgeBuf)
{
    if (self->purgeBuf == NULL)
        self->purgeBuf = MmsValue_newBoolean(purgeBuf);
    else
        MmsValue_setBoolean(self->purgeBuf, purgeBuf);
}

int16_t
ClientReportControlBlock_getResvTms(ClientReportControlBlock self)
{
    if (self->resvTms != NULL)
        return (int16_t) MmsValue_toInt32(self->resvTms);
    else
        return 0;
}

void
ClientReportControlBlock_setResvTms(ClientReportControlBlock self, int16_t resvTms)
{
    if (self->resvTms == NULL)
        self->resvTms = MmsValue_newIntegerFromInt16(resvTms);
    else
        MmsValue_setInt32(self->resvTms, (int32_t) resvTms);
}

MmsValue*
ClientReportControlBlock_getEntryId(ClientReportControlBlock self)
{
    return self->entryId;
}

void
ClientReportControlBlock_setEntryId(ClientReportControlBlock self, MmsValue* entryId)
{
    if (self->entryId != NULL) {
        MmsValue_update(self->entryId, entryId);
    }
    else {
        if (MmsValue_getType(entryId) != MMS_OCTET_STRING) {
            if (DEBUG_IED_CLIENT)
                printf("IED_CLIENT: ClientReportControlBlock_setEntryId invalid argument type\n");
        }
        else
            self->entryId = MmsValue_clone(entryId);
    }
}

uint64_t
ClientReportControlBlock_getEntryTime(ClientReportControlBlock self)
{
    if (self->timeOfEntry != NULL)
        return MmsValue_getBinaryTimeAsUtcMs(self->timeOfEntry);
    else
        return 0;
}

MmsValue*
ClientReportControlBlock_getOwner(ClientReportControlBlock self)
{
    return self->owner;
}

static void
updateOrClone(MmsValue** valuePtr, MmsValue* values, int index)
{
    if (*valuePtr != NULL)
        MmsValue_update(*valuePtr, MmsValue_getElement(values, index));
    else
        *valuePtr = MmsValue_clone(MmsValue_getElement(values, index));
}

bool
private_ClientReportControlBlock_updateValues(ClientReportControlBlock self, MmsValue* values)
{
    int rcbElementCount = MmsValue_getArraySize(values);

    updateOrClone(&(self->rptId), values, 0);
    updateOrClone(&(self->rptEna), values, 1);

    if (self->isBuffered) {
        updateOrClone(&(self->datSet), values, 2);
        updateOrClone(&(self->confRev), values, 3);
        updateOrClone(&(self->optFlds), values, 4);
        updateOrClone(&(self->bufTm), values, 5);
        updateOrClone(&(self->sqNum), values, 6);
        updateOrClone(&(self->trgOps), values,7);
        updateOrClone(&(self->intgPd), values, 8);
        updateOrClone(&(self->gi), values, 9);
        updateOrClone(&(self->purgeBuf), values, 10);
        updateOrClone(&(self->entryId), values, 11);
        updateOrClone(&(self->timeOfEntry), values, 12);

        if (rcbElementCount > 13) {
            MmsValue* element13 = MmsValue_getElement(values, 13);

            if (MmsValue_getType(element13) == MMS_OCTET_STRING)
                updateOrClone(&(self->owner), values, 13);
            else {
                updateOrClone(&(self->resvTms), values, 13);

                if (rcbElementCount > 14)
                    updateOrClone(&(self->owner), values, 14);
            }
        }

    }
    else {
        updateOrClone(&(self->resv), values, 2);
        updateOrClone(&(self->datSet), values, 3);
        updateOrClone(&(self->confRev), values, 4);
        updateOrClone(&(self->optFlds), values, 5);
        updateOrClone(&(self->bufTm), values, 6);
        updateOrClone(&(self->sqNum), values, 7);
        updateOrClone(&(self->trgOps), values, 8);
        updateOrClone(&(self->intgPd), values, 9);
        updateOrClone(&(self->gi), values, 10);

        if (rcbElementCount == 12) /* owner is optional */
            updateOrClone(&(self->owner), values, 11);
    }

	return true;
}
