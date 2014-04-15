
//OpenSCADA MMS(IEC-9506) implementation library file: libMMS.h
/******************************************************************************
 *   Copyright (C) 2014 by Roman Savochenko				      *
 *   rom_as@oscada.org, rom_as@fromru.com				      *
 *									      *
 *   This library is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU Lesser General Public License as	      *
 *   published by the Free Software Foundation; version 3 of the License.     *
 *									      *
 *   This library is distributed in the hope that it will be useful,	      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of	      *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the	      *
 *   GNU Lesser General Public License for more details.		      *
 *									      *
 *   You should have received a copy of the GNU Lesser General Public License *
 *   along with this library; if not, write to the			      *
 *   Free Software Foundation, Inc.,					      *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.		      *
 ******************************************************************************/

#ifndef LIBMMS_H
#define LIBMMS_H

#include <stdint.h>

#include <string>
#include <vector>
#include <map>

using std::string;
using std::vector;
using std::pair;
using std::map;

namespace MMS
{

//Constants
#define MMS_TPKT_Version		3

enum COTP_TPS	{ COTP_CR = 0xE0, COTP_CC = 0xD0, COTP_DT = 0xF0 };
enum COTP_Class	{ COTP_NoExplFlCntr = 0x01, COTP_ExtForm = 0x02, COTP_LastDtUnit = 0x80 };
enum COTP_OPTS	{ COTP_O_SzTPDU = 0xC0, COTP_O_SrcTSAP = 0xC1, COTP_O_DstTSAP = 0xC2 };
namespace SupportOpts { enum e { str1 = 0, str2, vnam, valt, vadr, vsca, tpy, vlis, real, cei }; };
namespace SupportServs { enum e { status = 0, getNameList, identify, rename, read, write, getVariableAccessAttributes, defineNamedVariable,
			defineScatteredAccess, getScatteredAccessAttributes, deleteVariableAccess, defineNamedVariableList,
			getNamedVariableListAttributes, deleteNamedVariableList, defineNamedType, getNamedTypeAttributes,
			deleteNamedType, input, output, takeControl, relinquishControl, defineSemaphore, deleteSemaphore,
			reportSemaphoreStatus, reportPoolSemaphoreStatus, reportSemaphoreEntryStatus, initiateDownloadSequence,
			downloadSegment, terminateDownloadSequence, initiateUploadSequence, uploadSegment, terminateUploadSequence,
			requestDomainDownload, requestDomainUpload, loadDomainContent, storeDomainContent, deleteDomain,
			getDomainAttributes, createProgramInvocation, deleteProgramInvocation, start, stop, resume, reset,
			kill, getProgramInvocationAttributes, obtainFile, defineEventCondition, deleteEventCondition,
			getEventConditionAttributes, reportEventConditionStatus, alterEventConditionMonitoring, triggerEvent,
			defineEventAction, deleteEventAction, getEventActionAttributes, reportActionStatus, defineEventEnrollment,
			deleteEventEnrollment, alterEventEnrollment, reportEventEnrollmentStatus, getEventEnrollmentAttributes,
			acknowledgeEventNotification, getAlarmSummary, getAlarmEnrollmentSummary, readJournal, writeJournal,
			initializeJournal, reportJournalStatus, createJournal, deleteJournal, getCapabilityList, fileOpen,
			fileRead, fileClose, fileRename, fileDelete, fileDirectory, unsolicitedStatus, informationReport,
			eventNotification, attachToEventCondition, attachToSemaphore, conclude, cancel }; };
enum MMS_PDU_TAG{ MMS_ConfirmedReq = 0xA0, MMS_ConfirmedResp = 0xA1, MMS_ConfirmedErr = 0xA2,
		  MMS_Unconfirmed = 0xA3, MMS_Reject = 0xA4, MMS_CancelReq = 0xA5, MMS_CancelResp = 0xA6, MMS_CancelErr = 0xA7,
		  MMS_InitiateReq = 0xA8, MMS_initiateResp = 0xA9, MMS_initiateErr = 0xAA,
		  MMS_concludeReq = 0xAB, MMS_concludeResp = 0xAC, MMS_concludeErr = 0xAD };
enum MMS_ConfServ { Conf_Identify = 0x82, Conf_getNameList = 0xA1, Conf_IdentifyResp = 0xA2, Conf_Read = 0xA4, Conf_Write = 0xA5,
		    Conf_getVariableAccessAttributes = 0xA6, Conf_getCapabilityList = 0xBF47 };
enum MMS_ObjClass { OCL_NmVar = 0x00, OCL_ScattAccess = 0x01, OCL_NmVarLs = 0x02, OCL_Domain = 0x09, OCL_ProgInvoc = 0x10 };
enum MMS_VarTps	{ VT_Error = 0x80, VT_Array = 0x81, VT_Struct = 0x82, VT_Bool = 0x83, VT_BitString = 0x84, VT_Int = 0x85, VT_UInt = 0x86,
		  VT_Float = 0x87, VT_OctString = 0x89, VT_VisString = 0x8A, VT_TimeOfDay = 0x8C, VT_BCD = 0x8D, VT_BoolArr = 0x8E };

//* External functions                        */
extern int64_t curTime( );
extern string int2s( int val );
extern string uint2s( unsigned val );
extern string ll2s( int64_t val );
extern string r2s( double val, int prec = 15, char tp = 'g' );
extern string strParse( const string &path, int level, const string &sep, int *off = NULL, bool mergeSepSymb = false );
extern string strMess( const char *fmt, ... );
extern void setBS( string &vl, uint32_t n );
extern bool tryBS( const string &vl, uint32_t n );

//*************************************************
//* MMSError					  *
//*************************************************
class Error
{
    public:
	//Methods
	Error( const char *fmt, ... );
	Error( int cod, const char *fmt, ... );

	//Attributes
	int	cod;
	string	mess;
};

//*************************************************
//* XML_N					  *
//*************************************************
class XML_N
{
    public:
	//Methods
	XML_N( const string &name = "" ) : mName(name), mText(""), mParent(NULL)	{  }
	XML_N( const XML_N &nd );
	~XML_N( )				{ clear(); }

	XML_N	&operator=( const XML_N &prm );

	string	name( ) const			{ return mName; }
	XML_N*	setName( const string &s )	{ mName = s; return this; }

	string	text( bool childs = false, bool recursive = false ) const;
	XML_N*	setText( const string &s, bool childs = false );

	void	attrList( vector<string> &list ) const;
	XML_N*	attrDel( const string &name );
	void	attrClear( );
	string	attr( const string &name, bool caseSens = true ) const;
	XML_N*	setAttr( const string &name, const string &val );

	XML_N*	clear( );

	bool	childEmpty( ) const		{ return mChildren.empty(); }
	unsigned childSize( ) const		{ return mChildren.size(); }
	void	childAdd( XML_N *nd );
	XML_N*	childAdd( const string &name = "" );
	int	childIns( unsigned id, XML_N *nd );
	XML_N*	childIns( unsigned id, const string &name = "" );
	void	childDel( const unsigned id );
	void	childDel( XML_N *nd );
	void	childClear( const string &name = "" );
	XML_N*	childGet( const int, bool noex = false ) const;
	XML_N*	childGet( const string &name, const int numb = 0, bool noex = false ) const;
	XML_N*	childGet( const string &attr, const string &name, bool noex = false ) const;
	XML_N*	getElementBy( const string &attr, const string &val );

	XML_N*	parent( )			{ return mParent; }
	XML_N*	root( );

    private:
	//Attributes
	string	mName;
	string	mText;
	vector<XML_N*>			mChildren;
	vector<pair<string,string> >	mAttr;
	XML_N	*mParent;
};

//*************************************************
//* Protocol MMS Core				  *
//*************************************************
class Core
{
    public:
	//Methods
	Core( );
	~Core( );

	// Main variables
	virtual void debugMess( const string &mess ) { }
	virtual int COTP_SzTPDU( )	{ return -1; }

	// Decoding process
	//----------------------------------------------------
	static const char *iVal( const string &buf, int &off, char vSz );
	static uint32_t iN( const string &rb, int &off, uint8_t vSz );
	//  ASN.1 (MMS) process
	static int ASN_i( const string &buf, int &off, int offLim = -1 );
	static uint16_t ASN_iTAG( const string &buf, int off );
	static uint32_t ASN_iN( const string &buf, int &off, int sz = -1 );
	static double ASN_iR( const string &buf, int &off, int sz = -1 );
	static string ASN_iS( const string &buf, int &off, int sz = -1 );
	static string ASN_iBS( const string &buf, int &off, int sz = -1, char *unUsBits = NULL );
	static void ASN_iAccessResult( const string &buf, int &off, int sz, XML_N &io );
	static void ASN_iTypeSpec( const string &buf, int &off, int sz, XML_N &io );

	// Encoding process
	//----------------------------------------------------
	static void oN( string &buf, uint32_t val, uint8_t sz, int off = -1 );
	//  ASN.1 (MMS) encoding
	static void ASN_o( string &buf, uint16_t tag, uint32_t sz = 0 );
	static int ASN_oC( string &buf, uint16_t tag, int off = -1 );
	static void ASN_oN( string &buf, uint8_t tag, uint32_t val = 0, uint8_t szMin = 1 );
	static void ASN_oR( string &buf, uint8_t tag, double val, char exp = 8 );	//Exponent possible also 11
	static void ASN_oS( string &buf, uint8_t tag, const string &vl );
	static void ASN_oBS( string &buf, uint8_t tag, const string &vl, char unUsBits = 0 );
	static void ASN_oNmObj( string &buf, uint8_t tag, const string &vl, const string &domain = "" );

	//Attributes
	map<uint8_t, vector<string> > RejectErrs, ConfirmErrs;
};

//*************************************************
//* Protocol MMS client part			  *
//*************************************************
class Client: public Core
{
    public:
	//Methods
	Client( );
	~Client( );

	// Main variables
	virtual uint16_t COTP_DestTSAP( ){ return 0x200; }
	string callParameterCBB( )	{ return m_callParameterCBB; }
	string callServicesSupported( )	{ return m_callServicesSupported; }

	void setCallParameterCBB( const string &val );
	void setCallServicesSupported( const string &val );

	// State
	string idVName( )		{ return id_vName; }
	string idModelName( )		{ return id_modelName; }
	string idRev( )			{ return id_rev; }
	string parameterCBB( )		{ return m_parameterCBB; }
	string servicesSupported( )	{ return m_servicesSupported; }

	void reset( );

	// External imlementations
	virtual int	messIO( const char *obuf, int len_ob, char *ibuf = NULL, int len_ib = 0 ) = 0;

	// Main call methods
	virtual void	protIO( XML_N &io );
	virtual void	reqService( XML_N &io );

    private:
	//Attributes
	bool	isInitiated;
	uint32_t mInvokeID;

	// Identify
	string	id_vName, id_modelName, id_rev,
		m_callParameterCBB, m_callServicesSupported,
		m_parameterCBB, m_servicesSupported;
};

} //End namespace MMS

#endif //LIBMMS
