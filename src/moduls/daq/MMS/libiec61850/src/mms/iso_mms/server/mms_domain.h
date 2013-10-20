/*
 *  mms_domain.h
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

#ifndef MMS_DOMAIN_H_
#define MMS_DOMAIN_H_

/**
 * \defgroup mms_server_api_group MMS server API
 */
/**@{*/

#include "mms_type_spec.h"
#include "mms_common.h"
#include "mms_named_variable_list.h"

/**
 * Server side data structure to hold informations for a MMS domain (Logical Device)
 */
struct sMmsDomain {
	char* domainName;
	int namedVariablesCount;
	MmsVariableSpecification** namedVariables;
	LinkedList /*<MmsNamedVariableList>*/ namedVariableLists;
};

/**
 * Create a new MmsDomain instance
 *
 * This method should not be invoked by client code!
 *
 * \param domainName the name of the MMS domain
 *
 * \return the new MmsDomain instance
 */
MmsDomain*
MmsDomain_create(char* domainName);

char*
MmsDomain_getName(MmsDomain* self);

/**
 * Delete a MmsDomain instance
 *
 * This method should not be invoked by client code!
 */
void
MmsDomain_destroy(MmsDomain* self);

/**
 * Add a new MMS Named Variable List (Data set) to a MmsDomain instance
 *
 * The passed MmsNamedVariableList instance will be handled by the MmsDomain instance
 * and will be destroyed if the MmsDomain_destroy function on this MmsDomain instance
 * is called.
 *
 * \param self instance of MmsDomain to operate on
 * \param variableList new named variable list that will be added to this MmsDomain
 *
 * \return true if operation was successful.
 */
bool
MmsDomain_addNamedVariableList(MmsDomain* self, MmsNamedVariableList variableList);

/**
 * Delete a MMS Named Variable List from this MmsDomain instance
 *
 * A call to this function will also destroy the MmsNamedVariableList instance.
 *
 * \param self instance of MmsDomain to operate on
 * \param variableListName the name of the variable list to delete.
 *
 */
void
MmsDomain_deleteNamedVariableList(MmsDomain* self, char* variableListName);

MmsNamedVariableList
MmsDomain_getNamedVariableList(MmsDomain* self, char* variableListName);

LinkedList
MmsDomain_getNamedVariableLists(MmsDomain* self);

LinkedList
MmsDomain_getNamedVariableListValues(MmsDomain* self, char* variableListName);

LinkedList
MmsDomain_createNamedVariableListValues(MmsDomain* self, char* variableListName);

/**
 * Get the MmsTypeSpecification instance of a MMS named variable
 *
 * \param self instance of MmsDomain to operate on
 * \param nameId name of the named variable
 *
 * \return MmsTypeSpecification instance of the named variable
 */
MmsVariableSpecification*
MmsDomain_getNamedVariable(MmsDomain* domain, char* nameId);

/**@}*/

#endif /* MMS_DOMAIN_H_ */
