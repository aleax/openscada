
//OpenSCADA module DAQ.GPIO file: da_SUNXI.h
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

#ifndef DA_SUNXI_H
#define DA_SUNXI_H

#include "ttypeparam.h"
#include "GPIO_module.h"

namespace ModGPIO
{

//*************************************************
//* SUNXI                                         *
//*************************************************
class SUNXI : public TTypeParam
{
    public:
    //Methods
    SUNXI( );
    //~SUNXI( );

    string DB( TController *cntr )	{ return mod->modId()+"SUNXIPrm_"+cntr->id(); }

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

} //End namespace

#endif //DA_SUNXI_H
