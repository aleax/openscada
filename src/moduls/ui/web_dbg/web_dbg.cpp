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
#include <tkernel.h>
#include <tmessage.h>

#include "web_dbg.h"

//============ Modul info! =====================================================
#define NAME_MODUL  "web_dbg"
#define NAME_TYPE   "UI"
#define VER_TYPE    VER_UI
#define SUB_TYPE    "WWW"
#define VERSION     "0.0.3"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "Web debug modul."
#define LICENSE     "GPL"
//==============================================================================
extern "C" 
{
    SAtMod module( int n_mod )
    {
    	SAtMod AtMod;

	if(n_mod==0)
	{
	    AtMod.name  = NAME_MODUL;
	    AtMod.type  = NAME_TYPE;
    	    AtMod.t_ver = VER_TYPE;
	}
	else
	    AtMod.name  = "";

	return( AtMod );
    }

    TModule *attach( const SAtMod &AtMod, const string &source )
    {
	WebDbg::TWEB *self_addr = NULL;

	if( AtMod.name == NAME_MODUL && AtMod.type == NAME_TYPE && AtMod.t_ver == VER_TYPE )
	    self_addr = new WebDbg::TWEB( source );       

	return ( self_addr );
    }    
}

using namespace WebDbg;

//==============================================================================
//================ WebDbg::TWEB ================================================
//==============================================================================
SExpFunc TWEB::ExpFuncLc[] =
{
    {"HttpGet",(void(TModule::*)( )) &TWEB::HttpGet,"void HttpGet( const string &url, string &page, const string &sender, vector<string> &vars);",
     "Process Get comand from http protocol's!",10,0},
    {"HttpPost",(void(TModule::*)( )) &TWEB::HttpPost,"void HttpPost( const string &url, string &page, const string &sender, vector<string> &vars, const string &contein);",
     "Process Post comand from http protocol's!",10,0}     
};

TWEB::TWEB( string name )
{
    NameModul = NAME_MODUL;
    NameType  = NAME_TYPE;
    Vers      = VERSION;
    Autors    = AUTORS;
    DescrMod  = DESCRIPTION;
    License   = LICENSE;
    Source    = name;

    ExpFunc   = (SExpFunc *)ExpFuncLc;
    NExpFunc  = sizeof(ExpFuncLc)/sizeof(SExpFunc);
}

TWEB::~TWEB()
{

}

string TWEB::mod_info( const string &name )
{
    if( name == "SubType" ) return(SUB_TYPE);
    else return( TModule::mod_info( name) );
}

void TWEB::mod_info( vector<string> &list )
{
    TModule::mod_info(list);
    list.push_back("SubType");
}

void TWEB::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "======================= The module <%s:%s> options =======================\n"
    "---------- Parameters of the module section <%s> in config file ----------\n"	    
    "\n",NAME_TYPE,NAME_MODUL,NAME_MODUL);
}

void TWEB::mod_CheckCommandLine(  )
{
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
	    case 'h': pr_opt_descr(stdout); break;
	    case -1 : break;
	}
    } while(next_opt != -1);
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

