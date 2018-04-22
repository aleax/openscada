
//OpenSCADA system module DAQ.GPIO file: GPIO_module.h
/***************************************************************************
 *   Copyright (C) 2016-2018 by Roman Savochenko, <rom_as@oscada.org>      *
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

namespace ModGPIO
{

//********************************************************
//* ModGPIO::TMdPrm - Controller parameter               *
//********************************************************
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
    void fList( vector<string> &ls ) const		{ chldList(mFnc, ls); }
    bool fPresent( const string &id ) const		{ return chldPresent(mFnc, id); }
    AutoHD<TFunction> fAt( const string &id ) const	{ return chldAt(mFnc, id); }
    void fReg( TFunction *fnc )				{ chldAdd(mFnc, fnc); }
    void fUnreg( const string &id )			{ chldDel(mFnc, id); }

    TElem &elem( )		{ return pEl; }

    TMdContr &owner( ) const;

    //Attributes
    TElem	pEl;		//Work atribute elements

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
    int	mFnc;
};

//*************************************************
//* ModGPIO::TMdContr                              *
//*************************************************
class TMdContr: public TController
{
    friend class TMdPrm;
    public:
    //Methods
    TMdContr( string name_c, const string &daq_db, ::TElem *cfgelem );
    ~TMdContr( );

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
//* ModGPIO::TTpContr                              *
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

} //End namespace ModGPIO

#endif //MODULE_H
