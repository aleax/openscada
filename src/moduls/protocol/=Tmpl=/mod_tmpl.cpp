
//!!! Module name, file name and module's license. Change for your need.
//OpenSCADA system module Protocol.Tmpl file: mod_tmpl.cpp
/***************************************************************************
 *   Copyright (C) 2009 by Roman Savochenko                                *
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
#include <getopt.h>
#include <string.h>

//!!! OpenSCADA module's API includings. Add need for your module includings.
#include <config.h>
#include <tsys.h>
#include <tmess.h>
#include <tmodule.h>
#include <tuis.h>

//!!! Self your module's includings. Add need for your module includings.
#include "mod_tmpl.h"

//!!! Module's meta-information. Change for your module.
//*************************************************
//* Modul info!                                   *
#define MOD_ID		"Tmpl"
#define MOD_NAME	_("Protocol template")
#define MOD_TYPE	SPRT_ID
#define VER_TYPE	SPRT_VER
#define MOD_VER		"0.0.1"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("Protocol's subsystem template module.")
#define LICENSE		"GPL2"
//*************************************************

ModTmpl::TProt *ModTmpl::mod;

//!!! Required section for binding OpenSCADA core to this module. Gives information and create module's root object.
//!!! Not remove this section!
extern "C"
{
    TModule::SAt module( int n_mod )
    {
	if( n_mod==0 )	return TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE);
	return TModule::SAt("");
    }

    TModule *attach( const TModule::SAt &AtMod, const string &source )
    {
	if( AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE) )
	    return new ModTmpl::TProt( source );
	return NULL;
    }
}

//!!! Include for default enter to your module namespace.
using namespace ModTmpl;

//*************************************************
//* TProt                                         *
//*************************************************
//!!! Constructor for module's root object. Append into for your need.
TProt::TProt( string name ) : TProtocol(MOD_ID)
{
    //!!! Init shortcut to module root object. Don't change it!
    mod		= this;

    //!!! Load module meta-information to root object. Don't change it!
    mType	= MOD_TYPE;
    mName	= MOD_NAME;
    mVers	= MOD_VER;
    mAuthor	= AUTHORS;
    mDescr	= DESCRIPTION;
    mLicense	= LICENSE;
    mSource	= name;
}

//!!! Destructor for module's root object. Append into for your need.
TProt::~TProt()
{

}

//!!! Module's comandline options the print help function. Add your module commandline parameters info.
string TProt::optDescr( )
{
    char buf[STR_BUF_LEN];
    snprintf(buf,sizeof(buf),_(
	"======================= The module <%s:%s> options =======================\n"
	"---------- Parameters of the module section '%s' in config-file ----------\n"
	"AuthTime <min>      Life time of the authentication, minutes (default 10).\n\n"),
	MOD_TYPE,MOD_ID,nodePath().c_str());

    return buf;
}

//!!! Inherited (virtual) load object's node method. Append your module need data loadings
void TProt::load_( )
{
    //!!! Load self module command line parameters' values. Append your addition parameters process.
    //> Load parameters from command line
    int next_opt;
    const char *short_opt="h";
    struct option long_opt[] =
    {
	{"help"    ,0,NULL,'h'},
	{NULL      ,0,NULL, 0 }
    };

    optind=opterr=0;
    do
    {
	next_opt=getopt_long(SYS->argc,(char * const *)SYS->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': fprintf(stdout,"%s",optDescr().c_str()); break;
	    case -1 : break;
	}
    } while(next_opt != -1);

    //!!! Load addition your module specific data. For example, make loading addition module's parameters from OpenSCADA system DB or from main config-file
}

void TProt::save_( )
{
    //!!! Save addition your module specific data. For example, make saving addition module's parameters to OpenSCADA system DB
}

//!!! Main subsystem API function for self modules input protocol object creation. Change only your class names.
TProtocolIn *TProt::in_open( const string &name )
{
    return new TProtIn(name);
}

//!!! OpenSCADA control interface comands process virtual function.
//!!! For example, process access from standard confifurators of OpenSCADA to individual module's parameters.
//!!! Modify for self needs
void TProt::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if(opt->name() == "info")
    {
	TProtocol::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/help/g_help",_("Options help"),R_R___,"root",SPRT_ID,3,"tp","str","cols","90","rows","5");
	return;
    }

    //> Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/help/g_help" && ctrChkNode(opt,"get",R_R___,"root",SPRT_ID))	opt->setText(optDescr());
    else TProtocol::cntrCmdProc(opt);
}


//*************************************************
//* TProtIn                                       *
//*************************************************
//!!! Constructor for input protocol object. Append into for your need.
TProtIn::TProtIn( string name ) : TProtocolIn(name)
{

}

//!!! Destructor for input protocol object. Append into for your need.
TProtIn::~TProtIn()
{

}

//!!! The inherited (virtual) function of process input and preparint output data for called input transport
bool TProtIn::mess( const string &reqst, string &answer, const string &sender )
{
    //!!! Your code
}
