/*
 *  iec61850_client.h
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

#ifndef IEC61850_CLIENT_H_
#define IEC61850_CLIENT_H_

#include "libiec61850_platform_includes.h"
#include "iec61850_common.h"
#include "goose_subscriber.h"
#include "mms_value.h"
#include "mms_client_connection.h"
#include "linked_list.h"

/**
 *  * \defgroup iec61850_client_api_group IEC 61850/MMS client API
 */
/**@{*/

typedef struct sIedConnection* IedConnection;
typedef struct sClientDataSet* ClientDataSet;
typedef struct sClientReport* ClientReport;

typedef struct {
    int ctlNum;
    int error;
    int addCause;
} LastApplError;

typedef enum {
    IED_ERROR_OK,
    IED_ERROR_NOT_CONNECTED,
    IED_ERROR_TIMEOUT,
    IED_ERROR_ACCESS_DENIED,
    IED_ERROR_ENABLE_REPORT_FAILED_DATASET_MISMATCH,
    IED_ERROR_UNKNOWN,
    IED_ERROR_OBJECT_REFERENCE_INVALID,
} IedClientError;


/**************************************************
 * Connection creation and destruction
 **************************************************/

IedConnection
IedConnection_create();

void
IedConnection_destroy(IedConnection self);

/**************************************************
 * Association service
 **************************************************/

/**
 * \brief Connect to a server
 *
 * \param self the connection object
 * \param error the error code if an error occurs
 * \param hostname the host name or IP address of the server to connect to
 * \param tcpPort the TCP port number of the server to connect to
 */
void
IedConnection_connect(IedConnection self, IedClientError* error, char* hostname, int tcpPort);

/**
 * \brief Close the connection
 *
 * This will close the MMS association and the underlying TCP connection.
 *
 * \param self the connection object
 */
void
IedConnection_close(IedConnection self);

/**
 * \brief get a handle to the underlying MmsConnection
 *
 * Get access to the underlying MmsConnection used by this IedConnection.
 * This can be used to set/change specific MmsConnection parameters.
 *
 * \param self
 *
 * \return the MmsConnection instance used by this IedConnection.
 */
MmsConnection
IedConnection_getMmsConnection(IedConnection self);




/********************************************
 * Reporting services
 ********************************************/

/**
 * \brief Reserve a report control block (RCB)
 *
 * \param self the connection object
 * \param error the error code if an error occurs
 * \param rcbReference object reference of the report control block
 */
void
IedConnection_reserveRCB(IedConnection self, IedClientError* error, char* rcbReference);

/**
 * \brief Release a report control block (RCB)
 *
 * \param self the connection object
 * \param error the error code if an error occurs
 * \param rcbReference object reference of the report control block
 */
void
IedConnection_releaseRCB(IedConnection self, IedClientError* error, char* rcbReference);

typedef void (*ReportCallbackFunction) (void* parameter, ClientReport report);

/**
 * \brief enable a report control block (RCB)
 *
 * It is important that you provide a ClientDataSet instance that is already populated with an MmsValue object
 * of type MMS_STRUCTURE that contains the data set entries as structure elements. This is required because otherwise
 * the report handler is not able to correctly parse the report message from the server.
 *
 * \param connection the connection object
 * \param error the error code if an error occurs
 * \param rcbReference object reference of the report control block
 * \param dataSet a data set instance where to store the retrieved values the data set has to match with the
 *        data set of the RCB
 * \param triggerOptions the options for report triggering. If set to 0 the configured trigger options of
 *        the RCB will not be changed
 * \param callback user provided callback function to be invoked when a report is received.
 * \param callbackParameter user provided parameter that will be passed to the callback function
 *
 */
void
IedConnection_enableReporting(IedConnection self, IedClientError* error, char* rcbReference, ClientDataSet dataSet,
        int triggerOptions, ReportCallbackFunction callback, void* callbackParameter);

/**
 * \brief disable a report control block (RCB)
 *
 * \param self the connection object
 * \param error the error code if an error occurs
 * \param rcbReference object reference of the report control block
 */
void
IedConnection_disableReporting(IedConnection self, IedClientError* error, char* rcbReference);

/**
 * \brief Trigger a general interrogation (GI) report for the specified report control block (RCB)
 *
 * The RCB must have been enabled and GI set as trigger option before this command can be performed.
 *
 * \param connection the connection object
 * \param error the error code if an error occurs
 * \param rcbReference object reference of the report control block
 */
void
IedConnection_triggerGIReport(IedConnection self, IedClientError* error, char* rcbReference);


/**
 * \brief addSubscription service according to IEC 61400-25 (NOT YET IMPLEMENTED)
 *
 * Create a new data set and enable reporting for that data set.
 *
 * \param self the connection object
 * \param error the error code if an error occurs
 * \param fcda the data set members of the newly created data set
 */
ClientDataSet
IedConnection_addSubscription(IedConnection self, IedClientError* error, char* fcda[]);


/****************************************
 * Data model access services
 ****************************************/

/**
 * \brief read a functional constrained data attribute (FCDA) or functional constrained data (FCD).
 *
 * \param self  the connection object to operate on
 * \param error the error code if an error occurs
 * \param object reference of the object/attribute to read
 * \param fc the functional contraint of the data attribute or data object to read
 */
MmsValue*
IedConnection_readObject(IedConnection self, IedClientError* error, char* objectReference, FunctionalConstraint fc);

/**
 * \brief write a functional constrained data attribute (FCDA) or functional constrained data (FCD).
 *
 * \param self  the connection object to operate on
 * \param error the error code if an error occurs
 * \param object reference of the object/attribute to write
 * \param fc the functional contraint of the data attribute or data object to write
 * \param value the MmsValue to write (has to be of the correct type - MMS_STRUCTURE for FCD)
 */
void
IedConnection_writeObject(IedConnection self, IedClientError* error, char* objectReference, FunctionalConstraint fc,
        MmsValue* value);

/****************************************
 * Data set handling
 ****************************************/

/**
 * \brief get data set values from a server device
 *
 * \param connection the connection object
 * \param error the error code if an error occurs
 * \param dataSetReference object reference of the data set
 * \param dataSet a data set instance where to store the retrieved values or NULL if a new instance
 *        shall be created.
 *
 * \return data set instance with retrieved values of NULL if an error occurred.
 */
ClientDataSet
IedConnection_getDataSetValues(IedConnection self, IedClientError* error, char* dataSetReference, ClientDataSet dataSet);

/**
 * \brief create a new data set at the connected server device (NOT YET IMPLEMENTED)
 *
 * This function creates a new data set at the server. The parameter dataSetReference is the name of the new data set
 * to create. It is either in the form LDName/LNodeName.dataSetName or @dataSetName for an association specific data set.
 *
 * The dataSetElements parameter contains a linked list containing the object references of FCDs or FCDAs. The format of
 * this object references is LDName/LNodeName.item(arrayIndex)component[FC].
 *
 * \param connection the connection object
 * \param error the error code if an error occurs
 * \param dataSetReference object reference of the data set
 * \param dataSetElements a list of object references defining the members of the new data set
 *
 */
void
IedConnection_createDataSet(IedConnection self, IedClientError* error, char* dataSetReference, LinkedList /* char* */ dataSetElements);

/**
 * \brief delete a deletable data set at the connected server device (NOT YET IMPLEMENTED)
 *
 * This function deletes a data set at the server. The parameter dataSetReference is the name of the data set
 * to delete. It is either in the form LDName/LNodeName.dataSetName or @dataSetName for an association specific data set.
 *
 *
 * \param connection the connection object
 * \param error the error code if an error occurs
 * \param dataSetReference object reference of the data set
 */
void
IedConnection_deleteDataSet(IedConnection self, IedClientError* error, char* dataSetReference);


/******************************************************
 * Data set object
 *****************************************************/

ClientDataSet
ClientDataSet_create(char* dataSetReference);

void
ClientDataSet_setDataSetValues(ClientDataSet self, MmsValue* dataSetValues);

MmsValue*
ClientDataSet_getDataSetValues(ClientDataSet self);

char*
ClientDataSet_getReference(ClientDataSet self);

int
ClientDataSet_getDataSetSize(ClientDataSet self);

/************************************
 *  Control service functions
 ************************************/

/**
 * \brief control an controllable data object
 *
 * \param self the connection object
 * \param error the error code if an error occurs
 * \param objectReference the object reference of the controllable object
 * \param ctlVal the control value (setpoint)
 * \param operTime the timestamp of the time when the operation will be executed or zero for immediate execution
 * \param test perform a test instead of a real operation (send test flag with operate command) if true
 * \param interlockCheck perform an interlock check before command execution if true
 * \param synchroCheck perform a synchronization check before command execution if true
 *
 */

typedef struct sControlObjectClient* ControlObjectClient;

typedef enum {
	CONTROL_MODEL_STATUS_ONLY,
	CONTROL_MODEL_DIRECT_NORMAL,
	CONTROL_MODEL_SBO_NORMAL,
	CONTROL_MODEL_DIRECT_ENHANCED,
	CONTROL_MODEL_SBO_ENHANCED
} ControlModel;

ControlObjectClient
ControlObjectClient_create(char* objectReference, IedConnection connection);

void
ControlObjectClient_destroy(ControlObjectClient self);

char*
ControlObjectClient_getObjectReference(ControlObjectClient self);

ControlModel
ControlObjectClient_getControlModel(ControlObjectClient self);

bool
ControlObjectClient_operate(ControlObjectClient self, MmsValue* ctlVal, uint64_t operTime);

bool
ControlObjectClient_select(ControlObjectClient self);

bool
ControlObjectClient_selectWithValue(ControlObjectClient self, MmsValue* ctlVal);

bool
ControlObjectClient_cancel(ControlObjectClient self);

void
ControlObjectClient_setLastApplError(ControlObjectClient self, LastApplError lastAppIError);

LastApplError
ControlObjectClient_getLastApplError(ControlObjectClient self);

void
ControlObjectClient_setTestMode(ControlObjectClient self);

void
ControlObjectClient_enableInterlockCheck(ControlObjectClient self);

void
ControlObjectClient_enableSynchroCheck(ControlObjectClient self);

/*************************************
 * Model discovery services
 ************************************/

void
IedConnection_getDeviceModelFromServer(IedConnection self, IedClientError* error);

LinkedList /*<char*>*/
IedConnection_getLogicalDeviceList(IedConnection self, IedClientError* error);

LinkedList /*<char*>*/
IedConnection_getLogicalDeviceDirectory(IedConnection self, IedClientError* error, char* logicalDeviceName);

typedef enum {
    ACSI_CLASS_DATA_OBJECT,
    ACSI_CLASS_DATA_SET,
    ACSI_CLASS_BRCB,
    ACSI_CLASS_URCB,
    ACSI_CLASS_LCB,
    ACSI_CLASS_LOG,
    ACSI_CLASS_SGCB,
    ACSI_CLASS_GoCB,
    ACSI_CLASS_GsCB,
    ACSI_CLASS_MSVCB,
    ACSI_CLASS_USVCB
} ACSIClass;

LinkedList /*<char*>*/
IedConnection_getLogicalNodeVariables(IedConnection self, IedClientError* error,
		char* logicalNodeReference);

LinkedList /*<char*>*/
IedConnection_getLogicalNodeDirectory(IedConnection self, IedClientError* error,
		char* logicalNodeReference, ACSIClass acseClass);

LinkedList /*<char*>*/
IedConnection_getDataDirectory(IedConnection self, IedClientError* error,
		char* dataReference);

LinkedList /*<char*>*/
IedConnection_getDataDirectoryFC(IedConnection self, IedClientError* error,
		char* dataReference);

LastApplError
IedConnection_getLastApplError(IedConnection self);

MmsVariableSpecification*
IedConnection_getVariableSpecification(IedConnection self, IedClientError* error, char* objectReference,
		FunctionalConstraint fc);

/**
 * \brief Create a GOOSE subscriber for the specified GooseControlBlock of the server
 *        (NOT YET IMPLEMENTED)
 *
 * Calling this function with a valid reference for a Goose Control Block (GCB) at the
 * server will create a client data set according to the data set specified in the
 * GCB.
 */
GooseSubscriber
IedConnection_createGooseSubscriber(IedConnection self, char* gooseCBReference);


/**@}*/

void
IedConnectionPrivate_addControlClient(IedConnection self, ControlObjectClient control);

void
IedConnectionPrivate_removeControlClient(IedConnection self, ControlObjectClient control);

#endif /* IEC61850_CLIENT_H_ */
