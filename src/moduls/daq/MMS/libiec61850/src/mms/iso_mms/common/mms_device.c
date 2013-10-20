/*
 *  mms_device.c
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

#include "mms_device_model.h"

MmsDevice*
MmsDevice_create(char* deviceName)
{
	MmsDevice* self = (MmsDevice*) calloc(1, sizeof(MmsDevice));
	self->deviceName = deviceName;

	return self;
}

void
MmsDevice_destroy(MmsDevice* self) {

	int i;
	for (i = 0; i < self->domainCount; i++) {
		MmsDomain_destroy(self->domains[i]);
	}

	free(self->domains);
	free(self);
}



MmsDomain*
MmsDevice_getDomain(MmsDevice* self, char* domainId)
{
	int i;

	for (i = 0; i < self->domainCount; i++) {
		if (strcmp(self->domains[i]->domainName, domainId) == 0) {
			return self->domains[i];
		}

	}

	return NULL;
}


