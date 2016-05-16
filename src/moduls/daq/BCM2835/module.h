
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

	//Functions
	void fList( vector<string> &ls )	{ chldList(mFnc, ls); }
	bool fPresent( const string &id )	{ return chldPresent(mFnc, id); }
	AutoHD<TFunction> fAt( const string &id ){ return chldAt(mFnc, id); }
	void fReg( TFunction *fnc )		{ chldAdd(mFnc, fnc); }
	void fUnreg( const char *id )		{ chldDel(mFnc, id); }

	TElem &elem( )		{ return pEl; }

	TMdContr &owner( );

    protected:
	//Methods
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

    private:
	//Methods
	void postEnable( int flag );
	void vlGet( TVal &vo );
	void vlSet( TVal &vo, const TVariant &vl, const TVariant &pvl );
	void vlArchMake( TVal &val );

	//Attributes
	TElem	pEl;		//Work atribute elements
	int	mFnc;
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

	AutoHD<TMdPrm> at( const string &nm )	{ return TController::at(nm); }

    protected:
	//Methods
	void enable_( );
	void disable_( );
	void start_( );
	void stop_( );

    private:
	//Methods
	TParamContr *ParamAttach( const string &name, int type );
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


//*************************************************
//* Access by functions                           *

//*************************************************
//* GPIO mode                                     *
//*************************************************
class GPIO_mode : public TFunction
{
    public:
	GPIO_mode( ) : TFunction("mode",SDAQ_ID) {
	    ioAdd(new IO("rez",_("Result"),IO::Integer,IO::Return));
	    ioAdd(new IO("pin",_("Pin"),IO::Integer,IO::Default));
	    ioAdd(new IO("set",_("Force the input mode: 1-Input,2-Input (pull up),3-Input (pull down),4-Output"),IO::Integer,IO::Default,"0"));
	}

	string name( )	{ return _("GPIO: Mode"); }
	string descr( )	{ return _("GPIO mode, input or output."); }

	void calc( TValFunc *val );
};

//*************************************************
//* Get GPIO value                                *
//*************************************************
class GPIO_get : public TFunction
{
    public:
	GPIO_get( ) : TFunction("get",SDAQ_ID) {
	    ioAdd(new IO("rez",_("Result"),IO::Boolean,IO::Return));
	    ioAdd(new IO("pin",_("Pin"),IO::Integer,IO::Default));
	}

	string name( )	{ return _("GPIO: Get"); }
	string descr( )	{ return _("GPIO get pin level."); }

	void calc( TValFunc *val );
};

//*************************************************
//* Put value to GPIO                             *
//*************************************************
class GPIO_put : public TFunction
{
    public:
	GPIO_put( ) : TFunction("put",SDAQ_ID) {
	    ioAdd(new IO("pin",_("Pin"),IO::Integer,IO::Default));
	    ioAdd(new IO("val",_("Value"),IO::Boolean,IO::Default));
	}

	string name( )	{ return _("GPIO: Put"); }
	string descr( )	{ return _("GPIO put pin level."); }

	void calc( TValFunc *val );
};


} //End namespace ModBCM

#endif //MODULE_H
