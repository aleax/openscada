/* Test Modul
** ==============================================================
*/


#include <getopt.h>
#include <string>
#include <mysql/mysql.h>

#include "../../tkernel.h"
#include "../../tmessage.h"
#include "dbf.h"
#include "my_sql.h"


//============ Modul info! =====================================================
#define NAME_MODUL  "my_sql"
#define NAME_TYPE   "BaseDate"
#define VERSION     "0.1"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "Support MySQL BD!"
#define LICENSE     "GPL"
//==============================================================================

extern "C" TModule *attach( char *FName, int n_mod );

SExpFunc TMY_SQL::ExpFuncLc[]=
{ 
    {"BDOpen", (void(TModule::*)( )) &TMY_SQL::BDOpen,"TBD *BDOpen( string name, bool create );","Open directory, stored BD tables",10,0}
};

TMY_SQL::TMY_SQL(char *name)
{
    NameModul = NAME_MODUL;
    NameType  = NAME_TYPE;
    Vers      = VERSION;
    Autors    = AUTORS;
    DescrMod  = DESCRIPTION;
    License   = LICENSE;
    FileName  = strdup(name);

    ExpFunc  = (SExpFunc *)ExpFuncLc;
    NExpFunc = sizeof(ExpFuncLc)/sizeof(SExpFunc);
}

TMY_SQL::~TMY_SQL()
{
    free(FileName);	
}

TModule *attach( char *FName, int n_mod )
{
    TMY_SQL *self_addr;
    if(n_mod==0) self_addr =  new TMY_SQL( FName );
    else         self_addr = NULL;
    return static_cast< TModule *>( self_addr );
}

void TMY_SQL::info( const string & name, string & info )
{
    info.erase();
    TModule::info(name,info);
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
    return(new TBD_my_sql(host,user,pass,bd,port,u_sock,create));
}

void TMY_SQL::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "================== Module %s options ================================\n"
    "------------------ Fields <%s> sections of config file --------------\n"
    "def_port=<port>       set number of default port for MySQL;\n"
    "def_user=<port>       set name of default user for MySQL;\n"
    "\n",NAME_MODUL,NAME_MODUL);
}

void TMY_SQL::CheckCommandLine( char **argv, int argc )
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
	next_opt=getopt_long(argc,(char * const *)argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': pr_opt_descr(stdout); break;
	    case -1 : break;
	}
    } while(next_opt != -1);
}

void TMY_SQL::UpdateOpt()
{
    try{ def_port = atoi(owner->owner->GetOpt(NAME_MODUL,"def_port").c_str()); } catch(...){  }
    try{ def_user = owner->owner->GetOpt(NAME_MODUL,"def_user"); }               catch(...){  }
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
TBD_my_sql::TBD_my_sql( string _host, string _user, string _pass, string _bd, int _port, string _u_sock, bool create ) :
    host(_host), user(_user), pass(_pass), bd(_bd), port(_port), u_sock(_u_sock)	
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
    sprintf(SQL,"DROP %s;",name.c_str());
    if( mysql_real_query(&connect, SQL, strlen(SQL)) < 0)
	throw TError("%s: %s",NAME_MODUL,mysql_error(&connect));
}

//=============================================================
//====================== TTable_my_sql ========================
//=============================================================
TTable_my_sql::TTable_my_sql(TBD_my_sql *_bd,string _name, bool create) : bd(_bd), name(_name)
{
    char SQL[150];
    sprintf(SQL,"SELECT * FROM %s ;",name.c_str());
    if( mysql_real_query(&bd->connect, SQL, strlen(SQL)) < 0)
    {
	if(create == false) throw TError("%s: %s",NAME_MODUL,mysql_error(&bd->connect));
    	sprintf(SQL,"CREATE TABLE %s ( N int NULL ) ;",name.c_str());
    	if( mysql_real_query(&bd->connect, SQL, strlen(SQL)) < 0)
	    throw TError("%s: %s",NAME_MODUL,mysql_error(&bd->connect));
    }
    //mysql_shutdown(&bd->connect);
    //int res = mysql_select_db(&bd->connect, bd->bd.c_str());
    //App->Mess->put(1,"test %d %s",res,mysql_error(&bd->connect));
    /*int res;
    sprintf(SQL,"SELECT COUNT(*) FROM %s",name.c_str());
    if( (res = mysql_real_query(&bd->connect,SQL,strlen(SQL))) < 0)
	throw TError("%s: %s",NAME_MODUL,mysql_error(&bd->connect));
    App->Mess->put(1,"test %d",res);
    */
}

TTable_my_sql::~TTable_my_sql(  )
{

}

string TTable_my_sql::_GetCellS( int colm, int line )
{
    return("");
}

double TTable_my_sql::_GetCellR( int colm, int line )
{
    return(0.0);
}

int TTable_my_sql::_GetCellI( int colm, int line )
{
    return(0);
}

bool TTable_my_sql::_GetCellB( int colm, int line )
{
    return(true);
}

int TTable_my_sql::_NLines( )
{
    char SQL[150];

    sprintf(SQL,"SELECT * FROM %s",name.c_str());
    if( mysql_real_query(&bd->connect,SQL,strlen(SQL)) < 0)
	throw TError("%s: %s",NAME_MODUL,mysql_error(&bd->connect));
    return(1);
}

string TTable_my_sql::_GetCodePage( )
{
    return(mysql_character_set_name(&bd->connect));
}

