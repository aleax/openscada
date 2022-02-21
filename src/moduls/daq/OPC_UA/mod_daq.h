
//OpenSCADA module DAQ.OPC_UA file: mod_daq.h
/***************************************************************************
 *   Copyright (C) 2009-2022 by Roman Savochenko, <roman@oscada.org>       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef MOD_OPC_UA_H
#define MOD_OPC_UA_H

#include <stdint.h>

#include <string>
#include <vector>

#include <tsys.h>

#include "libOPC_UA/libOPC_UA.h"

#undef _
#define _(mess) mod->I18N(mess).c_str()
#undef trS
#define trS(mess) mod->I18N(mess,mess_PreSave)

using std::string;
using std::vector;
using namespace OSCADA;
using namespace OPC;

//*************************************************
//* DAQ modul info!                               *
#define DAQ_ID		"OPC_UA"
#define DAQ_NAME	trS("Client OPC-UA")
#define DAQ_TYPE	SDAQ_ID
#define DAQ_SUBVER	SDAQ_VER
#define DAQ_MVER	"2.6.5"
#define DAQ_AUTOR	trS("Roman Savochenko")
#define DAQ_DESCR	trS("Provides OPC-UA client service implementation.")
#define DAQ_LICENSE	"GPL2"
//*************************************************

namespace OPC_UA
{
//*************************************************
//* OPC_UA::TMdPrm                                *
//*************************************************
class TMdContr;

class TMdPrm : public TParamContr
{
    public:
	//Methods
	TMdPrm( string name, TTypeParam *tp_prm );
	~TMdPrm( );

	TCntrNode &operator=( const TCntrNode &node );

	bool isStd( ) const;
	bool isLogic( ) const;

	void loadDATA( bool incl = false );
	void enable( );
	void disable( );

	void upValStd( );
	void upValLog( bool first, bool last, double frq );

	string ndList( )			{ return cfg("ND_LS").getS(); }
	void setNdList( const string &vl )	{ cfg("ND_LS").setS(vl); }

	TElem *dynElCntr( )	{ return &pEl; }
	TElem &elem( )		{ return pEl; }

	TVariant objFuncCall( const string &id, vector<TVariant> &prms, const string &user_lang );

	AutoHD<TMdPrm> at( const string &nm )	{ return TParamContr::at(nm); }

	TMdContr &owner( ) const;

	//Attributes
	MtxString acqErr;

    protected:
	//Methods
	void load_( );
	void save_( );
	void cntrCmdProc( XMLNode *opt );
	void setType( const string &tpId );

    private:
	//Methods
	void postEnable( int flag );
	void postDisable( int flag );

	void vlGet( TVal &vo );
	void vlSet( TVal &vo, const TVariant &vl, const TVariant &pvl );
	void vlArchMake( TVal &vo );

	void loadIO( bool force = false );
	void saveIO( );

	//Attributes
	TElem	pEl;			//Work atribute elements

	// Logical type by template
	//Data
	//***************************************************
	//* Logical type parameter's context                *
	class TLogCtx : public TPrmTempl::Impl
	{
	    public:
	    //Methods
	    TLogCtx( TCntrNode *iobj, const string &name );

	    bool lnkInit( int num, bool toRecnt = false );
	    bool lnkActive( int num );
	    TVariant lnkInput( int num );
	    bool lnkOutput( int num, const TVariant &vl );

	    void cleanLnks( bool andFunc = false );

	    //bool cntrCmdProc( XMLNode *opt, const string &pref = "/cfg" );

	    //Attributes
	    bool chkLnkNeed;	// Check lnks need flag
	    int	idFreq, idStart, idStop, idErr, idSh, idNm, idDscr;	//Fixed system attributes identifiers

	    protected:
	    //Methods
	    string lnkHelp( );
	};

	//Attributes
	TLogCtx	*lCtx;
};

//*************************************************
//* OPC_UA::TMdContr                              *
//*************************************************
class TTpContr;

class TMdContr: public TController, public Client
{
    friend class TMdPrm;
    public:
	//Methods
	TMdContr( string name_c, const string &daq_db, ::TElem *cfgelem );
	~TMdContr( );

	string	getStatus( );

	int64_t	period( )	{ return mPer; }
	string	cron( )		{ return mSched; }
	int	prior( )	{ return mPrior; }
	int	syncPer( )	{ return mSync; }
	string	endPoint( )	{ return mEndP; }
	string	secPolicy( )	{ return mSecPol; }
	int	secMessMode( )	{ return mSecMessMode; }
	string	cert( )		{ return mCert; }
	string	pvKey( )	{ return mPvKey; }
	string	authData( );
	string	epParse( string *uri = NULL );

	void	setEndPoint( const string &iep )	{ mEndP = iep; }
	void	setSecPolicy( const string &isp )	{ mSecPol = isp; }
	void	setSecMessMode( int smm )		{ mSecMessMode = smm; }

	TVariant getValMIt( unsigned mItId, uint32_t *st = NULL );

	AutoHD<TMdPrm> at( const string &nm )	{ return TController::at(nm); }

	// OPC_UA Client methods
	string applicationUri( );
	string productUri( );
	string applicationName( );
	string sessionName( )	{ return "OpenSCADA station "+SYS->id(); }
	bool connect( int8_t est = -1 );
	void reqService( XML_N &io );
	void protIO( XML_N &io );
	int messIO( const char *oBuf, int oLen, char *iBuf = NULL, int iLen = 0, int time = 0 );
	void debugMess( const string &mess );

	TTpContr &owner( ) const;

	// Values processing
	TVariant getVal( const string &iaddr, MtxString &err );
	bool setVal( const TVariant &ivl, const string &iaddr, MtxString &err );

    protected:
	//Methods
	void load_( );
	void enable_( );
	void disable_( );
	void start_( );
	void stop_( );

	bool cfgChange( TCfg &co, const TVariant &pc );
	void prmEn( TMdPrm *prm, bool val );

	// Service
	void postDisable( int flag );		//Delete all DB if flag 1

	void cntrCmdProc( XMLNode *opt );	//Control interface command process

    private:
	//Methods
	TParamContr *ParamAttach( const string &name, int type );
	static void *Task( void *icntr );
	void setCntrDelay( const string &err );

	//Attributes
	ResMtx	enRes;
	ResRW	resOPC;		//Request and API-stack resource
	TCfg	&mSched,	//Schedule
		&mPrior,	//Process task priority
		&mSync,		//Synchronization inter remote station: attributes list update.
		&mEndP,		//Target endpoint
		&mSecPol,	//Security policy
		&mSecMessMode,	//Security policy mode
		&mCert,		//Client certificate
		&mPvKey,	//Client certificate's private key
		&mAuthUser, &mAuthPass;	//Auth user and password
	int64_t	&restTm;	//Restore timeout in seconds
	char	&mUseRead;	//Use Read function
	int64_t	mPer;

	bool	prcSt,		//Process task active
		callSt;		//Calc now stat
	int8_t	alSt;		//Alarm state

	AutoHD<TTransportOut>	tr;
	vector< AutoHD<TMdPrm> > pHd;

	string		mBrwsVar;

	MtxString	acqErr;
	map<string, SecuritySetting> epLst;

	float		tmDelay;	//Delay time for next try connect

	uint32_t	servSt;
};

//*************************************************
//* OPC_UA::TTpContr                              *
//*************************************************
class TTpContr: public TTypeDAQ
{
    public:
	//Methods
	TTpContr( string name );
	~TTpContr( );

	TElem	&prmIOE( )	{ return elPrmIO; }

    protected:
	//Methods
	void postEnable( int flag );

	void load_( );
	void save_( );

	bool redntAllow( )	{ return true; }

    private:
	//Methods
	TController *ContrAttach( const string &name, const string &daq_db );

	//Attributes
	TElem	elPrmIO;
};

extern TTpContr *mod;

} //End namespace OPC_UA

#endif //MOD_OPC_UA_H
