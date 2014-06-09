
//OpenSCADA system module Archive.DBArch file: arch.cpp
/***************************************************************************
 *   Copyright (C) 2007-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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
#define MOD_VER		"0.9.5"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("The Archive module. Allow functions for messages and values archiving to DB.")
#define LICENSE		"GPL2"
//*************************************************

DBArch::ModArch *DBArch::mod;

extern "C"
{
#ifdef MOD_INCL
    TModule::SAt arh_DBArch_module(int n_mod)
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
ModArch::ModArch( const string &name ) : TTipArchivator(MOD_ID)
{
    mod		= this;

    mName	= MOD_NAME;
    mType	= MOD_TYPE;
    mVers	= MOD_VER;
    mAuthor	= AUTHORS;
    mDescr	= DESCRIPTION;
    mLicense	= LICENSE;
    mSource	= name;
}

void ModArch::postEnable( int flag )
{
    TModule::postEnable(flag);

    if(flag&TCntrNode::NodeConnect)
    {
	//Add self DB-fields for archives
	owner().messE().fldAdd(new TFld("A_PRMS","Addon parameters",TFld::String,TFld::FullText,"10000"));
	owner().valE().fldAdd(new TFld("A_PRMS","Addon parameters",TFld::String,TFld::FullText,"10000"));

	//Archivators info table DB structure
	el_arch.fldAdd(new TFld("TBL","Table",TFld::String,TCfg::Key,"50"));
	el_arch.fldAdd(new TFld("BEGIN","Begin",TFld::String,TFld::NoFlag,"20"));
	el_arch.fldAdd(new TFld("END","End",TFld::String,TFld::NoFlag,"20"));
	el_arch.fldAdd(new TFld("PRM1","Parameter 1",TFld::String,TFld::NoFlag,"20"));
	el_arch.fldAdd(new TFld("PRM2","Parameter 2",TFld::String,TFld::NoFlag,"20"));
	el_arch.fldAdd(new TFld("PRM3","Parameter 3",TFld::String,TFld::NoFlag,"20"));

	//Message DB archive DB structure
	el_mess.fldAdd(new TFld("TM",_("Time (s)"),TFld::Integer,TCfg::Key|TFld::DateTimeDec,"10"));
	el_mess.fldAdd(new TFld("TMU",_("Time (us)"),TFld::Integer,TCfg::Key,"6","0"));
	el_mess.fldAdd(new TFld("CATEG",_("Category"),TFld::String,TCfg::Key,"100"));
	el_mess.fldAdd(new TFld("MESS",_("Message"),TFld::String,TFld::NoFlag/*TCfg::Key*/,"100000"));
	el_mess.fldAdd(new TFld("LEV",_("Level"),TFld::Integer,TFld::NoFlag,"2"));

	//Boolean and integer value DB archive DB structure
	el_vl_int.fldAdd(new TFld("TM",_("Time (s)"),TFld::Integer,TCfg::Key|TFld::DateTimeDec,"10"));
	el_vl_int.fldAdd(new TFld("TMU",_("Time (us)"),TFld::Integer,TCfg::Key,"10"));
	el_vl_int.fldAdd(new TFld("VAL",_("Value"),TFld::Integer,TFld::NoFlag));

	//Real value DB archive DB structure
	el_vl_real.fldAdd(new TFld("TM",_("Time (s)"),TFld::Integer,TCfg::Key|TFld::DateTimeDec,"10"));
	el_vl_real.fldAdd(new TFld("TMU",_("Time (us)"),TFld::Integer,TCfg::Key,"10"));
	el_vl_real.fldAdd(new TFld("VAL",_("Value"),TFld::Real,TFld::NoFlag));

	//String value DB archive DB structure
	el_vl_str.fldAdd(new TFld("TM",_("Time (s)"),TFld::Integer,TCfg::Key|TFld::DateTimeDec,"10"));
	el_vl_str.fldAdd(new TFld("TMU",_("Time (us)"),TFld::Integer,TCfg::Key,"10"));
	el_vl_str.fldAdd(new TFld("VAL",_("Value"),TFld::String,TFld::NoFlag,"1000"));
    }
}

ModArch::~ModArch( )
{
    try{ modStop(); }catch(...){ }
}

void ModArch::load_( )		{ }

void ModArch::modStart( )	{ }

void ModArch::modStop( )	{ }

TMArchivator *ModArch::AMess( const string &iid, const string &idb )	{ return new ModMArch(iid,idb,&owner().messE()); }

TVArchivator *ModArch::AVal( const string &iid, const string &idb )	{ return new ModVArch(iid,idb,&owner().valE()); }
