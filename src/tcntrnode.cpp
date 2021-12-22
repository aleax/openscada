
//OpenSCADA file: tcntrnode.cpp
/***************************************************************************
 *   Copyright (C) 2003-2021 by Roman Savochenko, <roman@oscada.org>       *
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
#include <unistd.h>
#include <stdarg.h>
#include <limits.h>

#include "xml.h"
#include "tsys.h"
#include "tmess.h"
#include "tvariant.h"
#include "tcntrnode.h"

using namespace OSCADA;

//*************************************************
//* TCntrNode                                     *
//*************************************************

//*************************************************
//* Controll scenaries language section           *
TCntrNode::TCntrNode( TCntrNode *iprev ) : mChM(true), mDataM(true), chGrp(NULL), mUse(0), mOi(USHRT_MAX), mFlg(0)
{
    setNodeMode(Disabled);
    prev.node = iprev;
    prev.grp = -1;
    modif();

    if(SYS && this != SYS && mess_lev() == TMess::Debug) SYS->cntrIter(objName(), 1);
}

TCntrNode::TCntrNode( const TCntrNode &src ) : mChM(true), mDataM(true), chGrp(NULL), mUse(0), mOi(USHRT_MAX), mFlg(0)
{
    setNodeMode(Disabled);
    prev.node = NULL;
    prev.grp = -1;
    modif();

    operator=(src);
}

TCntrNode::~TCntrNode( )
{
    nodeDelAll();
    if(chGrp) delete chGrp;

    if(this != SYS && mess_lev() == TMess::Debug) SYS->cntrIter(objName(), -1);
}

TCntrNode &TCntrNode::operator=( const TCntrNode &node )	{ return *this; }

void TCntrNode::mess_sys( int8_t level, const char *fmt,  ... )
{
    if(level < TMess::Debug || level > TMess::MaxLev || !Mess || !TMess::messLevelTest(Mess->messLevel(),level)) return;

    char str[prmStrBuf_SZ];
    va_list argptr;

    va_start(argptr, fmt);
    vsnprintf(str, sizeof(str), fmt, argptr);
    va_end(argptr);

    string mess = str, tvl;
    bool first = true;
    for(TCntrNode *cN = this; level != TMess::Debug && cN; cN = cN->nodePrev(true))
	if((tvl=cN->nodeNameSYSM()).size()) {
	    mess = tvl + (first?": ":" > ") + mess;
	    first = false;
	}

    Mess->put(nodePath().c_str(), level, "%s", mess.c_str());
}

TError TCntrNode::err_sys( const char *fmt,  ... ) const
{
    char str[prmStrBuf_SZ];
    va_list argptr;

    va_start(argptr, fmt);
    vsnprintf(str, sizeof(str), fmt, argptr);
    va_end(argptr);

    string mess = str, tvl;
    bool first = true;
    for(const TCntrNode *cN = this; cN; cN = cN->nodePrev(true))
	if((tvl=cN->nodeNameSYSM()).size()) {
	    mess = tvl + (first?": ":" > ") + mess;
	    first = false;
	}

    return TError(nodePath().c_str(), "%s", mess.c_str());
}

TError TCntrNode::err_sys( int cod, const char *fmt,  ... ) const
{
    char str[prmStrBuf_SZ];
    va_list argptr;

    va_start(argptr, fmt);
    vsnprintf(str, sizeof(str), fmt, argptr);
    va_end(argptr);

    string mess = str, tvl;
    bool first = true;
    for(const TCntrNode *cN = this; cN; cN = cN->nodePrev(true))
	if((tvl=cN->nodeNameSYSM()).size()) {
	    mess = tvl + (first?": ":" > ") + mess;
	    first = false;
	}

    return TError(cod, nodePath().c_str(), "%s", mess.c_str());
}

void TCntrNode::nodeDelAll( )
{
    if(nodeMode() != Disabled)	nodeDis();

    TMap::iterator p;
    MtxAlloc res(mChM, true);
    for(unsigned iG = 0; chGrp && iG < chGrp->size(); iG++)
	while((p=(*chGrp)[iG].elem.begin()) != (*chGrp)[iG].elem.end()) {
	    delete p->second;
	    (*chGrp)[iG].elem.erase(p);
	}
}

void TCntrNode::setNodeMode( char mode )
{
    dataRes().lock();
    mFlg = (mFlg&(~0x03))|(mode&0x03);
    dataRes().unlock();
}

XMLNode *TCntrNode::ctrId( XMLNode *inf, const string &name_id, bool noex )
{
    int l_off = 0;
    string s_el;

    XMLNode *t_node = inf;
    while(true) {
	s_el = TSYS::pathLev(name_id, 0, true, &l_off);
	if(s_el.empty()) return t_node;
	bool ok = false;
	for(unsigned i_f = 0; i_f < t_node->childSize(); i_f++)
	    if(t_node->childGet(i_f)->attr("id") == s_el) {
		t_node = t_node->childGet(i_f);
		ok = true;
		break;
	    }
	if(!ok) break;
    }

    if(noex) return NULL;
    throw TError("XML", _("Field id = %s(%s) is missing!"), name_id.c_str(), s_el.c_str());
}

void TCntrNode::cntrCmd( XMLNode *opt, int lev, const string &ipath, int off )
{
    string path = ipath.empty() ? opt->attr("path") : ipath;
    string s_br = TSYS::pathLev(path, lev, true, &off);

    bool trCtxHold = false;
    if(ipath.empty() && Mess->translDyn()) Mess->trCtx(opt->attr("user")+"\n"+opt->attr("lang"), &trCtxHold);

    try {
	if(!s_br.empty() && s_br[0] != '/') {
	    AutoHD<TCntrNode> chNd;
	    MtxAlloc res(mChM, true);
	    for(unsigned iG = 0; chGrp && iG < chGrp->size(); iG++)
		if(s_br.compare(0,(*chGrp)[iG].id.size(),(*chGrp)[iG].id) == 0) {
		    chNd = chldAt(iG, s_br.substr((*chGrp)[iG].id.size()));
		    break;
		}
	    //Go to the default thread
	    if(chNd.freeStat() && chGrp) chNd = chldAt(0, s_br);
	    res.unlock();
	    if(!chNd.freeStat()) chNd.at().cntrCmd(opt, 0, path, off);
	    return;
	}
	//Post the command to the node
	if(opt->name() == "CntrReqs")
	    for(unsigned iN = 0; iN < opt->childSize(); iN++) {
		XMLNode *nChld = opt->childGet(iN);
		nChld->setAttr("user", opt->attr("user"))->setAttr("lang", opt->attr("lang"));
		cntrCmd(nChld);
		nChld->attrDel("user"); nChld->attrDel("lang");
	    }
	else {
	    opt->setAttr("path", s_br);

	    cntrCmdProc(opt);
	    if(opt->attr("rez") != "0")
		throw TError("ContrItfc", _("%s:%s:> Error in the control item '%s'!"), opt->name().c_str(), (nodePath()+path).c_str(), s_br.c_str());

	    // Check and put the command to the redundant stations
	    string aNm = opt->name();
	    if(SYS->rdPrimCmdTr() && SYS->rdEnable() && SYS->rdActive() && !s2i(opt->attr("reforwardRedundReq")) && !s2i(opt->attr("reforwardRedundOff")) &&
		    (s2i(opt->attr("primaryCmd")) ||
			aNm == "set" || aNm == "add" || aNm == "ins" || aNm == "del" || aNm == "move" ||
			aNm == "load" || (aNm == "save" && !s2i(opt->attr("ctx"))) || aNm == "copy")) {
		string lstStat;
		opt->setAttr("path", nodePath()+"/"+TSYS::strEncode(s_br,TSYS::PathEl))->setAttr("primaryCmd", "")->setAttr("reforwardRedundReq", "1");
		try{ while((lstStat=SYS->rdStRequest(*opt,lstStat,true)).size()) ; }
		catch(TError &) { }
	    }
	    opt->attrDel("reforwardRedundReq");
	}
    } catch(TError &err) {
	if(err.cod == TError::Core_CntrWarning) opt->setAttr("rez", i2s(err.cod));
	else opt->setAttr("rez", i2s(TError::Core_CntrError));
	opt->childClear();
	opt->setAttr("mcat", err.cat);
	opt->setText(err.mess);
    }

    opt->setAttr("path", path);

    if(ipath.empty() && Mess->translDyn() && trCtxHold) Mess->trCtx("");
}

//*************************************************
//* Resource section                              *
void TCntrNode::nodeEn( int flag )
{
    if(nodeMode() == Enabled)	return;
    if(nodeMode() != Disabled)	throw err_sys(_("The node is already being processed!"));

    setNodeMode(DoEnable);

    preEnable(flag);

    MtxAlloc res(mChM, true);
    for(unsigned iG = 0; chGrp && iG < chGrp->size(); iG++) {
	vector<string> chLs;
	TMap::iterator p;
	chldList(iG, chLs, true, false);
	for(unsigned iN = 0; iG < chGrp->size() && iN < chLs.size(); iN++) {
	    if((p=(*chGrp)[iG].elem.find(chLs[iN].c_str())) == (*chGrp)[iG].elem.end()) continue;
	    AutoHD<TCntrNode> ndO(p->second);
	    if(ndO.at().nodeMode() != Disabled)	continue;
	    res.unlock();
	    ndO.at().nodeEn(flag);
	    res.lock();
	}
    }
    res.unlock();

    setNodeMode(Enabled);

    try { postEnable(flag); }
    catch(TError &err)	{ mess_err(err.cat.c_str(), "%s", err.mess.c_str()); }
}

void TCntrNode::nodeDis( long tm, int flag )
{
    if(nodeMode() == Disabled)	return;
    if(nodeMode() != Enabled)	throw err_sys(_("The node is already being processed!"));

    preDisable(flag);

    setNodeMode(DoDisable);

    try {
	MtxAlloc res(mChM, true);
	for(unsigned iG = 0; chGrp && iG < chGrp->size(); iG++) {
	    vector<string> chLs;
	    TMap::iterator p;
	    chldList(iG, chLs, true, false);
	    for(unsigned iN = 0; iG < chGrp->size() && iN < chLs.size(); iN++) {
		if((p=(*chGrp)[iG].elem.find(chLs[iN].c_str())) == (*chGrp)[iG].elem.end()) continue;
		AutoHD<TCntrNode> ndO(p->second);
		if(ndO.at().nodeMode() != Enabled)	continue;
		res.unlock();
		ndO.at().nodeDis(tm, flag);
		res.lock();
	    }
	}
	res.unlock();

	//Wait of free node
	time_t t_cur = time(NULL);
	MtxAlloc res1(dataRes(), true);		//!! Added to prevent a possible attach and it next disable and free the node, by mUse control
	while(mUse > 1) {
	    // Check timeout
	    if(/*tm &&*/ time(NULL) >= (t_cur+tm)) {
		if(!SYS->isFinalKill())
		    throw err_sys(_("Waiting time exceeded. The object is used by %d users. Release the object first!"), mUse-1);
		mess_sys(TMess::Error, _("Error blocking node.\n"
		    "The node forced to disable which can cause to crash.\n"
		    "This problem mostly in a user procedure, if the program exits!"));
		break;
	    }
	    res1.unlock();
	    TSYS::sysSleep(1/*prmWait_DL*/);
	    res1.lock();
	    if(mUse > 1) mess_sys(TMess::Warning, _("Expecting release %d users!"), mUse-1);
	}

	setNodeMode(Disabled);
	res1.unlock();
	modif();

	postDisable(flag);
    } catch(TError &err) {
	mess_err(err.cat.c_str(), "%s", err.mess.c_str());
	mess_sys(TMess::Error, _("Error turning off node. Turning on back the node."));
	setNodeMode(Disabled);
	nodeEn(NodeRestore|flag);
	throw;
    }
}

void TCntrNode::nodeList( vector<string> &list, const string &gid )
{
    vector<string> tls;
    list.clear();
    MtxAlloc res(mChM, true);
    for(unsigned iGr = 0; chGrp && iGr < chGrp->size(); iGr++)
	if(gid.empty() || gid == (*chGrp)[iGr].id) {
	    chldList(iGr, tls);
	    for(unsigned iL = 0; iL < tls.size(); iL++)
		list.push_back((*chGrp)[iGr].id+tls[iL]);
	    if(!gid.empty())	break;
	}
}

AutoHD<TCntrNode> TCntrNode::nodeAt( const string &path, int lev, char sep, int off, bool noex )
{
    try {
	string s_br = sep ? TSYS::strDecode(TSYS::strSepParse(path,lev,sep,&off),TSYS::PathEl) :
			    TSYS::pathLev(path, lev, true, &off);
	if(s_br.empty()) {
	    if(nodeMode() == Disabled) throw err_sys(_("Node is disabled!"));
	    return this;
	}

	AutoHD<TCntrNode> chN;
	MtxAlloc res(mChM, true);
	for(unsigned iG = 0; chGrp && iG < chGrp->size(); iG++)
	    if(s_br.compare(0,(*chGrp)[iG].id.size(),(*chGrp)[iG].id) == 0) {
		chN = chldAt(iG, s_br.substr((*chGrp)[iG].id.size()));
		break;
	    }
	if(chN.freeStat() && chGrp) chN = chldAt(0, s_br);	//Go to default group
	res.unlock();
	if(!chN.freeStat()) return chN.at().nodeAt(path, 0, sep, off, noex);
	throw err_sys(_("The node '%s' is missing!"), s_br.c_str());
    } catch(TError &err) { if(!noex) throw; }

    return NULL;
}

void TCntrNode::nodeDel( const string &path, char sep, int flag )
{
    AutoHD<TCntrNode> del_n = nodeAt(path, 0, sep);
    int n_grp = del_n.at().prev.grp;
    string n_id  = del_n.at().nodeName();
    del_n = AutoHD<TCntrNode>(del_n.at().prev.node);
    del_n.at().chldDel(n_grp, n_id, -1, flag);
}

void TCntrNode::nodeCopy( const string &src, const string &dst, const string &user )
{
    if(src == dst) return;

    //Attach to source node
    AutoHD<TCntrNode> srcN = SYS->nodeAt(src);

    //Parse destination node path
    string dElp, dEl, tEl;
    int nDel = 0;
    for(int off = 0; !(tEl=TSYS::pathLev(dst,0,true,&off)).empty(); nDel++)
    { if(nDel) dElp += "/" + dEl; dEl = tEl; }
    if(!nDel) throw srcN.at().err_sys(_("Copy from '%s' to '%s' is not possible."), src.c_str(), dst.c_str());

    //Connect to destination containers node
    AutoHD<TCntrNode> dstN = SYS->nodeAt(dElp);

    //Get allow branches' containers and find want group
    XMLNode brReq("info");
    brReq.setAttr("user",user)->setAttr("path","/%2fbr");
    dstN.at().cntrCmd(&brReq);
    if(s2i(brReq.attr("rez")) || !brReq.childGet(0,true))
	throw srcN.at().err_sys(_("Target node has no branches."));
    XMLNode *branch = brReq.childGet(0);
    int iB;
    for(iB = 0; iB < (int)branch->childSize(); iB++)
	if(branch->childGet(iB)->attr("id") == dEl.substr(0,branch->childGet(iB)->attr("id").size()) &&
		s2i(branch->childGet(iB)->attr("acs"))&SEC_WR)
	    break;
    if(iB >= (int)branch->childSize())
	throw srcN.at().err_sys(_("Target node does not have the required branch."));
    bool idm = s2i(branch->childGet(iB)->attr("idm"));
    string nGrp = branch->childGet(iB)->attr("id");
    dEl = dEl.substr(nGrp.size());
    iB = dstN.at().grpId(nGrp);
    if(iB < 0) throw srcN.at().err_sys(_("Target node does not have the required branch."));

    //Connect or create new destination node
    if(!dstN.at().chldPresent(iB,dEl)) {
	brReq.clear()->setName("add")->setAttr("user",user)->setAttr("path","/%2fbr%2f"+nGrp);
	if(idm) brReq.setAttr("id", dEl);
	else brReq.setText(dEl);
	dstN.at().cntrCmd(&brReq);
	if(s2i(brReq.attr("rez")))	throw TError(brReq.attr("mcat").c_str(), brReq.text().c_str());
    }

    //Same copy call
    dstN.at().chldAt(iB, dEl).at() = srcN.at();
}

unsigned TCntrNode::grpAdd( const string &iid, bool iordered )
{
    MtxAlloc res(mChM, true);
    if(!chGrp) chGrp = new vector<GrpEl>;

    unsigned g_id;
    for(g_id = 0; g_id < chGrp->size(); g_id++)
	if((*chGrp)[g_id].id == "<free>" || (*chGrp)[g_id].id == iid) break;
    if(g_id == chGrp->size())	chGrp->push_back(GrpEl());
    (*chGrp)[g_id].id = iid;
    (*chGrp)[g_id].ordered = iordered;

    return g_id;
}

void TCntrNode::grpDel( int8_t id )
{
    MtxAlloc res(mChM, true);
    if(!chGrp || id < 0 || id >= (int8_t)chGrp->size())	return;

    //Clear childs
    TMap::iterator p;
    while((p = (*chGrp)[id].elem.begin()) != (*chGrp)[id].elem.end()) {
	delete p->second;
	(*chGrp)[id].elem.erase(p);
    }
    //Mark for free
    (*chGrp)[id].id = "<free>";
}

int8_t TCntrNode::grpSize( )
{
    MtxAlloc res(mChM, true);
    return chGrp ? chGrp->size() : 0;
}

int8_t TCntrNode::grpId( const string &sid )
{
    MtxAlloc res(mChM, true);
    for(int g_id = 0; chGrp && g_id < (int)chGrp->size(); g_id++)
	if((*chGrp)[g_id].id == sid) return g_id;

    return -1;
}

TCntrNode::GrpEl &TCntrNode::grpAt( int8_t iid )
{
    if(iid < 0 || iid >= grpSize())	throw err_sys(_("Error group of branches '%d'."), iid);
    return (*chGrp)[iid];
}

void TCntrNode::chldList( int8_t igr, vector<string> &list, bool noex, bool onlyEn ) const
{
    list.clear();

    if(nodeMode() == Disabled) { if(noex) return; else throw err_sys(_("Node is disabled!")); }

    MtxAlloc res(const_cast<ResMtx&>(mChM), true);
    if(!chGrp || igr < 0 || igr >= (int)chGrp->size()) {
	if(noex) return;
	else throw err_sys(_("Error group of childs %d!"), igr);
    }
    list.reserve((*chGrp)[igr].elem.size());
    if(!(*chGrp)[igr].ordered)
	for(TMap::iterator p = (*chGrp)[igr].elem.begin(); p != (*chGrp)[igr].elem.end(); ++p) {
	    if(onlyEn && p->second->nodeMode() != Enabled) continue;
	    list.push_back(p->first);
	}
    else {
	for(TMap::iterator p = (*chGrp)[igr].elem.begin(); p != (*chGrp)[igr].elem.end(); ++p) {
	    if(onlyEn && p->second->nodeMode() != Enabled) continue;
	    while(p->second->mOi >= list.size()) list.push_back("");
	    list[p->second->mOi] = p->first;
	}
	if(onlyEn)	//Remove empty
	    for(int iL = 0; iL < (int)list.size(); iL++)
		if(!list[iL].size()) list.erase(list.begin()+(iL--));
    }
}

bool TCntrNode::chldPresent( int8_t igr, const string &name ) const
{
    MtxAlloc res(const_cast<ResMtx&>(mChM), true);
    if(!chGrp || igr >= (int)chGrp->size()) throw err_sys(_("Error group of childs %d!"), igr);
    if(nodeMode() == Disabled)	throw err_sys(_("Node is disabled!"));

    TMap::iterator p = (*chGrp)[igr].elem.find(name.c_str());
    if(p != (*chGrp)[igr].elem.end()) return true;

    return false;
}

string TCntrNode::chldAdd( int8_t igr, TCntrNode *node, int pos, bool noExp )
{
    if(nodeMode() != Enabled)		{ delete node; throw err_sys(_("Node is not enabled!")); }
    if(sTrm(node->nodeName()).empty())	{ delete node; throw err_sys(_("Id of the child that is adding is empty!")); }

    MtxAlloc res(mChM, true);
    if(!chGrp || igr >= (int)chGrp->size())	{ delete node; throw err_sys(_("Error group of childs %d!"), igr); }

    TMap::iterator p;
    if((p=(*chGrp)[igr].elem.find(node->nodeName())) != (*chGrp)[igr].elem.end()) {
	AutoHD<TCntrNode> chN(p->second);
	res.unlock();
	delete node;
	if(chN.at().nodeMode() == Disabled) chN.at().nodeEn(TCntrNode::NodeRestore);
	//if(!noExp) throw err_sys(_("The node '%s' is already present."), p->first);
	return p->first;
    }

    node->prev.node = this;
    node->prev.grp = igr;
    if((*chGrp)[igr].ordered) {
	pos = (pos < 0 || pos > (int)(*chGrp)[igr].elem.size()) ? (int)(*chGrp)[igr].elem.size() : pos;
	node->mOi = pos;
	for(p = (*chGrp)[igr].elem.begin(); p != (*chGrp)[igr].elem.end(); p++)
	    if(p->second->mOi >= pos) p->second->mOi++;
    }
    (*chGrp)[igr].elem.insert(std::pair<const char*,TCntrNode*>(node->nodeName(),node));
    res.unlock();

    if(node->nodeMode() == Disabled) node->nodeEn(TCntrNode::NodeConnect);

    return node->nodeName();
}

void TCntrNode::chldDel( int8_t igr, const string &name, long tm, int flag )
{
    if(nodeMode() == DoDisable)	return;
    if(nodeMode() == DoEnable) throw err_sys(_("Node is being processed now for enable!"));

    if(SYS->stopSignal())	tm = prmWait_TM*5;
    else if(tm < 0)		tm = 0;	//prmWait_TM;	//Do not wait anything by default

    AutoHD<TCntrNode> chN = chldAt(igr, name);
    if(chN.at().nodeMode() == Enabled) chN.at().nodeDis(tm, flag);
    chN.free();

    MtxAlloc res(mChM, true);
    if(!chGrp || igr >= (int)chGrp->size()) throw err_sys(_("Error group of childs %d!"), igr);
    TMap::iterator p = (*chGrp)[igr].elem.find(name.c_str());
    if(p == (*chGrp)[igr].elem.end()) return;
    if((*chGrp)[igr].ordered) {
	int pos = p->second->mOi;
	for(TMap::iterator p1 = (*chGrp)[igr].elem.begin(); p1 != (*chGrp)[igr].elem.end(); ++p1)
	    if(p1->second->mOi > pos) p1->second->mOi--;
    }
    delete p->second;
    (*chGrp)[igr].elem.erase(p);
}

string TCntrNode::storage( const string &cnt, bool forQueueOfData ) const
{
    const_cast<TCntrNode*>(this)->dataRes().lock();
    string prcS = cnt;
    const_cast<TCntrNode*>(this)->dataRes().unlock();

    //The work DB
    if(!forQueueOfData) return TSYS::strLine(prcS, 0);

    //The queue
    string work = TSYS::strLine(prcS, 0), tLn;
    for(int pos = 1 /*queue start*/; (tLn=TSYS::strLine(prcS,pos)).size(); ++pos)
	if(tLn != work) return tLn;

    return "";
}

void TCntrNode::setStorage( string &cnt, const string &vl, bool forQueueOfData )
{
    dataRes().lock();
    string prcS = cnt, prcS_, tLn;
    dataRes().unlock();

    //The queue
    string work = TSYS::strLine(prcS,0);
    if(forQueueOfData)	prcS_ = work + "\n";
    //The work DB
    else if(vl.empty() || vl == work)	return;

    if(vl.size()) prcS_ += vl + "\n";

    bool isFound = false;
    for(int pos = 1 /*queue start*/; (tLn=TSYS::strLine(prcS,pos)).size(); ++pos) {
	// Pass for equal items
	if(forQueueOfData && tLn == vl)	continue;
	// Pass for removing first not work item
	if(forQueueOfData && vl.empty() && !isFound && tLn != work) { isFound = true; continue; }
	prcS_ += tLn + "\n";
    }

    dataRes().lock();
    cnt = prcS_;
    dataRes().unlock();
}

void TCntrNode::setNodeFlg( char flg )
{
    dataRes().lock();
    mFlg |= flg&(SelfModify|SelfModifyS|SelfSaveForceOnChild);
    dataRes().unlock();
}

unsigned TCntrNode::nodeUse( bool selfOnly )
{
    //MtxAlloc res1(dataRes(), true);

    unsigned i_use = mUse;
    //res1.unlock();

    TMap::iterator p;
    MtxAlloc res2(mChM, true);
    for(unsigned iG = 0; !selfOnly && chGrp && iG < chGrp->size(); iG++)
	for(p = (*chGrp)[iG].elem.begin(); p != (*chGrp)[iG].elem.end(); ++p)
	    if(p->second->nodeMode() != Disabled)
		i_use += p->second->nodeUse();
    res2.unlock();

    return i_use;
}

string TCntrNode::nodePath( char sep, bool from_root ) const
{
    if(sep) {
	if(prev.node) {
	    if(from_root && !prev.node->prev.node)
		return ((prev.grp<0)?"":(*(prev.node->chGrp))[prev.grp].id) + nodeName();
	    else
		return prev.node->nodePath(sep,from_root)+string(1,sep)+
			((prev.grp<0)?"":(*(prev.node->chGrp))[prev.grp].id) + nodeName();
	}
	else return nodeName();
    }
    else return prev.node ? (prev.node->nodePath(sep,from_root)+((prev.grp<0)?"":(*(prev.node->chGrp))[prev.grp].id)+nodeName()+"/") :
			    (from_root?string("/"):(string("/")+nodeName()+"/"));
}

TCntrNode *TCntrNode::nodePrev( bool noex ) const
{
    if(prev.node) return prev.node;
    if(noex)	return NULL;
    throw err_sys(_("Node root or not connected!"));
}

AutoHD<TCntrNode> TCntrNode::chldAt( int8_t igr, const string &name, const string &user ) const
{
    if(nodeMode() == Disabled)	throw err_sys("Node is disabled!");

    MtxAlloc res(const_cast<ResMtx&>(mChM), true);
    if(!chGrp || igr >= (int)chGrp->size()) throw err_sys(_("Error group of childs %d!"), igr);
    TMap::iterator p = (*chGrp)[igr].elem.find(name.c_str());
    if(p == (*chGrp)[igr].elem.end())
	throw err_sys(TError::Core_NoNode, _("Element '%s' is not present!"), name.c_str());
    AutoHD<TCntrNode> chN(p->second, user);
    if(chN.at().nodeMode() == Disabled) throw err_sys(_("Element '%s' is disabled!"), name.c_str());
    res.unlock();

    return chN;
}

int TCntrNode::isModify( int f )
{
    int rflg = 0;
    MtxAlloc res1(dataRes(), true);
    if(f&Self && mFlg&SelfModify) rflg |= Self;
    if(f&Child) {
	res1.unlock();

	MtxAlloc res2(mChM, true);
	for(unsigned iG = 0, iN; chGrp && iG < chGrp->size(); iG++) {
	    vector<string> chLs;
	    TMap::iterator p;
	    chldList(iG, chLs, true, false);
	    for(iN = 0; iG < chGrp->size() && iN < chLs.size(); iN++) {
		if((p=(*chGrp)[iG].elem.find(chLs[iN].c_str())) == (*chGrp)[iG].elem.end()) continue;
		AutoHD<TCntrNode> ndO(p->second);
		if(ndO.at().nodeMode() != Enabled) continue;
		res2.unlock();
		int chRflg = ndO.at().isModify(Self|Child);
		res2.lock();
		if(chRflg) { rflg |= Child; break; }
	    }
	    if(iN < chLs.size()) break;
	}
	res2.unlock();
    }

    return rflg;
}

void TCntrNode::modif( bool save )
{
    dataRes().lock();
    mFlg |= (save?(SelfModifyS|SelfModify):SelfModify);
    dataRes().unlock();
}

void TCntrNode::modifClr( bool save )
{
    dataRes().lock();
    mFlg &= ~(save?SelfModifyS:SelfModify);
    dataRes().unlock();
}

void TCntrNode::modifG( )
{
    modif();

    MtxAlloc res(mChM, true);
    for(unsigned iG = 0; chGrp && iG < chGrp->size(); iG++)
	for(TMap::iterator p = (*chGrp)[iG].elem.begin(); p != (*chGrp)[iG].elem.end(); ++p)
	    p->second->modifG();
}

void TCntrNode::modifGClr( )
{
    modifClr();

    MtxAlloc res(mChM, true);
    for(unsigned iG = 0; chGrp && iG < chGrp->size(); iG++)
	for(TMap::iterator p = (*chGrp)[iG].elem.begin(); p != (*chGrp)[iG].elem.end(); ++p)
	    p->second->modifGClr();
}

void TCntrNode::load( TConfig *cfg, string *errs )
{
    MtxAlloc res(SYS->cfgLoadSaveM(), true);

    bool loadOwn = false;
    //Self load
    if((isModify(Self)&Self))
	try {
	    if(nodeMode() == TCntrNode::Disabled) nodeEn(NodeRestore);
	    modifClr(true);			//Save flag clear
	    load_(cfg);
	    load_();
	    modifClr(nodeFlg()&SelfModifyS);	//Save modify or clear
	    loadOwn = true;
	} catch(TError &err) {
	    if(err.cat.empty())	modifClr();	//Clearing the modification at the loading for not selected DB
	    if(errs && err.cat.size()) (*errs) += nodePath('.')+": "+err.mess+"\n";
	    /*mess_err(err.cat.c_str(), "%s", err.mess.c_str());
	    mess_sys(TMess::Error, _("Error node loading: %s"), err.mess.c_str());*/
	}

    //Childs load process
    if((isModify(Child)&Child)) {
	MtxAlloc res(mChM, true);
	for(unsigned iG = 0; chGrp && iG < chGrp->size(); iG++) {
	    vector<string> chLs;
	    TMap::iterator p;
	    chldList(iG, chLs, true);
	    for(unsigned iN = 0; iG < chGrp->size() && iN < chLs.size(); iN++) {
		if((p=(*chGrp)[iG].elem.find(chLs[iN].c_str())) == (*chGrp)[iG].elem.end()) continue;
		AutoHD<TCntrNode> ndO(p->second);
		if(!ndO.at().isModify(Self|Child))	continue;
		res.unlock();
		ndO.at().load(NULL, errs);
		res.lock();
	    }
	}
    }

    if(loadOwn) load__();
}

void TCntrNode::save( unsigned lev, string *errs )
{
    MtxAlloc res(SYS->cfgLoadSaveM(), true);

    bool isError = false;
    int mdfFlg = isModify(All);

    //Self save
    try {
	if(mdfFlg&Self || (mdfFlg&Child && mFlg&SelfSaveForceOnChild) || SYS->cfgCtx())	save_();
	// Check for prev nodes flag SelfSaveForceOnChild
	if(lev == 0) {
	    TCntrNode *nd = this;
	    while(true) {
		if(!(nd=nd->nodePrev(true))) break;
		if(nd->nodeFlg()&SelfSaveForceOnChild)	nd->save_();
	    }
	}
    } catch(TError &err) {
	if(errs && err.cat.size()) (*errs) += nodePath('.')+": "+err.mess+"\n";
	/*mess_err(err.cat.c_str(), "%s", err.mess.c_str());
	mess_sys(TMess::Error, _("Error node saving: %s"), err.mess.c_str());*/
	isError = true;
    }

    //Childs save process
    if(mdfFlg&Child || SYS->cfgCtx()) {
	MtxAlloc res(mChM, true);
	for(unsigned iG = 0; chGrp && iG < chGrp->size(); iG++) {
	    vector<string> chLs;
	    TMap::iterator p;
	    chldList(iG, chLs, true);
	    for(unsigned iN = 0; iG < chGrp->size() && iN < chLs.size(); iN++) {
		if((p=(*chGrp)[iG].elem.find(chLs[iN].c_str())) == (*chGrp)[iG].elem.end()) continue;
		AutoHD<TCntrNode> ndO(p->second);
		if(!ndO.at().isModify(Self|Child) && !SYS->cfgCtx())	continue;
		res.unlock();
		ndO.at().save(lev+1, errs);
		res.lock();
	    }
	}
    }
    if(!isError && !SYS->cfgCtx()) modifClr();
}

void TCntrNode::AHDConnect( )
{
    dataRes().lock();
    mUse++;
    dataRes().unlock();
    if(mUse > 65000) mess_sys(TMess::Error, _("Very many users at the node!!!"));
}

bool TCntrNode::AHDDisConnect( )
{
    dataRes().lock();
    mUse--;
    dataRes().unlock();

    return false;
}

TVariant TCntrNode::objPropGet( const string &id )			{ return TVariant(); }

void TCntrNode::objPropSet( const string &id, TVariant val )		{ }

TVariant TCntrNode::objFuncCall( const string &iid, vector<TVariant> &prms, const string &user_lang )
{
    // TArrayObj nodeList(string grp = "", string path = "") - child nodes list
    //  grp - nodes group
    //  path - path to source node
    if(iid == "nodeList") {
	try {
	    AutoHD<TCntrNode> nd = (prms.size() >= 2) ? nodeAt(prms[1].getS()) : AutoHD<TCntrNode>(this);
	    TArrayObj *rez = new TArrayObj();
	    vector<string> nls;
	    nd.at().nodeList(nls, (prms.size() >= 1) ? prms[0].getS() : string(""));
	    for(unsigned iN = 0; iN < nls.size(); iN++) rez->arSet(iN, nls[iN]);
	    return rez;
	} catch(TError&)	{ }
	return false;
    }
    // TCntrNodeObj nodeAt(string path, string sep = "") - attach to node
    //  path - path to node
    //  sep - the symbol separator for separated string path
    if(iid == "nodeAt" && prms.size() >= 1) {
	AutoHD<TCntrNode> nd = nodeAt(prms[0].getS(), 0, (prms.size()>=2 && prms[1].getS().size())?prms[1].getS()[0]:0, 0, true);
	return nd.freeStat() ? TVariant(false) : TVariant(new TCntrNodeObj(nd,user_lang));
    }
    // TCntrNodeObj nodePrev() - get previous node
    if(iid == "nodePrev") {
	TCntrNode *tprv = nodePrev(true);
	if(tprv) return new TCntrNodeObj(AutoHD<TCntrNode>(tprv), user_lang);
	return false;
    }
    // string nodePath(string sep = "", bool from_root = true) - get the node path into OpenSCADA objects tree
    //  sep - Separator symbol for separated path;
    //  from_root - path forming from root tree and do not include station ID.
    if(iid == "nodePath")
	return nodePath(((prms.size() && prms[0].getS().size()) ? prms[0].getS()[0] : 0), ((prms.size() >= 2) ? prms[1].getB() : true));
    // int messSys(int level, string mess) - formation of the program message <mess> with the <level>
    //		with the node path as a category and with the human readable path before the message.
    //  level - message level;
    //  mess - message text.
    if(iid == "messSys" && prms.size() >= 2) { mess_sys(prms[0].getI(), "%s", prms[1].getS().c_str()); return 0; }

    throw err_sys(_("Error the function '%s' or missing its parameters."), iid.c_str());
}

XMLNode *TCntrNode::_ctrMkNode( const char *n_nd, XMLNode *nd, int pos, const char *path, const string &dscr,
    int perm, const char *user, const char *grp )
{
    int woff = 0;
    string req = nd->attr("path");
    string reqt, reqt1;

    //Check to display
    bool itbr = false;
    for(int iOff = 0, iOff1 = 0; (reqt=TSYS::pathLev(req,0,true,&iOff)).size(); woff = iOff)
	if(reqt != (reqt1=TSYS::pathLev(path,0,true,&iOff1))) {
	    if(!reqt1.empty()) return NULL;
	    itbr = true;
	    break;
	}

    //Check for permission
    char n_acs = SYS->security().at().access(nd->attr("user"), SEC_RD|SEC_WR|SEC_XT, user, grp, perm);
    if(!(n_acs&SEC_RD)) {
	if(nd->name() == "info") ctrRemoveNode(nd, path);	//To prevent the node's presence with changed here permission to RO.
	return NULL;
    }
    if(itbr)	return nd;

    XMLNode *obj = nd;
    if(obj->name() == "info")	obj = nd->childGet(0, true);
    if(!obj) {
	obj = nd->childAdd();
	nd->setAttr("rez", i2s(TError::NoError));
    }

    //Go to element
    for(int itN = 0; (reqt=TSYS::pathLev(path,0,true,&woff)).size(); reqt1 = reqt, itN++) {
	XMLNode *obj1 = obj->childGet("id", reqt, true);
	if(obj1) { obj = obj1; continue; }
	//int wofft = woff;
	if(TSYS::pathLev(path,0,true,&woff).size())
	    throw TError("ContrItfc", _("Item %d of the path '%s' is missing!"), itN, path);
	if(pos == -1)	obj = obj->childAdd();
	else obj = obj->childIns((pos<0)?pos+1:pos);
    }
    obj->setName(n_nd)->setAttr("id", reqt1)->setAttr("acs", i2s(n_acs | ((perm>>3)&070)));
    if(dscr != EVAL_STR) obj->setAttr("dscr", dscr);

    return obj;
}

XMLNode *TCntrNode::ctrMkNode( const char *n_nd, XMLNode *nd, int pos, const char *path, const string &dscr,
    int perm, const char *user, const char *grp, int n_attr, ... )
{
    XMLNode *obj = _ctrMkNode(n_nd, nd, pos, path, dscr, perm, user, grp);

    //Get addon attributes
    if(obj && n_attr) {
	char *atr_id, *atr_vl;
	va_list argptr;
	va_start(argptr,n_attr);
	for(int i_a = 0; i_a < n_attr; i_a++) {
	    if(!(atr_id=va_arg(argptr,char*)) || !(atr_vl=va_arg(argptr, char*))) break;
	    obj->setAttr(atr_id, atr_vl);
	}
	va_end(argptr);
    }

    return obj;
}

XMLNode *TCntrNode::ctrMkNode2( const char *n_nd, XMLNode *nd, int pos, const char *path, const string &dscr,
    int perm, const char *user, const char *grp, ... )
{
    XMLNode *obj = _ctrMkNode(n_nd, nd, pos, path, dscr, perm, user, grp);

    //Get addon attributes
    if(obj) {
	char *atr_id, *atr_vl;
	va_list argptr;
	va_start(argptr, grp);
	while(true) {
	    if(!(atr_id=va_arg(argptr,char*)) || !(atr_vl=va_arg(argptr,char*))) break;
	    obj->setAttr(atr_id, atr_vl);
	}
	va_end(argptr);
    }

    return obj;
}

bool TCntrNode::ctrRemoveNode( XMLNode *nd, const char *path )
{
    int woff = 0;
    string req = nd->attr("path");
    string reqt, reqt1;

    for(int iOff = 0, iOff1 = 0; (reqt=TSYS::pathLev(req,0,true,&iOff)).size(); woff = iOff)
	if(reqt != (reqt1=TSYS::pathLev(path,0,true,&iOff1)))
	    return false;

    XMLNode *obj = nd;
    if(obj->name() == "info") obj = nd->childGet(0,true);
    if(!obj)	return false;

    //Find element
    while((reqt=TSYS::pathLev(path,0,true,&woff)).size()) {
	XMLNode *obj1 = obj->childGet("id", reqt, true);
	if(!obj1) return false;
	obj = obj1;
    }
    obj->parent()->childDel(obj);

    return true;
}

bool TCntrNode::ctrChkNode( XMLNode *nd, const char *cmd, int perm, const char *user, const char *grp, char mode, const char *warn )
{
    if(nd->name() != cmd) return false;
    if(((char)perm&mode) != mode && SYS->security().at().access(nd->attr("user"),mode,user,grp,perm) != mode)
	throw TError("ContrItfc", _("Error accessing item '%s'!"), nd->attr("path").c_str());
    if(warn && !s2i(nd->attr("force")))
	throw TError(TError::Core_CntrWarning, "ContrItfc", _("Warning element '%s'! %s"), nd->attr("path").c_str(),warn);
    nd->setAttr("rez", i2s(TError::NoError));

    return true;
}

void TCntrNode::cntrCmdProc( XMLNode *opt )
{
    if(opt->name() == "info")
	ctrMkNode("oscada_cntr",opt,-1,"/",TSYS::strMess(_("Node: %s"),nodeName()),R_R_R_,"root","root");

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/obj") {
	// Get node modify flag
	if(ctrChkNode(opt,"modify",R_R_R_))	opt->setText(isModify(TCntrNode::All)?"1":"0");
	// Do load node
	else if(ctrChkNode(opt,"load",RWRWRW,"root","root",SEC_WR)) {
	    string selDB;
	    MtxAlloc res(SYS->cfgLoadSaveM());
	    if((selDB=opt->attr("force")).size()) {
		if(!isdigit(selDB[0]) || s2i(selDB)) modifG();
		if(!isdigit(selDB[0]))	{ res.lock(); SYS->setSelDB(selDB); }
	    }
	    else if(opt->childSize()) modifG();

	    if(opt->childSize())	{ res.lock(); SYS->setCfgCtx(opt); }

	    string errs;
	    load(NULL, &errs);
	    if(selDB.size() && !isdigit(selDB[0]))	SYS->setSelDB("");
	    if(SYS->cfgCtx())	{ SYS->setCfgCtx(NULL); modifG(); }
	    if(errs.size()) throw err_sys(_("Error loading:\n%s"), errs.c_str());
	}
	// Save the node
	else if(ctrChkNode(opt,"save",RWRWRW,"root","root",SEC_WR)) {
	    MtxAlloc res(SYS->cfgLoadSaveM());
	    if(s2i(opt->attr("force"))) modifG();
	    if(s2i(opt->attr("ctx")))	{ res.lock(); SYS->setCfgCtx(opt); }

	    string errs;
	    save(0, &errs);
	    if(SYS->cfgCtx())	SYS->setCfgCtx(NULL);
	    if(errs.size()) throw err_sys(_("Error saving:\n%s"), errs.c_str());
	}
	// Copy the node
	else if(ctrChkNode(opt,"copy",RWRWRW,"root","root",SEC_WR))
	    nodeCopy(opt->attr("src"), opt->attr("dst"), opt->attr("user"));
	// Request node childs parameters
	else if(ctrChkNode(opt,"chlds",R_R_R_,"root","root",SEC_RD)) {
	    string tchGrp = opt->attr("grp");
	    bool icoCheck = s2i(opt->attr("icoCheck"));
	    vector<string> ls;
	    XMLNode req("get"); req.setAttr("path","/br/"+tchGrp)->setAttr("user",opt->attr("user"))->setAttr("lang",opt->attr("lang"));
	    cntrCmdProc(&req);
	    int chGrpId = grpId(tchGrp);
	    if(chGrpId >= 0)
		for(unsigned iCh = 0; iCh < req.childSize(); iCh++) {
		    XMLNode *chN = opt->childAdd();
		    *chN = *req.childGet(iCh);
		    //  Connect to child and get info from it
		    AutoHD<TCntrNode> ch = chldAt(chGrpId,chN->attr("id").empty()?chN->text():chN->attr("id"));
		    //   Check icon
		    XMLNode reqIco("get"); reqIco.setAttr("path","/ico")->setAttr("user",opt->attr("user"))->setAttr("lang",opt->attr("lang"));
		    ch.at().cntrCmdProc(&reqIco);
		    if(icoCheck) chN->setAttr("icoSize", i2s(reqIco.text().size()));
		    else chN->childAdd("ico")->setText(reqIco.text());
		    //   Process groups
		    XMLNode brReq("info"); brReq.setAttr("path","/br")->setAttr("user",opt->attr("user"))->setAttr("lang",opt->attr("lang"));
		    ch.at().cntrCmdProc(&brReq);
		    for(unsigned i_br = 0; brReq.childSize() && i_br < brReq.childGet(0)->childSize(); i_br++) {
			XMLNode *chB = chN->childAdd();
			*chB = *brReq.childGet(0)->childGet(i_br);
			int grpBrId = ch.at().grpId(chB->attr("id"));
			ch.at().chldList(grpBrId, ls);
			chB->setAttr("chPresent",ls.size()?"1":"0");
		    }
		}
	}
    }
    else if((a_path.find("/db/list") == 0 || a_path.find("/db/tblList") == 0) && ctrChkNode(opt)) {
	string tblList = "";
	if(a_path.find("/db/tblList") == 0)
	    if(!(tblList=TSYS::strParse(a_path,1,":")).size())	tblList = _("[TableName]");
	vector<string> c_list;
	TBDS::dbList(c_list);
	if(TSYS::strParse(a_path,1,":").find("onlydb") == string::npos) {
	    if(nodePath() != "/") opt->childAdd("el")->setText(tblList.size() ? ("*.*."+tblList) : "*.*");
	    opt->childAdd("el")->setText(tblList.size() ? (DB_CFG"."+tblList) : DB_CFG);
	}
	for(unsigned iDB = 0; iDB < c_list.size(); iDB++)
	    opt->childAdd("el")->setText(tblList.size() ? c_list[iDB]+"."+tblList : c_list[iDB]);
    }
    else if(a_path == "/plang/list" && ctrChkNode(opt)) {
	opt->childAdd("el")->setText("");
	vector<string>  ls, lls;
	SYS->daq().at().modList(ls);
	for(unsigned iM = 0; iM < ls.size(); iM++) {
	    if(!SYS->daq().at().at(ls[iM]).at().compileFuncLangs(&lls))	continue;
	    for(unsigned iL = 0; iL < lls.size(); iL++)
		opt->childAdd("el")->setText(ls[iM]+"."+lls[iL]);
	}
    }
}
