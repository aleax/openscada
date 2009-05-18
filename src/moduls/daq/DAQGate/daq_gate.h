
//OpenSCADA system module DAQ.DAQGate file: daq_gate.h
/***************************************************************************
 *   Copyright (C) 2007-2009 by Roman Savochenko                           *
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
    public:
	//Methods
	TMdPrm( string name, TTipParam *tp_prm );
	~TMdPrm( );

	string cntrAdr( )	{ return mCntrAdr; }

	void setCntrAdr( const string &vl );

	void enable( );
	void disable( );

	void update( );				//Update parameter

	TElem &elem( )		{ return p_el; }
	TMdContr &owner( );

    protected:
	//Methods
	void load_( );				//Synchronize parameter
	void save_( );				//Save blocking
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

    private:
	//Methods
	void postEnable( int flag );
	TVal* vlNew( );
	void vlSet( TVal &val, const TVariant &pvl );
	void vlArchMake( TVal &val );

	//Attributes
	TElem	p_el;				//Work atribute elements
	string	mCntrAdr;			//Parameter's remote controller address'
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

	string getStatus( );

	double	period( )	{ return vmax(mPer,0.1); }
	int	prior( )	{ return mPrior; }
	double	syncPer( )	{ return mSync; }
	double	restDtTm( )	{ return mRestDtTm; }

	AutoHD<TMdPrm> at( const string &nm )	{ return TController::at(nm); }

	void prmEn( const string &id, bool val );

	//> Request to OpenSCADA control interface
	int cntrIfCmd( XMLNode &node, bool strongSt = false );

    protected:
	//Methods
	void enable_( );
	void disable_( );
	void start_( );
	void stop_( );
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

    private:
	//Methods
	TParamContr *ParamAttach( const string &name, int type );
	static void *Task( void *icntr );

	//Attributes
	Res	enRes;				//Resource for enable params and request to remote OpenSCADA station
	double	&mPer,				//Acquisition task (seconds)
		&mSync,				//Synchronization inter remote OpenSCADA station:
						//configuration update, attributes list update, local and remote archives sync.
		&mRestDtTm;			//Restore data maximum length time (hour)
	int	&mRestTm,			//Restore timeout in s
		&mPrior;			//Process task priority
	string	&mStations,			//Remote stations list
		&mContrPrm;			//Transported remote cotrollers and parameters list


	bool	prcSt,				//Process task active
		endrunReq;			//Request to stop of the Process task
	vector< AutoHD<TMdPrm> > pHd;
	map<string,float> mStatWork;		//Work stations and it status

	pthread_t procPthr;			//Process task thread

	double	tmGath;				//Gathering time
};

//******************************************************
//* TTpContr                                           *
//******************************************************
class TTpContr: public TTipDAQ
{
    public:
	//Data
	class RemHost
	{
	    public:
		//Methods
		RemHost( const string &ist, const string &itransp, const string &iaddr,
			const string &iuser, const string &ipass ) :
		    stat(ist), transp(itransp), addr(iaddr), user(iuser), pass(ipass),
		    ses_id(-1), link_ok(false) { }

		//Attributes
		string	stat;		//External station
		string	transp;		//Connect transport
		string	addr;		//External host address
		string	user;		//External host user
		string	pass;		//External host password
		int	ses_id;		//Session ID
		bool	link_ok;	//Link OK
	};

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
	string optDescr( );

	//Attributes
	vector<RemHost>	openHosts;
};

extern TTpContr *mod;

} //End namespace

#endif //DAQ_GATE_H
