/*
 *  mms_device.h
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

#ifndef MMS_DEVICE_H_
#define MMS_DEVICE_H_

typedef struct {
	char* deviceName;
	int domainCount;
	MmsDomain** domains;
} MmsDevice;

/** \addtogroup mms_server_api_group
 *  @{
 */

/**
 * Create a new MmsDevice instance.
 *
 * MmsDevice objects are the root objects of the address space of an MMS server.
 *
 * An MmsDevice object can contain one or more MmsDomain instances.
 *
 * \param deviceName the name of the MMS device or NULL if the device has no name.
 *
 * \return the new MmsDevice instance
 */
MmsDevice*
MmsDevice_create(char* deviceName);

/**
 * Delete the MmsDevice instance
 *
 * This method should not be invoked by client code!
 */
void
MmsDevice_destroy(MmsDevice* self);

/**
 * Get the MmsDomain object with the specified MMS domain name.
 *
 * \param deviceName the name of the MMS device or NULL if the device has no name.
 *
 * \return the new MmsDevice instance
 */
MmsDomain*
MmsDevice_getDomain(MmsDevice* self, char* domainId);

/**@}*/

#endif /* MMS_DEVICE_H_ */
