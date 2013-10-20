/*
 *  mms_client_connection.h
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

#ifndef MMS_CLIENT_CONNECTION_H_
#define MMS_CLIENT_CONNECTION_H_

/**
 * \defgroup mms_client_api_group MMS client API
 */
/**@{*/

#include "libiec61850_platform_includes.h"

#include "mms_common.h"
#include "mms_type_spec.h"
#include "mms_value.h"
#include "iso_client_connection.h"
#include "linked_list.h"

/**
 * Detailed MMS Client error codes
 */
typedef enum {
    MMS_ERROR_NONE,
    MMS_ERROR_TIMEOUT,
    MMS_ERROR_CONNECTION_LOST,
    MMS_ERROR_SERVICE_ERROR
} MmsClientError;

/**
 * Contains MMS layer specific parameters
 */
typedef struct sMmsConnectionParameters {
	int maxServOutstandingCalling;
	int maxServOutstandingCalled;
	int dataStructureNestingLevel;
	int maxPduSize; /* local detail */
} MmsConnectionParameters;

typedef struct {
    char* vendorName;
    char* modelName;
    char* revision;
} MmsServerIdentity;

typedef void (*MmsInformationReportHandler) (void* parameter, char* domainName,
        char* variableListName, MmsValue* value, bool isVariableListName);

/**
 * Opaque handle for MMS client connection instance.
 */
typedef struct sMmsConnection* MmsConnection;


/*******************************************************************************
 * Connection management functions
 *******************************************************************************/

/**
 * Create a new MmsConnection instance
 *
 * \return the newly created instance.
 */
MmsConnection
MmsConnection_create();

/**
 * \brief Set the request timeout for this connection
 *
 * \param self MmsConnection instance to operate on
 * \param timeoutInMs request timeout in milliseconds
 */
void
MmsConnection_setRequestTimeout(MmsConnection self, uint32_t timeoutInMs);

/**
 * Install a handler function for MMS information reports (unsolicited messages from the server).
 * The handler function will be called whenever the client receives an MMS information report message.
 * Note that the API user is responsible to properly free the passed MmsValue object.
 *
 * \param self MmsConnection instance to operate on
 * \param handler the handler function to install for this client connection
 * \param parameter a user specified parameter that will be passed to the handler function on each
 *        invocation.
 */
void
MmsConnection_setInformationReportHandler(MmsConnection self, MmsInformationReportHandler handler,
        void* parameter);

/**
 * Set the connection parameters for a MmsConnection instance
 *
 * \param self MmsConnection instance to operate on
 * \param params the parameters to use
 */
void
MmsConnection_setConnectionParameters(MmsConnection self, MmsConnectionParameters params);

/**
 * Set the ISO connection parameters for a MmsConnection instance
 *
 * \param self MmsConnection instance to operate on
 * \param params the ISO client parameters to use
 */
void
MmsConnection_setIsoConnectionParameters(MmsConnection self, IsoConnectionParameters* params);

/**
 * Destroy a MmsConnection instance and release all resources
 *
 * \param self MmsConnection instance to operate on
 */
void
MmsConnection_destroy(MmsConnection self);

/**
 * Get a detailed description of the last error occurred.
 *
 * \param self MmsConnection instance to operate on
 */
MmsClientError
MmsConnection_getError(MmsConnection self);


/*******************************************************************************
 * Blocking functions for connection establishment and data access
 *******************************************************************************/


/**
 * Connect to a MMS server. This will open a new TCP connection and send
 * a MMS initiate request.
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 * \param serverName hostname or IP address of the server to connect
 * \param serverPort TCP port number of the server to connect (usually 102)
 *
 * \return MMS_OK on success. MMS_ERROR if the connection attempt failed.
 */
MmsIndication
MmsConnection_connect(MmsConnection self, MmsClientError* mmsError, char* serverName, int serverPort);

/**
 * Get the domains names for all domains of the server.
 *
 * This will result in a VMD specific GetNameList request.
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 *
 * \return the list of domain names or NULL if the request failed.
 *
 */
LinkedList /* <char*> */
MmsConnection_getDomainNames(MmsConnection self, MmsClientError* mmsError);

/**
 * Get the names of all variables present in a MMS domain of the server.
 *
 * This will result in a domain specific GetNameList request.
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 * \param domainId the domain name for the domain specific request
 *
 * \return the of domain specific variable names or NULL if the request failed.
 */
LinkedList /* <char*> */
MmsConnection_getDomainVariableNames(MmsConnection self, MmsClientError* mmsError, char* domainId);

/**
 * Get the names of all named variable lists present in a MMS domain of the server.
 *
 * This will result in a domain specific GetNameList request.
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 * \param domainId the domain name for the domain specific request
 *
 * \return the domain specific named variable list names or NULL if the request failed.
 */
LinkedList /* <char*> */
MmsConnection_getDomainVariableListNames(MmsConnection self, MmsClientError* mmsError, char* domainId);

/**
 * Get the names of all named variable lists associated with this client connection.
 *
 * This will result in an association specific GetNameList request.
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 *
 * \return the association specific named variable list names or NULL if the request failed.
 */
LinkedList /* <char*> */
MmsConnection_getVariableListNamesAssociationSpecific(MmsConnection self, MmsClientError* mmsError);


/**
 * Read a single variable from the server.
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 * \param domainId the domain name of the variable to be read
 * \param itemId name of the variable to be read
 *
 * \return Returns a MmsValue object or NULL if the request failed. The MmsValue object can
 * either be a simple value or a complex value or array.
 */
MmsValue*
MmsConnection_readVariable(MmsConnection self, MmsClientError* mmsError, char* domainId, char* itemId);

/**
 * Read an element of a single array variable from the server.
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 * \param domainId the domain name of the variable to be read
 * \param itemId name of the variable to be read
 * \param startIndex index of element to read or start index if a element range is to be read
 * \param numberOfElements Number of elements to read or 0 if a single element is to be read
 *
 * \return Returns a MmsValue object or NULL if the request failed. The MmsValue object is either
 * a simple or complex type if numberOfElements is 0, or an array containing the selected
 * array elements of numberOfElements > 0.
 */
MmsValue*
MmsConnection_readArrayElements(MmsConnection self, MmsClientError* mmsError, char* domainId, char* itemId,
		uint32_t startIndex, uint32_t numberOfElements);

/**
 * Read multiple variables of a domain from the server with one request message.
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 * \param domainId the domain name of the requested variables.
 * \param items: LinkedList<char*> is the list of item IDs of the requested variables.
 *
 * \return  Returns a MmsValue object or NULL if the request failed. The MmsValue object is
 * is of type MMS_ARRAY and contains the variable values of simple or complex type
 * in the order as they appeared in the item ID list.
 */
MmsValue*
MmsConnection_readMultipleVariables(MmsConnection self, MmsClientError* mmsError, char* domainId,
		LinkedList /*<char*>*/ items);

/**
 * \brief Write a single variable to the server.
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 * \param domainId the domain name of the variable to be written
 * \param itemId name of the variable to be written
 * \param value value of the variable to be written
 *
 * \return MMS_OK on success. MMS_ERROR if the write attempt failed.
 */
MmsIndication
MmsConnection_writeVariable(MmsConnection self, MmsClientError* mmsError,
        char* domainId, char* itemId, MmsValue* value);

/**
 * \brief Write multiple variables at the server  (NOT YET IMPLEMENTED).
 *
 * This function will write multiple variables at the server.
 *
 * The parameter accessResults is a pointer to a LinkedList reference. The methods will create a new LinkedList
 * object that contains the AccessResults of the single variable write attempts. It is up to the user to free this
 * objects properly (e.g. with LinkedList_destroyDeep(accessResults, MmsValue_delete)).
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 * \param domainId the common domain name of all variables to be written
 * \param items a linked list containing the names of the variables to be written. The names are C strings.
 * \param values values of the variables to be written
 * \param (OUTPUT) the MmsValue objects of type MMS_DATA_ACCESS_ERROR representing the write success of a single variable
 *        write.
 *
 * \return MMS_OK if the service succeeded. MMS_ERROR if a service error occurred.
 */
MmsIndication
MmsConnection_writeMultipleVariables(MmsConnection self, MmsClientError* mmsError, char* domainId,
        LinkedList /*<char*>*/ items, LinkedList /* <MmsValue*> */ values,
        LinkedList* /* <MmsValue*> */ accessResults);

/**
 * Get the variable access attributes of a MMS named variable of the server
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 * \param domainId the domain name of the variable
 * \param itemId name of the variable
 *
 * \return Returns a MmsTypeSpecification object or NULL if the request failed.
 */
MmsVariableSpecification*
MmsConnection_getVariableAccessAttributes(MmsConnection self, MmsClientError* mmsError,
		char* domainId, char* itemId);

/**
 * Read the values of a domain specific named variable list
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 * \param domainId the domain name of the requested variables.
 * \param listName the name of the named variable list
 * \param specWithResult if specWithResult is set to true, a IEC 61850 compliant request will be sent.
 *
 * \return Returns a MmsValue object or NULL if the request failed. The MmsValue object is
 * is of type MMS_ARRAY and contains the variable values of simple or complex type
 * in the order as they appeared in named variable list definition.
 */
MmsValue*
MmsConnection_readNamedVariableListValues(MmsConnection self, MmsClientError* mmsError, char* domainId,
        char* listName,	bool specWithResult);


/**
 * Read the values of a association specific named variable list
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 * \param listName the name of the named variable list
 * \param specWithResult if specWithResult is set to true, a IEC 61850 compliant request will be sent.
 *
 * \return Returns a MmsValue object or NULL if the request failed. The MmsValue object is
 * is of type MMS_ARRAY and contains the variable values of simple or complex type
 * in the order as they appeared in named variable list definition.
 */
MmsValue*
MmsConnection_readNamedVariableListValuesAssociationSpecific(MmsConnection self, MmsClientError* mmsError,
        char* listName,	bool specWithResult);

/**
 * Define a new named variable list at the server.
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 * \param domainId the domain name of the domain for the new variable list
 * \param listName the name of the named variable list
 * \param variableSpecs a list of variable specifications for the new variable list. The list
 *        elements have to be of type MmsVariableAccessSpecification*.
 *
 * \return MMS_OK on success. MMS_ERROR if the write attempt failed.
 */
MmsIndication
MmsConnection_defineNamedVariableList(MmsConnection self, MmsClientError* mmsError, char* domainId,
        char* listName,	LinkedList variableSpecs);


/**
 * Define a new association specific named variable list at the server.
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 * \param listName the name of the named variable list
 * \param variableSpecs list of variable specifications for the new variable list.The list
 *        elements have to be of type MmsVariableAccessSpecification*.
 *
 * \return MMS_OK on success. MMS_ERROR if the write attempt failed.
 */
MmsIndication
MmsConnection_defineNamedVariableListAssociationSpecific(MmsConnection self, MmsClientError* mmsError,
        char* listName,	LinkedList variableSpecs);

/**
 * Read the entry list of a named variable list at the server.
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 * \param domainId the domain name of the domain of the variable list
 * \param listName the name of the named variable list
 * \param deletable THIS IS A OUTPUT PARAMETER - indicates if the variable list is deletable by the
 * client. The user may provide a NULL pointer if the value doesn't matter.
 *
 * \return List of names of the variable list entries or NULL if the request failed
 */
LinkedList /* <MmsVariableAccessSpecification*> */
MmsConnection_readNamedVariableListDirectory(MmsConnection self, MmsClientError* mmsError,
        char* domainId, char* listName, bool* deletable);

/**
 * Read the entry list of an association specific named variable list at the server.
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 * \param listName the name of the named variable list
 *
 * \return List of names of the variable list entries or NULL if the request failed
 */
LinkedList /* <MmsVariableAccessSpecification*> */
MmsConnection_readAssociationSpecificNamedVariableListDirectory(MmsConnection self, MmsClientError* mmsError,
        char* listName);

/**
 * Delete a named variable list at the server.
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 * \param domainId the domain name of the domain of the variable list
 * \param listName the name of the named variable list
 *
 * \return MMS_OK on success. MMS_ERROR if the request failed.
 */
MmsIndication
MmsConnection_deleteNamedVariableList(MmsConnection self, MmsClientError* mmsError, char* domainId, char* listName);

/**
 * Delete an association specific named variable list at the server.
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 * \param listName the name of the named variable list
 *
 * \return MMS_OK on success. MMS_ERROR if the request failed.
 */
MmsIndication
MmsConnection_deleteAssociationSpecificNamedVariableList(MmsConnection self, MmsClientError* mmsError,
        char* listName);

/**
 * Create a new MmsVariableSpecification that can be used to define named variable lists.
 * The created object can be deleted with free(). If the parameter strings were dynamically
 * allocated the deallocation is in the responsibility of the user.
 *
 * \param domainId the MMS domain name of the variable
 * \param itemId the name for the MMS variable
 *
 * \return reference to the new MmsVariableSpecfication object
 */
MmsVariableAccessSpecification*
MmsVariableAccessSpecification_create(char* domainId, char* itemId);

/**
 * Create a new MmsVariableSpecification that can be used to define named variable lists.
 * The created object can be deleted with free(). If the parameter strings were dynamically
 * allocated the deallocation is in the responsibility of the user. This function should be
 * used for named variable list entries that are array elements or components of array elements
 * in the case when the array element is of complex (structured) type.
 *
 * \param domainId the MMS domain name of the variable
 * \param itemId the name for the MMS variable
 * \param index the array index to describe an array element
 * \param componentName the name of the component of the array element. Should be set to NULL
 *        if the array element is of simple type or the whole array element is required.
 *
 * \return reference to the new MmsVariableSpecfication object
 */
MmsVariableAccessSpecification*
MmsVariableAccessSpecification_createAlternateAccess(char* domainId, char* itemId, int32_t index,
		char* componentName);

/**
 * Delete the MmsVariableAccessSpecification data structure
 *
 * \param self the instance to delete
 */
void
MmsVariableAccessSpecification_destroy(MmsVariableAccessSpecification* self);

/**
 * Get the MMS local detail parameter (local detail means maximum MMS PDU size). This defaults to
 * 65000. This function should not be called after a successful connection attempt.
 *
 * \param localDetail the maximum size of the MMS PDU that will be accepted.
 */
void
MmsConnection_setLocalDetail(MmsConnection self, int32_t localDetail);

int32_t
MmsConnection_getLocalDetail(MmsConnection self);

/**
 * \brief get the identity of the connected server
 *
 * This function will return the identity of the server if the server supports the MMS identify service.
 * The server identity consists of a vendor name, model name, and a revision.
 *
 * \param  self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 */
MmsServerIdentity*
MmsConnection_identify(MmsConnection self, MmsClientError* mmsError);

void
MmsServerIdentity_destroy(MmsServerIdentity* self);

/**@}*/

#endif /* MMS_CLIENT_CONNECTION_H_ */
