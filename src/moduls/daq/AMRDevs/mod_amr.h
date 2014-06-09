
//OpenSCADA system module DAQ.AMRDevs file: mod_tmpl.h
/***************************************************************************
 *   Copyright (C) 2010-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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

#ifndef MOD_AMR_H
#define MOD_AMR_H

#include <stdint.h>

#include <string>
#include <vector>

#include <tcontroller.h>
#include <ttipdaq.h>
#include <tparamcontr.h>

//#include "da.h"

#undef _
#define _(mess) mod->I18N(mess)

using std::string;
using std::vector;
using namespace OSCADA;

namespace AMRDevs
{

//*************************************************
//* AMRDevs::TMdPrm                               *
//*************************************************
class TMdContr;

class TMdPrm : public TParamContr
{
    public:
	//Methods
	TMdPrm( string name, TTipParam *tp_prm );
	~TMdPrm( );

	TElem &elem( )		{ return els; }

	void enable( );
	void disable( );

	TMdContr &owner( );

	//Attributes
	TElem	els;		//Work atribute elements
	void	*extPrms;
	pthread_mutex_t	dataM;
	MtxString	mErr;
	vector<string>	als;
	double	numBytes;

    private:
	//Methods
	void postEnable( int flag );
	void cntrCmdProc( XMLNode *opt );
	void vlGet( TVal &val );
	void vlArchMake( TVal &val );
};

//*************************************************
//* AMRDevs::TMdContr                             *
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
	int64_t	restTm( )	{ return mRestTm; }
	int64_t	connTry( )	{ return mConnTry; }

	AutoHD<TMdPrm> at( const string &nm )	{ return TController::at(nm); }

    protected:
	//Methods
	void prmEn( const string &id, bool val );

	void start_( );
	void stop_( );
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

    private:
	//Methods
	TParamContr *ParamAttach( const string &name, int type );
	static void *Task( void *icntr );

	//Attributes
	Res	en_res;		//Resource for enable params
	int64_t	&mPrior,	//Process task priority
		&mRestTm,	//Restore timeout in s
		&mConnTry;	//Connections try
	int64_t	mPer;

	bool	prc_st,		// Process task active
		endrun_req;	// Request to stop of the Process task

	vector< AutoHD<TMdPrm> >  p_hd;

	double	tm_gath;	// Gathering time
};

//*************************************************
//* AMRDevs::TTpContr                             *
//*************************************************
class TTpContr: public TTipDAQ
{
    public:
	//Methods
	TTpContr( string name );
	~TTpContr( );

	uint16_t CRC16( const string &src );

    protected:
	//Methods
	void postEnable( int flag );

	void load_( );
	void save_( );

	bool redntAllow( )	{ return false; }

    private:
	//Methods
	TController *ContrAttach( const string &name, const string &daq_db );

	//Attributes
	static uint8_t CRCHi[];
	static uint8_t CRCLo[];
};

extern TTpContr *mod;

} //End namespace AMRDevs

#endif //MOD_AMR_H
