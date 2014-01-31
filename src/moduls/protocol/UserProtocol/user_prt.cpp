
//OpenSCADA system module Protocol.UserProtocol file: user_prt.cpp
/***************************************************************************
 *   Copyright (C) 2010 by Roman Savochenko                                *
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

#include <string.h>

#include <tsys.h>
#include <tmess.h>
#include <tmodule.h>
#include <tuis.h>

#include "user_prt.h"

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"UserProtocol"
#define MOD_NAME	_("User protocol")
#define MOD_TYPE	SPRT_ID
#define VER_TYPE	SPRT_VER
#define MOD_VER		"0.6.2"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("Allow creation self-user protocols on any OpenSCADA language.")
#define LICENSE		"GPL2"
//*************************************************

UserProtocol::TProt *UserProtocol::mod;

extern "C"
{
#ifdef MOD_INCL
    TModule::SAt prt_UserProtocol_module( int n_mod )
#else
    TModule::SAt module( int n_mod )
#endif
    {
	if( n_mod==0 )	return TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE);
	return TModule::SAt("");
    }

#ifdef MOD_INCL
    TModule *prt_UserProtocol_attach( const TModule::SAt &AtMod, const string &source )
#else
    TModule *attach( const TModule::SAt &AtMod, const string &source )
#endif
    {
	if( AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE) )
	    return new UserProtocol::TProt( source );
	return NULL;
    }
}

using namespace UserProtocol;

//*************************************************
//* TProt                                         *
//*************************************************
TProt::TProt( string name ) : TProtocol(MOD_ID)
{
    mod		= this;

    mType	= MOD_TYPE;
    mName	= MOD_NAME;
    mVers	= MOD_VER;
    mAuthor	= AUTHORS;
    mDescr	= DESCRIPTION;
    mLicense	= LICENSE;
    mSource	= name;

    mPrtU = grpAdd("up_");

    //> User protocol DB structure
    mUPrtEl.fldAdd(new TFld("ID",_("ID"),TFld::String,TCfg::Key|TFld::NoWrite,OBJ_ID_SZ));
    mUPrtEl.fldAdd(new TFld("NAME",_("Name"),TFld::String,TCfg::TransltText,OBJ_NM_SZ));
    mUPrtEl.fldAdd(new TFld("DESCR",_("Description"),TFld::String,TFld::FullText|TCfg::TransltText,"300"));
    mUPrtEl.fldAdd(new TFld("EN",_("To enable"),TFld::Boolean,0,"1","0"));
    mUPrtEl.fldAdd(new TFld("InPROG",_("Input program"),TFld::String,TFld::FullText|TCfg::TransltText,"1000000"));
    mUPrtEl.fldAdd(new TFld("OutPROG",_("Output program"),TFld::String,TFld::FullText|TCfg::TransltText,"1000000"));
}

TProt::~TProt()
{
    nodeDelAll();
}

void TProt::itemListIn( vector<string> &ls, const string &curIt )
{
    ls.clear();
    if( TSYS::strParse(curIt,1,".").empty() )	uPrtList(ls);
}

void TProt::uPrtAdd( const string &iid, const string &db )
{
    chldAdd(mPrtU, new UserPrt(iid,db,&uPrtEl()));
}

void TProt::load_( )
{
    //> Load DB
    //>> Search and create new user protocols
    try
    {
	TConfig g_cfg(&uPrtEl());
	g_cfg.cfgViewAll(false);
	vector<string> db_ls;
	map<string, bool> itReg;

	//>>> Search into DB
	SYS->db().at().dbList(db_ls,true);
	db_ls.push_back(DB_CFG);
	for(unsigned i_db = 0; i_db < db_ls.size(); i_db++)
	    for(unsigned fld_cnt = 0; SYS->db().at().dataSeek(db_ls[i_db]+"."+modId()+"_uPrt",nodePath()+modId()+"_uPrt",fld_cnt++,g_cfg); )
	    {
		string id = g_cfg.cfg("ID").getS();
		if(!uPrtPresent(id)) uPrtAdd(id,(db_ls[i_db]==SYS->workDB())?"*.*":db_ls[i_db]);
		itReg[id] = true;
	    }

	//>>> Check for remove items removed from DB
        if(!SYS->selDB().empty())
        {
            uPrtList(db_ls);
            for(unsigned i_it = 0; i_it < db_ls.size(); i_it++)
                if(itReg.find(db_ls[i_it]) == itReg.end() && SYS->chkSelDB(uPrtAt(db_ls[i_it]).at().DB()))
                    uPrtDel(db_ls[i_it]);
        }
    }catch(TError err)
    {
	mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	mess_err(nodePath().c_str(),_("Search and create new user protocol error."));
    }
}

void TProt::save_( )
{

}

void TProt::modStart( )
{
    vector<string> ls;
    uPrtList(ls);
    for(unsigned i_n = 0; i_n < ls.size(); i_n++)
	if(uPrtAt(ls[i_n]).at().toEnable())
	    uPrtAt(ls[i_n]).at().setEnable(true);
}

void TProt::modStop( )
{
    vector<string> ls;
    uPrtList(ls);
    for(unsigned i_n = 0; i_n < ls.size(); i_n++)
	uPrtAt(ls[i_n]).at().setEnable(false);
}

TProtocolIn *TProt::in_open( const string &name )
{
    return new TProtIn(name);
}

void TProt::outMess( XMLNode &io, TTransportOut &tro )
{
    TValFunc funcV;

    //> Get user protocol for using
    string pIt = io.attr("ProtIt");
    if(!uPrtPresent(pIt)) return;
    AutoHD<UserPrt> up = uPrtAt(pIt);
    funcV.setFunc(&((AutoHD<TFunction>)SYS->nodeAt(up.at().workOutProg())).at());

    ResAlloc res( tro.nodeRes(), true );

    //> Load inputs
    AutoHD<XMLNodeObj> xnd(new XMLNodeObj());
    funcV.setO(0,xnd);
    xnd.at().fromXMLNode(io);
    funcV.setO(1,new TCntrNodeObj(AutoHD<TCntrNode>(&tro),"root"));
    //> Call processing
    funcV.calc( );
    //> Get outputs
    xnd.at().toXMLNode(io);

    up.at().cntOutReq++;
}

void TProt::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if( opt->name() == "info" )
    {
	TProtocol::cntrCmdProc(opt);
	ctrMkNode("grp",opt,-1,"/br/up_",_("User protocol"),RWRWR_,"root",SPRT_ID,2,"idm",OBJ_NM_SZ,"idSz",OBJ_ID_SZ);
	if(ctrMkNode("area",opt,0,"/up",_("User protocols")))
	    ctrMkNode("list",opt,-1,"/up/up",_("Protocols"),RWRWR_,"root",SPRT_ID,5,
		"tp","br","idm",OBJ_NM_SZ,"s_com","add,del","br_pref","up_","idSz",OBJ_ID_SZ);
	return;
    }

    //> Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/br/up_" || a_path == "/up/up")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD))
	{
	    vector<string> lst;
	    uPrtList(lst);
	    for(unsigned i_f=0; i_f < lst.size(); i_f++)
		opt->childAdd("el")->setAttr("id",lst[i_f])->setText(uPrtAt(lst[i_f]).at().name());
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SPRT_ID,SEC_WR))
	{
	    string vid = TSYS::strEncode(opt->attr("id"),TSYS::oscdID);
	    uPrtAdd(vid); uPrtAt(vid).at().setName(opt->text());
	}
	if(ctrChkNode(opt,"del",RWRWR_,"root",SPRT_ID,SEC_WR))	chldDel(mPrtU,opt->attr("id"),-1,1);
    }

    else TProtocol::cntrCmdProc(opt);
}

//*************************************************
//* TProtIn                                       *
//*************************************************
TProtIn::TProtIn( string name ) : TProtocolIn(name)
{

}

TProtIn::~TProtIn()
{

}

TProt &TProtIn::owner( )	{ return *(TProt*)nodePrev(); }

bool TProtIn::mess( const string &reqst, string &answer )
{
    try
    {
	//> Find user protocol for using
	if(!funcV.func())
	{
	    AutoHD<TTransportIn> tri = srcTr();
	    string selNode = TSYS::strParse(tri.at().protocolFull(),1,".");
	    if(!owner().uPrtPresent(selNode)) return false;
	    up = owner().uPrtAt(selNode);
	    if(!up.at().enableStat() || up.at().workInProg().empty()) return false;
	    funcV.setFunc(&((AutoHD<TFunction>)SYS->nodeAt(up.at().workInProg())).at());
	    funcV.setO(4,new TCntrNodeObj(AutoHD<TCntrNode>(&tri.at()),"root"));
	}

	//> Load inputs
	funcV.setB(0, false);
	funcV.setS(1, funcV.getS(1)+reqst);
	funcV.setS(2, "");
	funcV.setS(3, srcAddr());
	//> Call processing
	funcV.calc( );
	//> Get outputs
	bool rez = funcV.getB(0);
	if(!rez) funcV.setS(1,"");
	answer = funcV.getS(2);

	up.at().cntInReq++;

	return rez;
    }
    catch(TError err) { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }

    return false;
}

//*************************************************
//* UserPrt                                       *
//*************************************************
UserPrt::UserPrt( const string &iid, const string &idb, TElem *el ) :
    TConfig(el), cntInReq(0), cntOutReq(0), mId(cfg("ID")), mAEn(cfg("EN").getBd()), mEn(false), mDB(idb)
{
    mId = iid;
}

UserPrt::~UserPrt( )
{
    try { setEnable(false); } catch(...) { }
}

TCntrNode &UserPrt::operator=( TCntrNode &node )
{
    UserPrt *src_n = dynamic_cast<UserPrt*>(&node);
    if(!src_n) return *this;

    if( enableStat( ) )	setEnable(false);

    //> Copy parameters
    exclCopy(*src_n, "ID;");
    setDB(src_n->DB());

    return *this;
}

void UserPrt::postDisable( int flag )
{
    try
    {
	if( flag ) SYS->db().at().dataDel(fullDB(),owner().nodePath()+tbl(),*this,true);
    }catch(TError err)
    { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
}

TProt &UserPrt::owner( )	{ return *(TProt*)nodePrev(); }

string UserPrt::name( )
{
    string tNm = cfg("NAME").getS();
    return tNm.size() ? tNm : id();
}

string UserPrt::tbl( )		{ return owner().modId()+"_uPrt"; }

string UserPrt::inProgLang( )
{
    string mProg = cfg("InPROG").getS();
    return mProg.substr(0,mProg.find("\n"));
}

string UserPrt::inProg( )
{
    string mProg = cfg("InPROG").getS();
    size_t lngEnd = mProg.find("\n");
    return mProg.substr( (lngEnd==string::npos)?0:lngEnd+1 );
}

void UserPrt::setInProgLang( const string &ilng )
{
    cfg("InPROG").setS( ilng+"\n"+inProg() );
    if( enableStat() ) setEnable(false);
    modif();
}

void UserPrt::setInProg( const string &iprg )
{
    cfg("InPROG").setS( inProgLang()+"\n"+iprg );
    if( enableStat() ) setEnable(false);
    modif();
}

string UserPrt::outProgLang( )
{
    string mProg = cfg("OutPROG").getS();
    return mProg.substr(0,mProg.find("\n"));
}

string UserPrt::outProg( )
{
    string mProg = cfg("OutPROG").getS();
    size_t lngEnd = mProg.find("\n");
    return mProg.substr( (lngEnd==string::npos)?0:lngEnd+1 );
}

void UserPrt::setOutProgLang( const string &ilng )
{
    cfg("OutPROG").setS( ilng+"\n"+outProg() );
    if( enableStat() ) setEnable(false);
    modif();
}

void UserPrt::setOutProg( const string &iprg )
{
    cfg("OutPROG").setS( outProgLang()+"\n"+iprg );
    if( enableStat() ) setEnable(false);
    modif();
}

void UserPrt::load_( )
{
    if( !SYS->chkSelDB(DB()) ) return;
    cfgViewAll(true);
    SYS->db().at().dataGet(fullDB(),owner().nodePath()+tbl(),*this);
}

void UserPrt::save_( )
{
    SYS->db().at().dataSet(fullDB(),owner().nodePath()+tbl(),*this);
}

bool UserPrt::cfgChange( TCfg &cfg )
{
    modif();

    return true;
}

void UserPrt::setEnable( bool vl )
{
    if(mEn == vl) return;

    cntInReq = cntOutReq = 0;

    if(vl)
    {
	//> Prepare and compile input transport function
	if(!inProg().empty())
	{
	    TFunction funcIO("uprt_"+id()+"_in");
	    funcIO.ioIns(new IO("rez",_("Result"),IO::Boolean,IO::Return), 0);
	    funcIO.ioIns(new IO("request",_("Request"),IO::String,IO::Default), 1);
	    funcIO.ioIns(new IO("answer",_("Answer"),IO::String,IO::Output), 2);
	    funcIO.ioIns(new IO("sender",_("Sender"),IO::String,IO::Default), 3);
	    funcIO.ioIns(new IO("tr",_("Transport"),IO::Object,IO::Default), 4);

	    mWorkInProg = SYS->daq().at().at(TSYS::strSepParse(inProgLang(),0,'.')).at().
		compileFunc(TSYS::strSepParse(inProgLang(),1,'.'),funcIO,inProg());
	} else mWorkInProg = "";
	//> Prepare and compile input transport function
	if(!outProg().empty())
	{
	    TFunction funcIO("uprt_"+id()+"_out");
	    funcIO.ioIns(new IO("io",_("IO"),IO::Object,IO::Default), 0);
	    funcIO.ioIns(new IO("tr",_("Transport"),IO::Object,IO::Default), 1);

	    mWorkOutProg = SYS->daq().at().at(TSYS::strSepParse(outProgLang(),0,'.')).at().
		compileFunc(TSYS::strSepParse(outProgLang(),1,'.'),funcIO,outProg());
	} else mWorkOutProg = "";
    }

    mEn = vl;
}

string UserPrt::getStatus( )
{
    string rez = _("Disabled. ");
    if(enableStat())
    {
	rez = _("Enabled. ");
	rez += TSYS::strMess( _("Requests input %.4g, output %.4g."), cntInReq, cntOutReq );
    }

    return rez;
}

void UserPrt::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if(opt->name() == "info")
    {
	TCntrNode::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("User protocol: ")+name());
	if(ctrMkNode("area",opt,-1,"/up",_("User protocol")))
	{
	    if(ctrMkNode("area",opt,-1,"/up/st",_("State")))
	    {
		ctrMkNode("fld",opt,-1,"/up/st/status",_("Status"),R_R_R_,"root",SPRT_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/up/st/en_st",_("Enable"),RWRWR_,"root",SPRT_ID,1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/up/st/db",_("DB"),RWRWR_,"root",SPRT_ID,4,
		    "tp","str","dest","select","select","/db/list","help",TMess::labDB());
	    }
	    if(ctrMkNode("area",opt,-1,"/up/cfg",_("Configuration")))
	    {
		TConfig::cntrCmdMake(opt,"/up/cfg",0,"root",SPRT_ID,RWRWR_);
		ctrRemoveNode(opt,"/up/cfg/InPROG");
		ctrRemoveNode(opt,"/up/cfg/OutPROG");
	    }
	    if(ctrMkNode("area",opt,-1,"/in",_("Input"),RWRW__,"root",SPRT_ID))
	    {
		ctrMkNode("fld",opt,-1,"/in/PROGLang",_("Input program language"),RWRW__,"root",SPRT_ID,3,"tp","str","dest","sel_ed","select","/up/cfg/plangIls");
		ctrMkNode("fld",opt,-1,"/in/PROG",_("Input program"),RWRW__,"root",SPRT_ID,4,"tp","str","rows","10","SnthHgl","1",
		    "help",_("Next attributes has defined for input requests processing:\n"
			    "   'rez' - processing result (false-full request;true-not full request);\n"
			    "   'request' - request message;\n"
			    "   'answer' - answer message;\n"
			    "   'sender' - request sender;\n"
			    "   'tr' - sender transport."));
	    }
	    if(ctrMkNode("area",opt,-1,"/out",_("Output"),RWRW__,"root",SPRT_ID))
	    {
		ctrMkNode("fld",opt,-1,"/out/PROGLang",_("Output program language"),RWRW__,"root",SPRT_ID,3,"tp","str","dest","sel_ed","select","/up/cfg/plangOls");
		ctrMkNode("fld",opt,-1,"/out/PROG",_("Output program"),RWRW__,"root",SPRT_ID,4,"tp","str","rows","10","SnthHgl","1",
		    "help",_("Next attributes has defined for output requests processing:\n"
			    "   'io' - input/output interface's XMLNode object;\n"
			    "   'tr' - associated transport."));
	    }
	}
	return;
    }
    //> Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/up/st/status" && ctrChkNode(opt))	opt->setText(getStatus());
    else if(a_path == "/up/st/en_st")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD))	opt->setText(enableStat()?"1":"0");
	if(ctrChkNode(opt,"set",RWRWR_,"root",SPRT_ID,SEC_WR))	setEnable(atoi(opt->text().c_str()));
    }
    else if(a_path == "/up/st/db")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD))	opt->setText(DB());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SPRT_ID,SEC_WR))	setDB(opt->text());
    }
    else if((a_path == "/up/cfg/plangIls" || a_path == "/up/cfg/plangOls") && ctrChkNode(opt))
    {
	string tplng = (a_path=="/up/cfg/plangIls") ? inProgLang() : outProgLang();
	int c_lv = 0;
	string c_path = "", c_el;
	opt->childAdd("el")->setText(c_path);
	for(int c_off = 0; (c_el=TSYS::strSepParse(tplng,0,'.',&c_off)).size(); c_lv++)
	{
	    c_path += c_lv ? "."+c_el : c_el;
	    opt->childAdd("el")->setText(c_path);
	}
	if(c_lv) c_path+=".";
	vector<string>  ls;
	switch(c_lv)
	{
	    case 0:
		SYS->daq().at().modList(ls);
		for(unsigned i_l = 0; i_l < ls.size(); )
		    if(!SYS->daq().at().at(ls[i_l]).at().compileFuncLangs()) ls.erase(ls.begin()+i_l);
		    else i_l++;
		break;
	    case 1:
		if(SYS->daq().at().modPresent(TSYS::strSepParse(tplng,0,'.')))
		    SYS->daq().at().at(TSYS::strSepParse(tplng,0,'.')).at().compileFuncLangs(&ls);
		break;
	}
	for(unsigned i_l = 0; i_l < ls.size(); i_l++)
	    opt->childAdd("el")->setText(c_path+ls[i_l]);
    }
    else if(a_path.substr(0,7) == "/up/cfg") TConfig::cntrCmdProc(opt,TSYS::pathLev(a_path,2),"root",SPRT_ID,RWRWR_);
    else if(a_path == "/in/PROGLang")
    {
	if(ctrChkNode(opt,"get",RWRW__,"root",SPRT_ID,SEC_RD))	opt->setText(inProgLang());
	if(ctrChkNode(opt,"set",RWRW__,"root",SPRT_ID,SEC_WR))	setInProgLang(opt->text());
    }
    else if(a_path == "/in/PROG")
    {
	if(ctrChkNode(opt,"get",RWRW__,"root",SPRT_ID,SEC_RD))	opt->setText(inProg());
	if(ctrChkNode(opt,"set",RWRW__,"root",SPRT_ID,SEC_WR))	setInProg(opt->text());
	if(ctrChkNode(opt,"SnthHgl",RWRW__,"root",SPRT_ID,SEC_RD))
            try
            {
                SYS->daq().at().at(TSYS::strParse(inProgLang(),0,".")).at().
                                compileFuncSynthHighl(TSYS::strParse(inProgLang(),1,"."),*opt);
            } catch(...){ }
    }
    else if(a_path == "/out/PROGLang")
    {
	if(ctrChkNode(opt,"get",RWRW__,"root",SPRT_ID,SEC_RD))	opt->setText(outProgLang());
	if(ctrChkNode(opt,"set",RWRW__,"root",SPRT_ID,SEC_WR))	setOutProgLang(opt->text());
    }
    else if(a_path == "/out/PROG")
    {
	if(ctrChkNode(opt,"get",RWRW__,"root",SPRT_ID,SEC_RD))	opt->setText(outProg());
	if(ctrChkNode(opt,"set",RWRW__,"root",SPRT_ID,SEC_WR))	setOutProg(opt->text());
	if(ctrChkNode(opt,"SnthHgl",RWRW__,"root",SPRT_ID,SEC_RD))
            try
            {
                SYS->daq().at().at(TSYS::strParse(outProgLang(),0,".")).at().
                                compileFuncSynthHighl(TSYS::strParse(outProgLang(),1,"."),*opt);
            } catch(...){ }
    }
    else TCntrNode::cntrCmdProc(opt);
}
