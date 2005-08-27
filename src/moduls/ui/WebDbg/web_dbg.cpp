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
 
#include <getopt.h>
#include <string>

#include <tsys.h>
#include <tmess.h>

#include "web_dbg.h"

//============ Modul info! =====================================================
#define MOD_ID	    "WebDbg"
#define MOD_NAME    "WEB debuger"
#define MOD_TYPE    "UI"
#define VER_TYPE    VER_UI
#define SUB_TYPE    "WWW"
#define VERSION     "0.0.3"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "Web debug modul."
#define LICENSE     "GPL"
//==============================================================================
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
	WebDbg::TWEB *self_addr = NULL;

	if( AtMod.id == MOD_ID && AtMod.type == MOD_TYPE && AtMod.t_ver == VER_TYPE )
	    self_addr = new WebDbg::TWEB( source );       

	return ( self_addr );
    }    
}

using namespace WebDbg;

//==============================================================================
//================ WebDbg::TWEB ================================================
//==============================================================================
TWEB::TWEB( string name )
{
    mId		= MOD_ID;
    mName       = MOD_NAME;
    mType	= MOD_TYPE;
    Vers	= VERSION;
    Autors	= AUTORS;
    DescrMod	= DESCRIPTION;
    License	= LICENSE;
    Source	= name;
    
    //Reg export functions
    modFuncReg( new ExpFunc("void HttpGet(const string&,string&,const string&,vector<string>&);",
        "Process Get comand from http protocol's!",(void(TModule::*)( )) &TWEB::HttpGet) );
    modFuncReg( new ExpFunc("void HttpPost(const string&,string&,const string&,vector<string>&,const string&);",
        "Process Set comand from http protocol's!",(void(TModule::*)( )) &TWEB::HttpPost) );				
}

TWEB::~TWEB()
{

}

string TWEB::modInfo( const string &name )
{
    if( name == "SubType" ) return(SUB_TYPE);
    else return( TModule::modInfo( name) );
}

void TWEB::modInfo( vector<string> &list )
{
    TModule::modInfo(list);
    list.push_back("SubType");
}

string TWEB::optDescr( )
{
    char buf[STR_BUF_LEN];

    snprintf(buf,sizeof(buf),I18N(
	"======================= The module <%s:%s> options =======================\n"
	"---------- Parameters of the module section <%s> in config file ----------\n\n"),
	MOD_TYPE,MOD_ID,nodePath().c_str());

    return(buf);
}

void TWEB::modLoad( )
{
    //========== Load parameters from command line ============
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{"help"      ,0,NULL,'h'},
	{NULL        ,0,NULL,0  }
    };

    optind=opterr=0;
    do
    {
	next_opt=getopt_long(SYS->argc,(char * const *)SYS->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': fprintf(stdout,optDescr().c_str()); break;
	    case -1 : break;
	}
    } while(next_opt != -1);    
    
    //========== Load parameters from config file =============
}

char *TWEB::w_head =
    "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n"
    "<html> <head>\n"
    "  <title>OpenSCADA debug web modul!</title>\n"
    " </head>\n";

char *TWEB::w_head_ =
    "</html>\n";

char *TWEB::w_body =
    " <body bgcolor=\"#330033\" text=\"#ffffff\" link=\"#3366ff\" vlink=\"#339999\" alink=\"#33ccff\">\n"
    "  <h1 align=\"center\"><font color=\"#ffff00\"> Welcome to OpenSCADA debug web modul!</font></h1>\n"
    "  <hr width=\"100%\" size=\"2\">\n"
    "  <br><br>\n";

char *TWEB::w_body_ =
    " </body>\n";    

void TWEB::HttpGet( const string &url, string &page, const string &sender, vector<string> &vars )
{
    page = page+w_head+w_body+w_body_+w_head_;    
}

void TWEB::HttpPost( const string &url, string &page, const string &sender, vector<string> &vars, const string &contein )
{

}

