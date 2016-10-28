
//OpenSCADA system module DAQ.MMS file: module.h
/***************************************************************************
 *   Copyright (C) 2013-2016 by Roman Savochenko, <rom_as@oscada.org>      *
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
#include <ttypedaq.h>
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
	TMdPrm( string name, TTypeParam *tp_prm );
	~TMdPrm( );

	string varList( )			{ return cfg("VAR_LS").getS(); }
	void setVarList( const string &vl )	{ cfg("VAR_LS").setS(vl); }

	TElem &elem( )				{ return p_el; }

	void enable( );
	void disable( );

	string attrPrc( );
	void getVals( );
	void setEval( );

	TMdContr &owner( ) const;

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
	struct StackTp {
	    StackTp( TArrayObj *iarr, MMS::XML_N *ivl, int iInPos ) : arr(iarr), vl(ivl), inPos(iInPos) { }

	    TArrayObj *arr;
	    MMS::XML_N *vl;
	    int inPos;
	};

	//Methods
	TMdContr( string name_c, const string &daq_db, ::TElem *cfgelem );
	~TMdContr( );

	string	getStatus( );

	bool	connOK( )	{ return startStat() && tmDelay <= 0; }
	int64_t	period( )	{ return mPer; }
	string	cron( )		{ return mSched; }
	int	prior( )	{ return mPrior; }
	int	restTm( )	{ return mRestTm; }
	int	syncPer( )	{ return mSync; }
	string	addr( ) const	{ return mAddr; }

	AutoHD<TMdPrm> at( const string &nm )	{ return TController::at(nm); }

	// MMS Client methods
	uint16_t COTP_DestTSAP( ){ return cfg("COTP_DestTSAP").getI(); }
	void regVar( const string &vl, const string &opts );

	void reqService( MMS::XML_N &io );
	void protIO( MMS::XML_N &io );
	int messIO( const char *oBuf, int oLen, char *iBuf = NULL, int iLen = 0 );
	void debugMess( const string &mess );

	string getNameList( const string &domain = "" );

    protected:
	//Methods
	void prmEn( TMdPrm *prm, bool val );

	void enable_( );
	void disable_( );
	void start_( );
	void stop_( );

	bool cfgChange( TCfg &co, const TVariant &pc );
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

	class NamesCacheEl
	{
	    public:
		NamesCacheEl( time_t itm, const string &inms ) : tm(itm), nms(inms)	{ }
		NamesCacheEl( ) : tm(0)	{ }
		time_t	tm;
		string	nms;
	};

	//Methods
	TParamContr *ParamAttach( const string &name, int type );
	static void *Task( void *icntr );

	//Attributes
	ResMtx	enRes, cntrRes;	//Resource for enable params, controller DAQ API
	TCfg	&mSched,	//Schedule
		&mPrior,	//Process task priority
		&mRestTm,	//Restore timeout in s
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

	map<string, NamesCacheEl> namesCache;
};

//*************************************************
//* ModMMS::TTpContr                              *
//*************************************************
class TTpContr: public TTypeDAQ
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
