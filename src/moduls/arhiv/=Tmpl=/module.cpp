
//OpenSCADA module Archive.Tmpl file: module.cpp
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
#include <sys/stat.h>
#include <signal.h>

// OpenSCADA API includings - add need ones
#include <tsys.h>
#include <resalloc.h>
#include <tmess.h>

// Own includings of the module - add need ones
#include "module.h"

//*************************************************
//* Module info!                                  *
#define MOD_ID		"Tmpl"
#define MOD_NAME	trS("Archivator template")
#define MOD_TYPE	SARH_ID
#define VER_TYPE	SARH_VER
#define MOD_VER		"0.0.1"
#define AUTHORS		trS("MyName MyFamily")
#define DESCRIPTION	trS("Archive's subsystem template module.")
#define LICENSE		"MyLicense"
//*************************************************

ArhTmpl::ModArch *ArhTmpl::mod;

// Required section for binding OpenSCADA core to this module, It gives information and creates module root object - do not change
extern "C"
{
#ifdef MOD_INCL
    TModule::SAt arh_Tmpl_module( int n_mod )
#else
    TModule::SAt module( int n_mod )
#endif
    {
	if(n_mod == 0)	return TModule::SAt(MOD_ID, MOD_TYPE, VER_TYPE);
	return TModule::SAt("");
    }

#ifdef MOD_INCL
    TModule *arh_Tmpl_attach( const TModule::SAt &AtMod, const string &source )
#else
    TModule *attach( const TModule::SAt &AtMod, const string &source )
#endif
    {
	if(AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE)) return new ArhTmpl::ModArch(source);
	return NULL;
    }
}

using namespace ArhTmpl;

//*************************************************
//* ArhTmpl::ModArch                               *
//*************************************************
ModArch::ModArch( const string &name ) : TTypeArchivator(MOD_ID)
{
    mod = this;

    modInfoMainSet(MOD_NAME, MOD_TYPE, MOD_VER, AUTHORS, DESCRIPTION, LICENSE, name);
}

ModArch::~ModArch( )
{
    try{ modStop(); } catch(...) { }
}

void ModArch::postEnable( int flag )
{
    TModule::postEnable(flag);

    if(flag&TCntrNode::NodeConnect) {
	//Adding the own DB-fields for archives
	owner().messE().fldAdd(new TFld("A_PRMS","Addon parameters",TFld::String,TFld::FullText,"10000"));
	owner().valE().fldAdd(new TFld("A_PRMS","Addon parameters",TFld::String,TFld::FullText,"10000"));
    }
}

TMArchivator *ModArch::AMess( const string &id, const string &db ) { return new ModMArch(id, db, &owner().messE()); }

TVArchivator *ModArch::AVal( const string &id, const string &db ) { return new ModVArch(id, db, &owner().valE()); }
