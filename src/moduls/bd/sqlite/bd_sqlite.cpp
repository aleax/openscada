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
#include <errno.h>
#include <sqlite3.h>

#include <tsys.h>
#include <tkernel.h>
#include <tmessage.h>
#include "bd_sqlite.h"


//============ Modul info! =====================================================
#define MOD_ID      "sqlite"
#define MOD_NAME    "DB SQLite"
#define MOD_TYPE    "BD"
#define VER_TYPE    VER_BD
#define VERSION     "0.0.1"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "BD modul. Allow support of the BD SQLite."
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
	BDSQLite::BDMod *self_addr = NULL;

	if( AtMod.id == MOD_ID && AtMod.type == MOD_TYPE && AtMod.t_ver == VER_TYPE )
	    self_addr = new BDSQLite::BDMod( source );       

	return ( self_addr );
    }
}

using namespace BDSQLite;

//==============================================================================
//====================== BDSQLite::BDMod =======================================
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
    return(new MBD(name,create));
}
	    
void BDMod::delBD( const string &name )
{
    if(remove(name.c_str()) != 0)
        throw TError("%s: delete bd %s error: <%s>",MOD_ID,name.c_str(),strerror(errno));
}

string BDMod::optDescr( )
{
    char buf[STR_BUF_LEN];

    snprintf(buf,sizeof(buf),I18N(
	"======================= The module <%s:%s> options =======================\n"
	"---------- Parameters of the module section <%s> in config file ----------\n"
	"\n"),
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

}

//=============================================================
//====================== BDSQLite::MBD ========================
//=============================================================
MBD::MBD( string name, bool create ) : TBD(name), m_db(NULL), openTrans(false)
{
    int rc;
    
    rc = sqlite3_open(name.c_str(), &m_db); 
    if( rc )
    { 
	string err = sqlite3_errmsg(m_db);
	sqlite3_close(m_db);
	throw TError("%s: %s\n",MOD_ID, err.c_str());
    }
};

MBD::~MBD( )
{    
    sqlite3_close(m_db);
};

TTable *MBD::openTable( const string &name, bool create )
{
    return( new MTable(this,name,create) );
}

void MBD::delTable( const string &name )
{
    string req ="DROP TABLE \""+name+"\";";
    sqlReq( req.c_str() );
}

void MBD::sqlReq( const string &req, vector< vector<string> > *tbl )
{    
    char *zErrMsg;
    int rc,nrow=0,ncol=0;
    char **result;
    
    //printf("TEST 03: query: <%s>\n",req.c_str());
    rc = sqlite3_get_table( m_db,req.c_str(),&result, &nrow, &ncol, &zErrMsg );
    if( rc != SQLITE_OK ) throw TError("%s: %s\n",MOD_ID, zErrMsg);
    if( tbl != NULL && ncol > 0 )
    {
	vector<string> row;
	//Add head
	for(int i=0; i < ncol; i++)	
	    row.push_back(result[i]?result[i]:"");
	tbl->push_back(row);
	//Add data
	for(int i=0; i < nrow; i++)
    	{
	    row.clear();
    	    for(int ii=0; ii < ncol; ii++)	
		row.push_back(result[(i+1)*ncol+ii]?result[(i+1)*ncol+ii]:"");
    	    tbl->push_back(row);
	}    
    }
    sqlite3_free_table(result);
}


//=============================================================
//=================== MBDMySQL::Table =========================
//=============================================================
MTable::MTable(MBD *bd, string name, bool create ) : TTable(name,bd), m_bd(bd), my_trans(false)
{
    try
    {	
	string req;
	//Open only transaction for SQLite BD
	if( !bd->openTrans ) 
	{
	    bd->openTrans = true;
	    my_trans = true;
	    req ="BEGIN;";
	}
	req = req+"SELECT * FROM \""+name+"\" LIMIT 0;";
	bd->sqlReq( req.c_str() );
    }
    catch(...) { if( !create ) throw; }
}

MTable::~MTable(  )
{
    if( my_trans )
    { 
	m_bd->sqlReq( "COMMIT;" );
	m_bd->openTrans = false;
    }
}

void MTable::fieldList( const string &key, vector<string> &fields )
{
    vector< vector<string> > tbl;
    
    string req =string("SELECT \"")+key+"\" FROM \""+name()+"\";";
    m_bd->sqlReq( req.c_str(), &tbl );

    for( int i=1; i < tbl.size(); i++ )
	fields.push_back(tbl[i][0]);
}

void MTable::fieldGet( TConfig &cfg )
{
    vector< vector<string> > tbl;
    
    //Get config fields list
    vector<string> cf_el;
    cfg.cfgList(cf_el);
    
    //Get avoid fields list
    string req ="PRAGMA table_info("+name()+");";
    m_bd->sqlReq( req.c_str(), &tbl );	
    if( tbl.size() == 0 ) throw TError("%s: Table <%s> is empty!\n",MOD_ID, name().c_str());
    //Prepare request
    req = "SELECT * ";
    string req_where;
    //Add fields list to queue    
    bool next = false, next_wr = false;
    for( int i_cf = 0; i_cf < cf_el.size(); i_cf++ )
	for( int i_fld = 1; i_fld < tbl.size(); i_fld++ )
	    if( cf_el[i_cf] == tbl[i_fld][1] )
	    {
		//if( !next ) next = true; else req=req+",";
		//req=req+"\""+tbl[i_fld][1]+"\"";
		
		if( cfg.cfg(cf_el[i_cf]).fld().type()&F_KEY )
		{
		    if( !next_wr ) next_wr = true; else req_where=req_where+"AND ";
		    req_where=req_where+"\""+tbl[i_fld][1]+"\"='"+cfg.cfg(cf_el[i_cf]).getS()+"'";
		}		
	    }
    req = req+" FROM \""+name()+"\" WHERE "+req_where+";";
    //Query
    tbl.clear();
    m_bd->sqlReq( req.c_str(), &tbl );
    if( tbl.size() < 2 ) throw TError("%s: Table <%s>. Row no avoid!\n",MOD_ID, name().c_str());
    //Processing of query
    for( int i_cf = 0; i_cf < cf_el.size(); i_cf++ )
	for( int i_fld = 0; i_fld < tbl[0].size(); i_fld++ )
	    if( cf_el[i_cf] == tbl[0][i_fld] )
	    {
		string val = tbl[1][i_fld];
		TCfg &u_cfg = cfg.cfg(cf_el[i_cf]);
		if( u_cfg.fld().type()&T_STRING )	u_cfg.setS(val);
		else if( u_cfg.fld().type()&(T_DEC|T_OCT|T_HEX) )	u_cfg.setI(atoi(val.c_str()));
		else if( u_cfg.fld().type()&T_REAL )	u_cfg.setR(atof(val.c_str()));
		else if( u_cfg.fld().type()&T_BOOL )	u_cfg.setB(atoi(val.c_str()));
	    }
}

void MTable::fieldSet( TConfig &cfg )
{
    vector< vector<string> > tbl, tbl_str;
    
    //Get config fields list
    vector<string> cf_el;
    cfg.cfgList(cf_el);
    
    //Fix BD structure
    fieldFix(cfg);    
    
    //Get avoid fields list
    string req ="PRAGMA table_info("+name()+");";
    m_bd->sqlReq( req.c_str(), &tbl_str );
    if( tbl_str.size() == 0 ) throw TError("%s: Table <%s> is empty!\n",MOD_ID, name().c_str());		        
    
    //Get avoid fields list
    string req_where = "WHERE ";
    //Add key list to queue
    bool next = false;
    for( int i_cf = 0; i_cf < cf_el.size(); i_cf++ )
        for( int i_fld = 0; i_fld < tbl_str.size(); i_fld++ )
            if( cf_el[i_cf] == tbl_str[i_fld][1] && cfg.cfg(cf_el[i_cf]).fld().type()&F_KEY )
            {
		if( !next ) next = true; else req_where=req_where+"AND ";
		req_where=req_where+"\""+cf_el[i_cf]+"\"='"+cfg.cfg(cf_el[i_cf]).getS()+"' "; //!!!! May be check of field type
	    }    
    //Query
    req = "SELECT * FROM \""+name()+"\" "+req_where+";";
    m_bd->sqlReq( req.c_str(), &tbl );
    if( tbl.size() < 2 )
    {
	//Add line
	req = "INSERT INTO \""+name()+"\" ";
	string ins_name, ins_value;
	next = false;
	for( int i_cf = 0; i_cf < cf_el.size(); i_cf++ )
	    for( int i_fld = 0; i_fld < tbl_str.size(); i_fld++ )
                if( cf_el[i_cf] == tbl_str[i_fld][1] )
        	{				    
		    if( !next ) next = true; 
		    else 
		    {
			ins_name=ins_name+",";
			ins_value=ins_value+",";
		    }
		    ins_name=ins_name+"\""+cf_el[i_cf]+"\" ";		    
		    string val;
		    TCfg &u_cfg = cfg.cfg(cf_el[i_cf]);
		    if( u_cfg.fld().type()&T_STRING )		val = u_cfg.getS();
		    else if( u_cfg.fld().type()&(T_DEC|T_OCT|T_HEX) )	val = SYS->int2str(u_cfg.getI());
		    else if( u_cfg.fld().type()&T_REAL )	    	
		    {
			val = SYS->real2str(u_cfg.getR());
			for(int i_vl = 0; i_vl < val.size(); i_vl++) if(val[i_vl]==',') val[i_vl]='.';
		    }
		    else if( u_cfg.fld().type()&T_BOOL )	val = SYS->int2str(u_cfg.getB());
		    ins_value=ins_value+"'"+val+"' ";
		}
      	req = req + "("+ins_name+") VALUES ("+ins_value+")";
    }
    else
    {
	//Update line    
	req = "UPDATE \""+name()+"\" SET ";
	next = false;
	for( int i_cf = 0; i_cf < cf_el.size(); i_cf++ )
    	    for( int i_fld = 0; i_fld < tbl_str.size(); i_fld++ )
        	if( cf_el[i_cf] == tbl_str[i_fld][1] )
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
		    else if( u_cfg.fld().type()&T_BOOL )		val = SYS->int2str(u_cfg.getB());		
		    req=req+"\""+cf_el[i_cf]+"\"='"+val+"' ";
		}
    	req = req + req_where;
    }
    req += ";";
    //Query
    //printf("TEST 02: query: <%s>\n",req.c_str());
    m_bd->sqlReq( req.c_str() );
    //printf("TEST 01b: End from set\n");
}

void MTable::fieldDel( TConfig &cfg )
{
    vector< vector<string> > tbl;
    
    //Get config fields list
    vector<string> cf_el;
    cfg.cfgList(cf_el);

    //Get avoid fields list
    string req ="PRAGMA table_info("+name()+");";
    m_bd->sqlReq( req.c_str(), &tbl );
    if( tbl.size() == 0 ) throw TError("%s: Table <%s> is empty!\n",MOD_ID, name().c_str());
    //Prepare request
    req = "DELETE FROM \""+name()+"\" WHERE ";
    //Add key list to queue
    bool next = false;
    for( int i_cf = 0; i_cf < cf_el.size(); i_cf++ )
	for( int i_fld = 1; i_fld < tbl.size(); i_fld++ )
	    if( cf_el[i_cf] == tbl[i_fld][1] && cfg.cfg(cf_el[i_cf]).fld().type()&F_KEY )
	    {
		if( !next ) next = true; else req=req+"AND ";		
		req=req+"\""+tbl[i_fld][1]+"\"='"+cfg.cfg(cf_el[i_cf]).getS()+"' "; //!!!! May be check of field type
	    }
    req += ";";
    m_bd->sqlReq( req.c_str() );
}

void MTable::fieldFix( TConfig &cfg )
{
    bool fix = false;
    string all_flds;
    
    string req;
    vector< vector<string> > tbl;
    
    //Get config fields list
    vector<string> cf_el;
    cfg.cfgList(cf_el);
    
    //Get avoid fields list
    req ="PRAGMA table_info("+name()+");";
    m_bd->sqlReq( req.c_str(), &tbl );
    if( tbl.size() != 0 )    
    {    	
	//Check structure
	bool next = false;
	//Check avoid fields and find new fields
        for( int i_cf = 0; i_cf < cf_el.size(); i_cf++ )
	{
	    int i_fld;
	    for( i_fld = 1; i_fld < tbl.size(); i_fld++ )
	        if( cf_el[i_cf] == tbl[i_fld][1] )
		{
		    if( !next ) next = true; else all_flds+=",";
            	    all_flds = all_flds+"\""+cf_el[i_cf]+"\"";				
		    if( cfg.cfg(cf_el[i_cf]).fld().type()&T_STRING && tbl[i_fld][2] == "TEXT") break;
		    else if( cfg.cfg(cf_el[i_cf]).fld().type()&(T_DEC|T_OCT|T_HEX|T_BOOL) && tbl[i_fld][2] == "INTEGER") break;
		    else if( cfg.cfg(cf_el[i_cf]).fld().type()&T_REAL && tbl[i_fld][2] == "DOUBLE" ) break;		    
		    fix = true;
		    break;
		}
	    if( i_fld >= tbl.size() ) fix = true;
	}
	//Check delete fields
	for( int i_fld = 1; i_fld < tbl.size(); i_fld++ )
	{
	    int i_cf;
	    for( i_cf = 0; i_cf < cf_el.size(); i_cf++ )
		if( cf_el[i_cf] == tbl[i_fld][1] ) break;
	    if( i_cf >= cf_el.size() ) fix = true;
	}		
	
	if( !fix ) return;	//Structure OK!	
	//Fix structure
	//Move data to temporary DB
	req = "CREATE TEMPORARY TABLE \"temp_"+name()+"\"("+all_flds+");"
	    "INSERT INTO \"temp_"+name()+"\" SELECT "+all_flds+" FROM \""+name()+"\";"
	    "DROP TABLE \""+name()+"\";";
	m_bd->sqlReq( req.c_str() );
    } 
           
    //Create new table
    req ="CREATE TABLE \""+name()+"\" (";
    bool next = false;
    for( int i_cf = 0; i_cf < cf_el.size(); i_cf++ )
    {
        if( !next ) next = true; else req=req+",";
        TCfg &cf = cfg.cfg(cf_el[i_cf]);
        req = req+"\""+cf_el[i_cf]+"\" ";
        //Type param
        if( cf.fld().type()&T_STRING )
    	    req+="TEXT DEFAULT '"+cf.fld().def()+"' ";
        else if( cf.fld().type()&T_REAL )
    	    req+="DOUBLE DEFAULT '"+cf.fld().def()+"' ";
        else if( cf.fld().type()&(T_DEC|T_OCT|T_HEX|T_BOOL) )
    	    req+="INTEGER DEFAULT '"+cf.fld().def()+"' ";
	//Primary key
	if( cf.fld().type()&F_KEY )
	    req+="PRIMARY KEY ";
    }
    req += ");";
    //printf("TEST 03: query: <%s>\n",req.c_str());
    m_bd->sqlReq( req.c_str() );

    //printf("TEST 01: %d\n",fix);    
    //Copy data from temporary DB
    if( fix )
    {
	req = "INSERT INTO \""+name()+"\"("+all_flds+") SELECT "+all_flds+" FROM \"temp_"+name()+"\";"
	    "DROP TABLE \"temp_"+name()+"\";";
	//printf("TEST 02: %s\n",req.c_str());    
	m_bd->sqlReq( req.c_str() );    
    }    
}    

