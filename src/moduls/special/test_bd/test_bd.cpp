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
#include <tbds.h>
#include "test_bd.h"

//============ Modul info! =====================================================
#define NAME_MODUL  "test_bd"
#define NAME_TYPE   "Special"
#define VER_TYPE    VER_SPC
#define SUB_TYPE    "TEST"
#define VERSION     "0.0.3"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "BD test module: Open BD, ... ."
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

    TModule *attach( SAtMod &AtMod, string source )
    {
	BDTest::TTest *self_addr = NULL;

	if( AtMod.name == NAME_MODUL && AtMod.type == NAME_TYPE && AtMod.t_ver == VER_TYPE )
	    self_addr = new BDTest::TTest( source );       

	return ( self_addr );
    }
    /*
    TModule *attach( char *FName, int n_mod )
    {
	BDTest::TTest *self_addr;
	if(n_mod==0) self_addr = new BDTest::TTest( FName );
	else         self_addr = NULL;
	return ( self_addr );
    }
    */
}

using namespace BDTest;

//==============================================================================
//================= BDTest::TTest ==============================================
//==============================================================================
TTest::TTest( string name )
{
    NameModul = NAME_MODUL;
    NameType  = NAME_TYPE;
    Vers      = VERSION;
    Autors    = AUTORS;
    DescrMod  = DESCRIPTION;
    License   = LICENSE;
    Source    = name;
}

TTest::~TTest()
{

}

string TTest::mod_info( const string name )
{
    if( name == "SubType" ) return(SUB_TYPE);
    else return( TModule::mod_info( name) );
}

void TTest::mod_info( vector<string> &list )
{
    TModule::mod_info(list);
    list.push_back("SubType");
}

void TTest::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "============== Module %s command line options =======================\n"
    "------------------ Fields <%s> sections of config file --------------\n"
    "\n",NAME_MODUL,NAME_MODUL);
}

void TTest::mod_CheckCommandLine(  )
{
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
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

void TTest::mod_UpdateOpt( )
{

}

void TTest::start(  )
{
    m_put_s("TEST",MESS_DEBUG,"***** Begin test block *****");
    TBDS &bd = Owner().Owner().BD();    
    //------------------- Test MySQL BD -----------------------
    try
    {
	m_put_s("TEST",MESS_DEBUG,"Open table");
	SHDBD t_hd = bd.open( SBDS("my_sql",";;;oscada;;/var/lib/mysql/mysql.sock;","generic"), true);
	//string val = bd.at(t_hd).GetCodePage( );
	//m_put("TEST",MESS_DEBUG,"table val = %s",val.c_str());
	bd.close(t_hd);
	bd.del( SBDS("my_sql",";;;oscada;;/var/lib/mysql/mysql.sock;","generic") );
    }catch(TError error)
    { m_put_s("TEST",MESS_DEBUG,error.what()); }
    m_put_s("TEST",MESS_DEBUG,"***** End test block *****");
}

