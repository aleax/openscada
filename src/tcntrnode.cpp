
//OpenSCADA system file: tcntrnode.cpp
/***************************************************************************
 *   Copyright (C) 2003-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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
pthread_mutex_t TCntrNode::connM = PTHREAD_MUTEX_INITIALIZER;

//*************************************************
//* Controll scenaries language section           *
TCntrNode::TCntrNode( TCntrNode *iprev ) : chGrp(NULL), mUse(0), mOi(USHRT_MAX), mFlg(0)
{
    setNodeMode(Disable);
    prev.node = iprev;
    prev.grp = -1;
    modif();
}

TCntrNode::~TCntrNode( )
{
    nodeDelAll();
    if(chGrp) delete chGrp;
}

TCntrNode &TCntrNode::operator=( TCntrNode &node )	{ return *this; }

void TCntrNode::nodeDelAll( )
{
    if(nodeMode() != Disable)	nodeDis();

    TMap::iterator p;
    for(unsigned i_g = 0; chGrp && i_g < chGrp->size(); i_g++)
	while((p = (*chGrp)[i_g].elem.begin()) != (*chGrp)[i_g].elem.end())
	{
	    delete p->second;
	    (*chGrp)[i_g].elem.erase(p);
	}
}

void TCntrNode::setNodeMode( char mode )
{
    pthread_mutex_lock(&connM);
    mFlg = (mFlg&(~0x03))|(mode&0x03);
    pthread_mutex_unlock(&connM);
}

XMLNode *TCntrNode::ctrId( XMLNode *inf, const string &name_id, bool noex )
{
    int l_off = 0;
    string s_el;

    XMLNode *t_node = inf;
    while(true) {
	s_el = TSYS::pathLev(name_id,0,true,&l_off);
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
    throw TError("XML",_("Field id = %s(%s) no present!"),name_id.c_str(),s_el.c_str());
}

void TCntrNode::cntrCmd( XMLNode *opt, int lev, const string &ipath, int off )
{
    string path = ipath.empty() ? opt->attr("path") : ipath;
    string s_br = TSYS::pathLev(path,lev,true,&off);

    try {
	if(!s_br.empty() && s_br[0] != '/') {
	    for(unsigned i_g = 0; chGrp && i_g < chGrp->size(); i_g++)
		if(s_br.compare(0,(*chGrp)[i_g].id.size(),(*chGrp)[i_g].id) == 0) {
		    chldAt(i_g,s_br.substr((*chGrp)[i_g].id.size())).at().cntrCmd(opt,0,path,off);
		    return;
		}
	    //Go to default thread
	    if(chGrp) chldAt(0,s_br).at().cntrCmd(opt,0,path,off);
	    return;
	}
	//Post command to node
	if(opt->name() == "CntrReqs")
	    for(unsigned i_n = 0; i_n < opt->childSize(); i_n++) {
		XMLNode *nChld = opt->childGet(i_n);
		nChld->setAttr("user",opt->attr("user"));
		cntrCmd(nChld);
		nChld->attrDel("user");
	    }
	else {
	    opt->setAttr("path",s_br);
	    cntrCmdProc(opt);
	    if(opt->attr("rez") != "0")
		throw TError("ContrItfc",_("%s:%s:> Control element '%s' error!"),opt->name().c_str(),(nodePath()+path).c_str(),s_br.c_str());
	}
    }
    catch(TError err) {
	if(err.cat == "warning") opt->setAttr("rez","1");
	else opt->setAttr("rez","2");
	opt->childClear();
	opt->setAttr("mcat",err.cat);
	opt->setText(err.mess);
    }
    opt->setAttr("path",path);
}

//*************************************************
//* Resource section                              *
void TCntrNode::nodeEn( int flag )
{
    if(nodeMode() == Enable)	return;		//throw TError(nodePath().c_str(),"Node already enabled!");
    if(nodeMode() != Disable)	throw TError(nodePath().c_str(),_("Node already in process!"));

    setNodeMode(MkEnable);

    preEnable(flag);

    TMap::iterator p;
    for(unsigned i_g = 0; chGrp && i_g < chGrp->size(); i_g++)
	for(p = (*chGrp)[i_g].elem.begin(); p != (*chGrp)[i_g].elem.end(); ++p)
	    if(p->second->nodeMode( ) == Disable)
		p->second->nodeEn( flag );

    setNodeMode(Enable);

    try { postEnable(flag); }
    catch(TError err)	{ mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
};

void TCntrNode::nodeDis( long tm, int flag )
{
    if(nodeMode() == Disable)	return;		//throw TError(nodePath().c_str(),"Node already disabled!");
    if(nodeMode() != Enable)	throw TError(nodePath().c_str(),_("Node already in process!"));

    preDisable(flag);

    setNodeMode(MkDisable);

    try {
	TMap::iterator p;
	for(unsigned i_g = 0; chGrp && i_g < chGrp->size(); i_g++)
	    for(p = (*chGrp)[i_g].elem.begin(); p != (*chGrp)[i_g].elem.end(); ++p)
		if(p->second->nodeMode() == Enable)
		    p->second->nodeDis(tm, flag);

	//Wait of free node
	time_t t_cur = time(NULL);
	while(1) {
	    if(!mUse)	break;
            mess_debug(nodePath().c_str(),_("Waiting for freeing by %d users!"),mUse);
	    // Check timeout
	    if(tm && time(NULL) > t_cur+tm) {
		if(!TSYS::finalKill)
		    throw TError(nodePath().c_str(),_("Timeouted of wait. Object is used by %d users. Free object first!"),mUse);
		mess_err(nodePath().c_str(),_("Blocking node error. Inform developers please!"));
		break;
	    }
	    TSYS::sysSleep(STD_WAIT_DELAY*1e-3);
	}

	setNodeMode(Disable);
	modif();

	postDisable(flag);
    }
    catch(TError err) {
	mess_warning(err.cat.c_str(),_("Node disable error. Restore node enabling."));
	setNodeMode(Disable);
	nodeEn(NodeRestore|(flag<<8));
	throw;
    }
    //try{ postDisable(flag); }
    //catch(TError err)	{ mess_warning(err.cat.c_str(),err.mess.c_str()); }
};

void TCntrNode::nodeList( vector<string> &list, const string &gid )
{
    vector<string> tls;
    list.clear();
    for(unsigned i_gr = 0; chGrp && i_gr < chGrp->size(); i_gr++)
	if(gid.empty() || gid == (*chGrp)[i_gr].id) {
	    chldList(i_gr, tls);
	    for(unsigned i_l = 0; i_l < tls.size(); i_l++)
		list.push_back((*chGrp)[i_gr].id+tls[i_l]);
	    if(!gid.empty())	break;
	}
}

AutoHD<TCntrNode> TCntrNode::nodeAt( const string &path, int lev, char sep, int off, bool noex )
{
    try {
	string s_br = sep ? TSYS::strDecode(TSYS::strSepParse(path,lev,sep,&off),TSYS::PathEl) :
			    TSYS::pathLev(path,lev,true,&off);
	if(s_br.empty()) {
	    if(nodeMode() == Disable) throw TError(nodePath().c_str(),_("Node is disabled!"));
	    return this;
	}
	ResAlloc res(hd_res,false);
	for(unsigned i_g = 0; chGrp && i_g < chGrp->size(); i_g++)
	    if(s_br.compare(0,(*chGrp)[i_g].id.size(),(*chGrp)[i_g].id) == 0)
		return chldAt(i_g,s_br.substr((*chGrp)[i_g].id.size())).at().nodeAt(path,0,sep,off,noex);
	//Go to default group
	if(chGrp) return chldAt(0,s_br).at().nodeAt(path,0,sep,off,noex);
	throw TError(nodePath().c_str(),_("Node '%s' no present!"),s_br.c_str());
    }
    catch(TError err) { if(!noex) throw; }

    return NULL;
}

void TCntrNode::nodeDel( const string &path, char sep, int flag, bool shDel )
{
    AutoHD<TCntrNode> del_n = nodeAt(path, 0, sep);
    int n_grp = del_n.at().prev.grp;
    string n_id  = del_n.at().nodeName();
    del_n = AutoHD<TCntrNode>(del_n.at().prev.node);
    del_n.at().chldDel(n_grp, n_id, -1, flag, shDel);
}

void TCntrNode::nodeCopy( const string &src, const string &dst, const string &user )
{
    if(src == dst) return;

    //Attach to source node
    AutoHD<TCntrNode> src_n = SYS->nodeAt(src);

    //Parse destination node path
    string d_elp, d_el, t_el;
    int n_del = 0;
    for(int off = 0; !(t_el=TSYS::pathLev(dst,0,true,&off)).empty(); n_del++)
    { if(n_del) d_elp += ("/"+d_el); d_el = t_el; }
    if(!n_del) throw TError(SYS->nodePath().c_str(),_("Copy from '%s' to '%s' is impossible"),src.c_str(),dst.c_str());

    //Connect to destination containers node
    AutoHD<TCntrNode> dst_n = SYS->nodeAt(d_elp);

    //Get allow branches' containers and find want group
    XMLNode br_req("info");
    br_req.setAttr("user",user)->setAttr("path","/%2fbr");
    dst_n.at().cntrCmd(&br_req);
    if(s2i(br_req.attr("rez")) || !br_req.childGet(0,true))
	throw TError(SYS->nodePath().c_str(),_("Destination node doesn't have branches."));
    XMLNode *branch = br_req.childGet(0);
    unsigned i_b;
    for(i_b = 0; i_b < branch->childSize(); i_b++)
	if(branch->childGet(i_b)->attr("id") == d_el.substr(0,branch->childGet(i_b)->attr("id").size()) &&
		s2i(branch->childGet(i_b)->attr("acs"))&SEC_WR)
	    break;
    if(i_b >= branch->childSize())
        throw TError(SYS->nodePath().c_str(),_("Destination node doesn't have necessary branche."));
    bool idm = s2i(branch->childGet(i_b)->attr("idm"));
    string n_grp = branch->childGet(i_b)->attr("id");
    d_el = d_el.substr(n_grp.size());
    i_b = dst_n.at().grpId(n_grp);
    if(i_b < 0) throw TError(SYS->nodePath().c_str(),_("Destination node doesn't have necessary branche."));

    //Connect or create new destination node
    if(!dst_n.at().chldPresent(i_b,d_el)) {
	br_req.clear()->setName("add")->setAttr("user",user)->setAttr("path","/%2fbr%2f"+n_grp);
	if(idm) br_req.setAttr("id", d_el);
	else br_req.setText(d_el);
	dst_n.at().cntrCmd(&br_req);
	if(s2i(br_req.attr("rez")))	throw TError(br_req.attr("mcat").c_str(),br_req.text().c_str());
    }

    //Same copy call
    dst_n.at().chldAt(i_b, d_el).at() = src_n.at();
}

unsigned TCntrNode::grpAdd( const string &iid, bool iordered )
{
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

int8_t TCntrNode::grpSize( )	{ return chGrp ? chGrp->size() : 0; }

int8_t TCntrNode::grpId( const string &sid )
{
    for(int g_id = 0; chGrp && g_id < (int)chGrp->size(); g_id++)
	if((*chGrp)[g_id].id == sid) return g_id;

    return -1;
}

TCntrNode::GrpEl &TCntrNode::grpAt( int8_t iid )
{
    if(iid < 0 || iid >= grpSize())	throw TError(nodePath().c_str(),_("Branch group '%d' error."),iid);
    return (*chGrp)[iid];
}

void TCntrNode::chldList( int8_t igr, vector<string> &list, bool noex )
{
    list.clear();

    ResAlloc res(hd_res, false);
    if(!chGrp || igr < 0 || igr >= (int)chGrp->size()) {
	if(noex) return;
	else throw TError(nodePath().c_str(),_("Group of childs %d error!"),igr);
    }
    if(nodeMode() == Disable) { if(noex) return; else throw TError(nodePath().c_str(),"Node is disabled!"); }

    list.reserve((*chGrp)[igr].elem.size());
    if(!(*chGrp)[igr].ordered) {
	for(TMap::iterator p = (*chGrp)[igr].elem.begin(); p != (*chGrp)[igr].elem.end(); ++p)
	    if(p->second->nodeMode() != Disable)
		list.push_back(p->first);
    }
    else {
	bool disYes = false;
	for(TMap::iterator p = (*chGrp)[igr].elem.begin(); p != (*chGrp)[igr].elem.end(); ++p)
	    if(p->second->nodeMode() == Disable) disYes = true;
	    else {
		while(p->second->mOi >= list.size()) list.push_back("");
		list[p->second->mOi] = p->first;
	    }
	for(unsigned i_p = 0; disYes && i_p < list.size(); )
	    if(list[i_p].empty()) list.erase(list.begin()+i_p);
	    else i_p++;
    }
}

bool TCntrNode::chldPresent( int8_t igr, const string &name )
{
    ResAlloc res(hd_res,false);
    if(!chGrp || igr >= (int)chGrp->size()) throw TError(nodePath().c_str(),_("Group of childs %d error!"),igr);
    if(nodeMode() == Disable)	throw TError(nodePath().c_str(),"Node is disabled!");

    TMap::iterator p = (*chGrp)[igr].elem.find(name.c_str());
    if(p != (*chGrp)[igr].elem.end() && p->second->nodeMode() == Enable) return true;

    return false;
}

void TCntrNode::chldAdd( int8_t igr, TCntrNode *node, int pos, bool noExp )
{
    ResAlloc res(hd_res,false);
    if(!chGrp || igr >= (int)chGrp->size())
    { delete node; throw TError(nodePath().c_str(),_("Group of childs %d error!"),igr); }
    if(nodeMode() != Enable)
    { delete node; throw TError(nodePath().c_str(),_("Node is not enabled!")); }

    TMap::iterator p;
    if(TSYS::strNoSpace(node->nodeName()).empty())
    { delete node; throw TError(nodePath().c_str(),_("Add child id is empty!")); }

    if((p=(*chGrp)[igr].elem.find(node->nodeName())) != (*chGrp)[igr].elem.end())
    {
	delete node;
	if(p->second->nodeMode() == Disable)	p->second->nodeEn(TCntrNode::NodeRestore);
	if(!noExp) throw TError(nodePath().c_str(),_("Node '%s' is already present."),p->first);
	return;
    }

    res.request(true);
    node->prev.node = this;
    node->prev.grp = igr;
    if((*chGrp)[igr].ordered) {
	pos = (pos < 0 || pos > (int)(*chGrp)[igr].elem.size()) ? (int)(*chGrp)[igr].elem.size() : pos;
	node->mOi = pos;
	for(p = (*chGrp)[igr].elem.begin(); p != (*chGrp)[igr].elem.end(); p++)
	    if(p->second->mOi >= pos) p->second->mOi++;
    }
    (*chGrp)[igr].elem.insert(std::pair<const char *,TCntrNode*>(node->nodeName(),node));
    res.release();

    if(node->nodeMode() == Disable)	node->nodeEn(TCntrNode::NodeConnect);
}

void TCntrNode::chldDel( int8_t igr, const string &name, long tm, int flag, bool shDel )
{
    if(tm < 0)	tm = DEF_TIMEOUT;
    ResAlloc res(hd_res, false);
    if(!chGrp || igr >= (int)chGrp->size()) throw TError(nodePath().c_str(), _("Group of childs %d error!"), igr);
    if(!(nodeMode() == Enable || nodeMode() == Disable))
	throw TError(nodePath().c_str(),_("Node is begin processed now!"));

    TMap::iterator p = (*chGrp)[igr].elem.find(name.c_str());
    if(p == (*chGrp)[igr].elem.end())
	throw TError(nodePath().c_str(),_("Child '%s' is not present!"), name.c_str());

    if(p->second->nodeMode() == Enable) p->second->nodeDis(tm, (flag<<8)|(shDel?NodeShiftDel:0));

    if(!shDel) {
	res.request(true);
	p = (*chGrp)[igr].elem.find(name.c_str());
	if(p == (*chGrp)[igr].elem.end()) return;
	if((*chGrp)[igr].ordered) {
	    int pos = p->second->mOi;
	    for(TMap::iterator p1 = (*chGrp)[igr].elem.begin(); p1 != (*chGrp)[igr].elem.end(); ++p1)
		if(p1->second->mOi > pos) p1->second->mOi--;
	}
	delete p->second;
	(*chGrp)[igr].elem.erase(p);
    }
}

void TCntrNode::setNodeFlg( char flg )
{
    pthread_mutex_lock(&connM);
    mFlg |= flg&(SelfModify|SelfModifyS|SelfSaveForceOnChild);
    pthread_mutex_unlock(&connM);
}

unsigned TCntrNode::nodeUse( bool selfOnly )
{
    ResAlloc res(hd_res,false);
    //if(nodeMode() == Disable)	throw TError(nodePath().c_str(),"Node is disabled!");

    unsigned i_use = mUse;
    TMap::iterator p;
    for(unsigned i_g = 0; !selfOnly && chGrp && i_g < chGrp->size(); i_g++)
	for(p = (*chGrp)[i_g].elem.begin(); p != (*chGrp)[i_g].elem.end(); ++p)
	    if(p->second->nodeMode() != Disable)
		i_use += p->second->nodeUse();

    return i_use;
}

string TCntrNode::nodePath( char sep, bool from_root )
{
    if(sep) {
	if(prev.node) {
	    if(from_root && !prev.node->prev.node)
		return ((prev.grp<0)?"":(*(prev.node->chGrp))[prev.grp].id)+nodeName();
	    else
		return prev.node->nodePath(sep,from_root)+string(1,sep)+
			((prev.grp<0)?"":(*(prev.node->chGrp))[prev.grp].id)+nodeName();
	}
	else return nodeName();
    }
    else return prev.node ? (prev.node->nodePath(sep,from_root)+((prev.grp<0)?"":(*(prev.node->chGrp))[prev.grp].id)+nodeName()+"/") :
			    (from_root?string("/"):(string("/")+nodeName()+"/"));
}

TCntrNode *TCntrNode::nodePrev( bool noex )
{
    if(prev.node) return prev.node;
    if(noex)	return NULL;
    throw TError(nodePath().c_str(), _("Node is the root or is not connected!"));
}

AutoHD<TCntrNode> TCntrNode::chldAt( int8_t igr, const string &name, const string &user )
{
    ResAlloc res(hd_res, false);
    if(!chGrp || igr >= (int)chGrp->size()) throw TError(nodePath().c_str(),_("Group of childs %d error!"),igr);
    if(nodeMode() == Disable)	throw TError(nodePath().c_str(),"Node is disabled!");

    TMap::iterator p = (*chGrp)[igr].elem.find(name.c_str());
    if(p == (*chGrp)[igr].elem.end() || p->second->nodeMode() == Disable)
	throw TError(nodePath().c_str(),_("Element '%s' is not present or disabled!"), name.c_str());

    return AutoHD<TCntrNode>(p->second,user);
}

int TCntrNode::isModify( int f )
{
    ResAlloc res(hd_res, false);
    int rflg = 0;

    if(f&Self && mFlg&SelfModify) rflg |= Self;
    if(f&Child)
	for(unsigned i_g = 0; chGrp && i_g < chGrp->size(); i_g++)
	{
	    TMap::iterator p;
	    for(p = (*chGrp)[i_g].elem.begin(); p != (*chGrp)[i_g].elem.end(); ++p)
		if(p->second->isModify(Self|Child))	{ rflg |= Child; break; }
	    if(p != (*chGrp)[i_g].elem.end())	break;
	}

    return rflg;
}

void TCntrNode::modif( bool save )
{
    pthread_mutex_lock(&connM);
    mFlg |= (save?(SelfModifyS|SelfModify):SelfModify);
    pthread_mutex_unlock(&connM);
}

void TCntrNode::modifClr( bool save )
{
    pthread_mutex_lock(&connM);
    mFlg &= ~(save?SelfModifyS:SelfModify);
    pthread_mutex_unlock(&connM);
}

void TCntrNode::modifG( )
{
    ResAlloc res(hd_res, false);
    modif();
    for(unsigned i_g = 0; chGrp && i_g < chGrp->size(); i_g++)
	for(TMap::iterator p = (*chGrp)[i_g].elem.begin(); p != (*chGrp)[i_g].elem.end(); ++p)
	    p->second->modifG();
}

void TCntrNode::modifGClr( )
{
    ResAlloc res(hd_res, false);
    modifClr();
    for(unsigned i_g = 0; chGrp && i_g < chGrp->size(); i_g++)
	for(TMap::iterator p = (*chGrp)[i_g].elem.begin(); p != (*chGrp)[i_g].elem.end(); ++p)
	    p->second->modifGClr();
}

void TCntrNode::load( bool force, string *errs )
{
    //Self load
    if((isModify(Self)&Self) || force)
	try {
	    if(nodeMode() == TCntrNode::Disable) nodeEn(NodeRestore|NodeShiftDel);
	    modifClr(true);			//Save flag clear
	    load_();
	    modifClr(nodeFlg()&SelfModifyS);	//Save modify or clear
	}
	catch(TError err) {
	    if(errs && err.cat.size()) (*errs) += nodePath('.')+": "+err.mess+"\n";
	    /*mess_err(err.cat.c_str(), "%s", err.mess.c_str());
	    mess_err(nodePath().c_str(), _("Load node error: %s"), err.mess.c_str());*/
	}

    //Childs load process
    if((isModify(Child)&Child) || force) {
	ResAlloc res(hd_res, false);
	for(unsigned i_g = 0; chGrp && i_g < chGrp->size(); i_g++)
	    for(TMap::iterator p = (*chGrp)[i_g].elem.begin(); p != (*chGrp)[i_g].elem.end(); ++p)
		if(p->second->isModify(Self|Child) || force) p->second->load(force, errs);
    }
}

void TCntrNode::save( unsigned lev, string *errs )
{
    bool isError = false;
    int mdfFlg = isModify(All);

    //Self save
    try {
	if(mdfFlg&Self || (mdfFlg&Child && mFlg&SelfSaveForceOnChild))	save_();
	// Check for prev nodes flag SelfSaveForceOnChild
	if(lev == 0) {
	    TCntrNode *nd = this;
	    while(true) {
		if(!(nd=nd->nodePrev(true))) break;
		if(nd->nodeFlg()&SelfSaveForceOnChild)	nd->save_();
	    }
	}
    }
    catch(TError err) {
	if(errs && err.cat.size()) (*errs) += nodePath('.')+": "+err.mess+"\n";
	/*mess_err(err.cat.c_str(), "%s", err.mess.c_str());
	mess_err(nodePath().c_str(), _("Save node error: %s"), err.mess.c_str());*/
	isError = true;
    }

    //Childs save process
    if(mdfFlg&Child) {
	ResAlloc res(hd_res, false);
	for(unsigned i_g = 0; chGrp && i_g < chGrp->size(); i_g++)
	    for(TMap::iterator p = (*chGrp)[i_g].elem.begin(); p != (*chGrp)[i_g].elem.end(); ++p)
		if(p->second->isModify(Self|Child)) p->second->save(lev+1, errs);
    }
    if(!isError) modifClr();
}

void TCntrNode::AHDConnect()
{
    pthread_mutex_lock(&connM);
    mUse++;
    pthread_mutex_unlock(&connM);
    if(mUse > 65000) mess_err(nodePath().c_str(),_("Too more users for node!!!"));
}

bool TCntrNode::AHDDisConnect()
{
    pthread_mutex_lock(&connM);
    mUse--;
    pthread_mutex_unlock(&connM);
    return false;
}

TVariant TCntrNode::objPropGet( const string &id )			{ return TVariant(); }

void TCntrNode::objPropSet( const string &id, TVariant val )		{ }

TVariant TCntrNode::objFuncCall( const string &iid, vector<TVariant> &prms, const string &user )
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
	    for(unsigned i_l = 0; i_l < nls.size(); i_l++) rez->propSet(i2s(i_l), nls[i_l]);
	    return rez;
	}
	catch(TError)	{ }
	return false;
    }
    // TCntrNodeObj nodeAt(string path, string sep = "") - attach to node
    //  path - path to node
    //  sep - the symbol separator for separated string path
    if(iid == "nodeAt" && prms.size() >= 1) {
	AutoHD<TCntrNode> nd = nodeAt(prms[0].getS(), 0, (prms.size()>=2 && prms[1].getS().size())?prms[1].getS()[0]:0, 0, true);
	return nd.freeStat() ? TVariant(false) : TVariant(new TCntrNodeObj(nd,user));
    }
    // TCntrNodeObj nodePrev() - get previous node
    if(iid == "nodePrev") {
	TCntrNode *prev = nodePrev(true);
	if(prev) return new TCntrNodeObj(AutoHD<TCntrNode>(prev), user);
	return false;
    }
    // string nodePath(string sep = "", bool from_root = true) - get the node path into OpenSCADA objects tree
    //  sep - Separator symbol for separated path
    //  from_root - path forming from root tree and do not include station ID.
    if(iid == "nodePath")
	return nodePath(((prms.size() && prms[0].getS().size()) ? prms[0].getS()[0] : 0), ((prms.size() >= 2) ? prms[1].getB() : true));

    throw TError(nodePath().c_str(),_("Function '%s' error or not enough parameters."),iid.c_str());
}

XMLNode *TCntrNode::_ctrMkNode( const char *n_nd, XMLNode *nd, int pos, const char *path, const string &dscr,
    int perm, const char *user, const char *grp )
{
    int woff = 0;
    string req = nd->attr("path");
    string reqt, reqt1;

    //Check displaing node
    int itbr = 0;
    for(int i_off = 0, i_off1 = 0; (reqt=TSYS::pathLev(req,0,true,&i_off)).size(); woff = i_off)
	if(reqt != (reqt1=TSYS::pathLev(path,0,true,&i_off1))) {
	    if(!reqt1.empty()) return NULL;
	    itbr = 1;
	    break;
	}

    //Check permission
    char n_acs = SYS->security().at().access(nd->attr("user"), SEC_RD|SEC_WR|SEC_XT, user, grp, perm);
    if(!(n_acs&SEC_RD)) return NULL;
    if(itbr)	return nd;

    XMLNode *obj = nd;
    if(obj->name() == "info")	obj = nd->childGet(0, true);
    if(!obj) {
	obj = nd->childAdd();
	nd->setAttr("rez", "0");
    }

    //Go to element
    for( ;(reqt=TSYS::pathLev(path,0,true,&woff)).size(); reqt1 = reqt) {
	XMLNode *obj1 = obj->childGet("id", reqt, true);
	if(obj1) { obj = obj1; continue; }
	//int wofft = woff;
	if(TSYS::pathLev(path,0,true,&woff).size())
	    throw TError("ContrItfc", _("Some tags on path '%s' are missed!"), req.c_str());
	if(pos == -1)	obj = obj->childAdd();
	else obj = obj->childIns((pos<0)?pos+1:pos);
    }
    obj->setName(n_nd)->setAttr("id", reqt1)->setAttr("acs", i2s(n_acs));
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

    for(int i_off = 0, i_off1 = 0; (reqt=TSYS::pathLev(req,0,true,&i_off)).size(); woff=i_off)
	if(reqt != (reqt1=TSYS::pathLev(path,0,true,&i_off1)))
	    return false;

    XMLNode *obj = nd;
    if(obj->name() == "info") obj = nd->childGet(0,true);
    if(!obj)	return false;

    //Find element
    while((reqt=TSYS::pathLev(path,0,true,&woff)).size()) {
	XMLNode *obj1 = obj->childGet("id",reqt,true);
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
	throw TError("ContrItfc",_("Error access to element '%s'!"),nd->attr("path").c_str());
    if(warn && !s2i(nd->attr("force")))
	throw TError("warning",_("Element '%s' warning! %s"),nd->attr("path").c_str(),warn);
    nd->setAttr("rez","0");

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
	    string errs;
	    load(s2i(opt->attr("force")), &errs);
	    if(errs.size()) throw TError(nodePath().c_str(), _("Load error:\n%s"), errs.c_str());
	}
	// Do save node
	else if(ctrChkNode(opt,"save",RWRWRW,"root","root",SEC_WR)) {
	    if(s2i(opt->attr("force"))) modifG();

	    string errs;
	    save(0, &errs);
	    if(errs.size()) throw TError(nodePath().c_str(), _("Save error:\n%s"), errs.c_str());
	}
	// Do copy node
	else if(ctrChkNode(opt,"copy",RWRWRW,"root","root",SEC_WR))
	    nodeCopy(opt->attr("src"),opt->attr("dst"),opt->attr("user"));
	// Request node childs parameters
	else if(ctrChkNode(opt,"chlds",R_R_R_,"root","root",SEC_RD)) {
	    string chGrp = opt->attr("grp");
	    bool icoCheck = s2i(opt->attr("icoCheck"));
	    vector<string> ls;
	    XMLNode req("get"); req.setAttr("path","/br/"+chGrp)->setAttr("user",opt->attr("user"));
	    cntrCmdProc(&req);
	    int chGrpId = grpId(chGrp);
	    if(chGrpId >= 0)
		for(unsigned i_ch = 0; i_ch < req.childSize(); i_ch++) {
		    XMLNode *chN = opt->childAdd();
		    *chN = *req.childGet(i_ch);
		    //  Connect to child and get info from it
		    AutoHD<TCntrNode> ch = chldAt(chGrpId,chN->attr("id").empty()?chN->text():chN->attr("id"));
		    //   Check icon
		    XMLNode reqIco("get"); reqIco.setAttr("path","/ico")->setAttr("user",opt->attr("user"));
		    ch.at().cntrCmdProc(&reqIco);
		    if(icoCheck) chN->setAttr("icoSize", i2s(reqIco.text().size()));
		    else chN->childAdd("ico")->setText(reqIco.text());
		    //   Process groups
		    XMLNode brReq("info"); brReq.setAttr("path","/br")->setAttr("user",opt->attr("user"));
		    ch.at().cntrCmdProc(&brReq);
		    for(unsigned i_br = 0; brReq.childSize() && i_br < brReq.childGet(0)->childSize(); i_br++) {
			XMLNode *chB = chN->childAdd();
			*chB = *brReq.childGet(0)->childGet(i_br);
			int grpBrId = ch.at().grpId(chB->attr("id"));
			ch.at().chldList(grpBrId,ls);
			chB->setAttr("chPresent",ls.size()?"1":"0");
		    }
		}
	}
    }
    else if((a_path == "/db/list" || a_path.compare(0,11,"/db/tblList") == 0) && ctrChkNode(opt)) {
	string tblList = "";
	if(a_path.compare(0,11,"/db/tblList") == 0)
	    if(!(tblList=TSYS::strParse(a_path,1,":")).size())	tblList = _("[TableName]");
	vector<string> c_list;
	SYS->db().at().dbList(c_list);
	opt->childAdd("el")->setText(tblList.size() ? ("*.*."+tblList) : "*.*");
	opt->childAdd("el")->setText(tblList.size() ? (DB_CFG"."+tblList) : DB_CFG);
	for(unsigned i_db = 0; i_db < c_list.size(); i_db++)
	    opt->childAdd("el")->setText(tblList.size() ? c_list[i_db]+"."+tblList : c_list[i_db]);
    }
}
