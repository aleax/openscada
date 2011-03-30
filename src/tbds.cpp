
//OpenSCADA system file: tbds.cpp
/***************************************************************************
 *   Copyright (C) 2003-2010 by Roman Savochenko                           *
 *   rom_as@oscada.org, rom_as@fromru.com                                  *
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

using namespace OSCADA;

//************************************************
//* TBDS                                         *
//************************************************
TBDS::TBDS( ) : TSubSYS(SDB_ID,_("Data Bases"),true), mSYSStPref(true)
{
    //> Generic system DB
    fldAdd( new TFld("user","User",TFld::String,TCfg::Key,"20") );
    fldAdd( new TFld("id",_("Value ID"),TFld::String,TCfg::Key,"100") );
    fldAdd( new TFld("val","Value"  ,TFld::String,TCfg::TransltText,"1000") );

    //> Open data bases DB structure
    el_db.fldAdd( new TFld("ID",_("ID"),TFld::String,TCfg::Key,"20") );
    el_db.fldAdd( new TFld("TYPE",_("DB type (module)"),TFld::String,TCfg::Key,"20") );
    el_db.fldAdd( new TFld("NAME",_("Name"),TFld::String,TCfg::TransltText,"50") );
    el_db.fldAdd( new TFld("DESCR",_("Description"),TFld::String,TCfg::TransltText,"200") );
    el_db.fldAdd( new TFld("ADDR",_("Address"),TFld::String,TFld::NoFlag,"100") );
    el_db.fldAdd( new TFld("CODEPAGE",_("Code page"),TFld::String,TFld::NoFlag,"20") );
    el_db.fldAdd( new TFld("EN",_("To enable"),TFld::Boolean,TFld::NoFlag,"1","1") );
}

TBDS::~TBDS(  )
{
    ResAlloc res(nodeRes(),true);
    while( genDBCache.size() )
    {
	delete genDBCache.front();
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

void TBDS::dbList( vector<string> &ls, bool checkSel )
{
    ls.clear();

    if( checkSel && !SYS->selDB().empty() )
    {
	if( SYS->selDB() != "<cfg>" ) ls.push_back(SYS->selDB());
	return;
    }

    vector<string> tdb_ls, db_ls;
    modList(tdb_ls);
    for(unsigned i_tp = 0; i_tp < tdb_ls.size(); i_tp++)
    {
	SYS->db().at().at(tdb_ls[i_tp]).at().list(db_ls);
	for(unsigned i_db = 0; i_db < db_ls.size(); i_db++)
	    ls.push_back(tdb_ls[i_tp]+"."+db_ls[i_db]);
    }
}

void TBDS::closeOldTables( int secOld )
{
    vector<string> tdbs, dbs, tbls;

    try
    {
	modList(tdbs);
	for(unsigned i_tdb = 0; i_tdb < tdbs.size(); i_tdb++)
	{
	    at(tdbs[i_tdb]).at().list(dbs);
	    for(unsigned i_db = 0; i_db < dbs.size(); i_db++)
	    {
		AutoHD<TBD> db = at(tdbs[i_tdb]).at().at(dbs[i_db]);
		db.at().list(tbls);
		for(unsigned i_tbl = 0; i_tbl < tbls.size(); i_tbl++)
		{
		    AutoHD<TTable> tbl = db.at().at(tbls[i_tbl]);
		    if((time(NULL)-tbl.at().lstUse()) > secOld)
		    {
			tbl.free();
			db.at().close(tbls[i_tbl]);
		    }
		}

		//> Check for transaction close
		db.at().transCloseCheck();
	    }
	}
    }catch(...){  }
}

AutoHD<TTable> TBDS::open( const string &bdn, bool create )
{
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
	    ResAlloc res(SYS->nodeRes(),false);
	    XMLNode *nd = ctrId(&SYS->cfgRoot(),path);
	    for(unsigned i_fld = 0, i_el; i_fld < nd->childSize(); i_fld++)
	    {
		XMLNode *el = nd->childGet(i_fld);
		if( el->name() == "fld" )
		{
		    //> Check keywords
		    vector<string> cf_el;
		    cfg.cfgList(cf_el);

		    //> Check keywords
		    for(i_el = 0; i_el < cf_el.size(); i_el++)
			if(cfg.cfg(cf_el[i_el]).fld().flg()&TCfg::Key &&
			    cfg.cfg(cf_el[i_el]).keyUse() &&
			    cfg.cfg(cf_el[i_el]).getS() != el->attr(cf_el[i_el])) break;
		    if(i_el == cf_el.size() && lev <= c_lev++)
		    {
			for(i_el = 0; i_el < cf_el.size(); i_el++)
			{
			    TCfg &u_cfg = cfg.cfg(cf_el[i_el]);
			    u_cfg.setS(el->attr(cf_el[i_el]));
			    if(!cfg.noTransl() && u_cfg.fld().flg()&TCfg::TransltText &&
				(Mess->lang2CodeBase().empty() || Mess->lang2Code() != Mess->lang2CodeBase()))
			    {
				string vl = el->attr(cf_el[i_el]+"_"+Mess->lang2Code());
				if(!vl.empty()) u_cfg.setS(vl);
			    }
			}
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
	    //tbl.free(); close(bdn);
	    return rez;
	}
    }

    return false;
}

bool TBDS::dataGet( const string &bdn, const string &path, TConfig &cfg )
{
    AutoHD<TTable> tbl = open(bdn);

    //> Load from DB
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
	if(db_true) return true;
    }
    //> Load from Config file if tbl no present
    XMLNode *nd;
    ResAlloc res(SYS->nodeRes(),false);
    try{ nd = ctrId(&SYS->cfgRoot(),path); }
    catch(...){ return false; }

    //>> Scan fields and fill Config
    for(unsigned i_fld = 0, i_el; i_fld < nd->childSize(); i_fld++)
    {
	XMLNode *el = nd->childGet(i_fld);
	if(el->name() == "fld")
	{
	    //Check keywords
	    vector<string> cf_el;
	    cfg.cfgList(cf_el);

	    for(i_el = 0; i_el < cf_el.size(); i_el++)
		if(cfg.cfg(cf_el[i_el]).fld().flg()&TCfg::Key &&
		    cfg.cfg(cf_el[i_el]).getS() != el->attr(cf_el[i_el])) break;
	    if(i_el == cf_el.size())
	    {
		for(i_el = 0; i_el < cf_el.size(); i_el++)
		{
		    TCfg &u_cfg = cfg.cfg(cf_el[i_el]);
		    u_cfg.setS(el->attr(cf_el[i_el]));
		    if(!cfg.noTransl() && u_cfg.fld().flg()&TCfg::TransltText &&
			(Mess->lang2CodeBase().empty() || Mess->lang2Code() != Mess->lang2CodeBase()))
		    {
			string vl = el->attr(cf_el[i_el]+"_"+Mess->lang2Code());
			if(!vl.empty()) u_cfg.setS(vl);
		    }
		}
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
	return db_true;
    }
    //throw TError("BD","Field no present.");
    return false;
}

bool TBDS::dataDel( const string &bdn, const string &path, TConfig &cfg, bool useKeyAll )
{
    vector<string> cels;
    AutoHD<TTable> tbl = open(bdn);

    if(!tbl.freeStat())
    {
	bool db_true = true;
	try
	{
	    //> Select for using all keys
	    if(useKeyAll)
	    {
		cfg.cfgList(cels);
		for(unsigned i_el = 0; i_el < cels.size(); )
		{
		    if((cfg.cfg(cels[i_el]).fld().flg()&TCfg::Key) && !cfg.cfg(cels[i_el]).keyUse())
			cfg.cfg(cels[i_el]).setKeyUse(true);
		    else { cels.erase(cels.begin()+i_el); continue; }
		    i_el++;
		}
	    }

	    tbl.at().fieldDel(cfg);

	    //> Restore not using keys selection
	    if(useKeyAll)
		for(unsigned i_el = 0; i_el < cels.size(); i_el++)
		    cfg.cfg(cels[i_el]).setKeyUse(false);
	}
	catch(TError err) { mess_warning(err.cat.c_str(),"%s",err.mess.c_str()); db_true = false; }
	//tbl.free(); close(bdn);
	return db_true;
    }

    return false;
}

void TBDS::genDBSet(const string &path, const string &val, const string &user, char rFlg )
{
    string rez;

    //Set to DB
    if(SYS->present(SDB_ID))
    {
	AutoHD<TBDS> dbs = SYS->db();
	AutoHD<TTable> tbl = dbs.at().open(dbs.at().fullDBSYS(),true);
	if( !tbl.freeStat() )
	{
	    TConfig db_el(&dbs.at());
	    db_el.setNoTransl( !(rFlg&TBDS::UseTranslate) );
	    db_el.cfg("user").setS(user);
	    if(dbs.at().mSYSStPref) db_el.cfg("id").setS(path);
	    else
	    {
		int off = 0;
		TSYS::pathLev(path,0,true,&off);
		db_el.cfg("id").setS(path.substr(off));
	    }
	    db_el.cfg("val").setS(val);

	    try{ tbl.at().fieldSet(db_el); }
	    catch(TError err){ }
	}
	else
	{
	    //Put to cache
	    ResAlloc res(dbs.at().nodeRes(),true);
	    for(unsigned i_cel = 0; i_cel < dbs.at().genDBCache.size(); i_cel++)
		if(dbs.at().genDBCache[i_cel]->cfg("user").getS() == user && dbs.at().genDBCache[i_cel]->cfg("id").getS() == path)
		{ dbs.at().genDBCache[i_cel]->cfg("val").setS(val); return; }
	    TConfig *db_el = new TConfig(&dbs.at());
	    db_el->cfg("user").setS(user);
	    db_el->cfg("id").setS(path);
	    db_el->cfg("val").setS(val);
	    dbs.at().genDBCache.push_front(db_el);
	    while( dbs.at().genDBCache.size() > 100 )
	    {
		delete dbs.at().genDBCache.back();
		dbs.at().genDBCache.pop_back();
	    }
	}
    }
}

string TBDS::genDBGet(const string &path, const string &oval, const string &user, char rFlg )
{
    bool bd_ok = false;
    string rez = oval;

    //> Get from generic DB
    if(SYS->present(SDB_ID) && !(rFlg&TBDS::OnlyCfg))
    {
	AutoHD<TBDS> dbs = SYS->db();
	AutoHD<TTable> tbl = dbs.at().open(SYS->db().at().fullDBSYS());
	if(!tbl.freeStat())
	{
	    TConfig db_el(&dbs.at());
	    db_el.setNoTransl(!(rFlg&TBDS::UseTranslate));
	    db_el.cfg("user").setS(user);
	    if(dbs.at().mSYSStPref) db_el.cfg("id").setS(path);
	    else
	    {
		int off = 0;
		TSYS::pathLev(path,0,true,&off);
		db_el.cfg("id").setS(path.substr(off));
	    }
	    try
	    {
		tbl.at().fieldGet(db_el);
		rez = db_el.cfg("val").getS();
		bd_ok = true;
	    }
	    catch(TError err){  }
	}
    }

    if(!bd_ok)
    {
	//> Get from cache
	if(SYS->present(SDB_ID))
	{
	    AutoHD<TBDS> dbs = SYS->db();
	    ResAlloc res(dbs.at().nodeRes(),false);
	    for(unsigned i_cel = 0; i_cel < dbs.at().genDBCache.size(); i_cel++)
		if(dbs.at().genDBCache[i_cel]->cfg("user").getS() == user && dbs.at().genDBCache[i_cel]->cfg("id").getS() == path)
		    return dbs.at().genDBCache[i_cel]->cfg("val").getS();
	}

	//> Get from config file
	ResAlloc res(SYS->nodeRes(),false);
	XMLNode *tgtN = TCntrNode::ctrId(&SYS->cfgRoot(),path,true);
	if(tgtN) rez = tgtN->text();
	if(rFlg&TBDS::UseTranslate)
	{
	    tgtN = TCntrNode::ctrId(&SYS->cfgRoot(),path+"_"+Mess->lang2Code(),true);
	    if(tgtN) rez = tgtN->text();
	}
    }

    return rez;
}

string TBDS::optDescr(  )
{
    char buf[STR_BUF_LEN];
    snprintf(buf,sizeof(buf),_(
	"========================= Subsystem \"DB\" options ========================\n"
	"----------- The config file station <%s> parameters -----------\n"
	"SYSStPref    <1>   Use station id prefix into generic (SYS) table.\n\n"
	),nodePath().c_str());

    return(buf);
}

void TBDS::load_( )
{
    //> Load parameters from command line
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
	    case 'h': fprintf(stdout,"%s",optDescr().c_str()); break;
	    case -1 : break;
	}
    } while(next_opt != -1);

    //> Load parameters from config file
    mSYSStPref = (bool)atoi(TBDS::genDBGet(nodePath()+"SYSStPref",(mSYSStPref?"1":"0"),"root",TBDS::OnlyCfg).c_str());

    //> DB open
    //>> Open, load and enable generic DB
    string db_tp = TSYS::strSepParse(fullDB(),0,'.');
    string db_nm = TSYS::strSepParse(fullDB(),1,'.');
    if( !at(db_tp).at().openStat(db_nm) )
    {
	at(db_tp).at().open(db_nm);
	at(db_tp).at().at(db_nm).at().load();
	at(db_tp).at().at(db_nm).at().enable();
    }

    //>> Open other DB stored into table 'DB' and config file
    try
    {
	string id,type;
	if( SYS->chkSelDB(fullDB()) )
	{
	    TConfig c_el(&el_db);
	    c_el.cfgViewAll(false);
	    for( int fld_cnt = 0; SYS->db().at().dataSeek(fullDB(),nodePath()+"DB/",fld_cnt++,c_el); )
	    {
		id = c_el.cfg("ID").getS();
		type = c_el.cfg("TYPE").getS();
		if( (type+"."+id) != SYS->workDB() && modPresent(type) && !at(type).at().openStat(id) )
		    at(type).at().open(id);
	    }
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
    if(opt->name() == "info")
    {
	TSubSYS::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/help/g_help",_("Options help"),R_R___,"root",SDB_ID,3,"tp","str","cols","90","rows","10");
	return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/help/g_help" && ctrChkNode(opt,"get",R_R___,"root",SDB_ID,SEC_RD))	opt->setText(optDescr());
    else TSubSYS::cntrCmdProc(opt);
}

//************************************************
//* TTipBD                                       *
//************************************************
TTipBD::TTipBD( const string &id ) : TModule(id), full_db_del(false)
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

TBDS &TTipBD::owner( )	{ return (TBDS&)TModule::owner(); }

void TTipBD::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info")
    {
	TModule::cntrCmdProc(opt);
	ctrMkNode("grp",opt,-1,"/br/db_",_("DB"),RWRWR_,"root",SDB_ID,2,"idm","1","idSz","20");
	if(ctrMkNode("area",opt,0,"/db",_("DB"),R_R_R_))
	{
	    ctrMkNode("fld",opt,-1,"/db/ful_db_del",_("Full DB delete"),RWRW__,"root",SDB_ID,2,
		"tp","bool","help",_("Select for full deletion DB on DB close. Else DB will be simply closed."));
	    ctrMkNode("list",opt,-1,"/db/odb",_("DB"),RWRWR_,"root",SDB_ID,5,"tp","br","idm","1","s_com","add,del","br_pref","db_","idSz","20");
	}
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/db/ful_db_del")
    {
	if(ctrChkNode(opt,"get",RWRW__,"root",SDB_ID,SEC_RD))	opt->setText(full_db_del?"1":"0");
	if(ctrChkNode(opt,"set",RWRW__,"root",SDB_ID,SEC_WR))	full_db_del = atoi(opt->text().c_str());
    }
    else if(a_path == "/br/db_" || a_path == "/db/odb")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDB_ID,SEC_RD))
	{
	    vector<string> lst;
	    list(lst);
	    for(unsigned i_l=0; i_l < lst.size(); i_l++)
		opt->childAdd("el")->setAttr("id",lst[i_l])->setText(at(lst[i_l]).at().name());
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SDB_ID,SEC_WR))
	{
	    string vid = TSYS::strEncode(opt->attr("id"),TSYS::oscdID);
	    open(vid); at(vid).at().setName(opt->text());
	}
	if(ctrChkNode(opt,"del",RWRWR_,"root",SDB_ID,SEC_WR))	close(opt->attr("id"),true);
    }
    else TModule::cntrCmdProc(opt);
}

//************************************************
//* TBD                                          *
//************************************************
TBD::TBD( const string &iid, TElem *cf_el ) : TConfig( cf_el ),
    mId(cfg("ID").getSd()), mName(cfg("NAME").getSd()), mDscr(cfg("DESCR").getSd()),
    mAddr(cfg("ADDR").getSd()), mCodepage(cfg("CODEPAGE").getSd()), mToEn(cfg("EN").getBd()),
    mEn(false), userSQLTrans(EVAL_BOOL)
{
    mId = iid;
    mTbl = grpAdd("tbl_");
}

TCntrNode &TBD::operator=( TCntrNode &node )
{
    TBD *src_n = dynamic_cast<TBD*>(&node);
    if(!src_n) return *this;

    if(!enableStat())
    {
	string tid = id();
	*(TConfig*)this = *(TConfig*)src_n;
	cfg("TYPE").setS(owner().modId());
	mId = tid;
	modif();
    }

    if(src_n->enableStat() && enableStat())
    {
	vector<string> tbl_ls;
	src_n->allowList(tbl_ls);
	for(unsigned i_l = 0; i_l < tbl_ls.size(); i_l++)
	{
	    //> Open source and destination tables
	    src_n->open(tbl_ls[i_l], false);
	    open(tbl_ls[i_l], true);
	    //> Copy table
	    (TCntrNode&)at(tbl_ls[i_l]).at() = (TCntrNode&)src_n->at(tbl_ls[i_l]).at();
	    //> Close source and destination tables
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
	    SYS->db().at().dataDel(owner().owner().fullDB(),SYS->db().at().nodePath()+"DB/",*this,true);
    }catch(TError err)
    { mess_warning(err.cat.c_str(),"%s",err.mess.c_str()); }
}

TTipBD &TBD::owner( )	{ return *(TTipBD*)nodePrev(); }

string TBD::name( )	{ return (mName.size())?mName:id(); }

void TBD::enable( )
{
    if( enableStat() ) return;

    mEn = true;
}

void TBD::disable( )
{
    if(!enableStat()) return;
    //Close all tables
    vector<string> t_list;
    list(t_list);
    for(unsigned i_l = 0; i_l < t_list.size(); i_l++)
	close(t_list[i_l]);

    mEn = false;
}

void TBD::open( const string &table, bool create )
{
    if(chldPresent(mTbl,table)) return;
    chldAdd(mTbl,openTable(table, create));
}

void TBD::load_( )
{
    if( !SYS->chkSelDB(SYS->workDB()) ) return;
    SYS->db().at().dataGet(owner().owner().fullDB(),SYS->db().at().nodePath()+"DB/",*this);
    if( !enableStat() && toEnable() )	enable();
}

void TBD::save_( )
{
    SYS->db().at().dataSet(owner().owner().fullDB(),SYS->db().at().nodePath()+"DB/",*this);
}

TVariant TBD::objFuncCall( const string &iid, vector<TVariant> &prms, const string &user )
{
    // Array SQLReq(string req) - formation of the SQL-request to the DB.
    //  req - SQL-request text
    if(iid == "SQLReq" && prms.size() >= 1)
    {
	TArrayObj *rez = new TArrayObj();
	try
	{
	    vector< vector<string> > rtbl;
	    sqlReq(prms[0].getS(), &rtbl, ((prms.size()>=2)?prms[1].getB():EVAL_BOOL));
	    for(unsigned i_r = 0; i_r < rtbl.size(); i_r++)
	    {
		TArrayObj *row = new TArrayObj();
		for(unsigned i_c = 0; i_c < rtbl[i_r].size(); i_c++)
		{
		    row->propSet(TSYS::int2str(i_c),rtbl[i_r][i_c]);
		    if(i_r) row->TVarObj::propSet(rtbl[0][i_c], rtbl[i_r][i_c]);
		}
		rez->propSet(TSYS::int2str(i_r),row);
	    }
	}catch(...){ }
	return rez;
    }
    return TCntrNode::objFuncCall(iid,prms,user);
}

AutoHD<TCntrNode> TBD::chldAt( int8_t igr, const string &name, const string &user )
{
    try { return TCntrNode::chldAt(igr, name, user); }
    catch(...)
    {
	if(igr == mTbl && !openStat(name))
	{
	    open(name,false);
	    return TCntrNode::chldAt(igr, name, user);
	}
	else throw;
    }
}

void TBD::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info")
    {
	TCntrNode::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Data base: ")+name(),RWRWR_,"root",SDB_ID);
	ctrMkNode("branches",opt,-1,"/br","",R_R_R_);
	ctrMkNode("grp",opt,-1,"/br/tbl_",_("Opened table"),RWRW__,"root",SDB_ID,1,"idSz","255");
	if(ctrMkNode("area",opt,0,"/prm",_("Data base")))
	{
	    if(ctrMkNode("area",opt,-1,"/prm/st",_("State")))
	    {
		ctrMkNode("fld",opt,-1,"/prm/st/st",_("Enable"),RWRWR_,"root",SDB_ID,1,"tp","bool");
		ctrMkNode("list",opt,-1,"/prm/st/allow_tbls",_("Accessible tables"),RWRWR_,"root",SDB_ID,4,
		    "tp","br","br_pref","tbl_","s_com","del","help",_("Tables which are in the DB, tables which are not opened at that moment."));
		ctrMkNode("comm",opt,-1,"/prm/st/load",_("Load system from this DB"),RWRW__,"root","root");
	    }
	    if(ctrMkNode("area",opt,-1,"/prm/cfg",_("Config")))
	    {
		ctrMkNode("fld",opt,-1,"/prm/cfg/id",cfg("ID").fld().descr(),R_R_R_,"root",SDB_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/prm/cfg/nm",cfg("NAME").fld().descr(),RWRWR_,"root",SDB_ID,2,"tp","str","len","50");
		ctrMkNode("fld",opt,-1,"/prm/cfg/dscr",cfg("DESCR").fld().descr(),RWRWR_,"root",SDB_ID,3,"tp","str","cols","100","rows","3");
		ctrMkNode("fld",opt,-1,"/prm/cfg/addr",cfg("ADDR").fld().descr(),RWRW__,"root",SDB_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/prm/cfg/codep",cfg("CODEPAGE").fld().descr(),RWRWR_,"root",SDB_ID,2,
		    "tp","str","help",_("Codepage of data into DB. For example: UTF-8, KOI8-R, KOI8-U ... ."));
		ctrMkNode("fld",opt,-1,"/prm/cfg/toen",cfg("EN").fld().descr(),RWRWR_,"root",SDB_ID,1,"tp","bool");
	    }
	}
	if(ctrMkNode("area",opt,1,"/tbls",_("Tables"),R_R___))
	    ctrMkNode("list",opt,-1,"/tbls/otbl",_("Opened tables"),RWRW__,"root",SDB_ID,5,
		"tp","br","idSz","255","s_com","add,del","br_pref","tbl_",
		"help",_("Opened table list.\nAdding and deleting tables operations are really open and close tables operations."));
	if(enableStat( ) && ctrMkNode("area",opt,-1,"/sql",_("SQL"),R_R___,"root",SDB_ID))
	{
	    ctrMkNode("fld",opt,-1,"/sql/req",_("Request"),RWRW__,"root",SDB_ID,3,"tp","str","cols","100","rows","5");
	    ctrMkNode("fld",opt,-1,"/sql/trans",_("Transaction"),RWRW__,"root",SDB_ID,4,"tp","dec","dest","select",
		"sel_id","0;1;2","sel_list",_("Out;Into;No matter"));
	    ctrMkNode("comm",opt,-1,"/sql/send",_("Send"),RWRW__,"root",SDB_ID);
	    ctrMkNode("table",opt,-1,"/sql/tbl",_("Result"),R_R___,"root",SDB_ID);
	}
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/prm/st/st" )
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDB_ID,SEC_RD))	opt->setText(enableStat()?"1":"0");
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDB_ID,SEC_WR))	atoi(opt->text().c_str())?enable():disable();
    }
    else if(a_path == "/prm/st/allow_tbls")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDB_ID,SEC_RD))
	{
	    vector<string> lst;
	    allowList(lst);
	    for(unsigned i_l=0; i_l < lst.size(); i_l++)
		opt->childAdd("el")->setText(lst[i_l]);
	}
	if(ctrChkNode(opt,"del",RWRWR_,"root",SDB_ID,SEC_WR))
	{
	    open(opt->text(), false);
	    close(opt->text(), true);
	}
    }
    else if(a_path == "/prm/st/load" && ctrChkNode(opt,"set",RWRW__,"root","root",SEC_WR))
    {
	SYS->setSelDB(owner().modId()+"."+id());
	SYS->modifG();
	SYS->load();
	SYS->setSelDB("");
    }
    else if(a_path == "/prm/cfg/id" && ctrChkNode(opt))		opt->setText(id());
    else if(a_path == "/prm/cfg/nm")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDB_ID,SEC_RD))	opt->setText(name());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDB_ID,SEC_WR))	setName(opt->text());
    }
    else if(a_path == "/prm/cfg/dscr")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDB_ID,SEC_RD))	opt->setText(dscr());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDB_ID,SEC_WR))	setDscr(opt->text());
    }
    else if(a_path == "/prm/cfg/addr")
    {
	if(ctrChkNode(opt,"get",RWRW__,"root",SDB_ID,SEC_RD))	opt->setText(addr());
	if(ctrChkNode(opt,"set",RWRW__,"root",SDB_ID,SEC_WR))	setAddr(opt->text());
    }
    else if(a_path == "/prm/cfg/codep")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDB_ID,SEC_RD))	opt->setText(codePage());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDB_ID,SEC_WR))	setCodePage(opt->text());
    }
    else if(a_path == "/prm/cfg/toen")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDB_ID,SEC_RD))	opt->setText(toEnable()?"1":"0");
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDB_ID,SEC_WR))	setToEnable( atoi(opt->text().c_str()) );
    }
    else if(a_path == "/br/tbl_" || a_path == "/tbls/otbl")
    {
	if(ctrChkNode(opt,"get",RWRW__,"root",SDB_ID,SEC_RD))
	{
	    vector<string> lst;
	    list(lst);
	    for(unsigned i_l=0; i_l < lst.size(); i_l++)
		opt->childAdd("el")->setText(lst[i_l]);
	}
	if(ctrChkNode(opt,"add",RWRW__,"root",SDB_ID,SEC_WR))	open(opt->text(),true);
	if(ctrChkNode(opt,"del",RWRW__,"root",SDB_ID,SEC_WR))	close(opt->text());
    }
    else if(a_path == "/sql/req")
    {
	if(ctrChkNode(opt,"get",RWRW__,"root",SDB_ID))	opt->setText(userSQLReq);
	if(ctrChkNode(opt,"set",RWRW__,"root",SDB_ID))	userSQLReq = opt->text();
    }
    else if(a_path == "/sql/trans")
    {
	if(ctrChkNode(opt,"get",RWRW__,"root",SDB_ID))	opt->setText(TSYS::int2str(userSQLTrans));
	if(ctrChkNode(opt,"set",RWRW__,"root",SDB_ID))	userSQLTrans = atoi(opt->text().c_str());
    }
    else if(a_path == "/sql/send" && enableStat( ) && ctrChkNode(opt,"set",RWRW__,"root",SDB_ID,SEC_WR))
	sqlReq(userSQLReq,&userSQLResTbl,userSQLTrans);
    else if(a_path == "/sql/tbl" && ctrChkNode(opt,"get",R_R___,"root",SDB_ID,SEC_RD))
	for(unsigned i_r = 0; i_r < userSQLResTbl.size(); i_r++)
	    for(unsigned i_c = 0; i_c < userSQLResTbl[i_r].size(); i_c++)
	    {
		if(i_r == 0)
		    ctrMkNode("list",opt,i_c,("/sql/tbl/"+userSQLResTbl[0][i_c]).c_str(),userSQLResTbl[0][i_c],R_R___,"root",SDB_ID,1,"tp","str");
		else opt->childGet(i_c)->childAdd("el")->setText(userSQLResTbl[i_r][i_c]);
	    }
    else TCntrNode::cntrCmdProc(opt);
}

//************************************************
//* TTable                                       *
//************************************************
TTable::TTable( const string &name ) :  mName(name)
{
    modifClr();
    mLstUse = time(NULL);
}

TTable::~TTable()
{

}

TCntrNode &TTable::operator=( TCntrNode &node )
{
    TTable *src_n = dynamic_cast<TTable*>(&node);
    if( !src_n || !src_n->owner().enableStat() || !owner().enableStat() ) return *this;

    //> Table content copy
    TConfig req;
    src_n->fieldStruct(req);
    //>> Scan source table and write to destination table
    for( int row = 0; src_n->fieldSeek(row,req); row++ ) fieldSet(req);

    return *this;
}

TBD &TTable::owner( )	{ return *(TBD*)nodePrev(); }

TVariant TTable::objFuncCall( const string &iid, vector<TVariant> &prms, const string &user )
{
    // XMLNodeObj fieldStruct() - field structure get.
    if(iid == "fieldStruct")
    {
        XMLNodeObj *rez = new XMLNodeObj("field");
	TConfig cfg;
	vector<string> elst;

	try
	{
	    fieldStruct(cfg);

	    cfg.cfgList(elst);
	    for(unsigned i_el = 0; i_el < elst.size(); i_el++)
	    {
		TCfg &icfg = cfg.cfg(elst[i_el]);
		string stp = "str";
		switch(icfg.fld().type())
		{
		    case TFld::Boolean:	stp = "bool";	break;
		    case TFld::Integer:	stp = "int";	break;
		    case TFld::Real:	stp = "real";	break;
		    case TFld::String:	stp = "str";	break;
	        }
		XMLNodeObj *el = new XMLNodeObj(icfg.name());
		el->propSet("type",stp);
		el->propSet("len",TSYS::int2str(icfg.fld().len())+"."+TSYS::int2str(icfg.fld().dec()));
		el->propSet("def",icfg.fld().def());
		el->propSet("key",(icfg.fld().flg()&TCfg::Key)?"1":"0");
		rez->childAdd(el);
	    }
	} catch(TError err) { }

	return rez;
    }
    // string fieldSeek(int row, XMLNodeObj fld) - Field seek for <row> and <fld>.
    // string fieldGet(XMLNodeObj fld) - Get field value for set keys
    // string fieldSet(XMLNodeObj fld) - Set field value for set keys
    // string fieldDel(XMLNodeObj fld) - Remove field for set keys
    //  row - seeked row number;
    //  fld - source and result field's record.
    if((iid == "fieldSeek" && prms.size() >= 2 && dynamic_cast<XMLNodeObj*>(prms[1].getO())) ||
	((iid == "fieldGet" || iid == "fieldSet" || iid == "fieldDel") &&
	    prms.size() >= 1 && dynamic_cast<XMLNodeObj*>(prms[0].getO())))
    {
	string rez = "1";
	bool isRet = false;

	try
	{
	    XMLNodeObj *fld = (XMLNodeObj*)prms[(iid=="fieldSeek")?1:0].getO();
	    TConfig cfg;
	    for(unsigned i_r = 0; i_r < fld->childSize(); i_r++)
	    {
		XMLNodeObj *xel = fld->childGet(i_r);
		TFld::Type eltp = TFld::String;
		if(xel->propGet("type").getS() == "int")	eltp = TFld::Integer;
		else if(xel->propGet("type").getS() == "real")	eltp = TFld::Real;
		else if(xel->propGet("type").getS() == "bool")	eltp = TFld::Boolean;
		cfg.elem().fldAdd(new TFld(xel->name().c_str(),"",eltp,(xel->propGet("key").getI()==1?(int)TCfg::Key:(int)TFld::NoFlag),
		    xel->propGet("len").getS().c_str(),xel->propGet("def").getS().c_str()));
		cfg.cfg(xel->name()).setS(xel->text());
	    }

	    if(iid == "fieldSeek")	{ rez = TSYS::int2str(fieldSeek(prms[0].getI(), cfg)); isRet = true; }
	    else if(iid == "fieldGet")	{ fieldGet(cfg); isRet = true; }
	    else if(iid == "fieldSet")	fieldSet(cfg);
	    else if(iid == "fieldDel")	fieldDel(cfg);

	    if(isRet)
	    {
		vector<string> el;
		cfg.cfgList(el);
		for(unsigned i_el = 0; i_el < el.size(); i_el++)
		    fld->childGet(i_el)->setText(cfg.cfg(el[i_el]).getS());
	    }
	}catch(TError err) { rez = "0:"+err.mess; }

	return rez;
    }
    return TCntrNode::objFuncCall(iid,prms,user);
}

void TTable::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
	TCntrNode::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Table: ")+name(),RWRW__,"root",SDB_ID);
	if(ctrMkNode("area",opt,0,"/prm",_("Table")))
	{
	    if(ctrMkNode("area",opt,-1,"/prm/cfg",_("Config")))
		ctrMkNode("fld",opt,-1,"/prm/cfg/nm",_("Name"),R_R___,"root",SDB_ID,1,"tp","str");
	    XMLNode *tbl;
	    if((tbl=ctrMkNode("table",opt,-1,"/prm/tbl",_("Data"),RWRW__,"root",SDB_ID,1,"s_com","add,del")))
	    {
		TConfig req;
		try{ fieldStruct(req); } catch(...) { }
		for(unsigned i_f = 0; i_f < req.elem().fldSize(); i_f++)
		{
		    string     eid = req.elem().fldAt(i_f).name();
		    TFld::Type etp = req.elem().fldAt(i_f).type();
		    if( req.elem().fldAt(i_f).flg()&TCfg::Key )
		        tbl->setAttr("key",tbl->attr("key")+eid+",");
		    ctrMkNode("list",opt,-1,("/prm/tbl/"+eid).c_str(),eid.c_str(),RWRW__,"root",SDB_ID,
		        1,"tp",(etp==TFld::Boolean)?"bool":(etp==TFld::Integer)?"dec":(etp==TFld::Real)?"real":"str");
		}
	    }
	}
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/cfg/nm" && ctrChkNode(opt,"get",R_R___,"root",SDB_ID,SEC_RD)) opt->setText(name());
    else if(a_path == "/prm/tbl")
    {
	TConfig req;
	string eid;
	fieldStruct(req);
	if(ctrChkNode(opt,"get",RWRW__,"root",SDB_ID,SEC_RD))
	    for(unsigned i_r = 0; fieldSeek(i_r,req); i_r++)
	        for(unsigned i_f = 0; i_f < req.elem().fldSize(); i_f++)
		{
		    eid = req.elem().fldAt(i_f).name();
		    if( i_r == 0 )	//Prepare columns
			ctrMkNode("list",opt,-1,("/prm/tbl/"+eid).c_str(),"",RWRWR_);
		    opt->childGet(i_f)->childAdd("el")->setText(req.cfg(eid).getS());
		}
	if(ctrChkNode(opt,"add",RWRW__,"root",SDB_ID,SEC_WR))
	{
	    for(unsigned i_f = 0; i_f < req.elem().fldSize(); i_f++)
	    {
		eid = req.elem().fldAt(i_f).name();
		if(!(req.elem().fldAt(i_f).flg()&TCfg::Key)) continue;
		req.cfg(eid).setS("newReqKey");
	    }
	    req.cfgViewAll(false);
	    fieldSet(req);
	}
	if(ctrChkNode(opt,"del",RWRW__,"root",SDB_ID,SEC_WR))
	{
	    for(unsigned i_f = 0; i_f < req.elem().fldSize(); i_f++)
		if(req.elem().fldAt(i_f).flg()&TCfg::Key)
		{
		    eid = req.elem().fldAt(i_f).name();
		    req.cfg(eid).setS(opt->attr("key_"+eid),true);
		}
	    fieldDel(req);
	}
	if(ctrChkNode(opt,"set",RWRW__,"root",SDB_ID,SEC_WR))
	{
	    //> Get structure
	    string  col = opt->attr("col");
	    bool key_chng = false;
	    for(unsigned i_f = 0; i_f < req.elem().fldSize(); i_f++)
		if(req.elem().fldAt(i_f).flg()&TCfg::Key)
		{
		    eid = req.elem().fldAt(i_f).name();
		    req.cfg(eid).setS(opt->attr("key_"+eid),true);
		    if(eid == col) key_chng = true;
		}
	    if(key_chng)
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
