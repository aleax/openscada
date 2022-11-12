
//OpenSCADA module Protocol.Tmpl file: module.cpp
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

// System includings - add need ones
#include <string.h>

// OpenSCADA API includings - add need ones
#include <tsys.h>
#include <tmess.h>
#include <tmodule.h>
#include <tuis.h>

// Own includings of the module - add need ones
#include "module.h"

//*************************************************
//* Module info!                                  *
#define MOD_ID		"Tmpl"
#define MOD_NAME	trS("Protocol template")
#define MOD_TYPE	SPRT_ID
#define VER_TYPE	SPRT_VER
#define MOD_VER		"0.0.1"
#define AUTHORS		trS("MyName MyFamily")
#define DESCRIPTION	trS("Protocol's subsystem template module.")
#define LICENSE		"MyLicense"
//*************************************************

PrtTmpl::TProt *PrtTmpl::mod;

// Required section for binding OpenSCADA core to this module, It gives information and creates module root object - do not change
extern "C"
{
#ifdef MOD_INCL
    TModule::SAt prt_Tmpl_module( int n_mod )
#else
    TModule::SAt module( int n_mod )
#endif
    {
	if(n_mod == 0) return TModule::SAt(MOD_ID, MOD_TYPE, VER_TYPE);
	return TModule::SAt("");
    }

#ifdef MOD_INCL
    TModule *prt_Tmpl_attach( const TModule::SAt &AtMod, const string &source )
#else
    TModule *attach( const TModule::SAt &AtMod, const string &source )
#endif
    {
	if(AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE)) return new PrtTmpl::TProt(source);
	return NULL;
    }
}

using namespace PrtTmpl;

//*************************************************
//* TProt                                         *
//*************************************************
TProt::TProt( string name ) : TProtocol(MOD_ID)
{
    mod = this;

    modInfoMainSet(MOD_NAME, MOD_TYPE, MOD_VER, AUTHORS, DESCRIPTION, LICENSE, name);
}

TProt::~TProt()
{

}

void TProt::load_( )
{
    //???? Append loading the configuration parameters
}

void TProt::save_( )
{
    //???? Append saving the configuration parameters
}

TProtocolIn *TProt::in_open( const string &id )	{ return new TProtIn(id); }

void TProt::outMess( XMLNode &io, TTransportOut &tro )
{
    MtxAlloc resN(tro.reqRes(), true);

    //???? Processing for the request preparing, sending and the answer receiving, parsing
}

void TProt::cntrCmdProc( XMLNode *opt )
{
    //???? Change and append for your specific configuration

    //Getting the page info
    if(opt->name() == "info") {
	TProtocol::cntrCmdProc(opt);
	//...
	return;
    }
    //Processing for commands to the page
    string a_path = opt->attr("path");
    //...
    TProtocol::cntrCmdProc(opt);
}

//*************************************************
//* TProtIn                                       *
//*************************************************
TProtIn::TProtIn( string name ) : TProtocolIn(name)
{

}

TProtIn::~TProtIn( )
{

}

bool TProtIn::mess( const string &reqst, string &answer )
{
    //???? Write here processing the input requests

    return false;
}
