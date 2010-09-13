
//OpenSCADA system module DAQ.OPC_UA file: mod_daq.h
/***************************************************************************
 *   Copyright (C) 2009 by Roman Savochenko                                *
 *   rom_as@oscada.org, rom_as@fromru.com                                  *
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

#undef _
#define _(mess) mod->I18N(mess)

using std::string;
using std::vector;
using namespace OSCADA;

//*************************************************
//* DAQ modul info!                               *
#define DAQ_ID		"OPC_UA"
#define DAQ_NAME	"OPC UA"
#define DAQ_TYPE	"DAQ"
#define DAQ_SUBVER	VER_CNTR
#define DAQ_MVER	"0.5.0"
#define DAQ_AUTOR	"Roman Savochenko"
#define DAQ_DESCR	"Allow realisation of OPC UA client service."
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
	void vlArchMake( TVal &val );
	void vlGet( TVal &val );
	void vlSet( TVal &val, const TVariant &pvl );

	string	&mNdLst;		//Nodes list

	//Attributes
	TElem	p_el;			//Work atribute elements

	Res	prmRes;
};

//*************************************************
//* OPC_UA::TMdContr                              *
//*************************************************
class TMdContr: public TController
{
    friend class TMdPrm;
    public:
	//Methods
	TMdContr( string name_c, const string &daq_db, ::TElem *cfgelem );
	~TMdContr( );

	string getStatus( );

	long long period( )	{ return mPer; }
	string	cron( )		{ return mSched; }
	int	prior( )	{ return mPrior; }
	double	syncPer( )	{ return mSync; }
	string	endPoint( )	{ return mEndPoint; }
	string	secPolicy( )	{ return mSecPolicy; }
	int	secMessMode( )	{ return mSecMessMode; }
	string	cert( );
	string	pvKey( );
	int	pAttrLim( )	{ return mPAttrLim; }

	void	setEndPoint( const string &iep ){ if( mEndPoint != iep ) { mEndPoint = iep; modif(); } }
	void	setSecPolicy( const string &isp )	{ mSecPolicy = isp; modif(); }
	void	setSecMessMode( int smm )	{ mSecMessMode = smm; modif(); }

	AutoHD<TMdPrm> at( const string &nm )	{ return TController::at(nm); }

	void reqOPC( XMLNode &io );

	Res &nodeRes( )		{ return cntrRes; }

    protected:
	//Methods
	void prmEn( const string &id, bool val );

	void start_( );
	void stop_( );

	bool cfgChange( TCfg &cfg );
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

    private:
	//Data
	class SSess
	{
	    public:
		SSess( )		{ clearFull( ); }
		void clearSess( )	{ sesId = authTkId = 0; sesLifeTime = 1.2e6; }
		void clearFull( )
		{
		    endPoint = servCert = clKey = servKey = "";
		    secPolicy = "None"; secMessMode = 1;
		    secChnl = secToken = reqHndl = 0;
		    sqNumb = 33;
		    sqReqId = 1; 
		    secLifeTime = 0; 
		    sesAccess = 0;
		    clearSess( );
		}

		string		endPoint;
		uint32_t	secChnl;
		uint32_t	secToken;
		uint32_t	sqNumb;
		uint32_t	sqReqId;
		uint32_t	reqHndl;
		int		secLifeTime;
		uint32_t	sesId;
		uint32_t	authTkId;
		long long	sesAccess;
		double		sesLifeTime;
		string		servCert;
		string		secPolicy;
		char		secMessMode;
		string		clKey, servKey;
	};

	//Methods
	TParamContr *ParamAttach( const string &name, int type );
	static void *Task( void *icntr );

	//Attributes
	Res	en_res;		//Resource for enable params
	int	&mPrior;	//Process task priority
	double	&mSync;		//Synchronization inter remote station: attributes list update.
	string	&mSched,	//Calc schedule
		&mAddr,		//Transport device address
		&mEndPoint,	//Endpoint URL
		&mSecPolicy;	//Security policy
	int	&mSecMessMode,	//Security policy mode
		&mPAttrLim;	//Parameter attributes number limit
	long long mPer;

	bool	prc_st,		//Process task active
		endrun_req,	//Request to stop of the Process task
		mPCfgCh;	//Parameter's configuration is changed

	vector< AutoHD<TMdPrm> > p_hd;

	SSess	sess;

	string	mBrwsVar;

	ResString	acq_err;

	double		tm_gath;	//Gathering time
	float		tmDelay;	//Delay time for next try connect

	uint32_t	servSt;
	Res		cntrRes;
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
