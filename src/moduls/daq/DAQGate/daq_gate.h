
//OpenSCADA module DAQ.DAQGate file: daq_gate.h
/***************************************************************************
 *   Copyright (C) 2007-2022 by Roman Savochenko, <roman@oscada.org>       *
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

#ifndef DAQ_GATE_H
#define DAQ_GATE_H

#include <tcontroller.h>
#include <ttypedaq.h>
#include <tparamcontr.h>

#include <string>
#include <vector>

#undef _
#define _(mess) mod->I18N(mess).c_str()
#undef trS
#define trS(mess) mod->I18N(mess,mess_PreSave)

using std::string;
using std::vector;
using namespace OSCADA;

namespace DAQGate
{

//******************************************************
//* TMdVl                                              *
//******************************************************
class TMdPrm;

class TMdVl : public TVal
{
    public:
	TMdVl( )	{ }

	TMdPrm &owner( ) const;

    protected:
	//Methods
	void cntrCmdProc( XMLNode *opt );
};

//******************************************************
//* TMdPrm                                             *
//******************************************************
class TMdContr;

class TMdPrm : public TParamContr
{
    friend class TMdContr;
    public:
	//Methods
	TMdPrm( string name, TTypeParam *tp_prm );
	~TMdPrm( );

	TCntrNode &operator=( const TCntrNode &node );

	string stats( )		{ return mStats; }
	string prmAddr( )	{ return mPrmAddr; }

	void setStat( const string &vl, bool toRemove = false );
	void setPrmAddr( const string &vl )	{ mPrmAddr = vl; }

	AutoHD<TMdPrm> at( const string &nm )	{ return TParamContr::at(nm); }

	void enable( );
	void disable( );

	TElem &elem( )		{ return pEl; }
	TMdContr &owner( ) const;

	//Attributes
	unsigned char isPrcOK	: 1;
	unsigned char isEVAL	: 1;
	unsigned char isSynced	: 1;

    protected:
	//Methods
	void load_( );				//Load the parameter
	void loadIO( );
	void save_( );				//Save the parameter
	void sync( );				//Synchronize parameter
	void cntrCmdProc( XMLNode *opt );	//Control interface command process
	bool cfgChange( TCfg &co, const TVariant &pc );

    private:
	//Methods
	void postEnable( int flag );
	TVal* vlNew( );
	void vlGet( TVal &vo );
	void vlSet( TVal &vo, const TVariant &vl, const TVariant &pvl );
	void vlArchMake( TVal &val );

	//Attributes
	TElem	pEl;				//Work atribute elements
	TCfg	&mPrmAddr,			//Interstation parameter's address
		&mStats;			//Allowed stations list'
};

//******************************************************
//* TMdContr                                           *
//******************************************************
class TMdContr: public TController
{
    friend class TMdPrm;
    public:
	//Methods
	TMdContr( string name_c, const string &daq_db, ::TElem *cfgelem );
	~TMdContr( );

	string	getStatus( );

	double	period( )	{ return mPer; }
	string	cron( )		{ return mSched; }
	int	prior( )	{ return mPrior; }
	int	syncPer( )	{ return mSync; }
	double	restDtTm( )	{ return mRestDtTm; }

	string	catsPat( );
	void messSet( const string &mess, int lev, const string &type2Code = "OP", const string &prm = "", const string &cat = "" );

	AutoHD<TMdPrm> at( const string &nm )	{ return TController::at(nm); }

	// Request to OpenSCADA control interface
	int cntrIfCmd( XMLNode &node, bool noConnect = false );

    protected:
	//Methods
        void load_( );
	void enable_( );
	void disable_( );
	void start_( );
	void stop_( );
	void cntrCmdProc( XMLNode *opt );	//Control interface command process
	bool cfgChange( TCfg &co, const TVariant &pc );

	void prmEn( TMdPrm *prm, bool val );
	void sync( bool onlyPrmLs = false );

    private:
	//Data
	class StHd
	{
	    public:
	    StHd( ) : cntr(0), numR(0), numRA(0), numW(0), numRM(0), numWM(0) { lstMess.clear(); }

	    float cntr;
	    map<string, map<string,string> > asynchWrs;	//Asynchronous writings list: [prm][attr][vl]
							//   for the ready requests that is: ["<ReadyReqs>"][req][cmd]
	    map<string, TMess::SRec>	lstMess;
	    string err;

	    ResMtx reqM, aWrRes, dataRes;

	    float numR, numRA, numW, numRM, numWM;
	};
	class SPrmsStack
	{
	    public:
	    SPrmsStack( XMLNode	*ind, int ipos, const AutoHD<TMdPrm> &iprm, const string &ipath ) : nd(ind), pos(ipos), prm(iprm), path(ipath) { }

	    XMLNode		*nd;
	    int			pos;
	    AutoHD<TMdPrm>	prm;
	    string		path;
	};

	//Methods
	TParamContr *ParamAttach( const string &name, int type );
	static void *Task( void *icntr );

	//Attributes
	ResMtx	enRes;				//Resource for enable params and request to remote OpenSCADA station
	TCfg	&mSched,			//Calc schedule
		&mStat,				//Remote station
		&mMessLev;			//Messages level for gather
	double	&mRestDtTm;			//Restore data maximum length time (hour)
	int64_t	&mSync,				//Synchronization inter remote OpenSCADA station:
						//configuration update, attributes list update, local and remote archives sync.
		&mRestTm,			//Restore timeout in s
		&mPrior;			//Process task priority
	char	&mAsynchWr,			//Asynchronous write
		&mAllowToDelPrmAttr,		//Allow automatic remove parameters and attributes
		&mPlaceCntrToVirtPrm;		//Placing different controllers to the different virtual parameters

	bool	prcSt,				//Process task active
		callSt,				//Call now stat
		syncSt,				//Sync start
		syncForce,			//Force sync in the task
		endrunReq;			//Request to stop of the Process task
	map<string, StHd> mStatWork;		//Work stations and it status

	vector< AutoHD<TMdPrm> > pHd;

	MtxString curPat;

	double	mPer;
};

//******************************************************
//* TTpContr                                           *
//******************************************************
class TTpContr: public TTypeDAQ
{
    public:
	//Methods
	TTpContr( string name );
	~TTpContr( );

	bool redntAllow( )	{ return true; }

    protected:
	//Methods
	void postEnable( int flag );
	void load_( );

    private:
	//Methods
	TController *ContrAttach( const string &name, const string &daq_db );
};

extern TTpContr *mod;

} //End namespace

#endif //DAQ_GATE_H
