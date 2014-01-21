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
typedef struct sClientReportControlBlock* ClientReportControlBlock;

typedef struct {
    int ctlNum;
    int error;
    int addCause;
} LastApplError;

typedef enum {
    /* general errors */

    /** No error occurred - service request has been successful */
    IED_ERROR_OK = 0,

    /** The service request can not be executed because the client is not yet connected */
    IED_ERROR_NOT_CONNECTED = 1,

    /** Connect service not execute because the client is already connected */
    IED_ERROR_ALREADY_CONNECTED = 2,

    /** The service request can not be executed caused by a loss of connection */
    IED_ERROR_CONNECTION_LOST = 3,

    /* client side errors */

    /** API function has been called with an invalid argument */
    IED_ERROR_USER_PROVIDED_INVALID_ARGUMENT = 10,

    IED_ERROR_ENABLE_REPORT_FAILED_DATASET_MISMATCH = 11,

    /** The object provided object reference is invalid (there is a syntactical error). */
    IED_ERROR_OBJECT_REFERENCE_INVALID = 12,

    /* service error - error reported by server */

    /** The communication to the server failed with a timeout */
    IED_ERROR_TIMEOUT = 20,

    /** The server rejected the access to the requested object/service due to access control */
    IED_ERROR_ACCESS_DENIED = 21,

    /** The server reported that the requested object does not exist */
    IED_ERROR_OBJECT_DOES_NOT_EXIST = 22,

    /** The server reported that the requested object already exists */
    IED_ERROR_OBJECT_EXISTS = 23,

    /* unknown error */
    IED_ERROR_UNKNOWN = 99
} IedClientError;

typedef enum {
    REASON_NOT_INCLUDED = 0,
    REASON_DATA_CHANGE = 1,
    REASON_QUALITY_CHANGE = 2,
    REASON_DATA_UPDATE = 3,
    REASON_INTEGRITY = 4,
    REASON_GI = 5,
    REASON_UNKNOWN = 6
} ReasonForInclusion;

typedef enum
{
    IED_STATE_IDLE,
    IED_STATE_CONNECTED,
    IED_STATE_CLOSED
} IedConnectionState;

/* Element encoding mask values for ClientReportControlBlock */
#define RCB_ELEMENT_RPT_ID            1
#define RCB_ELEMENT_RPT_ENA           2
#define RCB_ELEMENT_RESV              4 /* only available in unbuffered RCBs */
#define RCB_ELEMENT_DATSET            8
#define RCB_ELEMENT_CONF_REV         16
#define RCB_ELEMENT_OPT_FLDS         32
#define RCB_ELEMENT_BUF_TM           64
#define RCB_ELEMENT_SQ_NUM          128
#define RCB_ELEMENT_TRG_OPS         256
#define RCB_ELEMENT_INTG_PD         512
#define RCB_ELEMENT_GI             1024
#define RCB_ELEMENT_PURGE_BUF      2048 /* only available in buffered RCBs */
#define RCB_ELEMENT_ENTRY_ID       4096 /* only available in buffered RCBs */
#define RCB_ELEMENT_TIME_OF_ENTRY  8192 /* only available in buffered RCBs */
#define RCB_ELEMENT_RESV_TMS      16384 /* only available in buffered RCBs */
#define RCB_ELEMENT_OWNER         32768

/**************************************************
 * ClientReportControlBlock class
 **************************************************/

ClientReportControlBlock
ClientReportControlBlock_create(char* objectReference);

void
ClientReportControlBlock_destroy(ClientReportControlBlock self);

char*
ClientReportControlBlock_getObjectReference(ClientReportControlBlock self);

bool
ClientReportControlBlock_isBuffered(ClientReportControlBlock self);

char*
ClientReportControlBlock_getRptId(ClientReportControlBlock self);

void
ClientReportControlBlock_setRptId(ClientReportControlBlock self, char* rptId);

bool
ClientReportControlBlock_getRptEna(ClientReportControlBlock self);

void
ClientReportControlBlock_setRptEna(ClientReportControlBlock self, bool rptEna);

bool
ClientReportControlBlock_getResv(ClientReportControlBlock self);

void
ClientReportControlBlock_setResv(ClientReportControlBlock self, bool resv);

char*
ClientReportControlBlock_getDataSetReference(ClientReportControlBlock self);

void
ClientReportControlBlock_setDataSetReference(ClientReportControlBlock self, char* dataSetReference);

uint32_t
ClientReportControlBlock_getConvRev(ClientReportControlBlock self);

MmsValue* /* <MMS_BIT_STRING(6)> */
ClientReportControlBlock_getOptFlds(ClientReportControlBlock self);

void
ClientReportControlBlock_setOptFlds(ClientReportControlBlock self, MmsValue* optFlds);

uint32_t
ClientReportControlBlock_getBufTm(ClientReportControlBlock self);

void
ClientReportControlBlock_setBufTm(ClientReportControlBlock self, uint32_t bufTm);

uint16_t
ClientReportControlBlock_getSqNum(ClientReportControlBlock self);

int
ClientReportControlBlock_getTrgOps(ClientReportControlBlock self);

void
ClientReportControlBlock_setTrgOps(ClientReportControlBlock self, int trgOps);

uint32_t
ClientReportControlBlock_getIntgPd(ClientReportControlBlock self);

void
ClientReportControlBlock_setIntgPd(ClientReportControlBlock self, uint32_t intgPd);

bool
ClientReportControlBlock_getGI(ClientReportControlBlock self);

void
ClientReportControlBlock_setGI(ClientReportControlBlock self, bool gi);

bool
ClientReportControlBlock_getPurgeBuf(ClientReportControlBlock self);

void
ClientReportControlBlock_setPurgeBuf(ClientReportControlBlock self, bool purgeBuf);

int16_t
ClientReportControlBlock_getResvTms(ClientReportControlBlock self);

void
ClientReportControlBlock_setResvTms(ClientReportControlBlock self, int16_t resvTms);

MmsValue* /* <MMS_OCTET_STRING> */
ClientReportControlBlock_getEntryId(ClientReportControlBlock self);

void
ClientReportControlBlock_setEntryId(ClientReportControlBlock self, MmsValue* entryId);

uint64_t
ClientReportControlBlock_getEntryTime(ClientReportControlBlock self);

MmsValue* /* <MMS_OCTET_STRING> */
ClientReportControlBlock_getOwner(ClientReportControlBlock self);

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

IedConnectionState
IedConnection_getState(IedConnection self);

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

/**
 * \brief Read access to attributes of a report control block (RCB) at the connected server
 *
 * The requested RCB has to be specified by its object reference. E.g.
 *
 * "simpleIOGernericIO/LLN0.RP.EventsRCB01"
 *
 * or
 *
 * "simpleIOGenericIO/LLN0.BR.EventsBRCB01"
 *
 * Report control blocks have either "RP" or "BR" as part of their name following the logical node part.
 * "RP" is part of the name of unbuffered RCBs whilst "BR" is part of the name of buffered RCBs.
 *
 * This function is used to perform the actual read service. To access the received values the functions
 * of ClientReportControlBlock have to be used.
 *
 * If called with a NULL argument for the updateRcb parameter a new ClientReportControlBlock instance is created
 * and populated with the values received by the server. It is up to the user to release this object by
 * calling the ClientReportControlBlock_destroy function when the object is no longer needed.
 *
 * If called with a reference to an existing ClientReportControlBlock instance
 *
 * Note: This function maps to a single MMS read request to retrieve the complete RCB at once.
 *
 * \param connection the connection object
 * \param error the error code if an error occurs
 * \param rcbReference object reference of the report control block
 * \param updateRcb a reference to an existing ClientReportControlBlock instance or NULL
 *
 * \return new ClientReportControlBlock instance or the instance provided by the user with
 *         the updateRcb parameter.
 */
ClientReportControlBlock
IedConnection_getRCBValues(IedConnection self, IedClientError* error, char* rcbReference,
        ClientReportControlBlock updateRcb);

/**
 * \brief Write access to attributes of a report control block (RCB) at the connected server
 *
 * The requested RCB has to be specified by its object reference (see also IedConnection_getRCBValues).
 * The object reference for the referenced RCB is contained in the provided ClientReportControlBlock instance.
 *
 * The parametersMask parameter specifies which attributes of the remote RCB have to be set by this request.
 * You can specify multiple attributes by ORing the defined bit values.
 *
 * The singleRequest parameter specifies the mapping to the corresponding MMS write request. Standard compliant
 * servers should accept both variants. But some server accept only one variant. Then the value of this parameter
 * will be of relevance.
 *
 * \param connection the connection object
 * \param error the error code if an error occurs
 * \param rcb object reference of the ClientReportControlBlock instance that actually holds the parameter
 *            values to be written.
 * \param parametersMask specifies the parameters contained in the setRCBValues request.
 * \param singleRequest specifies if the setRCBValues services is mapped to a single MMS write request containing
 *        multiple variables or to multiple MMS write requests.
 */
void
IedConnection_setRCBValues(IedConnection self, IedClientError* error, ClientReportControlBlock rcb,
        uint32_t parametersMask, bool singleRequest);

typedef void (*ReportCallbackFunction) (void* parameter, ClientReport report);


/**
 * \brief Install a report handler function for the specified report control block (RCB)
 *
 * It is important that you provide a ClientDataSet instance that is already populated with an MmsValue object
 * of type MMS_STRUCTURE that contains the data set entries as structure elements. This is required because otherwise
 * the report handler is not able to correctly parse the report message from the server.
 *
 * This function will replace a formerly set report handler function for the specified RCB.
 *
 * \param connection the connection object
 * \param rcbReference object reference of the report control block
 * \param callback user provided callback function to be invoked when a report is received.
 * \param callbackParameter user provided parameter that will be passed to the callback function
 * \param dataSet a data set instance where to store the retrieved values the data set has to match with the
 *        data set of the RCB
 */
void
IedConnection_installReportHandler(IedConnection self, char* rcbReference, ReportCallbackFunction handler,
        void* handlerParameter, ClientDataSet dataSet);

/**
 * \brief uninstall a report handler function for the specified report control block (RCB)
 */
void
IedConnection_uninstallReportHandler(IedConnection self, char* rcbReference);

/**
 * \brief enable a report control block (RCB)
 *
 * This is a convenience function that unifies the setRCBValues and installReportHandler functions.
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

/****************************************
 * Access to received reports
 ****************************************/

/**
 * \brief return the data set associated with this ClientReport object
 *
 * \param self the ClientReport object handled to the report handler function
 * \return data set object
 */
ClientDataSet
ClientReport_getDataSet(ClientReport self);

/**
 * \brief return reference (name) of the server RCB associated with this ClientReport object
 *
 * \param self the ClientReport object handled to the report handler function
 * \return report control block reference as string
 */
char*
ClientReport_getRcbReference(ClientReport self);

/**
 * \brief get the reason code (reason for inclusion) for a specific report data set element
 *
 * \param self the ClientReport object handled to the report handler function
 * \param elementIndex index of the data set element (starting with 0)
 *
 * \return reason code for the inclusion of the specified element
 */
ReasonForInclusion
ClientReport_getReasonForInclusion(ClientReport self, int elementIndex);

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
IedConnection_readDataSetValues(IedConnection self, IedClientError* error, char* dataSetReference, ClientDataSet dataSet);

/**
 * \brief create a new data set at the connected server device
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
 * \brief delete a deletable data set at the connected server device
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


/**
 * \brief returns the object references of the elements of a data set
 *
 * Thereturn value contains a linked list containing the object references of FCDs or FCDAs. The format of
 * this object references is LDName/LNodeName.item(arrayIndex)component[FC].
 *
 * \param connection the connection object
 * \param error the error code if an error occurs
 * \param dataSetReference object reference of the data set
 * \param isDeletable this is an output parameter indicating that the requested data set is deletable by clients.
 *                    If this information is not required a NULL pointer can be used.
 *
 * \return LinkedList containing the data set elements as char* strings.
 */
LinkedList /* <char*> */
IedConnection_getDataSetDirectory(IedConnection self, IedClientError* error, char* dataSetReference, bool* isDeletable);

/********************************************************
 * Data set object (local representation of a data set)
 *******************************************************/

/**
 * \brief destroy an ClientDataSet instance. Has to be called by the application.
 *
 * Note: A ClientDataSet cannot be created directly by the application but only by the IedConnection_readDataSetValues
 *       function. Therefore there is no public ClientDataSet_create function.
 *
 * \param self the ClientDataSet instance
 */
void
ClientDataSet_destroy(ClientDataSet self);

/**
 * \brief get the data set values locally stored in the ClientDataSet instance.
 *
 * This function returns a pointer to the locally stored MmsValue instance of this
 * ClientDataSet instance. The MmsValue instance is of type MMS_ARRAY and contains one
 * array element for each data set member.
 * Note: This call does not invoke any interaction with the associated server. It will
 * only provide access to already stored value. To update the values with the current values
 * of the server the IecConnection_readDataSetValues function has to be called!
 *
 * \param self the ClientDataSet instance
 *
 * \return the locally stored data set values as MmsValue object of type MMS_ARRAY.
 */
MmsValue*
ClientDataSet_getValues(ClientDataSet self);

/**
 * \brief Get the object reference of the data set.
 *
 * \param self the ClientDataSet instance
 *
 * \return the object reference of the data set.
 */
char*
ClientDataSet_getReference(ClientDataSet self);

/**
 * \brief get the size of the data set (number of members)
 *
 * \param self the ClientDataSet instance
 *
 * \return the number of member contained in the data set.
 */
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

#endif /* IEC61850_CLIENT_H_ */
