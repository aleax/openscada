
//OpenSCADA system module DAQ.BFN file: mod_BFN.h
/***************************************************************************
 *   Copyright (C) 2010 by Roman Savochenko                                *
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

#ifndef MOD_BFN_H
#define MOD_BFN_H

#include <string>
#include <vector>

#include <tcontroller.h>
#include <ttipdaq.h>
#include <tparamcontr.h>

#undef _
#define _(mess) mod->I18N(mess)

using std::string;
using std::vector;
using namespace OSCADA;

namespace ModBFN
{

//*************************************************
//* ModBFN::TMdPrm                                *
//*************************************************
class TMdContr;

class TMdPrm : public TParamContr
{
    friend class TMdContr;
    public:
	//Methods
	TMdPrm( string name, TTipParam *tp_prm );
	~TMdPrm( );

	TElem &elem( )		{ return p_el; }

	void enable( );
	void disable( );

	TMdContr &owner( );

	void setEval( );

    protected:
	//Methods
	void load_( );
	void save_( );

    private:
	//Methods
	void postEnable( int flag );
	void vlArchMake( TVal &val );
	void vlGet( TVal &val );

	//Attributes
	int	curAlrmsId;
	TElem	p_el;			//Work atribute elements

	ResString acq_err;
};

//*************************************************
//* ModBFN::TMdContr                              *
//*************************************************
class TMdContr: public TController
{
    friend class TMdPrm;
    public:
	//Methods
	TMdContr( string name_c, const string &daq_db, ::TElem *cfgelem );
	~TMdContr( );

	string getStatus( );

	int64_t	period( )	{ return mPer; }
	string	cron( )		{ return cfg("SCHEDULE").getS(); }
	int64_t	prior( )	{ return mPrior; }
	string	addr( )		{ return cfg("ADDR").getS(); }
	double	syncPer( )	{ return mSync; }

	AutoHD<TMdPrm> at( const string &nm )	{ return TController::at(nm); }

	void reqBFN( XMLNode &io );

	string passPrefSOAP( const string &ndName );

    protected:
	//Methods
	void prmEn( const string &id, bool val );

	void enable_( );
	void start_( );
	void stop_( );

	void cntrCmdProc( XMLNode *opt );	//Control interface command process

    private:
	//Methods
	TParamContr *ParamAttach( const string &name, int type );
	static void *Task( void *icntr );

	//Attributes
	Res	en_res, req_res;// Resource for enable params
	int64_t	&mPrior;	// Process task priority
	double	&mSync;		// Synchronization inter remote station: attributes list update.
	int64_t	mPer;

	bool	prc_st,		// Process task active
		acq_st,		// Acquisition now
		endrun_req;	// Request to stop of the Process task
	int8_t	alSt;		// Alarm state

	ResString	acq_err;

	vector< AutoHD<TMdPrm> > p_hd;

	double	tm_gath;	// Gathering time
};

//*************************************************
//* ModBFN::TTpContr                              *
//*************************************************
class TTpContr: public TTipDAQ
{
    public:
	//Data
	class AlrmSymb
	{
	    public:
		AlrmSymb( ) : text(""), code(0)						{ }
		AlrmSymb( const string &itxt, unsigned icod ) : text(itxt), code(icod)	{ }

		string	 text;
		unsigned code;
	};
	//Methods
	TTpContr(string name);
	~TTpContr( );

	string symbDB( );
	string getSymbolCode( const string &id );
	AlrmSymb getSymbolAlarm( const string &id );

	void setSymbDB( const string &idb );

    protected:
	//Methods
	void postEnable( int flag );

	void load_( );
	void save_( );

	bool redntAllow( )	{ return true; }

	void cntrCmdProc( XMLNode *opt );	//Control interface command process

    private:
	//Methods
	TController *ContrAttach(const string &name, const string &daq_db);

	map<unsigned,string>	mSymbCode;
	map<unsigned,AlrmSymb>	mSymbAlrm;

	TElem	symbCode_el,	//> Make Symbols of codes container structure
		symbAlrm_el;	//> Make Symbols of alarms container structure
};

extern TTpContr *mod;

} //End namespace ModBFN

#endif //MOD_BFN_H
