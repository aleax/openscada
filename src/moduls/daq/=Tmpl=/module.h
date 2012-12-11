
//!!! Module name, file name and module's license. Change for your need.
//OpenSCADA system module DAQ.Tmpl file: module.h
/***************************************************************************
 *   Copyright (C) 2012 by MyName MyFamily                                 *
 *   my@email.org                                                          *
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

//!!! Multi-including this header file prevent. Change for your include file name
#ifndef MODULE_H
#define MODULE_H

//!!! System's includings. Add need for your module includings.
#include <string>
#include <vector>

//!!! OpenSCADA module's API includings. Add need for your module includings.
#include <tcontroller.h>
#include <ttipdaq.h>
#include <tparamcontr.h>

//!!! Individual module's translation function define. Don't change it!
#undef _
#define _(mess) mod->I18N(mess)

using std::string;
using std::vector;
using namespace OSCADA;

//!!! All module's objects you must include into self (individual) namespace. Change namespace for your module.
namespace ModTmpl
{

//!!! DAQ-subsystem parameter object realisation define. Add methods and attributes for your need.
//*************************************************
//* ModTmpl::TMdPrm                               *
//*************************************************
class TMdContr;

class TMdPrm : public TParamContr
{
    public:
	//Methods
	//!!! Constructor for DAQ-subsystem parameter object.
	TMdPrm( string name, TTipParam *tp_prm );
	//!!! Destructor for DAQ-subsystem parameter object.
	~TMdPrm( );

	//!!! Parameter's structure element link function
	TElem &elem( )		{ return p_el; }

	//!!! Processing virtual functions for enable and disable parameter
	void enable( );
	void disable( );

	//!!! Direct link to parameter's owner controller
	TMdContr &owner( );

    protected:
	//Methods
	//!!! Processing virtual functions for load and save parameter to DB
	void load_( );
	void save_( );

    private:
	//Methods
	//!!! Post-enable processing virtual function
	void postEnable( int flag );
	//!!! Processing virtual function for OpenSCADA control interface comands
	void cntrCmdProc( XMLNode *opt );
	//!!! Processing virtual function for setup archive's parameters which associated with the parameter on time archive creation
	void vlArchMake( TVal &val );

	//Attributes
	//!!! Parameter's structure element
	TElem	p_el;			//Work atribute elements
};

//!!! DAQ-subsystem controller object realisation define. Add methods and attributes for your need.
//*************************************************
//* ModTmpl::TMdContr                             *
//*************************************************
class TMdContr: public TController
{
    friend class TMdPrm;
    public:
	//Methods
	//!!! Constructor for DAQ-subsystem controller object.
	TMdContr( string name_c, const string &daq_db, ::TElem *cfgelem );
	//!!! Destructor for DAQ-subsystem controller object.
	~TMdContr( );

	//!!! Status processing function for DAQ-controllers
	string getStatus( );

	//!!! The controller's background task properties
	int64_t	period( )	{ return mPer; }
	string  cron( )         { return mSched; }
	int	prior( )	{ return mPrior; }

	//!!! Request for connection to parameter-object of this controller
	AutoHD<TMdPrm> at( const string &nm )	{ return TController::at(nm); }

    protected:
	//Methods
	//!!! Parameters register function, on time it enable, for fast processing into background task.
	void prmEn( const string &id, bool val );

	//!!! Processing virtual functions for start and stop DAQ-controller
	void start_( );
	void stop_( );

    private:
	//Methods
	//!!! Processing virtual functions for self object-parameter creation.
	TParamContr *ParamAttach( const string &name, int type );
	//!!! Processing virtual function for OpenSCADA control interface comands
	void cntrCmdProc( XMLNode *opt );
	//!!! Background task's function for periodic data acquisition.
	static void *Task( void *icntr );

	//Attributes
	//!!! The resource for Enable parameters.
	Res	en_res;		// Resource for enable params
	//!!! The links to the controller's background task properties into config.
	TCfg	&mSched,	// Schedule
		&mPrior;	// Process task priority
	int64_t	mPer;

	//!!! Background task's sync properties
	bool	prcSt,		// Process task active
 		callSt,		// Calc now stat
		endrunReq;	// Request to stop of the Process task

	//!!! Enabled and processing parameter's links list container.
	vector< AutoHD<TMdPrm> >  p_hd;

	double	tmGath;		// Gathering time
};

//!!! Root module object define. Add methods and attributes for your need.
//*************************************************
//* ModTmpl::TTpContr                             *
//*************************************************
class TTpContr: public TTipDAQ
{
    public:
	//Methods
	//!!! Constructor for Root module object.
	TTpContr( string name );
	//!!! Destructor for Root module object.
	~TTpContr( );

    protected:
	//Methods
	//!!! Post-enable processing virtual function
	void postEnable( int flag );

	//!!! Processing virtual functions for load and save Root module to DB
	void load_( );
	void save_( );

	//!!! The flag for redundantion mechanism support by module detection
	bool redntAllow( )	{ return true; }

    private:
	//Methods
	//!!! Processing virtual functions for self object-controller creation.
	TController *ContrAttach( const string &name, const string &daq_db );

	//!!! Module's comandline options for print help function.
	string optDescr( );
};

//!!! The module root link
extern TTpContr *mod;

} //End namespace ModTmpl

#endif //MODULE_H
