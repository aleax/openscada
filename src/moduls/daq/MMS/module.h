
//OpenSCADA system module DAQ.MMS file: module.h
/***************************************************************************
 *   Copyright (C) 2013-2014 by Roman Savochenko                           *
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

#ifndef MODULE_H
#define MODULE_H

#include <string>
#include <vector>

#include <tcontroller.h>
#include <ttipdaq.h>
#include <tparamcontr.h>

#include "libMMS/libMMS.h"

#undef _
#define _(mess) mod->I18N(mess)

using std::string;
using std::vector;
using namespace OSCADA;

namespace ModMMS
{

//*************************************************
//* ModMMS::TMdPrm                                *
//*************************************************
class TMdContr;

class TMdPrm : public TParamContr
{
    public:
	//Methods
	TMdPrm( string name, TTipParam *tp_prm );
	~TMdPrm( );

	string varList( )			{ return cfg("VAR_LS").getS(); }
	void setVarList( const string &vl )	{ cfg("VAR_LS").setS(vl); }

	TElem &elem( )				{ return p_el; }

	void enable( );
	void disable( );

	string attrPrc( );
	void getVals( );
	void setEval( );

	TMdContr &owner( );

    protected:
	//Methods
	void load_( );
	void save_( );

    private:
	//Methods
	void postEnable( int flag );
	void cntrCmdProc( XMLNode *opt );
	void vlArchMake( TVal &val );
	void vlGet( TVal &vo );
	void vlSet( TVal &vo, const TVariant &vl, const TVariant &pvl );

	//Attributes
	TElem	p_el;	//Work atribute elements
};

//*************************************************
//* ModMMS::TMdContr                              *
//*************************************************
class TMdContr: public TController, public MMS::Client
{
    friend class TMdPrm;
    public:
	//Data
	struct StackTp
	{
	    StackTp( TArrayObj *iarr, MMS::XML_N *ivl, int iInPos ) : arr(iarr), vl(ivl), inPos(iInPos) { }

	    TArrayObj *arr;
	    MMS::XML_N *vl;
	    int inPos;
	};

	//Methods
	TMdContr( string name_c, const string &daq_db, ::TElem *cfgelem );
	~TMdContr( );

	string	getStatus( );

	int64_t	period( )	{ return mPer; }
	string	cron( )		{ return mSched; }
	int	prior( )	{ return mPrior; }
	double	syncPer( )	{ return mSync; }
	string	addr( )		{ return mAddr; }

	AutoHD<TMdPrm> at( const string &nm )	{ return TController::at(nm); }

	// MMS Client methods
	uint16_t COTP_DestTSAP( ){ return cfg("COTP_DestTSAP").getI(); }
	void regVar( const string &vl, const string &opts );

	void reqService( MMS::XML_N &io );
	void protIO( MMS::XML_N &io );
	int messIO( const char *obuf, int len_ob, char *ibuf = NULL, int len_ib = 0 );
	void debugMess( const string &mess );

    protected:
	//Methods
	void prmEn( TMdPrm *prm, bool val );

	void enable_( );
	void disable_( );
	void start_( );
	void stop_( );

	bool cfgChange( TCfg &cfg );
	void cntrCmdProc( XMLNode *opt );       //Control interface command process

    private:
	//Data
	class VarStr
	{
	    public:
	    VarStr( ) : single(false), div(0)	{ }
	    VarStr( const TVariant &iv, bool iSingle = false, char iDiv = 0 ) : val(iv), single(iSingle), div(iDiv)	{ }

	    TVariant	val;
	    unsigned	single	: 1;
	    unsigned	div	: 7;
	};

	//Methods
	TParamContr *ParamAttach( const string &name, int type );
	static void *Task( void *icntr );

	//Attributes
	pthread_mutex_t	enRes, cntrRes, dataRes;	//Resource for enable params, controller DAQ API and data
	TCfg	&mSched,	//Schedule
		&mPrior,	//Process task priority
		&mSync,		//Synchronization inter remote station: attributes list update.
		&mAddr,		//MMS server address
		&mVarsRdReq;	//Variables into single request
	int64_t	mPer;

	bool	prcSt,		//Process task active
		callSt,		//Calc now stat
		isReload;
	int8_t	alSt;		//Alarm state

	vector< AutoHD<TMdPrm> > pHD;

	MtxString acq_err;

	double	tmGath;		//Gathering time
	float	tmDelay;	//Delay time for next try connect

	AutoHD<TTransportOut>	tr;
	map<string, VarStr>	mVars;
};

//*************************************************
//* ModMMS::TTpContr                              *
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
	void cntrCmdProc( XMLNode *opt );       //Control interface command process
};

extern TTpContr *mod;

} //End namespace ModMMS

#endif //MODULE_H
