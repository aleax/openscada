
//OpenSCADA module DAQ.Tmpl file: module.h
/***************************************************************************
 *   Copyright (C) 2022 by MyName MyFamily, <my@email.org>                 *
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

// Preventing of the header file multi-including - change at the header file name changing
#ifndef MODULE_H
#define MODULE_H

// System includings - add need ones
#include <string>
#include <vector>

// OpenSCADA API includings - add need ones
#include <tcontroller.h>
#include <ttypedaq.h>
#include <tparamcontr.h>

// Definition of the individual module translation functions - do not change
#undef _
#define _(mess) mod->I18N(mess).c_str()
#undef trS
#define trS(mess) mod->I18N(mess,mess_PreSave)

using std::string;
using std::vector;
using namespace OSCADA;

// All the module objects in own (individual) namespace - change for your module
namespace DAQTmpl
{

// Definition the object of the controller parameter of the subsystem "DAQ" - add methods and attributes at your need
//*************************************************
//* DAQTmpl::TMdPrm                               *
//*************************************************
class TMdContr;

class TMdPrm : public TParamContr
{
    public:
	//Methods
	TMdPrm( string name, TTypeParam *tp_prm );
	~TMdPrm( );

	TElem &elem( )		{ return pEl; }

	void enable( );
	void disable( );

	TMdContr &owner( ) const;

    private:
	//Methods
	void postEnable( int flag );
	void cntrCmdProc( XMLNode *opt );
	void vlArchMake( TVal &val );

	//Attributes
	TElem	pEl;			//Elements of the dynamic attributes
};

// Definition the controller object of the subsystem "DAQ" - add methods and attributes at your need
//*************************************************
//* DAQTmpl::TMdContr                             *
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
	int	prior( )	{ return mPrior; }

	AutoHD<TMdPrm> at( const string &nm )	{ return TController::at(nm); }

    protected:
	//Methods
	void prmEn( TMdPrm *prm, bool val );

	void start_( );
	void stop_( );

	void cntrCmdProc( XMLNode *opt );
	bool cfgChange( TCfg &co, const TVariant &pc );

    private:
	//Methods
	TParamContr *ParamAttach( const string &id, int type );
	static void *Task( void *icntr );

	//Attributes
	ResMtx	enRes;		// Resources lock for enabling the parameters
	int64_t	&mPrior;	// Priority of the processing task
	int64_t	mPer;

	bool	prcSt,		// Activity of the task processing
		callSt;		// State of the calculation just now

	vector< AutoHD<TMdPrm> > pHd;

	double	tmGath;		// Time of the data gathering
};

// Definition the root module object of the subsystem "DAQ" - add methods and attributes at your need
//*************************************************
//* DAQTmpl::TTpContr                             *
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

	bool redntAllow( )	{ return true; }

    private:
	//Methods
	TController *ContrAttach( const string &id, const string &daq_db );
};

extern TTpContr *mod;	//The module root link

} //End the namespace DAQTmpl

#endif //MODULE_H
