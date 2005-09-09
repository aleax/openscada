/***************************************************************************
 *   Copyright (C) 2004 by Roman Savochenko                                *
 *   rom_as@fromru.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
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


#include <tsys.h>

#include "libcompl1.h"
#include "stdmath.h"
#include "statfunc.h"

//============ Modul info! =====================================================
#define MOD_ID      "StaticFuncLibs"
#define MOD_NAME    "Static function's libs"
#define MOD_TYPE    "Special"
#define VER_TYPE    VER_SPC
#define SUB_TYPE    "LIB"
#define VERSION     "0.9.0"
//==============================================================================

StatFunc::Libs *StatFunc::st_lib;

extern "C"
{
    TModule::SAt module( int n_mod )
    {
	TModule::SAt AtMod;

	if(n_mod==0)
	{
	    AtMod.id	= MOD_ID;
	    AtMod.type  = MOD_TYPE;
	    AtMod.t_ver = VER_TYPE;
	}
    	else
	    AtMod.id	= "";

	return( AtMod );
    }

    TModule *attach( const TModule::SAt &AtMod, const string &source )
    {
	StatFunc::Libs *self_addr = NULL;

    	if( AtMod.id == MOD_ID && AtMod.type == MOD_TYPE && AtMod.t_ver == VER_TYPE )
	    StatFunc::st_lib = self_addr = new StatFunc::Libs( source );

	return ( self_addr );
    }
}

using namespace StatFunc;

//Complex1 functions library
Libs::Libs( string src )
{
    //== Set modul info! ============
    mId 	= MOD_ID;
    mName       = MOD_NAME;
    mType  	= MOD_TYPE;
    Vers      	= VERSION;
    Autors    	= "Roman Savochenko";
    DescrMod  	= "Allow static function libraries. Include libraries: \n"
		  " - SCADA Complex1 functions;\n"
		  " - standart mathematic functions.";
    License   	= "GPL";
    Source    	= src;
}

Libs::~Libs()
{
    if( owner().owner().func().at().present("complex1") )
	owner().owner().func().at().unreg("complex1");
    if( owner().owner().func().at().present("math") )
        owner().owner().func().at().unreg("math");	    
}

void Libs::postEnable( )
{
    TModule::postEnable( );
    
    //Reg libraries
    owner().owner().func().at().reg(new Complex1());
    owner().owner().func().at().reg(new StdMath());
}

void Libs::modStart( )
{
    owner().owner().func().at().at("complex1").at().start(true);
    owner().owner().func().at().at("math").at().start(true);
}
	    
void Libs::modStop( )
{
    owner().owner().func().at().at("complex1").at().start(false);
    owner().owner().func().at().at("math").at().start(false);
}
