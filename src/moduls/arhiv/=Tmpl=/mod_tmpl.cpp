
//!!! Module name, file name and module's license. Change for your need.
//OpenSCADA system module Archive.ModTmpl file: mod_tmpl.cpp
/***************************************************************************
 *   Copyright (C) 2010 by Roman Savochenko                                *
 *   rom_as@oscada.org, rom_as@fromru.com                                  *
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

//!!! System's includings. Add need for your module includings.
#include <sys/stat.h>
#include <signal.h>
#include <getopt.h>
#include <string>

//!!! OpenSCADA module's API includings. Add need for your module includings.
#include <tsys.h>
#include <resalloc.h>
#include <tmess.h>

//!!! Self your module's includings. Add need for your module includings.
#include "arch.h"

//!!! Module's meta-information. Change for your module.
//*************************************************
//* Modul info!                                   *
#define MOD_ID		"Tmpl"
#define MOD_NAME	_("Transport template")
#define MOD_TYPE	SARH_ID
#define VER_TYPE	SARH_VER
#define MOD_VER		"0.0.1"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("Archive's subsystem template module.")
#define LICENSE		"GPL2"
//*************************************************

ModTmpl::ModArch *ModTmpl::mod;

//!!! Required section for binding OpenSCADA kernel's to this module. Gives information and create module's root object.
//!!! Not remove this section!
extern "C"
{
    TModule::SAt module( int n_mod )
    {
	if( n_mod==0 ) 	return TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE);
	return TModule::SAt("");
    }

    TModule *attach( const TModule::SAt &AtMod, const string &source )
    {
	if( AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE) ) 
	    return new ModTmpl::ModArch( source );
	return NULL;
    }
}

//!!! Include for default call into your module's namespace.
using namespace ModTmpl;

//*************************************************
//* ModTmpl::ModArch                               *
//*************************************************
//!!! Constructor for module's root object. Append into for your need.
ModArch::ModArch( const string &name ) : TTipArchivator(MOD_ID)
{
    //!!! Init shortcut to module's root object. No change it.
    mod		= this;

    //!!! Loading module's meta-information into root object. No change it.
    mName	= MOD_NAME;
    mType	= MOD_TYPE;
    mVers	= MOD_VER;
    mAuthor	= AUTHORS;
    mDescr	= DESCRIPTION;
    mLicense	= LICENSE;
    mSource	= name;
}

//!!! Module's post enable call. Add your module objects initialize code.
void ModArch::postEnable( int flag )
{
    TModule::postEnable( flag );

    if(!(flag&TCntrNode::NodeConnect))	return;

    //!!! Place here your code for internal objects initialize.
}

//!!! Destructor for root module's object.
ModArch::~ModArch()
{
    try{ modStop(); }catch(...){}
}

//!!! Inherited (virtual) load and save object's node methods. Call from OpenSCADA kernel.
void ModArch::load_()
{
    //> Load parameters from command line

}

//!!! Main subsystem API function for self modules message objects creation.
TMArchivator *ModArch::AMess(const string &iid, const string &idb)
{
    return new ModMArch(iid,idb,&owner().messE());
}

//!!! Main subsystem API functions for self modules value archive objects creation.
TVArchivator *ModArch::AVal(const string &iid, const string &idb)
{
    return new ModVArch(iid,idb,&owner().valE());
}
