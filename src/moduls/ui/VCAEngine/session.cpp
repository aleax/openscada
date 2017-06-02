
//OpenSCADA system module UI.VCAEngine file: session.cpp
/***************************************************************************
 *   Copyright (C) 2007-2016 by Roman Savochenko, <rom_as@oscada.org>      *
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

#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>

#include <tsys.h>

#include "vcaengine.h"
#include "session.h"

using namespace VCA;

//************************************************
//* Session: Project's session			 *
//************************************************
Session::Session( const string &iid, const string &iproj ) : mAlrmRes(true), mCalcRes(true),
    mId(iid), mPrjnm(iproj), mOwner("root"), mGrp("UI"), mUser(dataRes()), mPer(100), mPermit(RWRWR_), mEnable(false), mStart(false),
    endrunReq(false), mBackgrnd(false), mConnects(0), mCalcClk(1), mStyleIdW(-1)
{
    mUser = "root";
    mPage = grpAdd("pg_");
    sec = SYS->security();
    mReqTm = time(NULL);
}

Session::~Session( )
{
    for(map<uint8_t,Notify*>::iterator iN = mNotify.begin(); iN != mNotify.end(); ++iN) delete iN->second;
}

void Session::postEnable( int flag )
{
    if(flag&TCntrNode::NodeRestore) setEnable(true);
}

void Session::preDisable( int flag )
{
    if(enable()) setEnable(false);
}

void Session::setUser( const string &it )
{
    mUser = it;
    if(!enable()) mOwner = it;
}

void Session::setEnable( bool val )
{
    int64_t d_tm = 0;
    MtxAlloc res(mCalcRes, true);

    if(val == enable())	return;

    vector<string> pg_ls;

    if(val) {
	mess_debug(nodePath().c_str(),_("Enable session."));
	try {
	    if(mess_lev() == TMess::Debug) d_tm = TSYS::curTime();

	    //Connect to project
	    mParent = mod->prjAt(mPrjnm);

	    //Get data from project
	    mOwner	= parent().at().owner();
	    mGrp	= parent().at().grp();
	    mPermit	= parent().at().permit();
	    setPeriod(parent().at().period());

	    //Load previous style
	    string stVl = sessAttr("<Style>", user());
	    if(stVl.empty()) stVl = i2s(parent().at().stlCurent());
	    stlCurentSet(s2i(stVl));

	    if(mess_lev() == TMess::Debug) {
		mess_debug(nodePath().c_str(), _("Load previous style time: %f ms."), 1e-3*(TSYS::curTime()-d_tm));
		d_tm = TSYS::curTime();
	    }

	    //Create root pages
	    parent().at().list(pg_ls);
	    for(unsigned i_p = 0; i_p < pg_ls.size(); i_p++)
		if(!present(pg_ls[i_p]))
		    add(pg_ls[i_p],parent().at().at(pg_ls[i_p]).at().path());

	    if(mess_lev() == TMess::Debug) {
		mess_debug(nodePath().c_str(), _("Create root pages time: %f ms."), 1e-3*(TSYS::curTime()-d_tm));
		d_tm = TSYS::curTime();
	    }

	    //Pages enable
	    list(pg_ls);
	    for(unsigned i_ls = 0; i_ls < pg_ls.size(); i_ls++)
		try{ at(pg_ls[i_ls]).at().setEnable(true); }
		catch(TError &err) { mess_err(err.cat.c_str(), "%s", err.mess.c_str()); }

	    if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("Enable root pages time: %f ms."), 1e-3*(TSYS::curTime()-d_tm));

	    modifGClr();
	} catch(...) { mParent.free(); }
    }
    else {
	if(start()) setStart(false);

	mess_debug(nodePath().c_str(),_("Disable session."));

	//Pages disable
	list(pg_ls);
	for(unsigned i_ls = 0; i_ls < pg_ls.size(); i_ls++)
	    at(pg_ls[i_ls]).at().setEnable(false);

	//Delete pages
	for(unsigned i_ls = 0; i_ls < pg_ls.size(); i_ls++)
	    del(pg_ls[i_ls]);

	//Disconnect from project
	mParent.free();
    }

    mEnable = val;
}

void Session::setStart( bool val )
{
    int64_t d_tm = 0;

    MtxAlloc res(mCalcRes, true);

    vector<string> pg_ls;

    if(val) {
	//Enable session if it disabled
	if(!enable())	setEnable(true);

	if(mess_lev() == TMess::Debug)	d_tm = TSYS::curTime();

	mess_debug(nodePath().c_str(),_("Start session."));

	//Load Styles from project
	mStProp.clear();
	if(stlCurent() >= 0) {
	    parent().at().stlPropList(pg_ls);
	    for(unsigned i_sp = 0; i_sp < pg_ls.size(); i_sp++)
		mStProp[pg_ls[i_sp]] = parent().at().stlPropGet(pg_ls[i_sp], "", stlCurent());
	}

	if(mess_lev() == TMess::Debug) {
	    mess_debug(nodePath().c_str(), _("Load styles from project time: %f ms."), 1e-3*(TSYS::curTime()-d_tm));
	    d_tm = TSYS::curTime();
	}

	//Process all pages to on
	list(pg_ls);
	for(unsigned i_ls = 0; i_ls < pg_ls.size(); i_ls++)
	    at(pg_ls[i_ls]).at().setProcess(true);

	if(mess_lev() == TMess::Debug) {
	    mess_debug(nodePath().c_str(), _("Process on for all root pages time: %f ms."), 1e-3*(TSYS::curTime()-d_tm));
	    d_tm = TSYS::curTime();
	}

	//VCA server's force off
	MtxAlloc resAl(mAlrmRes, true);
	for(map<uint8_t,Notify*>::iterator iN = mNotify.begin(); iN != mNotify.end(); ++iN) iN->second->ntf(0);
	resAl.unlock();

	//Start process task
	if(!mStart) SYS->taskCreate(nodePath('.',true), 0, Session::Task, this);

	if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("Start process task time: %f ms."), 1e-3*(TSYS::curTime()-d_tm));
    }
    else {
	mess_debug(nodePath().c_str(),_("Stop session."));

	//Stop process task
	if(mStart) SYS->taskDestroy(nodePath('.',true), &endrunReq);

	//VCA server's force off
	MtxAlloc resAl(mAlrmRes, true);
	for(map<uint8_t,Notify*>::iterator iN = mNotify.begin(); iN != mNotify.end(); ++iN) iN->second->ntf(0);
	resAl.unlock();

	//Process all pages to off
	list(pg_ls);
	for(unsigned i_ls = 0; i_ls < pg_ls.size(); i_ls++)
	    at(pg_ls[i_ls]).at().setProcess(false);
    }
}

int Session::connect( )
{
    dataRes().lock();
    mConnects++;

    int rez;
    do { rez = (SYS->sysTm()%10000000)*10 + (int)(10*(float)rand()/(float)RAND_MAX); }
    while(mCons.find(rez) != mCons.end());
    mCons[rez] = true;
    dataRes().unlock();

    return rez;
}

void Session::disconnect( int conId )
{
    dataRes().lock();
    if(mConnects>0) mConnects--;

    map<int, bool>::iterator mC = mCons.find(conId);
    if(mC != mCons.end()) mCons.erase(mC);

    dataRes().unlock();
}

bool Session::modifChk( unsigned int tm, unsigned int iMdfClc )
{
    return (mCalcClk>=tm) ? (iMdfClc >= tm && iMdfClc <= mCalcClk) : (iMdfClc >= tm || iMdfClc <= mCalcClk);
}

string Session::ico( ) const		{ return (!parent().freeStat()) ? parent().at().ico() : ""; }

AutoHD<Project> Session::parent( ) const{ return mParent; }

void Session::add( const string &iid, const string &iparent )
{
    if(present(iid))	return;
    chldAdd(mPage, new SessPage(iid,iparent,this));
}

vector<string> Session::openList( )
{
    dataRes().lock();
    vector<string> rez = mOpen;
    dataRes().unlock();
    return rez;
}

void Session::openReg( const string &iid )
{
    unsigned i_op;
    dataRes().lock();
    for(i_op = 0; i_op < mOpen.size(); i_op++)
	if(iid == mOpen[i_op]) break;
    if(i_op >= mOpen.size())	mOpen.push_back(iid);
    dataRes().unlock();

    //Check for notifiers register
    for(unsigned iNtf = 0; iNtf < 7; iNtf++) {
	string aNtf = TSYS::strMess("notify%d", iNtf);
	AutoHD<SessPage> pgO = nodeAt(iid, 1);
	if(pgO.at().attrPresent(aNtf)) ntfReg(iNtf, pgO.at().attrAt(aNtf).at().getS(), iid);
    }
}

void Session::openUnreg( const string &iid )
{
    dataRes().lock();
    for(unsigned i_op = 0; i_op < mOpen.size(); i_op++)
	if(iid == mOpen[i_op]) mOpen.erase(mOpen.begin()+i_op);
    dataRes().unlock();

    //Check for notifiers unregister of the page
    for(unsigned iNtf = 0; iNtf < 7; iNtf++) ntfReg(iNtf, "", iid);
}

AutoHD<SessPage> Session::at( const string &id ) const	{ return chldAt(mPage, id); }

void Session::uiComm( const string &com, const string &prm, SessWdg *src )
{
    //Find of pattern adequancy for opened page
    string oppg, pBase;		//Opened page according of pattern

    vector<string> opLs = openList();
    for(unsigned iOp = 0; iOp < opLs.size(); iOp++) {
	string curPtEl, curEl;
	for(int iEl = 0; (curPtEl=TSYS::pathLev(prm,iEl++)).size(); )
	    if((curEl=TSYS::pathLev(opLs[iOp],iEl)).empty() || (curPtEl.compare(0,3,"pg_") == 0 && curPtEl != curEl)) break;
	if(curPtEl.empty()) { oppg = opLs[iOp]; break; }
    }

    pBase = oppg;
    if(pBase.empty() && src) pBase = src->path();

    //Individual commands process
    try {
	// Go to the destination page
	string curPtEl;
	AutoHD<SessPage> cpg;
	for(unsigned iEl = 0; (curPtEl=TSYS::pathLev(prm,iEl++)).size(); ) {
	    string opPg;
	    if(curPtEl.compare(0,3,"pg_") == 0) opPg = curPtEl.substr(3);
	    else if(curPtEl == "*" || (curPtEl == "$" && (com == "next" || com == "prev"))) {
		vector<string> pls;
		if(cpg.freeStat()) list(pls); else cpg.at().pageList(pls);
		if(pls.empty())	return;
		string curEl = TSYS::pathLev(pBase,iEl);
		if(curEl.empty()) {
		    if(curPtEl == "$")	return;
		    opPg = pls[0];
		}
		else {
		    curEl = curEl.substr(3);
		    int i_l;
		    for(i_l = 0; i_l < (int)pls.size(); i_l++)
			if(curEl == pls[i_l]) break;
		    if(i_l < (int)pls.size()) {
			if(curPtEl == "$") {
			    if(com == "next") i_l++;
			    if(com == "prev") i_l--;
			    i_l = (i_l < 0) ? (int)pls.size()-1 : (i_l >= (int)pls.size()) ? 0 : i_l;
			    opPg = pls[i_l];
			    if(opPg == curEl) return;
			}
			else opPg = curEl;
		    }
		    else {
			if(curPtEl == "$") return;
			opPg = pls[0];
		    }
		}
	    } else opPg = curPtEl;

	    // Go to next page
	    cpg = cpg.freeStat() ? at(opPg) : cpg.at().pageAt(opPg);
	}

	//Open founded page
	if(!cpg.freeStat()) {
	    if(!oppg.empty() && ((AutoHD<SessPage>)mod->nodeAt(oppg)).at().path() != cpg.at().path())
		((AutoHD<SessPage>)mod->nodeAt(oppg)).at().attrAt("pgOpenSrc").at().setS("");
	    cpg.at().attrAt("pgOpenSrc").at().setS(src->path());
	}
    }
    catch(TError &er) {
	//throw TError(nodePath().c_str(), _("Command '%s' for parameters '%s' error: %s"), com.c_str(), prm.c_str(), er.mess.c_str());
    }
}

string Session::sessAttr( const string &idw, const string &id, bool onlyAllow )
{
    TConfig cEl(&mod->elPrjSes());
    cEl.cfg("IDW").setS(idw);
    cEl.cfg("ID").setS(id);
    cEl.cfg("IO_VAL").setView(!onlyAllow);
    string db  = parent().at().DB();
    string tbl = parent().at().tbl()+"_ses";
    return (SYS->db().at().dataGet(db+"."+tbl,mod->nodePath()+tbl,cEl,false,true)) ? (onlyAllow?"1":cEl.cfg("IO_VAL").getS()) : "";
}

void Session::sessAttrSet( const string &idw, const string &id, const string &val )
{
    TConfig cEl(&mod->elPrjSes());
    cEl.cfg("IDW").setS(idw);
    cEl.cfg("ID").setS(id);
    cEl.cfg("IO_VAL").setS(val);
    string db  = parent().at().DB();
    string tbl = parent().at().tbl()+"_ses";
    SYS->db().at().dataSet(db+"."+tbl, mod->nodePath()+tbl, cEl, false, true);
}

void Session::alarmSet( const string &wpath, const string &alrm )
{
    if(wpath.empty()) return;

    //Notifications queue update
    MtxAlloc resAl(mAlrmRes, true);
    for(map<uint8_t,Notify*>::iterator iN = mNotify.begin(); iN != mNotify.end(); ++iN) iN->second->queueSet(wpath, alrm);
}

int Session::alarmStat( )
{
    uint8_t alev = 0, atp = 0, aqtp = 0;
    vector<string> ls;
    list(ls);
    for(unsigned i_p = 0; i_p < ls.size(); i_p++) {
	int ast = at(ls[i_p]).at().attrAt("alarmSt").at().getI();
	alev = vmax(alev,ast&0xFF);
	atp |= (ast>>8)&0xFF;
	aqtp |= (ast>>16)&0xFF;
    }

    return (aqtp<<16)|(atp<<8)|alev;
}

void Session::alarmQuittance( const string &wpath, uint8_t quit_tmpl, bool ret )
{
    string tStr;
    if(!wpath.empty())
	for(int off = 0; (tStr=TSYS::strParse(wpath,0,";",&off)).size(); )
	    ((AutoHD<SessWdg>)mod->nodeAt(tStr)).at().alarmQuittance(quit_tmpl, true, ret);
    else {
	vector<string> ls;
	list(ls);
	for(unsigned i_p = 0; i_p < ls.size(); i_p++)
	    at(ls[i_p]).at().alarmQuittance(quit_tmpl, true, ret);
    }

    //The notifications queue quittance
    MtxAlloc resAl(mAlrmRes, true);
    for(map<uint8_t,Notify*>::iterator iN = mNotify.begin(); iN != mNotify.end(); ++iN)
	iN->second->queueQuittance(wpath, quit_tmpl, ret);
}

void Session::ntfReg( uint8_t tp, const string &props, const string &pgCrtor )
{
    vector<string> pgPropsQ;

    MtxAlloc res(mAlrmRes, true);

    //Find for presented notification type
    map<uint8_t,Notify*>::iterator iN = mNotify.find(tp);
    if(iN != mNotify.end()) {
	if(pgCrtor == iN->second->pgCrtor() && props == iN->second->props())	return;
	pgPropsQ = iN->second->pgPropsQ;
	if(pgCrtor != iN->second->pgCrtor()) {
	    // Check the queue for pointed page already here
	    for(vector<string>::iterator iQ = iN->second->pgPropsQ.begin(); iQ != iN->second->pgPropsQ.end(); ++iQ)
		if(TSYS::strLine(*iQ,0) == pgCrtor) {
		    if(props.empty()) iN->second->pgPropsQ.erase(iQ);
		    else *iQ = pgCrtor+"\n"+props;
		    return;
	    }
	    if(props.empty()) return;
	    pgPropsQ.push_back(iN->second->pgProps);
	}
	delete iN->second;
	mNotify.erase(iN);
    }

    //New or replaced creation
    if(props.size()) {
	mNotify[tp] = new Notify(tp, pgCrtor+"\n"+props, this);
	mNotify[tp]->pgPropsQ = pgPropsQ;
    }
    //Take and place a notificator from the queue
    else if(pgPropsQ.size()) {
	mNotify[tp] = new Notify(tp, pgPropsQ.back(), this); pgPropsQ.pop_back();
	mNotify[tp]->pgPropsQ = pgPropsQ;
    }
}

void *Session::Task( void *icontr )
{
    vector<string> pls;
    Session &ses = *(Session *)icontr;

    ses.endrunReq = false;
    ses.mStart	   = true;

    ses.list(pls);
    while(!ses.endrunReq) {
	//Calc session pages and all other items at recursion
	for(unsigned i_l = 0; i_l < pls.size(); i_l++)
	    try { ses.at(pls[i_l]).at().calc(false, false); }
	    catch(TError &err) {
		mess_err(err.cat.c_str(),"%s",err.mess.c_str());
		mess_err(ses.nodePath().c_str(),_("Session '%s' calculate error."),pls[i_l].c_str());
	    }

	//VCA server's notifications processing
	MtxAlloc resAl(ses.mAlrmRes, true);
	int aSt = ses.alarmStat();
	for(map<uint8_t,Notify*>::iterator iN = ses.mNotify.begin(); iN != ses.mNotify.end(); ++iN) iN->second->ntf(aSt);
	resAl.unlock();

	//Sleep to next cycle
	TSYS::taskSleep((int64_t)ses.period()*1000000);
	if((ses.mCalcClk++) == 0) ses.mCalcClk = 1;
    }

    ses.mStart = false;

    return NULL;
}

void Session::stlCurentSet( int sid )
{
    if(mStyleIdW == sid) return;

    mStyleIdW = sid;

    if(start()) {
	MtxAlloc res(dataRes(), true);

	//Load Styles from project
	mStProp.clear();

	if(sid >= 0 && sid < parent().at().stlSize()) {
	    vector<string> pg_ls;
	    parent().at().stlPropList(pg_ls);
	    for(unsigned i_sp = 0; i_sp < pg_ls.size(); i_sp++)
		mStProp[pg_ls[i_sp]] = parent().at().stlPropGet(pg_ls[i_sp], "", sid);
	}
	else mStyleIdW = -1;
    }

    //Write to DB
    if(enable()) sessAttrSet("<Style>", user(), i2s(mStyleIdW));
}

string Session::stlPropGet( const string &pid, const string &def )
{
    MtxAlloc res(dataRes(), true);

    if(stlCurent() < 0 || pid.empty() || pid == "<Styles>") return def;

    map<string,string>::iterator iStPrp = mStProp.find(pid);
    if(iStPrp != mStProp.end()) return iStPrp->second;

    return def;
}

bool Session::stlPropSet( const string &pid, const string &vl )
{
    MtxAlloc res(dataRes(), true);
    if(stlCurent() < 0 || pid.empty() || pid == "<Styles>") return false;
    map<string,string>::iterator iStPrp = mStProp.find(pid);
    if(iStPrp == mStProp.end()) return false;
    iStPrp->second = vl;

    return true;
}

TVariant Session::objFuncCall( const string &iid, vector<TVariant> &prms, const string &cuser )
{
    // string user( ) - the session user
    if(iid == "user")	return user();
    // int alrmQuittance( int quit_tmpl, string wpath = "", bool ret = false ) -
    //        alarm quittance, or return for <ret>, <wpath> with template <quit_tmpl>. If <wpath> is empty string then make global quittance.
    //  quit_tmpl - quittance template
    //  wpath - path to widget
    //  ret - return the quittance
    else if(iid == "alrmQuittance" && prms.size() >= 1) {
	alarmQuittance((prms.size()>=2) ? prms[1].getS() : "", ~prms[0].getI(), (prms.size()>=3) ? prms[2].getB() : false);
	return 0;
    }

    return TCntrNode::objFuncCall(iid,prms,cuser);
}

void Session::cntrCmdProc( XMLNode *opt )
{
    string a_path = opt->attr("path");
    //Service commands process
    if(a_path == "/serv/pg") {	//Pages operations
	if(ctrChkNode(opt,"openlist",permit(),owner().c_str(),grp().c_str(),SEC_RD)) {	//Open pages list
	    // Check for propper connection ID
	    int conId = s2i(opt->attr("conId"));
	    dataRes().lock();
	    bool conIdOK = (conId == 0 || mCons.find(conId) != mCons.end());
	    dataRes().unlock();
	    if(!conIdOK) throw TError(nodePath().c_str(), _("Unregistered connection %d on the session."), conId);

	    // Main process
	    unsigned tm = strtoul(opt->attr("tm").c_str(), NULL, 10);
	    unsigned ntm = calcClk();
	    vector<string> lst = openList();
	    for(unsigned i_f = 0; i_f < lst.size(); i_f++) {
		XMLNode *pel = opt->childAdd("pg");
		pel->setText(lst[i_f]);

		if(tm) {
		    // Permission to view page check
		    AutoHD<SessWdg> swdg = nodeAt(lst[i_f], 1, 0, 0, true);
		    if(swdg.freeStat() ||
			    !SYS->security().at().access(opt->attr("user"),SEC_RD,swdg.at().owner(),swdg.at().grp(),swdg.at().permit()))
			continue;
		    // Changed widgets list add
		    vector<string> updEl;
		    swdg.at().getUpdtWdg("", tm, updEl);
		    pel->setAttr("updWdg", i2s(updEl.size()));
		}
	    }
	    opt->setAttr("tm", u2s(ntm));
	}
	else if(ctrChkNode(opt,"open",permit(),owner().c_str(),grp().c_str(),SEC_WR))		//Open pages
	    ((AutoHD<SessWdg>)nodeAt(opt->attr("pg"),1)).at().attrAt("pgOpen").at().setB(true);
	else if(ctrChkNode(opt,"close",RWRWRW,owner().c_str(),grp().c_str(),SEC_WR))		//Close open pages
	    ((AutoHD<SessWdg>)nodeAt(opt->attr("pg"),1)).at().attrAt("pgOpen").at().setB(false);
	mReqTm = time(NULL);
	return;
    }
    else if(a_path == "/serv/alarm") {	//Alarm operations
	if(ctrChkNode(opt,"get",permit(),owner().c_str(),grp().c_str(),SEC_RD)) {
	    // Get alarm status
	    int aSt = alarmStat();
	    opt->setAttr("alarmSt", i2s(aSt));

	    // Get visualiser side notification's resource
	    if(opt->attr("mode") == "resource") {
		MtxAlloc resAl(mAlrmRes, true);
		map<uint8_t,Notify*>::iterator iN = mNotify.find(s2i(opt->attr("tp")));
		unsigned tm = strtoul(opt->attr("tm").c_str(), NULL, 10);
		string res, mess, lang, wdg = opt->attr("wdg");
		if(iN != mNotify.end()) res = TSYS::strEncode(iN->second->ntfRes(tm,wdg,mess,lang), TSYS::base64);
		opt->setAttr("tm", u2s(tm))->setAttr("wdg", wdg)->setAttr("mess", mess)->setAttr("lang", lang)->setText(res);
	    }
	}
	else if(ctrChkNode(opt,"quittance",permit(),owner().c_str(),grp().c_str(),SEC_WR))
	    alarmQuittance(opt->attr("wdg"), ~s2i(opt->attr("tmpl")), s2i(opt->attr("ret")));
	return;
    }

    //Get page info
    if(opt->name() == "info") {
	TCntrNode::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Session: ")+id(),permit(),owner().c_str(),grp().c_str());
	if(ico().size()) ctrMkNode("img",opt,-1,"/ico","",R_R_R_);
	if(ctrMkNode("branches",opt,-1,"/br","",R_R_R_))
	    ctrMkNode("grp",opt,-1,"/br/pg_",_("Page"),R_R_R_,"root",SUI_ID,1,"idm","1");
	if(ctrMkNode("area",opt,-1,"/obj",_("Session"))) {
	    if(ctrMkNode("area",opt,-1,"/obj/st",_("State"))) {
		ctrMkNode("fld",opt,-1,"/obj/st/en",_("Enable"),permit(),owner().c_str(),grp().c_str(),1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/obj/st/start",_("Start"),permit(),owner().c_str(),grp().c_str(),1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/obj/st/user",_("User"),R_R_R_,"root",SUI_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/obj/st/owner",_("Owner"),R_R_R_,"root",SUI_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/obj/st/grp",_("Group"),R_R_R_,"root",SUI_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/obj/st/u_a",_("Access"),R_R_R_,"root",SUI_ID,4,"tp","dec","dest","select",
		    "sel_id","0;4;6","sel_list",_("No access;View;View and control"));
		ctrMkNode("fld",opt,-1,"/obj/st/g_a","",R_R_R_,"root",SUI_ID,4,"tp","dec","dest","select",
		    "sel_id","0;4;6","sel_list",_("No access;View;View and control"));
		ctrMkNode("fld",opt,-1,"/obj/st/o_a","",R_R_R_,"root",SUI_ID,4,"tp","dec","dest","select",
		    "sel_id","0;4;6","sel_list",_("No access;View;View and control"));
		ctrMkNode("fld",opt,-1,"/obj/st/prj",_("Project"),permit(),owner().c_str(),grp().c_str(),4,
		    "tp","str","idm","1","dest","select","select","/obj/prj_ls");
		ctrMkNode("fld",opt,-1,"/obj/st/backgrnd",_("Background execution mode"),R_R_R_,"root",SUI_ID,1,"tp","bool");
		if(start()) {
		    ctrMkNode("fld",opt,-1,"/obj/st/calc_tm",_("Calculate session time"),R_R_R_,"root",SUI_ID,1,"tp","str");
		    ctrMkNode("fld",opt,-1,"/obj/st/connect",_("Connections counter"),R_R_R_,"root",SUI_ID,1,"tp","int");
		    ctrMkNode("fld",opt,-1,"/obj/st/reqTime",_("Last request"),R_R_R_,"root",SUI_ID,1,"tp","time");
		    if(!backgrnd()) ctrMkNode("fld",opt,-1,"/obj/st/leftToClose",_("Left to force close (s)"),R_R_R_,"root",SUI_ID,1,"tp","dec");
		}
	    }
	    if(ctrMkNode("area",opt,-1,"/obj/cfg",_("Configuration"))) {
		ctrMkNode("fld",opt,-1,"/obj/cfg/per",_("Period (ms)"),permit(),owner().c_str(),grp().c_str(),1,"tp","dec");
		ctrMkNode("fld",opt,-1,"/obj/cfg/style",_("Style"),permit(),owner().c_str(),grp().c_str(),3,
		    "tp","dec","dest","select","select","/obj/cfg/stLst");
		ctrMkNode("list",opt,-1,"/obj/cfg/openPg",_("Opened pages"),R_R_R_,"root",SUI_ID,1,"tp","str");
	    }
	}
	if(ctrMkNode("area",opt,-1,"/page",_("Pages")))
	    ctrMkNode("list",opt,-1,"/page/page",_("Pages"),R_R_R_,"root",SUI_ID,3,"tp","br","idm","1","br_pref","pg_");
	return;
    }

    //Process command to page
    if(a_path == "/ico" && ctrChkNode(opt))	opt->setText(ico());
    else if(a_path == "/obj/st/en") {
	if(ctrChkNode(opt,"get",permit(),owner().c_str(),grp().c_str(),SEC_RD))	opt->setText(i2s(enable()));
	if(ctrChkNode(opt,"set",permit(),owner().c_str(),grp().c_str(),SEC_WR))	setEnable(s2i(opt->text()));
    }
    else if(a_path == "/obj/st/start") {
	if(ctrChkNode(opt,"get",permit(),owner().c_str(),grp().c_str(),SEC_RD))	opt->setText(i2s(start()));
	if(ctrChkNode(opt,"set",permit(),owner().c_str(),grp().c_str(),SEC_WR))	setStart(s2i(opt->text()));
    }
    else if(a_path == "/obj/st/user" && ctrChkNode(opt))	opt->setText(user());
    else if(a_path == "/obj/st/owner" && ctrChkNode(opt))	opt->setText(owner());
    else if(a_path == "/obj/st/grp" && ctrChkNode(opt))		opt->setText(grp());
    else if((a_path == "/obj/st/u_a" || a_path == "/obj/st/g_a" || a_path == "/obj/st/o_a") && ctrChkNode(opt))
    {
	if(a_path == "/obj/st/u_a")	opt->setText(i2s((permit()>>6)&0x7));
	if(a_path == "/obj/st/g_a")	opt->setText(i2s((permit()>>3)&0x7));
	if(a_path == "/obj/st/o_a")	opt->setText(i2s(permit()&0x7));
    }
    else if(a_path == "/obj/st/prj") {
	if(ctrChkNode(opt,"get",permit(),owner().c_str(),grp().c_str(),SEC_RD))	opt->setText(projNm());
	if(ctrChkNode(opt,"set",permit(),owner().c_str(),grp().c_str(),SEC_WR))	setProjNm(opt->text());
    }
    else if(a_path == "/obj/st/backgrnd" && ctrChkNode(opt))	opt->setText(i2s(backgrnd()));
    else if(a_path == "/obj/st/calc_tm" && ctrChkNode(opt))
	opt->setText(tm2s(SYS->taskUtilizTm(nodePath('.',true)))+"["+tm2s(SYS->taskUtilizTm(nodePath('.',true),true))+"]");
    else if(a_path == "/obj/st/connect" && ctrChkNode(opt))	opt->setText(i2s(connects()));
    else if(a_path == "/obj/st/reqTime" && ctrChkNode(opt))	opt->setText(i2s(reqTm()));
    else if(a_path == "/obj/st/leftToClose" && ctrChkNode(opt))	opt->setText(i2s(vmax(0,DIS_SES_TM-(time(NULL)-reqTm()))));
    else if(a_path == "/obj/prj_ls" && ctrChkNode(opt)) {
	vector<string> lst;
	mod->prjList(lst);
	for(unsigned i_f=0; i_f < lst.size(); i_f++)
	    opt->childAdd("el")->setAttr("id",lst[i_f])->setText(mod->prjAt(lst[i_f]).at().name());
    }
    else if(a_path == "/obj/cfg/per") {
	if(ctrChkNode(opt,"get",permit(),owner().c_str(),grp().c_str(),SEC_RD))	opt->setText(i2s(period()));
	if(ctrChkNode(opt,"set",permit(),owner().c_str(),grp().c_str(),SEC_WR))	setPeriod(s2i(opt->text()));
    }
    else if(a_path == "/obj/cfg/style") {
	if(ctrChkNode(opt,"get",permit(),owner().c_str(),grp().c_str(),SEC_RD))	opt->setText(i2s(stlCurent()));
	if(ctrChkNode(opt,"set",permit(),owner().c_str(),grp().c_str(),SEC_WR))	stlCurentSet(s2i(opt->text()));
    }
    else if(a_path == "/obj/cfg/stLst" && ctrChkNode(opt)) {
	opt->childAdd("el")->setAttr("id","-1")->setText(_("No style"));
	if(enable())
	    for(int iSt = 0; iSt < parent().at().stlSize(); iSt++)
		opt->childAdd("el")->setAttr("id", i2s(iSt))->setText(TSYS::strSepParse(parent().at().stlGet(iSt),0,';'));
    }
    else if(a_path == "/obj/cfg/openPg" && ctrChkNode(opt)) {
	vector<string> lst = openList();
	for(unsigned i_f = 0; i_f < lst.size(); i_f++)
	    opt->childAdd("el")->setText(lst[i_f]);
    }
    else if((a_path == "/br/pg_" || a_path == "/page/page") && ctrChkNode(opt)) {
	vector<string> lst;
	list(lst);
	for(unsigned i_f=0; i_f < lst.size(); i_f++)
	    opt->childAdd("el")->setAttr("id",lst[i_f])->setText(at(lst[i_f]).at().name());
    }
    else TCntrNode::cntrCmdProc(opt);
}

//* Notify: Generic notifying object.		 *
//************************************************
Session::Notify::Notify( uint8_t itp, const string &ipgProps, Session *iown ) : pgProps(ipgProps),
    tp(itp), alSt(0xFFFFFFFF), repDelay(-1), comIsExtScript(false), f_notify(false), f_resource(false), f_queue(false), f_qMergeMess(false),
    toDo(false), alEn(false), mQueueCurNtf(-1), mQueueCurTm(0), dataM(true), mOwner(iown)
{
    //Parse properties
    string iLn, iOpt, iProps = props();
    bool hasLang  = false, hasFlags = false;
    for(int off = 0, lCnt = 0, fPos; (!hasLang || !hasFlags) && (iLn=TSYS::strLine(iProps,0,&off)).size(); lCnt++)
	if(!hasLang && !lCnt && iLn.find("#!") == 0) { hasLang = comIsExtScript = true; continue; }
	else if(!hasFlags && (size_t)(fPos=iLn.find("flags=")) != string::npos) {
	    for(fPos += 6; (iOpt=TSYS::strParse(iLn,0,"|",&fPos)).size(); )
		if(iOpt.compare(0,6,"notify") == 0) {
		    f_notify = true;
		    repDelay = (iOpt.size() > 6) ? vmax(0,vmin(100,atoi(iOpt.c_str()+6))) : -1;
		}
		else if(iOpt == "resource")	f_resource = true;
		else if(iOpt == "queue")	{ f_queue = true; if(repDelay < 0) repDelay = 0; }
		else if(iOpt == "qMergeMess")	f_qMergeMess = true;
	    hasFlags = true;
	}

    //The command procedure prepare
    if(comIsExtScript) {
	// Prepare the external script
	comProc = "ses_"+owner()->id()+"_ntf"+i2s(tp);
	bool fOK = false;
	int hd = open(comProc.c_str(), O_CREAT|O_TRUNC|O_WRONLY, 0775);
	if(hd >= 0) {
	    fOK = write(hd, props().data(), props().size()) == (ssize_t)props().size();
	    close(hd);
	}
	if(!fOK) {
	    mess_err(owner()->nodePath().c_str(), _("Function of the notificator '%s' error: %s"), comProc.c_str(), strerror(errno));
	    comProc = "";
	}
    }
    else {
	// Prepare internal procedure
	TFunction funcIO("ses_"+owner()->id()+"_ntf"+i2s(tp));
	//funcIO.setStor(DB());
	funcIO.ioIns(new IO("en",_("Enabled notification"),IO::Boolean,IO::Default), IFA_en);
	funcIO.ioIns(new IO("doNtf",_("Doing notification"),IO::Boolean,IO::Default), IFA_doNtf);
	funcIO.ioIns(new IO("doRes",_("Doing resource"),IO::Boolean,IO::Default), IFA_doRes);
	funcIO.ioIns(new IO("res",_("Resource stream"),IO::String,IO::Output), IFA_res);
	funcIO.ioIns(new IO("mess",_("Notification message"),IO::String,IO::Default), IFA_mess);
	funcIO.ioIns(new IO("lang",_("Notification message's language"),IO::String,IO::Default), IFA_lang);
	try { comProc = SYS->daq().at().at("JavaLikeCalc").at().compileFunc("JavaScript", funcIO, props()); }
	catch(TError &er) {
	    mess_err(owner()->nodePath().c_str(), _("Function of the notificator '%s' error: %s"), funcIO.id().c_str(), er.mess.c_str());
	}
    }

    if(f_notify) {
	//Call conditional variable init
	pthread_cond_init(&callCV, NULL);

	//Notification task create
	SYS->taskCreate(owner()->nodePath('.',true)+".ntf"+i2s(tp), 0, Session::Notify::Task, this);
    }
}

Session::Notify::~Notify( )
{
    if(f_notify) {
	SYS->taskDestroy(owner()->nodePath('.',true)+".ntf"+i2s(tp), NULL, 10, false, &callCV);
	pthread_cond_destroy(&callCV);
    }

    //The command procedure remove
    if(comIsExtScript && comProc.size()) remove(comProc.c_str());
}

string	Session::Notify::pgCrtor( )	{ return TSYS::strLine(pgProps, 0); }

string	Session::Notify::props( )
{
    int off = 0;
    TSYS::strLine(pgProps, 0, &off);
    return pgProps.substr(off);
}

void Session::Notify::ntf( int ialSt )
{
    //Check for the alarm state change
    if(!f_notify || !(((ialSt^alSt)>>16)&(1<<tp)))	return;

    alEn = (bool)((ialSt>>16)&(1<<tp));
    pthread_mutex_lock(&dataM.mtx());
    toDo = true;
    pthread_cond_signal(&callCV);
    pthread_mutex_unlock(&dataM.mtx());

    alSt = ialSt;
}

string Session::Notify::ntfRes( unsigned &itm, string &wpath, string &mess, string &lang )
{
    string rez;

    if((lang=SYS->security().at().usrAt(owner()->user()).at().lang()).empty()) lang = Mess->lang();

    // Just the resource doing request
    if(!f_queue && f_resource) commCall(false, true, rez, "", lang);

    if(f_queue) {
	unsigned tm = itm;
	itm = owner()->calcClk();

	// Find entry, return it and the resource
	MtxAlloc res(dataM, true);
	int iQ, iFirst = -1, iNext = -1;
	for(iQ = mQueue.size()-1; iQ >= 0; iQ--) {
	    if(mQueue[iQ].quittance) continue;
	    if(wpath.empty() || owner()->modifChk(tm,mQueue[iQ].clc) || iNext > 0)	break;	//First, new and next entries break
	    if(iFirst < 0) iFirst = iQ;
	    if(wpath == mQueue[iQ].path) iNext = iQ;
	}
	if(iQ < 0 && iFirst >= 0) iQ = iFirst;	//Return to first entry
	if(iQ >= 0) {
	    wpath = mQueue[iQ].path;
	    mess = mQueue[iQ].mess;
	    //  Get the resource direct
	    if(!mQueue[iQ].tpArg.empty())
		rez = TSYS::strDecode(((AutoHD<SessWdg>)mod->nodeAt(TSYS::strParse(mQueue[iQ].path,0,";"))).at().
				resourceGet(mQueue[iQ].tpArg), TSYS::base64);
	    //  Call the resource producing procedure
	    else commCall(false, true, rez, mQueue[iQ].mess, lang);
	    mQueueCurNtf = iQ;
	} else { mQueueCurNtf = -1; wpath = mess = ""; }
    }

    return rez;
}

void Session::Notify::queueSet( const string &wpath, const string &alrm )
{
    //Calls to the queue update from alarmSet()
    if(!f_queue) return;

    int aOff = 0;
    uint8_t	aLev = s2i(TSYS::strParse(alrm,0,"|",&aOff));
    string	aCat = TSYS::strParse(alrm, 0, "|", &aOff),
		aMess= TSYS::strParse(alrm, 0, "|", &aOff);
    uint8_t	aTp  = s2i(TSYS::strParse(alrm,0,"|",&aOff));
    string	aArg = TSYS::strParse(alrm,0,"|",&aOff);

    //if(!(aTp&(1<<tp)))	return;

    QueueIt qIt(wpath+";", aLev, aCat, aMess, aArg, owner()->calcClk());

    MtxAlloc res(dataM, true);

    unsigned iQ = 0;
    // Check for the entry to present
    while(iQ < mQueue.size() && mQueue[iQ].path.find(qIt.path) == string::npos && (!f_qMergeMess || qIt.mess != mQueue[iQ].mess)) iQ++;
    //while(iQ < mQueue.size() && mQueue[iQ].path != qIt.path) iQ++;

    // Clean up the entry from queue
    if(!qIt.lev || !(aTp&(1<<tp))) {
	if(iQ < mQueue.size()) mQueue.erase(mQueue.begin()+iQ);
	return;
    }
    // Update presented and same level entry
    if(iQ < mQueue.size() && f_qMergeMess && qIt.mess == mQueue[iQ].mess) {
	if(mQueue[iQ].path.find(qIt.path) == string::npos) mQueue[iQ].path += qIt.path;
	mQueue[iQ].lev = vmax(mQueue[iQ].lev, qIt.lev);
	mQueue[iQ].quittance = false;
    }
    else if(iQ < mQueue.size() && qIt.lev == mQueue[iQ].lev) mQueue[iQ] = qIt;
    else {
	// Remove presented by different level entry
	if(iQ < mQueue.size()) {
	    mQueue.erase(mQueue.begin()+iQ);
	    //  Entry into the notification update
	    if((int)iQ == mQueueCurNtf) mQueueCurNtf = -1;
	    if((int)iQ < mQueueCurNtf && mQueueCurNtf >= 0) mQueueCurNtf--;
	}
	//  Place the entry to the queue
	unsigned iQ1 = 0;
	while(iQ1 < mQueue.size() && qIt.lev >= mQueue[iQ1].lev) iQ1++;
	if(iQ1 < mQueue.size()) {
	    mQueue.insert(mQueue.begin()+iQ1, qIt);
	    if((int)iQ <= mQueueCurNtf && mQueueCurNtf >= 0) mQueueCurNtf++;
	}
	else mQueue.push_back(qIt);
    }
}

void Session::Notify::queueQuittance( const string &wpath, uint8_t quitTmpl, bool ret )
{
    //Calls to the queue update from alarmQuittance()
    if(!f_queue || (quitTmpl&(1<<tp))) return;

    pthread_mutex_lock(&dataM.mtx());
    string tStr, tStr1;
    for(unsigned iQ = 0; iQ < mQueue.size(); iQ++) {
	bool toQuitt = false;
	if(!wpath.size()) toQuitt = true;
	else for(int off = 0; !toQuitt && (tStr=TSYS::strParse(wpath,0,";",&off)).size(); )
	    for(int off1 = 0; !toQuitt && (tStr1=TSYS::strParse(mQueue[iQ].path,0,";",&off1)).size(); )
		toQuitt = tStr1.compare(0,tStr.size(),tStr) == 0;
	if(toQuitt) mQueue[iQ].quittance = !ret;
    }

    pthread_mutex_unlock(&dataM.mtx());
}

void Session::Notify::commCall( bool doNtf, bool doRes, string &res, const string &mess, const string &lang )
{
    if(comProc.empty()) return;

    //Shared data obtain
    pthread_mutex_lock(&dataM.mtx());
    string wcomProc = comProc;
    pthread_mutex_unlock(&dataM.mtx());

    if(comIsExtScript) {
	string resFile = "ses_"+owner()->id()+"_res"+i2s(tp);
	int hdRes = res.size() ? open(resFile.c_str(), O_CREAT|O_TRUNC|O_WRONLY, 0664) : -1;
	if(hdRes >= 0) { write(hdRes, res.data(), res.size()); close(hdRes); }
	// Prepare environment and execute the external script
	system(("en="+i2s(alEn)+" doNtf="+i2s(doNtf)+" doRes="+i2s(doRes)+" res="+resFile+
		" mess=\""+TSYS::strEncode(mess,TSYS::SQL)+"\" lang=\""+TSYS::strEncode(lang,TSYS::SQL)+"\" ./"+wcomProc).c_str());
	if(doRes) {
	    hdRes = open(resFile.c_str(), O_RDONLY);
	    if(hdRes >= 0) {
		char buf[STR_BUF_LEN];
		res.clear();
		if(lseek(hdRes,0,SEEK_END) < 100*1024*1024) {
		    lseek(hdRes, 0, SEEK_SET);
		    for(int len; (len=read(hdRes,buf,sizeof(buf))) > 0; ) res.append(buf, len);
		}
		close(hdRes);
	    }
	}
	if(hdRes >= 0) remove(resFile.c_str());
    }
    else {
	// Prepare and execute internal procedure
	TValFunc funcV;
	funcV.setFunc(&((AutoHD<TFunction>)SYS->nodeAt(wcomProc)).at());

	//  Load inputs
	funcV.setB(IFA_en, alEn);
	funcV.setB(IFA_doNtf, doNtf);
	funcV.setB(IFA_doRes, doRes);
	funcV.setS(IFA_res, res);
	funcV.setS(IFA_mess, mess);
	funcV.setS(IFA_lang, lang);

	//  Call to processing
	funcV.calc();

	//  Get outputs
	if(doRes) res = funcV.getS(IFA_res);
    }
}

void *Session::Notify::Task( void *intf )
{
    Session::Notify &ntf = *(Session::Notify*)intf;

    pthread_mutex_lock(&ntf.dataM.mtx());
    while(!TSYS::taskEndRun() || ntf.toDo) {
	if(!ntf.toDo) pthread_cond_wait(&ntf.callCV, &ntf.dataM.mtx());
	if(!ntf.toDo || ntf.comProc.empty()) { ntf.toDo = false; continue; }
	ntf.toDo = false;
	pthread_mutex_unlock(&ntf.dataM.mtx());

	string ntfRes, ntfMess, ntfLang;
	unsigned delayCnt = 0;
	do {
	    if(delayCnt) { TSYS::sysSleep(1); delayCnt--; continue; }

	    //  Get the resources for the notification
	    if((ntf.f_queue || ntf.f_resource) && ntf.alEn)
		ntfRes = ntf.ntfRes(ntf.mQueueCurTm, ntf.mQueueCurPath, ntfMess, ntfLang);

	    //  Same notification
	    ntf.commCall(true, false, ntfRes, ntfMess, ntfLang);

	    delayCnt = ntf.repDelay;
	} while((ntf.repDelay >= 0 || ntf.f_queue) && ntf.alEn && !TSYS::taskEndRun());

	pthread_mutex_lock(&ntf.dataM.mtx());
    }
    pthread_mutex_unlock(&ntf.dataM.mtx());

    return NULL;
}

//************************************************
//* SessPage: Page of Project's session          *
//************************************************
SessPage::SessPage( const string &iid, const string &ipage, Session *sess ) :
    SessWdg(iid,ipage,sess), mClosePgCom(false), mDisMan(false), mFuncM(true)
{
    mPage = grpAdd("pg_");
}

SessPage::~SessPage( )
{

}

string SessPage::path( ) const	{ return ownerFullId(true)+"/pg_"+id(); }

void SessPage::setEnable( bool val, bool force )
{
    vector<string> pg_ls;

    MtxAlloc fRes(funcM(), true);	//Prevent multiple entry

    //Page enable
    if(val) {
	mess_sys(TMess::Debug, _("Enable page."));
	mToEn = true;
	// Check for full enable need
	bool pgOpen = (!(parent().at().prjFlags()&Page::Empty) && parent().at().attrAt("pgOpen").at().getB());
	if((pgOpen || force || parent().at().attrAt("pgNoOpenProc").at().getB()) && !enable()) {
	    SessWdg::setEnable(true);
	    if(pgOpen) ownerSess()->openReg(path());
	}
	// Child pages process
	if(!force) {
	    //Create included pages
	    parent().at().pageList(pg_ls);
	    for(unsigned i_p = 0; i_p < pg_ls.size(); i_p++)
		if(!pagePresent(pg_ls[i_p]))
		    pageAdd(pg_ls[i_p], parent().at().pageAt(pg_ls[i_p]).at().path());

	    //Enable included pages
	    pageList(pg_ls);
	    for(unsigned i_l = 0; i_l < pg_ls.size(); i_l++)
		try{ pageAt(pg_ls[i_l]).at().setEnable(true); }
		catch(TError &err)	{ mess_err(err.cat.c_str(), "%s", err.mess.c_str()); }
	}
	mToEn = false;
    }
    else if(enable()) {
	mess_sys(TMess::Debug, _("Disable page."));

	//Unregister opened page
	if(!(parent().at().prjFlags()&Page::Empty) && attrPresent("pgOpen") && attrAt("pgOpen").at().getB())
	    ownerSess()->openUnreg(path());

	//Disable include pages
	pageList(pg_ls);
	for(unsigned i_l = 0; i_l < pg_ls.size(); i_l++)
	    pageAt(pg_ls[i_l]).at().setEnable(false);

	//Delete included pages
	for(unsigned i_l = 0; i_l < pg_ls.size(); i_l++)
	    pageDel(pg_ls[i_l]);

	SessWdg::setEnable(false);
	mDisMan = true;
    }
}

void SessPage::setProcess( bool val, bool lastFirstCalc )
{
    //Change process state for included pages
    //!!!! Need for rewrite by process included to containers but not included to the subtree !!!!
    vector<string> ls;
    pageList(ls);
    for(unsigned i_l = 0; i_l < ls.size(); i_l++)
	pageAt(ls[i_l]).at().setProcess(val, lastFirstCalc);

    if(!enable()) return;

    //Change self process state
    if(val && !parent().at().parent().freeStat() && (attrAt("pgOpen").at().getB() || attrAt("pgNoOpenProc").at().getB()))
	SessWdg::setProcess(true, lastFirstCalc);
    else if(!val) SessWdg::setProcess(false, lastFirstCalc);
}

AutoHD<Page> SessPage::parent( ) const
{
    if(!enable()) {
	if(parentNm() == "..") return AutoHD<TCntrNode>(nodePrev());
	else return mod->nodeAt(parentNm());
    }
    return Widget::parent();
}

void SessPage::pageAdd( const string &iid, const string &iparent )
{
    if(pagePresent(iid)) return;
    chldAdd(mPage,new SessPage(iid,iparent,ownerSess()));
}

AutoHD<SessPage> SessPage::pageAt( const string &iid ) const	{ return chldAt(mPage,iid); }

AutoHD<Widget> SessPage::wdgAt( const string &wdg, int lev, int off ) const
{
    //Check for global
    if(lev == 0 && off == 0 && wdg.compare(0,1,"/") == 0)
	try { return (AutoHD<Widget>)ownerSess()->nodeAt(wdg,1); }
	catch(TError &err) { return AutoHD<Widget>(); }

    int offt = off;
    string iw = TSYS::pathLev(wdg,lev,true,&offt);
    if(iw.compare(0,3,"pg_") == 0) {
	if(pagePresent(iw.substr(3))) return pageAt(iw.substr(3)).at().wdgAt(wdg, 0, offt);
	else return AutoHD<Widget>();
    }

    return Widget::wdgAt(wdg, lev, off);
}

float SessPage::tmCalcAll( )
{
    float vl = SessWdg::tmCalcAll();
    vector<string> lst;
    pageList(lst);
    for(unsigned iW = 0; iW < lst.size(); iW++)
	if(pageAt(lst[iW]).at().process())
	    vl += pageAt(lst[iW]).at().tmCalcAll();

    return vl;
}

float SessPage::tmCalcMaxAll( )
{
    float vl = SessWdg::tmCalcMaxAll();
    vector<string> lst;
    pageList(lst);
    for(unsigned iW = 0; iW < lst.size(); iW++)
	vl = vmax(vl, pageAt(lst[iW]).at().tmCalcMaxAll());

    return vl;
}

void SessPage::calc( bool first, bool last )
{
    //Process self data
    if(process()) SessWdg::calc(first, last);

    if(mClosePgCom) { mClosePgCom = false; setProcess(false); return; }

    //Put calculate to include pages
    if(!first && !last) {	//Only for ordinal calls by first's and last's calls direct from setProcess() gone.
	vector<string> ls;
	pageList(ls);
	for(unsigned i_l = 0; i_l < ls.size(); i_l++)
	    pageAt(ls[i_l]).at().calc(first, last);
    }
}

bool SessPage::attrChange( Attr &cfg, TVariant prev )
{
    //Page open process
    if(enable() && !prev.isNull()) {
	if(cfg.id() == "pgOpen") {
	    if(cfg.getB()) {
		mClosePgCom = false;
		ownerSess()->openReg(path());	//Moved up for allow access and pages including from "f_start"
		if(!process())	setProcess(true);
	    }
	    else {
		ownerSess()->openUnreg(path());
		if(process() && !attrAt("pgNoOpenProc").at().getB())	mClosePgCom = true;
		if(!attrAt("pgOpenSrc").at().getS().empty()) attrAt("pgOpenSrc").at().setS("");
		pgClose();
	    }
	}
	else if(cfg.id() == "pgOpenSrc") {
	    if(!cfg.getS().empty()) {
		try {
		    AutoHD<SessWdg> src = mod->nodeAt(cfg.getS());

		    //Set interwidget's links for the new page
		    bool emptyPresnt = false;
		    string atrId, prmLnk, sCfgVal;
		    vector<string> cAtrLs;
		    attrList(cAtrLs);
		    for(unsigned iAl = 0; iAl < cAtrLs.size(); iAl++) {
			AutoHD<Attr> attr = attrAt(cAtrLs[iAl]);
			if(!(attr.at().flgSelf()&(Attr::CfgLnkIn|Attr::CfgLnkOut) &&
			      TSYS::strParse(attr.at().cfgTempl(),0,"|") == "<page>")) continue;
			atrId = TSYS::strParse(attr.at().cfgTempl(),1,"|");
			if(src.at().attrPresent(atrId)) {
			    if((sCfgVal=src.at().attrAt(atrId).at().cfgVal()).compare(0,4,"prm:") == 0 &&
				!SYS->daq().at().attrAt(sCfgVal.substr(4),0,true).freeStat())
			    {
				if(prmLnk.empty()) prmLnk = sCfgVal.substr(4);
				attr.at().setCfgVal(sCfgVal);
			    }
			    else attr.at().setCfgVal("wdg:"+cfg.getS()+"/a_"+atrId);
			}
			else {
			    attr.at().setCfgVal("");
			    attr.at().setS(EVAL_STR);
			    emptyPresnt = true;
			}
		    }

		    //Find links into the source if no link founded
		    if(prmLnk.empty()) {
			vector<string> sAtrLs;
			src.at().attrList(sAtrLs);
			for(unsigned iAl = 0; iAl < sAtrLs.size(); iAl++) {
			    AutoHD<Attr> attr = src.at().attrAt(sAtrLs[iAl]);
			    if(attr.at().flgSelf()&(Attr::CfgLnkIn|Attr::CfgLnkOut) &&
				attr.at().cfgVal().size() > 4 && attr.at().cfgVal().substr(0,4) == "prm:" && !SYS->daq().at().attrAt(attr.at().cfgVal().substr(4),0,true).freeStat())
			    {
				prmLnk = attr.at().cfgVal().substr(4);
				break;
			    }
			}
		    }

		    //Fill parameter's links for other attributes
		    if(emptyPresnt && !prmLnk.empty()) {
			size_t aPos = prmLnk.rfind("/");
			if(aPos != string::npos) prmLnk.erase(aPos);
			AutoHD<TValue> prml = SYS->daq().at().prmAt(prmLnk,0,true);
			for(unsigned iAl = 0; !prml.freeStat() && iAl < cAtrLs.size(); iAl++) {
			    AutoHD<Attr> attr = attrAt(cAtrLs[iAl]);
			    if(!(attr.at().flgSelf()&(Attr::CfgLnkIn|Attr::CfgLnkOut) &&
				  TSYS::strSepParse(attr.at().cfgTempl(),0,'|') == "<page>" &&
				  (attr.at().cfgVal().empty() || attr.at().flgGlob()&Attr::Address)))	continue;
			    atrId = TSYS::strSepParse(attr.at().cfgTempl(),1,'|');
			    if(prml.at().vlPresent(atrId))	attr.at().setCfgVal("prm:"+prmLnk+"/"+atrId);
			}
		    }
		} catch(TError &err) { }
	    }
	    if(cfg.owner()->attrAt("pgOpen").at().getB() != !cfg.getS().empty())
		cfg.owner()->attrAt("pgOpen").at().setB(!cfg.getS().empty());
	}
    }

    return SessWdg::attrChange(cfg, prev);
}

void SessPage::alarmSet( bool isSet )
{
    int aStCur  = attrAt("alarmSt").at().getI( );
    string aCur = attrAt("alarm").at().getS( );
    int alev = s2i(TSYS::strSepParse(aCur,0,'|')) & 0xFF;
    int atp  = s2i(TSYS::strSepParse(aCur,3,'|')) & 0xFF;
    int aqtp = isSet ? atp : (aStCur>>16) & 0xFF & atp;

    vector<string> lst;

    //Included pages process
    pageList(lst);
    for(unsigned i_p = 0; i_p < lst.size(); i_p++) {
	if(!pageAt(lst[i_p]).at().enable()) continue;
	int iacur = pageAt(lst[i_p]).at().attrAt("alarmSt").at().getI();
	alev = vmax(alev, iacur&0xFF);
	atp |= (iacur>>8) & 0xFF;
	aqtp |= (iacur>>16) & 0xFF;
    }

    //Included widgets process
    wdgList(lst);
    for(unsigned i_w = 0; i_w < lst.size(); i_w++) {
	if(!wdgAt(lst[i_w]).at().enable()) continue;
	int iacur = wdgAt(lst[i_w]).at().attrAt("alarmSt").at().getI();
	alev = vmax(alev, iacur&0xFF);
	atp |= (iacur>>8) & 0xFF;
	aqtp |= (iacur>>16) & 0xFF;
    }

    attrAt("alarmSt").at().setI((alev && atp) ? (aqtp<<16)|(atp<<8)|alev : 0);

    if(ownerSessWdg(true)) ownerSessWdg(true)->alarmSet();
    if(isSet) ownerSess()->alarmSet(path(), aCur);
}

void SessPage::alarmQuittance( uint8_t quit_tmpl, bool isSet, bool ret )
{
    int alarmSt = attrAt("alarmSt").at().getI();
    if(!ret && !((((alarmSt>>16)&0xFF)^quit_tmpl)&((alarmSt>>16)&0xFF)))return;
    if(ret && !(((~(alarmSt>>16)&0xFF)^quit_tmpl)&((alarmSt>>8)&0xFF)))	return;

    //Self quittance
    if(!ret) attrAt("alarmSt").at().setI(alarmSt & (quit_tmpl<<16|0xFFFF));
    else attrAt("alarmSt").at().setI(alarmSt | (((~quit_tmpl)<<16)&(alarmSt<<8)&0xFF0000));

    vector<string> lst;
    //Included pages quittance
    pageList(lst);
    for(unsigned i_p = 0; i_p < lst.size(); i_p++)
	pageAt(lst[i_p]).at().alarmQuittance(quit_tmpl, false, ret);

    //Include widgets quittance
    wdgList( lst );
    for(unsigned i_w = 0; i_w < lst.size(); i_w++)
	((AutoHD<SessWdg>)wdgAt(lst[i_w])).at().alarmQuittance(quit_tmpl, false, ret);

    if(isSet && ownerSessWdg(true))	ownerSessWdg(true)->alarmSet();
}

bool SessPage::attrPresent( const string &attr ) const
{
    if(!enable() && !mToEn && !mDisMan) const_cast<SessPage*>(this)->setEnable(true, true);
    return Widget::attrPresent(attr);
}

AutoHD<Attr> SessPage::attrAt( const string &attr, int lev ) const
{
    if(lev < 0 && !enable() && !mToEn && !mDisMan) const_cast<SessPage*>(this)->setEnable(true, true);
    return Widget::attrAt(attr, lev);
}

TVariant SessPage::vlGet( Attr &a )
{
    if(a.owner() == this) {
	if(a.id() == "owner") {
	    short perm = attrAt("perm").at().getI(true);
	    if(!(perm&01000)) return a.getS(true);
	    SessPage *oP = ownerPage();
	    return oP ? oP->attrAt("owner").at().getS() : ownerSess()->owner()+":"+ownerSess()->grp();
	}
	else if(a.id() == "perm") {
	    short perm = a.getI(true);
	    if(!(perm&01000)) return perm;
	    SessPage *oP = ownerPage();
	    return (oP?oP->attrAt("perm").at().getI():ownerSess()->permit())|01000;
	}
    }
    return Widget::vlGet(a);
}

TVariant SessPage::stlReq( Attr &a, const TVariant &vl, bool wr )
{
    if(stlLock()) return vl;
    string pid = sTrm(a.cfgTempl());
    if(pid.empty()) pid = a.id();
    if(!wr) return ownerSess()->stlPropGet(pid, vl.getS());
    if(ownerSess()->stlPropSet(pid,vl.getS())) return TVariant();
    return vl;
}

bool SessPage::cntrCmdGeneric( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	SessWdg::cntrCmdGeneric(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Session page: ")+ownerFullId()+"/"+id());
	if(enable() && !(parent().at().prjFlags()&Page::Empty))
	    ctrMkNode("fld",opt,1,"/wdg/st/open",_("Open"),RWRWR_,owner().c_str(),grp().c_str(),1,"tp","bool");
	if(enable() && parent().at().prjFlags()&(Page::Template|Page::Container)) {
	    if(ctrMkNode("area",opt,1,"/page",_("Pages")))
		ctrMkNode("list",opt,-1,"/page/page",_("Pages"),R_R_R_,"root","UI",3,"tp","br","idm","1","br_pref","pg_");
	    if(ctrMkNode("branches",opt,-1,"/br","",R_R_R_))
		ctrMkNode("grp",opt,-1,"/br/pg_",_("Page"),R_R_R_,"root","UI",1,"idm","1");
	}
        return true;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/wdg/st/open" && enable() && !(parent().at().prjFlags()&Page::Empty)) {
	if(ctrChkNode(opt,"get",RWRWR_,owner().c_str(),grp().c_str(),SEC_RD))
	    opt->setText(i2s(attrAt("pgOpen").at().getB()));
	if(ctrChkNode(opt,"set",RWRWR_,owner().c_str(),grp().c_str(),SEC_WR))
	    attrAt("pgOpen").at().setB(s2i(opt->text()));
    }
    else if((a_path == "/br/pg_" || a_path == "/page/page") && ctrChkNode(opt)) {
	vector<string> lst;
	pageList(lst);
	for(unsigned i_f=0; i_f < lst.size(); i_f++)
	    opt->childAdd("el")->setAttr("id",lst[i_f])->setText(pageAt(lst[i_f]).at().name());
    }
    else return SessWdg::cntrCmdGeneric(opt);

    return true;
}

//************************************************
//* SessWdg: Session page's widget               *
//************************************************
SessWdg::SessWdg( const string &iid, const string &iparent, Session *isess ) :
    Widget(iid,iparent), TValFunc(iid+"_wdg",NULL), tmCalc(0), tmCalcMax(0),
    mProc(false), inLnkGet(true), mToEn(false), mMdfClc(0), mCalcClk(isess->calcClk()), mCalcRes(true), mSess(isess)
{
    BACrtHoldOvr = true;
}

SessWdg::~SessWdg( )
{

}

void SessWdg::preDisable( int flag )
{
    if(process()) setProcess(false);

    Widget::preDisable(flag);
}

void SessWdg::postEnable( int flag )
{
    Widget::postEnable(flag);

    if(flag&TCntrNode::NodeConnect) {
	mToEn = true;
	attrAdd(new TFld("event","Events",TFld::String,TFld::FullText));
	attrAdd(new TFld("alarmSt","Alarm status",TFld::Integer,TFld::HexDec,"5","0"));
	attrAdd(new TFld("alarm","Alarm",TFld::String,TFld::NoFlag,"200"));
	mToEn = false;
    }
}

SessWdg *SessWdg::ownerSessWdg( bool base ) const
{
    if(nodePrev(true)) {
	if(!base && dynamic_cast<SessPage*>(nodePrev())) return NULL;
	return dynamic_cast<SessWdg*>(nodePrev());
    }

    return NULL;
}

SessPage *SessWdg::ownerPage( ) const
{
    if(nodePrev(true) && dynamic_cast<SessPage*>(nodePrev()))	return (SessPage*)nodePrev();
    SessWdg *own = ownerSessWdg();
    if(own)	return own->ownerPage();

    return NULL;
}

string SessWdg::path( ) const	{ return ownerFullId(true)+"/wdg_"+id(); }

string SessWdg::ownerFullId( bool contr ) const
{
    SessWdg *ownW = ownerSessWdg();
    if(ownW) return ownW->ownerFullId(contr)+(contr?"/wdg_":"/")+ownW->id();
    SessPage *ownP = ownerPage();
    if(ownP) return ownP->ownerFullId(contr)+(contr?"/pg_":"/")+ownP->id();

    return string(contr?"/ses_":"/")+ownerSess()->id();
}

void SessWdg::setEnable( bool val, bool force )
{
    try { Widget::setEnable(val); } catch(...) { return; }

    if(!val) {
	//Delete included widgets
	vector<string> ls;
	wdgList(ls);
	for(unsigned i_l = 0; i_l < ls.size(); i_l++)
	    wdgDel(ls[i_l]);
    }

    SessWdg *sw;
    if(val && (sw=ownerSessWdg(true)) && sw->process()) {
	setProcess(true);
	sw->prcElListUpdate();
    }
}

void SessWdg::setProcess( bool val, bool lastFirstCalc )
{
    if(val && !enable()) setEnable(true);

    //Prepare process function value level
    bool diff = (val!=process());
    if(val && diff && !sTrm(calcProg()).empty()) {
	// Prepare function io structure
	TFunction fio(parent().at().calcId());
	fio.setStor(calcProgStors());

	//  Add generic io
	fio.ioIns(new IO("f_frq","Function calculate frequency (Hz)",IO::Real,IO::Default,"1000",false), SpIO_Frq);
	fio.ioIns(new IO("f_start","Function start flag",IO::Boolean,IO::Default,"0",false), SpIO_Start);
	fio.ioIns(new IO("f_stop","Function stop flag",IO::Boolean,IO::Default,"0",false), SpIO_Stop);
	fio.ioIns(new IO("this","This widget's object for access to user's API",IO::Object,IO::Default), SpIO_This);

	//  Add calc widget's attributes
	vector<string> iwls, als;
	//   Self attributes check
	attrList(als);
	AutoHD<Widget> fulw = parentNoLink();
	for(unsigned i_a = 0; i_a < als.size(); i_a++) {
	    AutoHD<Attr> cattr = attrAt(als[i_a]);
	    if((fulw.at().attrPresent(als[i_a])&&fulw.at().attrAt(als[i_a]).at().flgSelf()&Attr::ProcAttr) || als[i_a] == "focus")
		fio.ioAdd(new IO(als[i_a].c_str(),cattr.at().name().c_str(),cattr.at().fld().typeIO(),IO::Output,"",false,("./"+als[i_a]).c_str()));
	}

	//   Include attributes check
	wdgList(iwls);
	for(unsigned i_w = 0; i_w < iwls.size(); i_w++) {
	    AutoHD<Widget> curw = wdgAt(iwls[i_w]);
	    curw.at().attrList(als);
	    for(unsigned i_a = 0; i_a < als.size(); i_a++) {
		AutoHD<Attr> cattr = curw.at().attrAt(als[i_a]);
		if(cattr.at().flgSelf()&Attr::ProcAttr || als[i_a] == "focus")
		    fio.ioAdd(new IO((iwls[i_w]+"_"+als[i_a]).c_str(),(curw.at().name()+"."+cattr.at().name()).c_str(),
			cattr.at().fld().typeIO(),IO::Output,"",false,(iwls[i_w]+"/"+als[i_a]).c_str()));
	    }
	}
	fio.ioAdd(new IO("event","Event",IO::String,IO::Output));
	fio.ioAdd(new IO("alarmSt","Alarm status",IO::Integer,IO::Output,"",false,"./alarmSt"));
	fio.ioAdd(new IO("alarm","Alarm",IO::String,IO::Output,"",false,"./alarm"));

	// Compile function
	mWorkProg = "";
	try {
	    mWorkProg = SYS->daq().at().at(TSYS::strSepParse(calcLang(),0,'.')).at().
		compileFunc(TSYS::strSepParse(calcLang(),1,'.'),fio,calcProg(),mod->nodePath('.',true)+";");
	} catch(TError &err) {
	    // Second compile try
	    try {
		fio.setId(TSYS::path2sepstr(path(),'_'));
		mWorkProg = SYS->daq().at().at(TSYS::strSepParse(calcLang(),0,'.')).at().
		    compileFunc(TSYS::strSepParse(calcLang(),1,'.'),fio,calcProg(),mod->nodePath('.',true)+";");
	    } catch(TError &err) {
		mess_err(nodePath().c_str(),_("Compile function '%s' by language '%s' for widget error: %s"),
					    fio.id().c_str(),calcLang().c_str(),err.mess.c_str());
	    }
	}

	// Connect to compiled function
	if(mWorkProg.size()) {
	    TValFunc::setFunc(&((AutoHD<TFunction>)SYS->nodeAt(mWorkProg)).at());
	    TValFunc::setUser(ownerSess()->user());
	    setO(3, new TCntrNodeObj(AutoHD<TCntrNode>(this),ownerSess()->user()));
	}
    }
    if(!val) {
	// Last calc, before any free
	if(diff && lastFirstCalc) calc(false, true);

	// Free function link
	mProc = false;
	MtxAlloc res(mCalcRes, true);
	TValFunc::setFunc(NULL);
    }

    //Change process for included widgets
    vector<string> ls;
    wdgList(ls);
    for(unsigned i_l = 0; i_l < ls.size(); i_l++)
	((AutoHD<SessWdg>)wdgAt(ls[i_l])).at().setProcess(val, false);

    mProc = val;

    // Make process element's lists
    if(val) {
	tmCalc = tmCalcMax = 0;
	prcElListUpdate();
    }

    // First calc, after all set
    if(val && diff && lastFirstCalc) calc(true, false);
}

string SessWdg::ico( ) const		{ return parent().freeStat() ? "" : parent().at().ico(); }

string SessWdg::calcLang( ) const	{ return parent().freeStat() ? "" : parent().at().calcLang(); }

string SessWdg::calcProg( ) const	{ return parent().freeStat() ? "" : parent().at().calcProg(); }

string SessWdg::calcProgStors( const string &attr ){ return parent().freeStat() ? "" : parent().at().calcProgStors(attr); }

int SessWdg::calcPer( ) const		{ return parent().freeStat() ? 0 : parent().at().calcPer(); }

string SessWdg::resourceGet( const string &iid, string *mime )
{
    string  id = TSYS::strParse(iid, 0, "?"),
	    mimeType,
	    mimeData = sessAttr("media://"+id);	//Try load from the session attribute
    if(mimeData.size()) {
	int off = 0;
	mimeType = TSYS::strLine(mimeData, 0, &off);
	if(mime) *mime = mimeType;
	return mimeData.substr(off);
    }

    //Load original
    mimeData = parent().at().resourceGet(id, &mimeType);
    if(mime) *mime = mimeType;

    return mimeData;
}

void SessWdg::resourceSet( const string &id, const string &data, const string &mime )
{
    sessAttrSet("media://"+id, data.empty() ? "" : mime+"\n"+data);
}

void SessWdg::wdgAdd( const string &iid, const string &name, const string &iparent, bool force )
{
    if(!isContainer())	throw TError(nodePath().c_str(),_("Widget is not container!"));
    if(wdgPresent(iid))	return;

    chldAdd(inclWdg, new SessWdg(iid,iparent,ownerSess()));
}

void SessWdg::inheritAttr( const string &aid )
{
    MtxAlloc res(mCalcRes, true);
    Widget::inheritAttr(aid);

    if(enable() && !aid.empty() && ownerSess()->start() && attrPresent(aid)) {
	AutoHD<Attr> attr = attrAt(aid);
	if(!(attr.at().flgGlob()&Attr::IsUser) || attr.at().flgSelf()&Attr::VizerSpec)
	    attr.at().setFlgSelf((Attr::SelfAttrFlgs)(attr.at().flgSelf()|Attr::SessAttrInh));
    }
}

void SessWdg::attrAdd( TFld *attr, int pos, bool inher, bool forceMdf, bool allInher )
{
    Widget::attrAdd(attr, pos, inher, forceMdf || enable(), allInher);
}

AutoHD<Widget> SessWdg::wdgAt( const string &wdg, int lev, int off ) const
{
    //Check for global
    if(lev == 0 && off == 0 && wdg.compare(0,1,"/") == 0)
	try { return (AutoHD<Widget>)ownerSess()->nodeAt(wdg,1); }
	catch(TError &err) { return AutoHD<Widget>(); }

    return Widget::wdgAt(wdg, lev, off);
}

float SessWdg::tmCalcAll( )
{
    float vl = tmCalc;
    vector<string> lst;
    wdgList(lst);
    for(unsigned iW = 0; iW < lst.size(); iW++)
	if(((AutoHD<SessWdg>)wdgAt(lst[iW])).at().process())
	    vl += ((AutoHD<SessWdg>)wdgAt(lst[iW])).at().tmCalcAll();

    return vl;
}

float SessWdg::tmCalcMaxAll( )
{
    float vl = tmCalcMax;
    vector<string> lst;
    wdgList(lst);
    for(unsigned iW = 0; iW < lst.size(); iW++)
	vl = vmax(vl, ((AutoHD<SessWdg>)wdgAt(lst[iW])).at().tmCalcMaxAll());

    return vl;
}

void SessWdg::pgClose( )
{
    try {
	if(!dynamic_cast<SessPage*>(this) && rootId() == "Box" && attrAt("pgGrp").at().getS() != "" && attrAt("pgOpenSrc").at().getS() != "") {
	    ((AutoHD<SessWdg>)mod->nodeAt(attrAt("pgOpenSrc").at().getS())).at().attrAt("pgOpen").at().setB(false);
	    attrAt("pgOpenSrc").at().setS("");
	}
    } catch(TError&) { }

    vector<string> list;
    wdgList(list);
    for(unsigned iW = 0; iW < list.size(); iW++)
	((AutoHD<SessWdg>)wdgAt(list[iW])).at().pgClose();
}

string SessWdg::sessAttr( const string &id, bool onlyAllow )
{
    int off = 0;
    TSYS::pathLev(path(), 0, true, &off);
    return ownerSess()->sessAttr(path().substr(off), id, onlyAllow);
}

void SessWdg::sessAttrSet( const string &id, const string &val )
{
    int off = 0;
    TSYS::pathLev(path(), 0, true, &off);
    ownerSess()->sessAttrSet(path().substr(off), id, val);
}

void SessWdg::eventAdd( const string &ev )
{
    if(!enable() || !attrPresent("event")) return;
    ownerSess()->dataRes().lock();
    attrAt("event").at().setS(attrAt("event").at().getS()+ev);
    ownerSess()->dataRes().unlock();
}

string SessWdg::eventGet( bool clear )
{
    if(!enable() || !attrPresent("event")) return "";

    ownerSess()->dataRes().lock();
    string rez = attrAt("event").at().getS();
    if(clear)	attrAt("event").at().setS("");
    ownerSess()->dataRes().unlock();

    return rez;
}

void SessWdg::alarmSet( bool isSet )
{
    int aStCur  = attrAt("alarmSt").at().getI( );
    string aCur = attrAt("alarm").at().getS( );
    int alev = s2i(TSYS::strSepParse(aCur,0,'|')) & 0xFF;
    int atp  = s2i(TSYS::strSepParse(aCur,3,'|')) & 0xFF;
    int aqtp = isSet ? atp : (aStCur>>16) & 0xFF;

    vector<string> lst;

    //Included widgets process
    wdgList(lst);
    for(unsigned iW = 0; iW < lst.size(); iW++) {
	int iacur = wdgAt(lst[iW]).at().attrAt("alarmSt").at().getI();
	alev = vmax(alev, iacur&0xFF);
	atp |= (iacur>>8) & 0xFF;
	aqtp |= (iacur>>16) & 0xFF;
    }

    attrAt("alarmSt").at().setI((alev && atp) ? (aqtp<<16)|(atp<<8)|alev : 0);

    if(ownerSessWdg(true)) ownerSessWdg(true)->alarmSet();
    if(isSet) ownerSess()->alarmSet(path(), aCur);
}

void SessWdg::alarmQuittance( uint8_t quit_tmpl, bool isSet, bool ret )
{
    int alarmSt = attrAt("alarmSt").at().getI();
    if(!ret && !((((alarmSt>>16)&0xFF)^quit_tmpl)&((alarmSt>>16)&0xFF)))return;
    if(ret && !(((~(alarmSt>>16)&0xFF)^quit_tmpl)&((alarmSt>>8)&0xFF)))	return;

    //Self quittance
    if(!ret) attrAt("alarmSt").at().setI(alarmSt & (quit_tmpl<<16|0xFFFF));
    else attrAt("alarmSt").at().setI(alarmSt | (((~quit_tmpl)<<16)&(alarmSt<<8)&0xFF0000));

    vector<string> lst;
    //Include widgets quittance
    wdgList(lst);
    for(unsigned iW = 0; iW < lst.size(); iW++)
	((AutoHD<SessWdg>)wdgAt(lst[iW])).at().alarmQuittance(quit_tmpl, false, ret);

    if(isSet && ownerSessWdg(true)) ownerSessWdg(true)->alarmSet();
}

void SessWdg::prcElListUpdate( )
{
    vector<string> ls;

    wdgList(ls);
    MtxAlloc resDt(ownerSess()->dataRes(), true);
    mWdgChldAct.clear();
    for(unsigned i_l = 0; i_l < ls.size(); i_l++)
	try { if(((AutoHD<SessWdg>)wdgAt(ls[i_l])).at().process()) mWdgChldAct.push_back(ls[i_l]); }
	catch(TError &err) { }
    resDt.unlock();

    attrList(ls);
    mAttrLnkLs.clear();
    for(unsigned i_a = 0; i_a < ls.size(); i_a++) {
	AutoHD<Attr> attr = attrAt(ls[i_a]);
	if(attr.at().flgSelf()&(Attr::CfgConst|Attr::CfgLnkIn|Attr::CfgLnkOut))
	    mAttrLnkLs.push_back(ls[i_a]);
    }
}

void SessWdg::getUpdtWdg( const string &ipath, unsigned int tm, vector<string> &els )
{
    string wpath = ipath + "/" + id();
    if(modifChk(tm,mMdfClc)) els.push_back(wpath);

    MtxAlloc resDt(ownerSess()->dataRes(), true);
    for(unsigned i_ch = 0; i_ch < mWdgChldAct.size(); i_ch++)
	try {
	    AutoHD<SessWdg> wdg = wdgAt(mWdgChldAct[i_ch]);
	    resDt.unlock();
	    wdg.at().getUpdtWdg(wpath, tm, els);
	    resDt.lock();
	} catch(TError &err) { }
}

unsigned int SessWdg::modifVal( Attr &cfg )
{
    if(s2i(cfg.fld().reserve()) || cfg.flgSelf()&Attr::VizerSpec) mMdfClc = mCalcClk;

    return mCalcClk;
}

bool SessWdg::modifChk( unsigned int tm, unsigned int iMdfClc )
{
    return (mCalcClk>=tm) ? (iMdfClc >= tm && iMdfClc <= mCalcClk) : (iMdfClc >= tm || iMdfClc <= mCalcClk);
}

void SessWdg::calc( bool first, bool last )
{
    if(!process()) return;

    MtxAlloc res(mCalcRes, true);

    string sw_attr, s_attr, obj_tp;

//    if( !(ownerSess()->calcClk()%vmax(1,10000/ownerSess()->period())) ) prcElListUpdate( );

    //Calculate include widgets
    MtxAlloc resDt(ownerSess()->dataRes(), true);
    for(unsigned i_l = 0; i_l < mWdgChldAct.size(); i_l++)
	try {
	    AutoHD<SessWdg> wdg = wdgAt(mWdgChldAct[i_l]);
	    resDt.unlock();
	    wdg.at().calc(first, last);
	    resDt.lock();
	} catch(TError &err) { }
    resDt.unlock();

    try {
	int pgOpenPrc = -1;
	int64_t tCnt;

	if(mess_lev() == TMess::Debug) tCnt = TSYS::curTime();

	//Load events to process
	if(!((ownerSess()->calcClk())%(vmax(calcPer()/ownerSess()->period(),1))) || first || last) {
	    string wevent = eventGet(true);
	    //Process input links and constants
	    AutoHD<Attr> attr;
	    AutoHD<TVal> vl;
	    inLnkGet = true;
	    for(unsigned i_a = 0; i_a < mAttrLnkLs.size(); i_a++) {
		attr = attrAt(mAttrLnkLs[i_a]);
		if(attr.at().flgSelf()&Attr::CfgConst && !attr.at().cfgVal().empty())	attr.at().setS(attr.at().cfgVal());
		else if(attr.at().flgSelf()&Attr::CfgLnkIn && !attr.at().cfgVal().empty()) {
		    obj_tp = TSYS::strSepParse(attr.at().cfgVal(),0,':') + ":";
		    if(obj_tp == "val:")	attr.at().setS(attr.at().cfgVal().substr(obj_tp.size()));
		    else if(obj_tp == "prm:") {
			int detOff = obj_tp.size();	//Links subdetail process
			vl = SYS->daq().at().attrAt(TSYS::strParse(attr.at().cfgVal(),0,"#",&detOff),0,true);
			if(vl.freeStat()) { attr.at().setS(EVAL_STR); continue; }
			if(attr.at().flgGlob()&Attr::Address) {
			    string nP = vl.at().nodePath(0,true);
			    attr.at().setS((nP.size()&&nP[nP.size()-1]=='/')?nP.substr(0,nP.size()-1):"");// "/DAQ"+attr.at().cfgVal().substr(obj_tp.size()));
			}
			else if(vl.at().fld().type() == TFld::Object && detOff < (int)attr.at().cfgVal().size())
			    attr.at().set(vl.at().getO().at().propGet(attr.at().cfgVal().substr(detOff),0));
			else attr.at().set(vl.at().get());

			/*vl = SYS->daq().at().attrAt(attr.at().cfgVal().substr(obj_tp.size()),0,true);
			if(vl.freeStat()) { attr.at().setS(EVAL_STR); continue; }

			if(attr.at().flgGlob()&Attr::Address) {
			    string nP = vl.at().nodePath(0,true);
			    attr.at().setS((nP.size()&&nP[nP.size()-1]=='/')?nP.substr(0,nP.size()-1):"");// "/DAQ"+attr.at().cfgVal().substr(obj_tp.size()));
			}
			else attr.at().set(vl.at().get());*/
		    }
		    else if(obj_tp == "wdg:")
			try { attr.at().set(attrAt(attr.at().cfgVal().substr(obj_tp.size()),0).at().get()); }
			catch(TError &err) { attr.at().setS(EVAL_STR); continue; }
		    else if(obj_tp == "arh:" && attr.at().flgGlob()&Attr::Address)
			attr.at().setS("/Archive/va_"+attr.at().cfgVal().substr(obj_tp.size()));
		}
		/*else if(attr.at().flgSelf()&Attr::CfgLnkOut) {
		    obj_tp = TSYS::strSepParse(attr.at().cfgVal(),0,':') + ":";
		    if(!attr.at().cfgVal().size() ||
			    (obj_tp == "prm:" && SYS->daq().at().attrAt(TSYS::strParse(attr.at().cfgVal().substr(obj_tp.size()),0,"#"),0,true).freeStat()) ||
			    (obj_tp == "wdg:" && attrAt(attr.at().cfgVal().substr(obj_tp.size()),0).freeStat()))
			attr.at().setS(EVAL_STR, false, true);
		}*/
		else if(attr.at().flgSelf()&Attr::CfgLnkIn) attr.at().setS(EVAL_STR);
		attr.free();
	    }
	    inLnkGet = false;

	    if(TValFunc::func()) {
		//Load events to calc procedure
		int evId = ioId("event");
		if(evId >= 0)	setS(evId, wevent);

		// Load the data to the calc area
		setR(SpIO_Frq, 1000.0/(ownerSess()->period()*vmax(calcPer()/ownerSess()->period(),1)));
		setB(SpIO_Start, first);
		setB(SpIO_Stop, last);
		for(int i_io = SpIO_Sz; i_io < ioSize(); i_io++) {
		    if(func()->io(i_io)->rez().empty()) continue;
		    sw_attr = TSYS::pathLev(func()->io(i_io)->rez(), 0);
		    s_attr  = TSYS::pathLev(func()->io(i_io)->rez(), 1);
		    attr = (sw_attr==".") ? attrAt(s_attr) : wdgAt(sw_attr).at().attrAt(s_attr);
		    set(i_io, attr.at().get());
		}

		// Calc
		setMdfChk(true);
		TValFunc::calc();

		// Save the data from the calc area
		for(int i_io = SpIO_Sz; i_io < ioSize(); i_io++) {
		    if(func()->io(i_io)->rez().empty() || !ioMdf(i_io)) continue;
		    sw_attr = TSYS::pathLev(func()->io(i_io)->rez(), 0);
		    s_attr  = TSYS::pathLev(func()->io(i_io)->rez(), 1);
		    attr = (sw_attr==".") ? attrAt(s_attr) : wdgAt(sw_attr).at().attrAt(s_attr);

		    if(s_attr == "pgOpen" && attr.at().getB() != getB(i_io)) { pgOpenPrc = i_io; continue; }
		    attr.at().set(get(i_io));
		}
		// Save events from calc procedure
		if(evId >= 0) wevent = getS(evId);
	    }

	    res.unlock();

	    //Close page process by pgOpen changing
	    if(pgOpenPrc >= 0) attrAt("pgOpen").at().setB(getB(pgOpenPrc));

	    // Process widget's events
	    if(!wevent.empty()) {
		int t_off;
		bool isPg = dynamic_cast<SessPage*>(this);
		string sevup, sev, sev_ev, sev_path, sprc_lst, sprc, sprc_ev, sprc_path;
		for(int el_off = 0; (sev=TSYS::strSepParse(wevent,0,'\n',&el_off)).size(); ) {
		    // Check for process events
		    t_off = 0;
		    sev_ev   = TSYS::strSepParse(sev, 0, ':', &t_off);
		    sev_path = TSYS::strSepParse(sev, 0, ':', &t_off);

		    if(sev_path.empty() && eventProc(sev_ev))	continue;	//Try local events process by the root widget

		    sprc_lst = attrAt("evProc").at().getS();
		    bool evProc = false;
		    for(int elp_off = 0; (sprc=TSYS::strSepParse(sprc_lst,0,'\n',&elp_off)).size(); ) {
			t_off = 0;
			sprc_ev   = TSYS::strSepParse(sprc, 0, ':', &t_off);
			sprc_path = TSYS::strSepParse(sprc, 0, ':', &t_off);
			if(sprc_ev == sev_ev && (sprc_path == "*" || sprc_path == sev_path)) {
			    sprc_path = TSYS::strSepParse(sprc, 0, ':', &t_off);
			    SessWdg *sev = this;
			    if(!sev_path.empty()) sev = (TSYS::pathLev(sev_path,0).compare(0,4,"ses_") == 0) ?
					    &((AutoHD<SessWdg>)mod->nodeAt(sev_path)).at() :  &((AutoHD<SessWdg>)nodeAt(sev_path)).at();
			    ownerSess()->uiComm(sprc_path, TSYS::strSepParse(sprc,0,':',&t_off), sev);
			    evProc = true;
			}
		    }
		    if(!evProc) {
			if(!isPg) sevup += sev_ev + ":/" + id() + sev_path + "\n";
			else sevup += sev_ev + ":" + (TSYS::pathLev(sev_path,0).compare(0,4,"ses_")?path():"") + sev_path + "\n";
		    }
		}
		// Put left events to parent widget
		SessWdg *owner = ownerSessWdg(true);
		if(owner && !sevup.empty()) {
		    if(!isPg) owner->eventAdd(sevup);
		    // Up event to the upper page
		    else {
			vector<string> lst = ownerSess()->openList();
			string prev;
			for(unsigned i_f = 0; i_f < lst.size(); i_f++)
			    if(lst[i_f] == path()) {
				if(prev.size()) ((AutoHD<SessPage>)mod->nodeAt(prev)).at().eventAdd(sevup);
				break;
			    }
			    else prev = lst[i_f];
		    }
		}
	    }

	    //Generic calc
	    Widget::calc(this);

	    if(mess_lev() == TMess::Debug) { tmCalc = 1e-6*(TSYS::curTime()-tCnt); tmCalcMax = vmax(tmCalcMax, tmCalc); }
	}
    } catch(TError &err) {
	res.unlock();
	mess_err(err.cat.c_str(), err.mess.c_str());
	mess_err(nodePath().c_str(), _("Widget calculation error. Process is disabled."));
	if(!last) setProcess(false);
    }
}

bool SessWdg::attrChange( Attr &cfg, TVariant prev )
{
    Widget::attrChange(cfg, prev);

    //Special session attributes process
    // Focus attribute process for active active
    if(cfg.id() == "active" && cfg.getB() && !cfg.owner()->attrPresent("focus"))
	cfg.owner()->attrAdd(new TFld("focus","Focus",TFld::Boolean,TFld::NoFlag,"1","false","","","-2"));
    //Alarm event for widget process
    else if(cfg.id() == "alarm" && enable() && !prev.isNull()) alarmSet(true);
    //Alarm status process
    else if(cfg.id() == "alarmSt" && cfg.getI()&0x1000000) {
	int tmpl = ~(cfg.getI()&0xFF);
	cfg.setI(prev.getI(), false, true);
	ownerSess()->alarmQuittance(path(), tmpl, cfg.getI()&0x2000000);
    }

    //External link process
    if(!inLnkGet && !prev.isNull() && cfg.flgSelf()&Attr::CfgLnkOut && !cfg.cfgVal().empty()) {
	if(cfg.flgSelf()&Attr::SessAttrInh) cfg.setFlgSelf((Attr::SelfAttrFlgs)(cfg.flgSelf()&(~Attr::SessAttrInh)));
	string obj_tp = TSYS::strSepParse(cfg.cfgVal(),0,':') + ":";
	try {
	    if(obj_tp == "prm:") {
		int detOff = obj_tp.size();	//Links subdetail process
		AutoHD<TVal> vl = SYS->daq().at().attrAt(TSYS::strParse(cfg.cfgVal(),0,"#",&detOff));
		if(vl.at().fld().type() == TFld::Object && detOff < (int)cfg.cfgVal().size()) {
		    vl.at().getO().at().propSet(cfg.cfgVal().substr(detOff),0,cfg.get());
		    vl.at().setO(vl.at().getO());	//For modify object sign
		}
		else vl.at().set(cfg.get());

		//SYS->daq().at().attrAt(cfg.cfgVal().substr(obj_tp.size()),0,true).at().set(cfg.get());
	    }
	    else if(obj_tp == "wdg:")	attrAt(cfg.cfgVal().substr(obj_tp.size()),0).at().set(cfg.get());
	} catch(...)	{ }
    }

    return true;
}

TVariant SessWdg::objFuncCall( const string &iid, vector<TVariant> &prms, const string &user )
{
    // TCntrNodeObj ownerSess( ) - Get session object
    if(iid == "ownerSess")	return new TCntrNodeObj(ownerSess(),user);
    // TCntrNodeObj ownerPage( ) - Get page-owner object
    if(iid == "ownerPage") {
	SessPage *opg = ownerPage();
	if(!opg) return 0;
	return new TCntrNodeObj(opg,user);
    }
    // TCntrNodeObj ownerWdg(bool base) - Get widget-owner object
    //  base - include widgets and pages for true
    if(iid == "ownerWdg") {
	SessWdg *wdg = ownerSessWdg(prms.size() ? prms[0].getB() : 0);
	if(!wdg) return 0;
	return new TCntrNodeObj(wdg, user);
    }
    // TCntrNodeObj wdgAdd(string wid, string wname, string parent) - add new widget
    //  wid - widget identifier
    //  wname - widget name
    //  parent - parent widget
    if(iid == "wdgAdd" && prms.size() >= 3) {
	try {
	    //Create widget
	    wdgAdd(prms[0].getS(), prms[1].getS(), prms[2].getS());
	    //Enable widget
	    AutoHD<SessWdg> nw = wdgAt(prms[0].getS());
	    nw.at().setEnable(true);

	    return new TCntrNodeObj(&nw.at(), user);
	} catch(TError &err) { return false; }
    }
    // bool wdgDel(string wid) - delete the widget, return true for success
    //  wid - widget identifier
    if(iid == "wdgDel" && prms.size()) {
	try { wdgDel(prms[0].getS()); }
	catch(TError &err){ return false; }
	return true;
    }
    // TCntrNodeObj wdgAt(string wid, bool byPath = false) - attach to the child widget or global by <path>
    //  wid - widget identifier
    //  byPath - attach by absolute or relative path. First item of absolute path (session or project id) is passed.
    if(iid == "wdgAt" && prms.size()) {
	try { return new TCntrNodeObj(wdgAt(prms[0].getS(),(prms.size()>1&&prms[1].getB())?0:-1),user); }
	catch(TError &err) { }
	return false;
    }
    // bool attrPresent(string attr) - check for attribute <attr> present.
    //  attr - checked attribute
    if(iid == "attrPresent" && prms.size())	return attrPresent(prms[0].getS());
    // ElTp attr(string attr, bool fromSess = false) - get attribute <attr> value or from the session table <fromSess>.
    //  attr - readed attribute;
    //  fromSess - read attribute from session table.
    if(iid == "attr" && prms.size()) {
	if(prms.size() > 1 && prms[1].getB())	return sessAttr(prms[0].getS());
	else if(attrPresent(prms[0].getS()))	return attrAt(prms[0].getS()).at().get();
	return string("");
    }
    // TCntrNodeObj attrSet(string attr, ElTp vl, bool toSess = false) - set attribute <attr> to value <vl> or to the session table <toSess>.
    //  attr - writed attribute;
    //  vl - value;
    //  toSess - write to session table.
    if(iid == "attrSet" && prms.size() >= 2) {
	if(prms.size() > 2 && prms[2].getB()) sessAttrSet(prms[0].getS(), prms[1].getS());
	else if(attrPresent(prms[0].getS())) attrAt(prms[0].getS()).at().set(prms[1]);

	return new TCntrNodeObj(this, user);
    }
    // string link(string attr, bool prm = false) - get link for attribute or attribute block (prm)
    //  attr - attribute identifier
    //  prm  - attribute block for true
    if(iid == "link" && prms.size()) {
	XMLNode req("get");
	req.setAttr("user", "root");
	if(prms.size() >= 2 && prms[1].getB()) req.setAttr("path", TSYS::strMess("/links/lnk/pr_%s",prms[0].getS().c_str()));
	else req.setAttr("path", TSYS::strMess("/links/lnk/el_%s",prms[0].getS().c_str()));
	if(cntrCmdLinks(&req)) return req.text();
	return "";
    }
    // string linkSet(string attr, string vl, bool prm = false) - set link for attribute or attribute block (prm) to vl
    //  attr - attribute identifier
    //  vl   - link value
    //  prm  - attribute block for true
    if(iid == "linkSet" && prms.size() >= 2) {
	XMLNode req("set");
	req.setAttr("user", "root")->setText(prms[1].getS());
	if(prms.size() >= 3 && prms[2].getB()) req.setAttr("path", TSYS::strMess("/links/lnk/pr_%s",prms[0].getS().c_str()));
	else req.setAttr("path", TSYS::strMess("/links/lnk/el_%s",prms[0].getS().c_str()));
	return cntrCmdLinks(&req);
    }
    // string mime(string addr, string type = "") - read mime data from the session table or primal source
    //  addr - address to mime by link attribute to mime or direct mime address
    //  type - return attribute for mime type store
    if(iid == "mime" && prms.size()) {
	string addr = prms[0], rez, tp;
	//Check for likely attribute
	if(attrPresent(addr)) {
	    AutoHD<Attr> a = attrAt(addr);
	    if(a.at().type() == TFld::String /*&& a.at().flgGlob()&Attr::Image*/) addr = a.at().getS();
	}
	rez = resourceGet(addr, &tp);
	if(prms.size() >= 2) { prms[1].setS(tp); prms[1].setModify(); }

	return TSYS::strDecode(rez, TSYS::base64);
    }
    // int mimeSet(string addr, string data, string type = "") - set or clear data to the session table
    //  addr - address to mime by link attribute to mime or direct mime address
    //  data - set to the mime data, empty for clear into
    //  type - mime type for store data
    if(iid == "mimeSet" && prms.size() >= 2) {
	string addr = prms[0];
	//Check for likely attribute
	AutoHD<Attr> a;
	if(attrPresent(addr)) {
	    a = attrAt(addr);
	    if(a.at().type() == TFld::String /*&& a.at().flgGlob()&Attr::Image*/) addr = a.at().getS();
	    else a.free();
	}
	resourceSet(TSYS::strParse(addr,0,"?"), TSYS::strEncode(prms[1],TSYS::base64), (prms.size()>=3)?prms[2]:"");
	if(!a.freeStat()) a.at().setS(TSYS::strParse(addr,1,"?").size()?TSYS::strLabEnum(addr):addr+"?0");	//Mark the attribute to modify state

	return (int)prms[1].getS().size();
    }
    // int mess{Debug,Info,Note,Warning,Err,Crit,Alert,Emerg} -
    //		formation of the system message <mess> with the category by the widget path and the appropriate level
    //  mess - message text
    if(iid == "messDebug" && prms.size())	{ mess_debug(nodePath().c_str(), "%s", prms[0].getS().c_str()); return 0; }
    if(iid == "messInfo" && prms.size())	{ mess_info(nodePath().c_str(), "%s", prms[0].getS().c_str()); return 0; }
    if(iid == "messNote" && prms.size())	{ mess_note(nodePath().c_str(), "%s", prms[0].getS().c_str()); return 0; }
    if(iid == "messWarning" && prms.size())	{ mess_warning(nodePath().c_str(), "%s", prms[0].getS().c_str()); return 0; }
    if(iid == "messErr" && prms.size())		{ mess_err(nodePath().c_str(), "%s", prms[0].getS().c_str()); return 0; }
    if(iid == "messCrit" && prms.size())	{ mess_crit(nodePath().c_str(), "%s", prms[0].getS().c_str()); return 0; }
    if(iid == "messAlert" && prms.size())	{ mess_alert(nodePath().c_str(), "%s", prms[0].getS().c_str()); return 0; }
    if(iid == "messEmerg" && prms.size())	{ mess_emerg(nodePath().c_str(), "%s", prms[0].getS().c_str()); return 0; }

    //Request to primitive
    TVariant rez = objFuncCall_w(iid, prms, user, this);
    if(!rez.isNull())	return rez;

    return TCntrNode::objFuncCall(iid, prms, user);
}

bool SessWdg::cntrCmdServ( XMLNode *opt )
{
    string a_path = opt->attr("path"), u = opt->attr("user");
    if(a_path == "/serv/attr") {	//Attribute's value operations
	if(ctrChkNode(opt,"get",R_R_R_,"root","UI",SEC_RD)) {	//Get values
	    unsigned tm = s2ll(opt->attr("tm"));
	    if(!tm) {
		opt->childAdd("el")->setAttr("id","perm")->setAttr("p",i2s(A_PERM))->
		    setText(i2s(ownerSess()->sec.at().access(u,SEC_RD|SEC_WR,owner(),grp(),permit())));
		if(dynamic_cast<SessPage*>(this)) opt->childAdd("el")->setAttr("id", "name")->setAttr("p", i2s(A_PG_NAME))->setText(name());
	    }
	    if(!tm || modifChk(tm,mMdfClc)) {
		AutoHD<Attr> attr;
		vector<string> als;
		attrList(als);
		for(unsigned i_l = 0; i_l < als.size(); i_l++) {
		    attr = attrAt(als[i_l]);
		    if(((!(attr.at().flgGlob()&Attr::IsUser) && s2i(attr.at().fld().reserve())) || attr.at().flgSelf()&Attr::VizerSpec) &&
			    modifChk(tm,attr.at().modif()))
			opt->childAdd("el")->setAttr("id", als[i_l].c_str())->
					     setAttr("p", attr.at().fld().reserve())->
					     setText(attr.at().isTransl()?trU(attr.at().getS(),u):attr.at().getS());
		}
	    }
	}
	else if(ctrChkNode(opt,"set",permit(),owner().c_str(),grp().c_str(),SEC_WR)) {	//Set values
	    if(ownerSess()->user() != u) ownerSess()->setUser(u);
	    for(unsigned i_ch = 0; i_ch < opt->childSize(); i_ch++) {
		XMLNode *aN = opt->childGet(i_ch);
		string aid = aN->attr("id");
		if(aid == "event") eventAdd(aN->text()+"\n");
		else attrAt(aid).at().setS(aN->text());
	    }
	}
    }
    else if(a_path == "/serv/attrBr" && ctrChkNode(opt,"get",R_R_R_,"root","UI",SEC_RD)) {	//Get attributes all updated elements' of the branch
	unsigned tm = strtoul(opt->attr("tm").c_str(), NULL, 10);
	bool     fullTree = s2i(opt->attr("FullTree"));
	int perm = ownerSess()->sec.at().access(u,(tm?SEC_RD:SEC_RD|SEC_WR),owner(),grp(),permit());

	//Self attributes put
	if(!tm || modifChk(tm,mMdfClc)) {
	    if(!tm) {
		if(dynamic_cast<SessPage*>(this)) opt->childAdd("el")->setAttr("id","name")->setAttr("p",i2s(A_PG_NAME))->setText(name());
		opt->childAdd("el")->setAttr("id","perm")->setAttr("p",i2s(A_PERM))->setText(i2s(perm));
	    }
	    AutoHD<Attr> attr;
	    vector<string> als;
	    attrList(als);
	    for(unsigned i_l = 0; i_l < als.size(); i_l++) {
		attr = attrAt(als[i_l]);
		if(((!(attr.at().flgGlob()&Attr::IsUser) && s2i(attr.at().fld().reserve())) || attr.at().flgSelf()&Attr::VizerSpec) &&
			modifChk(tm,attr.at().modif()))
		    opt->childAdd("el")->setAttr("id", als[i_l].c_str())->
				     setAttr("p", attr.at().fld().reserve())->
				     setText(attr.at().isTransl()?trU(attr.at().getS(),u):attr.at().getS());
	    }
	}

	//Child widgets process
	if(enable() && (perm&SEC_RD)) {
	    vector<string>	lst;
	    wdgList(lst);

	    for(unsigned i_f = 0; i_f < lst.size(); i_f++) {
		AutoHD<SessWdg> iwdg = wdgAt(lst[i_f]);
		XMLNode *wn = new XMLNode("get");
		wn->setAttr("path", a_path)->setAttr("user", u)->setAttr("tm", opt->attr("tm"))->setAttr("FullTree", opt->attr("FullTree"));
		iwdg.at().cntrCmdServ(wn);
		if(wn->childSize() || fullTree) {
		    wn->setName("w")->attrDel("path")->attrDel("user")->
			attrDel("rez")->attrDel("tm")->attrDel("FullTree")->setAttr("id", lst[i_f]);
		    opt->childAdd(wn);
		}
		else delete wn;
	    }
	}
    }
    else if(a_path.compare(0,15,"/serv/attrSess/") == 0) {	//Session attribute's value operations
	if(ctrChkNode(opt,"get",R_R_R_,"root","UI",SEC_RD))	opt->setText(sessAttr(TSYS::pathLev(a_path,2)));
	else if(ctrChkNode(opt,"set",permit(),owner().c_str(),grp().c_str(),SEC_WR))	sessAttrSet(TSYS::pathLev(a_path,2), opt->text());
    }
    else if(a_path.compare(0,11,"/serv/attr/") == 0 && ctrChkNode(opt,"activate")) {
	string tStr = TSYS::pathLev(a_path, 2);
	//Visualizer specific attributes creation at the request
	if(!attrPresent(tStr) && opt->attr("aNm").size()) {
	    parent().at().attrAdd(new TFld(tStr.c_str(),opt->attr("aNm").c_str(),(TFld::Type)s2i(opt->attr("aTp")),
			s2i(opt->attr("aFlg"))|Attr::IsUser,"","",opt->attr("aVls").c_str(),opt->attr("aNms").c_str()));
	    parent().at().attrAt(tStr).at().setModif(1);
	    parent().at().modif();
	}
	if(parent().at().attrPresent(tStr))
	    parent().at().attrAt(tStr).at().setFlgSelf((Attr::SelfAttrFlgs)(parent().at().attrAt(tStr).at().flgSelf()|Attr::VizerSpec));
	if(attrPresent(tStr)) {
	    attrAt(tStr).at().setFlgSelf((Attr::SelfAttrFlgs)(attrAt(tStr).at().flgSelf()|Attr::VizerSpec));
	    attrAt(tStr).at().setModif(modifVal(attrAt(tStr).at()));	//Force set modify for allow load next
	}
    }
    else return Widget::cntrCmdServ(opt);

    return true;
}

bool SessWdg::cntrCmdGeneric( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	Widget::cntrCmdGeneric(opt);
	ctrMkNode("fld",opt,1,"/wdg/st/proc",_("Process"),RWRWR_,owner().c_str(),grp().c_str(),1,"tp","bool");
	if(mess_lev() == TMess::Debug)
	    ctrMkNode("fld",opt,1,"/wdg/st/tmSpent",_("Spent time"),R_R_R_,owner().c_str(),grp().c_str(),1,"tp","str");
	return true;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/wdg/st/proc") {
	if(ctrChkNode(opt,"get",RWRWR_,owner().c_str(),grp().c_str(),SEC_RD)) opt->setText(i2s(process()));
	if(ctrChkNode(opt,"set",RWRWR_,owner().c_str(),grp().c_str(),SEC_WR)) setProcess(s2i(opt->text()));
    }
    else if(a_path == "/wdg/st/tmSpent" && ctrChkNode(opt,"get"))
	opt->setText(_("Subtree=")+tm2s(tmCalcAll())+"["+tm2s(tmCalcMaxAll())+"], "+_("Item=")+tm2s(tmCalc)+"["+tm2s(tmCalcMax)+"]");
    else return Widget::cntrCmdGeneric(opt);

    return true;
}

bool SessWdg::cntrCmdAttributes( XMLNode *opt, Widget *src )
{
    //Get page info
    if(opt->name() == "info") {
	Widget::cntrCmdAttributes(opt);
	if(ctrMkNode("area",opt,-1,"/attr",_("Attributes"))) {
	    // Properties form create
	    vector<string> list_a;
	    attrList(list_a);
	    for(unsigned iEl = 0; iEl < list_a.size(); iEl++) {
		XMLNode *el = attrAt(list_a[iEl]).at().fld().cntrCmdMake(opt,"/attr",-1,owner().c_str(),grp().c_str(),permit()|R_R_R_);
		if(el) el->setAttr("len", "")->setAttr("wdgFlg", i2s(attrAt(list_a[iEl]).at().flgGlob()));
	    }
	}
	return true;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path.compare(0,6,"/attr/") == 0) {
	AutoHD<Attr> attr = attrAt(TSYS::pathLev(a_path,1));
	if(ctrChkNode(opt,"get",((attr.at().fld().flg()&TFld::NoWrite)?(permit()&~0222):permit())|R_R_R_,owner().c_str(),grp().c_str(),SEC_RD))
	    opt->setText(attr.at().isTransl()?trU(attr.at().getS(),opt->attr("user")):attr.at().getS());
	else if(ctrChkNode(opt,"set",((attr.at().fld().flg()&TFld::NoWrite)?(permit()&~0222):permit())|R_R_R_,owner().c_str(),grp().c_str(),SEC_WR))
	{
	    if(attr.at().id() == "event")	eventAdd(opt->text()+"\n");
	    else				attr.at().setS(opt->text());
	}
	else return Widget::cntrCmdAttributes(opt);
    }
    else return Widget::cntrCmdAttributes(opt);

    return true;
}

void SessWdg::cntrCmdProc( XMLNode *opt )
{
    if(cntrCmdServ(opt)) return;

    //Get page info
    if(opt->name() == "info") {
	cntrCmdGeneric(opt);
	cntrCmdAttributes(opt);
	if(!parent().freeStat()) cntrCmdLinks(opt,true);
	return;
    }
    if(!(cntrCmdGeneric(opt) || cntrCmdAttributes(opt) || (parent().freeStat() ? false : cntrCmdLinks(opt))))
	TCntrNode::cntrCmdProc(opt);
}
