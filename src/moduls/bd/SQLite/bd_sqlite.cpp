
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

#include <resalloc.h>
#include <tsys.h>
#include <tmess.h>
#include "bd_sqlite.h"


//============ Modul info! =====================================================
#define MOD_ID      "SQLite"
#define MOD_NAME    "DB SQLite"
#define MOD_TYPE    "BD"
#define VER_TYPE    VER_BD
#define VERSION     "1.2.0"
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

    snprintf(buf,sizeof(buf),_(
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

string BDMod::sqlReqCode( const string &req, char symb )
{
    string sout = req;
    for( int i_sz = 0; i_sz < sout.size(); i_sz++ )
	if( sout[i_sz] == symb ) sout.replace(i_sz++,1,2,symb);
    return sout; 	
}

//=============================================================
//====================== BDSQLite::MBD ========================
//=============================================================
MBD::MBD( const string &iid, TElem *cf_el ) : TBD(iid,cf_el), commCnt(0)
{
    conn_res = ResAlloc::resCreate( );
}

MBD::~MBD( )
{
    ResAlloc::resDelete(conn_res);
}

void MBD::postDisable(int flag)
{
    TBD::postDisable(flag);
    
    if( flag && owner().fullDeleteDB() )
    {
	if(remove(addr().c_str()) != 0)
    	    throw TError(nodePath().c_str(),_("Delete bd error: %s"),strerror(errno));
    }
}

void MBD::enable( )
{
    if( enableStat() )  return;
    
    cd_pg = codePage().size()?codePage():Mess->charset();    
    int rc = sqlite3_open(addr().c_str(),&m_db); 
    if( rc )
    { 
	string err = sqlite3_errmsg(m_db);
	sqlite3_close(m_db);
	throw TError(nodePath().c_str(),_("Open DB file error: %s"),err.c_str());
    }
    
    TBD::enable( );    
}

void MBD::disable( )
{
    if( !enableStat() )  return;

    //Last commit
    if(commCnt) { commCnt = COM_MAX_CNT; sqlReq(""); }
    
    TBD::disable( );

    //Close DB
    ResAlloc res(conn_res,true);
    sqlite3_close(m_db);
}	

TTable *MBD::openTable( const string &inm, bool create )
{
    if( !enableStat() )
        throw TError(nodePath().c_str(),_("Error open table <%s>. DB disabled."),inm.c_str());
	    
    return new MTable(inm,this,create);
}

void MBD::sqlReq( const string &ireq, vector< vector<string> > *tbl )
{    
    char *zErrMsg;
    int rc,nrow=0,ncol=0;
    char **result;    

    if(!enableStat())	return;

    //printf("TEST 03: query: <%s>\n",req.c_str());
    //Commit set
    string req = ireq;
    
    ResAlloc res(conn_res,true);
    
    if(!commCnt) req=string("BEGIN;")+ireq;
    if((++commCnt) > COM_MAX_CNT )
    {
	req=ireq+"COMMIT;";
	commCnt=0;
    }
    
    //Put request
    rc = sqlite3_get_table(m_db,Mess->codeConvOut(cd_pg.c_str(),req).c_str(),&result, &nrow, &ncol, &zErrMsg );
    if( rc != SQLITE_OK ) 
    {
	//Fix transaction
	if((commCnt-1) < 0) commCnt=COM_MAX_CNT;
	throw TError(nodePath().c_str(),_("Get table error: %s"),zErrMsg);
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

void MBD::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
        TBD::cntrCmdProc(opt);
        if(ctrMkNode("area",opt,1,"/serv",_("DB service")))
    	    ctrMkNode("comm",opt,-1,"/serv/end_tr",_("Close transaction"),0660);
	return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/serv/end_tr" && ctrChkNode(opt,"set",0660,"root","root",SEQ_WR) )
    {
	if( commCnt ) { commCnt = COM_MAX_CNT; sqlReq(""); }
    }
    else TBD::cntrCmdProc(opt);			
}

//=============================================================
//=================== MBDMySQL::Table =========================
//=============================================================
MTable::MTable(string name, MBD *iown, bool create ) : TTable(name)
{
    nodePrev(iown);    

    try { owner().sqlReq("SELECT * FROM '"+mod->sqlReqCode(name)+"' LIMIT 0;"); }
    catch(...) { if( !create ) throw; }
}

MTable::~MTable(  )
{
}

void MTable::postDisable(int flag)
{
    if( flag )
    {
	try{ owner().sqlReq("DROP TABLE '"+mod->sqlReqCode(name())+"';"); }
	catch(TError err) { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }    
    }
}

bool MTable::fieldSeek( int row, TConfig &cfg )
{
    vector< vector<string> > tbl;
    
    //- Get config fields list -
    vector<string> cf_el;
    cfg.cfgList(cf_el);

    //- Get present fields list -
    string req ="PRAGMA table_info('"+mod->sqlReqCode(name())+"');";
    owner().sqlReq( req, &tbl );	
    if( tbl.size() == 0 ) throw TError(nodePath().c_str(),_("Table is empty."));
    
    //- Make WHERE -
    req = "SELECT ";
    string req_where = "WHERE ";
    //- Add use keys to list -
    bool first_sel = true;
    bool next = false;
    for( int i_cf = 0; i_cf < cf_el.size(); i_cf++ )
	for( int i_fld = 1; i_fld < tbl.size(); i_fld++ )
	    if( cf_el[i_cf] == tbl[i_fld][1] )
	    {
    		if( cfg.cfg(cf_el[i_cf]).fld().flg()&TCfg::Key && cfg.cfg(cf_el[i_cf]).getS().size() )
    		{
		    if( !next ) next = true; 
		    else req_where=req_where+"AND ";
		    req_where=req_where+"\""+mod->sqlReqCode(cf_el[i_cf],'"')+"\"='"+
					mod->sqlReqCode(cfg.cfg(cf_el[i_cf]).getS())+"' ";
		}
		else if( cfg.cfg(cf_el[i_cf]).fld().flg()&TCfg::Key || cfg.cfg(cf_el[i_cf]).view() )
                {
	            if( first_sel ) req=req+"\""+mod->sqlReqCode(cf_el[i_cf],'"')+"\"";
		    else req=req+",\""+mod->sqlReqCode(cf_el[i_cf],'"')+"\"";
		    first_sel = false;
	        }
                break;
	    }
    
    req = req+" FROM '"+mod->sqlReqCode(name())+"' "+((next)?req_where:"")+" LIMIT "+TSYS::int2str(row)+",1;";
    tbl.clear();
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
		    case TFld::Integer:	u_cfg.setI(atoi(val.c_str()));	break;
		    case TFld::Real:	u_cfg.setR(atof(val.c_str()));	break;
		    case TFld::Boolean:	u_cfg.setB(atoi(val.c_str()));	break;
		}
		break;
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
    string req ="PRAGMA table_info('"+mod->sqlReqCode(name())+"');";
    owner().sqlReq( req, &tbl );	
    if( tbl.size() == 0 ) throw TError(nodePath().c_str(),_("Table is empty."));
    //Prepare request
    req = "SELECT ";
    string req_where;
    //Add fields list to queue    
    bool first_sel = true;    
    bool next_wr = false;
    for( int i_cf = 0; i_cf < cf_el.size(); i_cf++ )
	for( int i_fld = 1; i_fld < tbl.size(); i_fld++ )
	    if( cf_el[i_cf] == tbl[i_fld][1] )
	    {
		if( cfg.cfg(cf_el[i_cf]).fld().flg()&TCfg::Key )
		{
		    if( !next_wr ) next_wr = true; else req_where=req_where+"AND ";
		    req_where=req_where+"\""+mod->sqlReqCode(tbl[i_fld][1],'"')+"\"='"+mod->sqlReqCode(cfg.cfg(cf_el[i_cf]).getS())+"'";
		}
		else if( cfg.cfg(cf_el[i_cf]).view() )
		{
            	    if( first_sel ) req=req+"\""+mod->sqlReqCode(cf_el[i_cf],'"')+"\"";
		    else req=req+",\""+mod->sqlReqCode(cf_el[i_cf],'"')+"\"";
		    first_sel = false;
		}
		break;
	    }
    req = req+" FROM '"+mod->sqlReqCode(name())+"' WHERE "+req_where+";";
    //Query
    tbl.clear();
    owner().sqlReq( req, &tbl );
    if( tbl.size() < 2 ) throw TError(nodePath().c_str(),_("Row no present."));
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
		    case TFld::Integer:	u_cfg.setI(atoi(val.c_str()));	break;
		    case TFld::Real:	u_cfg.setR(atof(val.c_str()));	break;
		    case TFld::Boolean:	u_cfg.setB(atoi(val.c_str()));	break;
		}
		break;
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
    string req ="PRAGMA table_info('"+mod->sqlReqCode(name())+"');";
    owner().sqlReq( req, &tbl_str );
    if( tbl_str.size() == 0 ) throw TError(nodePath().c_str(),_("Table is empty."));
    
    //Get present fields list
    string req_where = "WHERE ";
    //Add key list to queue
    bool next = false;
    for( int i_cf = 0; i_cf < cf_el.size(); i_cf++ )
        for( int i_fld = 1; i_fld < tbl_str.size(); i_fld++ )
            if( cf_el[i_cf] == tbl_str[i_fld][1] && cfg.cfg(cf_el[i_cf]).fld().flg()&TCfg::Key )
            {
		if( !next ) next = true; else req_where=req_where+"AND ";
		req_where=req_where+"\""+mod->sqlReqCode(cf_el[i_cf],'"')+"\"='"+mod->sqlReqCode(cfg.cfg(cf_el[i_cf]).getS())+"' ";
	    }    
    //Query
    req = "SELECT 1 FROM '"+mod->sqlReqCode(name())+"' "+req_where+";";
    owner().sqlReq( req, &tbl );
    if( tbl.size() < 2 )
    {
	//Add line
	req = "INSERT INTO '"+mod->sqlReqCode(name())+"' ";
	string ins_name, ins_value;
	next = false;
	for( int i_cf = 0; i_cf < cf_el.size(); i_cf++ )
	    for( int i_fld = 1; i_fld < tbl_str.size(); i_fld++ )
                if( cf_el[i_cf] == tbl_str[i_fld][1] )
		{
		    if(	!(cfg.cfg(cf_el[i_cf]).fld().flg()&TCfg::Key) && !cfg.cfg(cf_el[i_cf]).view() )
			break;
		    
		    if( !next ) next = true; 
		    else 
		    {
			ins_name=ins_name+",";
			ins_value=ins_value+",";
		    }
		    ins_name=ins_name+"\""+mod->sqlReqCode(cf_el[i_cf],'"')+"\" ";		    
		    string val;
		    TCfg &u_cfg = cfg.cfg(cf_el[i_cf]);
		    switch(u_cfg.fld().type())
		    {
			case TFld::String:	val = u_cfg.getS();	break;
			case TFld::Integer:	val = SYS->int2str(u_cfg.getI());	break;
			case TFld::Real:	val = SYS->real2str(u_cfg.getR());	break;
			case TFld::Boolean:	val = SYS->int2str(u_cfg.getB());	break;
		    }
		    ins_value=ins_value+"'"+mod->sqlReqCode(val)+"' ";
		    break;
		}
      	req = req + "("+ins_name+") VALUES ("+ins_value+")";
    }
    else
    {
	//Update line    
	req = "UPDATE '"+mod->sqlReqCode(name())+"' SET ";
	next = false;
	for( int i_cf = 0; i_cf < cf_el.size(); i_cf++ )
    	    for( int i_fld = 1; i_fld < tbl_str.size(); i_fld++ )
        	if( cf_el[i_cf] == tbl_str[i_fld][1] )
		{				
		    if( cfg.cfg(cf_el[i_cf]).fld().flg()&TCfg::Key || !cfg.cfg(cf_el[i_cf]).view() )
                        break;
			     
		    if( !next ) next = true; else req=req+",";
		    string val;
		    TCfg &u_cfg = cfg.cfg(cf_el[i_cf]);
		    switch(u_cfg.fld().type())
		    {
			case TFld::String:	val = u_cfg.getS();	break;
			case TFld::Integer:	val = SYS->int2str(u_cfg.getI());	break;
			case TFld::Real:	val = SYS->real2str(u_cfg.getR());	break;
			case TFld::Boolean:	val = SYS->int2str(u_cfg.getB());	break;
		    }
		    req=req+"\""+mod->sqlReqCode(cf_el[i_cf],'"')+"\"='"+mod->sqlReqCode(val)+"' ";
		    
		    break;
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
    string req ="PRAGMA table_info('"+mod->sqlReqCode(name())+"');";
    owner().sqlReq( req, &tbl );
    if( tbl.size() == 0 ) throw TError(nodePath().c_str(),_("Table is empty."));
    //Prepare request
    req = "DELETE FROM '"+mod->sqlReqCode(name())+"' WHERE ";
    //Add key list to queue
    bool next = false;
    for( int i_cf = 0; i_cf < cf_el.size(); i_cf++ )
	for( int i_fld = 1; i_fld < tbl.size(); i_fld++ )
	    if( cf_el[i_cf] == tbl[i_fld][1] )
	    {
		if( cfg.cfg(cf_el[i_cf]).fld().flg()&TCfg::Key && cfg.cfg(cf_el[i_cf]).getS().size() )
		{
		    if( !next ) next = true; else req=req+"AND ";		
		    req=req+"\""+mod->sqlReqCode(tbl[i_fld][1],'"')+"\"='"+mod->sqlReqCode(cfg.cfg(cf_el[i_cf]).getS())+"' ";
		}
		break;
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
    req ="PRAGMA table_info('"+mod->sqlReqCode(name())+"');";
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
            	    all_flds = all_flds+"\""+mod->sqlReqCode(cf_el[i_cf],'"')+"\"";				
		    switch(cfg.cfg(cf_el[i_cf]).fld().type())
		    {
			case TFld::String:	if( tbl[i_fld][2] != "TEXT") fix = true;	break;
			case TFld::Integer: case TFld::Boolean:	
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
	req = "CREATE TEMPORARY TABLE 'temp_"+mod->sqlReqCode(name())+"'("+all_flds+");"
	    "INSERT INTO 'temp_"+mod->sqlReqCode(name())+"' SELECT "+all_flds+" FROM '"+mod->sqlReqCode(name())+"';"
	    "DROP TABLE '"+mod->sqlReqCode(name())+"';";
	owner().sqlReq( req );
    } 
           
    //Create new table
    req ="CREATE TABLE '"+mod->sqlReqCode(name())+"' (";
    bool next = false;
    bool next_key = false;
    string pr_keys;	    
    for( int i_cf = 0; i_cf < cf_el.size(); i_cf++ )
    {
        if( !next ) next = true; else req=req+",";
        TCfg &cf = cfg.cfg(cf_el[i_cf]);
        req = req+"\""+mod->sqlReqCode(cf_el[i_cf],'"')+"\" ";
        //Type param
	switch(cf.fld().type())
	{
	    case TFld::String:	req+="TEXT DEFAULT '"+mod->sqlReqCode(cf.fld().def())+"' ";	break;
	    case TFld::Integer: case TFld::Boolean:    
				req+="INTEGER DEFAULT '"+mod->sqlReqCode(cf.fld().def())+"' ";	break;
	    case TFld::Real:    req+="DOUBLE DEFAULT '"+mod->sqlReqCode(cf.fld().def())+"' ";	break;
	}
	//Primary key
	if( cf.fld().flg()&TCfg::Key )
	{
	    if( !next_key ) next_key = true;
	    else pr_keys=pr_keys+",";
	    pr_keys=pr_keys+"\""+mod->sqlReqCode(cf_el[i_cf],'"')+"\"";
	}
    }
    req += ", PRIMARY KEY ("+pr_keys+"));";
    //printf("TEST 03: query: <%s>\n",req.c_str());
    owner().sqlReq( req );

    //printf("TEST 01: %d\n",fix);    
    //Copy data from temporary DB
    if( fix )
    {
	req = "INSERT INTO '"+mod->sqlReqCode(name())+"'("+all_flds+") SELECT "+all_flds+" FROM 'temp_"+mod->sqlReqCode(name())+"';"
	    "DROP TABLE 'temp_"+mod->sqlReqCode(name())+"';";
	//printf("TEST 02: %s\n",req.c_str());    
	owner().sqlReq( req );    
    }    
}    

