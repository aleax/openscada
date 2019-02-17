
//OpenSCADA module Protocol.UserProtocol file: user_prt.cpp
/***************************************************************************
 *   Copyright (C) 2010-2019 by Roman Savochenko, <rom_as@oscada.org>      *
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
#define MOD_VER		"1.1.5"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("Allows you to create your own user protocols on an internal OpenSCADA language.")
#define LICENSE		"GPL2"
//*************************************************

UserProtocol::TProt *UserProtocol::mod;

extern "C"
{
#ifdef MOD_INCL
    TModule::SAt prot_UserProtocol_module( int n_mod )
#else
    TModule::SAt module( int n_mod )
#endif
    {
	if(n_mod == 0)	return TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE);
	return TModule::SAt("");
    }

#ifdef MOD_INCL
    TModule *prot_UserProtocol_attach( const TModule::SAt &AtMod, const string &source )
#else
    TModule *attach( const TModule::SAt &AtMod, const string &source )
#endif
    {
	if(AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE)) return new UserProtocol::TProt(source);
	return NULL;
    }
}

using namespace UserProtocol;

//*************************************************
//* TProt                                         *
//*************************************************
TProt::TProt( string name ) : TProtocol(MOD_ID)
{
    mod = this;

    modInfoMainSet(MOD_NAME, MOD_TYPE, MOD_VER, AUTHORS, DESCRIPTION, LICENSE, name);

    mPrtU = grpAdd("up_");

    // User protocol DB structure
    mUPrtEl.fldAdd(new TFld("ID",_("Identifier"),TFld::String,TCfg::Key|TFld::NoWrite,OBJ_ID_SZ));
    mUPrtEl.fldAdd(new TFld("NAME",_("Name"),TFld::String,TFld::TransltText,OBJ_NM_SZ));
    mUPrtEl.fldAdd(new TFld("DESCR",_("Description"),TFld::String,TFld::FullText|TFld::TransltText,"300"));
    mUPrtEl.fldAdd(new TFld("EN",_("To enable"),TFld::Boolean,0,"1","0"));
    mUPrtEl.fldAdd(new TFld("DAQTmpl",_("Representative DAQ template"),TFld::String,TFld::NoFlag,"50"));
    mUPrtEl.fldAdd(new TFld("WaitReqTm",_("Timeout of a request waiting, milliseconds"),TFld::Integer,TFld::NoFlag,"6","0"));
    mUPrtEl.fldAdd(new TFld("InPROG",_("Input procedure"),TFld::String,TFld::FullText|TFld::TransltText,"1000000"));
    mUPrtEl.fldAdd(new TFld("OutPROG",_("Output procedure"),TFld::String,TFld::FullText|TFld::TransltText,"1000000"));
    mUPrtEl.fldAdd(new TFld("PR_TR",_("Completely translate the procedure"),TFld::Boolean,TFld::NoFlag,"1","0"));
    mUPrtEl.fldAdd(new TFld("TIMESTAMP",_("Date of modification"),TFld::Integer,TFld::DateTimeDec));

    //User protocol data IO DB structure
    mUPrtIOEl.fldAdd(new TFld("UPRT_ID",_("User protocol ID"),TFld::String,TCfg::Key,OBJ_ID_SZ));
    mUPrtIOEl.fldAdd(new TFld("ID",_("Identifier"),TFld::String,TCfg::Key,OBJ_ID_SZ));
    mUPrtIOEl.fldAdd(new TFld("VALUE",_("Value"),TFld::String,TFld::TransltText,"100"));
}

TProt::~TProt( )
{
    nodeDelAll();
}

void TProt::itemListIn( vector<string> &ls, const string &curIt )
{
    ls.clear();
    if(TSYS::strParse(curIt,1,".").empty())	uPrtList(ls);
}

string TProt::uPrtAdd( const string &iid, const string &db )
{
    return chldAdd(mPrtU, new UserPrt(TSYS::strEncode(sTrm(iid),TSYS::oscdID),db,&uPrtEl()));
}

void TProt::load_( )
{
    //Load DB
    // Search and create new user protocols
    try {
	TConfig gCfg(&uPrtEl());
	//gCfg.cfgViewAll(false);
	vector<string> dbLs;
	map<string, bool> itReg;
	vector<vector<string> > full;

	//  Search into DB
	SYS->db().at().dbList(dbLs, true);
	dbLs.push_back(DB_CFG);
	for(unsigned iDB = 0; iDB < dbLs.size(); iDB++)
	    for(unsigned fldCnt = 0; SYS->db().at().dataSeek(dbLs[iDB]+"."+modId()+"_uPrt",nodePath()+modId()+"_uPrt",fldCnt++,gCfg,false,&full); ) {
		string id = gCfg.cfg("ID").getS();
		if(!uPrtPresent(id)) uPrtAdd(id,(dbLs[iDB]==SYS->workDB())?"*.*":dbLs[iDB]);
		uPrtAt(id).at().load(&gCfg);
		gCfg.cfg("DAQTmpl").setS("");	//!!!! To prevent the new field from duplicating on different not updated tables.
		itReg[id] = true;
	    }

	//  Check for remove items removed from DB
	if(!SYS->selDB().empty()) {
	    uPrtList(dbLs);
	    for(unsigned iIt = 0; iIt < dbLs.size(); iIt++)
		if(itReg.find(dbLs[iIt]) == itReg.end() && SYS->chkSelDB(uPrtAt(dbLs[iIt]).at().DB()))
		    uPrtDel(dbLs[iIt]);
	}
    } catch(TError &err) {
	mess_err(err.cat.c_str(), "%s", err.mess.c_str());
	mess_err(nodePath().c_str(), _("Error searching and creating a new user protocol."));
    }
}

void TProt::save_( )
{

}

void TProt::modStart( )
{
    vector<string> ls;
    uPrtList(ls);
    for(unsigned iN = 0; iN < ls.size(); iN++)
	if(uPrtAt(ls[iN]).at().toEnable())
	    try { uPrtAt(ls[iN]).at().setEnable(true); }
	    catch(TError &err) {
		mess_err(err.cat.c_str(), "%s", err.mess.c_str());
		mess_sys(TMess::Error, _("Error starting the protocol '%s'."), ls[iN].c_str());
	    }
}

void TProt::modStop( )
{
    vector<string> ls;
    uPrtList(ls);
    for(unsigned iN = 0; iN < ls.size(); iN++)
	uPrtAt(ls[iN]).at().setEnable(false);
}

TProtocolIn *TProt::in_open( const string &name )	{ return new TProtIn(name); }

void TProt::outMess( XMLNode &io, TTransportOut &tro )
{
    string pIt = io.attr("ProtIt");
    if(uPrtPresent(pIt)) uPrtAt(pIt).at().outMess(io, tro);
}

void TProt::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TProtocol::cntrCmdProc(opt);
	ctrMkNode("grp",opt,-1,"/br/up_",_("User protocol"),RWRWR_,"root",SPRT_ID,2,"idm",OBJ_NM_SZ,"idSz",OBJ_ID_SZ);
	if(ctrMkNode("area",opt,0,"/up",_("User protocols")))
	    ctrMkNode("list",opt,-1,"/up/up",_("Protocols"),RWRWR_,"root",SPRT_ID,5,
		"tp","br","idm",OBJ_NM_SZ,"s_com","add,del","br_pref","up_","idSz",OBJ_ID_SZ);
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/br/up_" || a_path == "/up/up") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD)) {
	    vector<string> lst;
	    uPrtList(lst);
	    for(unsigned i_f=0; i_f < lst.size(); i_f++)
		opt->childAdd("el")->setAttr("id",lst[i_f])->setText(uPrtAt(lst[i_f]).at().name());
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SPRT_ID,SEC_WR))	{ opt->setAttr("id", uPrtAdd(opt->attr("id"))); uPrtAt(opt->attr("id")).at().setName(opt->text()); }
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

TProtIn::~TProtIn( )
{

}

TProt &TProtIn::owner( ) const	{ return *(TProt*)nodePrev(); }

unsigned TProtIn::waitReqTm( )	{ return !up.freeStat() ? up.at().waitReqTm() : 0; }

void TProtIn::setSrcTr( TTransportIn *vl )
{
    TProtocolIn::setSrcTr(vl);
    string selNode = TSYS::strParse(name(), 1, "#");
    if(owner().uPrtPresent(selNode)) up = owner().uPrtAt(selNode);
}

bool TProtIn::mess( const string &reqst, string &answer )
{
    if(!up.freeStat())	return up.at().inMess(reqst, answer, this);

    return false;
}

//*************************************************
//* UserPrt                                       *
//*************************************************
UserPrt::UserPrt( const string &iid, const string &idb, TElem *el ) :
    TConfig(el), TPrmTempl::Impl(this,("InUserProtocol_"+iid).c_str()), cntInReq(0), cntOutReq(0), mId(cfg("ID")), mAEn(cfg("EN").getBd()), mEn(false),
    mWaitReqTm(cfg("WaitReqTm").getId()), mTimeStamp(cfg("TIMESTAMP").getId()), mDB(idb),
    ioTrIn(-1), ioTrOut(-1), ioRez(-1), ioReq(-1), ioAnsw(-1), ioSend(-1), ioIO(-1), chkLnkNeed(false)
{
    mId = iid;
    cfg("InPROG").setExtVal(true);
    cfg("OutPROG").setExtVal(true);
}

UserPrt::~UserPrt( )
{
    try { setEnable(false); } catch(...) { }
}

TCntrNode &UserPrt::operator=( const TCntrNode &node )
{
    UserPrt *src_n = const_cast<UserPrt*>(dynamic_cast<const UserPrt*>(&node));
    if(!src_n) return *this;

    if(enableStat())	setEnable(false);

    //Copy parameters
    exclCopy(*src_n, "ID;");
    setDB(src_n->DB());

    //Copy for current values and links (by the templates)
    if(src_n->DAQTmpl().size() && src_n->enableStat()) {
	setEnable(true);

	ResAlloc res(inCfgRes, false);
	ResAlloc res1(src_n->inCfgRes, false);
	for(int iIO = 0; iIO < src_n->func()->ioSize(); iIO++)
	    if(src_n->func()->io(iIO)->flg()&TPrmTempl::CfgLink)
		lnkAddrSet(iIO, src_n->lnkAddr(iIO));
	    else set(iIO, src_n->get(iIO));

	chkLnkNeed = initLnks();
    }

    return *this;
}

void UserPrt::postDisable( int flag )
{
    if(flag) SYS->db().at().dataDel(fullDB(), owner().nodePath()+tbl(), *this, true);
}

TProt &UserPrt::owner( ) const	{ return *(TProt*)nodePrev(); }

string UserPrt::name( )
{
    string tNm = cfg("NAME").getS();
    return tNm.size() ? tNm : id();
}

string UserPrt::tbl( ) const	{ return owner().modId() + "_uPrt"; }

string UserPrt::inProgLang( )
{
    string mProg = cfg("InPROG").getS();
    return mProg.substr(0, mProg.find("\n"));
}

string UserPrt::inProg( )
{
    string mProg = cfg("InPROG").getS();
    size_t lngEnd = mProg.find("\n");
    return mProg.substr((lngEnd==string::npos)?0:lngEnd+1);
}

void UserPrt::setInProgLang( const string &ilng )
{
    cfg("InPROG").setS(ilng+"\n"+inProg());
    //if(enableStat()) setEnable(false);
    modif();
}

void UserPrt::setInProg( const string &iprg )
{
    cfg("InPROG").setS(inProgLang()+"\n"+iprg);
    //if(enableStat()) setEnable(false);
    modif();
}

string UserPrt::outProgLang( )
{
    string mProg = cfg("OutPROG").getS();
    return mProg.substr(0, mProg.find("\n"));
}

string UserPrt::outProg( )
{
    string mProg = cfg("OutPROG").getS();
    size_t lngEnd = mProg.find("\n");
    return mProg.substr((lngEnd==string::npos)?0:lngEnd+1);
}

void UserPrt::setOutProgLang( const string &ilng )
{
    cfg("OutPROG").setS(ilng+"\n"+outProg());
    //if(enableStat()) setEnable(false);
    modif();
}

void UserPrt::setOutProg( const string &iprg )
{
    cfg("OutPROG").setS(outProgLang()+"\n"+iprg);
    //if(enableStat()) setEnable(false);
    modif();
}

bool UserPrt::inMess( const string &reqst, string &answer, TProtIn *prt )
{
    if(ioRez < 0 || ioReq < 0 || ioAnsw < 0) return true;

    try {
	//Try enable, mostly for allow to use static functons into the procedures
	if(!enableStat() && toEnable() && inProgLang().size()) setEnable(true);

	MtxAlloc res1(inReqRes, true);
	ResAlloc res2(inCfgRes, false);

	//Malfunction input protocol checking
	if(!enableStat() || !func()) return false;

	if(chkLnkNeed) chkLnkNeed = initLnks(true);

	//The input function's execution context creation
	if(ioTrIn >= 0) setO(ioTrIn, new TCntrNodeObj(AutoHD<TCntrNode>(&prt->srcTr().at()),"root"));

	//Load inputs
	inputLinks();
	setB(ioRez, false);
	setS(ioReq, prt->req+reqst);
	setS(ioAnsw, "");
	if(ioSend >= 0) setS(ioSend, prt->srcAddr());
	//Call processing
	setMdfChk(true);
	calc();

	//Get outputs
	if(ioTrIn >= 0) setO(ioTrIn, new TEValObj());
	outputLinks();
	bool rez = getB(ioRez);

	prt->req = getS(ioReq);
	if(prt->req.size() > USER_FILE_LIMIT) {
	    mess_sys(TMess::Warning, _("Size of the accumulated request exceeded for %s, but the user protocol must tend for removing processed data itself. Fix this!"),
		TSYS::cpct2str(USER_FILE_LIMIT).c_str());
	    prt->req = "";
	}
	answer = getS(ioAnsw);

	cntInReq++;

	return rez;
    } catch(TError &err) {
	MtxAlloc res1(inReqRes, true);
	ResAlloc res2(inCfgRes, false);
	if(func() && ioTrIn >= 0) setO(ioTrIn, new TEValObj());
	mess_err(err.cat.c_str(), "%s", err.mess.c_str());
    }

    return false;
}

void UserPrt::outMess( XMLNode &io, TTransportOut &tro )
{
    if(ioTrOut < 0 || ioIO < 0) return;

    TValFunc funcV;

    //Get user protocol for using
    if(DAQTmpl().size())
	funcV.setFunc(&SYS->daq().at().tmplLibAt(TSYS::strParse(DAQTmpl(),0,".")).at().at(TSYS::strParse(DAQTmpl(),1,".")).at().func().at());
    else funcV.setFunc(&((AutoHD<TFunction>)SYS->nodeAt(workOutProg())).at());

    // Restoring the function running for stopping early by the safety timeout
    if(funcV.func() && !funcV.func()->startStat()) funcV.func()->setStart(true);

    MtxAlloc res(tro.reqRes(), true);

    //Load inputs
    AutoHD<XMLNodeObj> xnd(new XMLNodeObj());
    funcV.setO(ioIO, xnd);
    xnd.at().fromXMLNode(io);
    funcV.setO(ioTrOut, new TCntrNodeObj(AutoHD<TCntrNode>(&tro),"root"));
    //Call processing
    funcV.calc();
    //Get outputs
    xnd.at().toXMLNode(io);

    cntOutReq++;
}

void UserPrt::load_( TConfig *icfg )
{
    if(!SYS->chkSelDB(DB())) throw TError();

    if(icfg) *(TConfig*)this = *icfg;
    else {
	//cfgViewAll(true);
	SYS->db().at().dataGet(fullDB(),owner().nodePath()+tbl(),*this);
    }

    loadIO();
}

void UserPrt::loadIO( )
{
    ResAlloc res(inCfgRes, false);
    if(func() && DAQTmpl().size()) {
	//Load IO
	vector<vector<string> > full;
	vector<string> u_pos;
	TConfig cf(&owner().uPrtIOEl());
	cf.cfg("UPRT_ID").setS(id(), TCfg::ForceUse);
	cf.cfg("VALUE").setExtVal(true);
	for(int ioCnt = 0; SYS->db().at().dataSeek(fullDB()+"_io",owner().nodePath()+tbl()+"_io",ioCnt++,cf,false,&full); ) {
	    string sid = cf.cfg("ID").getS();
	    int iid = func()->ioId(sid);
	    if(iid < 0)	continue;

	    if(func()->io(iid)->flg()&TPrmTempl::CfgLink) lnkAddrSet(iid, cf.cfg("VALUE").getS());
	    else setS(iid, cf.cfg("VALUE").getS());
	}
	chkLnkNeed = initLnks();
    }
}

void UserPrt::save_( )
{
    mTimeStamp = SYS->sysTm();
    SYS->db().at().dataSet(fullDB(),owner().nodePath()+tbl(),*this);

    saveIO();
}

void UserPrt::saveIO( )
{
    ResAlloc res(inCfgRes, false);
    if(func() && DAQTmpl().size()) {
	//Save IO
	TConfig cf(&owner().uPrtIOEl());
	cf.cfg("UPRT_ID").setS(id(), true);
	for(int iIO = 0; iIO < func()->ioSize(); iIO++) {
	    if(iIO == ioRez || iIO == ioReq || iIO == ioAnsw || iIO == ioSend || iIO == ioTrIn || iIO == ioTrOut || iIO == ioIO ||
		func()->io(iIO)->flg()&TPrmTempl::LockAttr) continue;
	    cf.cfg("ID").setS(func()->io(iIO)->id());
	    cf.cfg("VALUE").setNoTransl(func()->io(iIO)->type() != IO::String || (func()->io(iIO)->flg()&TPrmTempl::CfgLink));
	    if(func()->io(iIO)->flg()&TPrmTempl::CfgLink) cf.cfg("VALUE").setS(lnkAddr(iIO));  //f->io(iIO)->rez());
	    else cf.cfg("VALUE").setS(getS(iIO));
	    SYS->db().at().dataSet(fullDB()+"_io",owner().nodePath()+tbl()+"_io",cf);
	}

	//Clear IO
	vector<vector<string> > full;
	cf.cfgViewAll(false);
	for(int fldCnt = 0; SYS->db().at().dataSeek(fullDB()+"_io",owner().nodePath()+tbl()+"_io",fldCnt++,cf,false,&full); ) {
	    string sio = cf.cfg("ID").getS();
	    if(func()->ioId(sio) < 0) {
		if(!SYS->db().at().dataDel(fullDB()+"_io",owner().nodePath()+tbl()+"_io",cf,true,false,true)) break;
		if(full.empty()) fldCnt--;
	    }
	}
    }
}

bool UserPrt::cfgChange( TCfg &co, const TVariant &pc )
{
    if(co.name() == "PR_TR") {
	cfg("InPROG").setNoTransl(!progTr());
	cfg("OutPROG").setNoTransl(!progTr());
    }
    /*else if(co.name() == "InPROG") {
	string  lfnc = TSYS::strParse(inProgLang(), 0, "."), wfnc = TSYS::strParse(inProgLang(), 1, ".");
	isDAQTmpl = SYS->daq().at().tmplLibPresent(lfnc) && SYS->daq().at().tmplLibAt(lfnc).at().present(wfnc);
    }*/
    //else if((co.name() == "InPROG" || co.name() == "OutPROG") && enableStat())	prgChOnEn = true;
    modif();
    return true;
}

void UserPrt::setEnable( bool vl )
{
    if(mEn == vl) return;

    cntInReq = cntOutReq = 0;

    ResAlloc res(inCfgRes, true);

    if(vl) {
	//Connect to a DAQ template or prepare and compile a function of the input part
	//string  lfnc = TSYS::strParse(inProgLang(), 0, "."), wfnc = TSYS::strParse(inProgLang(), 1, ".");
	//isDAQTmpl = SYS->daq().at().tmplLibPresent(lfnc) && SYS->daq().at().tmplLibAt(lfnc).at().present(wfnc);

	// Trying the DAQ template
	if(DAQTmpl().size()) {
	    setFunc(&SYS->daq().at().tmplLibAt(TSYS::strParse(DAQTmpl(),0,".")).at().at(TSYS::strParse(DAQTmpl(),1,".")).at().func().at());
	    addLinksAttrs();
	    // Checking for requiered conditions to the template
	    try {
		//Generic
		if((ioTrIn=ioTrOut=func()->ioId("tr")) >= 0 && func()->io(ioTrIn)->type() != IO::Object)ioTrIn = ioTrOut = -1;
		//Input part
		if((ioRez=func()->ioId("rez")) >= 0 && func()->io(ioRez)->type() != IO::Boolean)	ioRez = -1;
		if((ioReq=func()->ioId("request")) >= 0 && func()->io(ioReq)->type() != IO::String)	ioReq = -1;
		if((ioAnsw=func()->ioId("answer")) >= 0 && func()->io(ioAnsw)->type() != IO::String)	ioAnsw = -1;
		if((ioSend=func()->ioId("sender")) >= 0 && func()->io(ioSend)->type() != IO::String)	ioSend = -1;
		//Output part
		if((ioIO=func()->ioId("io")) >= 0 && func()->io(ioIO)->type() != IO::Object)		ioIO = -1;

		if(!((ioTrIn >= 0 && ioIO >= 0) || (ioRez >= 0 && ioReq >= 0 && ioAnsw >= 0)))
		    throw err_sys(_("The template '%s' does not have one or more required attribute in the needed type.\n"
			"Input part: rez=%d, request=%d, answer=%d. Output part: tr=%d, io=%d.\n"
			"See to the documentation and append their!"),
			inProgLang().c_str(), ioRez, ioReq, ioAnsw, ioTrIn, ioIO);
	    } catch(TError &err) { setFunc(NULL); throw; }
	}
	// Compiling the direct function
	else {
	    //Prepare and compile an input transport function
	    if(inProg().size()) {
		TFunction funcIO("uprt_"+id()+"_in");
		funcIO.setStor(DB());
		ioRez  = funcIO.ioAdd(new IO("rez",_("Input result"),IO::Boolean,IO::Return));
		ioReq  = funcIO.ioAdd(new IO("request",_("Input request"),IO::String,IO::Default));
		ioAnsw = funcIO.ioAdd(new IO("answer",_("Input answer"),IO::String,IO::Output));
		ioSend = funcIO.ioAdd(new IO("sender",_("Input sender"),IO::String,IO::Default));
		ioTrIn = funcIO.ioAdd(new IO("tr",_("Transport"),IO::Object,IO::Default));

		string workInProg = SYS->daq().at().at(TSYS::strSepParse(inProgLang(),0,'.')).at().
		    compileFunc(TSYS::strSepParse(inProgLang(),1,'.'),funcIO,inProg());
		setFunc(&((AutoHD<TFunction>)SYS->nodeAt(workInProg)).at());
	    }

	    //Prepare and compile an output transport function
	    if(outProg().size()) {
		TFunction funcIO("uprt_"+id()+"_out");
		funcIO.setStor(DB());
		ioIO = funcIO.ioAdd(new IO("io",_("Output IO"),IO::Object,IO::Default));
		ioTrOut = funcIO.ioAdd(new IO("tr",_("Transport"),IO::Object,IO::Default));

		mWorkOutProg = SYS->daq().at().at(TSYS::strSepParse(outProgLang(),0,'.')).at().
		    compileFunc(TSYS::strSepParse(outProgLang(),1,'.'),funcIO,outProg());
	    } else mWorkOutProg = "";
	}

	//Load IO
	loadIO();
    } else cleanLnks(true);

    mEn = vl;
}

string UserPrt::getStatus( )
{
    string rez = _("Disabled. ");
    if(enableStat()) {
	rez = _("Enabled. ");
	rez += TSYS::strMess(_("Requests input %.4g, output %.4g."), cntInReq, cntOutReq);
    }

    return rez;
}

void UserPrt::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TCntrNode::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("User protocol: ")+name());
	if(ctrMkNode("area",opt,-1,"/up",_("User protocol"))) {
	    if(ctrMkNode("area",opt,-1,"/up/st",_("State"))) {
		ctrMkNode("fld",opt,-1,"/up/st/status",_("Status"),R_R_R_,"root",SPRT_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/up/st/en_st",_("Enabled"),RWRWR_,"root",SPRT_ID,1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/up/st/db",_("DB"),RWRWR_,"root",SPRT_ID,4,
		    "tp","str","dest","select","select","/db/list","help",TMess::labDB());
		ctrMkNode("fld",opt,-1,"/up/st/timestamp",_("Date of modification"),R_R_R_,"root",SPRT_ID,1,"tp","time");
	    }
	    if(ctrMkNode("area",opt,-1,"/up/cfg",_("Configuration"))) {
		TConfig::cntrCmdMake(opt,"/up/cfg",0,"root",SPRT_ID,RWRWR_);
		ctrRemoveNode(opt,"/up/cfg/InPROG");
		ctrRemoveNode(opt,"/up/cfg/OutPROG");
		ctrRemoveNode(opt,"/up/cfg/TIMESTAMP");
		ctrRemoveNode(opt,"/up/cfg/WaitReqTm");
		ctrMkNode("fld",opt,-1,"/up/cfg/DAQTmpl",_("DAQ template"),(enableStat()?R_R___:RWRW__),"root",SPRT_ID,3,
		    "tp","str", "dest","select", "select","/up/cfg/listTmpl");
		if(DAQTmpl().size())	ctrRemoveNode(opt,"/up/cfg/PR_TR");
		else {
		    ctrMkNode("fld",opt,-1,"/up/cfg/inPROGLang",_("Input procedure language"),(enableStat()?R_R___:RWRW__),"root",SPRT_ID,3,
			"tp","str", "dest","select", "select","/plang/list");
		    ctrMkNode("fld",opt,-1,"/up/cfg/outPROGLang",_("Output procedure language"),(enableStat()?R_R___:RWRW__),"root",SPRT_ID,3,
			"tp","str", "dest","select", "select","/plang/list");
		}
	    }
	    if(ctrMkNode("area",opt,-1,"/in",_("Input"),(DAQTmpl().size()||inProgLang().size()?RWRW__:0),"root",SPRT_ID)) {
		ctrMkNode("fld",opt,-1,"/in/WaitReqTm",_("Timeout of a request waiting, milliseconds"),RWRW__,"root",SPRT_ID,2,
		    "tp","dec", "help",_("Use this for the poolling mode enabling through setting this timeout to a nonzero value.\n"
					"Into the poolling mode an input transport will call this protocol with the empty message at no request during this timeout."));
		ResAlloc res(inCfgRes, false);
		if(func() && chkLnkNeed) chkLnkNeed = initLnks(true);
		if(func() && ctrMkNode("table",opt,-1,"/in/io",_("IO"),RWRW__,"root",SPRT_ID,1,"rows","10")) {
		    ctrMkNode("list",opt,-1,"/in/io/id",_("Identifier"),R_R___,"root",SPRT_ID,1, "tp","str");
		    ctrMkNode("list",opt,-1,"/in/io/nm",_("Name"),R_R___,"root",SPRT_ID,1,"tp","str");
		    ctrMkNode("list",opt,-1,"/in/io/tp",_("Type"),R_R___,"root",SPRT_ID,5,"tp","dec","idm","1","dest","select",
			"sel_id",TSYS::strMess("%d;%d;%d;%d;%d",IO::Real,IO::Integer,IO::Boolean,IO::String,IO::Object).c_str(),
			"sel_list",_("Real;Integer;Boolean;String;Object"));
		    ctrMkNode("list",opt,-1,"/in/io/vl",_("Value"),RWRW__,"root",SPRT_ID,1,"tp","str");
		}
		if(!DAQTmpl().size())
		    ctrMkNode("fld",opt,-1,"/in/PROG",_("Input procedure"),(enableStat()?R_R___:RWRW__),"root",SPRT_ID,4, "tp","str", "rows","10", "SnthHgl","1",
			"help",_("Next attributes define for the input requests processing:\n"
				"   'rez' - result of the processing (false - full request; true - not full request);\n"
				"   'request' - request message;\n"
				"   'answer' - answer message;\n"
				"   'sender' - request sender;\n"
				"   'tr' - sender transport."));
		else if(func()) TPrmTempl::Impl::cntrCmdProc(opt, "/in/cfg");
	    }
	    if(ctrMkNode("area",opt,-1,"/out",_("Output"),(outProgLang().size()?RWRW__:0),"root",SPRT_ID)) {
		ctrMkNode("fld",opt,-1,"/out/PROG",_("Output procedure"),(enableStat()?R_R___:RWRW__),"root",SPRT_ID,4, "tp","str", "rows","10", "SnthHgl","1",
		    "help",_("Next attributes define for the output requests processing:\n"
			    "   'io' - XMLNode object of the input/output interface;\n"
			    "   'tr' - associated transport."));
	    }
	}
	return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/up/st/status" && ctrChkNode(opt))	opt->setText(getStatus());
    else if(a_path == "/up/st/en_st") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD))	opt->setText(enableStat()?"1":"0");
	if(ctrChkNode(opt,"set",RWRWR_,"root",SPRT_ID,SEC_WR))	setEnable(s2i(opt->text()));
    }
    else if(a_path == "/up/st/db") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD))	opt->setText(DB());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SPRT_ID,SEC_WR))	setDB(opt->text());
    }
    else if(a_path == "/up/st/timestamp" && ctrChkNode(opt))	opt->setText(i2s(timeStamp()));
    else if(a_path == "/up/cfg/inPROGLang") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SPRT_ID,SEC_RD))	opt->setText(inProgLang());
	if(ctrChkNode(opt,"set",RWRW__,"root",SPRT_ID,SEC_WR))	setInProgLang(opt->text());
    }
    else if(a_path == "/up/cfg/outPROGLang") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SPRT_ID,SEC_RD))	opt->setText(outProgLang());
	if(ctrChkNode(opt,"set",RWRW__,"root",SPRT_ID,SEC_WR))	setOutProgLang(opt->text());
    }
    else if(a_path == "/up/cfg/listTmpl" && ctrChkNode(opt)) {
	vector<string> lls, ls;
	//Templates
	SYS->daq().at().tmplLibList(lls);
	for(unsigned iL = 0; iL < lls.size(); iL++) {
	    SYS->daq().at().tmplLibAt(lls[iL]).at().list(ls);
	    for(unsigned iT = 0; iT < ls.size(); iT++)
		opt->childAdd("el")->setText(lls[iL]+"."+ls[iT]);
	}
	opt->childAdd("el")->setText("");
    }
    else if(a_path.substr(0,7) == "/up/cfg") TConfig::cntrCmdProc(opt,TSYS::pathLev(a_path,2),"root",SPRT_ID,RWRWR_);
    else if(a_path == "/in/WaitReqTm") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SPRT_ID,SEC_RD))	opt->setText(i2s(waitReqTm()));
	if(ctrChkNode(opt,"set",RWRW__,"root",SPRT_ID,SEC_WR))	setWaitReqTm(s2i(opt->text()));
    }
    else if(a_path.find("/in") == 0) {
	ResAlloc res(inCfgRes, false);
	if(func() && a_path == "/in/io") {
	    if(ctrChkNode(opt,"get",RWRW__,"root",SPRT_ID,SEC_RD)) {
		XMLNode *nId   = ctrMkNode("list",opt,-1,"/in/io/id","");
		XMLNode *nNm   = ctrMkNode("list",opt,-1,"/in/io/nm","");
		XMLNode *nType = ctrMkNode("list",opt,-1,"/in/io/tp","");
		XMLNode *nVal  = ctrMkNode("list",opt,-1,"/in/io/vl","");

		for(int id = 0; id < func()->ioSize(); id++) {
		    if(nId)	nId->childAdd("el")->setText(func()->io(id)->id());
		    if(nNm)	nNm->childAdd("el")->setText(func()->io(id)->name());
		    if(nType)	nType->childAdd("el")->setText(i2s(func()->io(id)->type()));
		    if(nVal)	nVal->childAdd("el")->setText(getS(id));
		}
	    }
	    if(ctrChkNode(opt,"set",RWRW__,"root",SPRT_ID,SEC_WR)) {
		int row = s2i(opt->attr("row"));
		string col = opt->attr("col");
		if(col == "vl") {
		    setS(row, opt->text());
		    lnkOutput(row, opt->text());
		}
		modif();
	    }
	}
	else if(a_path == "/in/PROG") {
	    if(ctrChkNode(opt,"get",RWRW__,"root",SPRT_ID,SEC_RD))	opt->setText(inProg());
	    if(ctrChkNode(opt,"set",RWRW__,"root",SPRT_ID,SEC_WR))	setInProg(opt->text());
	    if(ctrChkNode(opt,"SnthHgl",RWRW__,"root",SPRT_ID,SEC_RD))
		try {
		    SYS->daq().at().at(TSYS::strParse(inProgLang(),0,".")).at().
				    compileFuncSynthHighl(TSYS::strParse(inProgLang(),1,"."),*opt);
		} catch(...) { }
	}
	else if(a_path.find("/in/cfg") == 0 && DAQTmpl().size() && func()) TPrmTempl::Impl::cntrCmdProc(opt, "/in/cfg");
    }
    else if(a_path == "/out/PROG") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SPRT_ID,SEC_RD))	opt->setText(outProg());
	if(ctrChkNode(opt,"set",RWRW__,"root",SPRT_ID,SEC_WR))	setOutProg(opt->text());
	if(ctrChkNode(opt,"SnthHgl",RWRW__,"root",SPRT_ID,SEC_RD))
	    try {
		SYS->daq().at().at(TSYS::strParse(outProgLang(),0,".")).at().
				compileFuncSynthHighl(TSYS::strParse(outProgLang(),1,"."),*opt);
	    } catch(...) { }
    }
    else TCntrNode::cntrCmdProc(opt);
}
