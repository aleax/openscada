
//OpenSCADA system module DAQ.DAQGate file: daq_gate.h
/***************************************************************************
 *   Copyright (C) 2007-2014 by Roman Savochenko                           *
 *   rom_as@fromru.com                                                     *
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
#include <ttipdaq.h>
#include <tparamcontr.h>

#include <string>
#include <vector>

#undef _
#define _(mess) mod->I18N(mess)

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

	TMdPrm &owner( );

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
	TMdPrm( string name, TTipParam *tp_prm );
	~TMdPrm( );

	string stats( )		{ return mStats; }
	string prmAddr( )	{ return mPrmAddr; }

	void setStats( const string &vl );
	void setPrmAddr( const string &vl )	{ mPrmAddr = vl; }

	void enable( );
	void disable( );

	TElem &elem( )		{ return p_el; }
	TMdContr &owner( );

	//Attributes
	unsigned char isPrcOK	: 1;
	unsigned char isEVAL	: 1;
	unsigned char isSynced	: 1;

    protected:
	//Methods
	void load_( );				//Load parameter
	void save_( );				//Save parameter
	void sync( );				//Synchronize parameter
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

    private:
	//Methods
	void postEnable( int flag );
	TVal* vlNew( );
	void vlGet( TVal &vo );
	void vlSet( TVal &vo, const TVariant &vl, const TVariant &pvl );
	void vlArchMake( TVal &val );

	//Attributes
	TElem	p_el;				//Work atribute elements
	string	mStats;				//Allowed stations list'
	TCfg	&mPrmAddr;			//Interstation parameter's address
};

//******************************************************
//* TMdContr                                           *
//******************************************************
class TMdContr: public TController
{
    public:
	//Methods
	TMdContr( string name_c, const string &daq_db, ::TElem *cfgelem );
	~TMdContr( );

	string	getStatus( );

	double	period( )	{ return mPer; }
	string	cron( )		{ return mSched; }
	int	prior( )	{ return mPrior; }
	double	syncPer( )	{ return mSync; }
	double	restDtTm( )	{ return mRestDtTm; }

	string	catsPat( );
	//string	prm2path( );

	AutoHD<TMdPrm> at( const string &nm )	{ return TController::at(nm); }

	//> Request to OpenSCADA control interface
	int cntrIfCmd( XMLNode &node );

    protected:
	//Methods
        void load_( );
	void enable_( );
	void disable_( );
	void start_( );
	void stop_( );
	void cntrCmdProc( XMLNode *opt );	//Control interface command process
	bool cfgChange( TCfg &cfg );

    private:
	//Data
	class StHd
	{
	    public:
	    StHd( ) : cntr(0) { lstMess.clear(); }

	    float cntr;
	    map<string, time_t> lstMess;
	};

	//Methods
	TParamContr *ParamAttach( const string &name, int type );
	static void *Task( void *icntr );

	//Attributes
	Res	enRes;				//Resource for enable params and request to remote OpenSCADA station
	TCfg	&mSched,			//Calc schedule
		&mMessLev;			//Messages level for gather
	double	&mSync,				//Synchronization inter remote OpenSCADA station:
						//configuration update, attributes list update, local and remote archives sync.
		&mRestDtTm;			//Restore data maximum length time (hour)
	int64_t	&mPerOld,			//Acquisition task (seconds)
		&mRestTm,			//Restore timeout in s
		&mPrior;			//Process task priority

	bool	prcSt,				//Process task active
		call_st,			//Calc now stat
		endrunReq;			//Request to stop of the Process task
	int8_t	alSt;				//Alarm state
	vector< pair<string,StHd> > mStatWork;	//Work stations and it status

	double	mPer, tmGath;			//Gathering time
};

//******************************************************
//* TTpContr                                           *
//******************************************************
class TTpContr: public TTipDAQ
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
