
//OpenSCADA system module DAQ.OneWire file: module.h
/***************************************************************************
 *   Copyright (C) 2012-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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

#undef _
#define _(mess) mod->I18N(mess)

using std::string;
using std::vector;
using namespace OSCADA;

namespace ModOneWire
{

//*************************************************
//* ModOneWire::TMdPrm                            *
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

	string	dev( )		{ return mDev; }

	string OWFSDevLs( );
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

	//Attributes
	TElem	p_el;	//Work atribute elements
	TCfg	&mDev;	//Device
};

//*************************************************
//* ModOneWire::TMdContr                          *
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
	string  cron( )         { return mSched; }
	int	prior( )	{ return mPrior; }
	int	bus( )		{ return mBus; }

	string OWFSBusLs( );

	AutoHD<TMdPrm> at( const string &nm )	{ return TController::at(nm); }

    protected:
	//Methods
	void prmEn( const string &id, bool val );

	void enable_( );
	void start_( );
	void stop_( );

    private:
	//Methods
	TParamContr *ParamAttach( const string &name, int type );
	void cntrCmdProc( XMLNode *opt );       //Control interface command process
	static void *Task( void *icntr );

	//Attributes
	Res	en_res;		// Resource for enable params
	TCfg	&mSched,	// Schedule
		&mPrior,	// Process task priority
		&mBus,		// 1Wire BUS
		&mSimult;	// Simultaneously temperature sensors read
	int64_t	mPer;

	bool	prcSt,		// Process task active
 		callSt;		// Calc now stat

	vector< AutoHD<TMdPrm> >  p_hd;

	double	tmGath;		// Gathering time
};

//*************************************************
//* ModOneWire::TTpContr                          *
//*************************************************
class TTpContr: public TTipDAQ
{
    public:
	//Methods
	TTpContr( string name );
	~TTpContr( );

	string OWFSPrms( )	{ return mOWFSPrms; }
        void setOWFSPrms( const string &vl );

	string OWFSGet( const string &path );
	int OWFSPut( const string &path, const string &val );

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

	//Attributes
	bool	OW_initOK;
	string	mOWFSPrms;

	Res	OWFSRes;
};

extern TTpContr *mod;

} //End namespace ModOneWire

#endif //MODULE_H
