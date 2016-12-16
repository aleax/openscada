
//OpenSCADA system module Archive.DBArch file: arch.cpp
/***************************************************************************
 *   Copyright (C) 2007-2016 by Roman Savochenko, <rom_as@oscada.org>      *
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

#include <sys/stat.h>
#include <signal.h>
#include <string>

#include <tsys.h>
#include <resalloc.h>
#include <tmess.h>

#include "arch.h"

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"DBArch"
#define MOD_NAME	_("To DB archivator")
#define MOD_TYPE	SARH_ID
#define VER_TYPE	SARH_VER
#define MOD_VER		"2.4.0"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("The archiver module. Provides functions for messages and values archiving to DB.")
#define LICENSE		"GPL2"
//*************************************************

DBArch::ModArch *DBArch::mod;

extern "C"
{
#ifdef MOD_INCL
    TModule::SAt arh_DBArch_module( int n_mod )
#else
    TModule::SAt module( int n_mod )
#endif
    {
	if(n_mod == 0)	return TModule::SAt(MOD_ID, MOD_TYPE, VER_TYPE);
	return TModule::SAt("");
    }

#ifdef MOD_INCL
    TModule *arh_DBArch_attach( const TModule::SAt &AtMod, const string &source )
#else
    TModule *attach( const TModule::SAt &AtMod, const string &source )
#endif
    {
	if(AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE)) return new DBArch::ModArch(source);
	return NULL;
    }
}

using namespace DBArch;

//*************************************************
//* DBArch::ModArch                               *
//*************************************************
ModArch::ModArch( const string &name ) : TTypeArchivator(MOD_ID)
{
    mod = this;

    modInfoMainSet(MOD_NAME, MOD_TYPE, MOD_VER, AUTHORS, DESCRIPTION, LICENSE, name);
}

void ModArch::postEnable( int flag )
{
    TModule::postEnable(flag);

    if(flag&TCntrNode::NodeConnect) {
	//Add self DB-fields for archives
	owner().messE().fldAdd(new TFld("A_PRMS","Addon parameters",TFld::String,TFld::FullText,"10000"));
	owner().valE().fldAdd(new TFld("A_PRMS","Addon parameters",TFld::String,TFld::FullText,"10000"));

	//Archivators info table DB structure
	elArch.fldAdd(new TFld("TBL","Table",TFld::String,TCfg::Key,"50"));
	elArch.fldAdd(new TFld("BEGIN","Begin",TFld::String,TFld::NoFlag,"20"));
	elArch.fldAdd(new TFld("END","End",TFld::String,TFld::NoFlag,"20"));
	elArch.fldAdd(new TFld("PRM1","Parameter 1",TFld::String,TFld::NoFlag,"20"));
	elArch.fldAdd(new TFld("PRM2","Parameter 2",TFld::String,TFld::NoFlag,"1000000"));
	elArch.fldAdd(new TFld("PRM3","Parameter 3",TFld::String,TFld::NoFlag,"20"));
    }
}

ModArch::~ModArch( )
{
    try{ modStop(); } catch(...) { }
}

void ModArch::perSYSCall( unsigned int cnt )
{
    try {
	//if(cnt%60) return;

	vector<string> aLs;

	//Check value archivators
	valList(aLs);
	for(unsigned iA = 0; iA < aLs.size(); iA++)
	    if(valAt(aLs[iA]).at().startStat())
		try{ valAt(aLs[iA]).at().checkArchivator(cnt); }
		catch(TError &err) {
		    mess_err(err.cat.c_str(), "%s", err.mess.c_str());
		    mess_sys(TMess::Error, _("Check value archivator '%s' error."), aLs[iA].c_str());
	    }
    } catch(TError &err) { mess_sys(TMess::Error, "%s", err.mess.c_str()); }
}

void ModArch::load_( )		{ }

void ModArch::modStart( )	{ }

void ModArch::modStop( )	{ }

TMArchivator *ModArch::AMess( const string &iid, const string &idb )	{ return new ModMArch(iid,idb,&owner().messE()); }

TVArchivator *ModArch::AVal( const string &iid, const string &idb )	{ return new ModVArch(iid,idb,&owner().valE()); }
