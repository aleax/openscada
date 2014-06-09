
//OpenSCADA system module UI.VCAEngine file: session.cpp
/***************************************************************************
 *   Copyright (C) 2007-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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

#include <pthread.h>
#include <signal.h>

#include <tsys.h>

#include "vcaengine.h"
#include "session.h"

using namespace VCA;

//************************************************
//* Session: Project's session			 *
//************************************************
Session::Session( const string &iid, const string &iproj ) :
    mId(iid), mPrjnm(iproj), mOwner("root"), mGrp("UI"), mUser(dataM), mPer(100), mPermit(RWRWR_), mEnable(false), mStart(false),
    endrun_req(false), mBackgrnd(false), mConnects(0), mCalcClk(1), mAlrmSndPlay(-1), mStyleIdW(-1)
{
    pthread_mutexattr_t attrM;
    pthread_mutexattr_init(&attrM);
    pthread_mutexattr_settype(&attrM, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&dataM, &attrM);
    pthread_mutex_init(&mCalcRes, &attrM);
    pthread_mutexattr_destroy(&attrM);

    mUser = "root";
    mPage = grpAdd("pg_");
    sec = SYS->security();
    mReqTm = time(NULL);
}

Session::~Session( )
{
    pthread_mutex_destroy(&dataM);
    pthread_mutex_destroy(&mCalcRes);
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
	mess_info(nodePath().c_str(),_("Enable session."));
	try {
	    if(mess_lev() == TMess::Debug) d_tm = TSYS::curTime();

	    //Connect to project
	    mParent = mod->prjAt(mPrjnm);

	    //Get data from project
	    mOwner	= parent().at().owner( );
	    mGrp	= parent().at().grp( );
	    mPermit	= parent().at().permit( );
	    setPeriod( parent().at().period( ) );

	    //Load previous style
	    TConfig c_el(&mod->elPrjSes());
	    c_el.cfg("IDW").setS("<Style>");
	    c_el.cfg("ID").setS(user());
	    if(SYS->db().at().dataGet(parent().at().DB()+"."+parent().at().tbl()+"_ses",mod->nodePath()+parent().at().tbl()+"_ses",c_el,false,true))
		stlCurentSet(c_el.cfg("IO_VAL").getI());
	    else stlCurentSet(parent().at().stlCurent());

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
		catch(TError err) { mess_err( err.cat.c_str(), "%s", err.mess.c_str() ); }

	    if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("Enable root pages time: %f ms."), 1e-3*(TSYS::curTime()-d_tm));

	    modifGClr();
	}
	catch(...){ mParent.free(); }
    }
    else {
	if(start()) setStart(false);

	mess_info(nodePath().c_str(),_("Disable session."));

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

    if(val)
    {
	//Enable session if it disabled
	if(!enable())	setEnable(true);

	if(mess_lev() == TMess::Debug)	d_tm = TSYS::curTime();

	mess_info(nodePath().c_str(),_("Start session."));

	//> Load Styles from project
	mStProp.clear();
	if(stlCurent() >= 0)
	{
	    parent().at().stlPropList(pg_ls);
	    for(unsigned i_sp = 0; i_sp < pg_ls.size(); i_sp++)
		mStProp[pg_ls[i_sp]] = parent().at().stlPropGet(pg_ls[i_sp], "", stlCurent());
	}

	if(mess_lev() == TMess::Debug)
	{
	    mess_debug(nodePath().c_str(), _("Load styles from project time: %f ms."), 1e-3*(TSYS::curTime()-d_tm));
	    d_tm = TSYS::curTime();
	}

	//Process all pages is on
	list(pg_ls);
	for(unsigned i_ls = 0; i_ls < pg_ls.size(); i_ls++)
	    at(pg_ls[i_ls]).at().setProcess(true);

	if(mess_lev() == TMess::Debug)
	{
	    mess_debug(nodePath().c_str(), _("Process on for all root pages time: %f ms."), 1e-3*(TSYS::curTime()-d_tm));
	    d_tm = TSYS::curTime();
	}

	//Start process task
	if(!mStart) SYS->taskCreate(nodePath('.',true), 0, Session::Task, this);

	if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("Start process task time: %f ms."), 1e-3*(TSYS::curTime()-d_tm));
    }
    else
    {
	mess_info(nodePath().c_str(),_("Stop session."));

	//Stop process task
	if(mStart) SYS->taskDestroy(nodePath('.',true), &endrun_req);

	//Process all pages is off
	list(pg_ls);
	for(unsigned i_ls = 0; i_ls < pg_ls.size(); i_ls++)
	    at(pg_ls[i_ls]).at().setProcess(false);
    }
}

bool Session::modifChk( unsigned int tm, unsigned int iMdfClc )
{
    return (mCalcClk>=tm) ? (iMdfClc >= tm && iMdfClc <= mCalcClk) : (iMdfClc >= tm || iMdfClc <= mCalcClk);
}

string Session::ico( )
{
    if(!parent().freeStat()) return parent().at().ico();
    return "";
}

double Session::calcTm( )		{ return SYS->taskUtilizTm(nodePath('.',true)); }

AutoHD<Project> Session::parent( )	{ return mParent; }

void Session::add( const string &iid, const string &iparent )
{
    if(present(iid))	return;
    chldAdd(mPage, new SessPage(iid,iparent,this));
}

vector<string> Session::openList( )
{
    pthread_mutex_lock(&dataM);
    vector<string> rez = mOpen;
    pthread_mutex_unlock(&dataM);
    return rez;
}

void Session::openReg( const string &iid )
{
    unsigned i_op;
    pthread_mutex_lock(&dataM);
    for(i_op = 0; i_op < mOpen.size(); i_op++)
	if(iid == mOpen[i_op]) break;
    if(i_op >= mOpen.size())	mOpen.push_back(iid);
    pthread_mutex_unlock(&dataM);
}

void Session::openUnreg( const string &iid )
{
    pthread_mutex_lock(&dataM);
    for(unsigned i_op = 0; i_op < mOpen.size(); i_op++)
	if(iid == mOpen[i_op]) mOpen.erase(mOpen.begin()+i_op);
    pthread_mutex_unlock(&dataM);
}

AutoHD<SessPage> Session::at( const string &id )
{
    return chldAt(mPage, id);
}

void Session::uiComm( const string &com, const string &prm, SessWdg *src )
{
    //Find of pattern adequancy for opened page
    string oppg, pBase;		//Opened page according of pattern

    vector<string> op_ls = openList();
    for(unsigned i_op = 0; i_op < op_ls.size(); i_op++)
    {
	string cur_pt_el, cur_el;
	for(int i_el = 0; (cur_pt_el=TSYS::pathLev(prm,i_el++)).size(); )
	    if((cur_el=TSYS::pathLev(op_ls[i_op],i_el)).empty() || (cur_pt_el.compare(0,3,"pg_") == 0 && cur_pt_el != cur_el)) break;
	if(cur_pt_el.empty()) { oppg = op_ls[i_op]; break; }
    }

    pBase = oppg;
    if(pBase.empty() && src) pBase = src->path();

    //Individual commands process
    try
    {
	// Go to destination page
	string cur_pt_el;
	AutoHD<SessPage> cpg;
	for(unsigned i_el = 0; (cur_pt_el=TSYS::pathLev(prm,i_el++)).size(); )
	{
	    string op_pg;
	    if(cur_pt_el.compare(0,3,"pg_") == 0) op_pg = cur_pt_el.substr(3);
	    else if(cur_pt_el == "*" || (cur_pt_el == "$" && ( com == "next" || com == "prev")))
	    {
		vector<string> pls;
		if(cpg.freeStat()) list(pls); else cpg.at().pageList(pls);
		if(pls.empty())	return;
		string cur_el = TSYS::pathLev(pBase,i_el);
		if(cur_el.empty())
		{
		    if(cur_pt_el == "$")	return;
		    op_pg = pls[0];
		}
		else
		{
		    cur_el = cur_el.substr(3);
		    int i_l;
		    for(i_l = 0; i_l < (int)pls.size(); i_l++)
			if(cur_el == pls[i_l]) break;
		    if(i_l < (int)pls.size())
		    {
			if(cur_pt_el == "$")
			{
			    if(com == "next") i_l++;
			    if(com == "prev") i_l--;
			    i_l = (i_l < 0) ? (int)pls.size()-1 : (i_l >= (int)pls.size()) ? 0 : i_l;
			    op_pg = pls[i_l];
			    if(op_pg == cur_el) return;
			}
			else op_pg = cur_el;
		    }
		    else
		    {
			if(cur_pt_el == "$") return;
			op_pg = pls[0];
		    }
		}
	    }
	    else op_pg = cur_pt_el;
	    //>> Go to next page
	    cpg = cpg.freeStat() ? at(op_pg) : cpg.at().pageAt(op_pg);
	}

	//> Open found page
	if(!cpg.freeStat())
	{
	    if(!oppg.empty() && ((AutoHD<SessPage>)mod->nodeAt(oppg)).at().path() != cpg.at().path())
		((AutoHD<SessPage>)mod->nodeAt(oppg)).at().attrAt("pgOpenSrc").at().setS("");
	    cpg.at().attrAt("pgOpenSrc").at().setS(src->path());
	}
    }catch(...){ }
}

void Session::alarmSet( const string &wpath, const string &alrm )
{
    if(wpath.empty()) return;

    //> Alarms queue process
    ResAlloc res(mAlrmRes, true);

    Alarm aobj(wpath, alrm, calcClk());

    unsigned i_q = 0;
    while(i_q < mAlrm.size() && mAlrm[i_q].path != aobj.path) i_q++;
    if(!aobj.lev)
    {
	if(i_q < mAlrm.size()) mAlrm.erase(mAlrm.begin()+i_q);
	return;
    }
    if(i_q < mAlrm.size() && aobj.lev == mAlrm[i_q].lev) mAlrm[i_q] = aobj;
    else
    {
	if(i_q < mAlrm.size())
	{
	    mAlrm.erase(mAlrm.begin()+i_q);
	    if((int)i_q == mAlrmSndPlay) mAlrmSndPlay = -1;
	    if((int)i_q < mAlrmSndPlay && mAlrmSndPlay >= 0) mAlrmSndPlay--;
	}
	unsigned i_q1 = 0;
	while(i_q1 < mAlrm.size() && aobj.lev >= mAlrm[i_q].lev) i_q1++;
	if(i_q1 < mAlrm.size())
	{
	    mAlrm.insert(mAlrm.begin()+i_q1, aobj);
	    if((int)i_q <= mAlrmSndPlay && mAlrmSndPlay >= 0) mAlrmSndPlay++;
	}
	else mAlrm.push_back(aobj);
    }
}

int Session::alarmStat( )
{
    uint8_t alev = 0, atp = 0, aqtp = 0;
    vector<string> ls;
    list(ls);
    for(unsigned i_p = 0; i_p < ls.size(); i_p++)
    {
	int ast = at(ls[i_p]).at().attrAt("alarmSt").at().getI();
	alev = vmax(alev,ast&0xFF);
	atp |= (ast>>8)&0xFF;
	aqtp |= (ast>>16)&0xFF;
    }

    return (aqtp<<16)|(atp<<8)|alev;
}

void Session::alarmQuittance( const string &wpath, uint8_t quit_tmpl )
{
    if(!wpath.empty()) ((AutoHD<SessWdg>)mod->nodeAt(wpath)).at().alarmQuittance(quit_tmpl, true);
    else
    {
	vector<string> ls;
	list(ls);
	for(unsigned i_p = 0; i_p < ls.size(); i_p++)
	    at(ls[i_p]).at().alarmQuittance(quit_tmpl, true);
    }

    //> Queue alarms quittance
    ResAlloc res(mAlrmRes, false);
    for(unsigned i_q = 0; i_q < mAlrm.size(); i_q++)
	if(mAlrm[i_q].path.substr(0,wpath.size()) == wpath)
	    mAlrm[i_q].qtp &= quit_tmpl;
}

void *Session::Task( void *icontr )
{
    vector<string> pls;
    Session &ses = *(Session *)icontr;

    ses.endrun_req = false;
    ses.mStart    = true;

    ses.list(pls);
    while(!ses.endrun_req)
    {
	//> Calc session pages and all other items at recursion
	for(unsigned i_l = 0; i_l < pls.size(); i_l++)
	    try { ses.at(pls[i_l]).at().calc(false,false); }
	    catch(TError err)
	    {
		mess_err(err.cat.c_str(),"%s",err.mess.c_str());
		mess_err(ses.nodePath().c_str(),_("Session '%s' calculate error."),pls[i_l].c_str());
	    }

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

    if(start())
    {
	MtxAlloc res(dataM, true);

	//Load Styles from project
	mStProp.clear();

	if(sid >= 0 && sid < parent().at().stlSize())
	{
	    vector<string> pg_ls;
	    parent().at().stlPropList(pg_ls);
	    for(unsigned i_sp = 0; i_sp < pg_ls.size(); i_sp++)
		mStProp[pg_ls[i_sp]] = parent().at().stlPropGet(pg_ls[i_sp], "", sid);
	}
	else mStyleIdW = -1;
    }

    //Write to DB
    if(enable())
    {
	TConfig c_el(&mod->elPrjSes());
	c_el.cfg("IDW").setS("<Style>");
	c_el.cfg("ID").setS(user());
	c_el.cfg("IO_VAL").setI(mStyleIdW);
	SYS->db().at().dataSet(parent().at().DB()+"."+parent().at().tbl()+"_ses", mod->nodePath()+parent().at().tbl()+"_ses", c_el, false, true);
    }
}

string Session::stlPropGet( const string &pid, const string &def )
{
    MtxAlloc res(dataM, true);

    if(stlCurent() < 0 || pid.empty() || pid == "<Styles>") return def;

    map<string,string>::iterator iStPrp = mStProp.find(pid);
    if(iStPrp != mStProp.end()) return iStPrp->second;

    return def;
}

bool Session::stlPropSet( const string &pid, const string &vl )
{
    MtxAlloc res(dataM, true);
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
    // string alrmSndPlay( ) - the widget's path for that on this time played the alarm message.
    else if(iid == "alrmSndPlay")
    {
	ResAlloc res(mAlrmRes, false);
	if(mAlrmSndPlay < 0 || mAlrmSndPlay >= (int)mAlrm.size()) return string("");
	return mAlrm[mAlrmSndPlay].path;
    }
    // int alrmQuittance(int quit_tmpl, string wpath = "") - alarm quittance <wpath> with template <quit_tmpl>. If <wpath> is empty
    //        string then make global quittance.
    //  quit_tmpl - quittance template
    //  wpath - path to widget
    else if(iid == "alrmQuittance" && prms.size() >= 1)
    {
	alarmQuittance((prms.size()>=2) ? prms[1].getS() : "", ~prms[0].getI());
	return 0;
    }
    return TCntrNode::objFuncCall(iid,prms,cuser);
}

void Session::cntrCmdProc( XMLNode *opt )
{
    string a_path = opt->attr("path");
    //Service commands process
    if(a_path == "/serv/pg")	//Pages operations
    {
	if(ctrChkNode(opt,"openlist",permit(),owner().c_str(),grp().c_str(),SEC_RD))	//Open pages list
	{
	    unsigned tm = strtoul(opt->attr("tm").c_str(),NULL,10);
	    unsigned ntm = calcClk();
	    vector<string> lst = openList();
	    for(unsigned i_f = 0; i_f < lst.size(); i_f++)
	    {
		XMLNode *pel = opt->childAdd("pg");
		pel->setText(lst[i_f]);

		if(tm)
		{
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
    else if(a_path == "/serv/alarm")	//Alarm operations
    {
	if(ctrChkNode(opt,"get",permit(),owner().c_str(),grp().c_str(),SEC_RD))
	{
	    // Get alarm status
	    int aSt = alarmStat();
	    opt->setAttr("alarmSt", i2s(aSt));

	    // Get alarm from sound queue
	    if(opt->attr("mode") == "sound")
	    {
		unsigned a_tm  = strtoul(opt->attr("tm").c_str(),NULL,10);
		opt->setAttr("tm", u2s(calcClk()));

		//> Find event, return it and alarm resource
		ResAlloc res(mAlrmRes, false);
		string wdg = opt->attr("wdg");
		int i_q, i_first = -1, i_next = -1;
		for(i_q = mAlrm.size()-1; i_q >= 0; i_q--)
		{
		    if(!(mAlrm[i_q].qtp & Engine::Sound)) continue;
		    if(wdg.empty() || modifChk(a_tm,mAlrm[i_q].clc) || i_next > 0)	break;	//First, new and next alarms break
		    if(i_first < 0) i_first = i_q;
		    if(wdg == mAlrm[i_q].path) i_next = i_q;
		}
		if(i_q < 0 && i_first >= 0) i_q = i_first;
		if(i_q >= 0)
		{
		    opt->setAttr("wdg", mAlrm[i_q].path);
		    if(!mAlrm[i_q].tpArg.empty())
			opt->setText(((AutoHD<SessWdg>)mod->nodeAt(mAlrm[i_q].path)).at().resourceGet(mAlrm[i_q].tpArg));
		    else opt->setText(mod->callSynth(mAlrm[i_q].mess));
		    mAlrmSndPlay = i_q;
		}else mAlrmSndPlay = -1;
	    } else if(!((aSt>>16) & Engine::Sound)) mAlrmSndPlay = -1;
	}
	else if(ctrChkNode(opt,"quittance",permit(),owner().c_str(),grp().c_str(),SEC_WR))
	    alarmQuittance(opt->attr("wdg"),~atoi(opt->attr("tmpl").c_str()));
	return;
    }

    //> Get page info
    if(opt->name() == "info")
    {
	TCntrNode::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Session: ")+id(),permit(),owner().c_str(),grp().c_str());
	if(ico().size()) ctrMkNode("img",opt,-1,"/ico","",R_R_R_);
	if(ctrMkNode("branches",opt,-1,"/br","",R_R_R_))
	    ctrMkNode("grp",opt,-1,"/br/pg_",_("Page"),R_R_R_,"root",SUI_ID,1,"idm","1");
	if(ctrMkNode("area",opt,-1,"/obj",_("Session")))
	{
	    if(ctrMkNode("area",opt,-1,"/obj/st",_("State")))
	    {
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
		ctrMkNode("fld",opt,-1,"/obj/st/prj",_("Project"),permit(),owner().c_str(),grp().c_str(),4,"tp","str","idm","1","dest","select","select","/obj/prj_ls");
		ctrMkNode("fld",opt,-1,"/obj/st/backgrnd",_("Background execution mode"),R_R_R_,"root",SUI_ID,1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/obj/st/connect",_("Connections counter"),R_R_R_,"root",SUI_ID,1,"tp","int");
		ctrMkNode("fld",opt,-1,"/obj/st/calc_tm",_("Calculate session time"),R_R_R_,"root",SUI_ID,1,"tp","str");
	    }
	    if(ctrMkNode("area",opt,-1,"/obj/cfg",_("Configuration")))
	    {
		ctrMkNode("fld",opt,-1,"/obj/cfg/per",_("Period (ms)"),permit(),owner().c_str(),grp().c_str(),1,"tp","dec");
		ctrMkNode("fld",opt,-1,"/obj/cfg/style",_("Style"),permit(),owner().c_str(),grp().c_str(),3,"tp","dec","dest","select","select","/obj/cfg/stLst");
		ctrMkNode("list",opt,-1,"/obj/cfg/openPg",_("Opened pages"),R_R_R_,"root",SUI_ID,1,"tp","str");
	    }
	}
	if(ctrMkNode("area",opt,-1,"/page",_("Pages")))
	    ctrMkNode("list",opt,-1,"/page/page",_("Pages"),R_R_R_,"root",SUI_ID,3,"tp","br","idm","1","br_pref","pg_");
	if(ctrMkNode("area",opt,-1,"/alarm",_("Alarms")))
	    if(ctrMkNode("table",opt,-1,"/alarm/alarm",_("Alarms list"),R_R_R_,"root",SUI_ID))
	    {
		ctrMkNode("list",opt,-1,"/alarm/alarm/wdg",_("Widget"),R_R_R_,"root",SUI_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/alarm/alarm/lev",_("Level"),R_R_R_,"root",SUI_ID,1,"tp","dec");
		ctrMkNode("list",opt,-1,"/alarm/alarm/cat",_("Category"),R_R_R_,"root",SUI_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/alarm/alarm/mess",_("Messages"),R_R_R_,"root",SUI_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/alarm/alarm/tp",_("Type"),R_R_R_,"root",SUI_ID,1,"tp","hex");
		ctrMkNode("list",opt,-1,"/alarm/alarm/qtp",_("Quittance"),R_R_R_,"root",SUI_ID,1,"tp","hex");
		ctrMkNode("list",opt,-1,"/alarm/alarm/tpArg",_("Type argument"),R_R_R_,"root",SUI_ID,1,"tp","str");
	    }
	return;
    }

    //> Process command to page
    if(a_path == "/ico" && ctrChkNode(opt))	opt->setText(ico());
    else if(a_path == "/obj/st/en")
    {
	if(ctrChkNode(opt,"get",permit(),owner().c_str(),grp().c_str(),SEC_RD))	opt->setText(i2s(enable()));
	if(ctrChkNode(opt,"set",permit(),owner().c_str(),grp().c_str(),SEC_WR))	setEnable(atoi(opt->text().c_str()));
    }
    else if(a_path == "/obj/st/start")
    {
	if(ctrChkNode(opt,"get",permit(),owner().c_str(),grp().c_str(),SEC_RD))	opt->setText(i2s(start()));
	if(ctrChkNode(opt,"set",permit(),owner().c_str(),grp().c_str(),SEC_WR))	setStart(atoi(opt->text().c_str()));
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
    else if(a_path == "/obj/st/prj")
    {
	if(ctrChkNode(opt,"get",permit(),owner().c_str(),grp().c_str(),SEC_RD))	opt->setText(projNm());
	if(ctrChkNode(opt,"set",permit(),owner().c_str(),grp().c_str(),SEC_WR))	setProjNm(opt->text());
    }
    else if(a_path == "/obj/st/backgrnd" && ctrChkNode(opt))	opt->setText(i2s(backgrnd()));
    else if(a_path == "/obj/st/connect" && ctrChkNode(opt))	opt->setText(i2s(connects()));
    else if(a_path == "/obj/st/calc_tm" && ctrChkNode(opt))	opt->setText(TSYS::time2str(calcTm()));
    else if(a_path == "/obj/prj_ls" && ctrChkNode(opt))
    {
	vector<string> lst;
	mod->prjList(lst);
	for(unsigned i_f=0; i_f < lst.size(); i_f++)
	    opt->childAdd("el")->setAttr("id",lst[i_f])->setText(mod->prjAt(lst[i_f]).at().name());
    }
    else if(a_path == "/obj/cfg/per")
    {
	if(ctrChkNode(opt,"get",permit(),owner().c_str(),grp().c_str(),SEC_RD))	opt->setText(i2s(period()));
	if(ctrChkNode(opt,"set",permit(),owner().c_str(),grp().c_str(),SEC_WR))	setPeriod(atoi(opt->text().c_str()));
    }
    else if(a_path == "/obj/cfg/style")
    {
	if(ctrChkNode(opt,"get",permit(),owner().c_str(),grp().c_str(),SEC_RD))	opt->setText(i2s(stlCurent()));
	if(ctrChkNode(opt,"set",permit(),owner().c_str(),grp().c_str(),SEC_WR))	stlCurentSet(atoi(opt->text().c_str()));
    }
    else if(a_path == "/obj/cfg/stLst" && ctrChkNode(opt))
    {
	opt->childAdd("el")->setAttr("id","-1")->setText(_("No style"));
	if(enable())
	    for(int iSt = 0; iSt < parent().at().stlSize(); iSt++)
		opt->childAdd("el")->setAttr("id", i2s(iSt))->setText(TSYS::strSepParse(parent().at().stlGet(iSt),0,';'));

    }
    else if(a_path == "/obj/cfg/openPg" && ctrChkNode(opt))
    {
	vector<string> lst = openList();
	for(unsigned i_f = 0; i_f < lst.size(); i_f++)
	    opt->childAdd("el")->setText(lst[i_f]);
    }
    else if((a_path == "/br/pg_" || a_path == "/page/page") && ctrChkNode(opt))
    {
	vector<string> lst;
	list(lst);
	for(unsigned i_f=0; i_f < lst.size(); i_f++)
	    opt->childAdd("el")->setAttr("id",lst[i_f])->setText(at(lst[i_f]).at().name());
    }
    else if(a_path == "/alarm/alarm" && ctrChkNode(opt))
    {
	//> Fill Archivators table
	XMLNode *n_wdg	= ctrMkNode("list", opt, -1, "/alarm/alarm/wdg", "", R_R_R_);
	XMLNode *n_lev	= ctrMkNode("list", opt, -1, "/alarm/alarm/lev", "", R_R_R_);
	XMLNode *n_cat	= ctrMkNode("list", opt, -1, "/alarm/alarm/cat", "", R_R_R_);
	XMLNode *n_mess	= ctrMkNode("list", opt, -1, "/alarm/alarm/mess", "", R_R_R_);
	XMLNode *n_tp	= ctrMkNode("list", opt, -1, "/alarm/alarm/tp", "", R_R_R_);
	XMLNode *n_qtp	= ctrMkNode("list", opt, -1, "/alarm/alarm/qtp", "", R_R_R_);
	XMLNode *n_tpArg= ctrMkNode("list", opt, -1, "/alarm/alarm/tpArg", "", R_R_R_);

	ResAlloc res(mAlrmRes, false);
	for(int i_q = mAlrm.size()-1; i_q >= 0; i_q--)
	{
	    if(n_wdg)	n_wdg->childAdd("el")->setText(mAlrm[i_q].path);
	    if(n_lev)	n_lev->childAdd("el")->setText(i2s(mAlrm[i_q].lev));
	    if(n_cat)	n_cat->childAdd("el")->setText(mAlrm[i_q].cat);
	    if(n_mess)	n_mess->childAdd("el")->setText(mAlrm[i_q].mess);
	    if(n_tp)	n_tp->childAdd("el")->setText(i2s(mAlrm[i_q].tp));
	    if(n_qtp)	n_qtp->childAdd("el")->setText(i2s(mAlrm[i_q].qtp));
	    if(n_tpArg)	n_tpArg->childAdd("el")->setText(mAlrm[i_q].tpArg);
	}
    }
    else TCntrNode::cntrCmdProc(opt);
}

//* Alarm: Alarm object				 *
//************************************************
Session::Alarm::Alarm( const string &ipath, const string &alrm, unsigned iclc ) : path(ipath), clc(iclc)
{
    int a_off = 0;
    lev   = atoi( TSYS::strSepParse(alrm,0,'|',&a_off).c_str() );
    cat   = TSYS::strSepParse(alrm,0,'|',&a_off);
    mess  = TSYS::strSepParse(alrm,0,'|',&a_off);
    qtp   = tp = atoi( TSYS::strSepParse(alrm,0,'|',&a_off).c_str() );
    tpArg = TSYS::strSepParse(alrm,0,'|',&a_off);
}

//************************************************
//* SessPage: Page of Project's session          *
//************************************************
SessPage::SessPage( const string &iid, const string &ipage, Session *sess ) :
    SessWdg(iid,ipage,sess), mClosePgCom(false)
{
    mPage = grpAdd("pg_");
}

SessPage::~SessPage( )
{

}

string SessPage::path( )
{
    return ownerFullId(true)+"/pg_"+id();
}

void SessPage::setEnable( bool val, bool force )
{
    vector<string> pg_ls;

    //> Page enable
    if(val)
    {
	mToEn = true;
	//>> Check for full enable need
	bool pgOpen = (!(parent().at().prjFlags()&Page::Empty) && parent().at().attrAt("pgOpen").at().getB());
	if((pgOpen || force || parent().at().attrAt("pgNoOpenProc").at().getB()) && !enable())
	{
	    SessWdg::setEnable(true);
	    if(pgOpen) ownerSess()->openReg(path());
	}
	//>> Child pages process
	if(!force)
	{
	    //> Create included pages
	    parent().at().pageList(pg_ls);
	    for(unsigned i_p = 0; i_p < pg_ls.size(); i_p++)
		if(!pagePresent(pg_ls[i_p]))
		    pageAdd(pg_ls[i_p],parent().at().pageAt(pg_ls[i_p]).at().path());
	    //> Enable included pages
	    pageList(pg_ls);
	    for(unsigned i_l = 0; i_l < pg_ls.size(); i_l++)
		try{ pageAt(pg_ls[i_l]).at().setEnable(true); }
		catch(TError err)	{ mess_err(err.cat.c_str(), "%s", err.mess.c_str()); }
	}
	mToEn = false;
    }
    else if(enable())
    {
	//> Unregister opened page
	if(!(parent().at().prjFlags()&Page::Empty) && attrPresent("pgOpen") && attrAt("pgOpen").at().getB())
	    ownerSess()->openUnreg(path());
	//> Disable include pages
	pageList(pg_ls);
	for(unsigned i_l = 0; i_l < pg_ls.size(); i_l++)
	    pageAt(pg_ls[i_l]).at().setEnable(false);
	//> Delete included pages
	for(unsigned i_l = 0; i_l < pg_ls.size(); i_l++)
	    pageDel(pg_ls[i_l]);

	SessWdg::setEnable(false);
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

AutoHD<Page> SessPage::parent( )
{
    if(!enable())
    {
	if(parentNm() == "..") return AutoHD<TCntrNode>(nodePrev());
	else return mod->nodeAt(parentNm());
    }
    return Widget::parent();
}

void SessPage::pageAdd( const string &iid, const string &iparent )
{
    if( pagePresent(iid) )return;
    chldAdd(mPage,new SessPage(iid,iparent,ownerSess()));
}

AutoHD<SessPage> SessPage::pageAt( const string &iid )
{
    return chldAt(mPage,iid);
}

AutoHD<Widget> SessPage::wdgAt( const string &wdg, int lev, int off )
{
    //> Check for global
    if(lev == 0 && off == 0 && wdg.compare(0,1,"/") == 0)
	try { return (AutoHD<Widget>)ownerSess()->nodeAt(wdg,1); }
	catch(TError err) { return AutoHD<Widget>(); }

    int offt = off;
    string iw = TSYS::pathLev(wdg,lev,true,&offt);
    if(iw.compare(0,3,"pg_") == 0)
    {
	if(pagePresent(iw.substr(3))) return pageAt(iw.substr(3)).at().wdgAt(wdg, 0, offt);
	else return AutoHD<Widget>();
    }

    return Widget::wdgAt(wdg, lev, off);
}

void SessPage::calc( bool first, bool last )
{
    //Process self data
    if(process()) SessWdg::calc(first, last);

    if(mClosePgCom) { mClosePgCom = false; setProcess(false); return; }

    //Put calculate to include pages
    vector<string> ls;
    pageList(ls);
    for(unsigned i_l = 0; i_l < ls.size(); i_l++)
	pageAt(ls[i_l]).at().calc(first, last);
}

bool SessPage::attrChange( Attr &cfg, TVariant prev )
{
    //Page open process
    if(enable() && !prev.isNull())
    {
	if(cfg.id() == "pgOpen")
	{
	    if(cfg.getB())
	    {
		mClosePgCom = false;
		ownerSess()->openReg(path());	//Moved up for allow access and pages including from "f_start"
		if(!process())	setProcess(true);
	    }
	    else
	    {
		ownerSess()->openUnreg(path());
		if(process() && !attrAt("pgNoOpenProc").at().getB())	mClosePgCom = true;
		if(!attrAt("pgOpenSrc").at().getS().empty()) attrAt("pgOpenSrc").at().setS("");
		pgClose();
	    }
	}
	else if(cfg.id() == "pgOpenSrc")
	{
	    if(!cfg.getS().empty())
	    {
		try
		{
		    AutoHD<SessWdg> src = mod->nodeAt(cfg.getS());

		    //Set interwidget's links for new page
		    bool emptyPresnt = false;
		    string atr_id, prm_lnk, sCfgVal;
		    vector<string> cAtrLs;
		    attrList(cAtrLs);
		    for(unsigned i_al = 0; i_al < cAtrLs.size(); i_al++)
		    {
			AutoHD<Attr> attr = attrAt(cAtrLs[i_al]);
			if(!(attr.at().flgSelf()&(Attr::CfgLnkIn|Attr::CfgLnkOut) &&
			      TSYS::strParse(attr.at().cfgTempl(),0,"|") == "<page>")) continue;
			atr_id = TSYS::strParse(attr.at().cfgTempl(),1,"|");
			if(src.at().attrPresent(atr_id))
			{
			    if((sCfgVal=src.at().attrAt(atr_id).at().cfgVal()).compare(0,4,"prm:") == 0 &&
				!SYS->daq().at().attrAt(sCfgVal.substr(4),0,true).freeStat())
			    {
				if(prm_lnk.empty()) prm_lnk = sCfgVal.substr(4);
				attr.at().setCfgVal(sCfgVal);
			    }
			    else attr.at().setCfgVal("wdg:"+cfg.getS()+"/a_"+atr_id);
			}
			else
			{
			    attr.at().setCfgVal("");
			    attr.at().setS(EVAL_STR);
			    emptyPresnt = true;
			}
		    }

		    //Find links into source if no link found
		    if(prm_lnk.empty())
		    {
			vector<string> sAtrLs;
			src.at().attrList(sAtrLs);
			for(unsigned i_al = 0; i_al < sAtrLs.size(); i_al++)
			{
			    AutoHD<Attr> attr = src.at().attrAt(sAtrLs[i_al]);
			    if(attr.at().flgSelf()&(Attr::CfgLnkIn|Attr::CfgLnkOut) && attr.at().cfgVal().size() > 4 && attr.at().cfgVal().substr(0,4) == "prm:")
			    {
				prm_lnk = attr.at().cfgVal().substr(4);
				break;
			    }
			}
		    }

		    //Fill parameter's links for other attributes
		    if(emptyPresnt && !prm_lnk.empty())
		    {
			size_t aPos = prm_lnk.rfind("/");
			if(aPos != string::npos) prm_lnk.erase(aPos);
			AutoHD<TValue> prml = SYS->daq().at().prmAt(prm_lnk,0,true);
			for(unsigned i_al = 0; !prml.freeStat() && i_al < cAtrLs.size(); i_al++)
			{
			    AutoHD<Attr> attr = attrAt(cAtrLs[i_al]);
			    if(!(attr.at().flgSelf()&(Attr::CfgLnkIn|Attr::CfgLnkOut) &&
				  TSYS::strSepParse(attr.at().cfgTempl(),0,'|') == "<page>" &&
				  (attr.at().cfgVal().empty() || attr.at().flgGlob()&Attr::Address)))	continue;
			    atr_id = TSYS::strSepParse(attr.at().cfgTempl(),1,'|');
			    if(prml.at().vlPresent(atr_id))	attr.at().setCfgVal("prm:"+prm_lnk+"/"+atr_id);
			}
		    }
		}
		catch(TError err) { }
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
    int alev = atoi(TSYS::strSepParse(aCur,0,'|').c_str()) & 0xFF;
    int atp  = atoi(TSYS::strSepParse(aCur,3,'|').c_str()) & 0xFF;
    int aqtp = isSet ? atp : (aStCur>>16) & 0xFF & atp;

    vector<string> lst;
    //> Included pages process
    pageList(lst);
    for(unsigned i_p = 0; i_p < lst.size(); i_p++)
    {
	int iacur = pageAt(lst[i_p]).at().attrAt("alarmSt").at().getI();
	alev = vmax(alev, iacur&0xFF);
	atp |= (iacur>>8) & 0xFF;
	aqtp |= (iacur>>16) & 0xFF;
    }

    //> Included widgets process
    wdgList( lst );
    for(unsigned i_w = 0; i_w < lst.size(); i_w++)
    {
	int iacur = wdgAt(lst[i_w]).at().attrAt("alarmSt").at().getI();
	alev = vmax(alev, iacur&0xFF);
	atp |= (iacur>>8) & 0xFF;
	aqtp |= (iacur>>16) & 0xFF;
    }

    attrAt("alarmSt").at().setI((alev && atp) ? (aqtp<<16)|(atp<<8)|alev : 0);

    if(ownerSessWdg(true)) ownerSessWdg(true)->alarmSet();
    if(isSet) ownerSess()->alarmSet(path(), aCur);
}

void SessPage::alarmQuittance( uint8_t quit_tmpl, bool isSet )
{
    int alarmSt = attrAt("alarmSt").at().getI();
    if(!((((alarmSt>>16)&0xFF)^quit_tmpl)&((alarmSt>>16)&0xFF))) return;

    //> Self quittance
    attrAt("alarmSt").at().setI(alarmSt & (quit_tmpl<<16|0xFFFF));

    vector<string> lst;
    //> Included pages quittance
    pageList(lst);
    for(unsigned i_p = 0; i_p < lst.size(); i_p++)
	pageAt(lst[i_p]).at().alarmQuittance(quit_tmpl);
    //> Include widgets quittance
    wdgList( lst );
    for(unsigned i_w = 0; i_w < lst.size(); i_w++)
	((AutoHD<SessWdg>)wdgAt(lst[i_w])).at().alarmQuittance(quit_tmpl);

    if(isSet && ownerSessWdg(true))	ownerSessWdg(true)->alarmSet();
}

bool SessPage::attrPresent(const string &attr)
{
    if(!enable() && !mToEn) setEnable(true, true);
    return Widget::attrPresent(attr);
}

AutoHD<Attr> SessPage::attrAt(const string &attr, int lev)
{
    if(lev < 0 && !enable() && !mToEn) setEnable(true, true);
    return Widget::attrAt(attr,lev);
}

TVariant SessPage::stlReq( Attr &a, const TVariant &vl, bool wr )
{
    if(stlLock()) return vl;
    string pid = TSYS::strNoSpace(a.cfgTempl());
    if(pid.empty()) pid = a.id();
    if(!wr) return ownerSess()->stlPropGet(pid, vl.getS());
    if(ownerSess()->stlPropSet(pid,vl.getS())) return TVariant();
    return vl;
}

bool SessPage::cntrCmdGeneric( XMLNode *opt )
{
    //> Get page info
    if(opt->name() == "info")
    {
	SessWdg::cntrCmdGeneric(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Session page: ")+ownerFullId()+"/"+id());
	if( enable() && !(parent().at().prjFlags( )&Page::Empty) )
	    ctrMkNode("fld",opt,1,"/wdg/st/open",_("Open"),RWRWR_,owner().c_str(),grp().c_str(),1,"tp","bool");
	if( enable() && parent().at().prjFlags()&(Page::Template|Page::Container) )
	{
	    if(ctrMkNode("area",opt,1,"/page",_("Pages")))
		ctrMkNode("list",opt,-1,"/page/page",_("Pages"),R_R_R_,"root","UI",3,"tp","br","idm","1","br_pref","pg_");
	    if(ctrMkNode("branches",opt,-1,"/br","",R_R_R_))
		ctrMkNode("grp",opt,-1,"/br/pg_",_("Page"),R_R_R_,"root","UI",1,"idm","1");
	}
        return true;
    }

    //> Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/wdg/st/open" && enable() && !(parent().at().prjFlags()&Page::Empty))
    {
	if(ctrChkNode(opt,"get",RWRWR_,owner().c_str(),grp().c_str(),SEC_RD))
	    opt->setText(i2s(attrAt("pgOpen").at().getB()));
	if(ctrChkNode(opt,"set",RWRWR_,owner().c_str(),grp().c_str(),SEC_WR))
	    attrAt("pgOpen").at().setB(atoi(opt->text().c_str()));
    }
    else if( (a_path == "/br/pg_" || a_path == "/page/page") && ctrChkNode(opt) )
    {
	vector<string> lst;
	pageList(lst);
	for( unsigned i_f=0; i_f < lst.size(); i_f++ )
	    opt->childAdd("el")->setAttr("id",lst[i_f])->setText(pageAt(lst[i_f]).at().name());
    }
    else return SessWdg::cntrCmdGeneric(opt);

    return true;
}

//************************************************
//* SessWdg: Session page's widget               *
//************************************************
SessWdg::SessWdg( const string &iid, const string &iparent, Session *isess ) :
    Widget(iid,iparent), TValFunc(iid+"_wdg",NULL), mProc(false), inLnkGet(true), mToEn(false), mMdfClc(0),
    mCalcClk(isess->calcClk()), mSess(isess)
{
    BACrtHoldOvr = true;

    pthread_mutexattr_t attrM;
    pthread_mutexattr_init(&attrM);
    pthread_mutexattr_settype(&attrM, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&mCalcRes, &attrM);
    pthread_mutexattr_destroy(&attrM);
}

SessWdg::~SessWdg( )
{
    pthread_mutex_destroy(&mCalcRes);
}

void SessWdg::preDisable( int flag )
{
    if(process()) setProcess(false);

    Widget::preDisable(flag);
}

void SessWdg::postEnable( int flag )
{
    Widget::postEnable(flag);

    if(flag&TCntrNode::NodeConnect)
    {
	mToEn = true;
	attrAdd(new TFld("event","Events",TFld::String,TFld::FullText));
	attrAdd(new TFld("alarmSt","Alarm status",TFld::Integer,TFld::HexDec,"5","0"));
	attrAdd(new TFld("alarm","Alarm",TFld::String,TFld::NoFlag,"200"));
	mToEn = false;
    }
}

SessWdg *SessWdg::ownerSessWdg( bool base )
{
    if(nodePrev(true))
    {
	if(!base && dynamic_cast<SessPage*>(nodePrev()))	return NULL;
	return dynamic_cast<SessWdg*>(nodePrev());
    }
    return NULL;
}

SessPage *SessWdg::ownerPage()
{
    if(nodePrev(true) && dynamic_cast<SessPage*>(nodePrev()))	return (SessPage*)nodePrev();
    SessWdg *own = ownerSessWdg();
    if(own)	return own->ownerPage();
    return NULL;
}

string SessWdg::path( )	{ return ownerFullId(true)+"/wdg_"+id(); }

string SessWdg::ownerFullId( bool contr )
{
    SessWdg *ownW = ownerSessWdg();
    if(ownW) return ownW->ownerFullId(contr)+(contr?"/wdg_":"/")+ownW->id();
    SessPage *ownP = ownerPage();
    if(ownP) return ownP->ownerFullId(contr)+(contr?"/pg_":"/")+ownP->id();
    return string(contr?"/ses_":"/")+ownerSess()->id();
}

void SessWdg::setEnable( bool val )
{
    try { Widget::setEnable(val); } catch(...) { return; }

    if(!val)
    {
	//> Delete included widgets
	vector<string> ls;
	wdgList(ls);
	for(unsigned i_l = 0; i_l < ls.size(); i_l++)
	    wdgDel(ls[i_l]);
    }
    SessWdg *sw;
    if(val && (sw=ownerSessWdg(true)) && sw->process())
    {
	setProcess(true);
	sw->prcElListUpdate();
    }
}

void SessWdg::setProcess( bool val, bool lastFirstCalc )
{
    if(val && !enable()) setEnable(true);

    //> Prepare process function value level
    bool diff = (val!=process());
    if(val && !TSYS::strNoSpace(calcProg()).empty())
    {
	//>> Prepare function io structure
	TFunction fio(parent().at().calcId());
	//>>> Add generic io
	fio.ioIns(new IO("f_frq","Function calculate frequency (Hz)",IO::Real,IO::Default,"1000",false), 0);
	fio.ioIns(new IO("f_start","Function start flag",IO::Boolean,IO::Default,"0",false), 1);
	fio.ioIns(new IO("f_stop","Function stop flag",IO::Boolean,IO::Default,"0",false), 2);
	fio.ioIns(new IO("this","This widget's object for access to user's API",IO::Object,IO::Default), 3);
	//>>> Add calc widget's attributes
	vector<string> iwls, als;
	//>>> Self attributes check
	attrList(als);
	AutoHD<Widget> fulw = parentNoLink();
	for(unsigned i_a = 0; i_a < als.size(); i_a++)
	{
	    AutoHD<Attr> cattr = attrAt(als[i_a]);
	    if((fulw.at().attrPresent(als[i_a])&&fulw.at().attrAt(als[i_a]).at().flgSelf()&Attr::ProcAttr) || als[i_a] == "focus")
		fio.ioAdd(new IO(als[i_a].c_str(),cattr.at().name().c_str(),cattr.at().fld().typeIO(),IO::Output,"",false,("./"+als[i_a]).c_str()));
	}

	//>>> Include attributes check
	wdgList(iwls);
	for(unsigned i_w = 0; i_w < iwls.size(); i_w++)
	{
	    AutoHD<Widget> curw = wdgAt(iwls[i_w]);
	    curw.at().attrList(als);
	    for(unsigned i_a = 0; i_a < als.size(); i_a++)
	    {
		AutoHD<Attr> cattr = curw.at().attrAt(als[i_a]);
		if(cattr.at().flgSelf()&Attr::ProcAttr || als[i_a] == "focus")
		    fio.ioAdd(new IO((iwls[i_w]+"_"+als[i_a]).c_str(),(curw.at().name()+"."+cattr.at().name()).c_str(),
			cattr.at().fld().typeIO(),IO::Output,"",false,(iwls[i_w]+"/"+als[i_a]).c_str()));
	    }
	}
	fio.ioAdd(new IO("event","Event",IO::String,IO::Output));
	fio.ioAdd(new IO("alarmSt","Alarm status",IO::Integer,IO::Output,"",false,"./alarmSt"));
	fio.ioAdd(new IO("alarm","Alarm",IO::String,IO::Output,"",false,"./alarm"));

	//>> Compile function
	mWorkProg = "";
	try
	{
	    mWorkProg = SYS->daq().at().at(TSYS::strSepParse(calcLang(),0,'.')).at().
		compileFunc(TSYS::strSepParse(calcLang(),1,'.'),fio,calcProg(),mod->nodePath('.',true)+";");
	}
	catch(TError err)
	{
	    //>> Second compile try
	    try
	    {
		fio.setId(TSYS::path2sepstr(path(),'_'));
		mWorkProg = SYS->daq().at().at(TSYS::strSepParse(calcLang(),0,'.')).at().
		    compileFunc(TSYS::strSepParse(calcLang(),1,'.'),fio,calcProg(),mod->nodePath('.',true)+";");
	    }
	    catch(TError err)
	    { mess_err(nodePath().c_str(),_("Compile function '%s' by language '%s' for widget error: %s"),fio.id().c_str(),calcLang().c_str(),err.mess.c_str()); }
	}

	//>> Connect to compiled function
	if(mWorkProg.size())
	{
	    TValFunc::setFunc(&((AutoHD<TFunction>)SYS->nodeAt(mWorkProg)).at());
	    TValFunc::setUser(ownerSess()->user());
	    setO(3, new TCntrNodeObj(AutoHD<TCntrNode>(this),ownerSess()->user()));
	}
    }
    if(!val)
    {
	//>> Last calc, before any free
	if(diff && lastFirstCalc) calc(false, true);

	//>> Free function link
	mProc = false;
	MtxAlloc res(mCalcRes, true);
	TValFunc::setFunc(NULL);
    }

    //> Change process for included widgets
    vector<string> ls;
    wdgList(ls);
    for(unsigned i_l = 0; i_l < ls.size(); i_l++)
	((AutoHD<SessWdg>)wdgAt(ls[i_l])).at().setProcess(val, false);

    mProc = val;

    //>> Make process element's lists
    if(val) prcElListUpdate();

    //>> First calc, after all set
    if(diff && val && lastFirstCalc) calc(true, false);
}

string SessWdg::ico( )
{
    if(!parent().freeStat())	return parent().at().ico();
    return "";
}

string SessWdg::calcLang( )
{
    if(!parent().freeStat())	return parent().at().calcLang();
    return "";
}

string SessWdg::calcProg( )
{
    if(!parent().freeStat())	return parent().at().calcProg();
    return "";
}

int SessWdg::calcPer( )
{
    if(!parent().freeStat())	return parent().at().calcPer();
    return 0;
}

string SessWdg::resourceGet( const string &id, string *mime )
{
    string mimeType, mimeData;

    //Try load from the session table
    int off = 0;
    string db  = ownerSess()->parent().at().DB();
    string tbl = ownerSess()->parent().at().tbl()+"_ses";

    TConfig c_el(&mod->elPrjSes());
    TSYS::pathLev(path(), 0, true, &off);
    c_el.cfg("IDW").setS(path().substr(off));
    c_el.cfg("ID").setS("media://"+id);
    if(SYS->db().at().dataGet(db+"."+tbl,mod->nodePath()+tbl,c_el,false,true)) {
	off = 0;
	mimeData = c_el.cfg("IO_VAL").getS(); c_el.cfg("IO_VAL").setS("");
	mimeType = TSYS::strLine(mimeData, 0, &off);
	if(mime) *mime = mimeType;
	return TSYS::strDecode(mimeData.substr(off), TSYS::base64);
    }

    //Load original
    mimeData = parent().at().resourceGet(id, &mimeType);
    if(mime) *mime = mimeType;

    return mimeData;
}

void SessWdg::resourceSet( const string &id, const string &data, const string &mime )
{
    int off = 0;
    string db  = ownerSess()->parent().at().DB();
    string tbl = ownerSess()->parent().at().tbl()+"_ses";

    TConfig c_el(&mod->elPrjSes());
    TSYS::pathLev(path(), 0, true, &off);
    c_el.cfg("IDW").setS(path().substr(off));
    c_el.cfg("ID").setS("media://"+id);

    if(data.empty())	//Clear the media into the session table
        SYS->db().at().dataDel(db+"."+tbl, mod->nodePath()+tbl, c_el, false, false, true);
    else {		//Set the media into the session table
	c_el.cfg("IO_VAL").setS(mime+"\n"+TSYS::strEncode(data,TSYS::base64));
	SYS->db().at().dataSet(db+"."+tbl, mod->nodePath()+tbl, c_el, false, true);
    }
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

    if(enable() && !aid.empty() && ownerSess()->start() && attrPresent(aid))
    {
	AutoHD<Attr> attr = attrAt(aid);
	if(!(attr.at().flgGlob()&Attr::IsUser))
	    attr.at().setFlgSelf((Attr::SelfAttrFlgs)(attr.at().flgSelf()|Attr::SessAttrInh));
    }
}

AutoHD<Widget> SessWdg::wdgAt( const string &wdg, int lev, int off )
{
    //> Check for global
    if(lev == 0 && off == 0 && wdg.compare(0,1,"/") == 0)
	try { return (AutoHD<Widget>)ownerSess()->nodeAt(wdg,1); }
	catch(TError err) { return AutoHD<Widget>(); }

    return Widget::wdgAt(wdg, lev, off);
}

void SessWdg::pgClose( )
{
    try
    {
	if(!dynamic_cast<SessPage*>(this) && rootId() == "Box" && attrAt("pgGrp").at().getS() != "" && attrAt("pgOpenSrc").at().getS() != "")
	{
	    ((AutoHD<SessWdg>)mod->nodeAt(attrAt("pgOpenSrc").at().getS())).at().attrAt("pgOpen").at().setB(false);
	    attrAt("pgOpenSrc").at().setS("");
	}
    }catch(TError) { }

    vector<string> list;
    wdgList(list);
    for(unsigned i_w = 0; i_w < list.size(); i_w++)
	((AutoHD<SessWdg>)wdgAt(list[i_w])).at().pgClose();
}

void SessWdg::eventAdd( const string &ev )
{
    if(!enable() || !attrPresent("event")) return;
    pthread_mutex_lock(&ownerSess()->dataMtx());
    attrAt("event").at().setS(attrAt("event").at().getS()+ev);
    pthread_mutex_unlock(&ownerSess()->dataMtx());
}

string SessWdg::eventGet( bool clear )
{
    if(!enable() || !attrPresent("event")) return "";

    pthread_mutex_lock(&ownerSess()->dataMtx());
    string rez = attrAt("event").at().getS();
    if(clear)	attrAt("event").at().setS("");
    pthread_mutex_unlock(&ownerSess()->dataMtx());

    return rez;
}

void SessWdg::alarmSet( bool isSet )
{
    int aStCur  = attrAt("alarmSt").at().getI( );
    string aCur = attrAt("alarm").at().getS( );
    int alev = atoi(TSYS::strSepParse(aCur,0,'|').c_str()) & 0xFF;
    int atp  = atoi(TSYS::strSepParse(aCur,3,'|').c_str()) & 0xFF;
    int aqtp = isSet ? atp : (aStCur>>16) & 0xFF;

    vector<string> lst;
    //> Included widgets process
    wdgList( lst );
    for(unsigned i_w = 0; i_w < lst.size(); i_w++)
    {
	int iacur = wdgAt(lst[i_w]).at().attrAt("alarmSt").at().getI();
	alev = vmax(alev, iacur&0xFF);
	atp |= (iacur>>8) & 0xFF;
	aqtp |= (iacur>>16) & 0xFF;
    }

    attrAt("alarmSt").at().setI((alev && atp) ? (aqtp<<16)|(atp<<8)|alev : 0);

    if(ownerSessWdg(true)) ownerSessWdg(true)->alarmSet();
    if(isSet) ownerSess()->alarmSet(path(), aCur);
}

void SessWdg::alarmQuittance( uint8_t quit_tmpl, bool isSet )
{
    int alarmSt = attrAt("alarmSt").at().getI();
    if(!((((alarmSt>>16)&0xFF)^quit_tmpl)&((alarmSt>>16)&0xFF))) return;

    //> Self quittance
    attrAt("alarmSt").at().setI(alarmSt & (quit_tmpl<<16|0xFFFF));

    vector<string> lst;
    //> Include widgets quittance
    wdgList( lst );
    for(unsigned i_w = 0; i_w < lst.size(); i_w++)
	((AutoHD<SessWdg>)wdgAt(lst[i_w])).at().alarmQuittance(quit_tmpl);

    if(isSet && ownerSessWdg(true)) ownerSessWdg(true)->alarmSet();
}

void SessWdg::prcElListUpdate( )
{
    vector<string> ls;

    wdgList(ls);
    MtxAlloc resDt(ownerSess()->dataMtx(), true);
    mWdgChldAct.clear();
    for(unsigned i_l = 0; i_l < ls.size(); i_l++)
	try { if(((AutoHD<SessWdg>)wdgAt(ls[i_l])).at().process()) mWdgChldAct.push_back(ls[i_l]); }
	catch(TError err) { }
    resDt.unlock();

    attrList(ls);
    mAttrLnkLs.clear();
    for(unsigned i_a = 0; i_a < ls.size(); i_a++)
    {
	AutoHD<Attr> attr = attrAt(ls[i_a]);
	if(attr.at().flgSelf()&(Attr::CfgConst|Attr::CfgLnkIn|Attr::CfgLnkOut))
	    mAttrLnkLs.push_back(ls[i_a]);
    }
}

void SessWdg::getUpdtWdg( const string &ipath, unsigned int tm, vector<string> &els )
{
    string wpath = ipath + "/" + id();
    if(modifChk(tm,mMdfClc)) els.push_back(wpath);

    MtxAlloc resDt(ownerSess()->dataMtx(), true);
    for(unsigned i_ch = 0; i_ch < mWdgChldAct.size(); i_ch++)
	try
	{
	    AutoHD<SessWdg> wdg = wdgAt(mWdgChldAct[i_ch]);
	    resDt.unlock();
	    wdg.at().getUpdtWdg(wpath, tm, els);
	    resDt.lock();
	}
	catch(TError err) { }
}

unsigned int SessWdg::modifVal( Attr &cfg )
{
    if(atoi(cfg.fld().reserve().c_str())) mMdfClc = mCalcClk;
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

    //> Calculate include widgets
    MtxAlloc resDt(ownerSess()->dataMtx(), true);
    for(unsigned i_l = 0; i_l < mWdgChldAct.size(); i_l++)
	try
	{
	    AutoHD<SessWdg> wdg = wdgAt(mWdgChldAct[i_l]);
	    resDt.unlock();
	    wdg.at().calc(first, last);
	    resDt.lock();
	}
	catch(TError err) { }
    resDt.unlock();

    try
    {
	int pgOpenPrc = -1;

	//> Load events to process
	if(!((ownerSess()->calcClk())%(vmax(calcPer()/ownerSess()->period(),1))) || first || last)
	{
	    string wevent = eventGet(true);
	    //> Process input links and constants
	    AutoHD<Attr> attr;
	    AutoHD<TVal> vl;
	    inLnkGet = true;
	    for(unsigned i_a = 0; i_a < mAttrLnkLs.size(); i_a++)
	    {
		attr = attrAt(mAttrLnkLs[i_a]);
		if(attr.at().flgSelf()&Attr::CfgConst && !attr.at().cfgVal().empty())	attr.at().setS(attr.at().cfgVal());
		else if(attr.at().flgSelf()&Attr::CfgLnkIn && !attr.at().cfgVal().empty())
		{
		    obj_tp = TSYS::strSepParse(attr.at().cfgVal(),0,':')+":";
		    if(obj_tp == "val:")	attr.at().setS(attr.at().cfgVal().substr(obj_tp.size()));
		    else if(obj_tp == "prm:")
		    {
			vl = SYS->daq().at().attrAt(attr.at().cfgVal().substr(obj_tp.size()),0,true);
			if(vl.freeStat()) { attr.at().setS(EVAL_STR); continue; }

			if(attr.at().flgGlob()&Attr::Address)
			{
			    string nP = vl.at().nodePath(0,true);
			    attr.at().setS((nP.size()&&nP[nP.size()-1]=='/')?nP.substr(0,nP.size()-1):"");// "/DAQ"+attr.at().cfgVal().substr(obj_tp.size()));
			}
			else attr.at().set(vl.at().get());
		    }
		    else if(obj_tp == "wdg:")
			try { attr.at().set(attrAt(attr.at().cfgVal().substr(obj_tp.size()),0).at().get()); }
			catch(TError err) { attr.at().setS(EVAL_STR); continue; }
		    else if(obj_tp == "arh:" && attr.at().flgGlob()&Attr::Address)
			attr.at().setS("/Archive/va_"+attr.at().cfgVal().substr(obj_tp.size()));
		}
		else if(attr.at().flgSelf()&Attr::CfgLnkIn) attr.at().setS(EVAL_STR);
		attr.free();
	    }
	    inLnkGet = false;

	    if(TValFunc::func())
	    {
		//> Load events to calc procedure
		int evId = ioId("event");
		if(evId >= 0)	setS(evId, wevent);

		//>> Load data to calc area
		setR(0, 1000.0/(ownerSess()->period()*vmax(calcPer()/ownerSess()->period(),1)));
		setB(1, first);
		setB(2, last);
		for(int i_io = 4; i_io < ioSize( ); i_io++)
		{
		    if(func()->io(i_io)->rez().empty()) continue;
		    sw_attr = TSYS::pathLev(func()->io(i_io)->rez(), 0);
		    s_attr  = TSYS::pathLev(func()->io(i_io)->rez(), 1);
		    attr = (sw_attr==".") ? attrAt(s_attr) : wdgAt(sw_attr).at().attrAt(s_attr);
		    set(i_io,attr.at().get());
		}

		//>> Calc
		setMdfChk(true);
		TValFunc::calc();

		//>> Save data from calc area
		for(int i_io = 4; i_io < ioSize( ); i_io++)
		{
		    if(func()->io(i_io)->rez().empty() || !ioMdf(i_io)) continue;
		    sw_attr = TSYS::pathLev(func()->io(i_io)->rez(), 0);
		    s_attr  = TSYS::pathLev(func()->io(i_io)->rez(), 1);
		    attr = (sw_attr==".") ? attrAt(s_attr) : wdgAt(sw_attr).at().attrAt(s_attr);

		    if(s_attr == "pgOpen" && attr.at().getB() != getB(i_io)) { pgOpenPrc = i_io; continue; }
		    attr.at().set(get(i_io));
		}
		//>> Save events from calc procedure
		if(evId >= 0) wevent = getS(evId);
	    }

	    res.unlock();

	    //> Close page process by pgOpen changing
	    if(pgOpenPrc >= 0) attrAt("pgOpen").at().setB(getB(pgOpenPrc));

	    //>> Process widget's events
	    if(!wevent.empty())
	    {
		int t_off;
		bool isPg = dynamic_cast<SessPage*>(this);
		string sevup, sev, sev_ev, sev_path, sprc_lst, sprc, sprc_ev, sprc_path;
		for(int el_off = 0; (sev=TSYS::strSepParse(wevent,0,'\n',&el_off)).size(); )
		{
		    //>> Check for process events
		    t_off = 0;
		    sev_ev   = TSYS::strSepParse(sev, 0, ':', &t_off);
		    sev_path = TSYS::strSepParse(sev, 0, ':', &t_off);
		    sprc_lst = attrAt("evProc").at().getS();
		    bool evProc = false;
		    for(int elp_off = 0; (sprc=TSYS::strSepParse(sprc_lst,0,'\n',&elp_off)).size(); )
		    {
			t_off = 0;
			sprc_ev   = TSYS::strSepParse(sprc, 0, ':', &t_off);
			sprc_path = TSYS::strSepParse(sprc, 0, ':', &t_off);
			if(sprc_ev == sev_ev && (sprc_path == "*" || sprc_path == sev_path))
			{
			    sprc_path = TSYS::strSepParse(sprc, 0, ':', &t_off);
			    SessWdg *sev = this;
			    if(!sev_path.empty()) sev = (TSYS::pathLev(sev_path,0).compare(0,4,"ses_") == 0) ?
					    &((AutoHD<SessWdg>)mod->nodeAt(sev_path)).at() :  &((AutoHD<SessWdg>)nodeAt(sev_path)).at();
			    ownerSess()->uiComm(sprc_path, TSYS::strSepParse(sprc,0,':',&t_off), sev);
			    evProc = true;
			}
		    }
		    if(!evProc)
		    {
			if(!isPg) sevup += sev_ev + ":/" + id() + sev_path + "\n";
			else sevup += sev_ev + ":" + (TSYS::pathLev(sev_path,0).compare(0,4,"ses_")?path():"") + sev_path + "\n";
		    }
		}
		//>> Put left events to parent widget
		SessWdg *owner = ownerSessWdg(true);
		if(owner && !sevup.empty())
		{
		    if(!isPg) owner->eventAdd(sevup);
		    //>> Up event to the upper page
		    else
		    {
			vector<string> lst = ownerSess()->openList();
			string prev;
			for(unsigned i_f = 0; i_f < lst.size(); i_f++)
			    if(lst[i_f] == path())
			    {
				if(prev.size()) ((AutoHD<SessPage>)mod->nodeAt(prev)).at().eventAdd(sevup);
				break;
			    }
			    else prev = lst[i_f];
		    }
		}
	    }

	    //> Generic calc
	    Widget::calc(this);
	}
    }
    catch(TError err)
    {
	res.unlock();
	mess_err(err.cat.c_str(), err.mess.c_str());
	mess_err(nodePath().c_str(), _("Widget calculation error. Process is disabled."));
	if(!last) setProcess(false);
    }
}

bool SessWdg::attrChange( Attr &cfg, TVariant prev )
{
    Widget::attrChange(cfg, prev);

    //> Special session attributes process
    //>> Focus attribute process for active active
    if(cfg.id() == "active" && cfg.getB() && !cfg.owner()->attrPresent("focus"))
	cfg.owner()->attrAdd(new TFld("focus","Focus",TFld::Boolean,TFld::NoFlag,"1","false","","","-2"));
    //> Alarm event for widget process
    else if(cfg.id() == "alarm" && enable() && !prev.isNull()) alarmSet(true);
    //> Alarm status process
    else if(cfg.id() == "alarmSt" && cfg.getI()&0x1000000)
    {
	int tmpl = ~(cfg.getI()&0xFF);
	cfg.setI(prev.getI(), false, true);
	ownerSess()->alarmQuittance(path(), tmpl);
    }

    //> External link process
    if(!inLnkGet && !prev.isNull() && cfg.flgSelf()&Attr::CfgLnkOut && !cfg.cfgVal().empty())
    {
	if(cfg.flgSelf()&Attr::SessAttrInh) cfg.setFlgSelf((Attr::SelfAttrFlgs)(cfg.flgSelf()&(~Attr::SessAttrInh)));
	string obj_tp = TSYS::strSepParse(cfg.cfgVal(),0,':') + ":";
	try
	{
	    if(obj_tp == "prm:")	SYS->daq().at().attrAt(cfg.cfgVal().substr(obj_tp.size()),0,true).at().set(cfg.get());
	    else if(obj_tp == "wdg:")	attrAt(cfg.cfgVal().substr(obj_tp.size()),0).at().set(cfg.get());
	}catch(...)	{ }
    }

    return true;
}

TVariant SessWdg::objFuncCall( const string &iid, vector<TVariant> &prms, const string &user )
{
    // TCntrNodeObj ownerSess( ) - Get session object
    if(iid == "ownerSess")	return new TCntrNodeObj(ownerSess(),user);
    // TCntrNodeObj ownerPage( ) - Get page-owner object
    if(iid == "ownerPage")
    {
	SessPage *opg = ownerPage();
	if(!opg) return 0;
	return new TCntrNodeObj(opg,user);
    }
    // TCntrNodeObj ownerWdg(bool base) - Get widget-owner object
    //  base - include widgets and pages for true
    if(iid == "ownerWdg")
    {
	SessWdg *wdg = ownerSessWdg(prms.size() ? prms[0].getB() : 0);
	if(!wdg) return 0;
	return new TCntrNodeObj(wdg, user);
    }
    // TCntrNodeObj wdgAdd(string wid, string wname, string parent) - add new widget
    //  wid - widget identifier
    //  wname - widget name
    //  parent - parent widget
    if(iid == "wdgAdd" && prms.size() >= 3)
    {
	try
	{
	    //> Create widget
	    wdgAdd(prms[0].getS(), prms[1].getS(), prms[2].getS());
	    //> Enable widget
	    AutoHD<SessWdg> nw = wdgAt(prms[0].getS());
	    nw.at().setEnable(true);

	    return new TCntrNodeObj(&nw.at(), user);
	}
	catch(TError err){ return false; }
    }
    // bool wdgDel(string wid) - delete widget, return true for success
    //  wid - widget identifier
    if(iid == "wdgDel" && prms.size())
    {
	try { wdgDel(prms[0].getS()); }
	catch(TError err){ return false; }
	return true;
    }
    // TCntrNodeObj wdgAt(string wid, bool byPath = false) - attach to child widget or global by <path>
    //  wid - widget identifier
    //  byPath - attach by absolute or relative path. First item of absolute path (session or project id) is passed.
    if(iid == "wdgAt" && prms.size())
    {
	try { return new TCntrNodeObj(wdgAt(prms[0].getS(),(prms.size()>1&&prms[1].getB())?0:-1),user); }
	catch(TError err){ }
	return false;
    }
    // bool attrPresent(string attr) - check for attribute <attr> present.
    //  attr - checked attribute
    if(iid == "attrPresent" && prms.size())	return attrPresent(prms[0].getS());
    // ElTp attr(string attr) - get attribute <attr> value.
    //  attr - readed attribute
    if(iid == "attr" && prms.size())
    {
	if(!attrPresent(prms[0].getS())) return string("");
	return attrAt(prms[0].getS()).at().get();
    }
    // TCntrNodeObj attrSet(string attr, ElTp vl)
    if(iid == "attrSet" && prms.size() >= 2)
    {
	if(attrPresent(prms[0].getS())) attrAt(prms[0].getS()).at().set(prms[1]);
	return new TCntrNodeObj(this, user);
    }
    // string link(string attr, bool prm = false) - get link for attribute or attribute block (prm)
    //  attr - attribute identifier
    //  prm  - attribute block for true
    if(iid == "link" && prms.size())
    {
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
    if(iid == "linkSet" && prms.size() >= 2)
    {
	XMLNode req("set");
	req.setAttr("user", "root")->setText(prms[1].getS());
	if(prms.size() >= 3 && prms[2].getB()) req.setAttr("path", TSYS::strMess("/links/lnk/pr_%s",prms[0].getS().c_str()));
	else req.setAttr("path", TSYS::strMess("/links/lnk/el_%s",prms[0].getS().c_str()));
	return cntrCmdLinks(&req);
    }
    // string mime(string addr, string type = "") - read mime data from the session table or primal source
    //  addr - address to mime by link attribute to mime or direct mime address
    //  type - return attribute for mime type store
    if(iid == "mime" && prms.size() >= 1)
    {
	string addr = prms[0], rez, tp;
	//Check for likely attribute
	if(attrPresent(addr))
	{
	    AutoHD<Attr> a = attrAt(addr);
	    if(a.at().type() == TFld::String && a.at().flgGlob()&Attr::Image) addr = a.at().getS();
	}
	rez = resourceGet(addr, &tp);
	if(prms.size() >= 2) { prms[1].setS(tp); prms[1].setModify(); }

	return rez;
    }
    // int mimeSet(string addr, string data, string type = "") - set or clear data to the session table
    //  addr - address to mime by link attribute to mime or direct mime address
    //  data - set to the mime data, empty for clear into 
    //  type - mime type for store data
    if(iid == "mimeSet" && prms.size() >= 2)
    {
	string addr = prms[0];
	//Check for likely attribute
	AutoHD<Attr> a;
	if(attrPresent(addr))
	{
	    a = attrAt(addr);
	    if(a.at().type() == TFld::String && a.at().flgGlob()&Attr::Image) addr = a.at().getS();
	    else a.free();
	}
	resourceSet(addr, prms[1], (prms.size()>=3)?prms[2]:"");	//???? Store to the session's context table
	if(!a.freeStat()) a.at().setS(a.at().getS(), false, true);	//Mark the attribute to modify state

	return (int)prms[1].getS().size();
    }

    //> Request to primitive
    TVariant rez = objFuncCall_w(iid, prms, user, this);
    if(!rez.isNull())	return rez;

    return TCntrNode::objFuncCall(iid, prms, user);
}

bool SessWdg::cntrCmdServ( XMLNode *opt )
{
    string a_path = opt->attr("path");
    if(a_path == "/serv/attr")		//Attribute's value operations
    {
	if(ctrChkNode(opt,"get",R_R_R_,"root","UI",SEC_RD))	//Get values
	{
	    unsigned tm = strtoul(opt->attr("tm").c_str(), 0, 10);
	    if(!tm)
	    {
		opt->childAdd("el")->setAttr("id","perm")->setAttr("p","-3")->
		    setText(i2s(ownerSess()->sec.at().access(opt->attr("user"),SEC_RD|SEC_WR,owner(),grp(),permit())));
		if(dynamic_cast<SessPage*>(this)) opt->childAdd("el")->setAttr("id", "name")->setAttr("p", "-4")->setText(name());
	    }
	    if(!tm || modifChk(tm,mMdfClc))
	    {
		AutoHD<Attr> attr;
		vector<string> als;
		attrList(als);
		for(unsigned i_l = 0; i_l < als.size(); i_l++)
		{
		    attr = attrAt(als[i_l]);
		    if(!(attr.at().flgGlob()&Attr::IsUser) && modifChk(tm,attr.at().modif()) && atoi(attr.at().fld().reserve().c_str()))
			opt->childAdd("el")->setAttr("id", als[i_l].c_str())->
					     setAttr("p", attr.at().fld().reserve())->
					     setText(attr.at().getS());
		}
	    }
	}
	else if(ctrChkNode(opt,"set",permit(),owner().c_str(),grp().c_str(),SEC_WR))	//Set values
	{
	    if(ownerSess()->user() != opt->attr("user")) ownerSess()->setUser(opt->attr("user"));
	    for(unsigned i_ch = 0; i_ch < opt->childSize(); i_ch++)
	    {
		string aid = opt->childGet(i_ch)->attr("id");
		if(aid == "event") eventAdd(opt->childGet(i_ch)->text()+"\n");
		else attrAt(aid).at().setS(opt->childGet(i_ch)->text());
	    }
	}
    }
    else if(a_path == "/serv/attrBr" && ctrChkNode(opt,"get",R_R_R_,"root","UI",SEC_RD))//Get attributes all updated elements' of the branch
    {
	unsigned tm = strtoul(opt->attr("tm").c_str(), NULL, 10);
	bool     fullTree = atoi(opt->attr("FullTree").c_str());
	int perm = ownerSess()->sec.at().access(opt->attr("user"),(tm?SEC_RD:SEC_RD|SEC_WR),owner(),grp(),permit());

	//>> Self attributes put
	if(!tm || modifChk(tm,mMdfClc))
	{
	    if(!tm)
	    {
		if(dynamic_cast<SessPage*>(this)) opt->childAdd("el")->setAttr("id","name")->setAttr("p","-4")->setText(name());
		opt->childAdd("el")->setAttr("id","perm")->setAttr("p","-3")->setText(i2s(perm));
	    }
	    AutoHD<Attr> attr;
	    vector<string> als;
	    attrList(als);
	    for(unsigned i_l = 0; i_l < als.size(); i_l++)
	    {
		attr = attrAt(als[i_l]);
		if(!(attr.at().flgGlob()&Attr::IsUser) && modifChk(tm,attr.at().modif()) && atoi(attr.at().fld().reserve().c_str()))
		    opt->childAdd("el")->setAttr("id", als[i_l].c_str())->
				     setAttr("p", attr.at().fld().reserve())->
				     setText(attr.at().getS());
	    }
	}

	//>> Child widgets process
	if(enable() && (perm&SEC_RD))
	{
	    vector<string>	lst;
	    wdgList(lst);

	    for(unsigned i_f = 0; i_f < lst.size(); i_f++)
	    {
		AutoHD<SessWdg> iwdg = wdgAt(lst[i_f]);
		XMLNode *wn = new XMLNode("get");
		wn->setAttr("path", a_path)->setAttr("user", opt->attr("user"))->
		    setAttr("tm", opt->attr("tm"))->setAttr("FullTree", opt->attr("FullTree"));
		iwdg.at().cntrCmdServ(wn);
		if(wn->childSize() || fullTree)
		{
		    wn->setName("w")->attrDel("path")->attrDel("user")->
			attrDel("rez")->attrDel("tm")->attrDel("FullTree")->setAttr("id", lst[i_f]);
		    opt->childAdd(wn);
		}
		else delete wn;
	    }
	}
    }
    else return Widget::cntrCmdServ(opt);

    return true;
}

bool SessWdg::cntrCmdGeneric( XMLNode *opt )
{
    //> Get page info
    if( opt->name() == "info" )
    {
	Widget::cntrCmdGeneric(opt);
	ctrMkNode("fld",opt,1,"/wdg/st/proc",_("Process"),RWRWR_,owner().c_str(),grp().c_str(),1,"tp","bool");
	return true;
    }

    //> Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/wdg/st/proc")
    {
	if(ctrChkNode(opt,"get",RWRWR_,owner().c_str(),grp().c_str(),SEC_RD)) opt->setText(i2s(process()));
	if(ctrChkNode(opt,"set",RWRWR_,owner().c_str(),grp().c_str(),SEC_WR)) setProcess(atoi(opt->text().c_str()));
    }
    else return Widget::cntrCmdGeneric(opt);

    return true;
}

bool SessWdg::cntrCmdAttributes( XMLNode *opt, Widget *src )
{
    //> Get page info
    if(opt->name() == "info")
    {
	Widget::cntrCmdAttributes(opt);
	if(ctrMkNode("area",opt,-1,"/attr",_("Attributes")))
	{
	    //>> Properties form create
	    vector<string> list_a;
	    attrList(list_a);
	    for(unsigned i_el = 0; i_el < list_a.size(); i_el++)
	    {
		XMLNode *el = attrAt(list_a[i_el]).at().fld().cntrCmdMake(opt,"/attr",-1,owner().c_str(),grp().c_str(),permit()|R_R_R_);
		if(el) el->setAttr("len", "")->setAttr("wdgFlg", i2s(attrAt(list_a[i_el]).at().flgGlob()));
	    }
	}
	return true;
    }

    //> Process command to page
    string a_path = opt->attr("path");
    if(a_path.substr(0,6) == "/attr/")
    {
	AutoHD<Attr> attr = attrAt(TSYS::pathLev(a_path,1));
	if(ctrChkNode(opt,"get",((attr.at().fld().flg()&TFld::NoWrite)?(permit()&~0222):permit())|R_R_R_,owner().c_str(),grp().c_str(),SEC_RD))
	    opt->setText(attr.at().getS());
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

    //> Get page info
    if(opt->name() == "info")
    {
	cntrCmdGeneric(opt);
	cntrCmdAttributes(opt);
	if(!parent( ).freeStat()) cntrCmdLinks(opt,true);
	return;
    }
    if(!(cntrCmdGeneric(opt) || cntrCmdAttributes(opt) || (parent( ).freeStat() ? false : cntrCmdLinks(opt))))
	TCntrNode::cntrCmdProc(opt);
}
