
//OpenSCADA system module DAQ.OPC_UA file: mod_daq.h
/***************************************************************************
 *   Copyright (C) 2009-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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
#define _(mess) mod->I18N(mess)

using std::string;
using std::vector;
using namespace OSCADA;
using namespace OPC;

//*************************************************
//* DAQ modul info!                               *
#define DAQ_ID		"OPC_UA"
#define DAQ_NAME	_("OPC UA")
#define DAQ_TYPE	SDAQ_ID
#define DAQ_SUBVER	SDAQ_VER
#define DAQ_MVER	"0.6.2"
#define DAQ_AUTOR	_("Roman Savochenko")
#define DAQ_DESCR	_("OPC UA client service implementation.")
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
	TMdPrm( string name, TTipParam *tp_prm );
	~TMdPrm( );

	string ndList( )			{ return cfg("ND_LS").getS(); }
	void setNdList( const string &vl )	{ cfg("ND_LS").setS(vl); }

	TElem &elem( )		{ return p_el; }

	void enable( );
	void disable( );

	TMdContr &owner( );

	string attrPrc( );

	Res &nodeRes( )		{ return prmRes; }

    protected:
	//Methods
	void load_( );
	void save_( );

    private:
	//Methods
	void postEnable( int flag );
	void cntrCmdProc( XMLNode *opt );
	void vlGet( TVal &vo );
	void vlSet( TVal &vo, const TVariant &vl, const TVariant &pvl );
	void vlArchMake( TVal &vo );

	//Attributes
	TElem	p_el;			//Work atribute elements

	Res	prmRes;
};

//*************************************************
//* OPC_UA::TMdContr                              *
//*************************************************
class TMdContr: public TController, public Client
{
    friend class TMdPrm;
    public:
	//Methods
	TMdContr( string name_c, const string &daq_db, ::TElem *cfgelem );
	~TMdContr( );

	string getStatus( );

	int64_t	period( )	{ return mPer; }
	string	cron( )		{ return mSched; }
	int	prior( )	{ return mPrior; }
	double	syncPer( )	{ return mSync; }
	string	endPoint( )	{ return mEndP; }
	string	secPolicy( )	{ return mSecPol; }
	int	secMessMode( )	{ return mSecMessMode; }
	string	cert( )		{ return mCert; }
	string	pvKey( )	{ return mPvKey; }
	string	authData( );
	int	pAttrLim( )	{ return mPAttrLim; }
	string	epParse( string *uri = NULL );

	void	setEndPoint( const string &iep )	{ mEndP = iep; }
	void	setSecPolicy( const string &isp )	{ mSecPol = isp; }
	void	setSecMessMode( int smm )		{ mSecMessMode = smm; }

	AutoHD<TMdPrm> at( const string &nm )	{ return TController::at(nm); }

	void reqService( XML_N &io );

	Res &nodeRes( )		{ return cntrRes; }

	// OPC_UA Client methods
	string sessionName( )	{ return "OpenSCADA station "+SYS->id(); }
	void protIO( XML_N &io );
	int messIO( const char *obuf, int len_ob, char *ibuf = NULL, int len_ib = 0 );
	void debugMess( const string &mess );

    protected:
	//Methods
	void prmEn( const string &id, bool val );

	void enable_( );
	void disable_( );
	void start_( );
	void stop_( );

	bool cfgChange( TCfg &cfg );
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

    private:
	//Methods
	TParamContr *ParamAttach( const string &name, int type );
	static void *Task( void *icntr );

	//Attributes
	Res	enRes, cntrRes;	//Resource for enable params
	TCfg	&mSched,	//Schedule
		&mPrior,	//Process task priority
		&mSync,		//Synchronization inter remote station: attributes list update.
		&mEndP,		//Target endpoint
		&mSecPol,	//Security policy
		&mSecMessMode,	//Security policy mode
		&mCert,		//Client certificate
		&mPvKey,	//Client certificate's private key
		&mAuthUser, &mAuthPass;	//Auth user and password
	int64_t	&mPAttrLim;	//Parameter attributes number limit
	int64_t	mPer;

	bool	prcSt,		//Process task active
		callSt,		//Calc now stat
		mPCfgCh;	//Parameter's configuration is changed
	int8_t	alSt;		//Alarm state

	AutoHD<TTransportOut>	tr;
	vector< AutoHD<TMdPrm> > p_hd;

	string		mBrwsVar;

	ResString	acq_err;
	map<string, SecuritySetting> epLst;

	double		tm_gath;	//Gathering time
	float		tmDelay;	//Delay time for next try connect

	uint32_t	servSt;
};

//*************************************************
//* OPC_UA::TTpContr                              *
//*************************************************
class TTpContr: public TTipDAQ
{
    public:
	//Methods
	TTpContr( string name );
	~TTpContr( );

    protected:
	//Methods
	void postEnable( int flag );

	void load_( );
	void save_( );

	bool redntAllow( )	{ return true; }

    private:
	//Methods
	TController *ContrAttach( const string &name, const string &daq_db );
};

extern TTpContr *mod;

} //End namespace OPC_UA

#endif //MOD_OPC_UA_H
