
//OpenSCADA module DAQ.GPIO file: da_BCM2835.h
/***************************************************************************
 *   Copyright (C) 2018 by Roman Savochenko, <rom_as@oscada.org>           *
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

#ifndef DA_BCM2835_H
#define DA_BCM2835_H

#include "ttypeparam.h"
#include "GPIO_module.h"

namespace ModGPIO
{

//*************************************************
//* BCM2835                                       *
//*************************************************
class BCM2835 : public TTypeParam
{
    public:
    //Methods
    BCM2835( );
    //~BCM2835( );

    string DB( TController *cntr )	{ return mod->modId()+"BCM2835Prm_"+cntr->id(); }

    void create( TParamContr *prm );
    void destroy( TParamContr *prm );

    void enable( TParamContr *prm );
    void disable( TParamContr *prm );

    //bool cfgChange( TParamContr *prm, TCfg &cfg );

    void vlGet( TParamContr *prm, TVal &vo );
    void vlSet( TParamContr *prm, TVal &vo, const TVariant &vl, const TVariant &pvl );

    //bool cntrCmdProc( TParamContr *prm, XMLNode *opt );

    private:
    //Attributes
    bool	use;	//The GPIO using
};

//*************************************************
//* Fast access by the static functions           *

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

} //End namespace

#endif //DA_BCM2835_H
