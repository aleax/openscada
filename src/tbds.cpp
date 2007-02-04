
//OpenSCADA system file: tbds.cpp
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

#include <unistd.h>
#include <getopt.h>

#include "resalloc.h"
#include "tsys.h"
#include "tmess.h"
#include "tmodule.h"
#include "tbds.h"

//================================================================
//=========== TBDS ===============================================
//================================================================
TBDS::TBDS( ) : TSubSYS("BD","Data Bases",true)
{
    genDBCacheRes = ResAlloc::resCreate( );

    //Generic system DB
    fldAdd( new TFld("user","User",TFld::String,TCfg::Key,"20") );
    fldAdd( new TFld("id","Value ID",TFld::String,TCfg::Key,"100") );
    fldAdd( new TFld("val","Value"  ,TFld::String,TFld::NoFlag,"1000") );
    
    //Open data bases DB structure
    el_db.fldAdd( new TFld("ID",_("ID"),TFld::String,TCfg::Key,"20") );
    el_db.fldAdd( new TFld("TYPE",_("DB type (module)"),TFld::String,TCfg::Key,"20") );
    el_db.fldAdd( new TFld("NAME",_("Name"),TFld::String,TFld::NoFlag,"50") );
    el_db.fldAdd( new TFld("DESCR",_("Description"),TFld::String,TFld::NoFlag,"200") );
    el_db.fldAdd( new TFld("ADDR",_("Address"),TFld::String,TFld::NoFlag,"100") );
    el_db.fldAdd( new TFld("CODEPAGE",_("Code page"),TFld::String,TFld::NoFlag,"20") );    
    el_db.fldAdd( new TFld("EN",_("To enable"),TFld::Boolean,TFld::NoFlag,"1","1") );
}

TBDS::~TBDS(  )
{
    ResAlloc res(genDBCacheRes,true);
    while(genDBCache.size())
    {
	delete genDBCache[0];
	genDBCache.pop_front();
    }
    res.release();

    ResAlloc::resDelete(genDBCacheRes);
}

AutoHD<TTable> TBDS::open( const string &bdn, bool create )
{
    bool bd_op = false;
    AutoHD<TTable> tbl;    
    
    try
    {
	string bd_t = TSYS::strSepParse(bdn,0,'.');
	string bd_n = TSYS::strSepParse(bdn,1,'.');
	string bd_tbl = TSYS::strSepParse(bdn,2,'.');
	if(bd_t == "*") bd_t = TSYS::strSepParse(SYS->workDB(),0,'.');
	if(bd_n == "*") bd_n = TSYS::strSepParse(SYS->workDB(),1,'.');
	if( at(bd_t).at().at(bd_n).at().enableStat() )
	{    
	    if( !at(bd_t).at().at(bd_n).at().openStat(bd_tbl) )
		at(bd_t).at().at(bd_n).at().open(bd_tbl,create);
	    tbl = at(bd_t).at().at(bd_n).at().at(bd_tbl);
	}
    }
    catch(TError err)
    { 
	/*mess_warning(err.cat.c_str(),"%s",err.mess.c_str());*/ 
    }

    return tbl;
}

void TBDS::close( const string &bdn, bool del )
{
    try
    {
	string bd_t = TSYS::strSepParse(bdn,0,'.');
        string bd_n = TSYS::strSepParse(bdn,1,'.');
        string bd_tbl = TSYS::strSepParse(bdn,2,'.');
	if(bd_t == "*") bd_t = TSYS::strSepParse(SYS->workDB(),0,'.');
        if(bd_n == "*") bd_n = TSYS::strSepParse(SYS->workDB(),1,'.');
	if( at(bd_t).at().at(bd_n).at().enableStat()&& 
		at(bd_t).at().at(bd_n).at().openStat(bd_tbl) && 
		at(bd_t).at().at(bd_n).at().at(bd_tbl).at().nodeUse() == 1 )
	    at(bd_t).at().at(bd_n).at().close(bd_tbl,del);
    }catch(TError err) 
    { 
	mess_warning(err.cat.c_str(),"%s",err.mess.c_str());
	mess_warning(nodePath().c_str(),_("Close DB <%s> error!"),bdn.c_str()); 
    }
}

string TBDS::fullDBSYS()
{
    return SYS->workDB()+".SYS";
}

string TBDS::fullDB()
{
    return SYS->workDB()+".DB";
}

bool TBDS::dataSeek( const string &bdn, const string &path, int lev, TConfig &cfg )
{
    int c_lev = 0;
    
    if( path.size() )
    {
	try
	{
	    XMLNode *nd = ctrId(&SYS->cfgRoot(),path);
	    for( int i_fld = 0; i_fld < nd->childSize(); i_fld++ )
	    {
		XMLNode *el = nd->childGet(i_fld);
		if( el->name() == "fld" )
		{
		    //Check keywords
		    vector<string> cf_el;
		    cfg.cfgList(cf_el);
		
		    //Check keywords		    
        	    int i_el;
        	    for( i_el = 0; i_el < cf_el.size(); i_el++ )
            		if( cfg.cfg(cf_el[i_el]).fld().flg()&TCfg::Key &&
	        	    cfg.cfg(cf_el[i_el]).getS().size() && 
			    cfg.cfg(cf_el[i_el]).getS() != el->attr(cf_el[i_el]) ) break;
		    if( i_el == cf_el.size() && lev <= c_lev++ )
		    {	
			for( int i_el = 0; i_el < cf_el.size(); i_el++ )
			    cfg.cfg(cf_el[i_el]).setS(el->attr(cf_el[i_el]));
			return true;
		    }
		}
	    }
	}
	catch(...){  }
    }
	
    if(bdn.size())
    {
        AutoHD<TTable> tbl = open(bdn);
        if( !tbl.freeStat() )
        {
	    bool rez = tbl.at().fieldSeek(lev-c_lev,cfg);
    	    tbl.free();
    	    close(bdn);
    	    return rez;
	}
    }    
    
    return false;
}

bool TBDS::dataGet( const string &bdn, const string &path, TConfig &cfg )
{
    AutoHD<TTable> tbl = open(bdn);

    //- Load from DB -
    if( !tbl.freeStat() )
    {
	bool db_true = true;
        try{ tbl.at().fieldGet(cfg); } catch(...){ db_true = false; }
	tbl.free();
        close(bdn);		
	if(db_true) return true;
    }
    //- Load from Config file if tbl no present -
    XMLNode *nd;
    try{ nd = ctrId(&SYS->cfgRoot(),path); }
    catch(...){ return false; }
    
    //-- Scan fields and fill Config --
    for( int i_fld = 0; i_fld < nd->childSize(); i_fld++ )
    {
        XMLNode *el = nd->childGet(i_fld);
        if( el->name() == "fld" )
    	{
	    //Check keywords
	    vector<string> cf_el;
    	    cfg.cfgList(cf_el);
		
	    int i_el;	
    	    for( i_el = 0; i_el < cf_el.size(); i_el++ )
    		if( cfg.cfg(cf_el[i_el]).fld().flg()&TCfg::Key && 
		    cfg.cfg(cf_el[i_el]).getS() != el->attr(cf_el[i_el]) ) break;
	    if( i_el == cf_el.size() )
	    {
	        for( int i_el = 0; i_el < cf_el.size(); i_el++ )
		    cfg.cfg(cf_el[i_el]).setS(el->attr(cf_el[i_el]));
		return true;    
	    }	    	    
	}
    }
    //throw TError(nodePath().c_str(),"Field <%s> no present.",path.c_str());
    return false;
}

void TBDS::dataSet( const string &bdn, const string &path, TConfig &cfg )
{
    AutoHD<TTable> tbl = open(bdn,true);
    
    if( !tbl.freeStat() )
    {
        tbl.at().fieldSet(cfg);
	tbl.free();
        close(bdn);		
	return;
    }
    //Load from Config file if tbl no present
    /*XMLNode *nd = ctrId(&SYS->cfgRoot(),path);

    //Scan fields and fill Config
    for( int i_fld = 0; i_fld < nd->childSize(); i_fld++ )
    {
        XMLNode *el = nd->childGet(i_fld);
        if( el->name() == "fld" )
        {
	    //Check keywords
	    vector<string> cf_el;
            cfg.cfgList(cf_el);
		
	    int i_el;	
            for( i_el = 0; i_el < cf_el.size(); i_el++ )
        	if( cfg.cfg(cf_el[i_el]).fld().flg()&TCfg::Key && 
		    cfg.cfg(cf_el[i_el]).getS() != el->attr(cf_el[i_el]) ) break;
	    if( i_el == cf_el.size() )
	    {
		for( int i_el = 0; i_el < cf_el.size(); i_el++ )
		    el->attr(cf_el[i_el],cfg.cfg(cf_el[i_el]).getS());
		return;    
	    }	    	    
	}
    }
    throw TError("BD","Field no present.");*/
}	

void TBDS::dataDel( const string &bdn, const string &path, TConfig &cfg )
{
    AutoHD<TTable> tbl = open(bdn,true);
    
    if( !tbl.freeStat() )
    {
	tbl.at().fieldDel(cfg);
	tbl.free();
        close(bdn);
	return;
    }		
}

void TBDS::genDBSet(const string &path, const string &val, const string &user)
{
    string rez;
    
    //Set to DB
    if(SYS->present("BD"))
    {
	AutoHD<TBDS> dbs = SYS->db();
	AutoHD<TTable> tbl = dbs.at().open(dbs.at().fullDBSYS(),true);
	if( !tbl.freeStat() )
	{
    	    TConfig db_el(&dbs.at());
	    db_el.cfg("user").setS(user);
    	    db_el.cfg("id").setS(path);
	    db_el.cfg("val").setS(val);
	
    	    try{ tbl.at().fieldSet(db_el); }
    	    catch(TError err){ }
	
	    tbl.free();
    	    dbs.at().close(dbs.at().fullDBSYS());
	}
	
	//Put to cache
	ResAlloc res(dbs.at().genDBCacheRes,true);
	for( int i_cel = 0; i_cel < dbs.at().genDBCache.size(); i_cel++ )
    	    if( dbs.at().genDBCache[i_cel]->cfg("user").getS() == user &&
        	dbs.at().genDBCache[i_cel]->cfg("id").getS() == path )
	    {
		dbs.at().genDBCache[i_cel]->cfg("val").setS(val);
		return;
	    }
	TConfig *db_el = new TConfig(&dbs.at());
	db_el->cfg("user").setS(user);
	db_el->cfg("id").setS(path);
	db_el->cfg("val").setS(val);
	dbs.at().genDBCache.push_front(db_el);
	if(dbs.at().genDBCache.size() > 100)
	{	
	    delete dbs.at().genDBCache[0];
	    dbs.at().genDBCache.pop_back();
	}
    }
}

string TBDS::genDBGet(const string &path, const string &oval, const string &user, bool onlyCfg )
{
    bool bd_ok=false;
    string rez = oval;
    
    if(SYS->present("BD"))
    {	
	AutoHD<TBDS> dbs = SYS->db();
	//Get from cache
	ResAlloc res(dbs.at().genDBCacheRes,false);
	for( int i_cel = 0; i_cel < dbs.at().genDBCache.size(); i_cel++ )
	    if( dbs.at().genDBCache[i_cel]->cfg("user").getS() == user && 
		    dbs.at().genDBCache[i_cel]->cfg("id").getS() == path )
		return dbs.at().genDBCache[i_cel]->cfg("val").getS();
	res.release();
    
	//Get from generic DB
	if(!onlyCfg)
	{
	    AutoHD<TTable> tbl = dbs.at().open(SYS->db().at().fullDBSYS());
	    if( !tbl.freeStat() )
	    {
		TConfig db_el(&dbs.at());
		db_el.cfg("user").setS(user);
		db_el.cfg("id").setS(path);
		try
		{ 
		    tbl.at().fieldGet(db_el);
		    rez = db_el.cfg("val").getS();
		    bd_ok = true;
		}
		catch(TError err){  }
	
		tbl.free();
		dbs.at().close(dbs.at().fullDBSYS());
	    }      
	}
    }
    //Get from config file
    if(!bd_ok)	
	try{ rez = SYS->ctrId(&SYS->cfgRoot(),path)->text(); }
	catch(...){ }

    //Put to cache    
    if(SYS->present("BD"))
    {    
	AutoHD<TBDS> dbs = SYS->db();
	ResAlloc res(dbs.at().genDBCacheRes,true);
	TConfig *db_el = new TConfig(&dbs.at());
	db_el->cfg("user").setS(user);
	db_el->cfg("id").setS(path);
	db_el->cfg("val").setS(rez);
	dbs.at().genDBCache.push_front(db_el);
	if(dbs.at().genDBCache.size() > 100)
	{	
	    delete dbs.at().genDBCache[0];
	    dbs.at().genDBCache.pop_back();
	}
    }
    
    return rez;
}

string TBDS::optDescr(  )
{
    char buf[STR_BUF_LEN];
    snprintf(buf,sizeof(buf),_(
    	"========================= Subsystem \"DB\" options ========================\n"
	"------------ Parameters of section <%s> in config file -----------\n\n"
	),nodePath().c_str());

    return(buf);
}

void TBDS::subLoad( )
{
    //- Load parameters from command line -
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

    //- Load parameters from config file -

    //- Open DB load -
    //Load and enable other DB store DB
    string db_tp = TSYS::strSepParse(fullDB(),0,'.');
    string db_nm = TSYS::strSepParse(fullDB(),1,'.');
    if( !at(db_tp).at().openStat(db_nm) )
    {
	at(db_tp).at().open(db_nm);
	at(db_tp).at().at(db_nm).at().load();
	at(db_tp).at().at(db_nm).at().enable();
    }
    
    string id,type;
    //- Search and open new DB -
    try
    {	
        TConfig c_el(&el_db);    
        int fld_cnt = 0;
        while( SYS->db().at().dataSeek(fullDB(),nodePath()+"DB/",fld_cnt++,c_el) )
	{
            id = c_el.cfg("ID").getS();
    	    type = c_el.cfg("TYPE").getS();	    
	    if( (type+"."+id) != SYS->workDB() && !at(type).at().openStat(id) )
    		at(type).at().open(id);
	    c_el.cfg("ID").setS("");
            c_el.cfg("TYPE").setS("");
        }
    }catch( TError err )
    { 
	mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	mess_err(nodePath().c_str(),_("Search and open new DB error."));
    }    
    
    //- Load and enable DB -
    vector<string> t_lst, o_lst;
    modList(t_lst);
    for( int i_t = 0; i_t < t_lst.size(); i_t++ )
    {
        AutoHD<TTipBD> mod = at(t_lst[i_t]);
        mod.at().list(o_lst);
        for( int i_o = 0; i_o < o_lst.size(); i_o++ )
	    try
	    {	
		mod.at().at(o_lst[i_o]).at().load();
		if( !mod.at().at(o_lst[i_o]).at().enableStat() && mod.at().at(o_lst[i_o]).at().toEnable() )
        	    mod.at().at(o_lst[i_o]).at().enable(); 
	    } catch(...){ }	
    }

    //- Load modules -
    TSubSYS::subLoad();
}

void TBDS::subSave( )
{
    vector<string> t_lst, o_lst;
    
    //Opened DB save to DB
    modList(t_lst);
    for( int i_t = 0; i_t < t_lst.size(); i_t++ )
    {
        AutoHD<TTipBD> mod = at(t_lst[i_t]);
        mod.at().list(o_lst);
        for( int i_o = 0; i_o < o_lst.size(); i_o++ )
            mod.at().at(o_lst[i_o]).at().save();
    }
}

void TBDS::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
	TSubSYS::cntrCmdProc(opt);
	if(ctrMkNode("area",opt,0,"/sub",_("Subsystem"),0444,"root",subId().c_str()))
	{
	    ctrMkNode("comm",opt,-1,"/sub/load_db",_("Load"),0660,"root",subId().c_str());
    	    ctrMkNode("comm",opt,-1,"/sub/upd_db",_("Save"),0660,"root",subId().c_str());
	}
	ctrMkNode("fld",opt,-1,"/help/g_help",_("Options help"),0440,"root",subId().c_str(),3,"tp","str","cols","90","rows","10");
	return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/help/g_help" && ctrChkNode(opt,"get",0440,"root",subId().c_str(),SEQ_RD) ) 		opt->text(optDescr());
    else if( a_path == "/sub/load_db" && ctrChkNode(opt,"set",0660,"root",subId().c_str(),SEQ_WR) )	subLoad();
    else if( a_path == "/sub/upd_db" && ctrChkNode(opt,"set",0660,"root",subId().c_str(),SEQ_WR) )	subSave();
    else TSubSYS::cntrCmdProc(opt);
}

//================================================================
//=========== TTipBD =============================================
//================================================================
TTipBD::TTipBD( ) : full_db_del(false)
{ 
    m_db = grpAdd("db_");
}

TTipBD::~TTipBD( )
{
    nodeDelAll();
}

void TTipBD::open( const string &iid )
{
    if( openStat(iid) ) return;
    chldAdd(m_db,openBD(iid));
}

void TTipBD::cntrCmdProc( XMLNode *opt )
{
    string grp = owner().subId().c_str();
    //Get page info
    if( opt->name() == "info" )
    {
	TModule::cntrCmdProc(opt);
	ctrMkNode("grp",opt,-1,"/br/db_",_("Opened DB"),0444,"root","root",1,"list","/db/odb");
	ctrMkNode("area",opt,0,"/db",_("DB"),0444);
	ctrMkNode("fld",opt,-1,"/db/ful_db_del",_("Full DB delete"),0660,"root",grp.c_str(),1,"tp","bool");
	ctrMkNode("list",opt,-1,"/db/odb",_("Opened DB"),0664,"root",grp.c_str(),4,"tp","br","idm","1","s_com","add,del","br_pref","db_");
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/db/ful_db_del"  )
    {
	if( ctrChkNode(opt,"get",0660,"root",grp.c_str(),SEQ_RD) )	opt->text(full_db_del?"1":"0");
	if( ctrChkNode(opt,"set",0660,"root",grp.c_str(),SEQ_WR) )	full_db_del = atoi(opt->text().c_str());
    }	
    else if( a_path == "/db/odb" )
    {
	if( ctrChkNode(opt,"get",0664,"root",grp.c_str(),SEQ_RD) )
	{
	    vector<string> lst;
	    list(lst);
	    for( int i_l=0; i_l < lst.size(); i_l++)
		opt->childAdd("el")->attr("id",lst[i_l])->text(at(lst[i_l]).at().name());
	}
	if( ctrChkNode(opt,"add",0664,"root",grp.c_str(),SEQ_WR) )
        {
	    open(opt->attr("id"));
	    at(opt->attr("id")).at().name(opt->text());
        }
        if( ctrChkNode(opt,"del",0664,"root",grp.c_str(),SEQ_WR) )	close(opt->attr("id"),true);
    }
    else TModule::cntrCmdProc(opt);
}    

//================================================================
//=========== TBD ================================================
//================================================================
TBD::TBD( const string &iid, TElem *cf_el ) : TConfig( cf_el ),
    m_id(cfg("ID").getSd()), m_name(cfg("NAME").getSd()), m_dscr(cfg("DESCR").getSd()), 
    m_addr(cfg("ADDR").getSd()), m_codepage(cfg("CODEPAGE").getSd()), m_toen(cfg("EN").getBd()), 
    m_creat(false), m_en(false)
{
    m_id = iid;
    m_tbl = grpAdd("tbl_");
}

void TBD::postEnable( )
{
    cfg("TYPE").setS(owner().modId());
}

TBD::~TBD()
{

}

void TBD::preDisable(int flag)
{
    disable();
}

void TBD::postDisable(int flag)
{
    try
    {
        if( flag )
            SYS->db().at().dataDel(owner().owner().fullDB(),SYS->db().at().nodePath()+"DB/",*this);
    }catch(TError err)
    { mess_warning(err.cat.c_str(),"%s",err.mess.c_str()); }
}

string TBD::name()
{
    return (m_name.size())?m_name:id();
}

void TBD::enable( )
{
    if( enableStat() ) return;
    
    m_en = true;
}

void TBD::disable( )
{
    if( !enableStat() ) return;
    //Close all tables
    vector<string> t_list;
    list(t_list);
    for(int i_l = 0; i_l < t_list.size(); i_l++)
        close(t_list[i_l]);
	    
    m_en = false;
}

void TBD::open( const string &table, bool create )
{
    if( chldPresent(m_tbl,table) ) return;
    chldAdd(m_tbl,openTable(table, create)); 
}

void TBD::load( )
{
    SYS->db().at().dataGet(owner().owner().fullDB(),SYS->db().at().nodePath()+"DB/",*this);
}

void TBD::save( )
{
    SYS->db().at().dataSet(owner().owner().fullDB(),SYS->db().at().nodePath()+"DB/",*this);
}

void TBD::cntrCmdProc( XMLNode *opt )
{
    string grp = owner().owner().subId();
    //Get page info
    if( opt->name() == "info" )
    {
        ctrMkNode("oscada_cntr",opt,-1,"/",_("Data base: ")+name());
        ctrMkNode("area",opt,-1,"/prm",_("Data base"));
        ctrMkNode("area",opt,-1,"/prm/st",_("State"));
        ctrMkNode("fld",opt,-1,"/prm/st/st",_("Enable"),0664,"root",grp.c_str(),1,"tp","bool");
	ctrMkNode("fld",opt,-1,"/prm/st/mk",_("Create new DB"),0664,"root",grp.c_str(),1,"tp","bool");
        ctrMkNode("area",opt,-1,"/prm/cfg",_("Config"));
        ctrMkNode("fld",opt,-1,"/prm/cfg/id",cfg("ID").fld().descr(),0444,"root",grp.c_str(),1,"tp","str");
        ctrMkNode("fld",opt,-1,"/prm/cfg/nm",cfg("NAME").fld().descr(),0664,"root",grp.c_str(),1,"tp","str");
        ctrMkNode("fld",opt,-1,"/prm/cfg/dscr",cfg("DESCR").fld().descr(),0664,"root",grp.c_str(),3,"tp","str","cols","50","rows","3");
        ctrMkNode("fld",opt,-1,"/prm/cfg/addr",cfg("ADDR").fld().descr(),0664,"root",grp.c_str(),1,"tp","str");
	ctrMkNode("fld",opt,-1,"/prm/cfg/codep",cfg("CODEPAGE").fld().descr(),0664,"root",grp.c_str(),1,"tp","str");
	ctrMkNode("fld",opt,-1,"/prm/cfg/toen",cfg("EN").fld().descr(),0664,"root",grp.c_str(),1,"tp","bool");
        ctrMkNode("comm",opt,-1,"/prm/cfg/load",_("Load"),0660,"root",grp.c_str());
        ctrMkNode("comm",opt,-1,"/prm/cfg/save",_("Save"),0660,"root",grp.c_str());    
        return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/prm/st/st" )
    {
    	if( ctrChkNode(opt,"get",0664,"root",grp.c_str(),SEQ_RD) )	opt->text(enableStat()?"1":"0");
	if( ctrChkNode(opt,"set",0664,"root",grp.c_str(),SEQ_WR) )	atoi(opt->text().c_str())?enable():disable();
    }
    else if( a_path == "/prm/st/mk" )
    {
	if( ctrChkNode(opt,"get",0664,"root",grp.c_str(),SEQ_RD) )	opt->text(create()?"1":"0");
	if( ctrChkNode(opt,"set",0664,"root",grp.c_str(),SEQ_WR) )	create(atoi(opt->text().c_str()));
    }
    else if( a_path == "/prm/cfg/id" && ctrChkNode(opt) )	opt->text(id());
    else if( a_path == "/prm/cfg/nm" )
    {
	if( ctrChkNode(opt,"get",0664,"root",grp.c_str(),SEQ_RD) )	opt->text(name());
	if( ctrChkNode(opt,"set",0664,"root",grp.c_str(),SEQ_WR) )	name(opt->text());
    }
    else if( a_path == "/prm/cfg/dscr" )
    {
	if( ctrChkNode(opt,"get",0664,"root",grp.c_str(),SEQ_RD) )	opt->text(dscr());
	if( ctrChkNode(opt,"set",0664,"root",grp.c_str(),SEQ_WR) )	dscr(opt->text());
    }	
    else if( a_path == "/prm/cfg/addr" )
    {
	if( ctrChkNode(opt,"get",0664,"root",grp.c_str(),SEQ_RD) )	opt->text(addr());
	if( ctrChkNode(opt,"set",0664,"root",grp.c_str(),SEQ_WR) )	addr(opt->text());
    }
    else if( a_path == "/prm/cfg/codep" )
    {
	if( ctrChkNode(opt,"get",0664,"root",grp.c_str(),SEQ_RD) )	opt->text(codePage());
	if( ctrChkNode(opt,"set",0664,"root",grp.c_str(),SEQ_WR) )	codePage(opt->text());
    }    
    else if( a_path == "/prm/cfg/toen" )
    {
	if( ctrChkNode(opt,"get",0664,"root",grp.c_str(),SEQ_RD) )	opt->text(toEnable()?"1":"0");
	if( ctrChkNode(opt,"set",0664,"root",grp.c_str(),SEQ_WR) )	toEnable( atoi(opt->text().c_str()) );
    }
    else if( a_path == "/prm/cfg/load" && ctrChkNode(opt,"set",0660,"root",grp.c_str(),SEQ_WR) )	load();
    else if( a_path == "/prm/cfg/save" && ctrChkNode(opt,"set",0660,"root",grp.c_str(),SEQ_WR) )    	save();
}

//================================================================
//=========== TTable =============================================
//================================================================
TTable::TTable( const string &name ) :  m_name(name)
{

}

TTable::~TTable()
{ 

}
    
