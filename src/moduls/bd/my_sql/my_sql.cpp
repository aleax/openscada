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
#define VERSION     "1.0.0"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "BD modul. Allow support of the BD MySQL."
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
	BDMySQL::BDMod *self_addr = NULL;

	if( AtMod.id == MOD_ID && AtMod.type == MOD_TYPE && AtMod.t_ver == VER_TYPE )
	    self_addr = new BDMySQL::BDMod( source );       

	return ( self_addr );
    }
}

using namespace BDMySQL;

//==============================================================================
//====================== BDMySQL::BDMod ========================================
//==============================================================================

BDMod::BDMod(string name)
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

BDMod::~BDMod()
{

}


TBD *BDMod::openBD( const string &name, bool create )
{
    int pos=0;
    string host = name.substr(pos,name.find(";",pos)-pos); pos = name.find(";",pos)+1;
    string user = name.substr(pos,name.find(";",pos)-pos); pos = name.find(";",pos)+1;
    string pass = name.substr(pos,name.find(";",pos)-pos); pos = name.find(";",pos)+1;
    string bd   = name.substr(pos,name.find(";",pos)-pos); pos = name.find(";",pos)+1;
    int    port = atoi(name.substr(pos,name.find(";",pos)-pos).c_str()); pos = name.find(";",pos)+1;
    string u_sock = name.substr(pos,name.find(";",pos)-pos);
    return(new MBD(name,host,user,pass,bd,port,u_sock,create));
}
	    
void BDMod::delBD( const string &name )
{
    MYSQL connect;
    int pos=0;
    
    string host = name.substr(pos,name.find(";",pos)-pos); pos = name.find(";",pos)+1;
    string user = name.substr(pos,name.find(";",pos)-pos); pos = name.find(";",pos)+1;
    string pass = name.substr(pos,name.find(";",pos)-pos); pos = name.find(";",pos)+1;
    string bd   = name.substr(pos,name.find(";",pos)-pos); pos = name.find(";",pos)+1;
    int    port = atoi(name.substr(pos,name.find(";",pos)-pos).c_str()); pos = name.find(";",pos)+1;
    string u_sock = name.substr(pos,name.find(";",pos)-pos);
    
    if(!mysql_init(&connect)) throw TError("%s: Error initializing client.\n",MOD_ID);
    connect.reconnect = 1;
    if(!mysql_real_connect(&connect,host.c_str(),user.c_str(),pass.c_str(),"",port,(u_sock.size())?u_sock.c_str():NULL,0))
	throw TError("%s: %s\n",MOD_ID,mysql_error(&connect));
    
    string req = "DROP DATABASE `"+bd+"`";
    if(mysql_real_query(&connect,req.c_str(),req.size()))
	throw TError("%s: %s\n",MOD_ID,mysql_error(&connect));
	
    mysql_close(&connect);
}

string BDMod::optDescr( )
{
    char buf[STR_BUF_LEN];

    snprintf(buf,sizeof(buf),I18N(
	"======================= The module <%s:%s> options =======================\n"
	"---------- Parameters of the module section <%s> in config file ----------\n"
	"def_port=<port>       default port for MySQL;\n"
	"def_user=<port>       default user for MySQL;\n\n"),
	MOD_TYPE,MOD_ID,MOD_ID);

    return(buf);
}			

void BDMod::modCheckCommandLine( )
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
	    case 'h': fprintf(stdout,optDescr().c_str()); break;
	    case -1 : break;
	}
    } while(next_opt != -1);
}

void BDMod::modUpdateOpt()
{
    try{ def_port = atoi( modCfgNode()->childGet("def_port")->text().c_str() ); }
    catch(...) {  }
    try{ def_user = modCfgNode()->childGet("def_user")->text(); }
    catch(...) {  }
}

//=============================================================
//====================== BDMySQL::MBD =========================
//=============================================================
MBD::MBD( string name, string _host, string _user, string _pass, string _bd, int _port, string _u_sock, bool create ) :
    TBD(name), host(_host), user(_user), pass(_pass), bd(_bd), port(_port), u_sock(_u_sock)	
{
    if(!mysql_init(&connect)) throw TError("%s: Error initializing client.\n",MOD_ID);
    connect.reconnect = 1;
    if(!mysql_real_connect(&connect,host.c_str(),user.c_str(),pass.c_str(),"",port,(u_sock.size())?u_sock.c_str():NULL,0))
	throw TError("%s: %s\n",MOD_ID,mysql_error(&connect));

    if( create )
    {
        string req = "CREATE DATABASE IF NOT EXISTS `"+bd+"`";
        if(mysql_real_query(&connect,req.c_str(),req.size()))
	throw TError("%s: %s\n",MOD_ID,mysql_error(&connect));
    }
    
    string req = "USE `"+bd+"`";
    if(mysql_real_query(&connect,req.c_str(),req.size()))
        throw TError("%s: %s\n",MOD_ID,mysql_error(&connect));		    
};

MBD::~MBD( )
{    
    mysql_close(&connect);
};

TTable *MBD::openTable( const string &name, bool create )
{
    return( new MTable(this,name,create) );
}

void MBD::delTable( const string &name )
{
    string req ="DROP TABLE `"+name+"`";
    if(mysql_real_query(&connect,req.c_str(),req.size()))
        throw TError("%s: %s\n",MOD_ID,mysql_error(&connect));		
}

//=============================================================
//=================== MBDMySQL::Table =========================
//=============================================================
MTable::MTable(MBD *bd, string name, bool create ) : TTable(name,bd), m_bd(bd)
{
    if( create )
    {	
	string req = "CREATE TABLE IF NOT EXISTS `"+name+"` (`name` char(10) NOT NULL DEFAULT '' PRIMARY KEY)";
	if(mysql_real_query(&m_bd->connect,req.c_str(),req.size()))
	    throw TError("%s: %s\n",MOD_ID,mysql_error(&m_bd->connect));
    }
}

MTable::~MTable(  )
{

}

void MTable::fieldList( const string &key, vector<string> &fields )
{
    MYSQL_RES *res = NULL;
    
    string req = "SELECT `"+key+"` FROM `"+name()+"`";
    if(mysql_real_query(&m_bd->connect,req.c_str(),req.size()))
        throw TError("%s: %s\n",MOD_ID,mysql_error(&m_bd->connect));
    if(mysql_field_count(&m_bd->connect) > 0)
    {
	if( !(res = mysql_store_result(&m_bd->connect)) )
	    throw TError("%s: %s\n",MOD_ID,mysql_error(&m_bd->connect));
	    
	MYSQL_ROW row; 
	while((row = mysql_fetch_row(res)))
	    fields.push_back(row[0]?row[0]:NULL);
	
	mysql_free_result(res);
    }
}

void MTable::fieldGet( TConfig &cfg )
{
    int num_fields;
    MYSQL_RES *res = NULL;
    
    //Get config fields list
    vector<string> cf_el;
    cfg.cfgList(cf_el);

    //Get avoid fields list
    string req = "SELECT * FROM `"+name()+"` WHERE 0";
    if(mysql_real_query(&m_bd->connect,req.c_str(),req.size()))
	throw TError("%s: %s\n",MOD_ID,mysql_error(&m_bd->connect));
    if( mysql_field_count(&m_bd->connect) == 0 ) return;
    if( !(res = mysql_store_result(&m_bd->connect)) )
    	throw TError("%s: %s\n",MOD_ID,mysql_error(&m_bd->connect));    
    try
    {    
	num_fields = mysql_num_fields(res);

	//Prepare request
	req = "SELECT ";
	string req_where;
	//Add fields list to queue
	bool next = false, next_wr = false;
	for( int i_cf = 0; i_cf < cf_el.size(); i_cf++ )
	    for( int i_fld = 0; i_fld < num_fields; i_fld++ )
		if( cf_el[i_cf] ==  mysql_fetch_field_direct(res,i_fld)->name )
		{
		    if( !next ) next = true; else req=req+",";
		    req=req+"`"+cf_el[i_cf]+"` ";
		
		    if( cfg.cfg(cf_el[i_cf]).fld().type()&F_KEY )
		    {
			if( !next_wr ) next_wr = true; else req_where=req_where+"AND ";
			req_where=req_where+"`"+cf_el[i_cf]+"`='"+cfg.cfg(cf_el[i_cf]).getS()+"' ";
		    }		
		}
	req = req+"FROM `"+name()+"` WHERE "+req_where;
	mysql_free_result(res);
    }catch(...){ mysql_free_result(res); throw; }
    
    //Query
    //printf("TEST 01: query: <%s>\n",req.c_str());
    if(mysql_real_query(&m_bd->connect,req.c_str(),req.size()))
        throw TError("%s: %s\n",MOD_ID,mysql_error(&m_bd->connect));
    if( mysql_field_count(&m_bd->connect) == 0 ) return;
    if( !(res = mysql_store_result(&m_bd->connect)) )
    	throw TError("%s: %s\n",MOD_ID,mysql_error(&m_bd->connect));        
    try
    {    
    	num_fields = mysql_num_fields(res);
	//Processing of query
	MYSQL_ROW row = mysql_fetch_row(res);
	if( !row ) throw TError("%s: Field no avoid!\n",MOD_ID);
	for( int i_cf = 0; i_cf < cf_el.size(); i_cf++ )
	    for( int i_fld = 0; i_fld < num_fields; i_fld++ )
		if( cf_el[i_cf] ==  mysql_fetch_field_direct(res,i_fld)->name )
		{
		    string val = row[i_fld]?row[i_fld]:NULL;
		    TCfg &u_cfg = cfg.cfg(cf_el[i_cf]);
		    if( u_cfg.fld().type()&T_STRING )	    		u_cfg.setS(val);
		    else if( u_cfg.fld().type()&(T_DEC|T_OCT|T_HEX) )	u_cfg.setI(atoi(val.c_str()));
		    else if( u_cfg.fld().type()&T_REAL )	    		u_cfg.setR(atof(val.c_str()));
		    else if( u_cfg.fld().type()&T_BOOL )			u_cfg.setB(atoi(val.c_str()));
		}
	mysql_free_result(res);
    }catch(...){ mysql_free_result(res); throw; }	
}

void MTable::fieldSet( TConfig &cfg )
{
    int num_fields;
    MYSQL_RES *res = NULL;
    
    //Get config fields list
    vector<string> cf_el;
    cfg.cfgList(cf_el);

    //Fix BD structure
    fieldFix(cfg);            

    //Get avoid fields list
    string req_where = "WHERE ";
    //Add key list to queue
    bool next = false;
    for( int i_cf = 0; i_cf < cf_el.size(); i_cf++ )
	if( cfg.cfg(cf_el[i_cf]).fld().type()&F_KEY )
	{
	    if( !next ) next = true; else req_where=req_where+"AND ";
	    req_where=req_where+"`"+cf_el[i_cf]+"`='"+cfg.cfg(cf_el[i_cf]).getS()+"' "; //!!!! May be check of field type
	}    
    //Query
    string req = "SELECT * FROM `"+name()+"` "+req_where;
    if(mysql_real_query(&m_bd->connect,req.c_str(),req.size()))
	throw TError("%s: %s\n",MOD_ID,mysql_error(&m_bd->connect));
    if( mysql_field_count(&m_bd->connect) == 0 ) return;
    if( !(res = mysql_store_result(&m_bd->connect)) )
    	throw TError("%s: %s\n",MOD_ID,mysql_error(&m_bd->connect));
    try
    {    
	num_fields = mysql_num_fields(res);

	if( res->row_count == 0 )
	{
	    //Add line
	    req = "INSERT INTO `"+name()+"` ";
	    string ins_name, ins_value;
	    next = false;
	    for( int i_cf = 0; i_cf < cf_el.size(); i_cf++ )
		{
		    if( !next ) next = true; 
		    else 
		    {
			ins_name=ins_name+",";
			ins_value=ins_value+",";
		    }
		    ins_name=ins_name+"`"+cf_el[i_cf]+"` ";		    
		    string val;
		    TCfg &u_cfg = cfg.cfg(cf_el[i_cf]);
		    if( u_cfg.fld().type()&T_STRING )	    		val = u_cfg.getS();
		    else if( u_cfg.fld().type()&(T_DEC|T_OCT|T_HEX) )	val = SYS->int2str(u_cfg.getI());
		    else if( u_cfg.fld().type()&T_REAL )	    	
		    {
			val = SYS->real2str(u_cfg.getR());
			for(int i_vl = 0; i_vl < val.size(); i_vl++) if(val[i_vl]==',') val[i_vl]='.';
		    }
		    else if( u_cfg.fld().type()&T_BOOL )		val = SYS->int2str(u_cfg.getB());
		    ins_value=ins_value+"'"+val+"' ";
		}
	    req = req + "("+ins_name+") VALUES ("+ins_value+")";
	}
	else
	{
	    //Update line    
	    req = "UPDATE `"+name()+"` SET ";
	    next = false;
	    for( int i_cf = 0; i_cf < cf_el.size(); i_cf++ )
		{
		    if( !next ) next = true; else req=req+",";
		    string val;
		    TCfg &u_cfg = cfg.cfg(cf_el[i_cf]);
		    if( u_cfg.fld().type()&T_STRING )	    		val = u_cfg.getS();
		    else if( u_cfg.fld().type()&(T_DEC|T_OCT|T_HEX) )	val = SYS->int2str(u_cfg.getI());
		    else if( u_cfg.fld().type()&T_REAL )	    		
		    {
			val = SYS->real2str(u_cfg.getR());
			for(int i_vl = 0; i_vl < val.size(); i_vl++) if(val[i_vl]==',') val[i_vl]='.';
		    }
		    else if( u_cfg.fld().type()&T_BOOL )			val = SYS->int2str(u_cfg.getB());		
		    req=req+"`"+cf_el[i_cf]+"`='"+val+"' ";
		}
	    req = req + req_where;
	}
	mysql_free_result(res);
    }catch(...){ mysql_free_result(res); throw; }	
    
    //Query
    //printf("TEST 02b: query: <%s>\n",req.c_str());
    if(mysql_real_query(&m_bd->connect,req.c_str(),req.size()))
        throw TError("%s: %s\n",MOD_ID,mysql_error(&m_bd->connect));
}

void MTable::fieldDel( TConfig &cfg )
{
    int num_fields;
    MYSQL_RES *res = NULL;
    
    //Get config fields list
    vector<string> cf_el;
    cfg.cfgList(cf_el);
    //Get avoid fields list
    string req = "SELECT * FROM `"+name()+"` WHERE 0";
    if(mysql_real_query(&m_bd->connect,req.c_str(),req.size()))
	throw TError("%s: %s\n",MOD_ID,mysql_error(&m_bd->connect));
    if( mysql_field_count(&m_bd->connect) == 0 ) return;
    if( !(res = mysql_store_result(&m_bd->connect)) )
    	throw TError("%s: %s\n",MOD_ID,mysql_error(&m_bd->connect));    
    try
    {    
	num_fields = mysql_num_fields(res);
    
	//Prepare request
	req = "DELETE FROM `"+name()+"` WHERE ";
	//Add key list to queue
	bool next = false;
	for( int i_cf = 0; i_cf < cf_el.size(); i_cf++ )
	    for( int i_fld = 0; i_fld < num_fields; i_fld++ )
		if( cf_el[i_cf] ==  mysql_fetch_field_direct(res,i_fld)->name && 
			cfg.cfg(cf_el[i_cf]).fld().type()&F_KEY )
		{
		    if( !next ) next = true; else req=req+"AND ";		
		    req=req+"`"+cf_el[i_cf]+"`='"+cfg.cfg(cf_el[i_cf]).getS()+"' "; //!!!! May be check of field type
		}
	mysql_free_result(res);
    }catch(...){ mysql_free_result(res); throw; }	
    
    //Query
    //printf("TEST 03: query: <%s>\n",req.c_str());
    if(mysql_real_query(&m_bd->connect,req.c_str(),req.size()))
        throw TError("%s: %s\n",MOD_ID,mysql_error(&m_bd->connect));
}

void MTable::fieldFix( TConfig &cfg )
{
    int num_fields;
    bool next;
    MYSQL_RES *res = NULL;
    
    //Get config fields list
    vector<string> cf_el;
    cfg.cfgList(cf_el);

    //Get avoid fields list
    string req = "SELECT * FROM `"+name()+"` WHERE 0";
    if(mysql_real_query(&m_bd->connect,req.c_str(),req.size()))
	throw TError("%s: %s\n",MOD_ID,mysql_error(&m_bd->connect));
    if( mysql_field_count(&m_bd->connect) == 0 ) return;
    if( !(res = mysql_store_result(&m_bd->connect)) )
    	throw TError("%s: %s\n",MOD_ID,mysql_error(&m_bd->connect));    
    try
    {    
	num_fields = mysql_num_fields(res);
    
	//Prepare request for fix structure
	req = "ALTER TABLE `"+name()+"` DROP PRIMARY KEY, ";
	next = false;
	for( int i_cf = 0; i_cf < cf_el.size(); i_cf++ )
	{
	    int i_fld;
	    for( i_fld = 0; i_fld < num_fields; i_fld++ ) 
		if( cf_el[i_cf] == mysql_fetch_field_direct(res,i_fld)->name ) break;
	    if( i_fld < num_fields )
	    {	    
		//Check field
		TCfg &u_cfg = cfg.cfg(cf_el[i_cf]);
		MYSQL_FIELD *b_fld = mysql_fetch_field_direct(res,i_fld);
		
		if( i_cf == i_fld )	//Check order 
		{	    
		    //printf("Field = <%s>; type = <%d>; len = <%d>; dec = <%d>; flags = <%d>!\n",
		    //    b_fld->name, b_fld->type, b_fld->length, b_fld->decimals, b_fld->flags );
		    if( (u_cfg.fld().type()&T_STRING && b_fld->type==FIELD_TYPE_STRING && 
			    u_cfg.fld().len() == b_fld->length ) )	continue;
		    else if( (u_cfg.fld().type()&T_REAL && b_fld->type==FIELD_TYPE_DOUBLE && 
			    u_cfg.fld().len() == b_fld->length && 
			    u_cfg.fld().dec() == b_fld->decimals ) )continue;
		    else if( (u_cfg.fld().type()&(T_DEC|T_OCT|T_HEX) && b_fld->type==FIELD_TYPE_LONG && 
			    u_cfg.fld().len() == b_fld->length ) )	continue;
		    else if( (u_cfg.fld().type()&T_BOOL && b_fld->type==FIELD_TYPE_TINY ) )	continue;
		}
    
    		//Change field
    		if( !next ) next = true; else req=req+",";
    		req=req+"CHANGE `"+cf_el[i_cf]+"` `"+cf_el[i_cf]+"` ";
    		fieldPrmSet(u_cfg,(i_cf>0)?cf_el[i_cf-1]:"",req);	    
    	    }
    	    else
    	    {
    		if( !next ) next = true; else req=req+",";
    		//Add field
    		req=req+"ADD `"+cf_el[i_cf]+"` ";
    		fieldPrmSet(cfg.cfg(cf_el[i_cf]),(i_cf>0)?cf_el[i_cf-1]:"",req);
    	    }
	}
	for( int i_fld = 0; i_fld < num_fields; i_fld++ ) 
	{
	    int i_cf;
	    for( i_cf = 0; i_cf < cf_el.size(); i_cf++ )
		if( cf_el[i_cf] == mysql_fetch_field_direct(res,i_fld)->name ) break;
	    if( i_cf >= cf_el.size() )
	    {
		if( !next ) next = true; else req=req+",";
		req=req+"DROP `"+mysql_fetch_field_direct(res,i_fld)->name+"` ";
	    }	
	}    
	mysql_free_result(res);
    }catch(...){ mysql_free_result(res); throw; }	
	
    if( next )
    {
	//printf("TEST 02a: query: <%s>\n",req.c_str());
	if(mysql_real_query(&m_bd->connect,req.c_str(),req.size()))
	    throw TError("%s: %s\n",MOD_ID,mysql_error(&m_bd->connect));
    }
}
    
void MTable::fieldPrmSet( TCfg &cfg, const string &last, string &req )
{
    //Type param
    if( cfg.fld().type()&T_STRING )   	
	req=req+"char("+SYS->int2str(cfg.fld().len())+") NOT NULL DEFAULT '"+cfg.fld().def()+"' ";
    else if( cfg.fld().type()&T_REAL )	
	req=req+"double("+SYS->int2str(cfg.fld().len())+","+SYS->int2str(cfg.fld().dec())+") NOT NULL DEFAULT '"+cfg.fld().def()+"' ";
    else if( cfg.fld().type()&(T_DEC|T_OCT|T_HEX) )	
	req=req+"int("+SYS->int2str(cfg.fld().len())+") NOT NULL DEFAULT '"+cfg.fld().def()+"' ";
    else if( cfg.fld().type()&T_BOOL )	
	req=req+"tinyint(1) NOT NULL DEFAULT '"+cfg.fld().def()+"' ";
    //Position param
    if( last.size() )	req=req+"AFTER `"+last+"` ";
    //Primary key
    if( cfg.fld().type()&F_KEY )
	req=req+",ADD PRIMARY KEY (`"+cfg.name()+"`) ";
}

