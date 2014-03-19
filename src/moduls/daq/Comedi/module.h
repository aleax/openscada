
//OpenSCADA system module DAQ.Comedi file: module.h
/***************************************************************************
 *   Copyright (C) 2012-2014 by Roman Savochenko                           *
 *   rom_as@oscada.org                                                     *
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

#include <comedilib.h>

#include <tcontroller.h>
#include <ttipdaq.h>
#include <tparamcontr.h>

#undef _
#define _(mess) mod->I18N(mess)

using std::string;
using std::vector;
using namespace OSCADA;

namespace ModComedi
{

//*************************************************
//* ModComedi::TMdPrm                             *
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

	void getVals( const string &atr = "" );
	string modPrm( const string &prm );

	void setModPrm( const string &prm, const string &val );

	TMdContr &owner( );

    protected:
	//Methods
	void load_( );
	void save_( );

    private:
	//Methods
	void postEnable( int flag );
	void cntrCmdProc( XMLNode *opt );
	void vlGet( TVal &vo );
	void vlSet( TVal &vo, const TVariant &vl, const TVariant &pvl );
	void vlArchMake( TVal &vo );

	//Attributes
	TElem	p_el;		//Work atribute elements
	char	&asynchRd;	//Asynchronous reading

	Res	dev_res;	//Resource for access to device
	comedi_t *devH;
	int	aiTm;
};

//*************************************************
//* ModComedi::TMdContr                           *
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
	string	cron( )		{ return mSched; }
	int64_t	prior( )	{ return mPrior; }

	AutoHD<TMdPrm> at( const string &nm )	{ return TController::at(nm); }

	void prmEn( const string &id, bool val );

    protected:
	//Methods
	void start_( );
	void stop_( );
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

    private:
	//Methods
	TParamContr *ParamAttach( const string &name, int type );
	static void *Task( void *icntr );

	//Attributes
	Res	en_res;				//Resource for enable params
	int64_t	&mPrior;			//Process task priority
	TCfg	&mSched;			//Calc schedule
	int64_t	mPer;

	bool	prcSt,				//Process task active
		call_st,			//Calc now stat
		endRunReq;			//Request to stop of the Process task
	vector< AutoHD<TMdPrm> >  p_hd;
	double	tm_gath;			//Gathering time
};

//*************************************************
//* ModComedi::TTpContr                           *
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

} //End namespace ModComedi

#endif //MODULE_H
