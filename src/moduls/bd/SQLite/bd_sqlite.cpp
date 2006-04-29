
//OpenSCADA system module BD.SQLite file: bd_sqlite.cpp
/***************************************************************************
 *   Copyright (C) 2003-2006 by Roman Savochenko                           *
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
#define VERSION     "1.0.0"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "BD modul. Allow support of the BD SQLite."
#define LICENSE     "GPL"
//==============================================================================

BDSQLite::BDMod *BDSQLite::mod;

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
	    self_addr = BDSQLite::mod = new BDSQLite::BDMod( source );

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
    mVers      	= VERSION;
    mAutor    	= AUTORS;
    mDescr  	= DESCRIPTION;
    mLicense   	= LICENSE;
    mSource    	= name;
}

BDMod::~BDMod()
{

}

TBD *BDMod::openBD( const string &iid )
{    
    return new MBD(iid,&owner().openDB_E());
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
MBD::MBD( const string &iid, TElem *cf_el ) : TBD(iid,cf_el), commCnt(0)
{

}

MBD::~MBD( )
{

}

void MBD::postDisable(int flag)
{
    TBD::postDisable(flag);
    
    if( flag && owner().fullDeleteDB() )
    {
	if(remove(dbFile().c_str()) != 0)
    	    throw TError(nodePath().c_str(),"Delete bd error: %s",strerror(errno));
    }
}

void MBD::enable( )
{
    if( enableStat() )  return;
    
    cd_pg = codepage();    
    int rc = sqlite3_open(dbFile().c_str(),&m_db); 
    if( rc )
    { 
	string err = sqlite3_errmsg(m_db);
	sqlite3_close(m_db);
	throw TError(nodePath().c_str(), err.c_str());
    }
    
    TBD::enable( );    
}

void MBD::disable( )
{
    if( !enableStat() )  return;
    
    TBD::disable( );
    
    //Last commit
    if(commCnt) { commCnt = COM_MAX_CNT; sqlReq(""); }
    //Close DB
    sqlite3_close(m_db);
}	

string MBD::dbFile()
{
    return TSYS::strSepParse(addr(),0,';');
}
    
string MBD::codepage()
{
    string code = TSYS::strSepParse(addr(),1,';');
    if(!code.size()) code = Mess->charset( );
    return code;
}

TTable *MBD::openTable( const string &inm, bool create )
{
    if( !enableStat() )
        throw TError(nodePath().c_str(),"Error open table <%s>. DB disabled.",inm.c_str());
	    
    return new MTable(inm,this,create);
}

void MBD::sqlReq( const string &ireq, vector< vector<string> > *tbl )
{    
    char *zErrMsg;
    int rc,nrow=0,ncol=0;
    char **result;    
    
    //printf("TEST 03: query: <%s>\n",req.c_str());
    //Commit set
    string req = ireq;
    if(!commCnt) req=string("BEGIN;")+ireq;
    if((++commCnt) > COM_MAX_CNT )
    {
	req=ireq+"COMMIT;";
	commCnt=0;
    }
    //Put request
    rc = sqlite3_get_table( m_db,Mess->codeConvOut(cd_pg.c_str(),req).c_str(),&result, &nrow, &ncol, &zErrMsg );
    if( rc != SQLITE_OK ) 
    {
	//Fix transaction
	if((commCnt-1) < 0) commCnt=COM_MAX_CNT;
	throw TError(nodePath().c_str(),zErrMsg);
    }
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
		row.push_back(result[(i+1)*ncol+ii]?Mess->codeConvIn(cd_pg.c_str(),result[(i+1)*ncol+ii]):"");
    	    tbl->push_back(row);
	}    
    }
    sqlite3_free_table(result);    
}

void MBD::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    if( cmd==TCntrNode::Info )
    {
        TBD::cntrCmd_(a_path,opt,cmd);
        ctrInsNode("area",1,opt,a_path.c_str(),"/serv",mod->I18N("DB service"));
        ctrMkNode("comm",opt,a_path.c_str(),"/serv/end_tr",mod->I18N("Close transaction"),0440,0,0);
    }
    else if( cmd==TCntrNode::Get )
        TBD::cntrCmd_(a_path,opt,cmd);
    else if( cmd==TCntrNode::Set )
    {
	if( a_path == "/serv/end_tr" )
        {
    	    if(commCnt) { commCnt = COM_MAX_CNT; sqlReq(""); }
        }
        else TBD::cntrCmd_(a_path,opt,cmd);
    }
}

//=============================================================
//=================== MBDMySQL::Table =========================
//=============================================================
MTable::MTable(string name, MBD *iown, bool create ) : TTable(name), my_trans(false)
{
    nodePrev(iown);    

    try { owner().sqlReq("SELECT * FROM \""+TSYS::strCode(name,TSYS::SQL)+"\" LIMIT 0;"); }
    catch(...) { if( !create ) throw; }
}

MTable::~MTable(  )
{
}

void MTable::postDisable(int flag)
{
    if( flag )
    {
	try{ owner().sqlReq("DROP TABLE \""+TSYS::strCode(name(),TSYS::SQL)+"\";"); }
	catch(TError err) { Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); }    
    }
}

bool MTable::fieldSeek( int row, TConfig &cfg )
{
    vector< vector<string> > tbl;
    
    //Get config fields list
    vector<string> cf_el;
    cfg.cfgList(cf_el);
    
    //Make WHERE
    string req_where = "WHERE ";
    //Add use keys to list
    bool next = false;
    for( int i_cf = 0; i_cf < cf_el.size(); i_cf++ )
        if( cfg.cfg(cf_el[i_cf]).fld().flg()&FLD_KEY && cfg.cfg(cf_el[i_cf]).getS().size() )
        {
	    if( !next ) next = true; 
	    else req_where=req_where+"AND ";
	    req_where=req_where+"\""+TSYS::strCode(cf_el[i_cf],TSYS::SQL)+"\"='"+TSYS::strCode(cfg.cfg(cf_el[i_cf]).getS(),TSYS::SQL)+"' ";
	}
    
    string req =string("SELECT * FROM \"")+TSYS::strCode(name(),TSYS::SQL)+"\" "+((next)?req_where:"")+" LIMIT "+TSYS::int2str(row)+",1;";
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
    
    //Get present fields list
    string req ="PRAGMA table_info("+TSYS::strCode(name(),TSYS::SQL)+");";
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
		    req_where=req_where+"\""+TSYS::strCode(tbl[i_fld][1],TSYS::SQL)+"\"='"+TSYS::strCode(cfg.cfg(cf_el[i_cf]).getS(),TSYS::SQL)+"'";
		}		
	    }
    req = req+" FROM \""+TSYS::strCode(name(),TSYS::SQL)+"\" WHERE "+req_where+";";
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
    
    //Get present fields list
    string req ="PRAGMA table_info("+TSYS::strCode(name(),TSYS::SQL)+");";
    owner().sqlReq( req, &tbl_str );
    if( tbl_str.size() == 0 ) throw TError(nodePath().c_str(),"Table is empty.");
    
    //Get present fields list
    string req_where = "WHERE ";
    //Add key list to queue
    bool next = false;
    for( int i_cf = 0; i_cf < cf_el.size(); i_cf++ )
        for( int i_fld = 1; i_fld < tbl_str.size(); i_fld++ )
            if( cf_el[i_cf] == tbl_str[i_fld][1] && cfg.cfg(cf_el[i_cf]).fld().flg()&FLD_KEY )
            {
		if( !next ) next = true; else req_where=req_where+"AND ";
		req_where=req_where+"\""+TSYS::strCode(cf_el[i_cf],TSYS::SQL)+"\"='"+TSYS::strCode(cfg.cfg(cf_el[i_cf]).getS(),TSYS::SQL)+"' ";
	    }    
    //Query
    req = "SELECT * FROM \""+TSYS::strCode(name(),TSYS::SQL)+"\" "+req_where+";";
    owner().sqlReq( req, &tbl );
    if( tbl.size() < 2 )
    {
	//Add line
	req = "INSERT INTO \""+TSYS::strCode(name(),TSYS::SQL)+"\" ";
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
		    ins_name=ins_name+"\""+TSYS::strCode(cf_el[i_cf],TSYS::SQL)+"\" ";		    
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
		    ins_value=ins_value+"'"+TSYS::strCode(val,TSYS::SQL)+"' ";
		}
      	req = req + "("+ins_name+") VALUES ("+ins_value+")";
    }
    else
    {
	//Update line    
	req = "UPDATE \""+TSYS::strCode(name(),TSYS::SQL)+"\" SET ";
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
		    req=req+"\""+TSYS::strCode(cf_el[i_cf],TSYS::SQL)+"\"='"+TSYS::strCode(val,TSYS::SQL)+"' ";
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

    //Get present fields list
    string req ="PRAGMA table_info("+TSYS::strCode(name(),TSYS::SQL)+");";
    owner().sqlReq( req, &tbl );
    if( tbl.size() == 0 ) throw TError(nodePath().c_str(),"Table is empty.");
    //Prepare request
    req = "DELETE FROM \""+TSYS::strCode(name(),TSYS::SQL)+"\" WHERE ";
    //Add key list to queue
    bool next = false;
    for( int i_cf = 0; i_cf < cf_el.size(); i_cf++ )
	for( int i_fld = 1; i_fld < tbl.size(); i_fld++ )
	    if( cf_el[i_cf] == tbl[i_fld][1] && cfg.cfg(cf_el[i_cf]).fld().flg()&FLD_KEY && cfg.cfg(cf_el[i_cf]).getS().size() )
	    {
		if( !next ) next = true; else req=req+"AND ";		
		req=req+"\""+TSYS::strCode(tbl[i_fld][1],TSYS::SQL)+"\"='"+TSYS::strCode(cfg.cfg(cf_el[i_cf]).getS(),TSYS::SQL)+"' ";
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
    
    //Get present fields list
    req ="PRAGMA table_info("+TSYS::strCode(name(),TSYS::SQL)+");";
    owner().sqlReq( req, &tbl );
    if( tbl.size() != 0 )    
    {    	
	//Check structure
	bool next = false;
	//Check present fields and find new fields
        for( int i_cf = 0; i_cf < cf_el.size(); i_cf++ )
	{
	    int i_fld;
	    for( i_fld = 1; i_fld < tbl.size(); i_fld++ )
	        if( cf_el[i_cf] == tbl[i_fld][1] )
		{
		    if( !next ) next = true; else all_flds+=",";
            	    all_flds = all_flds+"\""+TSYS::strCode(cf_el[i_cf],TSYS::SQL)+"\"";				
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
	req = "CREATE TEMPORARY TABLE \"temp_"+TSYS::strCode(name(),TSYS::SQL)+"\"("+all_flds+");"
	    "INSERT INTO \"temp_"+TSYS::strCode(name(),TSYS::SQL)+"\" SELECT "+all_flds+" FROM \""+TSYS::strCode(name(),TSYS::SQL)+"\";"
	    "DROP TABLE \""+TSYS::strCode(name(),TSYS::SQL)+"\";";
	owner().sqlReq( req );
    } 
           
    //Create new table
    req ="CREATE TABLE \""+TSYS::strCode(name(),TSYS::SQL)+"\" (";
    bool next = false;
    bool next_key = false;
    string pr_keys;	    
    for( int i_cf = 0; i_cf < cf_el.size(); i_cf++ )
    {
        if( !next ) next = true; else req=req+",";
        TCfg &cf = cfg.cfg(cf_el[i_cf]);
        req = req+"\""+TSYS::strCode(cf_el[i_cf],TSYS::SQL)+"\" ";
        //Type param
	switch(cf.fld().type())
	{
	    case TFld::String:	req+="TEXT DEFAULT '"+TSYS::strCode(cf.fld().def(),TSYS::SQL)+"' ";	break;
	    case TFld::Dec: case TFld::Hex: case TFld::Oct: case TFld::Bool:    
				req+="INTEGER DEFAULT '"+TSYS::strCode(cf.fld().def(),TSYS::SQL)+"' ";	break;
	    case TFld::Real:    req+="DOUBLE DEFAULT '"+TSYS::strCode(cf.fld().def(),TSYS::SQL)+"' ";	break;
	}
	//Primary key
	if( cf.fld().flg()&FLD_KEY )
	{
	    if( !next_key ) next_key = true;
	    else pr_keys=pr_keys+",";
	    pr_keys=pr_keys+"\""+TSYS::strCode(cf_el[i_cf],TSYS::SQL)+"\"";
	}
    }
    req += ", PRIMARY KEY ("+pr_keys+"));";
    //printf("TEST 03: query: <%s>\n",req.c_str());
    owner().sqlReq( req );

    //printf("TEST 01: %d\n",fix);    
    //Copy data from temporary DB
    if( fix )
    {
	req = "INSERT INTO \""+TSYS::strCode(name(),TSYS::SQL)+"\"("+all_flds+") SELECT "+all_flds+" FROM \"temp_"+TSYS::strCode(name(),TSYS::SQL)+"\";"
	    "DROP TABLE \"temp_"+TSYS::strCode(name(),TSYS::SQL)+"\";";
	//printf("TEST 02: %s\n",req.c_str());    
	owner().sqlReq( req );    
    }    
}    

