/*
 * 	mms_domain.c
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
#include "mms_server_internal.h"

static MmsVariableSpecification*
getNamedVariableRecursive(MmsVariableSpecification* variable, char* nameId)
{
	char* separator = strchr(nameId, '$');

	int i;

	if (separator == NULL) {

		i = 0;

		if (variable->type == MMS_STRUCTURE) {
			for (i = 0; i < variable->typeSpec.structure.elementCount; i++) {
				if (strcmp(variable->typeSpec.structure.elements[i]->name, nameId) == 0) {
					return variable->typeSpec.structure.elements[i];
				}
			}
		}

		return NULL;
	}
	else {
		MmsVariableSpecification* namedVariable = NULL;
		i = 0;

		for (i = 0; i < variable->typeSpec.structure.elementCount; i++) {

			if (strlen(variable->typeSpec.structure.elements[i]->name) == (separator - nameId)) {

				if (strncmp(variable->typeSpec.structure.elements[i]->name, nameId, separator - nameId) == 0) {
					namedVariable = variable->typeSpec.structure.elements[i];
					break;
				}

			}
		}

		if (namedVariable != NULL) {
		    if (namedVariable->type == MMS_STRUCTURE) {
		        namedVariable = getNamedVariableRecursive(namedVariable, separator + 1);
		    }
		    else if (namedVariable->type == MMS_ARRAY) {
		        namedVariable = namedVariable->typeSpec.array.elementTypeSpec;

		        namedVariable = getNamedVariableRecursive(namedVariable, separator + 1);
		    }
		}

		return namedVariable;
	}
}

static void
freeNamedVariables(MmsVariableSpecification** variables, int variablesCount)
{
	int i;
	for (i = 0; i < variablesCount; i++) {
		if (variables[i]->name != NULL)
			free(variables[i]->name);

		if (variables[i]->type == MMS_STRUCTURE) {
			freeNamedVariables(variables[i]->typeSpec.structure.elements,
					variables[i]->typeSpec.structure.elementCount);
			free(variables[i]->typeSpec.structure.elements);
		}
		else if (variables[i]->type == MMS_ARRAY) {
			freeNamedVariables(&(variables[i]->typeSpec.array.elementTypeSpec), 1);
		}
		free(variables[i]);
	}
}

MmsDomain*
MmsDomain_create(char* domainName)
{
	MmsDomain* self = (MmsDomain*) calloc(1, sizeof(MmsDomain));

	self->domainName = copyString(domainName);
	self->namedVariableLists = LinkedList_create();

	return self;
}

void
MmsDomain_destroy(MmsDomain* self)
{
	free(self->domainName);

	if (self->namedVariables != NULL) {
		freeNamedVariables(self->namedVariables,
				self->namedVariablesCount);

		free(self->namedVariables);
	}

	LinkedList_destroyDeep(self->namedVariableLists, (LinkedListValueDeleteFunction) MmsNamedVariableList_destroy);

	free(self);
}

char*
MmsDomain_getName(MmsDomain* self)
{
	return self->domainName;
}

bool
MmsDomain_addNamedVariableList(MmsDomain* self, MmsNamedVariableList variableList)
{
	//TODO check if operation is allowed!

	LinkedList_add(self->namedVariableLists, variableList);

	return true;
}

MmsNamedVariableList
MmsDomain_getNamedVariableList(MmsDomain* self, char* variableListName)
{
	MmsNamedVariableList variableList = NULL;

	LinkedList element = LinkedList_getNext(self->namedVariableLists);

	while (element != NULL) {
		MmsNamedVariableList varList = (MmsNamedVariableList) element->data;

		if (strcmp(MmsNamedVariableList_getName(varList), variableListName) == 0) {
			variableList = varList;
			break;
		}

		element = LinkedList_getNext(element);
	}

	return variableList;
}

void
MmsDomain_deleteNamedVariableList(MmsDomain* self, char* variableListName)
{
	mmsServer_deleteVariableList(self->namedVariableLists, variableListName);
}

LinkedList
MmsDomain_getNamedVariableLists(MmsDomain* self)
{
	return self->namedVariableLists;
}

MmsVariableSpecification*
MmsDomain_getNamedVariable(MmsDomain* self, char* nameId)
{
	if (self->namedVariables != NULL) {

		char* separator = strchr(nameId, '$');

		int i;

		if (separator == NULL) {

			for (i = 0; i < self->namedVariablesCount; i++) {
				if (strcmp(self->namedVariables[i]->name, nameId) == 0) {
					return self->namedVariables[i];
				}
			}

			return NULL;
		}
		else {
			MmsVariableSpecification* namedVariable = NULL;

			for (i = 0; i < self->namedVariablesCount; i++) {

				if (strlen(self->namedVariables[i]->name) == (separator - nameId)) {

					if (strncmp(self->namedVariables[i]->name, nameId, separator - nameId) == 0) {
						namedVariable = self->namedVariables[i];
						break;
					}
				}
			}

			if (namedVariable != NULL) {
				namedVariable = getNamedVariableRecursive(namedVariable, separator + 1);
			}

			return namedVariable;
		}
	}
	return NULL;
}


