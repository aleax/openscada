
//OpenSCADA module UI.VCAEngine file: project.cpp
/***************************************************************************
 *   Copyright (C) 2007-2022 by Roman Savochenko, <roman@oscada.org>       *
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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <algorithm>

#include <tsys.h>

#include "vcaengine.h"
#include "project.h"

using namespace VCA;

//************************************************
//* Project					 *
//************************************************
Project::Project( const string &id, const string &name, const string &lib_db ) :
    TConfig(&mod->elProject()), enableByNeed(false), mId(cfg("ID")), mDB(lib_db), mPermit(cfg("PERMIT").getId()),
    mPer(cfg("PER").getId())/*, mFlgs(cfg("FLGS").getId())*/, mStyleIdW(cfg("STYLE").getId()),
    mEnable(false), mFuncM(true)
{
    mId = id;
    cfg("NAME").setS(name);
    cfg("DB_TBL").setS(string("prj_")+id);	//????[v1.0] Remove, saved for the new projects work on old OpenSCADA versions
    mPage= grpAdd("pg_");
}

Project::~Project( )
{

}

TCntrNode &Project::operator=( const TCntrNode &node )
{
    const Project *src_n = dynamic_cast<const Project*>(&node);
    if(!src_n) return *this;

    //Copy generic configuration
    exclCopy(*src_n, "ID;DB_TBL;");
    setDB(src_n->DB());
    mDB_MimeSrc = src_n->fullDB();

    if(!src_n->enable()) return *this;
    if(!enable()) setEnable(true);

    //Styles copy
    const_cast<Project*>(this)->mStRes.lock(true);
    const_cast<Project*>(src_n)->mStRes.lock(false);
    mStProp = src_n->mStProp;
    const_cast<Project*>(this)->mStRes.unlock();
    const_cast<Project*>(src_n)->mStRes.unlock();

    //Copy included pages with tracing the page link errors and repeat the error pages copy after all other
    vector<string> pls, lnkErrEls;
    src_n->list(pls);
    for(unsigned iTr = 1; true; ++iTr) {
	for(unsigned iP = 0; iP < pls.size(); ++iP)
	    try {
		if(!present(pls[iP])) add(pls[iP], "");
		(TCntrNode&)at(pls[iP]).at() = (TCntrNode&)src_n->at(pls[iP]).at();
	    }
	    catch(TError &err) {
		if(err.cod == Engine::NotResLnk || (err.cod == TError::Core_NoNode && src_n->at(pls[iP]).at().prjFlags()&Page::Link))
		    lnkErrEls.push_back(pls[iP]);
		else throw;
	    }
	if(lnkErrEls.empty() || iTr >= 2) break;
	pls = lnkErrEls; lnkErrEls.clear();
    }

    return *this;
}

void Project::postEnable( int flag )
{
    if(flag&TCntrNode::NodeRestore) setEnable(true);
}

void Project::preDisable( int flag )
{
    if(mHerit.size())
	throw TError(nodePath().c_str(), _("The project '%s' is used now by %d sessions!"), id().c_str(), mHerit.size());
    if(enable()) setEnable(false);
}

void Project::postDisable( int flag )
{
    if(flag&(NodeRemove|NodeRemoveOnlyStor)) {
	//Delete libraries record
	TBDS::dataDel(DB(flag&NodeRemoveOnlyStor)+"."+mod->prjTable(), mod->nodePath()+"PRJ", *this, TBDS::UseAllKeys);

	//Delete function's files
	TBDS::dataDelTbl(fullDB(flag&NodeRemoveOnlyStor), mod->nodePath()+tbl());
	TBDS::dataDelTbl(fullDB(flag&NodeRemoveOnlyStor)+"_io", mod->nodePath()+tbl()+"_io");
	TBDS::dataDelTbl(fullDB(flag&NodeRemoveOnlyStor)+"_uio", mod->nodePath()+tbl()+"_uio");
	TBDS::dataDelTbl(fullDB(flag&NodeRemoveOnlyStor)+"_incl", mod->nodePath()+tbl()+"_incl");
	TBDS::dataDelTbl(fullDB(flag&NodeRemoveOnlyStor)+"_mime", mod->nodePath()+tbl()+"_mime");
	TBDS::dataDelTbl(fullDB(flag&NodeRemoveOnlyStor)+"_ses", mod->nodePath()+tbl()+"_ses");
	TBDS::dataDelTbl(fullDB(flag&NodeRemoveOnlyStor)+"_stl", mod->nodePath()+tbl()+"_stl");

	if(flag&NodeRemoveOnlyStor) { setStorage(mDB, "", true); return; }
    }
}

string Project::name( ) const
{
    string rezs = cfg("NAME").getS();
    return rezs.size() ? rezs : mId;
}

string Project::owner( ) const	{ return SYS->security().at().usrPresent(cfg("USER").getS()) ? cfg("USER").getS() : string("root"); }

string Project::grp( ) const	{ return SYS->security().at().grpPresent(cfg("GRP").getS()) ? cfg("GRP").getS() : string("UI"); }

string Project::getStatus( )
{
    string rez = enable() ? _("Enabled. ") : _("Disabled. ");

    vector<string> tls;
    list(tls);
    time_t maxTm = 0;
    for(unsigned iT = 0; iT < tls.size(); iT++)
	maxTm = vmax(maxTm, at(tls[iT]).at().timeStamp());
    rez += TSYS::strMess(_("Used %d. "), mHerit.size());
    rez += TSYS::strMess(_("Date of modification %s. "), atm2s(maxTm).c_str());

    return rez;
}

void Project::setOwner( const string &it )
{
    cfg("USER").setS(it);
    //Update librarie's group
    if(SYS->security().at().grpAt("UI").at().user(it)) setGrp("UI");
    else {
	vector<string> gls;
	SYS->security().at().usrGrpList(owner(),gls);
	setGrp(gls.size()?gls[0]:"UI");
    }
    modif();
}

void Project::setFullDB( const string &vl )
{
    int off = vl.size();
    cfg("DB_TBL").setS(TSYS::strParseEnd(vl,0,".",&off));
    setDB(vl.substr(0,off+1));
}

void Project::load_( TConfig *icfg )
{
    MtxAlloc fRes(funcM(), true);	//Prevent multiple entry

    if(!SYS->chkSelDB(DB())) throw TError();

    mess_debug(nodePath().c_str(), _("Loading the project."));

    if(icfg) *(TConfig*)this = *icfg;
    else TBDS::dataGet(DB()+"."+mod->prjTable(), mod->nodePath()+"PRJ", *this);

    //Create new pages
    map<string, bool> itReg;
    TConfig cEl(&mod->elPage());
    cEl.cfg("PROC").setExtVal(true);
    //cEl.cfgViewAll(false);
    cEl.cfg("OWNER").setS("/"+id(), true);
    for(int fldCnt = 0; TBDS::dataSeek(fullDB(),mod->nodePath()+tbl()+"/",fldCnt++,cEl,TBDS::UseCache); ) {
	string fId = cEl.cfg("ID").getS();
	if(!present(fId)) add(fId, "", "");
	at(fId).at().load(&cEl);
	itReg[fId] = true;
    }

    //Check for remove items removed from the DB
    if(SYS->chkSelDB(SYS->selDB(),true)) {
	vector<string> itLs;
	list(itLs);
	for(unsigned iIt = 0; iIt < itLs.size(); iIt++)
	    if(itReg.find(itLs[iIt]) == itReg.end())
		del(itLs[iIt]);
    }

    //Load styles
    ResAlloc res(mStRes, true);
    // Try to load the new style table
    int propLd = 0;
    TConfig cStls(&mod->elPrjStls());
    cStls.cfg("VAL").setNoTransl(false);
    cStls.cfg("VAL").setExtVal(true);
    for(int fldCnt = 0; TBDS::dataSeek(fullDB()+"_stls",nodePath()+tbl()+"_stls",fldCnt++,cStls,TBDS::UseCache); ++propLd) {
	int IDS = cStls.cfg("IDS").getI();
	string ID = cStls.cfg("ID").getS();
	vector<string> &prop = mStProp[ID];
	while(prop.size() <= IDS) prop.push_back("");
	prop[IDS] = (ID==STL_PRM_NM) ? cStls.cfg("VAL").getS() : cStls.cfg("VAL").getS(TCfg::ExtValOne);
    }
    // Load old style table at missing the new one
    if(!propLd) {
	TConfig cStl(&mod->elPrjStl());
	string svl;
	vector<string> vlst;
	for(int fldCnt = 0; TBDS::dataSeek(fullDB()+"_stl",nodePath()+tbl()+"_stl",fldCnt++,cStl,TBDS::UseCache); ) {
	    vlst.clear();
	    bool notEmpty = false;
	    for(int iS = Project::StlMaximum-1; iS >= 0; --iS) {
		if((svl=cStl.cfg("V_"+i2s(iS)).getS()).empty() && !notEmpty) continue;
		vlst.push_back(svl);
		if(!svl.empty()) notEmpty = true;
	    }
	    reverse(vlst.begin(), vlst.end());
	    mStProp[cStl.cfg("ID").getS()] = vlst;
	}
    }
}

void Project::save_( )
{
    if(enableByNeed)	return;

    TBDS::dataSet(DB()+"."+mod->prjTable(), mod->nodePath()+"PRJ", *this);

    //Resources copy
    if(mDB_MimeSrc.size() || DB(true).size()) {
	if(mDB_MimeSrc.empty()) mDB_MimeSrc = DB(true);

	vector<string> pls;
	resourceDataList(pls, mDB_MimeSrc);
	string mimeType, mimeData, srcDB = mDB_MimeSrc;
	mDB_MimeSrc = "";
	for(unsigned iM = 0; iM < pls.size(); iM++) {
	    resourceDataGet(pls[iM], mimeType, &mimeData, srcDB);
	    resourceDataSet(pls[iM], mimeType, mimeData, DB());
	}
    }

    //Session's data copy
    if(DB(true).size()) {
	string wtbl = tbl() + "_ses";
	TConfig cEl(&mod->elPrjSes());
	for(int fldCnt = 0; TBDS::dataSeek(DB(true)+"."+wtbl,"",fldCnt,cEl,TBDS::UseCache); fldCnt++)
	    TBDS::dataSet(DB()+"."+wtbl, "", cEl);
    }

    //Saving the styles
    ResAlloc res(mStRes, false);
    TConfig cStl(&mod->elPrjStls());
    vector<string> stls = mStProp[STL_PRM_NM];
    for(unsigned iS = 0; iS < stls.size(); iS++) {
	if(stls[iS].empty()) continue;
	cStl.cfg("IDS").setI(iS);
	for(map<string, vector<string> >::iterator iStPrp = mStProp.begin(); iStPrp != mStProp.end(); iStPrp++) {
	    cStl.cfg("ID").setS(iStPrp->first);
	    cStl.cfg("VAL").setNoTransl((iStPrp->first!=STL_PRM_NM));	//!!!! Only name translate now
	    cStl.cfg("VAL").setS(iStPrp->second[iS]);
	    TBDS::dataSet(fullDB()+"_stls", nodePath()+tbl()+"_stls", cStl);
	}
    }
    // Checking for the removed properties
    cStl.cfgViewAll(false);
    for(int fldCnt = 0; TBDS::dataSeek(fullDB()+"_stls",nodePath()+tbl()+"_stls",fldCnt++,cStl); )
	if(cStl.cfg("IDS").getI() >= stls.size() || stls[cStl.cfg("IDS").getI()].empty() ||
		mStProp.find(cStl.cfg("ID").getS()) == mStProp.end()) {
	    if(!TBDS::dataDel(fullDB()+"_stls",nodePath()+tbl()+"_stls",cStl,TBDS::UseAllKeys|TBDS::NoException)) break;
	    fldCnt--;
	}

    // Old variant
    /*TConfig cStl(&mod->elPrjStl());
    for(map<string, vector<string> >::iterator iStPrp = mStProp.begin(); iStPrp != mStProp.end(); iStPrp++) {
	cStl.cfg("ID").setS(iStPrp->first);
	for(unsigned iS = 0; iS < iStPrp->second.size() && iS < Project::StlMaximum; iS++)
	    cStl.cfg(TSYS::strMess("V_%d",iS)).setS(iStPrp->second[iS]);
	TBDS::dataSet(fullDB()+"_stl", nodePath()+tbl()+"_stl", cStl);
    }
    // Checking for the removed properties
    res.request(true);
    cStl.cfgViewAll(false);
    for(int fldCnt = 0; TBDS::dataSeek(fullDB()+"_stl",nodePath()+tbl()+"_stl",fldCnt++,cStl); )
	if(mStProp.find(cStl.cfg("ID").getS()) == mStProp.end()) {
	    if(!TBDS::dataDel(fullDB()+"_stl",nodePath()+tbl()+"_stl",cStl,TBDS::UseAllKeys|TBDS::NoException)) break;
	    fldCnt--;
	}*/

    setDB(DB(), true);
}

void Project::setEnable( bool val )
{
    if(val == enable()) return;

    MtxAlloc fRes(funcM(), true);	//Prevent multiple entry

    mess_sys(TMess::Info, val ? _("Enabling the project.") : _("Disabling the project."));

    vector<string> fLst;
    list(fLst);
    for(unsigned iLs = 0; iLs < fLst.size(); iLs++)
	try{ at(fLst[iLs]).at().setEnable(val); }
	catch(TError &err) { mess_err(err.cat.c_str(), "%s", err.mess.c_str()); }

    mEnable = val;
}

string Project::add( const string &iid, const string &name, const string &orig )
{
    if(present(iid))	throw err_sys(_("The page '%s' is already present!"), iid.c_str());
	//return "";

    Page *obj = new Page(TSYS::strEncode(sTrm(iid),TSYS::oscdID), orig);
    MtxAlloc res(chM(), true);
    add(obj);
    obj->setName(name);

    return obj->id();
}

void Project::add( Page *iwdg )
{
    if(present(iwdg->id())) delete iwdg;
    else chldAdd(mPage, iwdg);
}

AutoHD<Page> Project::at( const string &id ) const	{ return chldAt(mPage,id); }

void Project::resourceDataList( vector<string> &list, const string &idb ) const
{
    string wdb = DB(), wtbl;
    if(idb.size()) wdb = TBDS::dbPart(idb), wtbl = TBDS::dbPart(idb, true);
    wtbl = (wtbl.empty()?tbl():wtbl) + "_mime";

    TConfig cEl(&mod->elWdgData());
    cEl.cfgViewAll(false);

    list.clear();
    for(int fldCnt = 0; TBDS::dataSeek(wdb+"."+wtbl,mod->nodePath()+wtbl,fldCnt,cEl,TBDS::UseCache); fldCnt++)
	if(std::find(list.begin(),list.end(),cEl.cfg("ID").getS()) == list.end())
	    list.push_back(cEl.cfg("ID").getS());
}

bool Project::resourceDataGet( const string &iid, string &mimeType, string *mimeData, const string &idb, int off, int *size ) const
{
    bool is_file = (iid.compare(0,5,"file:")==0);
    bool is_res  = (iid.compare(0,4,"res:")==0);

    if(!is_file) {
	//Get resource file from DB
	string dbid = is_res ? iid.substr(4) : iid;

	string wdb = DB(), wtbl;
	if(idb.size()) wdb = TBDS::dbPart(idb), wtbl = TBDS::dbPart(idb, true);
	wtbl = (wtbl.empty()?tbl():wtbl) + "_mime";

	TConfig cEl(&mod->elWdgData());
	if(!mimeData) cEl.cfg("DATA").setView(false);
	cEl.cfg("ID").setS( dbid );
	if(TBDS::dataGet(wdb+"."+wtbl,mod->nodePath()+wtbl,cEl,TBDS::NoException)) {
	    mimeType = cEl.cfg("MIME").getS();
	    if(mimeData) *mimeData = cEl.cfg("DATA").getS();
	    return true;
	}
    }
    if(!is_res) {
	//Get resource file from file system
	string filepath = is_file ? iid.substr(5) : iid;
	char buf[prmStrBuf_SZ];

	mimeType = TUIS::mimeGet(filepath, "");
	if(!mimeData)	return true;
	*mimeData = "";

	int hd = open(filepath.c_str(), O_RDONLY);
	if(hd == -1)	return false;

	int f_size = lseek(hd, 0, SEEK_END);
	int partSz = f_size;

	if(off >= 0) {	//Partially reading
	    off = vmin(f_size, off);
	    partSz = vmin(limUserFile_SZ, vmin(f_size, off + ((size && *size)?*size:f_size)));
	    if(size) *size = f_size;
	    lseek(hd, off, SEEK_SET);
	} else lseek(hd, 0, SEEK_SET);

	for(int len = 0; (len=read(hd,buf,vmin(sizeof(buf),partSz-mimeData->size()))) > 0; )
	    mimeData->append(buf, len);

	if(close(hd) != 0)
	    mess_warning(nodePath().c_str(), _("Closing the file %d error '%s (%d)'!"), hd, strerror(errno), errno);

	*mimeData = TSYS::strEncode(*mimeData, TSYS::base64);

	return true;
    }

    return false;
}

void Project::resourceDataSet( const string &iid, const string &mimeType, const string &mimeData, const string &idb )
{
    if(mDB_MimeSrc.size()) return;	//Do not set the resource for just copied and not saved still Library

    string wdb = DB(), wtbl;
    if(idb.size()) wdb = TBDS::dbPart(idb), wtbl = TBDS::dbPart(idb, true);
    wtbl = (wtbl.empty()?tbl():wtbl) + "_mime";

    TConfig cEl(&mod->elWdgData());
    cEl.cfg("ID").setS(iid);
    cEl.cfg("MIME").setS(mimeType);
    if(!mimeData.size()) cEl.cfg("DATA").setView(false);
    else cEl.cfg("DATA").setS(mimeData);
    TBDS::dataSet(wdb+"."+wtbl ,mod->nodePath()+wtbl, cEl, TBDS::NoException);
}

void Project::resourceDataDel( const string &iid, const string &idb )
{
    string wtbl = tbl()+"_mime";
    string wdb  = idb.empty() ? DB() : idb;
    TConfig cEl(&mod->elWdgData());
    cEl.cfg("ID").setS(iid, true);
    TBDS::dataDel(wdb+"."+wtbl, mod->nodePath()+wtbl, cEl, TBDS::NoException);
}

void Project::stlList( vector<string> &ls )
{
    ls.clear();

    ResAlloc res(mStRes, false);
    map< string, vector<string> >::iterator iStPrp = mStProp.find(STL_PRM_NM);
    if(iStPrp == mStProp.end()) return;
    for(unsigned ist = 0; ist < iStPrp->second.size(); ist++)
	ls.push_back(iStPrp->second[ist]);
}

int Project::stlSize( )
{
    ResAlloc res(mStRes, false);
    map< string, vector<string> >::iterator iStPrp = mStProp.find(STL_PRM_NM);
    if(iStPrp != mStProp.end()) return iStPrp->second.size();

    return 0;
}

void Project::stlCurentSet( int sid )
{
    mStyleIdW = (sid < 0 || sid >= stlSize()) ? Project::StlDisabled : sid;
    modif();
}

string Project::stlGet( int sid )
{
    ResAlloc res(mStRes, false);
    map< string, vector<string> >::iterator iStPrp = mStProp.find(STL_PRM_NM);
    if(iStPrp == mStProp.end() || sid < 0 || sid >= (int)iStPrp->second.size()) return "";

    return iStPrp->second[sid];
}

void Project::stlSet( int sid, const string &stl )
{
    ResAlloc res(mStRes, true);
    map< string, vector<string> >::iterator iStPrp = mStProp.find(STL_PRM_NM);
    if(iStPrp == mStProp.end() || sid < 0 || sid > StlMaximum) return;
    while(sid >= (int)iStPrp->second.size()) iStPrp->second.push_back("");
    iStPrp->second[sid] = stl;
    modif();
}

void Project::stlPropList( vector<string> &ls )
{
    ls.clear();
    ResAlloc res(mStRes, false);
    for(map<string, vector<string> >::iterator iStPrp = mStProp.begin(); iStPrp != mStProp.end(); iStPrp++)
	if(iStPrp->first != STL_PRM_NM)
	    ls.push_back(iStPrp->first);
}

string Project::stlPropGet( const string &pid, const string &def, int sid )
{
    if(pid.empty() || pid == STL_PRM_NM)	return def;

    ResAlloc res(mStRes, false);
    if(sid < 0) sid = stlCurent();
    map<string, vector<string> >::iterator iStPrp = mStProp.end();
    if((iStPrp=mStProp.find(pid)) == mStProp.end()) {
	vector<string> vl;
	for(int iV = 0; iV < vmax(1,stlSize()); iV++) vl.push_back(def);
	res.request(true);
	mStProp[pid] = vl;
	modif();
	return def;
    }
    if(iStPrp != mStProp.end() && sid >= 0 && sid < iStPrp->second.size()) return iStPrp->second[sid];

    return def;
}

bool Project::stlPropSet( const string &pid, const string &vl, int sid )
{
    ResAlloc res(mStRes, true);
    if(sid < 0) sid = stlCurent();
    if(pid.empty() || sid < 0 || sid >= stlSize() || pid == STL_PRM_NM) return false;
    map<string, vector<string> >::iterator iStPrp = mStProp.find(pid);
    if(iStPrp == mStProp.end()) return false;
    while(sid >= (int)iStPrp->second.size()) iStPrp->second.push_back("");
    iStPrp->second[sid] = vl;
    modif();

    return true;
}

string Project::catsPat( )
{
    string cat = "/ses_"+id()+"\\d*";
    MtxAlloc res(mHeritRes, true);
    for(unsigned iH = 0; iH < mHerit.size(); iH++)
	cat += "|/ses_"+mHerit[iH].at().id();

    return cat;
}

void Project::heritReg( Session *s )
{
    //Search for already registered session-heritator
    MtxAlloc res(mHeritRes, true);
    for(unsigned iH = 0; iH < mHerit.size(); iH++)
	if(&mHerit[iH].at() == s)	return;
    mHerit.push_back(AutoHD<Session>(s));
}

void Project::heritUnreg( Session *s )
{
    //Search the session-heritator
    MtxAlloc res(mHeritRes, true);
    for(unsigned iH = 0; iH < mHerit.size(); iH++)
	if(&mHerit[iH].at() == s) {
	    mHerit.erase(mHerit.begin()+iH);
	    return;
	}
}

void Project::pageEnable( const string &pg, bool vl )
{
    //Process the path
    int pL = 0;
    string pPg, pPath;
    for(int off = 0; (pPg=TSYS::pathLev(pg,0,false,&off)).size() && off < (int)pg.size(); pL++)
	if(pL) pPath += "/" + pPg;
    if(pPg.compare(0,3,"pg_") == 0)	pPg = pPg.substr(3);

    //Call the connected sessions for add-remove a page
    MtxAlloc res(mHeritRes, true);
    for(unsigned iH = 0; iH < mHerit.size(); iH++)
	if(pL > 1) {
	    AutoHD<SessPage> sP = mHerit[iH].at().nodeAt(pPath, 0, 0, 0, true);
	    if(!sP.freeStat() && vl && !sP.at().pagePresent(pPg)) {
		sP.at().pageAdd(pPg, pg);
		sP.at().pageAt(pPg).at().setEnable(true);
	    }
	}
	else if(vl && !mHerit[iH].at().present(pPg)) {
	    mHerit[iH].at().add(pPg, pg);
	    mHerit[iH].at().at(pPg).at().setEnable(true);
	}
}

void Project::cntrCmdProc( XMLNode *opt )
{
    string a_path = opt->attr("path");

    //Service commands process
    if(a_path == "/serv/access") {	//Access operations
	if(ctrChkNode(opt,"read",RWRWRW,"root",SUI_ID,SEC_RD))
	    opt->setText(SYS->security().at().access(opt->attr("user"),SEC_RD,owner(),grp(),permit())?"1":"0");
	return;
    }

    //Get page info
    if(opt->name() == "info") {
	TCntrNode::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Project: ")+id(),RWRWR_,"root",SUI_ID);
	if(ico().size()) ctrMkNode("img",opt,-1,"/ico","",R_R_R_);
	if(ctrMkNode("branches",opt,-1,"/br","",R_R_R_))
	    ctrMkNode("grp",opt,-1,"/br/pg_",_("Page"),RWRWR_,"root",SUI_ID,2,"idm","1","idSz","30");
	if(ctrMkNode("area",opt,-1,"/obj",_("Project"))) {
	    if(ctrMkNode("area",opt,-1,"/obj/st",_("State"))) {
		ctrMkNode("fld",opt,-1,"/obj/st/status",_("Status"),R_R_R_,"root",SUI_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/obj/st/en",_("Enabled"),RWRWR_,"root",SUI_ID,1,"tp","bool");
		if(isStdStorAddr())
		    ctrMkNode("fld",opt,-1,"/obj/st/db",_("Project DB"),RWRWR_,"root",SUI_ID,4,
			"tp","str","dest","select","select","/db/list","help",TMess::labStor().c_str());
		else ctrMkNode("fld",opt,-1,"/obj/st/db",_("Project DB"),RWRWR_,"root",SUI_ID,4,
			"tp","str","dest","sel_ed","select",("/db/tblList:prj_"+id()).c_str(),
			"help",_("Storage address in the format \"{DB module}.{DB name}.{Table name}\".\nTo use the Generic Storage, set '*.*.{Table name}'."));
		if(DB(true).size())
		    ctrMkNode("comm",opt,-1,"/obj/st/removeFromDB",TSYS::strMess(_("Remove from '%s'"),DB(true).c_str()).c_str(),RWRW__,"root",SUI_ID,
			1,"help",(DB(true)=="*.*")?TMess::labStorRemGenStor().c_str():"");
		ctrMkNode("fld",opt,-1,"/obj/st/timestamp",_("Date of modification"),R_R_R_,"root",SUI_ID,1,"tp","time");
		ctrMkNode("fld",opt,-1,"/obj/st/use",_("Used"),R_R_R_,"root",SUI_ID,1,"tp","dec");
	    }
	    if(ctrMkNode("area",opt,-1,"/obj/cfg",_("Configuration"))) {
		ctrMkNode("fld",opt,-1,"/obj/cfg/id",_("Identifier"),R_R_R_,"root",SUI_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/obj/cfg/name",_("Name"),RWRWR_,"root",SUI_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/obj/cfg/descr",_("Description"),RWRWR_,"root",SUI_ID,3,"tp","str","cols","100","rows","3");
		ctrMkNode("img",opt,-1,"/obj/cfg/ico",_("Icon"),RWRWR_,"root",SUI_ID,2,"v_sz","64","h_sz","64");
		ctrMkNode("fld",opt,-1,"/obj/cfg/owner",_("Owner and group"),RWRWR_,"root",SUI_ID,3,"tp","str","dest","select","select","/obj/u_lst");
		ctrMkNode("fld",opt,-1,"/obj/cfg/grp","",RWRWR_,"root",SUI_ID,3,"tp","str","dest","select","select","/obj/g_lst");
		ctrMkNode("fld",opt,-1,"/obj/cfg/u_a",_("Access"),RWRWR_,"root",SUI_ID,4,"tp","dec","dest","select",
		    "sel_id","0;4;6","sel_list",_("No access;View;View and control"));
		ctrMkNode("fld",opt,-1,"/obj/cfg/g_a","",RWRWR_,"root",SUI_ID,4,"tp","dec","dest","select",
		    "sel_id","0;4;6","sel_list",_("No access;View;View and control"));
		ctrMkNode("fld",opt,-1,"/obj/cfg/o_a","",RWRWR_,"root",SUI_ID,4,"tp","dec","dest","select",
		    "sel_id","0;4;6","sel_list",_("No access;View;View and control"));
		ctrMkNode("fld",opt,-1,"/obj/cfg/per",_("Period of calculation"),RWRWR_,"root",SUI_ID,2,"tp","dec",
		    "help",_("Project's session calculate period on milliseconds."));
		ctrMkNode("fld",opt,-1,"/obj/cfg/toEnByNeed",_("Enable as needed"),RWRWR_,"root",SUI_ID,1,"tp","bool");
	    }
	}
	if(ctrMkNode("area",opt,-1,"/page",_("Pages"))) {
	    ctrMkNode("fld",opt,-1,"/page/nmb",_("Number"),R_R_R_,"root",SUI_ID,1,"tp","str");
	    ctrMkNode("list",opt,-1,"/page/page",_("Pages"),RWRWR_,"root",SUI_ID,5,"tp","br","idm","1","s_com","add,del","br_pref","pg_","idSz","30");
	}
	if(ctrMkNode("area",opt,-1,"/mime",_("Resources")))
	    if(ctrMkNode("table",opt,-1,"/mime/mime",_("Resources"),RWRWR_,"root",SUI_ID,2,"s_com","add,del","key","id")) {
		ctrMkNode("list",opt,-1,"/mime/mime/id",_("Identifier"),RWRWR_,"root",SUI_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/mime/mime/tp",_("MIME"),RWRWR_,"root",SUI_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/mime/mime/dt",_("Data"),RWRWR_,"root",SUI_ID,2,"tp","str","dest","data");
	    }
	if(ctrMkNode("area",opt,-1,"/style",_("Styles"))) {
	    ctrMkNode("fld",opt,-1,"/style/style",_("Style"),RWRWR_,"root",SUI_ID,3,"tp","dec","dest","select","select","/style/stLst");
	    if(stlCurent() >= 0 && stlCurent() < stlSize()) {
		ctrMkNode("fld",opt,-1,"/style/name",_("Name"),RWRWR_,"root",SUI_ID,1,"tp","str");
		if(ctrMkNode("table",opt,-1,"/style/props",_("Properties"),RWRWR_,"root",SUI_ID,2,"s_com","del","key","id")) {
		    ctrMkNode("list",opt,-1,"/style/props/id",_("Identifier"),R_R_R_,"root",SUI_ID,1,"tp","str");
		    ctrMkNode("list",opt,-1,"/style/props/vl",_("Value"),RWRWR_,"root",SUI_ID,1,"tp","str");
		}
		ctrMkNode("comm",opt,-1,"/style/erase",_("Delete"),RWRWR_,"root",SUI_ID);
	    }
	}
	if(ctrMkNode("area",opt,-1,"/mess",_("Diagnostics"))) {
	    ctrMkNode("fld",opt,-1,"/mess/tm",_("Time"),RWRW__,"root",SUI_ID,1,"tp","time");
	    ctrMkNode("fld",opt,-1,"/mess/size",_("Size, seconds"),RWRW__,"root",SUI_ID,1,"tp","dec");
	    if(ctrMkNode("table",opt,-1,"/mess/mess",_("Messages"),R_R___,"root",SUI_ID)) {
		ctrMkNode("list",opt,-1,"/mess/mess/0",_("Time"),R_R___,"root",SUI_ID,1,"tp","time");
		ctrMkNode("list",opt,-1,"/mess/mess/0a",_("mcsec"),R_R___,"root",SUI_ID,1,"tp","dec");
		ctrMkNode("list",opt,-1,"/mess/mess/1",_("Category"),R_R___,"root",SUI_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/mess/mess/2",_("Level"),R_R___,"root",SUI_ID,1,"tp","dec");
		ctrMkNode("list",opt,-1,"/mess/mess/3",_("Message"),R_R___,"root",SUI_ID,1,"tp","str");
	    }
	}
	return;
    }

    //Process command to page
    if(a_path == "/obj/st/status" && ctrChkNode(opt))		opt->setText(getStatus());
    else if(a_path == "/obj/st/en") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(i2s(enable()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR)) {
	    setEnable(s2i(opt->text()));
	    if(s2i(opt->text())) load();
	}
    }
    else if(a_path == "/obj/st/db") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(isStdStorAddr()?DB():fullDB());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	isStdStorAddr() ? setDB(opt->text()) : setFullDB(opt->text());
    }
    else if(a_path == "/obj/st/removeFromDB" && ctrChkNode(opt,"set",RWRW__,"root",SUI_ID,SEC_WR))
	postDisable(NodeRemoveOnlyStor);
    else if(a_path == "/obj/st/timestamp" && ctrChkNode(opt)) {
	vector<string> tls;
	list(tls);
	time_t maxTm = 0;
	for(unsigned iT = 0; iT < tls.size(); iT++) maxTm = vmax(maxTm, at(tls[iT]).at().timeStamp());
	opt->setText(i2s(maxTm));
    }
    else if(a_path == "/obj/st/use" && ctrChkNode(opt))	opt->setText(i2s(mHerit.size()));
    else if(a_path == "/obj/cfg/owner") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(owner());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setOwner(opt->text());
    }
    else if(a_path == "/obj/cfg/grp") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(grp());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setGrp(opt->text());
    }
    else if(a_path == "/obj/cfg/u_a" || a_path == "/obj/cfg/g_a" || a_path == "/obj/cfg/o_a") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD)) {
	    if(a_path == "/obj/cfg/u_a")	opt->setText(i2s((permit()>>6)&0x7));
	    if(a_path == "/obj/cfg/g_a")	opt->setText(i2s((permit()>>3)&0x7));
	    if(a_path == "/obj/cfg/o_a")	opt->setText(i2s(permit()&0x7));
	}
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR)) {
	    if(a_path == "/obj/cfg/u_a")	setPermit((permit()&(~(0x07<<6)))|(s2i(opt->text())<<6));
	    if(a_path == "/obj/cfg/g_a")	setPermit((permit()&(~(0x07<<3)))|(s2i(opt->text())<<3));
	    if(a_path == "/obj/cfg/o_a")	setPermit((permit()&(~0x07))|s2i(opt->text()));
	}
    }
    else if(a_path == "/obj/cfg/ico" || a_path == "/ico") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(ico());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setIco(opt->text());
    }
    else if(a_path == "/obj/cfg/id" && ctrChkNode(opt,"get",R_R_R_,"root",SUI_ID))	opt->setText(id());
    else if(a_path == "/obj/cfg/name") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(trD(name()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setName(trDSet(name(),opt->text()));
    }
    else if(a_path == "/obj/cfg/descr") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(trD(descr()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setDescr(trDSet(descr(),opt->text()));
    }
    else if(a_path == "/obj/cfg/per") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(i2s(period()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setPeriod(s2i(opt->text()));
    }
    else if(a_path == "/obj/cfg/toEnByNeed") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(i2s(toEnByNeed()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setToEnByNeed(s2i(opt->text()));
    }
    else if(a_path == "/br/pg_" || a_path == "/page/page") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD)) {
	    bool getChPgN = s2i(opt->attr("getChPgN"));
	    vector<string> lst, lst1;
	    list(lst);
	    for(unsigned iF = 0; iF < lst.size(); iF++) {
		XMLNode *no = opt->childAdd("el")->setAttr("id",lst[iF])->setText(trD(at(lst[iF]).at().name()));
		if(getChPgN) { at(lst[iF]).at().pageList(lst1); no->setAttr("chPgN", i2s(lst1.size())); }
	    }
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SUI_ID,SEC_WR)) {
	    opt->setAttr("id", add(opt->attr("id"),opt->text()));
	    at(opt->attr("id")).at().setOwner(opt->attr("user"));
	    at(opt->attr("id")).at().manCrt = true;
	}
	if(ctrChkNode(opt,"del",RWRWR_,"root",SUI_ID,SEC_WR))	del(opt->attr("id"),true);
    }
    else if(a_path == "/page/nmb" && ctrChkNode(opt)) {
	vector<string> c_list;
	list(c_list);
	unsigned e_c = 0;
	for(unsigned iP = 0; iP < c_list.size(); iP++)
	    if(at(c_list[iP]).at().enable()) e_c++;
	opt->setText(TSYS::strMess(_("All: %d; Enabled: %d"),c_list.size(),e_c));
    }
    else if(a_path == "/obj/u_lst" && ctrChkNode(opt)) {
	vector<string> ls;
	SYS->security().at().usrList(ls);
	for(unsigned iL = 0; iL < ls.size(); iL++)
	    opt->childAdd("el")->setText(ls[iL]);
    }
    else if(a_path == "/obj/g_lst" && ctrChkNode(opt)) {
	vector<string> ls;
	SYS->security().at().usrGrpList(owner(), ls);
	for(unsigned iL = 0; iL < ls.size(); iL++)
	    opt->childAdd("el")->setText(ls[iL]);
    }
    else if(a_path == "/mime/mime") {
	// Request data
	string idmime = opt->attr("key_id");
	string idcol  = opt->attr("col");

	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD)) {
	    if(!idmime.empty() && idcol == "dt" && s2i(opt->attr("data"))) {
		string mimeType, mimeData;
		if(resourceDataGet("res:"+idmime,mimeType,&mimeData)) opt->setText(mimeData);
	    }
	    else {
		XMLNode *n_id = ctrMkNode("list",opt,-1,"/mime/mime/id","");
		XMLNode *n_tp = ctrMkNode("list",opt,-1,"/mime/mime/tp","");
		XMLNode *n_dt = ctrMkNode("list",opt,-1,"/mime/mime/dt","");

		vector<string> lst;
		string mimeType;
		resourceDataList(lst);
		for(unsigned i_el = 0; i_el < lst.size(); i_el++)
		    if(resourceDataGet("res:"+lst[i_el],mimeType)) {
			if(n_id) n_id->childAdd("el")->setText(lst[i_el]);
			if(n_tp) n_tp->childAdd("el")->setText(TSYS::strSepParse(mimeType,0,';'));
			if(n_dt) n_dt->childAdd("el")->setText(TSYS::strSepParse(mimeType,1,';'));
		    }
	    }
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SUI_ID,SEC_WR))	resourceDataSet("newRes", "file/unknown;0", "");
	if(ctrChkNode(opt,"del",RWRWR_,"root",SUI_ID,SEC_WR))	resourceDataDel(opt->attr("key_id"));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR)) {
	    // Request data
	    if(idcol == "id") {
		string mimeType, mimeData;
		// Copy resources to new record
		if(resourceDataGet("res:"+idmime, mimeType, &mimeData)) {
		    resourceDataSet(opt->text(), TUIS::mimeGet(idmime,mimeData,mimeType), mimeData);
		    resourceDataDel(idmime);
		}
	    }
	    else if(idcol == "tp") {
		string mimeType;
		// Copy resources to new record
		if(resourceDataGet("res:"+idmime, mimeType))
		    resourceDataSet(idmime, opt->text()+";"+TSYS::strSepParse(mimeType,1,';'), "");
	    }
	    else if(idcol == "dt") {
		string mimeType;
		if(!resourceDataGet("res:"+idmime, mimeType)) mimeType = TUIS::mimeGet(idmime, TSYS::strDecode(opt->text(),TSYS::base64));
		resourceDataSet(idmime, TSYS::strSepParse(mimeType,0,';')+";"+r2s((float)opt->text().size()/1024,6),opt->text());
	    }
	}
    }
    else if(a_path == "/style/style") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(i2s(stlCurent()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR)) {
	    if(s2i(opt->text()) >= Project::StlDisabled) stlCurentSet(s2i(opt->text()));
	    else {
		ResAlloc res(mStRes, true);

		// Appending for the style name
		map< string, vector<string> >::iterator iStPrp = mStProp.find(STL_PRM_NM);
		if(iStPrp == mStProp.end()) mStProp[STL_PRM_NM] = vector<string>(1,trDSet("",_("New style")));
		else iStPrp->second.push_back(trDSet("",_("New style")));

		// Appending for the properties
		for(iStPrp = mStProp.begin(); iStPrp != mStProp.end(); iStPrp++)
		    if(iStPrp->first != STL_PRM_NM && stlSize() > (int)iStPrp->second.size())
			iStPrp->second.push_back(iStPrp->second[(mStyleIdW>=0)?mStyleIdW:iStPrp->second.size()-1]);

		mStyleIdW = mStProp[STL_PRM_NM].size()-1;
		modif();
	    }
	}
    }
    else if(a_path == "/style/stLst" && ctrChkNode(opt)) {
	opt->childAdd("el")->setAttr("id",i2s(Project::StlDisabled))->setText(_("<Disabled>"));
	opt->childAdd("el")->setAttr("id",i2s(Project::StlCreate))->setText(_("<Create a new style>"));
	for(int iSt = 0; iSt < stlSize(); iSt++)
	    opt->childAdd("el")->setAttr("id", i2s(iSt))->setText(trD(stlGet(iSt)));
    }
    else if(a_path == "/style/name") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(trD(stlGet(stlCurent())));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	stlSet(stlCurent(), trDSet(stlGet(stlCurent()),opt->text()));
    }
    else if(a_path == "/style/props") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD) && stlCurent() >= 0 && stlCurent() < stlSize()) {
	    XMLNode *n_id = ctrMkNode("list",opt,-1,"/style/props/id","");
	    XMLNode *n_vl = ctrMkNode("list",opt,-1,"/style/props/vl","");

	    ResAlloc res(mStRes, false);
	    for(map<string, vector<string> >::iterator iStPrp = mStProp.begin(); iStPrp != mStProp.end(); iStPrp++) {
		if(iStPrp->first == STL_PRM_NM) continue;
		if(n_id) n_id->childAdd("el")->setText(iStPrp->first);
		if(n_vl) n_vl->childAdd("el")->setText(iStPrp->second[stlCurent()]);
	    }
	}
	if(ctrChkNode(opt,"del",RWRWR_,"root",SUI_ID,SEC_WR)) {
	    ResAlloc res(mStRes, true);
	    if(mStProp.find(opt->attr("key_id")) != mStProp.end()) { mStProp.erase(opt->attr("key_id")); modif(); }
	}
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR) && opt->attr("col") == "vl" && stlCurent() >= 0 && stlCurent() < stlSize())
	{
	    ResAlloc res(mStRes, true);
	    map< string, vector<string> >::iterator iStPrp = mStProp.find(opt->attr("key_id"));
	    if(iStPrp != mStProp.end()) { iStPrp->second[stlCurent()] = opt->text(); modif(); }
	}
    }
    else if(a_path == "/style/erase" && ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR) && stlCurent() >= 0 && stlCurent() < stlSize())
    {
	ResAlloc res(mStRes, true);
	map< string, vector<string> >::iterator iStPrp;
	for(iStPrp = mStProp.begin(); iStPrp != mStProp.end(); iStPrp++)
	    if(iStPrp->second.size() > 1 || iStPrp->first == STL_PRM_NM)
		iStPrp->second.erase(iStPrp->second.begin()+stlCurent());
	stlCurentSet(Project::StlDisabled);
    }
    else if(a_path == "/mess/tm") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SUI_ID,SEC_RD)) {
	    opt->setText(TBDS::genPrmGet(mod->nodePath()+"messTm","0",opt->attr("user")));
	    if(!s2i(opt->text())) opt->setText(i2s(time(NULL)));
	}
	if(ctrChkNode(opt,"set",RWRW__,"root",SUI_ID,SEC_WR))
	    TBDS::genPrmSet(mod->nodePath()+"messTm",(s2i(opt->text())>=time(NULL))?"0":opt->text(),opt->attr("user"));
    }
    else if(a_path == "/mess/size") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))
	    opt->setText(TBDS::genPrmGet(mod->nodePath()+"messSize","600",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))
	    TBDS::genPrmSet(mod->nodePath()+"messSize",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/mess/mess" && ctrChkNode(opt,"get",R_R___,"root",SUI_ID)) {
	vector<TMess::SRec> rec;
	time_t gtm = s2i(TBDS::genPrmGet(mod->nodePath()+"messTm","0",opt->attr("user")));
	if(!gtm) gtm = time(NULL);
	int gsz = s2i(TBDS::genPrmGet(mod->nodePath()+"messSize","600",opt->attr("user")));
	SYS->archive().at().messGet(gtm-gsz, gtm, rec, "/("+catsPat()+")/", Mess->messLevel(), "");

	XMLNode *n_tm   = ctrMkNode("list",opt,-1,"/mess/mess/0","",R_R___,"root",SUI_ID);
	XMLNode *n_tmu  = ctrMkNode("list",opt,-1,"/mess/mess/0a","",R_R___,"root",SUI_ID);
	XMLNode *n_cat  = ctrMkNode("list",opt,-1,"/mess/mess/1","",R_R___,"root",SUI_ID);
	XMLNode *n_lvl  = ctrMkNode("list",opt,-1,"/mess/mess/2","",R_R___,"root",SUI_ID);
	XMLNode *n_mess = ctrMkNode("list",opt,-1,"/mess/mess/3","",R_R___,"root",SUI_ID);
	for(int i_rec = rec.size()-1; i_rec >= 0; i_rec--) {
	    if(n_tm)	n_tm->childAdd("el")->setText(i2s(rec[i_rec].time));
	    if(n_tmu)	n_tmu->childAdd("el")->setText(i2s(rec[i_rec].utime));
	    if(n_cat)	n_cat->childAdd("el")->setText(rec[i_rec].categ);
	    if(n_lvl)	n_lvl->childAdd("el")->setText(i2s(rec[i_rec].level));
	    if(n_mess)	n_mess->childAdd("el")->setText(rec[i_rec].mess);
	}
    }

    else TCntrNode::cntrCmdProc(opt);
}

//************************************************
//* Page: Project's page                         *
//************************************************
Page::Page( const string &iid, const string &isrcwdg ) : Widget(iid), TConfig(&mod->elPage()), manCrt(false),
    mFlgs(cfg("FLGS").getId()), mProcPer(cfg("PROC_PER").getId()), mTimeStamp(cfg("TIMESTAMP").getId())
{
    cfg("ID").setS(id());

    mPage = grpAdd("pg_");

    setParentAddr(isrcwdg);
    setNodeFlg(TCntrNode::SaveForceOnChild);
}

Page::~Page( )
{

}

TCntrNode &Page::operator=( const TCntrNode &node )
{
    const Page *src_n = dynamic_cast<const Page*>(&node);
    if(!src_n) return Widget::operator=(node);

    if(!src_n->enable()) return *this;

    //Copy generic configuration
    setPrjFlags(src_n->prjFlags());

    //Widget copy
    Widget::operator=(node);

    //Removing the inherited but missed widgets on the source
    vector<string> els, lnkErrEls;
    wdgList(els);
    for(unsigned iW = 0; iW < els.size(); iW++)
	if(!src_n->wdgPresent(els[iW])) wdgDel(els[iW], true);

    //Copying included pages with tracing the page link errors and repeat the error pages copy after all other
    if(addr().find(src_n->addr()+"/") != 0) {	//Call recursively only for separated branches copy and to prevent for copy the included ones
	vector<string> lnkErrEls;
	src_n->pageList(els);
	for(unsigned iTr = 1; true; ++iTr) {
	    for(unsigned iP = 0; iP < els.size(); ++iP)
		try {
		    if(!pagePresent(els[iP])) pageAdd(els[iP], "");
		    (TCntrNode&)pageAt(els[iP]).at() = (TCntrNode&)src_n->pageAt(els[iP]).at();
		}
		catch(TError &err) {
		    if(err.cod == Engine::NotResLnk || (err.cod == TError::Core_NoNode && src_n->pageAt(els[iP]).at().prjFlags()&Page::Link))
			lnkErrEls.push_back(els[iP]);
		    else throw;
		}
	    if(lnkErrEls.empty() || iTr >= 2) break;
	    els = lnkErrEls; lnkErrEls.clear();
	}

	if(lnkErrEls.size())
	    throw TError(Engine::NotResLnk, nodePath().c_str(), "The copying operation is terminated by the not resolved links.");
    }

    return *this;
}

Page *Page::ownerPage( ) const	{ return nodePrev(true) ? dynamic_cast<Page*>(nodePrev()) : NULL; }

Project *Page::ownerProj( ) const
{
    Page *own = ownerPage();
    if(own) return own->ownerProj();
    if(nodePrev(true)) return dynamic_cast<Project*>(nodePrev());
    return NULL;
}

string Page::addr( ) const	{ return ownerFullId(true)+"/pg_"+id(); }

string Page::ownerFullId( bool contr ) const
{
    Page *own = ownerPage( );
    if(own) return own->ownerFullId(contr)+(contr?"/pg_":"/")+own->id();
    return string(contr?"/prj_":"/")+ownerProj()->id();
}

int Page::timeStamp( )
{
    int curTm = mTimeStamp;
    vector<string> ls;
    pageList(ls);
    for(unsigned iL = 0; iL < ls.size(); iL++)
	curTm = vmax(curTm, pageAt(ls[iL]).at().timeStamp());

    return curTm;
}

void Page::postEnable( int flag )
{
    //Call parent method
    Widget::postEnable(flag);

    //Add main attributes
    if(flag&TCntrNode::NodeConnect) {
	attrAdd(new TFld("pgOpen",trS("Page: opened"),TFld::Boolean,TFld::NoFlag));
	attrAdd(new TFld("pgNoOpenProc",trS("Page: process not opened"),TFld::Boolean,TFld::NoFlag));
	attrAdd(new TFld("pgGrp",trS("Page: group"),TFld::String,TFld::NoFlag,"","","","",i2s(A_PG_GRP).c_str()));
	attrAdd(new TFld("pgOpenSrc",trS("Page: source of the opening"),TFld::String,TFld::NoFlag,"","","","",i2s(A_PG_OPEN_SRC).c_str()));
    }

    //Set owner key for this page
    cfg("OWNER").setS(ownerFullId());

    //Set default parent for parent template page
    if(ownerPage() && (ownerPage()->prjFlags()&Page::Template)) setParentAddr("..");
}

void Page::postDisable( int flag )
{
    if(flag&NodeRemove) {
	string db  = ownerProj()->DB();
	string tbl = ownerProj()->tbl();

	//Remove from library table
	TBDS::dataDel(db+"."+tbl, mod->nodePath()+tbl, *this, TBDS::UseAllKeys);

	//Remove widget's IO from library IO table
	TConfig cEl(&mod->elWdgIO());
	cEl.cfg("IDW").setS(addr(),true);
	TBDS::dataDel(db+"."+tbl+"_io", mod->nodePath()+tbl+"_io", cEl);

	//Remove widget's user IO from library IO table
	cEl.setElem(&mod->elWdgUIO());
	cEl.cfg("IDW").setS(addr(),true);
	TBDS::dataDel(db+"."+tbl+"_uio", mod->nodePath()+tbl+"_uio", cEl);

	//Remove widget's included widgets from library include table
	cEl.setElem(&mod->elInclWdg());
	cEl.cfg("IDW").setS(addr(),true);
	TBDS::dataDel(db+"."+tbl+"_incl", mod->nodePath()+tbl+"_incl", cEl);
    }
}

bool Page::cfgChange( TCfg &co, const TVariant &pc )
{
    if(co.name() == "PR_TR") cfg("PROC").setNoTransl(!calcProgTr());

    if(co.getS() == pc.getS()) return true;
    if(co.name() == "PROC") procChange();
    modif();
    return true;
}

string Page::ico( ) const
{
    if(cfg("ICO").getS().size())return cfg("ICO").getS();
    if(!parent().freeStat())	return parent().at().ico();
    return "";
}

void Page::setParentAddr( const string &isw )
{
    if(enable() && cfg("PARENT").getS() != isw) setEnable(false);
    cfg("PARENT").setS(isw);
    if(ownerPage() && (ownerPage()->prjFlags()&Page::Template) && !(ownerPage()->prjFlags()&Page::Container))
	cfg("PARENT").setS("..");
    modif();
}

string Page::getStatus( )
{
    string rez = Widget::getStatus();
    rez += TSYS::strMess(_("Date of modification %s. "), atm2s(timeStamp()).c_str());
    if(calcProg().size()) {
	rez += _("Calculating procedure - ");
	if(!parent().freeStat() && parent().at().calcProg().size() && calcProg() != parent().at().calcProg())
	    rez += _("REDEFINED");
	else if(!parent().freeStat() && parent().at().calcProg().size())
	    rez += _("inherited");
	else rez += _("presented");
	rez += ". ";
    }

    return rez;
}

string Page::calcId( )
{
    if(proc().empty()) {
	if(!parent().freeStat()) return parent().at().calcId();
	return "";
    }
    if(ownerPage()) return ownerPage()->calcId()+"_"+id();
    return "P_"+ownerProj()->id()+"_"+id();
}

string Page::calcLang( ) const
{
    if(proc().empty() && !parent().freeStat()) return parent().at().calcLang();

    return TSYS::strLine(proc(), 0);
}

string Page::calcProg( ) const
{
    if(!proc().size() && !parent().freeStat()) return parent().at().calcProg();

    string iprg = proc();
    int off = 0;
    TSYS::strLine(iprg, 0, &off);
    return iprg.substr(off);
}

string Page::calcProgStors( const string &attr )
{
    string rez = parent().freeStat() ? "" : parent().at().calcProgStors(attr);
    if(((attr.size() && attrAt(attr).at().aModif()) || (!attr.size() && proc().size())) && rez.find(ownerProj()->DB()) == string::npos)
	rez = ownerProj()->DB() + ";" + rez;

    return rez;
}

int Page::calcPer( ) const	{ return (mProcPer == PerVal_Parent && !parent().freeStat()) ? parent().at().calcPer() : mProcPer; }

void Page::setCalcLang( const string &ilng )	{ cfg("PROC").setS(ilng.empty() ? "" : ilng+"\n"+calcProg()); }

void Page::setCalcProg( const string &iprg )	{ cfg("PROC").setS(calcLang()+"\n"+iprg); }

void Page::setPrjFlags( int val )
{
    int dif = mFlgs^val;
    if(dif&(Page::Template|Page::Link) &&		//!!!! Clear the parent link and disable the page
	!((mFlgs == 0 && val == Page::Template) ||	//     at any change the project state Empty|Template|Link,
	    (mFlgs == Page::Template && val == 0)))	//     besides the Standard page change to Template or contrariwise
    {
	//Cleaning the page
	setParentAddr("");
	if(enable()) {
	    setEnable(false);
	    setEnable(true);
	}
    }
    mFlgs = val;
    modif();
}

void Page::load_( TConfig *icfg )
{
    if(!SYS->chkSelDB(ownerProj()->DB())) throw TError();

    setStlLock(true);

    //Load generic widget's data
    string db  = ownerProj()->DB();
    string tbl = ownerProj()->tbl();
    string tbl_io = tbl+"_io";
    if(icfg) *(TConfig*)this = *icfg;
    else {
	cfg("PROC").setExtVal(true);
	TBDS::dataGet(db+"."+tbl, mod->nodePath()+tbl, *this);
    }
    if(!calcProgTr()) cfg("PROC").setExtVal(false, true);
    setParentAddr(cfg("PARENT").getS());

    //Inheriting the modified attributes
    vector<string> als;
    attrList(als);
    string tAttrs = cfg("ATTRS").getS();
    for(unsigned iA = 0; iA < als.size(); iA++) {
	if(!attrPresent(als[iA])) continue;
	AutoHD<Attr> attr = attrAt(als[iA]);
	if(attr.at().aModif() && tAttrs.find(als[iA]+";") == string::npos) {
	    attr.at().setAModif_(0);
	    inheritAttr(als[iA]);
	}
    }

    //Loading for the generic attributes
    mod->attrsLoad(*this, db+"."+tbl, addr(), "", tAttrs, true);

    //Loading the inner pages only for containers and templates
    if(prjFlags()&(Page::Template|Page::Container)) {
	//Creating for new pages
	map<string, bool>	itReg;
	TConfig cEl(&mod->elPage());
	cEl.cfg("PROC").setExtVal(true);
	//cEl.cfgViewAll(false);
	cEl.cfg("OWNER").setS(ownerFullId()+"/"+id(), true);
	for(int fldCnt = 0; TBDS::dataSeek(db+"."+tbl,mod->nodePath()+tbl,fldCnt++,cEl,TBDS::UseCache); ) {
	    string fId = cEl.cfg("ID").getS();
	    if(!pagePresent(fId))
		try { pageAdd(fId, "", ""); }
		catch(TError &err) { if(err.cod != Engine::NoContainer) mess_err(err.cat.c_str(), "%s", err.mess.c_str()); }
	    if(pagePresent(fId)) pageAt(fId).at().load(&cEl);
	    itReg[fId] = true;
	}

	//Check for remove items removed from DB
	if(SYS->chkSelDB(SYS->selDB(),true)) {
	    vector<string> itLs;
	    pageList(itLs);
	    for(unsigned iIt = 0; iIt < itLs.size(); iIt++)
		if(itReg.find(itLs[iIt]) == itReg.end())
		    pageDel(itLs[iIt]);
	}

	//Load present pages
	vector<string> fLst;
	pageList(fLst);
	for(unsigned iLs = 0; iLs < fLst.size(); iLs++)
	    pageAt(fLst[iLs]).at().load();
    }

    //Load all widget attributes
    loadIO();

    setStlLock(false);
}

void Page::loadIO( )
{
    if(!enable()) return;

    //Load widget's work attributes
    mod->attrsLoad(*this, ownerProj()->DB()+"."+ownerProj()->tbl(), addr(), "", cfg("ATTRS").getS());

    //Load cotainer widgets
    if(!isContainer()) return;
    map<string, bool>   itReg;
    TConfig cEl(&mod->elInclWdg());
    string db  = ownerProj()->DB();
    string tbl = ownerProj()->tbl()+"_incl";
    cEl.cfg("IDW").setS(addr(), true);
    for(int fldCnt = 0; TBDS::dataSeek(db+"."+tbl,mod->nodePath()+tbl,fldCnt++,cEl); ) {
	string sid  = cEl.cfg("ID").getS();
	string spar = cEl.cfg("PARENT").getS();

	// Directly marked as deleted one
	if(spar == "<deleted>") {
	    if(wdgPresent(sid)) wdgDel(sid);
	    continue;
	}
	// Lost inherited widget due to it removing into the parent
	else if((mParentAddrPrev.empty() || spar.find(mParentAddrPrev) != 0) &&	//!!!! Do not remove at presence in the previous parent
		mod->nodeAt(spar,0,0,0,true).freeStat() &&
		sid.size() < spar.size() && spar.compare(spar.size()-sid.size(),sid.size(),sid) == 0)
	{
	    if(wdgPresent(sid)) wdgDel(sid);
	    TBDS::dataDel(db+"."+tbl, mod->nodePath()+tbl, cEl, TBDS::UseAllKeys);
	    fldCnt--;
	    continue;
	}
	// Record without any changes
	else if(!cEl.cfg("ATTRS").getS().size()) {
	    TBDS::dataDel(db+"."+tbl, mod->nodePath()+tbl, cEl, TBDS::UseAllKeys);
	    fldCnt--;
	}
	bool isNew = false;
	if((isNew=!wdgPresent(sid)))
	    try { wdgAdd(sid, "", ""); }
	    catch(TError &err){ mess_err(err.cat.c_str(),err.mess.c_str()); }

	wdgAt(sid).at().load(&cEl);
	if(isNew && enable()) wdgAt(sid).at().setEnable(true);
	itReg[sid] = true;
    }

    // Check for remove items removed from the DB
    if(SYS->chkSelDB(SYS->selDB(),true)) {
	vector<string> itLs;
	wdgList(itLs);
	for(unsigned iIt = 0; iIt < itLs.size(); iIt++)
	    if(itReg.find(itLs[iIt]) == itReg.end())
		wdgDel(itLs[iIt]);
    }
}

void Page::save_( )
{
    string db  = ownerProj()->DB();
    string tbl = ownerProj()->tbl();

    //Save generic attributes
    cfg("ATTRS").setS(mod->attrsSave(*this, db+"."+tbl, addr(), "", true));

    //Save generic widget's data
    mTimeStamp = SYS->sysTm();
    TBDS::dataSet(db+"."+tbl, mod->nodePath()+tbl, *this);

    //Save widget's attributes
    saveIO();

    //Updation/saving here the removing mark "<deleted>" of the included widgets since the storage can be changed
    if(!parent().freeStat()) {
	TConfig cEl(&mod->elInclWdg());
	string db  = ownerProj()->DB();
	string tbl = ownerProj()->tbl()+"_incl";
	cEl.cfg("IDW").setS(addr(), TCfg::ForceUse);

	vector<string> els;
	parent().at().wdgList(els);
	for(unsigned iW = 0; iW < els.size(); iW++) {
	    if(wdgPresent(els[iW]))	continue;
	    cEl.cfg("ID").setS(els[iW], TCfg::ForceUse);
	    cEl.cfg("PARENT").setS("<deleted>", TCfg::ForceUse);
	    TBDS::dataSet(db+"."+tbl, mod->nodePath()+tbl, cEl);
	}
    }
}

void Page::saveIO( )
{
    if(!enable()) return;

    //Save widget's attributes
    mod->attrsSave(*this, ownerProj()->DB()+"."+ownerProj()->tbl(), addr(), "");
}

void Page::wClear( )
{
    Widget::wClear();
    cfg("PROC").setS("");
    cfg("ATTRS").setS("");
}

void Page::setEnable( bool val, bool force )
{
    if(enable() == val) return;

    mess_sys(TMess::Debug, val ? _("Enabling the project page.") : _("Disabling the project page."));

    if(prjFlags()&Page::Empty) cfg("PARENT").setS("root");
    else if(prjFlags()&Page::Link) {
	//Checking for the recursion
	if(parentAddr().empty() || addr() == ("/"+TSYS::pathLev(addr(),0)+parentAddr()))
	    throw err_sys(_("The target page '%s' of the link is empty or recursive!"), parentAddr().c_str());

	mParent = ownerProj()->nodeAt(parentAddr());
    }

    try { Widget::setEnable(val); }	//!!!! Setup the previous parent to the actual one if that is broken for next childs renaming
    catch(TError&) {
	if(mParentAddrPrev.empty()) mParentAddrPrev = parentAddr();
	throw;
    }

    if(val && !parent().freeStat() && parent().at().rootId() != "Box" && !(prjFlags()&Page::Link)) {
	Widget::setEnable(false);
	throw TError(nodePath().c_str(),_("As a page, only a box based widget can be used!"));
    }

    if(val) {
	attrAdd(new TFld("pgOpen",_("Page: opened"),TFld::Boolean,TFld::NoFlag));
	attrAdd(new TFld("pgNoOpenProc",_("Page: process not opened"),TFld::Boolean,TFld::NoFlag));
    }

    //Enable/disable included pages
    vector<string> ls;
    pageList(ls);
    for(unsigned iL = 0; iL < ls.size(); iL++)
        try { pageAt(ls[iL]).at().setEnable(val); }
	catch(TError &err) { mess_err(err.cat.c_str(), "%s", err.mess.c_str()); }

    //Include widgets link update on the parrent change
    if(val) {
	bool lnkUpdt = (mParentAddrPrev.size() && parentAddr() != mParentAddrPrev);
	wdgList(ls, true);
	for(unsigned iL = 0; iL < ls.size(); iL++)
	    try {
		AutoHD<Widget> iw = wdgAt(ls[iL]);
		if(lnkUpdt && iw.at().parentAddr().compare(0,mParentAddrPrev.size()+1,mParentAddrPrev+"/") == 0) {
		    iw.at().setParentAddr(parentAddr()+iw.at().parentAddr().substr(mParentAddrPrev.size()));
		    iw.at().setEnable(true);
		}
		else if(manCrt) iw.at().modifClr();
	    } catch(TError &err) { }
	mParentAddrPrev = parentAddr();
    }

    ownerProj()->pageEnable(addr(), val);
}

void Page::wdgAdd( const string &wid, const string &name, const string &ipath, bool force )
{
    if(!isContainer())  throw TError(nodePath().c_str(),_("The widget is not a container!"));
    if(wdgPresent(wid)) throw err_sys(_("The widget '%s' is already present!"), wid.c_str());

    bool toRestoreInher = false;

    //Checking for the label "<deleted>"
    if(!force) {
	string db = ownerProj()->DB();
	string tbl = ownerProj()->tbl() + "_incl";
	TConfig cEl(&mod->elInclWdg());
	cEl.cfg("IDW").setS(addr());
	cEl.cfg("ID").setS(wid);
	if(TBDS::dataGet(db+"."+tbl,mod->nodePath()+tbl,cEl,TBDS::NoException) && cEl.cfg("PARENT").getS() == "<deleted>") {
	    TBDS::dataDel(db+"."+tbl, mod->nodePath()+tbl, cEl, TBDS::UseAllKeys|TBDS::NoException);
	    toRestoreInher = parent().at().wdgPresent(wid);
	}
    }

    //Same widget addition or restoring
    if(toRestoreInher) {
	inheritIncl(wid);
	wdgAt(wid).at().setEnable(true);
    }
    else {
	chldAdd(inclWdg, new PageWdg(wid,ipath));
	wdgAt(wid).at().setName(name);
    }

    //Call heritors include widgets update
    ResAlloc res(mHeritRes);
    for(unsigned iH = 0; iH < mHerit.size(); iH++)
	if(mHerit[iH].at().enable())
	    mHerit[iH].at().inheritIncl(wid);
    res.unlock();

    if(toRestoreInher)
	throw TError(TError::Core_CntrWarning, nodePath().c_str(), _("Restoring '%s' from the base container!"), wid.c_str());
}

AutoHD<Widget> Page::wdgAt( const string &wdg, int lev, int off ) const
{
    //Check for global
    if(lev == 0 && off == 0 && wdg.compare(0,1,"/") == 0)
	try { return (AutoHD<Widget>)ownerProj()->nodeAt(wdg,1); }
	catch(TError &err) { return AutoHD<Widget>(); }

    int offt = off;
    string iw = TSYS::pathLev(wdg, lev, true, &offt);
    if(iw.compare(0,3,"pg_") == 0) {
	if(pagePresent(iw.substr(3))) return pageAt(iw.substr(3)).at().wdgAt(wdg, 0, offt);
	else return AutoHD<Widget>();
    }

    return Widget::wdgAt(wdg, lev, off);
}

void Page::pageList( vector<string> &ls ) const
{
    ls.clear();
    if(prjFlags()&(Page::Template|Page::Container))
	chldList(mPage, ls);
}

string Page::pageAdd( const string &iid, const string &name, const string &orig )
{
    if(pagePresent(iid)) throw err_sys(_("The page '%s' is already present!"), iid.c_str());
    if(!(prjFlags()&(Page::Container|Page::Template)))
	throw TError(Engine::NoContainer, nodePath().c_str(), _("Page is not a container or a template!"));

    string id = chldAdd(mPage, new Page(TSYS::strEncode(sTrm(iid),TSYS::oscdID),orig));
    pageAt(id).at().setName(name);

    return id;
}

void Page::pageAdd( Page *iwdg )
{
    if(pagePresent(iwdg->id()))	delete iwdg;
    if(!(prjFlags()&(Page::Container|Page::Template))) {
	delete iwdg;
	throw TError(Engine::NoContainer, nodePath().c_str(), _("Page is not a container or a template!"));
    } else chldAdd(mPage, iwdg);
}

AutoHD<Page> Page::pageAt( const string &id ) const	{ return chldAt(mPage,id); }

void Page::resourceList( vector<string> &ls )
{
    //Append to the map for doublets remove
    map<string,bool> sortLs;
    for(unsigned iL = 0; iL < ls.size(); iL++) sortLs[ls[iL]] = true;
    ownerProj()->resourceDataList(ls);
    for(unsigned iL = 0; iL < ls.size(); iL++) sortLs[ls[iL]] = true;
    ls.clear();
    for(map<string,bool>::iterator iL = sortLs.begin(); iL != sortLs.end(); ++iL) ls.push_back(iL->first);

    if(!parent().freeStat()) parent().at().resourceList(ls);
}

string Page::resourceGet( const string &id, string *mime, int off, int *size, bool noParent ) const
{
    string mimeType, mimeData;

    if(!ownerProj()->resourceDataGet(id,mimeType,&mimeData,"",off,size) && !parent().freeStat() && !noParent)
	mimeData = parent().at().resourceGet(id, &mimeType, off, size);
    if(mime) *mime = mimeType;

    return mimeData;
}

void Page::resourceSet( const string &id, const string &data, const string &mime )
{
    ownerProj()->resourceDataSet(id, mime, data);
}

void Page::procChange( bool src )
{
    if(!src && proc().size()) return;

    //Update heritors' procedures
    ResAlloc res(mHeritRes);
    for(unsigned iH = 0; iH < mHerit.size(); iH++)
	if(mHerit[iH].at().enable())
	    mHerit[iH].at().procChange(false);
}

void Page::inheritAttr( const string &attr )
{
    bool mdf = isModify();
    Widget::inheritAttr(attr);
    if(!mdf && !(nodeFlg()&ModifiedS)) modifClr();
}

TVariant Page::vlGet( Attr &a )
{
    if(a.owner() == this) {
	if(a.id() == "owner") {
	    short perm = attrAt("perm").at().getI(true);
	    if(!(perm&01000)) return a.getS(true);
	    Page *oP = ownerPage();
	    return oP ? oP->attrAt("owner").at().getS() : ownerProj()->owner()+":"+ownerProj()->grp();
	}
	else if(a.id() == "perm") {
	    short perm = a.getI(true);
	    if(!(perm&01000)) return perm;
	    Page *oP = ownerPage();
	    return (oP?oP->attrAt("perm").at().getI():ownerProj()->permit())|01000;
	}
    }
    return Widget::vlGet(a);
}

TVariant Page::stlReq( Attr &a, const TVariant &vl, bool wr )
{
    if(stlLock()) return vl;
    string pid = sTrm(a.cfgTempl());
    if(pid.empty()) pid = a.id();
    if(!wr) return ownerProj()->stlPropGet(pid, vl.getS());
    if(ownerProj()->stlPropSet(pid,vl.getS())) return TVariant();
    return vl;
}

bool Page::cntrCmdGeneric( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	Widget::cntrCmdGeneric(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Project page: ")+addr(),RWRWR_,"root",SUI_ID);
	if(ctrMkNode("area",opt,-1,"/wdg",_("Widget")) && ctrMkNode("area",opt,-1,"/wdg/cfg",_("Configuration"))) {
	    if(!ownerPage() || !(ownerPage()->prjFlags()&Page::Template))
		ctrMkNode("fld",opt,2,"/wdg/st/pgTp",_("Page type"),RWRWR_,"root",SUI_ID,4,
			"tp","str", "idm","1", "dest","select", "select","/wdg/st/pgTpLst");
	    if((prjFlags()&Page::Empty) || (ownerPage() && (ownerPage()->prjFlags()&Page::Template) && !(ownerPage()->prjFlags()&Page::Container)))
		ctrMkNode("fld",opt,-1,"/wdg/st/parent",_("Parent"),R_R_R_,"root",SUI_ID,1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/wdg/st/timestamp",_("Date of modification"),R_R_R_,"root",SUI_ID,1,"tp","time");
	}
	if(prjFlags()&(Page::Template|Page::Container)) {
	    if(ctrMkNode("area",opt,1,"/page",_("Pages"))) {
		ctrMkNode("fld",opt,-1,"/page/nmb",_("Number"),R_R_R_,"root",SUI_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/page/page",_("Pages"),RWRWR_,"root",SUI_ID,5,"tp","br","idm","1","s_com","add,del","br_pref","pg_","idSz","30");
	    }
	    if(ctrMkNode("branches",opt,-1,"/br","",R_R_R_))
		ctrMkNode("grp",opt,-1,"/br/pg_",_("Page"),RWRWR_,"root",SUI_ID,2,"idm","1","idSz","30");
	}
	return true;
    }

    //Processing for the page commands
    string a_path = opt->attr("path");
    if(a_path == "/wdg/w_lst" && ctrChkNode(opt)) {
	if(ownerPage() && (ownerPage()->prjFlags()&Page::Template)) opt->childIns(0,"el")->setText("..");
	else if(prjFlags()&Page::Link) {
	    int c_lv = 0;
	    string c_path = "", c_el;
	    string lnk = parentAddr();

	    opt->childAdd("el")->setText(c_path);
	    for(int c_off = 0; (c_el=TSYS::pathLev(lnk,0,true,&c_off)).size(); c_lv++) {
		c_path += "/" + c_el;
		opt->childAdd("el")->setText(c_path);
	    }
	    try {
		vector<string>	ls;
		ownerProj()->nodeAt(lnk).at().nodeList(ls, "pg_");

		for(unsigned iL = 0; iL < ls.size(); iL++)
		    opt->childAdd("el")->setText(c_path+"/"+ls[iL]);
	    } catch(TError&) { }
	} else Widget::cntrCmdGeneric(opt);
    }
    else if(a_path == "/wdg/st/pgTp") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))
	    opt->setText(i2s(prjFlags()&(Page::Container|Page::Template|Page::Empty|Page::Link)));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR)) {
	    int prevFlgs = prjFlags(),
		newFlgs = prjFlags()^((prjFlags()^s2i(opt->text()))&(Page::Container|Page::Template|Page::Empty|Page::Link)),
		difFlgs = prevFlgs^newFlgs;

	    setPrjFlags(newFlgs);

	    vector<string> ls;
	    nodeList(ls, "pg_");
	    if(ls.size()) {
		if(difFlgs&Page::Template && !(newFlgs&Page::Template))
		    throw TError(TError::Core_CntrWarning, nodePath().c_str(), _("Consider to remove included pages linked to this page as a template due to it is not the template more!"));
		if(difFlgs&Page::Container && !(newFlgs&Page::Container))
		    throw TError(TError::Core_CntrWarning, nodePath().c_str(), _("Consider to remove included pages due to this page is not a container more!"));
	    }
	}
    }
    else if(a_path == "/wdg/st/pgTpLst" && ctrChkNode(opt)) {
	opt->childAdd("el")->setAttr("id","0")->setText(_("Standard"));
	if(!ownerPage() || !(ownerPage()->prjFlags()&Page::Template)) {
	    opt->childAdd("el")->setAttr("id", i2s(Page::Container))->setText(_("Container"));
	    opt->childAdd("el")->setAttr("id", i2s(Page::Container|Page::Empty))->setText(_("Logical container"));
	    opt->childAdd("el")->setAttr("id", i2s(Page::Template))->setText(_("Template"));
	    opt->childAdd("el")->setAttr("id", i2s(Page::Container|Page::Template))->setText(_("Container and template"));
	    opt->childAdd("el")->setAttr("id", i2s(Page::Link))->setText(_("Link"));
	}
    }
    else if(a_path == "/wdg/st/timestamp" && ctrChkNode(opt)) opt->setText(i2s(timeStamp()));
    else if(a_path == "/wdg/st/en" && ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR) && s2i(opt->text())) {
	setEnable(s2i(opt->text()));

	loadIO();	//Reloading the IOs only
	vector<string> lst;
	pageList(lst);
	for(unsigned iF = 0; iF < lst.size(); iF++)
	    pageAt(lst[iF]).at().load();
    }
    else if(a_path == "/br/pg_" || a_path == "/page/page") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD)) {
	    bool getChPgN = s2i(opt->attr("getChPgN"));
	    vector<string> lst, lst1;
	    pageList(lst);
	    for(unsigned iF = 0; iF < lst.size(); iF++) {
		XMLNode *no = opt->childAdd("el")->setAttr("id",lst[iF])->setText(trD(pageAt(lst[iF]).at().name()));
		if(getChPgN) { pageAt(lst[iF]).at().pageList(lst1); no->setAttr("chPgN", i2s(lst1.size())); }
	    }
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SUI_ID,SEC_WR)) {
	    opt->setAttr("id", pageAdd(opt->attr("id"),opt->text()));
	    pageAt(opt->attr("id")).at().setOwner(opt->attr("user"));
	    pageAt(opt->attr("id")).at().manCrt = true;
	}
	if(ctrChkNode(opt,"del",RWRWR_,"root",SUI_ID,SEC_WR))	pageDel(opt->attr("id"), true);
    }
    else if(a_path == "/page/nmb" && ctrChkNode(opt)) {
	vector<string> c_list;
	pageList(c_list);
	unsigned e_c = 0;
	for(unsigned iP = 0; iP < c_list.size(); iP++)
	    if(pageAt(c_list[iP]).at().enable()) e_c++;
	opt->setText(TSYS::strMess(_("All: %d; Enabled: %d"),c_list.size(),e_c));
    }
    else return Widget::cntrCmdGeneric(opt);

    return true;
}

void Page::cntrCmdProc( XMLNode *opt )
{
    if(cntrCmdServ(opt)) return;

    //Get page info
    if(opt->name() == "info") {
	cntrCmdGeneric(opt);
	cntrCmdAttributes(opt);
	if(!parent().freeStat()) {
	    cntrCmdLinks(opt);
	    cntrCmdProcess(opt);
	}
	return;
    }

    //Process command to page
    if(!(cntrCmdGeneric(opt) || cntrCmdAttributes(opt) || (parent( ).freeStat() ? false : cntrCmdLinks(opt) || cntrCmdProcess(opt))))
	TCntrNode::cntrCmdProc(opt);
}

bool Page::cntrCmdLinks( XMLNode *opt, bool lnk_ro )
{
    //Get page info
    if(opt->name() == "info")	return Widget::cntrCmdLinks(opt, lnk_ro);

    //Process command to page
    string a_path = opt->attr("path");
    if((a_path.find("/links/lnk/pl_") == 0 || a_path.find("/links/lnk/ls_") == 0) && ctrChkNode(opt)) {
	AutoHD<Widget> srcwdg(this);
	string nwdg = TSYS::strSepParse(a_path.substr(14),0,'.');
	string nattr = TSYS::strSepParse(a_path.substr(14),1,'.');
	if(nattr.size()) srcwdg = wdgAt(nwdg);
	else nattr = nwdg;

	bool is_pl = (a_path.substr(0,14) == "/links/lnk/pl_");
	if(!(srcwdg.at().attrAt(nattr).at().flgSelf()&(Attr::CfgLnkIn|Attr::CfgLnkOut))) {
	    if(!is_pl) throw TError(nodePath().c_str(),_("The variable is not a link"));
	    vector<string> a_ls;
	    string p_nm = TSYS::strSepParse(srcwdg.at().attrAt(nattr).at().cfgTempl(),0,'|');
	    srcwdg.at().attrList(a_ls);
	    unsigned iA;
	    for(iA = 0; iA < a_ls.size(); iA++)
		if(p_nm == TSYS::strSepParse(srcwdg.at().attrAt(a_ls[iA]).at().cfgTempl(),0,'|') &&
		    !(srcwdg.at().attrAt(a_ls[iA]).at().flgSelf()&Attr::CfgConst))
		{ nattr = a_ls[iA]; break; }
	    if(iA >= a_ls.size()) throw TError(nodePath().c_str(),_("The variable is not a link"));
	}

	string m_prm = srcwdg.at().attrAt(nattr).at().cfgVal();

	// Link interface process
	int c_lv = 0;
	string obj_tp = (m_prm.size() >= 4) ? m_prm.substr(0,4) : "";  //  TSYS::strSepParse(m_prm,0,':')+":";
	if(obj_tp.empty() || !(obj_tp == "val:" || obj_tp == "prm:" || obj_tp == "wdg:" || obj_tp == "arh:")) {
	    if(!is_pl) opt->childAdd("el")->setText(_("val:Constant value"));
	    opt->childAdd("el")->setText("prm:");
	    opt->childAdd("el")->setText("wdg:");
	    if(!is_pl && srcwdg.at().attrAt(nattr).at().flgGlob()&Attr::Address)
		opt->childAdd("el")->setText("arh:");
	}
        // Link elements process
	else {
	    int c_off = obj_tp.size();
	    vector<string> ls;
	    string c_path = obj_tp, cEl;
	    opt->childAdd("el")->setText("");

	    try {
		if(obj_tp == "prm:") {
		    m_prm = m_prm.substr(4);
		    if(is_pl && !SYS->daq().at().attrAt(m_prm,0,true).freeStat()) m_prm = m_prm.substr(0,m_prm.rfind("/"));
		    SYS->daq().at().ctrListPrmAttr(opt, m_prm, is_pl, 0, "prm:");
		}
		else if(obj_tp == "wdg:") {
		    opt->childAdd("el")->setText(c_path);
		    bool isAbs = (m_prm.compare(obj_tp.size(),1,"/") == 0);
		    for( ;(cEl=TSYS::pathLev(m_prm,0,true,&c_off)).size(); c_lv++) {
			c_path += ((c_lv||isAbs)?"/":"") + cEl;
			opt->childAdd("el")->setText(c_path);
		    }
		    if(!c_lv)  opt->childAdd("el")->setText(c_path+"/prj_"+ownerProj()->id());
		    else if(c_lv == 1 && isAbs) {
			ownerProj()->list(ls);
			if(ls.size()) opt->childAdd("el")->setText(_("=== Pages ==="));
			for(unsigned iL = 0; iL < ls.size(); iL++)
			    opt->childAdd("el")->setText(c_path+(c_lv?"/pg_":"pg_")+ls[iL]);
			return true;
		    }

		    AutoHD<Widget> wnd = srcwdg.at().wdgAt(c_path.substr(4),0);
		    if(!wnd.freeStat()) {
			if(!isAbs && dynamic_cast<Widget*>(wnd.at().nodePrev())) opt->childAdd("el")->setText(c_path+(c_lv?"/..":".."));
			if(dynamic_cast<Page*>(&wnd.at())) {
			    ((AutoHD<Page>)wnd).at().pageList(ls);
			    if(ls.size()) opt->childAdd("el")->setText(_("=== Pages ==="));
			    for(unsigned iL = 0; iL < ls.size(); iL++)
				opt->childAdd("el")->setText(c_path+(c_lv?"/pg_":"pg_")+ls[iL]);
			}
			wnd.at().wdgList(ls, true);
			if(ls.size()) opt->childAdd("el")->setText(_("=== Widgets ==="));
			for(unsigned iL = 0; iL < ls.size(); iL++)
			    opt->childAdd("el")->setText(c_path+(c_lv?"/wdg_":"wdg_")+ls[iL]);
			if(!is_pl) {
			    wnd.at().attrList(ls);
			    if(ls.size()) opt->childAdd("el")->setText(_("=== Attributes ==="));
			    for(unsigned iL = 0; iL < ls.size(); iL++)
			    opt->childAdd("el")->setText(c_path+(c_lv?"/a_":"a_")+ls[iL]);
			}
		    }
		}
		else if(m_prm == "arh:") {
		    SYS->archive().at().valList(ls);
		    for(unsigned iL = 0; iL < ls.size(); iL++)
			opt->childAdd("el")->setText(c_path+ls[iL]);
		}
	    } catch(TError &err) { }
	}
    }
    else return Widget::cntrCmdLinks(opt, lnk_ro);

    return true;
}

//************************************************
//* PageWdg: Container stored widget             *
//************************************************
PageWdg::PageWdg( const string &iid, const string &isrcwdg ) : Widget(iid), TConfig(&mod->elInclWdg()), mProcPer(cfg("PROC_PER").getId())
{
    cfg("ID").setS(id());
    mLnk = true;
    setParentAddr(isrcwdg);
}

PageWdg::~PageWdg( )
{

}

TCntrNode &PageWdg::operator=( const TCntrNode &node )
{
    if(ownerPage().parentAddr() == ".." && ownerPage().parent().at().wdgPresent(id())) {
	setParentAddr(ownerPage().parent().at().addr()+"/wdg_"+id());
	setEnable(true);
    }

    Widget::operator=(node);

    if(attrPresent("geomX") && ownerPage().attrPresent("geomW"))
	attrAt("geomX").at().setR(fmax(0,
	    fmin(ownerPage().attrAt("geomW").at().getR()-attrAt("geomW").at().getR()*attrAt("geomXsc").at().getR(),
		attrAt("geomX").at().getR())));
    if(attrPresent("geomY") && ownerPage().attrPresent("geomH"))
	attrAt("geomY").at().setR(fmax(0,
	    fmin(ownerPage().attrAt("geomH").at().getR()-attrAt("geomH").at().getR()*attrAt("geomYsc").at().getR(),
		attrAt("geomY").at().getR())));

    return *this;
}

Page &PageWdg::ownerPage( ) const	{ return *(Page*)nodePrev(); }

void PageWdg::postEnable( int flag )
{
    //Call parent method
    Widget::postEnable(flag);
    //Set parent page for this widget
    cfg("IDW").setS(ownerPage().addr());
}

void PageWdg::preDisable( int flag )
{
    if(flag&NodeRemove)
	ChldResrv = !(flag&NodeRemove_NoDelMark) && !parent().freeStat() && parent().at().isLink();

    Widget::preDisable(flag);
}

void PageWdg::postDisable( int flag )
{
    if(flag&NodeRemove) {
	string db  = ownerPage().ownerProj()->DB();
	string tbl = ownerPage().ownerProj()->tbl();

	//Remove from library table
	if(ChldResrv)	ownerPage().modif();	//To set the mark "<deleted>" in the parent
	else TBDS::dataDel(db+"."+tbl+"_incl", mod->nodePath()+tbl+"_incl", *this, TBDS::UseAllKeys);

	//Remove widget's work and users IO from library IO table
	string tAttrs = cfg("ATTRS").getS();

	TConfig cEl(&mod->elWdgIO());
	cEl.cfg("IDW").setS(ownerPage().addr(), true); cEl.cfg("IDC").setS(id(), true);
	TBDS::dataDel(db+"."+tbl+"_io", mod->nodePath()+tbl+"_io", cEl);
	cEl.setElem(&mod->elWdgUIO());
	cEl.cfg("IDW").setS(ownerPage().addr(), true); cEl.cfg("IDC").setS(id(), true);
	TBDS::dataDel(db+"."+tbl+"_uio", mod->nodePath()+tbl+"_uio", cEl);
    }
}

AutoHD<Widget> PageWdg::wdgAt( const string &wdg, int lev, int off ) const
{
    //Check for global
    if(lev == 0 && off == 0 && wdg.compare(0,1,"/") == 0)
	try { return (AutoHD<Widget>)ownerPage().ownerProj()->nodeAt(wdg,1); }
	catch(TError &err) { return AutoHD<Widget>(); }

    return Widget::wdgAt(wdg, lev, off);
}

string PageWdg::addr( ) const	{ return ownerPage().addr()+"/wdg_"+id(); }

string PageWdg::ico( ) const	{ return parent().freeStat() ? "" : parent().at().ico(); }

void PageWdg::setParentAddr( const string &isw )
{
    if(enable() && cfg("PARENT").getS() != isw) setEnable(false);
    cfg("PARENT").setS(isw);
}

void PageWdg::setEnable( bool val, bool force )
{
    if(enable() == val) return;

    Widget::setEnable(val);

    //Disable heritors widgets
    if(val)
	for(unsigned iH = 0; iH < ownerPage().herit().size(); iH++)
	    if(ownerPage().herit()[iH].at().wdgPresent(id()) && !ownerPage().herit()[iH].at().wdgAt(id()).at().enable())
		try { ownerPage().herit()[iH].at().wdgAt(id()).at().setEnable(true); }
		catch(...) { mess_err(nodePath().c_str(),_("Error enabling the inheriting widget '%s'."),id().c_str()); }
}

string PageWdg::calcId( )	{ return parent().freeStat() ? "" : parent().at().calcId(); }

string PageWdg::calcLang( ) const	{ return parent().freeStat() ? "" : parent().at().calcLang(); }

string PageWdg::calcProg( ) const	{ return parent().freeStat() ? "" : parent().at().calcProg(); }

string PageWdg::calcProgStors( const string &attr )
{
    string rez = parent().freeStat() ? "" : parent().at().calcProgStors(attr);
    if(attr.size() && attrAt(attr).at().aModif() && rez.find(ownerPage().ownerProj()->DB()) == string::npos)
	rez = ownerPage().ownerProj()->DB() + ";" + rez;

    return rez;
}

int PageWdg::calcPer( ) const	{ return (mProcPer == PerVal_Parent && !parent().freeStat()) ? parent().at().calcPer() : mProcPer; }

void PageWdg::load_( TConfig *icfg )
{
    if(!SYS->chkSelDB(ownerPage().ownerProj()->DB())) throw TError();

    setStlLock(true);

    //Load generic widget's data
    string db  = ownerPage().ownerProj()->DB();
    string tbl = ownerPage().ownerProj()->tbl()+"_incl";
    if(icfg) *(TConfig*)this = *icfg;
    else TBDS::dataGet(db+"."+tbl, mod->nodePath()+tbl, *this);

    //Inherit modify attributes
    vector<string> als;
    attrList(als);
    string tAttrs = cfg("ATTRS").getS();
    for(unsigned iA = 0; iA < als.size(); iA++) {
	if(!attrPresent(als[iA])) continue;
	AutoHD<Attr> attr = attrAt(als[iA]);
	if(attr.at().aModif() && tAttrs.find(als[iA]+";") == string::npos) {
	    attr.at().setAModif_(0);
	    inheritAttr(als[iA]);
	}
    }

    //Load generic attributes
    mod->attrsLoad(*this, db+"."+ownerPage().ownerProj()->tbl(), ownerPage().addr(), id(), tAttrs, true);

    //Load all other attributes
    loadIO();

    setStlLock(false);
}

void PageWdg::loadIO( )
{
    if(!enable()) return;

    //Load widget's work attributes
    mod->attrsLoad(*this, ownerPage().ownerProj()->DB()+"."+ownerPage().ownerProj()->tbl(), ownerPage().addr(), id(), cfg("ATTRS").getS());
}

void PageWdg::save_( )
{
    string db  = ownerPage().ownerProj()->DB();
    string tbl = ownerPage().ownerProj()->tbl();

    //Save generic attributes
    cfg("ATTRS").setS(mod->attrsSave(*this, db+"."+tbl, ownerPage().addr(), id(), true));

    //Save generic widget's data
    TBDS::dataSet(db+"."+tbl+"_incl", mod->nodePath()+tbl+"_incl", *this);

    //Save widget's attributes
    saveIO();
}

void PageWdg::saveIO( )
{
    if(!enable()) return;

    //Save widget's attributes
    mod->attrsSave(*this, ownerPage().ownerProj()->DB()+"."+ownerPage().ownerProj()->tbl(), ownerPage().addr(), id());
}

void PageWdg::wClear( )
{
    //Checking and restoring the container common inheritance
    if(enable() && ownerPage().parent().at().wdgPresent(id()) && parentAddr() != ownerPage().parent().at().wdgAt(id()).at().addr()) {
	setParentAddr(ownerPage().parent().at().wdgAt(id()).at().addr());
	setEnable(true);
    }

    Widget::wClear();
    cfg("ATTRS").setS("");
}

void PageWdg::inheritAttr( const string &attr )
{
    bool mdf = isModify();
    Widget::inheritAttr(attr);
    if(!mdf && !(nodeFlg()&ModifiedS)) modifClr();
}

void PageWdg::resourceList( vector<string> &ls )
{
    ownerPage().resourceList(ls);
    if(!parent().freeStat()) parent().at().resourceList(ls);
}

string PageWdg::resourceGet( const string &id, string *mime, int off, int *size, bool noParent ) const
{
    string mimeType, mimeData;

    if((mimeData=ownerPage().resourceGet(id,&mimeType,off,size)).empty() && !parent().freeStat() && !noParent)
	mimeData = parent().at().resourceGet(id, &mimeType, off, size);
    if(mime) *mime = mimeType;

    return mimeData;
}

void PageWdg::resourceSet( const string &id, const string &data, const string &mime )
{
    ownerPage().resourceSet(id, mime, data);
}

void PageWdg::procChange( bool src )
{
    //Update heritors' procedures
    ResAlloc res(mHeritRes);
    for(unsigned iH = 0; iH < mHerit.size(); iH++)
	if(mHerit[iH].at().enable())
	    mHerit[iH].at().procChange(false);
}

void PageWdg::cntrCmdProc( XMLNode *opt )
{
    if(cntrCmdServ(opt)) return;

    //Get page info
    if(opt->name() == "info") {
	cntrCmdGeneric(opt);
	cntrCmdAttributes(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Widget link: ")+id(),RWRWR_,"root",SUI_ID);
	return;
    }

    //Processing for the page commands
    string a_path = opt->attr("path");
    if(a_path == "/wdg/st/en" && ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR) && s2i(opt->text())) {
	setEnable(s2i(opt->text()));
	loadIO();	//Reloading the widget IOs only
    }
    else if(!(cntrCmdGeneric(opt) || cntrCmdAttributes(opt))) TCntrNode::cntrCmdProc(opt);
}
