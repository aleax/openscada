
//OpenSCADA system module DAQ.BCM2835 file: module.h
/***************************************************************************
 *   Copyright (C) 2016 by Roman Savochenko, <rom_as@oscada.org>           *
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

#undef _
#define _(mess) mod->I18N(mess)

using std::string;
using std::vector;
using namespace OSCADA;

namespace ModBCM
{

//*************************************************
//* ModBCM::TMdPrm                                *
//*************************************************
class TMdContr;

class TMdPrm : public TParamContr
{
    public:
	//Methods
	TMdPrm( string name, TTypeParam *tp_prm );
	~TMdPrm( );

	string modPrm( const string &prm, const string &def = "" );

	void setModPrm( const string &prm, const string &val );

	void enable( );
	void disable( );

	TElem &elem( )		{ return pEl; }

	TMdContr &owner( );

    protected:
	//Methods
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

    private:
	//Methods
	void postEnable( int flag );
	//void cntrCmdProc( XMLNode *opt );
	void vlGet( TVal &vo );
	void vlSet( TVal &vo, const TVariant &vl, const TVariant &pvl );
	void vlArchMake( TVal &val );

	//Attributes
	TElem	pEl;		//Work atribute elements
};

//*************************************************
//* ModBCM::TMdContr                              *
//*************************************************
class TMdContr: public TController
{
    friend class TMdPrm;
    public:
	//Methods
	TMdContr( string name_c, const string &daq_db, ::TElem *cfgelem );
	~TMdContr( );

	//string getStatus( );

	//int64_t	period( )	{ return mPer; }
	//string	cron( )		{ return mSched; }
	//int	prior( )	{ return mPrior; }

	AutoHD<TMdPrm> at( const string &nm )	{ return TController::at(nm); }

    protected:
	//Methods
	//void prmEn( TMdPrm *prm, bool val );

	void enable_( );
	void disable_( );
	void start_( );
	void stop_( );

    private:
	//Methods
	TParamContr *ParamAttach( const string &name, int type );
	//void cntrCmdProc( XMLNode *opt );
	//static void *Task( void *icntr );

	//Attributes
	//ResMtx	enRes;		// Resource allocation for enable parameters
	//TCfg	&mSched,	// Schedule
	//	&mPrior;	// Process task priority
	//int64_t	mPer;

	//bool	prcSt,		// Process task active
	//	callSt;		// Calc now stat

	//vector< AutoHD<TMdPrm> > pHD;

	//double	tmGath;		// Gathering time
};

//*************************************************
//* ModBCM::TTpContr                              *
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

    private:
	//Methods
	TController *ContrAttach( const string &name, const string &daq_db );
};

extern TTpContr *mod;

} //End namespace ModBCM

#endif //MODULE_H
