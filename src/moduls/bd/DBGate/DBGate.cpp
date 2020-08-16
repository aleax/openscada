
//OpenSCADA module BD.DBGate file: DBGate.cpp
/***************************************************************************
 *   Copyright (C) 2020 by Roman Savochenko, <roman@oscada.org>            *
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

#include <tsys.h>
#include <tmess.h>

#include "DBGate.h"

//************************************************
//* Modul info!                                  *
#define MOD_ID		"DBGate"
#define MOD_NAME	_("DB gate")
#define MOD_TYPE	SDB_ID
#define VER_TYPE	SDB_VER
#define MOD_VER		"1.0.0"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("BD module. Allows to locate databases of the remote OpenSCADA stations to local ones.")
#define MOD_LICENSE	"GPL2"
//************************************************

BD_DBGate::BDMod *BD_DBGate::mod;	//Pointer for direct access to the module

extern "C"
{
#ifdef MOD_INCL
    TModule::SAt bd_DBGate_module( int n_mod )
#else
    TModule::SAt module( int n_mod )
#endif
    {
	if(n_mod == 0)	return TModule::SAt(MOD_ID, MOD_TYPE, VER_TYPE);
	return TModule::SAt("");
    }
#ifdef MOD_INCL
    TModule *bd_DBGate_attach( const TModule::SAt &AtMod, const string &source )
#else
    TModule *attach( const TModule::SAt &AtMod, const string &source )
#endif
    {
	if(AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE)) return new BD_DBGate::BDMod(source);
	return NULL;
    }
}

using namespace BD_DBGate;

//************************************************
//* BD_DBGate::BDMod				 *
//************************************************
BDMod::BDMod( string name ) : TTypeBD(MOD_ID)
{
    mod = this;

    modInfoMainSet(MOD_NAME, MOD_TYPE, MOD_VER, AUTHORS, DESCRIPTION, MOD_LICENSE, name);
}

BDMod::~BDMod( )
{

}

TBD *BDMod::openBD( const string &name )	{ return new MBD(name, &owner().openDB_E()); }


//************************************************
//* BD_DBGate::MBD				 *
//************************************************
MBD::MBD( string iid, TElem *cf_el ) : TBD(iid, cf_el)
{

}

MBD::~MBD( )
{

}

void MBD::postDisable( int flag )
{
    TBD::postDisable(flag);

    /*if(flag && owner().fullDeleteDB()) {

    }*/
}

void MBD::enable( )
{
    if(enableStat())	return;

    XMLNode req("set");
    req.setAttr("path", "/%2fprm%2fst%2fst")->setText("1");
    if(!cntrIfCmd(req)) TBD::enable();
    else throw err_sys(_("Error enabling the remote DB '%s': %s"), addr().c_str(), req.text().c_str());
}

void MBD::disable( )
{
    TBD::disable();
}

void MBD::allowList( vector<string> &list ) const
{
    if(!enableStat())	return;
    list.clear();

    XMLNode req("get");
    req.setAttr("path", "/%2fprm%2fst%2fallow_tbls");
    const_cast<MBD*>(this)->cntrIfCmd(req);
    for(unsigned iL = 0; iL < req.childSize(); ++iL)
	list.push_back(req.childGet(iL)->text());
}

void MBD::open( const string &table, bool create )
{
    if(create) {
	XMLNode req("add"); req.setAttr("path", "/%2fbr%2ftbl_")->setText(table);
	cntrIfCmd(req);
    }

    TBD::open(table, create);
}

void MBD::close( const string &table, bool del, long tm )
{
    TBD::close(table, del, tm);

    if(del) {
	XMLNode req("del"); req.setAttr("path", "/%2fbr%2ftbl_")->setAttr("del","1")->setText(table);
	cntrIfCmd(req);
    }
}

TTable *MBD::openTable( const string &inm, bool create )
{
    if(!enableStat()) throw err_sys(_("Error open table '%s'. DB is disabled."), inm.c_str());

    return new MTable(inm, this);
}

int MBD::cntrIfCmd( XMLNode &node, bool ownNode )
{
    string reqStat = TSYS::strParse(addr(), 0, ".");
    if(!reqStat.size())	node.setAttr("err", i2s(TError::Tr_UnknownHost)+":"+TSYS::strMess(_("Station missed '%s'."),reqStat.c_str()));

    node.setAttr("path", "/"+reqStat+(ownNode?"/BD/"+TSYS::strParse(addr(),1,".")+"/"+TSYS::strParse(addr(),2,"."):"")+"/"+node.attr("path"));
	//setAttr("conTm", enableStat()?"":"1000");
    try { return SYS->transport().at().cntrIfCmd(node, MOD_ID+id()); }
    catch(TError &err) { disable(); throw; }
}

void MBD::sqlReq( const string &ireq, vector< vector<string> > *tbl, char intoTrans )
{
    if(tbl) tbl->clear();
    if(!enableStat())	return;

    XMLNode req("call");
    req.setAttr("path", "/%2fserv%2fSQL")->setAttr("intoTrans", i2s(intoTrans))->setAttr("withRez", i2s(tbl?1:0))->setText(ireq);
    cntrIfCmd(req);
    for(unsigned iC = 0; tbl && iC < req.childSize(); ++iC)
	for(unsigned iR = 0; iR < req.childGet(iC)->childSize(); ++iR) {
	    while(iC == 0 && iR >= tbl->size()) tbl->push_back(vector<string>());
	    (*tbl)[iR].push_back(req.childGet(iC)->childGet(iR)->text());
	}
}

void MBD::cntrCmdProc( XMLNode *opt )
{
    string a_path = opt->attr("path");

    //Get page info
    if(opt->name() == "info") {
	TBD::cntrCmdProc(opt);
	ctrRemoveNode(opt, "/prm/cfg/TRTM_CLS_ON_OPEN");
	ctrRemoveNode(opt, "/prm/cfg/TRTM_CLS_ON_REQ");
	ctrRemoveNode(opt, "/prm/cfg/CODEPAGE");
	if(enableStat() && a_path != "/br") cntrIfCmd(*opt);
	ctrMkNode("fld",opt,-1,"/prm/cfg/ADDR",EVAL_STR,enableStat()?R_R___:RWRW__,"root",SDB_ID,3,
	    "dest","select","select","/prm/cfg/dbRemList","help",
		_("DB gate address must be written-selected as: \"{RemoteStation}.{DBModule}.{DB}\".\n"
		  "Where:\n"
		  "  RemoteStation - remote OpenSCADA station, registered in the table of the OpenSCADA stations;\n"
		  "  DBModule - DB-module identifier of the remote station;\n"
		  "  DB       - database identifier of the remote station."));
	ctrMkNode2("comm",opt,4,"/prm/cfg/host_lnk",_("Go to configuration of the remote stations list"),enableStat()?0:RWRW__,"root",SDB_ID,
	    "tp","lnk", NULL);
	return;
    }

    //Process command to page
    if(a_path == "/prm/cfg/ID" || a_path == "/prm/cfg/ADDR" || a_path == "/prm/cfg/EN")
	TBD::cntrCmdProc(opt);
    else if(a_path == "/prm/cfg/NAME") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SDB_ID,SEC_RD)) {
	    if(enableStat() && !cfg("NAME").getS().size()) {
		opt->setAttr("path", "/"+TSYS::strEncode(opt->attr("path"),TSYS::PathEl));
		cntrIfCmd(*opt);
	    } else opt->setText(name());
	} else TBD::cntrCmdProc(opt);
    }
    else if(a_path == "/prm/cfg/DESCR") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SDB_ID,SEC_RD)) {
	    if(enableStat() && !dscr().size()) {
		opt->setAttr("path", "/"+TSYS::strEncode(opt->attr("path"),TSYS::PathEl));
		cntrIfCmd(*opt);
	    } else opt->setText(dscr());
	} else TBD::cntrCmdProc(opt);
    }
    else if(a_path == "/prm/cfg/dbRemList" && ctrChkNode(opt)) {
	int c_lv = 0;
	string c_path = "", c_el;
	vector<string> ls;
	opt->childAdd("el")->setText(c_path);
	for(int c_off = 0; (c_el=TSYS::strParse(addr(),0,".",&c_off)).size(); ++c_lv) {
	    c_path += (c_lv?".":"") + c_el;
	    opt->childAdd("el")->setText(c_path);
	}

	switch(c_lv) {
	    case 0: {
		vector<TTransportS::ExtHost> list;
		SYS->transport().at().extHostList("*", list);
		for(unsigned iL = 0; iL < list.size(); ++iL)
		    opt->childAdd("el")->setText(list[iL].id);
		break;
	    }
	    case 1: {
		XMLNode req("get");
		req.setAttr("path", "/BD/%2fbr%2fmod_");
		if(!cntrIfCmd(req,false))
		    for(unsigned iL = 0; iL < req.childSize(); ++iL)
			opt->childAdd("el")->setText(c_path+"."+req.childGet(iL)->attr("id"));
		break;
	    }
	    case 2: {
		XMLNode req("get");
		req.setAttr("path", "/BD/"+TSYS::strParse(addr(),1,".")+"/%2fbr%2fdb_");
		if(!cntrIfCmd(req,false))
		    for(unsigned iL = 0; iL < req.childSize(); ++iL)
			opt->childAdd("el")->setText(c_path+"."+req.childGet(iL)->attr("id"));
		break;
	    }
	}
    }
    else if(a_path == "/prm/cfg/host_lnk" && ctrChkNode(opt,"get",RWRW__,"root",SDB_ID,SEC_RD)) opt->setText("/Transport");
    else if(a_path.compare(0,9,"/prm/cfg/") == 0 && enableStat()) {
	opt->setAttr("path", "/"+TSYS::strEncode(opt->attr("path"),TSYS::PathEl));
	cntrIfCmd(*opt);
    }
    else TBD::cntrCmdProc(opt);
}

//************************************************
//* BD_DBGate::Table                                *
//************************************************
MTable::MTable( string name, MBD *iown ) : TTable(name)
{
    setNodePrev(iown);
}

MTable::~MTable( )
{

}

void MTable::postDisable( int flag )
{

}

MBD &MTable::owner( ) const	{ return (MBD&)TTable::owner(); }

void MTable::fieldStruct( TConfig &cfg )
{
    XMLNode req("call");
    req.setAttr("path", "/%2fserv%2ffieldStruct")->setAttr("tbl", name());
    owner().cntrIfCmd(req);
    SYS->db().at().dataSeek("", "", 0, cfg, false, NULL, &req);
}

bool MTable::fieldSeek( int row, TConfig &cfg, const string &cacheKey )
{
    cfg.cfgToDefault();	//reset the not key and viewed fields

    XMLNode req("call");
    req.setAttr("path", "/%2fserv%2ffieldSeek")->
	setAttr("tbl", name())->
	setAttr("row", i2s(row))->
	setAttr("cacheKey", cacheKey);
    SYS->db().at().dataSet("", "", cfg, false, false, &req);
    owner().cntrIfCmd(req);
    SYS->db().at().dataSeek("", "", 0, cfg, false, NULL, &req);

    return s2i(req.attr("fRez"));
}

void MTable::fieldGet( TConfig &cfg )
{
    XMLNode req("call");
    req.setAttr("path", "/%2fserv%2ffieldGet")->setAttr("tbl", name());
    SYS->db().at().dataSet("", "", cfg, false, false, &req);
    owner().cntrIfCmd(req);
    SYS->db().at().dataGet("", "", cfg, false, false, &req);
}

void MTable::fieldSet( TConfig &cfg )
{
    XMLNode req("call");
    req.setAttr("path", "/%2fserv%2ffieldSet")->setAttr("tbl", name());
    SYS->db().at().dataSet("", "", cfg, false, false, &req);
    owner().cntrIfCmd(req);
}

void MTable::fieldDel( TConfig &cfg )
{
    XMLNode req("call");
    req.setAttr("path", "/%2fserv%2ffieldDel")->setAttr("tbl", name());
    cfg.cfgViewAll(false);
    SYS->db().at().dataSet("", "", cfg, false, false, &req);
    owner().cntrIfCmd(req);
}
