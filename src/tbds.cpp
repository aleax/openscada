
//OpenSCADA system file: tbds.cpp
/***************************************************************************
 *   Copyright (C) 2003-2007 by Roman Savochenko                           *
 *   rom_as@fromru.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
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

#include "tsys.h"
#include "tmess.h"
#include "tmodule.h"
#include "tbds.h"

//************************************************
//* TBDS                                         *
//************************************************
TBDS::TBDS( ) : TSubSYS("BD","Data Bases",true)
{
    //- Generic system DB -
    fldAdd( new TFld("user","User",TFld::String,TCfg::Key,"20") );
    fldAdd( new TFld("id","Value ID",TFld::String,TCfg::Key,"100") );
    fldAdd( new TFld("val","Value"  ,TFld::String,TFld::NoFlag,"1000") );

    //- Open data bases DB structure -
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
}

string TBDS::realDBName( const string &bdn )
{
    string bd_t = TSYS::strSepParse(bdn,0,'.');
    string bd_n = TSYS::strSepParse(bdn,1,'.');
    string bd_tbl = TSYS::strSepParse(bdn,2,'.');

    return ((bd_t=="*") ? TSYS::strSepParse(SYS->workDB(),0,'.') : bd_t)+"."+
	   ((bd_n=="*") ? TSYS::strSepParse(SYS->workDB(),1,'.') : bd_n)+(bd_tbl.empty() ? "" : "."+bd_tbl);
}

void TBDS::dbList( vector<string> &ls )
{
    vector<string> tdb_ls, db_ls;

    ls.clear();

    modList(tdb_ls);
    for( int i_tp = 0; i_tp < tdb_ls.size(); i_tp++ )
    {
	SYS->db().at().at(tdb_ls[i_tp]).at().list(db_ls);
	for( int i_db = 0; i_db < db_ls.size(); i_db++ )
	    ls.push_back(tdb_ls[i_tp]+"."+db_ls[i_db]);
    }
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
		    //- Check keywords -
		    vector<string> cf_el;
		    cfg.cfgList(cf_el);
		
		    //- Check keywords -
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
	try{ tbl.at().fieldGet(cfg); }
	catch(TError err)
	{
	    if( err.cod != TSYS::DBRowNoPresent )
		mess_warning(err.cat.c_str(),"%s",err.mess.c_str());
	    db_true = false;
	}
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

bool TBDS::dataSet( const string &bdn, const string &path, TConfig &cfg )
{
    AutoHD<TTable> tbl = open(bdn,true);

    if( !tbl.freeStat() )
    {
	bool db_true = true;
	try{ tbl.at().fieldSet(cfg); }
	catch(TError err) { mess_warning(err.cat.c_str(),"%s",err.mess.c_str()); db_true = false; }
	tbl.free();
	try{ close(bdn); } 
	catch(TError err) { mess_warning(err.cat.c_str(),"%s",err.mess.c_str()); }
	return db_true;
    }
    //throw TError("BD","Field no present.");
    return false;
}

bool TBDS::dataDel( const string &bdn, const string &path, TConfig &cfg )
{
    AutoHD<TTable> tbl = open(bdn);

    if( !tbl.freeStat() )
    {
	bool db_true = true;
	try{ tbl.at().fieldDel(cfg); }
	catch(TError err) { mess_warning(err.cat.c_str(),"%s",err.mess.c_str()); db_true = false; }
	tbl.free();
	close(bdn);
	return db_true;
    }

    return false;
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

    if( SYS->present("BD") )
    {
	AutoHD<TBDS> dbs = SYS->db();

	//- Get from cache -
	ResAlloc res(dbs.at().genDBCacheRes,false);
	for( int i_cel = 0; i_cel < dbs.at().genDBCache.size(); i_cel++ )
	    if( dbs.at().genDBCache[i_cel]->cfg("user").getS() == user &&
			dbs.at().genDBCache[i_cel]->cfg("id").getS() == path )
		return dbs.at().genDBCache[i_cel]->cfg("val").getS();
	res.release();

	//- Get from generic DB -
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
    //- Get from config file -
    if(!bd_ok)
	try{ rez = SYS->ctrId(&SYS->cfgRoot(),path)->text(); }
	catch(...){ }

    //Put to cache
    if( SYS->present("BD") )
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

void TBDS::load_( )
{
    //- Load parameters from command line -
    int next_opt;
    const char *short_opt="h";
    struct option long_opt[] =
    {
	{"help"	,0,NULL,'h'},
	{NULL	,0,NULL,0  }
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
	c_el.cfgViewAll(false);
	int fld_cnt = 0;
	while( SYS->db().at().dataSeek(fullDB(),nodePath()+"DB/",fld_cnt++,c_el) )
	{
	    id = c_el.cfg("ID").getS();
	    type = c_el.cfg("TYPE").getS();
	    if( (type+"."+id) != SYS->workDB() && modPresent(type) && !at(type).at().openStat(id) )
		at(type).at().open(id);
	    c_el.cfg("ID").setS("");
	    c_el.cfg("TYPE").setS("");
	}
    }catch( TError err )
    {
	mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	mess_err(nodePath().c_str(),_("Search and open new DB error."));
    }
}

void TBDS::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
	TSubSYS::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/help/g_help",_("Options help"),0440,"root",subId().c_str(),3,"tp","str","cols","90","rows","10");
	return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/help/g_help" && ctrChkNode(opt,"get",0440,"root",subId().c_str(),SEQ_RD) )	opt->setText(optDescr());
    else TSubSYS::cntrCmdProc(opt);
}

//************************************************
//* TTipBD                                       *
//************************************************
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
    //Get page info
    if( opt->name() == "info" )
    {
	TModule::cntrCmdProc(opt);
	ctrMkNode("grp",opt,-1,"/br/db_",_("DB"),0664,"root","BD",1,"idm","1");
	if(ctrMkNode("area",opt,0,"/db",_("DB"),0444))
	{
	    ctrMkNode("fld",opt,-1,"/db/ful_db_del",_("Full DB delete"),0660,"root","BD",2,
		"tp","bool","help",_("Select for full deletion DB on DB close. Else DB will be simple closed."));
	    ctrMkNode("list",opt,-1,"/db/odb",_("DB"),0664,"root","BD",4,"tp","br","idm","1","s_com","add,del","br_pref","db_");
	}
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/db/ful_db_del"  )
    {
	if( ctrChkNode(opt,"get",0660,"root","BD",SEQ_RD) )	opt->setText(full_db_del?"1":"0");
	if( ctrChkNode(opt,"set",0660,"root","BD",SEQ_WR) )	full_db_del = atoi(opt->text().c_str());
    }
    else if( a_path == "/br/db_" || a_path == "/db/odb" )
    {
	if( ctrChkNode(opt,"get",0664,"root","BD",SEQ_RD) )
	{
	    vector<string> lst;
	    list(lst);
	    for( int i_l=0; i_l < lst.size(); i_l++)
		opt->childAdd("el")->setAttr("id",lst[i_l])->setText(at(lst[i_l]).at().name());
	}
	if( ctrChkNode(opt,"add",0664,"root","BD",SEQ_WR) )
	{
	    open(opt->attr("id"));
	    at(opt->attr("id")).at().setName(opt->text());
	}
	if( ctrChkNode(opt,"del",0664,"root","BD",SEQ_WR) )	close(opt->attr("id"),true);
    }
    else TModule::cntrCmdProc(opt);
}

//************************************************
//* TBD                                          *
//************************************************
TBD::TBD( const string &iid, TElem *cf_el ) : TConfig( cf_el ),
    m_id(cfg("ID").getSd()), m_name(cfg("NAME").getSd()), m_dscr(cfg("DESCR").getSd()),
    m_addr(cfg("ADDR").getSd()), m_codepage(cfg("CODEPAGE").getSd()), m_toen(cfg("EN").getBd()), m_en(false)
{
    m_id = iid;
    m_tbl = grpAdd("tbl_");
}

TCntrNode &TBD::operator=( TCntrNode &node )
{
    TBD *src_n = dynamic_cast<TBD*>(&node);
    if( !src_n ) return *this;

    if( !enableStat() )
    {
	string tid = id();
	*(TConfig*)this = *(TConfig*)src_n;
	m_id = tid;
    }

    if( src_n->enableStat() && enableStat() )
    {
	vector<string> tbl_ls;
	src_n->allowList(tbl_ls);
	for( int i_l = 0; i_l < tbl_ls.size(); i_l++ )
	{
	    //-- Open source and destination tables --
	    src_n->open(tbl_ls[i_l], false);
	    open(tbl_ls[i_l], true);
	    //-- Copy table --
	    (TCntrNode&)at(tbl_ls[i_l]).at() = (TCntrNode&)src_n->at(tbl_ls[i_l]).at();
	    //-- Close source and destination tables --
	    src_n->close(tbl_ls[i_l]);
	    close(tbl_ls[i_l]);
	}
    }

    return *this;
}

TBD::~TBD()
{

}

void TBD::postEnable( int flag )
{
    cfg("TYPE").setS(owner().modId());
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

void TBD::load_( )
{
    SYS->db().at().dataGet(owner().owner().fullDB(),SYS->db().at().nodePath()+"DB/",*this);
    if( !enableStat() && toEnable() )	enable();
}

void TBD::save_( )
{
    SYS->db().at().dataSet(owner().owner().fullDB(),SYS->db().at().nodePath()+"DB/",*this);
}

void TBD::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Data base: ")+name(),0664,"root","BD");
	ctrMkNode("branches",opt,-1,"/br","",0444);
	ctrMkNode("grp",opt,-1,"/br/tbl_",_("Opened table"),0664,"root","BD");
	if(ctrMkNode("area",opt,0,"/prm",_("Data base")))
	{
	    if(ctrMkNode("area",opt,-1,"/prm/st",_("State")))
	    {
		ctrMkNode("fld",opt,-1,"/prm/st/st",_("Enable"),0664,"root","BD",1,"tp","bool");
		ctrMkNode("list",opt,-1,"/prm/st/allow_tbls",_("Allow tables"),0664,"root","BD",3,
		    "tp","str","s_com","del","help",_("Tables contained into DB, no opened tables."));
	    }
	    if(ctrMkNode("area",opt,-1,"/prm/cfg",_("Config")))
	    {
		ctrMkNode("fld",opt,-1,"/prm/cfg/id",cfg("ID").fld().descr(),0444,"root","BD",1,"tp","str");
		ctrMkNode("fld",opt,-1,"/prm/cfg/nm",cfg("NAME").fld().descr(),0664,"root","BD",1,"tp","str");
		ctrMkNode("fld",opt,-1,"/prm/cfg/dscr",cfg("DESCR").fld().descr(),0664,"root","BD",3,"tp","str","cols","50","rows","3");
		ctrMkNode("fld",opt,-1,"/prm/cfg/addr",cfg("ADDR").fld().descr(),0664,"root","BD",1,"tp","str");
		ctrMkNode("fld",opt,-1,"/prm/cfg/codep",cfg("CODEPAGE").fld().descr(),0664,"root","BD",2,
		    "tp","str","help",_("Codepage of data into DB. For example: UTF-8, KOI8-R, KOI8-U ... ."));
		ctrMkNode("fld",opt,-1,"/prm/cfg/toen",cfg("EN").fld().descr(),0664,"root","BD",1,"tp","bool");
	    }
	}
	if(ctrMkNode("area",opt,1,"/tbls",_("Tables"),0444))
	    ctrMkNode("list",opt,-1,"/tbls/otbl",_("Opened tables"),0664,"root","BD",4,
		"tp","br","s_com","add,del","br_pref","tbl_","help",_("Opened table list.\nAdd and delete table operations is real open and close tables operations."));
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/prm/st/st" )
    {
	if( ctrChkNode(opt,"get",0664,"root","BD",SEQ_RD) )	opt->setText(enableStat()?"1":"0");
	if( ctrChkNode(opt,"set",0664,"root","BD",SEQ_WR) )	atoi(opt->text().c_str())?enable():disable();
    }
    else if( a_path == "/prm/st/allow_tbls" )
    {
	if( ctrChkNode(opt,"get",0664,"root","BD",SEQ_RD) )
	{
	    vector<string> lst;
	    allowList(lst);
	    for( int i_l=0; i_l < lst.size(); i_l++)
		opt->childAdd("el")->setText(lst[i_l]);
	}
	if( ctrChkNode(opt,"del",0664,"root","BD",SEQ_WR) )
	{
	    open( opt->text(), false );
	    close( opt->text(), true );
	}
    }
    else if( a_path == "/prm/cfg/id" && ctrChkNode(opt) )		opt->setText(id());
    else if( a_path == "/prm/cfg/nm" )
    {
	if( ctrChkNode(opt,"get",0664,"root","BD",SEQ_RD) )	opt->setText(name());
	if( ctrChkNode(opt,"set",0664,"root","BD",SEQ_WR) )	setName(opt->text());
    }
    else if( a_path == "/prm/cfg/dscr" )
    {
	if( ctrChkNode(opt,"get",0664,"root","BD",SEQ_RD) )	opt->setText(dscr());
	if( ctrChkNode(opt,"set",0664,"root","BD",SEQ_WR) )	setDscr(opt->text());
    }
    else if( a_path == "/prm/cfg/addr" )
    {
	if( ctrChkNode(opt,"get",0664,"root","BD",SEQ_RD) )	opt->setText(addr());
	if( ctrChkNode(opt,"set",0664,"root","BD",SEQ_WR) )	setAddr(opt->text());
    }
    else if( a_path == "/prm/cfg/codep" )
    {
	if( ctrChkNode(opt,"get",0664,"root","BD",SEQ_RD) )	opt->setText(codePage());
	if( ctrChkNode(opt,"set",0664,"root","BD",SEQ_WR) )	setCodePage(opt->text());
    }
    else if( a_path == "/prm/cfg/toen" )
    {
	if( ctrChkNode(opt,"get",0664,"root","BD",SEQ_RD) )	opt->setText(toEnable()?"1":"0");
	if( ctrChkNode(opt,"set",0664,"root","BD",SEQ_WR) )	setToEnable( atoi(opt->text().c_str()) );
    }
    else if( a_path == "/br/tbl_" || a_path == "/tbls/otbl" )
    {
	if( ctrChkNode(opt,"get",0664,"root","BD",SEQ_RD) )
	{
	    vector<string> lst;
	    list(lst);
	    for( int i_l=0; i_l < lst.size(); i_l++)
		opt->childAdd("el")->setText(lst[i_l]);
	}
	if( ctrChkNode(opt,"add",0664,"root","BD",SEQ_WR) )	open(opt->text(),true);
	if( ctrChkNode(opt,"del",0664,"root","BD",SEQ_WR) )	close(opt->text());
    }
    else TCntrNode::cntrCmdProc(opt);
}

//************************************************
//* TTable                                       *
//************************************************
TTable::TTable( const string &name ) :  m_name(name)
{
    modifClr();
}

TTable::~TTable()
{

}

TCntrNode &TTable::operator=( TCntrNode &node )
{
    TTable *src_n = dynamic_cast<TTable*>(&node);
    if( !src_n || !src_n->owner().enableStat() || !owner().enableStat() ) return *this;

    //- Table content copy -
    TConfig req;
    src_n->fieldStruct(req);
    //-- Scan source table and write to destination table
    for( int row = 0; src_n->fieldSeek(row,req); row++ )
    {
	fieldSet(req);
	//--- Clear key fields ---
	for( int i_e = 0; i_e < req.elem().fldSize(); i_e++ )
	    if( req.elem().fldAt(i_e).flg()&TCfg::Key )
		req.cfg(req.elem().fldAt(i_e).name()).setS("");
    }

    return *this;
}

void TTable::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Table: ")+name(),0664,"root","BD");
	if(ctrMkNode("area",opt,0,"/prm",_("Table")))
	{
	    if(ctrMkNode("area",opt,-1,"/prm/cfg",_("Config")))
		ctrMkNode("fld",opt,-1,"/prm/cfg/nm",_("Name"),0444,"root","BD",1,"tp","str");
	    XMLNode *tbl;
	    if( tbl=ctrMkNode("table",opt,-1,"/prm/tbl",_("Data"),0664,"root","BD",1,"s_com","add,del") )
	    {
		TConfig req;
		try{ fieldStruct(req); } catch(...) { }
		for( int i_f = 0; i_f < req.elem().fldSize(); i_f++ )
		{
		    string     eid = req.elem().fldAt(i_f).name();
		    TFld::Type etp = req.elem().fldAt(i_f).type();
		    if( req.elem().fldAt(i_f).flg()&TCfg::Key )
		        tbl->setAttr("key",tbl->attr("key")+eid+",");
		    ctrMkNode("list",opt,-1,("/prm/tbl/"+eid).c_str(),eid.c_str(),0664,"root","BD",
		        1,"tp",(etp==TFld::Boolean)?"bool":(etp==TFld::Integer)?"dec":(etp==TFld::Real)?"real":"str");
		}
	    }
	}
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/prm/cfg/nm" && ctrChkNode(opt) ) opt->setText(name());
    else if( a_path == "/prm/tbl" )
    {
	TConfig req;
	string eid;
	fieldStruct(req);
	if( ctrChkNode(opt,"get",0664,"root","BD",SEQ_RD) )
	    for( int i_r = 0; fieldSeek(i_r,req); i_r++ )
	        for( int i_f = 0; i_f < req.elem().fldSize(); i_f++ )
		{
		    eid = req.elem().fldAt(i_f).name();
		    if( i_r == 0 )	//Prepare columns
			ctrMkNode("list",opt,-1,("/prm/tbl/"+eid).c_str(),"",0664);
		    opt->childGet(i_f)->childAdd("el")->setText(req.cfg(eid).getS());
		    if( req.elem().fldAt(i_f).flg()&TCfg::Key )
			req.cfg(eid).setS("");
		}
	if( ctrChkNode(opt,"add",0664,"root","BD",SEQ_WR) )
	{
	    for( int i_f = 0; i_f < req.elem().fldSize(); i_f++ )
	    {
		eid = req.elem().fldAt(i_f).name();
		if( !(req.elem().fldAt(i_f).flg()&TCfg::Key) ) continue;
		req.cfg(eid).setS(_("newReqKey"));
	    }
	    req.cfgViewAll(false);
	    fieldSet(req);
	}
	if( ctrChkNode(opt,"del",0664,"root","BD",SEQ_WR) )
	{
	    for( int i_f = 0; i_f < req.elem().fldSize(); i_f++ )
	    {
	        eid = req.elem().fldAt(i_f).name();
	        if( !(req.elem().fldAt(i_f).flg()&TCfg::Key) ) continue;
	        req.cfg(eid).setS(opt->attr("key_"+eid));
	    }
	    fieldDel(req);
	}
	if( ctrChkNode(opt,"set",0664,"root","BD",SEQ_WR) )
	{
	    //- Get structure -
	    string  col = opt->attr("col");
	    bool key_chng = false;
	    for( int i_f = 0; i_f < req.elem().fldSize(); i_f++ )
	    {
	        eid = req.elem().fldAt(i_f).name();
	        if( !(req.elem().fldAt(i_f).flg()&TCfg::Key) ) continue;
	        req.cfg(eid).setS(opt->attr("key_"+eid));
	        if( eid == col ) key_chng = true;
	    }
	    if( key_chng )
	    {
	        fieldGet(req);
	        fieldDel(req);
	    }else req.cfgViewAll(false);
	    req.cfg(col).setS(opt->text(),true);
	    fieldSet(req);
	}
    }
    else TCntrNode::cntrCmdProc(opt);
}
