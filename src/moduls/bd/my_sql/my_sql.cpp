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
#include <mysql/mysql.h>

#include <tsys.h>
#include <tkernel.h>
#include <tmessage.h>
#include "my_sql.h"


//============ Modul info! =====================================================
#define MOD_ID      "my_sql"
#define MOD_NAME    "DB MySQL"
#define MOD_TYPE    "BD"
#define VER_TYPE    VER_BD
#define VERSION     "0.0.2"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "Support MySQL BD!"
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
	TMY_SQL *self_addr = NULL;

	if( AtMod.id == MOD_ID && AtMod.type == MOD_TYPE && AtMod.t_ver == VER_TYPE )
	    self_addr = new TMY_SQL( source );       

	return ( self_addr );
    }
}

TMY_SQL::TMY_SQL(string name)
{
    mId 	= MOD_ID;
    mName	= MOD_NAME;
    mType  	= MOD_TYPE;
    Vers      	= VERSION;
    Autors    	= AUTORS;
    DescrMod  	= DESCRIPTION;
    License   	= LICENSE;
    Source    	= name;
}

TMY_SQL::~TMY_SQL()
{

}


TBD *TMY_SQL::BDOpen( const string &name, bool create )
{
    int pos=0;
    string host = name.substr(pos,name.find(";",pos)-pos); pos = name.find(";",pos)+1;
    string user = name.substr(pos,name.find(";",pos)-pos); pos = name.find(";",pos)+1;
    string pass = name.substr(pos,name.find(";",pos)-pos); pos = name.find(";",pos)+1;
    string bd   = name.substr(pos,name.find(";",pos)-pos); pos = name.find(";",pos)+1;
    int    port = atoi(name.substr(pos,name.find(";",pos)-pos).c_str()); pos = name.find(";",pos)+1;
    string u_sock = name.substr(pos,name.find(";",pos)-pos);
    return(new TBD_my_sql(name,host,user,pass,bd,port,u_sock,create));
}

void TMY_SQL::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "======================= The module <%s:%s> options =======================\n"
    "---------- Parameters of the module section <%s> in config file ----------\n"
    "def_port=<port>       default port for MySQL;\n"
    "def_user=<port>       default user for MySQL;\n"
    "\n",MOD_TYPE,MOD_ID,MOD_ID);
}

void TMY_SQL::modCheckCommandLine( )
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

void TMY_SQL::modUpdateOpt()
{
    try{ def_port = atoi( modXMLCfgNode()->get_child("def_port")->get_text().c_str() ); }
    catch(...) {  }
    try{ def_user = modXMLCfgNode()->get_child("def_user")->get_text(); }
    catch(...) {  }
}

//=============================================================
//====================== TBD_my_sql ===========================
//=============================================================
TBD_my_sql::TBD_my_sql( string name, string _host, string _user, string _pass, string _bd, int _port, string _u_sock, bool create ) :
    TBD(name), host(_host), user(_user), pass(_pass), bd(_bd), port(_port), u_sock(_u_sock)	
{

    if(!mysql_init(&connect)) throw TError("%s: Error initializing client.\n",MOD_ID);    
    if(!mysql_real_connect(&connect,host.c_str(),user.c_str(),pass.c_str(),"",port,(u_sock.size())?u_sock.c_str():NULL,0))
	throw TError("%s: Connection error: %s\n",MOD_ID,mysql_error(&connect));
    if(mysql_select_db(&connect,bd.c_str()))
    {
	if(create == false)
    	    throw TError("%s: Select bd error: %s\n",MOD_ID,mysql_error(&connect));
	else if(mysql_create_db(&connect,bd.c_str()))
    	    throw TError("%s: Create bd error: %s\n",MOD_ID,mysql_error(&connect));	
    }
};

TBD_my_sql::~TBD_my_sql( )
{    
    mysql_close(&connect);
};

TTable *TBD_my_sql::TableOpen( const string &name, bool create )
{
    return( new TTable_my_sql(this,name,create) );
}

void TBD_my_sql::TableDel( const string &name )
{
    char SQL[150];
    snprintf(SQL,sizeof(SQL),"DROP TABLE %s;",name.c_str());
    if( mysql_real_query(&connect, SQL, strlen(SQL)) < 0)
	throw TError("%s: %s",MOD_ID,mysql_error(&connect));
}

//=============================================================
//====================== TTable_my_sql ========================
//=============================================================
TTable_my_sql::TTable_my_sql(TBD_my_sql *bd, string name, bool create ) : TTable(name,bd), m_bd(bd)
{
    char SQL[150];
    snprintf(SQL,sizeof(SQL),"SELECT * FROM %s ;",name.c_str());
    if( mysql_real_query(&m_bd->connect, SQL, strlen(SQL)) < 0)
    {
	if(create == false) throw TError("%s: %s",MOD_ID,mysql_error(&m_bd->connect));
    	snprintf(SQL,sizeof(SQL),"CREATE TABLE %s (id char(12) not null primary key);",name.c_str());
    	if( mysql_real_query(&m_bd->connect, SQL, strlen(SQL)) < 0)
	    throw TError("%s: %s",MOD_ID,mysql_error(&m_bd->connect));
    }
    //mysql_shutdown(&m_bd->connect);
    //int res = mysql_select_db(&m_bd->connect, m_bd->bd.c_str());
    //App->Mess->put(1,"test %d %s",res,mysql_error(&m_bd->connect));
    /*int res;
    sprintf(SQL,"SELECT COUNT(*) FROM %s",name.c_str());
    if( (res = mysql_real_query(&m_bd->connect,SQL,strlen(SQL))) < 0)
	throw TError("%s: %s",MOD_ID,mysql_error(&m_bd->connect));
    App->Mess->put(1,"test %d",res);
    */
}

TTable_my_sql::~TTable_my_sql(  )
{

}

string TTable_my_sql::getCellS( int colm, int line )
{
    return("");
}

double TTable_my_sql::getCellR( int colm, int line )
{
    return(0.0);
}

int TTable_my_sql::getCellI( int colm, int line )
{
    return(0);
}

bool TTable_my_sql::getCellB( int colm, int line )
{
    return(true);
}

int TTable_my_sql::nLines( )
{
    char SQL[150];

    sprintf(SQL,"SELECT * FROM %s",name().c_str());
    if( mysql_real_query(&m_bd->connect,SQL,strlen(SQL)) < 0)
	throw TError("%s: %s",MOD_ID,mysql_error(&m_bd->connect));
    return(1);
}

string TTable_my_sql::getCodePage( )
{
    return(mysql_character_set_name(&m_bd->connect));
}

