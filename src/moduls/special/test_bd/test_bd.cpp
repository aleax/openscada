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
#define MOD_ID      "test_bd"
#define MOD_NAME    "Test DB"
#define MOD_TYPE    "Special"
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
	    AtMod.id	= MOD_ID;
	    AtMod.type  = MOD_TYPE;
	    AtMod.t_ver = VER_TYPE;
	}
	else
	    AtMod.id	= "";

	return( AtMod );
    }

    TModule *attach( const SAtMod &AtMod, const string &source )
    {
	BDTest::TTest *self_addr = NULL;

	if( AtMod.id == MOD_ID && AtMod.type == MOD_TYPE && AtMod.t_ver == VER_TYPE )
	    self_addr = new BDTest::TTest( source );       

	return ( self_addr );
    }
}

using namespace BDTest;

//==============================================================================
//================= BDTest::TTest ==============================================
//==============================================================================
TTest::TTest( string name )
{
    mId 	= MOD_ID;
    mName       = MOD_NAME;
    mType  	= MOD_TYPE;
    Vers      	= VERSION;
    Autors    	= AUTORS;
    DescrMod  	= DESCRIPTION;
    License   	= LICENSE;
    Source    	= name;
}

TTest::~TTest()
{

}

string TTest::modInfo( const string &name )
{
    if( name == "SubType" ) return(SUB_TYPE);
    else return( TModule::modInfo( name) );
}

void TTest::modInfo( vector<string> &list )
{
    TModule::modInfo(list);
    list.push_back("SubType");
}

void TTest::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "======================= The module <%s:%s> options =======================\n"
    "---------- Parameters of the module section <%s> in config file ----------\n\n",
    MOD_TYPE,MOD_ID,MOD_ID);
}

void TTest::modCheckCommandLine(  )
{
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{"help"    ,0,NULL,'h'},
	{NULL      ,0,NULL,0  }
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

void TTest::modUpdateOpt( )
{

}

void TTest::start(  )
{
    m_put_s("TEST",MESS_DEBUG,"***** Begin test block *****");
    //------------------- Test MySQL BD -----------------------
    try
    {
	SBDS bds("my_sql",";;;oscada;;/var/lib/mysql/mysql.sock;","generic");
	
	m_put_s("TEST",MESS_DEBUG,"Test BD");
	AutoHD<TTable> tbl = Owner().Owner().BD().open(bds);	
	//Table operations
	tbl.free();
	Owner().Owner().BD().close(bds);
    }catch(TError error)
    { m_put_s("TEST",MESS_DEBUG,error.what()); }
    m_put_s("TEST",MESS_DEBUG,"***** End test block *****");
}

