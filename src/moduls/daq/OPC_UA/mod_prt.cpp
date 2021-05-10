
//OpenSCADA module DAQ.OPC_UA file: mod_prt.cpp
/***************************************************************************
 *   Copyright (C) 2009-2021 by Roman Savochenko, <roman@oscada.org>       *
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

#include <signal.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <tsys.h>
#include <tmess.h>
#include <tmodule.h>
#include <tuis.h>

#include "mod_prt.h"

OPC_UA::TProt *OPC_UA::modPrt;

using namespace OPC_UA;

//*************************************************
//* TProt                                         *
//*************************************************
TProt::TProt( string name ) : TProtocol(PRT_ID)
{
    modPrt	= this;

    modInfoMainSet(PRT_NAME, PRT_TYPE, PRT_MVER, PRT_AUTOR, PRT_DESCR, PRT_LICENSE, name);

    mEndPnt = grpAdd("ep_");

    //Node DB structure
    mEndPntEl.fldAdd(new TFld("ID",_("Identifier"),TFld::String,TCfg::Key|TFld::NoWrite,i2s(limObjID_SZ).c_str()));
    mEndPntEl.fldAdd(new TFld("NAME",_("Name"),TFld::String,TFld::TransltText,i2s(limObjNm_SZ).c_str()));
    mEndPntEl.fldAdd(new TFld("DESCR",_("Description"),TFld::String,TFld::FullText|TFld::TransltText,"300"));
    mEndPntEl.fldAdd(new TFld("EN",_("To enable"),TFld::Boolean,0,"1","0"));
    mEndPntEl.fldAdd(new TFld("SerialzType",_("Serializer type"),TFld::Integer,TFld::Selectable,"1","0","0",_("Binary")));
    mEndPntEl.fldAdd(new TFld("URL",_("URL"),TFld::String,0,"50","opc.tcp://localhost:4841"));
    mEndPntEl.fldAdd(new TFld("SecPolicies",_("Security policies"),TFld::String,TFld::FullText,"100","None:0\nBasic128Rsa15:1"));
    mEndPntEl.fldAdd(new TFld("ServCert",_("Server certificate (PEM)"),TFld::String,TFld::FullText,"10000"));
    mEndPntEl.fldAdd(new TFld("ServPvKey",_("Server private key (PEM)"),TFld::String,TFld::FullText,"10000"));
    mEndPntEl.fldAdd(new TFld("A_PRMS",_("Addition parameters"),TFld::String,TFld::FullText,"10000"));
}

TProt::~TProt( )			{ nodeDelAll(); }

string TProt::applicationUri( )		{ return "urn:"+SYS->host()+":OpenSCADA:DAQ.OPC_UA"; }

string TProt::productUri( )		{ return "urn:OpenSCADA:DAQ.OPC_UA";/*PACKAGE_SITE;*/ }

string TProt::applicationName( )	{ return "OpenSCADA.OPC-UA Server"; }

uint32_t TProt::clientRcvBufSz( const string &inPrtId )	{ return at(inPrtId).at().mRcvBufSz; }

uint32_t TProt::clientSndBufSz( const string &inPrtId )	{ return at(inPrtId).at().mSndBufSz; }

uint32_t TProt::clientMsgMaxSz( const string &inPrtId )	{ return at(inPrtId).at().mMsgMaxSz; }

uint32_t TProt::clientChunkMaxCnt( const string &inPrtId ) { return at(inPrtId).at().mChunkMaxCnt; }

void TProt::clientRcvBufSzSet( const string &inPrtId, uint32_t vl )	{ at(inPrtId).at().mRcvBufSz = vl; }

void TProt::clientSndBufSzSet( const string &inPrtId, uint32_t vl )	{ at(inPrtId).at().mSndBufSz = vl; }

void TProt::clientMsgMaxSzSet( const string &inPrtId, uint32_t vl )	{ at(inPrtId).at().mMsgMaxSz = vl; }

void TProt::clientChunkMaxCntSet( const string &inPrtId, uint32_t vl )	{ at(inPrtId).at().mChunkMaxCnt = vl; }

string TProt::epAdd( const string &iid, const string &db )
{
    return chldAdd(mEndPnt, new OPCEndPoint(TSYS::strEncode(sTrm(iid),TSYS::oscdID),db,&endPntEl()));
}

bool TProt::debug( )			{ return (mess_lev()==TMess::Debug); }

void TProt::debugMess( const string &mess )
{
    mess_debug(nodePath().c_str(), "%s", mess.c_str());
}

void TProt::epEnList( vector<string> &ls )
{
    ls.clear();
    for(unsigned i_ep = 0; i_ep < ep_hd.size(); i_ep++)
	ls.push_back(ep_hd[i_ep].at().id());
}

Server::EP *TProt::epEnAt( const string &ep )
{
    for(unsigned i_ep = 0; i_ep < ep_hd.size(); i_ep++)
	if(ep_hd[i_ep].at().id() == ep)
	    return &ep_hd[i_ep].at();

    return NULL;
}

bool TProt::inReq( string &request, const string &inPrtId, string *answ )
{
    ResAlloc res(enRes, false);
    bool rez = Server::inReq(request, inPrtId, answ);
    res.unlock();

#ifdef POLL_OF_TR
    //Poll for subscriptions process
    AutoHD<TProtIn> ip = at(inPrtId);
    if(ip.at().waitReqTm() && !ip.at().mSubscrIn && epPresent(ip.at().mEp)) {
	int64_t wTm = SYS->curTime();
	AutoHD<OPCEndPoint> ep = epAt(ip.at().mEp);
	bool tmToCall = (wTm-ip.at().mPrevTm)/1000 >= ip.at().waitReqTm();
	if(tmToCall || ep.at().forceSubscrQueue) {
	    if(tmToCall) ep.at().forceSubscrQueue = false;
	    ip.at().mPrevTm = wTm;
	    ip.at().mSubscrIn = true;
	    epAt(ip.at().mEp).at().subScrCycle(answ, inPrtId);
	    ip.at().mSubscrIn = false;
	}
    }
#endif

    return rez;
}

int TProt::writeToClient( const string &inPrtId, const string &data )	{ return at(inPrtId).at().writeTo(data); }

string TProt::clientAddr( const string &inPrtId )	{ return TSYS::strLine(at(inPrtId).at().srcAddr(), 0); }

void TProt::discoveryUrls( vector<string> &ls )
{
    ls.clear();
    //Get allowed enpoints list
    vector<string> epLs;
    epList(epLs);
    for(unsigned i_ep = 0; i_ep < epLs.size(); i_ep++) {
	AutoHD<OPCEndPoint> ep = epAt(epLs[i_ep]);
	if(!ep.at().enableStat()) continue;
	ls.push_back(ep.at().url());
	break;
    }
}

void TProt::epEn( const string &id, bool val )
{
    ResAlloc res(enRes, true);

    unsigned i_ep;
    for(i_ep = 0; i_ep < ep_hd.size(); i_ep++)
	if(ep_hd[i_ep].at().id() == id) break;

    if(val && i_ep >= ep_hd.size()) ep_hd.push_back(epAt(id));
    if(!val && i_ep < ep_hd.size()) ep_hd.erase(ep_hd.begin()+i_ep);
}

void TProt::load_( )
{
    //Load DB
    try {
	TConfig gCfg(&endPntEl());
	//gCfg.cfgViewAll(false);
	vector<string> itLs;
	map<string, bool> itReg;

	// Search into DB
	SYS->db().at().dbList(itLs, true);
	itLs.push_back(DB_CFG);
	for(unsigned iDb = 0; iDb < itLs.size(); iDb++)
	    for(int fldCnt = 0; SYS->db().at().dataSeek(itLs[iDb]+"."+modId()+"_ep",nodePath()+modId()+"_ep",fldCnt++,gCfg,false,true); ) {
		string id = gCfg.cfg("ID").getS();
		if(!epPresent(id)) epAdd(id,(itLs[iDb]==SYS->workDB())?"*.*":itLs[iDb]);
		epAt(id).at().load(&gCfg);
		itReg[id] = true;
	    }

	// Check for remove items removed from DB
	if(SYS->chkSelDB(SYS->selDB(),true)) {
	    epList(itLs);
	    for(unsigned iIt = 0; iIt < itLs.size(); iIt++)
		if(itReg.find(itLs[iIt]) == itReg.end() && SYS->chkSelDB(epAt(itLs[iIt]).at().DB()))
		    epDel(itLs[iIt]);
	}
    } catch(TError &err) {
	mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	mess_err(nodePath().c_str(),_("Search and create new server error."));
    }
}

void TProt::save_( )	{ }

void TProt::modStart( )
{
    vector<string> ls;
    epList(ls);
    for(unsigned iN = 0; iN < ls.size(); iN++)
	if(epAt(ls[iN]).at().toEnable())
	    epAt(ls[iN]).at().setEnable(true);
}

void TProt::modStop( )
{
    vector<string> ls;
    epList(ls);
    for(unsigned iN = 0; iN < ls.size(); iN++)
	epAt(ls[iN]).at().setEnable(false);
}

TProtocolIn *TProt::in_open( const string &name )	{ return new TProtIn(name); }

void TProt::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TProtocol::cntrCmdProc(opt);
	ctrMkNode("grp", opt, -1, "/br/ep_", _("End point"), RWRWR_, "root", SPRT_ID, 2, "idm",i2s(limObjNm_SZ).c_str(), "idSz",i2s(limObjID_SZ).c_str());
	if(ctrMkNode("area",opt,0,"/serv",_("Server")))
	    ctrMkNode("list",opt,-1,"/serv/asc",_("Active secure channels"),R_R_R_,"root",SPRT_ID);
	if(ctrMkNode("area",opt,1,"/ep",_("End points")))
	    ctrMkNode("list", opt, -1, "/ep/ep", _("End points"), RWRWR_, "root", SPRT_ID, 5,
		"tp","br", "idm",i2s(limObjNm_SZ).c_str(), "s_com","add,del", "br_pref","ep_", "idSz",i2s(limObjID_SZ).c_str());
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/br/ep_" || a_path == "/ep/ep") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD)) {
	    vector<string> lst;
	    epList(lst);
	    for(unsigned iF = 0; iF < lst.size(); iF++)
		opt->childAdd("el")->setAttr("id", lst[iF])->setText(epAt(lst[iF]).at().name());
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SPRT_ID,SEC_WR))	{ opt->setAttr("id", epAdd(opt->attr("id"))); epAt(opt->attr("id")).at().setName(opt->text()); }
	if(ctrChkNode(opt,"del",RWRWR_,"root",SPRT_ID,SEC_WR))	chldDel(mEndPnt,opt->attr("id"),-1,1);
    }
    else if(a_path == "/serv/asc" && ctrChkNode(opt)) {
	vector<uint32_t> chnls;
	chnlList(chnls);
	for(unsigned iCh = 0; iCh < chnls.size(); ++iCh) {
	    SecCnl scO = chnlGet(chnls[iCh]);
	    string secMess = _("Unknown");
	    switch(scO.secMessMode) {
		case MS_None:	secMess = _("None");	break;
		case MS_Sign:	secMess = _("Sign");	break;
		case MS_SignAndEncrypt: secMess = _("Sign&Encrypt");	break;
	    }
	    opt->childAdd("el")->setText(TSYS::strMess(_("%u(token %u): %s(%s,%s) at %s(live %s); Sequence server %u, client %u; Request ID %u"),
		chnls[iCh], scO.TokenId, scO.endPoint.c_str(), scO.secPolicy.c_str(), secMess.c_str(),
		atm2s(scO.tCreate*1e-6,"%Y-%m-%dT%H:%M:%S").c_str(),
		tm2s(1e-3*scO.tLife-1e-6*(curTime()-scO.tCreate)).c_str(),
		scO.servSeqN, scO.clSeqN, scO.reqId));
	}
    }
    else TProtocol::cntrCmdProc(opt);
}

//*************************************************
//* TProtIn                                       *
//*************************************************
TProtIn::TProtIn( string name ) : TProtocolIn(name), mSubscrIn(false), mPollTm(0), mPrevTm(0),
	mRcvBufSz(0), mSndBufSz(0), mMsgMaxSz(0), mChunkMaxCnt(0)	{ }

TProtIn::~TProtIn( )		{ }

TProt &TProtIn::owner( ) const	{ return *(TProt*)nodePrev(); }

bool TProtIn::mess( const string &reqst, string &answ )
{
    mBuf += reqst;
#ifdef POLL_OF_TR
    return owner().inReq(mBuf, name(), &answ);
#else
    return owner().inReq(mBuf, name());
#endif
}

//*************************************************
//* OPCEndPoint                                   *
//*************************************************
OPCEndPoint::OPCEndPoint( const string &iid, const string &idb, TElem *el ) :
    TConfig(el), EP(modPrt), mId(cfg("ID")), mName(cfg("NAME")), mDescr(cfg("DESCR")), mURL(cfg("URL")),
    mSerType(cfg("SerialzType").getId()), mAEn(cfg("EN").getBd()),
    mDB(idb), mLimSubScr(10), mLimMonitItms(1000), mLimRetrQueueTm(0)
{
    mId = iid;
    mURL = "opc.tcp://"+SYS->host();
}

OPCEndPoint::~OPCEndPoint( )
{
    try { setEnable(false); } catch(...) { }
}

TCntrNode &OPCEndPoint::operator=( const TCntrNode &node )
{
    const OPCEndPoint *src_n = dynamic_cast<const OPCEndPoint*>(&node);
    if(!src_n) return *this;

    if(enableStat())	setEnable(false);

    //Copy parameters
    exclCopy(*src_n, "ID;");
    setDB(src_n->DB());

    return *this;
}

void OPCEndPoint::postDisable( int flag )
{
    if(flag) SYS->db().at().dataDel(fullDB(), owner().nodePath()+tbl(), *this, true);
}

TProt &OPCEndPoint::owner( ) const	{ return *(TProt*)nodePrev(); }

string OPCEndPoint::name( )
{
    string tNm = mName;
    return tNm.size() ? tNm : id();
}

string OPCEndPoint::tbl( ) const	{ return owner().modId()+"_ep"; }

string OPCEndPoint::cert( )	{ return cfg("ServCert").getS(); }

string OPCEndPoint::pvKey( )	{ return cfg("ServPvKey").getS(); }

bool OPCEndPoint::cfgChange( TCfg &co, const TVariant &pc )	{ modif(); return true; }

#ifndef POLL_OF_TR
void *OPCEndPoint::Task( void *iep )
{
    OPCEndPoint &ep = *(OPCEndPoint *)iep;

    for(unsigned cntr = 0; !TSYS::taskEndRun(); cntr++) {
	ep.forceSubscrQueue = false;	//Disable the forcing mechanism for the method
	try { ep.subScrCycle(); }
	catch(OPCError &err)	{ mess_err(ep.nodePath().c_str(), err.mess.c_str()); }
	catch(TError &err)	{ mess_err(err.cat.c_str(), err.mess.c_str()); }

	TSYS::taskSleep((int64_t)(ep.subscrProcPer()*1000000));
    }

    return NULL;
}
#endif

void OPCEndPoint::load_( TConfig *icfg )
{
    if(!SYS->chkSelDB(DB())) throw TError();

    if(icfg) *(TConfig*)this = *icfg;
    else {
	//cfgViewAll(true);
	SYS->db().at().dataGet(fullDB(), owner().nodePath()+tbl(), *this);
    }

    //Security policies parse
    string sp = cfg("SecPolicies").getS();
    string spi;
    MtxAlloc res(secRes, true);
    mSec.clear();
    for(int off = 0; (spi=TSYS::strParse(sp,0,"\n",&off)).size(); )
	mSec.push_back(UA::SecuritySetting(TSYS::strParse(spi,0,":"),s2i(TSYS::strParse(spi,1,":"))));

    //Addition parameters load
    try {
	XMLNode	prmNd;
	string	vl;
	prmNd.load(cfg("A_PRMS").getS());
	vl = prmNd.attr("LimSubScr");		if(!vl.empty()) setLimSubScr(s2i(vl));
	vl = prmNd.attr("LimMonitItms");	if(!vl.empty()) setLimMonitItms(s2i(vl));
	vl = prmNd.attr("LimRetrQueueTm");	if(!vl.empty()) setLimRetrQueueTm(s2i(vl));
    } catch(...) { }
}

void OPCEndPoint::save_( )
{
    //Security policies store
    string sp;
    MtxAlloc res(secRes, true);
    for(unsigned iP = 0; iP < mSec.size(); iP++)
	sp += mSec[iP].policy + ":" + i2s(mSec[iP].messageMode)+"\n";
    cfg("SecPolicies").setS(sp);

    //Addition parameters save
    XMLNode prmNd("prms");
    prmNd.setAttr("LimSubScr", i2s(limSubScr()));
    prmNd.setAttr("LimMonitItms", i2s(limMonitItms()));
    prmNd.setAttr("LimRetrQueueTm", i2s(limRetrQueueTm()));
    cfg("A_PRMS").setS(prmNd.save(XMLNode::BrAllPast));

    SYS->db().at().dataSet(fullDB(),owner().nodePath()+tbl(),*this);
}

void OPCEndPoint::setEnable( bool vl )
{
    if(vl == enableStat()) return;
    owner().epEn(id(), vl);
    Server::EP::setEnable(vl);
    if(vl) {
	nodeReg(OpcUa_BaseObjectType,NodeId("DAQModuleObjectType",NS_OpenSCADA_DAQ),"DAQModuleObjectType",NC_ObjectType,OpcUa_HasSubtype);
	nodeReg(OpcUa_BaseObjectType,NodeId("DAQControllerObjectType",NS_OpenSCADA_DAQ),"DAQControllerObjectType",NC_ObjectType,OpcUa_HasSubtype);
	nodeReg(OpcUa_BaseObjectType,NodeId("DAQParameterObjectType",NS_OpenSCADA_DAQ),"DAQParameterObjectType",NC_ObjectType,OpcUa_HasSubtype);
	nodeReg(OpcUa_ObjectsFolder,NodeId(SYS->daq().at().subId(),NS_OpenSCADA_DAQ),SYS->daq().at().subId(),NC_Object,OpcUa_Organizes,OpcUa_FolderType)->
	    setAttr("DisplayName",SYS->daq().at().subName());
#ifndef POLL_OF_TR
	SYS->taskCreate(nodePath('.',true), 0/*mPrior*/, OPCEndPoint::Task, this);
    }
    else SYS->taskDestroy(nodePath('.',true));
#else
    }
#endif
}

void OPCEndPoint::setPublish( const string &inPrtId )
{
#ifdef POLL_OF_TR
    AutoHD<TProtIn> ip = owner().at(inPrtId);
    ip.at().mPollTm = subscrProcPer();
    ip.at().mEp = id();
#endif
    //Otherwise the task started/stoped into setEnable()
}

string OPCEndPoint::getStatus( )
{
    string rez = _("Disabled. ");
    if(enableStat()) {
	rez = _("Enabled. ");
	rez += TSYS::strMess(_("Requests %.4g."), (double)cntReq);
    }

    return rez;
}

uint32_t OPCEndPoint::reqData( int reqTp, XML_N &req )
{
    cntReq++;

    switch(reqTp) {
	case OpcUa_BrowseRequest: case OpcUa_BrowseNextRequest: {
	    TDAQS *nDAQ = NULL;
	    TTypeDAQ *nTpDAQ = NULL;
	    TController *nCntr = NULL;
	    TParamContr *nPrm = NULL;

	    int rez = Server::EP::reqData(reqTp, req);
	    //if(rez != OpcUa_BadBrowseNameInvalid) return rez;

	    NodeId nid = NodeId::fromAddr(req.attr("node"));
	    int rPn = s2i(req.attr("rPn"));

	    //Check for DAQ subsystem data
	    if(nid.ns() != NS_OpenSCADA_DAQ || TSYS::strParse(nid.strVal(),0,".") != SYS->daq().at().subId() ||
		(rPn && (int)req.childSize() >= rPn)) return rez;
	    NodeId rtId = NodeId::fromAddr(req.attr("RefTpId"));
	    uint32_t bd = s2i(req.attr("BrDir"));
	    uint32_t nClass = s2i(req.attr("ClassMask"));
	    string lstNd = req.attr("LastNode"); req.setAttr("LastNode","");

	    // Connect to DAQ node
	    int addrOff = 0;
	    string firstEl = TSYS::strParse(nid.strVal(),0,".",&addrOff);
	    AutoHD<TCntrNode> cNd = SYS->daq().at().daqAt(nid.strVal().substr(addrOff), '.', true);
	    if(cNd.freeStat()) return OpcUa_BadBrowseNameInvalid;

	    //typeDefinition reference browse
	    if(lstNd.empty() && rtId.numbVal() == OpcUa_References && (bd == BD_FORWARD || bd == BD_BOTH) &&
		!dynamic_cast<TDAQS*>(&cNd.at()))
	    {
		XML_N *ndTpDef = ndMap[NodeId(OpcUa_BaseDataVariableType).toAddr()];
		if(dynamic_cast<TTypeDAQ*>(&cNd.at()))		ndTpDef = ndMap[NodeId("DAQModuleObjectType",NS_OpenSCADA_DAQ).toAddr()];
		else if(dynamic_cast<TController*>(&cNd.at()))	ndTpDef = ndMap[NodeId("DAQControllerObjectType",NS_OpenSCADA_DAQ).toAddr()];
		else if(dynamic_cast<TParamContr*>(&cNd.at()))	ndTpDef = ndMap[NodeId("DAQParameterObjectType",NS_OpenSCADA_DAQ).toAddr()];
		unsigned cnClass = 0;
		if(ndTpDef && (!nClass || nClass&(cnClass=s2i(ndTpDef->attr("NodeClass")))))
		    req.childAdd("ref")->setAttr("NodeId", ndTpDef->attr("NodeId"))->
			setAttr("referenceTypeId", ndTpDef->attr("referenceTypeId"))->
			setAttr("dir", "1")->setAttr("name", ndTpDef->attr("name"))->
			setAttr("NodeClass", i2s(cnClass))->setAttr("typeDefinition", ndTpDef->attr("typeDefinition"));
	    }
	    // Inverse browse
	    if(lstNd.empty() && (!nClass || nClass&NC_Object) && (bd == BD_INVERSE || bd == BD_BOTH) && !dynamic_cast<TDAQS*>(&cNd.at()))
	    {
		XML_N *ndTpDef = NULL,
		      *rN = req.childAdd("ref")->setAttr("referenceTypeId", i2s(OpcUa_Organizes))->setAttr("dir", "0")->
						 setAttr("NodeClass", i2s(NC_Object));
		TCntrNode* ndUp = cNd.at().nodePrev();
		if((nDAQ=dynamic_cast<TDAQS*>(ndUp))) {
		    ndTpDef = ndMap[NodeId(OpcUa_FolderType).toAddr()];
		    rN->setAttr("NodeId", NodeId("DAQ",NS_OpenSCADA_DAQ).toAddr())->setAttr("name", nDAQ->subName())->
		    setAttr("typeDefinition", ndTpDef?ndTpDef->attr("typeDefinition"):"");
		}
		else if((nTpDAQ=dynamic_cast<TTypeDAQ*>(ndUp))) {
		    ndTpDef = ndMap[NodeId("DAQModuleObjectType",NS_OpenSCADA_DAQ).toAddr()];
		    rN->setAttr("NodeId", NodeId("DAQ."+nTpDAQ->DAQPath(),NS_OpenSCADA_DAQ).toAddr())->setAttr("name", nTpDAQ->modName())->
			setAttr("typeDefinition", ndTpDef?ndTpDef->attr("typeDefinition"):"");
		}
		else if((nCntr=dynamic_cast<TController*>(ndUp))) {
		    ndTpDef = ndMap[NodeId("DAQControllerObjectType",NS_OpenSCADA_DAQ).toAddr()];
		    rN->setAttr("NodeId", NodeId("DAQ."+nCntr->DAQPath(),NS_OpenSCADA_DAQ).toAddr())->setAttr("name", nCntr->name())->
			setAttr("typeDefinition", ndTpDef?ndTpDef->attr("typeDefinition"):"");
		}
		else if((nPrm=dynamic_cast<TParamContr*>(ndUp))) {
		    ndTpDef = ndMap[NodeId("DAQParameterObjectType",NS_OpenSCADA_DAQ).toAddr()];
		    rN->setAttr("NodeId", NodeId("DAQ."+nPrm->DAQPath(),NS_OpenSCADA_DAQ).toAddr())->setAttr("name", nPrm->name())->
			setAttr("typeDefinition", ndTpDef?ndTpDef->attr("typeDefinition"):"");
		}
	    }
	    // Forward browse
	    if(bd == BD_FORWARD || bd == BD_BOTH) {
		NodeId	tDef, refTpId = OpcUa_Organizes;
		vector<string> chLs;
		XML_N prevLs("PrevLs");

		//  Objects processing
		if(!nClass || nClass&NC_Object) {
		    if((nDAQ=dynamic_cast<TDAQS*>(&cNd.at()))) {
			nDAQ->modList(chLs);
			for(unsigned i_ch = 0; i_ch < chLs.size(); i_ch++)
			    prevLs.childAdd("ref")->setAttr("NodeId", NodeId("DAQ."+nDAQ->at(chLs[i_ch]).at().DAQPath(),NS_OpenSCADA_DAQ).toAddr())->
				setAttr("referenceTypeId", refTpId.toAddr())->setAttr("dir", "1")->setAttr("name", nDAQ->at(chLs[i_ch]).at().modName())->
				setAttr("NodeClass", i2s(NC_Object))->setAttr("typeDefinition", NodeId(OpcUa_FolderType).toAddr());
		    }
		    else if((nTpDAQ=dynamic_cast<TTypeDAQ*>(&cNd.at()))) {
			nTpDAQ->list(chLs);
			for(unsigned i_ch = 0; i_ch < chLs.size(); i_ch++)
			    prevLs.childAdd("ref")->setAttr("NodeId", NodeId("DAQ."+nTpDAQ->at(chLs[i_ch]).at().DAQPath(),NS_OpenSCADA_DAQ).toAddr())->
				setAttr("referenceTypeId", refTpId.toAddr())->setAttr("dir", "1")->setAttr("name", nTpDAQ->at(chLs[i_ch]).at().name())->
				setAttr("NodeClass", i2s(NC_Object))->setAttr("typeDefinition", NodeId("DAQControllerObjectType",NS_OpenSCADA_DAQ).toAddr());
		    }
		    else if((nCntr=dynamic_cast<TController*>(&cNd.at()))) {
			nCntr->list(chLs);
			for(unsigned i_ch = 0; i_ch < chLs.size(); i_ch++)
			    prevLs.childAdd("ref")->setAttr("NodeId", NodeId("DAQ."+nCntr->at(chLs[i_ch]).at().DAQPath(),NS_OpenSCADA_DAQ).toAddr())->
				setAttr("referenceTypeId", refTpId.toAddr())->setAttr("dir", "1")->setAttr("name", nCntr->at(chLs[i_ch]).at().name())->
				setAttr("NodeClass", i2s(NC_Object))->setAttr("typeDefinition", NodeId("DAQParameterObjectType",NS_OpenSCADA_DAQ).toAddr());
		    }
		    else if((nPrm=dynamic_cast<TParamContr*>(&cNd.at()))) {
			nPrm->list(chLs);
			for(unsigned i_ch = 0; i_ch < chLs.size(); i_ch++)
			    prevLs.childAdd("ref")->setAttr("NodeId", NodeId("DAQ."+nPrm->at(chLs[i_ch]).at().DAQPath(),NS_OpenSCADA_DAQ).toAddr())->
				setAttr("referenceTypeId", refTpId.toAddr())->setAttr("dir", "1")->setAttr("name", nPrm->at(chLs[i_ch]).at().name())->
				setAttr("NodeClass", i2s(NC_Object))->setAttr("typeDefinition", NodeId("DAQParameterObjectType",NS_OpenSCADA_DAQ).toAddr());
		    }
		}
		//  Variables processing
		if((!nClass || nClass&NC_Variable) && (nPrm=dynamic_cast<TParamContr*>(&cNd.at()))) {
		    nPrm->vlList(chLs);
		    for(unsigned i_ch = 0; i_ch < chLs.size(); i_ch++)
			prevLs.childAdd("ref")->setAttr("NodeId", NodeId("DAQ."+nPrm->vlAt(chLs[i_ch]).at().DAQPath(),NS_OpenSCADA_DAQ).toAddr())->
			    setAttr("referenceTypeId", NodeId(OpcUa_Organizes/*OpcUa_HasComponent*/).toAddr())->setAttr("dir", "1")->
			    setAttr("name", nPrm->vlAt(chLs[i_ch]).at().name())->
			    setAttr("NodeClass", i2s(NC_Variable))->setAttr("typeDefinition", NodeId(OpcUa_BaseDataVariableType).toAddr());
		}

		bool lstOK = lstNd.empty() ? true : false;
		for(unsigned iP = 0; iP < prevLs.childSize(); iP++) {
		    XML_N *pN = prevLs.childGet(iP);
		    if(!lstOK) { lstOK = (lstNd==pN->attr("NodeId")); continue; }
		    *(req.childAdd("ref")) = *pN;
		    if(rPn && (int)req.childSize() >= rPn && (iP+1) < prevLs.childSize()) {
			req.setAttr("LastNode", pN->attr("NodeId"));
			break;
		    }
		}
	    }
	    return 0;
	}
	case OpcUa_ReadRequest: {
	    TDAQS *nDAQ = NULL;
	    TTypeDAQ *nTpDAQ = NULL;
	    TController *nCntr = NULL;
	    TParamContr *nPrm = NULL;
	    TVal *nVal = NULL;

	    NodeId nid = NodeId::fromAddr(req.attr("node"));

	    //Some system nodes process
	    if(nid.ns() == NS_OPC_UA) {
		map<string, XML_N*>::iterator ndX = ndMap.find(nid.toAddr());
		if(ndX != ndMap.end())
		    switch(nid.numbVal()) {
			case OpcUa_Server_NamespaceArray:
			    req.setAttr("Value", ndX->second->attr("Value")+"\nOpenSCADA_DAQ");
			    break;
		    }
	    }

	    uint32_t rez = Server::EP::reqData(reqTp, req);
	    if(rez != OpcUa_BadNodeIdUnknown) return rez;

	    // OpenSCADA DAQ parameter's attribute
	    if(nid.ns() != NS_OpenSCADA_DAQ)	return OpcUa_BadNodeIdUnknown;
	    uint32_t aid = s2i(req.attr("aid"));

	    // Connect to DAQ node
	    int addrOff = 0;
	    string firstEl = TSYS::strParse(nid.strVal(),0,".",&addrOff);
	    AutoHD<TCntrNode> cNd = SYS->daq().at().daqAt(nid.strVal().substr(addrOff), '.', true);
	    if(cNd.freeStat()) return OpcUa_BadNodeIdUnknown;

	    switch(aid) {
		case AId_NodeId: req.setAttr("type", i2s(OpcUa_NodeId))->setText(nid.toAddr());	return 0;
		case AId_BrowseName: {
		    req.setAttr("type", i2s(OpcUa_QualifiedName));
		    if((nDAQ=dynamic_cast<TDAQS*>(&cNd.at())))			req.setText(nDAQ->subName());
		    else if((nTpDAQ=dynamic_cast<TTypeDAQ*>(&cNd.at())))	req.setText(nTpDAQ->modName());
		    else if((nCntr=dynamic_cast<TController*>(&cNd.at())))	req.setText(nCntr->name());
		    else if((nPrm=dynamic_cast<TParamContr*>(&cNd.at())))	req.setText(nPrm->name());
		    else if((nVal=dynamic_cast<TVal*>(&cNd.at())))		req.setText(nVal->name());
		    else req.setText(cNd.at().nodeName());
		    return 0;
		}
		case AId_InverseName: req.setAttr("type", i2s(OpcUa_LocalizedText))->setText("");			return 0;
		case AId_WriteMask: case AId_UserWriteMask: req.setAttr("type", i2s(OpcUa_UInt32))->setText("0");	return 0;
		default: {
		    if((nVal=dynamic_cast<TVal*>(&cNd.at()))) {
			if(s2i(req.attr("dtPerGet")))
			    req.setAttr("dtPer", (nVal->arch().freeStat()?"0":r2s(nVal->arch().at().period()*1e-6)));
			//  Variable
			switch(aid) {
			    case AId_NodeClass: req.setAttr("type", i2s(OpcUa_Int32))->setText(i2s(NC_Variable));	return 0;
			    case AId_DisplayName: req.setAttr("type", i2s(OpcUa_LocalizedText))->setText(nVal->name());	return 0;
			    case AId_Descr: req.setAttr("type", i2s(OpcUa_LocalizedText))->setText(nVal->fld().descr());	return 0;
			    case AId_Value: {
				int64_t tm = 0;
				bool dtOK = true;
				switch(nVal->fld().type()) {
				    case TFld::Boolean:	req.setAttr("type", i2s(OpcUa_Boolean))->setText(nVal->getS(&tm));	break;
				    case TFld::Integer:	req.setAttr("type", i2s(OpcUa_IntAuto/*OpcUa_Int64*/))->setText(nVal->getS(&tm));	break;
				    case TFld::Real:	req.setAttr("type", i2s(OpcUa_Double))->setText(nVal->getS(&tm));	break;
				    case TFld::String:	req.setAttr("type", i2s(OpcUa_String))->setText(nVal->getS(&tm));	break;
				    case TFld::Object: {	//!!!! With structures support append detect ones
					AutoHD<TArrayObj> arr = nVal->getO(&tm);
					string rVl;
					if(arr.freeStat()) { dtOK = false; break; }
					switch(arr.at().arGet(0).type()) {
					    case TVariant::Boolean:
						for(size_t iA = 0; iA < arr.at().arSize(); iA++) rVl += arr.at().arGet(iA).getS() + "\n";
						req.setAttr("type", i2s(OpcUa_Array|OpcUa_Boolean))->setText(rVl);
						break;
					    case TVariant::Integer:
						for(size_t iA = 0; iA < arr.at().arSize(); iA++) rVl += arr.at().arGet(iA).getS() + "\n";
						req.setAttr("type", i2s(OpcUa_Array|OpcUa_Int64))->setText(rVl);
						break;
					    case TVariant::Real:
						for(size_t iA = 0; iA < arr.at().arSize(); iA++) rVl += arr.at().arGet(iA).getS() + "\n";
						req.setAttr("type", i2s(OpcUa_Array|OpcUa_Double))->setText(rVl);
						break;
					    case TVariant::String:
						for(size_t iA = 0; iA < arr.at().arSize(); iA++) rVl += arr.at().arGet(iA).getS() + "\n";
						req.setAttr("type", i2s(OpcUa_Array|OpcUa_String))->setText(rVl);
						break;
					    default: dtOK = false;
					}
					break;
				    }
				    default: dtOK = false;
				}
				if(dtOK) {
				    if(s2i(req.attr("dtTmGet"))) req.setAttr("dtTm",ll2s(tm));
				    return 0;
				}
				break;
			    }
			    case AId_DataType:
				switch(nVal->fld().type()) {
				    case TFld::Boolean: req.setAttr("type", i2s(OpcUa_NodeId))->setText(i2s(OpcUa_Boolean));	return 0;
				    case TFld::Integer: req.setAttr("type", i2s(OpcUa_NodeId))->setText(i2s(OpcUa_Int32));	return 0;
				    case TFld::Real:    req.setAttr("type", i2s(OpcUa_NodeId))->setText(i2s(OpcUa_Double));	return 0;
				    case TFld::String:  req.setAttr("type", i2s(OpcUa_NodeId))->setText(i2s(OpcUa_String));	return 0;
				    case TFld::Object: {	//!!!! With structures support append detect ones
					int64_t tm = 0;
					AutoHD<TArrayObj> arr = nVal->getO(&tm);
					if(arr.freeStat()) break;
					switch(arr.at().arGet(0).type()) {
					    case TVariant::Boolean:
						req.setAttr("type", i2s(OpcUa_NodeId))->setText(i2s(OpcUa_Array|OpcUa_Boolean));return 0;
					    case TVariant::Integer:
						req.setAttr("type", i2s(OpcUa_NodeId))->setText(i2s(OpcUa_Array|OpcUa_Int64));	return 0;
					    case TVariant::Real:
						req.setAttr("type", i2s(OpcUa_NodeId))->setText(i2s(OpcUa_Array|OpcUa_Double));	return 0;
					    case TVariant::String:
						req.setAttr("type", i2s(OpcUa_NodeId))->setText(i2s(OpcUa_Array|OpcUa_String));	return 0;
					    default: break;
					}
					break;
				    }
				    default: break;
				}
				break;
			    case AId_ValueRank: req.setAttr("type", i2s(OpcUa_Int32))->setText("-1");				return 0;
			    case AId_ArrayDimensions: req.setAttr("type", i2s(OpcUa_Array|OpcUa_UInt32))->setText("");		return 0;
			    case AId_AccessLevel: case AId_UserAccessLevel:
				req.setAttr("type", i2s(OpcUa_Byte))->setText(i2s(ACS_Read | (nVal->fld().flg()&TFld::NoWrite ? 0 : ACS_Write)));
				return 0;
			    case AId_MinimumSamplingInterval: req.setAttr("type", i2s(OpcUa_Double))->setText("0");		return 0;
			    case AId_Historizing: req.setAttr("type", i2s(OpcUa_Boolean))->setText("0");			return 0;
			    default: return OpcUa_BadAttributeIdInvalid;
			}
		    }
		    //  Objects
		    else switch(aid) {
			case AId_NodeClass: req.setAttr("type", i2s(OpcUa_Int32))->setText(i2s(NC_Object));			return 0;
			case AId_DisplayName: {
			    req.setAttr("type", i2s(OpcUa_LocalizedText));
			    if((nTpDAQ=dynamic_cast<TTypeDAQ*>(&cNd.at())))		req.setText(nTpDAQ->modName());
			    else if((nCntr=dynamic_cast<TController*>(&cNd.at())))	req.setText(nCntr->name());
			    else if((nPrm=dynamic_cast<TParamContr*>(&cNd.at())))	req.setText(nPrm->name());
			    else if((nVal=dynamic_cast<TVal*>(&cNd.at())))		req.setText(nVal->name());
			    else req.setText(cNd.at().nodeName());
			    return 0;
			}
			case AId_Descr:
			    req.setAttr("type", i2s(OpcUa_LocalizedText));
			    if((nTpDAQ=dynamic_cast<TTypeDAQ*>(&cNd.at())))		req.setText(nTpDAQ->modInfo("Description"));
			    else if((nCntr=dynamic_cast<TController*>(&cNd.at())))	req.setText(nCntr->descr());
			    else if((nPrm=dynamic_cast<TParamContr*>(&cNd.at())))	req.setText(nPrm->descr());
			    else return OpcUa_BadAttributeIdInvalid;
			    return 0;
			case AId_EventNotifier: req.setAttr("type", i2s(OpcUa_Byte))->setText("0");				return 0;
			default: return OpcUa_BadAttributeIdInvalid;
		    }
		}
	    }
	    return OpcUa_BadNodeIdUnknown;
	}
	case OpcUa_WriteRequest: {
	    uint32_t rez = Server::EP::reqData(reqTp, req);
	    if(rez != OpcUa_BadNodeIdUnknown) return rez;

	    NodeId nid = NodeId::fromAddr(req.attr("node"));

	    // OpenSCADA DAQ parameter's attribute
	    if(nid.ns() != NS_OpenSCADA_DAQ)	return OpcUa_BadNodeIdUnknown;
	    uint32_t aid = s2i(req.attr("aid"));
	    unsigned vTp = s2i(req.attr("VarTp"));

	    // Connect to DAQ node
	    int addrOff = 0;
	    string firstEl = TSYS::strParse(nid.strVal(),0,".",&addrOff);
	    AutoHD<TCntrNode> cNd = SYS->daq().at().daqAt(nid.strVal().substr(addrOff), '.', true);
	    if(cNd.freeStat()) return OpcUa_BadNodeIdUnknown;

	    if(aid != AId_Value || !dynamic_cast<TVal*>(&cNd.at())) return OpcUa_BadNothingToDo;
	    AutoHD<TVal> vNd = cNd;
	    if(vNd.at().fld().type() == TFld::Object && (vTp&OpcUa_Array)) {
		TArrayObj *arr = new TArrayObj();
		string vEl;
		switch(vTp&OpcUa_VarMask) {
		    case OpcUa_Boolean:
			for(int off = 0, iEl = 0; (vEl=TSYS::strLine(req.text(),0,&off)).size(); iEl++)
			    arr->arSet(iEl, bool(s2i(vEl)));
			break;
		    case OpcUa_SByte: case OpcUa_Byte: case OpcUa_Int16: case OpcUa_UInt16:
		    case OpcUa_Int32: case OpcUa_UInt32: case OpcUa_Int64: case OpcUa_UInt64:
			for(int off = 0, iEl = 0; (vEl=TSYS::strLine(req.text(),0,&off)).size(); iEl++)
			    arr->arSet(iEl, (int64_t)s2ll(vEl));
			break;
		    case OpcUa_Float: case OpcUa_Double:
			for(int off = 0, iEl = 0; (vEl=TSYS::strLine(req.text(),0,&off)).size(); iEl++)
			    arr->arSet(iEl, s2r(vEl));
			break;
		    default:
			for(int off = 0, iEl = 0; (vEl=TSYS::strLine(req.text(),0,&off)).size(); iEl++)
			    arr->arSet(iEl, vEl);
			break;
		}
		vNd.at().setO(arr);
	    }
	    else vNd.at().setS(req.text());

	    return 0;
	}
    }

    return OpcUa_BadNodeIdUnknown;
}

void OPCEndPoint::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TCntrNode::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("End point: ")+name(),RWRWR_,"root",SPRT_ID);
	if(ctrMkNode("area",opt,-1,"/ep",_("End point"))) {
	    if(ctrMkNode("area",opt,-1,"/ep/st",_("State"))) {
		ctrMkNode("fld",opt,-1,"/ep/st/status",_("Status"),R_R_R_,"root",SPRT_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/ep/st/asess",_("Active sessions"),(enableStat()?R_R_R_:0),"root",SPRT_ID);
		ctrMkNode("list",opt,-1,"/ep/st/asubscr",_("Active subscriptions"),(enableStat()?R_R_R_:0),"root",SPRT_ID);
		ctrMkNode("fld",opt,-1,"/ep/st/en_st",_("Enabled"),RWRWR_,"root",SPRT_ID,1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/ep/st/db",_("DB"),RWRWR_,"root",SPRT_ID,4,
		    "tp","str","dest","select","select","/db/list","help",TMess::labDB());
	    }
	    if(ctrMkNode("area",opt,-1,"/ep/cfg",_("Configuration"))) {
		TConfig::cntrCmdMake(opt,"/ep/cfg",0,"root",SPRT_ID,RWRWR_);
		ctrRemoveNode(opt, "/ep/cfg/A_PRMS");
		ctrMkNode("fld",opt,-1,"/ep/cfg/ServCert",cfg("ServCert").fld().descr(),RWRW__,"root",SPRT_ID,3,"tp","str","cols","90","rows","7");
		ctrMkNode("fld",opt,-1,"/ep/cfg/ServPvKey",cfg("ServPvKey").fld().descr(),RWRW__,"root",SPRT_ID,3,"tp","str","cols","90","rows","7");
		ctrRemoveNode(opt,"/ep/cfg/SecPolicies");
		if(ctrMkNode("table",opt,-1,"/ep/cfg/secPlc",cfg("SecPolicies").fld().descr(),RWRWR_,"root",SPRT_ID,1,"s_com","add,del"))
		{
		    ctrMkNode("list",opt,-1,"/ep/cfg/secPlc/0",_("Policy"),RWRWR_,"root",SPRT_ID,3,"tp","str","dest","select","sel_list","None;Basic128Rsa15;Basic256");
		    ctrMkNode("list",opt,-1,"/ep/cfg/secPlc/1",_("Message mode"),RWRWR_,"root",SPRT_ID,4,"tp","dec","dest","select","sel_id","1;2;3","sel_list",_("None;Sign;Sign&Encrypt"));
		}
	    }
	    if(ctrMkNode("area",opt,-1,"/data",_("Data")))
		if(ctrMkNode("area",opt,-1,"/data/lim",_("Limits"))) {
		    ctrMkNode("fld",opt,-1,"/data/lim/subScr",_("Subscriptions"),RWRWR_,"root",SPRT_ID,1,"tp","dec");
		    ctrMkNode("fld",opt,-1,"/data/lim/monitItms",_("Monitored items"),RWRWR_,"root",SPRT_ID,1,"tp","dec");
		    ctrMkNode("fld",opt,-1,"/data/lim/retrQueueTm",_("Retransmission queue time, sek."),RWRWR_,"root",SPRT_ID,2,
			"tp","dec", "help",_("Set to 0 for automatic by cntrKeepAlive*publInterv"));
		}
	}
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/ep/st/status" && ctrChkNode(opt))	opt->setText(getStatus());
    else if(a_path == "/ep/st/en_st") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD))	opt->setText(enableStat()?"1":"0");
	if(ctrChkNode(opt,"set",RWRWR_,"root",SPRT_ID,SEC_WR))	setEnable(s2i(opt->text()));
    }
    else if(a_path == "/ep/st/db") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD))	opt->setText(DB());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SPRT_ID,SEC_WR))	setDB(opt->text());
    }
    else if(a_path == "/ep/st/asess" && ctrChkNode(opt)) {
	OPCAlloc mtx(mtxData, true);
	for(unsigned iSess = 0; iSess < sessN(); ++iSess) {
	    Server::Sess &sesO = mSess[iSess];
	    string sChnlLs;
	    for(unsigned iCh = 0; iCh < sesO.secCnls.size(); ++iCh)
		sChnlLs += (sChnlLs.size()?",":"") + uint2str(sesO.secCnls[iCh]);
	    opt->childAdd("el")->setText(TSYS::strMess(_("%d(%s): at %s(live %s), secure channels \"%s\"; Publish requests %u"),
		iSess+1, sesO.inPrtId.c_str(),
		atm2s(sesO.tAccess*1e-6,"%Y-%m-%dT%H:%M:%S").c_str(),
		tm2s(1e-3*sesO.tInact-1e-6*(curTime()-sesO.tAccess)).c_str(),
		sChnlLs.c_str(), sesO.publishReqs.size()));
	}
    }
    else if(a_path == "/ep/st/asubscr" && ctrChkNode(opt)) {
	OPCAlloc mtx(mtxData, true);
	for(unsigned iSbscr = 0; iSbscr < subscrN(); ++iSbscr) {
	    Server::Subscr &sbscrO = mSubScr[iSbscr];
	    string state = _("Unknown");
	    switch(sbscrO.st) {
		case SS_CLOSED:		state = _("Closed");	break;
		case SS_CREATING:	state = _("Creating");	break;
		case SS_NORMAL:		state = _("Normal");	break;
		case SS_LATE:		state = _("Late");	break;
		case SS_KEEPALIVE:	state = _("KeepAlive");	break;
	    }
	    opt->childAdd("el")->setText(TSYS::strMess(_("%d: %s, session %d; Publish %s, interval %s, last %s, sequence %d, lifetime %d, keep alive %d; Monitored items %u; Retransmission queue %u"),
		iSbscr+1, state.c_str(), sbscrO.sess,
		(sbscrO.publEn?_("Enabled"):_("Disabled")),
		tm2s(1e-3*sbscrO.publInterval).c_str(),
		atm2s(1e-6*sbscrO.lstPublTm,"%Y-%m-%dT%H:%M:%S").c_str(),
		sbscrO.seqN, vmax(0,sbscrO.lifetimeCnt-sbscrO.wKA), vmax(0,sbscrO.maxKeepAliveCnt-sbscrO.wKA),
		sbscrO.mItems.size(), sbscrO.retrQueue.size()));
	}
    }
    else if(a_path == "/ep/cfg/ls_itr" && ctrChkNode(opt)) {
	opt->childAdd("el")->setText("*");
	vector<string> sls;
	SYS->transport().at().inTrList(sls);
	for(unsigned i_s = 0; i_s < sls.size(); i_s++)
	    opt->childAdd("el")->setText(sls[i_s]);
    }
    else if(a_path == "/ep/cfg/secPlc") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD)) {
	    XMLNode *n_pol	= ctrMkNode("list",opt,-1,"/ep/cfg/secPlc/0","",RWRWR_);
	    XMLNode *n_mm	= ctrMkNode("list",opt,-1,"/ep/cfg/secPlc/1","",RWRWR_);
	    MtxAlloc res(secRes, true);
	    for(unsigned iP = 0; iP < mSec.size(); iP++) {
		if(n_pol) n_pol->childAdd("el")->setText(mSec[iP].policy);
		if(n_mm)  n_mm->childAdd("el")->setText(i2s(mSec[iP].messageMode));
	    }
	    return;
	}
	MtxAlloc res(secRes, true);
	if(ctrChkNode(opt,"add",RWRWR_,"root",SPRT_ID,SEC_WR)) { mSec.push_back(UA::SecuritySetting("None",MS_None)); modif(); return; }
	int row = s2i(opt->attr("row"));
	if(row < 0 || row >= (int)mSec.size()) throw TError(nodePath().c_str(),_("No present selected row."));
	if(ctrChkNode(opt,"del",RWRWR_,"root",SPRT_ID,SEC_WR)) { mSec.erase(mSec.begin()+row); modif(); return; }
	if(ctrChkNode(opt,"set",RWRWR_,"root",SPRT_ID,SEC_WR)) {
	    int col = s2i(opt->attr("col"));
	    if(col == 0)	mSec[row].policy = opt->text();
	    else if(col == 1)	mSec[row].messageMode = (MessageSecurityMode)s2i(opt->text());
	    modif();
	}
    }
    else if(a_path.compare(0,7,"/ep/cfg") == 0) TConfig::cntrCmdProc(opt, TSYS::pathLev(a_path,2), "root", SPRT_ID, RWRWR_);
    else if(a_path == "/data/lim/subScr") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD))	opt->setText(i2s(limSubScr()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SPRT_ID,SEC_WR))	setLimSubScr(s2i(opt->text()));
    }
    else if(a_path == "/data/lim/monitItms") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD))	opt->setText(i2s(limMonitItms()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SPRT_ID,SEC_WR))	setLimMonitItms(s2i(opt->text()));
    }
    else if(a_path == "/data/lim/retrQueueTm") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD))	opt->setText(i2s(limRetrQueueTm()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SPRT_ID,SEC_WR))	setLimRetrQueueTm(s2i(opt->text()));
    }
    else TCntrNode::cntrCmdProc(opt);
}
