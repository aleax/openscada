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
#include <tmess.h>
#include "bd_sqlite.h"


//============ Modul info! =====================================================
#define MOD_ID      "SQLite"
#define MOD_NAME    "DB SQLite"
#define MOD_TYPE    "BD"
#define VER_TYPE    VER_BD
#define VERSION     "0.9.0"
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
    return(new MBD(name,this,create));
}
	    
void BDMod::delBD( const string &name )
{
    if(remove(name.c_str()) != 0)
        throw TError(nodePath().c_str(),"Delete bd <%s> error: %s",name.c_str(),strerror(errno));
}

string BDMod::optDescr( )
{
    char buf[STR_BUF_LEN];

    snprintf(buf,sizeof(buf),I18N(
	"======================= The module <%s:%s> options =======================\n"
	"---------- Parameters of the module section <%s> in config file ----------\n"
	"\n"),
	MOD_TYPE,MOD_ID,nodePath().c_str());

    return(buf);
}			

void BDMod::modLoad( )
{
    //========== Load parameters from command line ============
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
    
    //========== Load parameters from config file =============

}

//=============================================================
//====================== BDSQLite::MBD ========================
//=============================================================
MBD::MBD( string name, TTipBD *owner, bool create ) : TBD(name,owner), m_db(NULL), openTrans(false)
{
    int rc;
    
    rc = sqlite3_open(name.c_str(), &m_db); 
    if( rc )
    { 
	string err = sqlite3_errmsg(m_db);
	sqlite3_close(m_db);
	throw TError(nodePath().c_str(), err.c_str());
    }
    sqlReq("BEGIN;");		   
};

MBD::~MBD( )
{    
    try
    {
	sqlReq("COMMIT;");
	sqlite3_close(m_db);
    }catch(TError err){ Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); }
};

TTable *MBD::openTable( const string &name, bool create )
{
    return( new MTable(name,this,create) );
}

void MBD::delTable( const string &name )
{
    string req ="DROP TABLE \""+name+"\";";
    sqlReq( req );
}

void MBD::sqlReq( const string &req, vector< vector<string> > *tbl )
{    
    char *zErrMsg;
    int rc,nrow=0,ncol=0;
    char **result;
    
    //printf("TEST 03: query: <%s>\n",req.c_str());
    rc = sqlite3_get_table( m_db,req.c_str(),&result, &nrow, &ncol, &zErrMsg );
    if( rc != SQLITE_OK ) throw TError(nodePath().c_str(),zErrMsg);
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
MTable::MTable(string name, MBD *bd, bool create ) : TTable(name,bd), my_trans(false)
{
    try { owner().sqlReq("SELECT * FROM \""+name+"\" LIMIT 0;"); }
    catch(...) { if( !create ) throw; }
}

MTable::~MTable(  )
{
}

bool MTable::fieldSeek( int row, TConfig &cfg )
{
    vector< vector<string> > tbl;
    
    //Get config fields list
    vector<string> cf_el;
    cfg.cfgList(cf_el);
    
    string req =string("SELECT * FROM \"")+name()+"\" LIMIT "+TSYS::int2str(row)+",1;";
    owner().sqlReq( req, &tbl );
    if( tbl.size() < 2 ) return false;
    //Processing of query
    for( int i_cf = 0; i_cf < cf_el.size(); i_cf++ )
	for( int i_fld = 0; i_fld < tbl[0].size(); i_fld++ )
	    if( cf_el[i_cf] == tbl[0][i_fld] )
	    {
		string val = tbl[1][i_fld];
		TCfg &u_cfg = cfg.cfg(cf_el[i_cf]);
		switch(u_cfg.fld().type())
		{
		    case TFld::String:	u_cfg.setS(val); break;
		    case TFld::Dec: case TFld::Oct: case TFld::Hex:	
					u_cfg.setI(atoi(val.c_str()));	break;
		    case TFld::Real:	u_cfg.setR(atof(val.c_str()));	break;
		    case TFld::Bool:	u_cfg.setB(atoi(val.c_str()));	break;
		}
	    }

    return true;
}

void MTable::fieldGet( TConfig &cfg )
{
    vector< vector<string> > tbl;
    
    //Get config fields list
    vector<string> cf_el;
    cfg.cfgList(cf_el);
    
    //Get avoid fields list
    string req ="PRAGMA table_info("+name()+");";
    owner().sqlReq( req, &tbl );	
    if( tbl.size() == 0 ) throw TError(nodePath().c_str(),"Table is empty.");
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
		
		if( cfg.cfg(cf_el[i_cf]).fld().flg()&FLD_KEY )
		{
		    if( !next_wr ) next_wr = true; else req_where=req_where+"AND ";
		    req_where=req_where+"\""+tbl[i_fld][1]+"\"='"+cfg.cfg(cf_el[i_cf]).getS()+"'";
		}		
	    }
    req = req+" FROM \""+name()+"\" WHERE "+req_where+";";
    //Query
    tbl.clear();
    owner().sqlReq( req, &tbl );
    if( tbl.size() < 2 ) throw TError(nodePath().c_str(),"Row no present.");
    //Processing of query
    for( int i_cf = 0; i_cf < cf_el.size(); i_cf++ )
	for( int i_fld = 0; i_fld < tbl[0].size(); i_fld++ )
	    if( cf_el[i_cf] == tbl[0][i_fld] )
	    {
		string val = tbl[1][i_fld];
		TCfg &u_cfg = cfg.cfg(cf_el[i_cf]);
		switch(u_cfg.fld().type())
		{
		    case TFld::String:	u_cfg.setS(val); break;
		    case TFld::Dec: case TFld::Oct: case TFld::Hex:	
					u_cfg.setI(atoi(val.c_str()));	break;
		    case TFld::Real:	u_cfg.setR(atof(val.c_str()));	break;
		    case TFld::Bool:	u_cfg.setB(atoi(val.c_str()));	break;
		}
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
    owner().sqlReq( req, &tbl_str );
    if( tbl_str.size() == 0 ) throw TError(nodePath().c_str(),"Table is empty.");
    
    //Get avoid fields list
    string req_where = "WHERE ";
    //Add key list to queue
    bool next = false;
    for( int i_cf = 0; i_cf < cf_el.size(); i_cf++ )
        for( int i_fld = 1; i_fld < tbl_str.size(); i_fld++ )
            if( cf_el[i_cf] == tbl_str[i_fld][1] && cfg.cfg(cf_el[i_cf]).fld().flg()&FLD_KEY )
            {
		if( !next ) next = true; else req_where=req_where+"AND ";
		req_where=req_where+"\""+cf_el[i_cf]+"\"='"+cfg.cfg(cf_el[i_cf]).getS()+"' "; //!!!! May be check of field type
	    }    
    //Query
    req = "SELECT * FROM \""+name()+"\" "+req_where+";";
    owner().sqlReq( req, &tbl );
    if( tbl.size() < 2 )
    {
	//Add line
	req = "INSERT INTO \""+name()+"\" ";
	string ins_name, ins_value;
	next = false;
	for( int i_cf = 0; i_cf < cf_el.size(); i_cf++ )
	    for( int i_fld = 1; i_fld < tbl_str.size(); i_fld++ )
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
		    switch(u_cfg.fld().type())
		    {
			case TFld::String:	val = u_cfg.getS();	break;
			case TFld::Dec:	case TFld::Oct: case TFld::Hex:	
						val = SYS->int2str(u_cfg.getI());	break;
			case TFld::Real:	val = SYS->real2str(u_cfg.getR());	break;
			case TFld::Bool:	val = SYS->int2str(u_cfg.getB());	break;
		    }
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
    	    for( int i_fld = 1; i_fld < tbl_str.size(); i_fld++ )
        	if( cf_el[i_cf] == tbl_str[i_fld][1] )
		{					     
		    if( !next ) next = true; else req=req+",";
		    string val;
		    TCfg &u_cfg = cfg.cfg(cf_el[i_cf]);
		    switch(u_cfg.fld().type())
		    {
			case TFld::String:	val = u_cfg.getS();	break;
			case TFld::Dec:	case TFld::Oct:	case TFld::Hex:
						val = SYS->int2str(u_cfg.getI());	break;
			case TFld::Real:	val = SYS->real2str(u_cfg.getR());	break;
			case TFld::Bool:	val = SYS->int2str(u_cfg.getB());	break;
		    }
		    req=req+"\""+cf_el[i_cf]+"\"='"+val+"' ";
		}
    	req = req + req_where;
    }
    req += ";";
    //Query
    //printf("TEST 02: query: <%s>\n",req.c_str());
    owner().sqlReq( req );
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
    owner().sqlReq( req, &tbl );
    if( tbl.size() == 0 ) throw TError(nodePath().c_str(),"Table is empty.");
    //Prepare request
    req = "DELETE FROM \""+name()+"\" WHERE ";
    //Add key list to queue
    bool next = false;
    for( int i_cf = 0; i_cf < cf_el.size(); i_cf++ )
	for( int i_fld = 1; i_fld < tbl.size(); i_fld++ )
	    if( cf_el[i_cf] == tbl[i_fld][1] && cfg.cfg(cf_el[i_cf]).fld().flg()&FLD_KEY )
	    {
		if( !next ) next = true; else req=req+"AND ";		
		req=req+"\""+tbl[i_fld][1]+"\"='"+cfg.cfg(cf_el[i_cf]).getS()+"' "; //!!!! May be check of field type
	    }
    req += ";";
    owner().sqlReq( req );
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
    owner().sqlReq( req, &tbl );
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
		    switch(cfg.cfg(cf_el[i_cf]).fld().type())
		    {
			case TFld::String:	if( tbl[i_fld][2] != "TEXT") fix = true;	break;
			case TFld::Dec:	case TFld::Hex:	case TFld::Oct:	case TFld::Bool:	
					    	if( tbl[i_fld][2] != "INTEGER")	fix = true;	break;
			case TFld::Real:  	if( tbl[i_fld][2] != "DOUBLE" ) fix = true;	break;
			default: fix = true;
		    }
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
	owner().sqlReq( req );
    } 
           
    //Create new table
    req ="CREATE TABLE \""+name()+"\" (";
    bool next = false;
    bool next_key = false;
    string pr_keys;	    
    for( int i_cf = 0; i_cf < cf_el.size(); i_cf++ )
    {
        if( !next ) next = true; else req=req+",";
        TCfg &cf = cfg.cfg(cf_el[i_cf]);
        req = req+"\""+cf_el[i_cf]+"\" ";
        //Type param
	switch(cf.fld().type())
	{
	    case TFld::String:	req+="TEXT DEFAULT '"+cf.fld().def()+"' ";	break;
	    case TFld::Dec: case TFld::Hex: case TFld::Oct: case TFld::Bool:    
				req+="INTEGER DEFAULT '"+cf.fld().def()+"' ";	break;
	    case TFld::Real:    req+="DOUBLE DEFAULT '"+cf.fld().def()+"' ";	break;
	}
	//Primary key
	if( cf.fld().flg()&FLD_KEY )
	{
	    if( !next_key ) next_key = true;
	    else pr_keys=pr_keys+",";
	    pr_keys=pr_keys+"\""+cf_el[i_cf]+"\"";
	}
    }
    req += ", PRIMARY KEY ("+pr_keys+"));";
    //printf("TEST 03: query: <%s>\n",req.c_str());
    owner().sqlReq( req );

    //printf("TEST 01: %d\n",fix);    
    //Copy data from temporary DB
    if( fix )
    {
	req = "INSERT INTO \""+name()+"\"("+all_flds+") SELECT "+all_flds+" FROM \"temp_"+name()+"\";"
	    "DROP TABLE \"temp_"+name()+"\";";
	//printf("TEST 02: %s\n",req.c_str());    
	owner().sqlReq( req );    
    }    
}    

