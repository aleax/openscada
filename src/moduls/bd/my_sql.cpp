/* Test Modul
** ==============================================================
*/


#include <getopt.h>
#include <string>
#include <mysql/mysql.h>

#include "../../tsys.h"
#include "../../tkernel.h"
#include "../../tmessage.h"
#include "dbf.h"
#include "my_sql.h"


//============ Modul info! =====================================================
#define NAME_MODUL  "my_sql"
#define NAME_TYPE   "BaseDate"
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
	TMY_SQL *self_addr = NULL;

	if( AtMod.name == NAME_MODUL && AtMod.type == NAME_TYPE && AtMod.t_ver == VER_TYPE )
	    self_addr = new TMY_SQL( source );       

	return ( self_addr );
    }
																			      
    /*
    TModule *attach( char *FName, int n_mod )
    {
	TMY_SQL *self_addr;
	if(n_mod==0) self_addr =  new TMY_SQL( FName );
	else         self_addr = NULL;
	return static_cast< TModule *>( self_addr );
    }
    */
}

TMY_SQL::TMY_SQL(string name)
{
    NameModul = NAME_MODUL;
    NameType  = NAME_TYPE;
    Vers      = VERSION;
    Autors    = AUTORS;
    DescrMod  = DESCRIPTION;
    License   = LICENSE;
    Source    = name;
}

TMY_SQL::~TMY_SQL()
{

}


TBD *TMY_SQL::BDOpen( string name, bool create )
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
    "============== Module %s command line options =======================\n"
    "------------------ Fields <%s> sections of config file --------------\n"
    "def_port=<port>       set number of default port for MySQL;\n"
    "def_user=<port>       set name of default user for MySQL;\n"
    "\n",NAME_MODUL,NAME_MODUL);
}

void TMY_SQL::mod_CheckCommandLine( )
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

void TMY_SQL::mod_UpdateOpt()
{
    try{ def_port = atoi( mod_XMLCfgNode()->get_child("def_port")->get_text().c_str() ); }
    catch(...) {  }
    try{ def_user = mod_XMLCfgNode()->get_child("def_user")->get_text(); }
    catch(...) {  }
}
/*
void TMY_SQL::connect( void *obj )
{
    Tbd = (TTipBD *)obj;
}
*/
//=============================================================
//====================== TBD_my_sql ===========================
//=============================================================
TBD_my_sql::TBD_my_sql( string name, string _host, string _user, string _pass, string _bd, int _port, string _u_sock, bool create ) :
    TBD(name), host(_host), user(_user), pass(_pass), bd(_bd), port(_port), u_sock(_u_sock)	
{

    if(!mysql_init(&connect)) throw TError("%s: Error initializing client.\n",NAME_MODUL);    
    if(!mysql_real_connect(&connect,host.c_str(),user.c_str(),pass.c_str(),"",port,(u_sock.size())?u_sock.c_str():NULL,0))
	throw TError("%s: Connection error: %s\n",NAME_MODUL,mysql_error(&connect));
    if(mysql_select_db(&connect,bd.c_str()))
    {
	if(create == false)
    	    throw TError("%s: Select bd error: %s\n",NAME_MODUL,mysql_error(&connect));
	else if(mysql_create_db(&connect,bd.c_str()))
    	    throw TError("%s: Create bd error: %s\n",NAME_MODUL,mysql_error(&connect));	
    }
};

TBD_my_sql::~TBD_my_sql( )
{    
    mysql_close(&connect);
};

TTable *TBD_my_sql::TableOpen( string name, bool create )
{
    return( new TTable_my_sql(this,name,create) );
}

void TBD_my_sql::TableDel( string name )
{
    char SQL[150];
    snprintf(SQL,sizeof(SQL),"DROP TABLE %s;",name.c_str());
    if( mysql_real_query(&connect, SQL, strlen(SQL)) < 0)
	throw TError("%s: %s",NAME_MODUL,mysql_error(&connect));
}

//=============================================================
//====================== TTable_my_sql ========================
//=============================================================
TTable_my_sql::TTable_my_sql(TBD_my_sql *bd, string name, bool create) : TTable(name), m_bd(bd)
{
    char SQL[150];
    snprintf(SQL,sizeof(SQL),"SELECT * FROM %s ;",name.c_str());
    if( mysql_real_query(&m_bd->connect, SQL, strlen(SQL)) < 0)
    {
	if(create == false) throw TError("%s: %s",NAME_MODUL,mysql_error(&m_bd->connect));
    	snprintf(SQL,sizeof(SQL),"CREATE TABLE %s (id char(12) not null primary key);",name.c_str());
    	if( mysql_real_query(&m_bd->connect, SQL, strlen(SQL)) < 0)
	    throw TError("%s: %s",NAME_MODUL,mysql_error(&m_bd->connect));
    }
    //mysql_shutdown(&m_bd->connect);
    //int res = mysql_select_db(&m_bd->connect, m_bd->bd.c_str());
    //App->Mess->put(1,"test %d %s",res,mysql_error(&m_bd->connect));
    /*int res;
    sprintf(SQL,"SELECT COUNT(*) FROM %s",name.c_str());
    if( (res = mysql_real_query(&m_bd->connect,SQL,strlen(SQL))) < 0)
	throw TError("%s: %s",NAME_MODUL,mysql_error(&m_bd->connect));
    App->Mess->put(1,"test %d",res);
    */
}

TTable_my_sql::~TTable_my_sql(  )
{

}

string TTable_my_sql::GetCellS( int colm, int line )
{
    return("");
}

double TTable_my_sql::GetCellR( int colm, int line )
{
    return(0.0);
}

int TTable_my_sql::GetCellI( int colm, int line )
{
    return(0);
}

bool TTable_my_sql::GetCellB( int colm, int line )
{
    return(true);
}

int TTable_my_sql::NLines( )
{
    char SQL[150];

    sprintf(SQL,"SELECT * FROM %s",Name().c_str());
    if( mysql_real_query(&m_bd->connect,SQL,strlen(SQL)) < 0)
	throw TError("%s: %s",NAME_MODUL,mysql_error(&m_bd->connect));
    return(1);
}

string TTable_my_sql::GetCodePage( )
{
    return(mysql_character_set_name(&m_bd->connect));
}

