/*
 *  iec61850_server.h
 *
 *  IEC 61850 server API for libiec61850.
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
 *
 */

#ifndef IED_SERVER_API_H_
#define IED_SERVER_API_H_

/** \defgroup server_api_group IEC 61850 server API
 *  @{
 */

#include "mms_server.h"
#include "model.h"

/**
 * An opaque handle for an IED server instance
 */
typedef struct sIedServer* IedServer;

/**
 * An opaque handle for a client connection
 */
typedef struct sClientConnection* ClientConnection;

/**
 * \brief Create a new IedServer instance
 *
 * \param iedModel reference to the IedModel data structure to be used as IEC 61850 model of the device
 *
 * \return the newly generated IedServer instance
 */
IedServer
IedServer_create(IedModel* iedModel);

/**
 * \brief Destroy an IedServer instance and release all resources (memory, TCP sockets)
 *
 * \param self the instance of IedServer to operate on.
 */
void
IedServer_destroy(IedServer self);

/**
 * \brief Start handling client connections
 *
 * \param self the instance of IedServer to operate on.
 * \param tcpPort the TCP port the server is listening
 */
void
IedServer_start(IedServer self, int tcpPort);

/**
 * \brief Stop handling client connections
 *
 * \param self the instance of IedServer to operate on.
 */
void
IedServer_stop(IedServer self);

/**
 * \brief Check if IedServer instance is listening for client connections
 *
 * \param self the instance of IedServer to operate on.
 *
 * \return true if IedServer instance is listening for client connections
 */
bool
IedServer_isRunning(IedServer self);

MmsServer
IedServer_getMmsServer(IedServer self);

/**
 * \brief get the IsoServer instance for this IedServer object
 *
 * \param self the instance of IedServer to operate on.
 *
 * \return the IsoServer instance of this IedServer object
 */
IsoServer
IedServer_getIsoServer(IedServer self);

/**
 * \brief get the peer address of this connection as string
 *
 * Note: the returned string is only valid as long as the client connection exists. It is save to use
 * the string inside of the connection indication callback function.
 *
 * \param self the ClientConnection instance
 * \return peer address as C string.
 */
char*
ClientConnection_getPeerAddress(ClientConnection self);

/**
 * \brief User provided callback function that is invoked whenever a new client connects or an existing connection is closed
 *        or detected as lost.
 *
 * \param self the instance of IedServer where the connection event occured.
 * \param connection the new or closed client connect object
 * \param connected true if a new connection is indicated, false if the connection has been closed or detected as lost.
 * \param parameter a user provided parameter
 */
typedef void (*IedConnectionIndicationHandler) (IedServer self, ClientConnection connection, bool connected, void* parameter);

/**
 * \brief set a callback function that will be called on connection events (open or close).
 *
 * \param self the instance of IedServer to operate on.
 * \param handler the user provided callback function
 * \param parameter a user provided parameter that is passed to the callback function.
 */
void
IedServer_setConnectionIndicationHandler(IedServer self, IedConnectionIndicationHandler handler, void* parameter);

/**
 * \brief Get data attribute value
 *
 * Get the MmsValue object of an MMS Named Variable that is part of the device model.
 * You should not manipulate the received object directly. Instead you should use
 * the IedServer_updateValue method.
 *
 * \param self the instance of IedServer to operate on.
 * \param dataAttribute the data attribute handle
 *
 * \return MmsValue object of the MMS Named Variable or NULL if the value does not exist.
 */
MmsValue*
IedServer_getAttributeValue(IedServer self, DataAttribute* dataAttribute);


/**
 * \brief Update the MmsValue object of an IEC 61850 data attribute.
 *
 * The data attribute handle of type DataAttribute* are imported with static_model.h.
 * You should use this function instead of directly operating on the MmsValue instance
 * that is hold by the MMS server. Otherwise the IEC 61850 server is not aware of the
 * changed value and will e.g. not generate a report.
 *
 * \param self the instance of IedServer to operate on.
 * \param dataAttribute the data attribute handle
 * \param value MmsValue object used to update the value cached by the server.
 *
 * \return MmsValue object of the MMS Named Variable or NULL if the value does not exist.
 */
void
IedServer_updateAttributeValue(IedServer self, DataAttribute* dataAttribute, MmsValue* value);

/**
 * \brief Inform the IEC 61850 stack that the quality of a data attribute has changed.
 *
 * The data attribute handle of type DataAttribute* are imported with static_model.h.
 * This function is required to trigger reports that have been configured with
 * QUALITY CHANGE trigger condition.
 *
 * \param self the instance of IedServer to operate on.
 * \param dataAttribute the data attribute handle
 */
void
IedServer_attributeQualityChanged(IedServer self, DataAttribute* dataAttribute);

/**
 * \brief Control model callback to perform the static tests (optional).
 *
 * User provided callback function for the control model. It will be invoked after
 * a control operation has been invoked by the client. This callback function is
 * intended to perform the static tests. It should check if the interlock conditions
 * are met if the interlockCheck parameter is true.
 *
 * \param parameter the parameter that was specified when setting the control handler
 * \param ctlVal the control value of the control operation.
 * \param test indicates if the operate request is a test operation
 * \param interlockCheck the interlockCheck parameter provided by the client
 * \param connection the connection object of the client connection that invoked the control operation
 *
 * \return true if the static tests had been successful, false otherwise
 */
typedef bool (*ControlPerformCheckHandler) (void* parameter, MmsValue* ctlVal, bool test, bool interlockCheck,
        ClientConnection connection);

/**
 * \brief Control model callback to perform the dynamic tests (optional).
 *
 * User provided callback function for the control model. It will be invoked after
 * a control operation has been invoked by the client. This callback function is
 * intended to perform the dynamic tests. It should check if the synchronization conditions
 * are met if the synchroCheck parameter is set to true.
 *
 * \param parameter the parameter that was specified when setting the control handler
 * \param ctlVal the control value of the control operation.
 * \param test indicates if the operate request is a test operation
 * \param synchroCheck the synchroCheck parameter provided by the client
 *
 * \return true if the dynamic tests had been successful, false otherwise
 */
typedef bool (*ControlWaitForExecutionHandler) (void* parameter, MmsValue* ctlVal, bool test, bool synchroCheck);

/**
 * \brief Control model callback to actually perform the control operation.
 *
 * User provided callback function for the control model. It will be invoked when
 * a control operation happens (Oper). Here the user should perform the control operation
 * (e.g. by setting an digital output or switching a relay).
 *
 * \param parameter the parameter that was specified when setting the control handler
 * \param ctlVal the control value of the control operation.
 * \param test indicates if the operate request is a test operation
 *
 * \return true if the control action bas been successful, false otherwise
 */
typedef bool (*ControlHandler) (void* parameter, MmsValue* ctlVal, bool test);

/**
 * \brief Set control handler for controllable data object
 *
 * This functions sets a user provided control handler for a data object. The data object
 * has to be an instance of a controllable CDC (Common Data Class) like e.g. SPC, DPC or APC.
 * The control handler is a callback function that will be called by the IEC server when a
 * client invokes a control operation on the data object.
 *
 * \param self the instance of IedServer to operate on.
 * \param node the controllable data object handle
 * \param handler a callback function of type ControlHandler
 * \param parameter a user provided parameter that is passed to the control handler.
 */
void
IedServer_setControlHandler(IedServer self, DataObject* node, ControlHandler handler, void* parameter);

/**
 * \brief Set a handler for a controllable data object to perform operative tests
 *
 * This functions sets a user provided handler that should perform the operative tests for a control operation.
 * Setting this handler is not required. If not set the server assumes that the checks will always be successful.
 * The handler has to return true upon a successful test of false if the test fails. In the later case the control
 * operation will be aborted.
 *
 * \param self the instance of IedServer to operate on.
 * \param node the controllable data object handle
 * \param handler a callback function of type ControlHandler
 * \param parameter a user provided parameter that is passed to the control handler.
 *
 */
void
IedServer_setPerformCheckHandler(IedServer self, DataObject* node, ControlPerformCheckHandler handler, void* parameter);

/**
 * \brief Set a handler for a controllable data object to perform dynamic tests
 *
 * This functions sets a user provided handler that should perform the dynamic tests for a control operation.
 * Setting this handler is not required. If not set the server assumes that the checks will always be successful.
 * The handler has to return true upon a successful test of false if the test fails. In the later case the control
 * operation will be aborted. If this handler is set than the server will start a new thread before calling the
 * handler. This thread will also be used to execute the ControlHandler.
 *
 * \param self the instance of IedServer to operate on.
 * \param node the controllable data object handle
 * \param handler a callback function of type ControlHandler
 * \param parameter a user provided parameter that is passed to the control handler.
 *
 */
void
IedServer_setWaitForExecutionHandler(IedServer self, DataObject* node, ControlWaitForExecutionHandler handler, void* parameter);

/**
 * \brief Lock the MMS server data model.
 *
 * Client requests will be postponed until the lock is removed
 *
 * \param self the instance of IedServer to operate on.
 */
void
IedServer_lockDataModel(IedServer self);

/**
 * \brief Unlock the MMS server data model and process pending client requests.
 *
 * \param self the instance of IedServer to operate on.
 */
void
IedServer_unlockDataModel(IedServer self);

/**
 * \brief Enable all GOOSE control blocks.
 *
 * This will set the GoEna attribute of all configured GOOSE control blocks
 * to true. If this method is not called at the startup or reset of the server
 * then configured GOOSE control blocks keep inactive until a MMS client enables
 * them by writing to the GOOSE control block.
 *
 * \param self the instance of IedServer to operate on.
 */
void
IedServer_enableGoosePublishing(IedServer self);

/**
 * \brief callback handler to monitor client access to data attributes
 *
 * User provided callback function to observe (monitor) MMS client access to
 * IEC 61850 data attributes. The application can install the same handler
 * multiple times and distinguish data attributes by the dataAttribute parameter.
 *
 * \param the data attribute that has been written by an MMS client.
 * \param connection the connection object of the client connection that invoked the write operation
 */
typedef void (*AttributeChangedHandler) (DataAttribute* dataAttribute, ClientConnection connection);

/**
 * \brief Install an observer for a data attribute.
 *
 * This instructs the server to monitor write attempts by MMS clients to specific
 * data attributes. If a successful write attempt happens the server will call
 * the provided callback function to inform the application. This can be used to
 * monitor important configuration values.
 *
 * \param self the instance of IedServer to operate on.
 * \param dataAttribute the data attribute to monitor
 * \param handler the callback function that is invoked if a client has written to
 *        the monitored data attribute.
 */
void
IedServer_observeDataAttribute(IedServer self, DataAttribute* dataAttribute,
        AttributeChangedHandler handler);

/**@}*/

#endif /* IED_SERVER_API_H_ */
