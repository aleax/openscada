
//OpenSCADA system module DAQ.OPC_UA file: mod_prt.cpp
/***************************************************************************
 *   Copyright (C) 2009-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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

    mType	= PRT_TYPE;
    mName	= PRT_NAME;
    mVers	= PRT_MVER;
    mAuthor	= PRT_AUTOR;
    mDescr	= PRT_DESCR;
    mLicense	= PRT_LICENSE;
    mSource	= name;

    mEndPnt = grpAdd("ep_");

    //Node DB structure
    mEndPntEl.fldAdd(new TFld("ID",_("ID"),TFld::String,TCfg::Key|TFld::NoWrite,OBJ_ID_SZ));
    mEndPntEl.fldAdd(new TFld("NAME",_("Name"),TFld::String,TCfg::TransltText,OBJ_NM_SZ));
    mEndPntEl.fldAdd(new TFld("DESCR",_("Description"),TFld::String,TFld::FullText|TCfg::TransltText,"300"));
    mEndPntEl.fldAdd(new TFld("EN",_("To enable"),TFld::Boolean,0,"1","0"));
    mEndPntEl.fldAdd(new TFld("SerialzType",_("Serializer type"),TFld::Integer,TFld::Selected,"1","0","0",_("Binary")));
    mEndPntEl.fldAdd(new TFld("URL",_("URL"),TFld::String,0,"50","opc.tcp://localhost:4841"));
    mEndPntEl.fldAdd(new TFld("SecPolicies",_("Security policies"),TFld::String,TFld::FullText,"100","None:0\nBasic128Rsa15:1"));
    mEndPntEl.fldAdd(new TFld("ServCert",_("Server certificate (PEM)"),TFld::String,TFld::FullText,"10000"));
    mEndPntEl.fldAdd(new TFld("ServPvKey",_("Server private key (PEM)"),TFld::String,TFld::FullText,"10000"));
}

TProt::~TProt( )			{ nodeDelAll(); }

string TProt::applicationUri( )		{ return "urn:"+SYS->host()+":OpenSCADA:DAQ.OPC_UA"; }

string TProt::productUri( )		{ return PACKAGE_SITE; }

string TProt::applicationName( )	{ return "OpenSCADA.OPC-UA Server"; }

void TProt::epAdd( const string &iid, const string &db )
{
    chldAdd(mEndPnt, new OPCEndPoint(iid,db,&endPntEl()));
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
    return Server::inReq(request, inPrtId, answ);
}

int TProt::writeToClient( const string &inPrtId, const string &data )
{
    return at(inPrtId).at().writeTo(data);
}

void TProt::discoveryUrls( vector<string> &ls )
{
    ls.clear();
    //Get allowed enpoints list
    vector<string> epLs;
    epList(epLs);
    for(unsigned i_ep = 0; i_ep < epLs.size(); i_ep++)
    {
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
    try
    {
	TConfig g_cfg(&endPntEl());
	g_cfg.cfgViewAll(false);
	vector<string> db_ls;
	map<string, bool> itReg;

	// Search into DB
	SYS->db().at().dbList(db_ls,true);
	db_ls.push_back(DB_CFG);
	for(unsigned i_db = 0; i_db < db_ls.size(); i_db++)
	    for(int fld_cnt = 0; SYS->db().at().dataSeek(db_ls[i_db]+"."+modId()+"_ep",nodePath()+modId()+"_ep",fld_cnt++,g_cfg); )
	    {
		string id = g_cfg.cfg("ID").getS();
		if(!epPresent(id)) epAdd(id,(db_ls[i_db]==SYS->workDB())?"*.*":db_ls[i_db]);
		itReg[id] = true;
	    }

	// Check for remove items removed from DB
	if(!SYS->selDB().empty())
	{
	    epList(db_ls);
	    for(unsigned i_it = 0; i_it < db_ls.size(); i_it++)
		if(itReg.find(db_ls[i_it]) == itReg.end() && SYS->chkSelDB(epAt(db_ls[i_it]).at().DB()))
		    epDel(db_ls[i_it]);
	}
    }
    catch(TError err)
    {
	mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	mess_err(nodePath().c_str(),_("Search and create new server error."));
    }
}

void TProt::save_( )	{ }

void TProt::modStart( )
{
    vector<string> ls;
    epList(ls);
    for(unsigned i_n = 0; i_n < ls.size(); i_n++)
	if(epAt(ls[i_n]).at().toEnable())
	    epAt(ls[i_n]).at().setEnable(true);
}

void TProt::modStop( )
{
    vector<string> ls;
    epList(ls);
    for(unsigned i_n = 0; i_n < ls.size(); i_n++)
	epAt(ls[i_n]).at().setEnable(false);
}

TProtocolIn *TProt::in_open( const string &name )	{ return new TProtIn(name); }

void TProt::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info")
    {
	TProtocol::cntrCmdProc(opt);
	ctrMkNode("grp", opt, -1, "/br/ep_", _("End point"), RWRWR_, "root", SPRT_ID, 2, "idm",OBJ_NM_SZ, "idSz",OBJ_ID_SZ);
	if(ctrMkNode("area",opt,0,"/ep",_("End points")))
	    ctrMkNode("list", opt, -1, "/ep/ep", _("End points"), RWRWR_, "root", SPRT_ID, 5,
		"tp","br", "idm",OBJ_NM_SZ, "s_com","add,del", "br_pref","ep_", "idSz",OBJ_ID_SZ);
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/br/ep_" || a_path == "/ep/ep")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD))
	{
	    vector<string> lst;
	    epList(lst);
	    for(unsigned i_f = 0; i_f < lst.size(); i_f++)
		opt->childAdd("el")->setAttr("id", lst[i_f])->setText(epAt(lst[i_f]).at().name());
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SPRT_ID,SEC_WR))
	{
	    string vid = TSYS::strEncode(opt->attr("id"), TSYS::oscdID);
	    epAdd(vid); epAt(vid).at().setName(opt->text());
	}
	if(ctrChkNode(opt,"del",RWRWR_,"root",SPRT_ID,SEC_WR))	chldDel(mEndPnt,opt->attr("id"),-1,1);
    }
    else TProtocol::cntrCmdProc(opt);
}

//*************************************************
//* TProtIn                                       *
//*************************************************
TProtIn::TProtIn( string name ) : TProtocolIn(name)	{ }

TProtIn::~TProtIn( )		{ }

TProt &TProtIn::owner( )	{ return *(TProt*)nodePrev(); }

bool TProtIn::mess( const string &reqst, string &answ )
{
    mBuf += reqst;
    return owner().inReq(mBuf, name());
}

//*************************************************
//* OPCEndPoint                                   *
//*************************************************
OPCEndPoint::OPCEndPoint( const string &iid, const string &idb, TElem *el ) :
    TConfig(el), EP(modPrt), mId(cfg("ID")), mName(cfg("NAME")), mDescr(cfg("DESCR")), mURL(cfg("URL")),
    mSerType(cfg("SerialzType").getId()), mAEn(cfg("EN").getBd()),
    mDB(idb)
{
    mId = iid;
    mURL = "opc.tcp://"+SYS->host();
}

OPCEndPoint::~OPCEndPoint( )
{
    try{ setEnable(false); } catch(...) { }
}

TCntrNode &OPCEndPoint::operator=( TCntrNode &node )
{
    OPCEndPoint *src_n = dynamic_cast<OPCEndPoint*>(&node);
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

TProt &OPCEndPoint::owner( )	{ return *(TProt*)nodePrev(); }

string OPCEndPoint::name( )
{
    string tNm = mName;
    return tNm.size() ? tNm : id();
}

string OPCEndPoint::tbl( )	{ return owner().modId()+"_ep"; }

string OPCEndPoint::cert( )	{ return cfg("ServCert").getS(); }

string OPCEndPoint::pvKey( )	{ return cfg("ServPvKey").getS(); }

bool OPCEndPoint::cfgChange( TCfg &co, const TVariant &pc )	{ modif(); return true; }

void *OPCEndPoint::Task( void *iep )
{
    OPCEndPoint &ep = *(OPCEndPoint *)iep;

    for(unsigned cntr = 0; !TSYS::taskEndRun(); cntr++)
    {
	try { ep.subScrCycle(cntr); }
	catch(OPCError err)	{ mess_err(ep.nodePath().c_str(), err.mess.c_str()); }
	catch(TError err)	{ mess_err(err.cat.c_str(), err.mess.c_str()); }

	TSYS::taskSleep((int64_t)(ep.subscrProcPer()*1000000));
    }

    return NULL;
}

void OPCEndPoint::load_( )
{
    if(!SYS->chkSelDB(DB())) throw TError();
    cfgViewAll(true);
    SYS->db().at().dataGet(fullDB(), owner().nodePath()+tbl(), *this);

    //Security policies parse
    string sp = cfg("SecPolicies").getS();
    string spi;
    ResAlloc res(nodeRes(), true);
    mSec.clear();
    for(int off = 0; (spi=TSYS::strParse(sp,0,"\n",&off)).size(); )
	mSec.push_back(UA::SecuritySetting(TSYS::strParse(spi,0,":"),s2i(TSYS::strParse(spi,1,":"))));
}

void OPCEndPoint::save_( )
{
    //Security policies store
    string sp;
    ResAlloc res(nodeRes(), false);
    for(unsigned i_p = 0; i_p < mSec.size(); i_p++)
	sp += mSec[i_p].policy + ":" + i2s(mSec[i_p].messageMode)+"\n";
    cfg("SecPolicies").setS(sp);

    SYS->db().at().dataSet(fullDB(),owner().nodePath()+tbl(),*this);
}

void OPCEndPoint::setEnable( bool vl )
{
    if(vl == enableStat()) return;
    owner().epEn(id(), vl);
    Server::EP::setEnable(vl);
    if(vl)
    {
	nodeReg(OpcUa_BaseObjectType,NodeId("DAQModuleObjectType",NS_OpenSCADA_DAQ),"DAQModuleObjectType",NC_ObjectType,OpcUa_HasSubtype);
	nodeReg(OpcUa_BaseObjectType,NodeId("DAQControllerObjectType",NS_OpenSCADA_DAQ),"DAQControllerObjectType",NC_ObjectType,OpcUa_HasSubtype);
	nodeReg(OpcUa_BaseObjectType,NodeId("DAQParameterObjectType",NS_OpenSCADA_DAQ),"DAQParameterObjectType",NC_ObjectType,OpcUa_HasSubtype);
	nodeReg(OpcUa_ObjectsFolder,NodeId(SYS->daq().at().subId(),NS_OpenSCADA_DAQ),SYS->daq().at().subId(),NC_Object,OpcUa_Organizes,OpcUa_FolderType)->
	    setAttr("DisplayName",SYS->daq().at().subName());
	SYS->taskCreate(nodePath('.',true), 0/*mPrior*/, OPCEndPoint::Task, this);
    }
    else SYS->taskDestroy(nodePath('.',true));
}

string OPCEndPoint::getStatus( )
{
    string rez = _("Disabled. ");
    if(enableStat())
    {
	rez = _("Enabled. ");
	rez += TSYS::strMess(_("Requests %.4g."), (double)cntReq);
    }

    return rez;
}

uint32_t OPCEndPoint::reqData( int reqTp, XML_N &req )
{
    cntReq++;

    switch(reqTp)
    {
	case OpcUa_BrowseRequest: case OpcUa_BrowseNextRequest:
	{
	    TDAQS *nDAQ = NULL;
	    TTipDAQ *nTpDAQ = NULL;
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
	    if(lstNd.empty() && rtId.numbVal() == OpcUa_References && (bd == BD_FORWARD || bd == BD_BOTH) && !dynamic_cast<TDAQS*>(&cNd.at()))
	    {
		XML_N *ndTpDef = ndMap[NodeId(OpcUa_BaseDataVariableType).toAddr()];
		if(dynamic_cast<TTipDAQ*>(&cNd.at()))		ndTpDef = ndMap[NodeId("DAQModuleObjectType",NS_OpenSCADA_DAQ).toAddr()];
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
		if((nDAQ=dynamic_cast<TDAQS*>(ndUp)))
		{
		    ndTpDef = ndMap[NodeId(OpcUa_FolderType).toAddr()];
		    rN->setAttr("NodeId", NodeId("DAQ",NS_OpenSCADA_DAQ).toAddr())->setAttr("name", nDAQ->subName())->
		    setAttr("typeDefinition", ndTpDef?ndTpDef->attr("typeDefinition"):"");
		}
		else if((nTpDAQ=dynamic_cast<TTipDAQ*>(ndUp)))
		{
		    ndTpDef = ndMap[NodeId("DAQModuleObjectType",NS_OpenSCADA_DAQ).toAddr()];
		    rN->setAttr("NodeId", NodeId("DAQ."+nTpDAQ->DAQPath(),NS_OpenSCADA_DAQ).toAddr())->setAttr("name", nTpDAQ->modName())->
			setAttr("typeDefinition", ndTpDef?ndTpDef->attr("typeDefinition"):"");
		}
		else if((nCntr=dynamic_cast<TController*>(ndUp)))
		{
		    ndTpDef = ndMap[NodeId("DAQControllerObjectType",NS_OpenSCADA_DAQ).toAddr()];
		    rN->setAttr("NodeId", NodeId("DAQ."+nCntr->DAQPath(),NS_OpenSCADA_DAQ).toAddr())->setAttr("name", nCntr->name())->
			setAttr("typeDefinition", ndTpDef?ndTpDef->attr("typeDefinition"):"");
		}
		else if((nPrm=dynamic_cast<TParamContr*>(ndUp)))
		{
		    ndTpDef = ndMap[NodeId("DAQParameterObjectType",NS_OpenSCADA_DAQ).toAddr()];
		    rN->setAttr("NodeId", NodeId("DAQ."+nPrm->DAQPath(),NS_OpenSCADA_DAQ).toAddr())->setAttr("name", nPrm->name())->
			setAttr("typeDefinition", ndTpDef?ndTpDef->attr("typeDefinition"):"");
		}
	    }
	    // Forward browse
	    if((!nClass || nClass&NC_Object) && (bd == BD_FORWARD || bd == BD_BOTH))
	    {
		NodeId	tDef, refTpId = OpcUa_Organizes;
		uint32_t nCl = NC_Object;
		vector<string> chLs;
		XML_N prevLs("PrevLs");

		if((nDAQ=dynamic_cast<TDAQS*>(&cNd.at())))
		{
		    nDAQ->modList(chLs);
		    for(unsigned i_ch = 0; i_ch < chLs.size(); i_ch++)
			prevLs.childAdd("ref")->setAttr("NodeId", NodeId("DAQ."+nDAQ->at(chLs[i_ch]).at().DAQPath(),NS_OpenSCADA_DAQ).toAddr())->
			    setAttr("referenceTypeId", refTpId.toAddr())->setAttr("dir", "1")->setAttr("name", nDAQ->at(chLs[i_ch]).at().modName())->
			    setAttr("NodeClass", i2s(nCl))->setAttr("typeDefinition", NodeId(OpcUa_FolderType).toAddr());
		}
		else if((nTpDAQ=dynamic_cast<TTipDAQ*>(&cNd.at())))
		{
		    nTpDAQ->list(chLs);
		    for(unsigned i_ch = 0; i_ch < chLs.size(); i_ch++)
			prevLs.childAdd("ref")->setAttr("NodeId", NodeId("DAQ."+nTpDAQ->at(chLs[i_ch]).at().DAQPath(),NS_OpenSCADA_DAQ).toAddr())->
			    setAttr("referenceTypeId", refTpId.toAddr())->setAttr("dir", "1")->setAttr("name", nTpDAQ->at(chLs[i_ch]).at().name())->
			    setAttr("NodeClass", i2s(nCl))->setAttr("typeDefinition", NodeId("DAQControllerObjectType",NS_OpenSCADA_DAQ).toAddr());
		}
		else if((nCntr=dynamic_cast<TController*>(&cNd.at())))
		{
		    nCntr->list(chLs);
		    for(unsigned i_ch = 0; i_ch < chLs.size(); i_ch++)
			prevLs.childAdd("ref")->setAttr("NodeId", NodeId("DAQ."+nCntr->at(chLs[i_ch]).at().DAQPath(),NS_OpenSCADA_DAQ).toAddr())->
			    setAttr("referenceTypeId", refTpId.toAddr())->setAttr("dir", "1")->setAttr("name", nCntr->at(chLs[i_ch]).at().name())->
			    setAttr("NodeClass", i2s(nCl))->setAttr("typeDefinition", NodeId("DAQParameterObjectType",NS_OpenSCADA_DAQ).toAddr());
		}
		else if((nPrm=dynamic_cast<TParamContr*>(&cNd.at())))
		{
		    nPrm->list(chLs);
		    for(unsigned i_ch = 0; i_ch < chLs.size(); i_ch++)
			prevLs.childAdd("ref")->setAttr("NodeId", NodeId("DAQ."+nPrm->at(chLs[i_ch]).at().DAQPath(),NS_OpenSCADA_DAQ).toAddr())->
			    setAttr("referenceTypeId", refTpId.toAddr())->setAttr("dir", "1")->setAttr("name", nPrm->at(chLs[i_ch]).at().name())->
			    setAttr("NodeClass", i2s(nCl))->setAttr("typeDefinition", NodeId("DAQParameterObjectType",NS_OpenSCADA_DAQ).toAddr());
		    nPrm->vlList(chLs);
		    for(unsigned i_ch = 0; i_ch < chLs.size(); i_ch++)
			prevLs.childAdd("ref")->setAttr("NodeId", NodeId("DAQ."+nPrm->vlAt(chLs[i_ch]).at().DAQPath(),NS_OpenSCADA_DAQ).toAddr())->
			    setAttr("referenceTypeId", NodeId(OpcUa_HasComponent).toAddr())->setAttr("dir", "1")->
			    setAttr("name", nPrm->vlAt(chLs[i_ch]).at().name())->
			    setAttr("NodeClass", i2s(NC_Variable))->setAttr("typeDefinition", NodeId(OpcUa_BaseDataVariableType).toAddr());
		}

		bool lstOK = lstNd.empty() ? true : false;
		for(unsigned i_p = 0; i_p < prevLs.childSize(); i_p++)
		{
		    XML_N *pN = prevLs.childGet(i_p);
		    if(!lstOK) { lstOK = (lstNd==pN->attr("NodeId")); continue; }
		    *(req.childAdd("ref")) = *pN;
		    if(rPn && (int)req.childSize() >= rPn && (i_p+1) < prevLs.childSize())
		    {
			req.setAttr("LastNode", pN->attr("NodeId"));
			break;
		    }
		}
	    }
	    return 0;
	}
	case OpcUa_ReadRequest:
	{
	    TDAQS *nDAQ = NULL;
	    TTipDAQ *nTpDAQ = NULL;
	    TController *nCntr = NULL;
	    TParamContr *nPrm = NULL;
	    TVal *nVal = NULL;

	    NodeId nid = NodeId::fromAddr(req.attr("node"));

	    //Some system nodes process
	    if(nid.ns() == NS_OPC_UA)
	    {
		map<string, XML_N*>::iterator ndX = ndMap.find(nid.toAddr());
		if(ndX != ndMap.end())
		    switch(nid.numbVal())
		    {
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

	    switch(aid)
	    {
		case AId_NodeId: req.setAttr("type", i2s(OpcUa_NodeId))->setText(nid.toAddr());				return 0;
		case AId_BrowseName:
		{
		    req.setAttr("type", i2s(OpcUa_QualifiedName));
		    if((nDAQ=dynamic_cast<TDAQS*>(&cNd.at())))			req.setText(nDAQ->subName());
		    else if((nTpDAQ=dynamic_cast<TTipDAQ*>(&cNd.at())))		req.setText(nTpDAQ->modName());
		    else if((nCntr=dynamic_cast<TController*>(&cNd.at())))	req.setText(nCntr->name());
		    else if((nPrm=dynamic_cast<TParamContr*>(&cNd.at())))	req.setText(nPrm->name());
		    else if((nVal=dynamic_cast<TVal*>(&cNd.at())))		req.setText(nVal->name());
		    else req.setText(cNd.at().nodeName());
		    return 0;
		}
		case AId_InverseName: req.setAttr("type", i2s(OpcUa_LocalizedText))->setText("");			return 0;
		case AId_WriteMask: case AId_UserWriteMask: req.setAttr("type", i2s(OpcUa_UInt32))->setText("0");	return 0;
		default:
		{
		    if((nVal=dynamic_cast<TVal*>(&cNd.at())))
		    {
			if(s2i(req.attr("dtPerGet")))
			    req.setAttr("dtPer", (nVal->arch().freeStat()?"0":r2s(nVal->arch().at().period()*1e-6)));
			//  Variable
			switch(aid)
			{
			    case AId_NodeClass: req.setAttr("type", i2s(OpcUa_Int32))->setText(i2s(NC_Variable));	return 0;
			    case AId_DisplayName: req.setAttr("type", i2s(OpcUa_LocalizedText))->setText(nVal->name());	return 0;
			    case AId_Descr: req.setAttr("type", i2s(OpcUa_String))->setText(nVal->fld().descr());	return 0;
			    case AId_Value:
			    {
				int64_t tm = 0;
				bool dtOK = true;
				switch(nVal->fld().type())
				{
				    case TFld::Boolean:	req.setAttr("type", i2s(OpcUa_Boolean))->setText(nVal->getS(&tm));	break;
				    case TFld::Integer:	req.setAttr("type", i2s(OpcUa_Int32))->setText(nVal->getS(&tm));	break;
				    case TFld::Real:	req.setAttr("type", i2s(OpcUa_Double))->setText(nVal->getS(&tm));	break;
				    case TFld::String:	req.setAttr("type", i2s(OpcUa_String))->setText(nVal->getS(&tm));	break;
				    default: dtOK = false;
				}
				if(dtOK) { if(s2i(req.attr("dtTmGet"))) req.setAttr("dtTm",ll2s(tm)); return 0; }
				break;
			    }
			    case AId_DataType:
				switch(nVal->fld().type())
				{
				    case TFld::Boolean: req.setAttr("type", i2s(OpcUa_NodeId))->setText(i2s(OpcUa_Boolean));	return 0;
				    case TFld::Integer: req.setAttr("type", i2s(OpcUa_NodeId))->setText(i2s(OpcUa_Int32));	return 0;
				    case TFld::Real:    req.setAttr("type", i2s(OpcUa_NodeId))->setText(i2s(OpcUa_Double));	return 0;
				    case TFld::String:  req.setAttr("type", i2s(OpcUa_NodeId))->setText(i2s(OpcUa_String));	return 0;
				    default: break;
				}
				break;
			    case AId_ValueRank: req.setAttr("type", i2s(OpcUa_Int32))->setText("-1");				return 0;
			    case AId_ArrayDimensions: req.setAttr("type", i2s(0x80|OpcUa_Int32))->setText("");			return 0;
			    case AId_AccessLevel: case AId_UserAccessLevel:
				req.setAttr("type", i2s(OpcUa_Byte))->setText(i2s(ACS_Read | (nVal->fld().flg()&TFld::NoWrite ? 0 : ACS_Write)));
				return 0;
			    case AId_MinimumSamplingInterval: req.setAttr("type", i2s(OpcUa_Double))->setText("0");		return 0;
			    case AId_Historizing: req.setAttr("type", i2s(OpcUa_Boolean))->setText("0");			return 0;
			    default: return OpcUa_BadAttributeIdInvalid;
			}
		    }
		    //  Objects
		    else switch(aid)
		    {
			case AId_NodeClass: req.setAttr("type", i2s(OpcUa_Int32))->setText(i2s(NC_Object));			return 0;
			case AId_DisplayName:
			{
			    req.setAttr("type", i2s(OpcUa_LocalizedText));
			    if((nTpDAQ=dynamic_cast<TTipDAQ*>(&cNd.at())))		req.setText(nTpDAQ->modName());
			    else if((nCntr=dynamic_cast<TController*>(&cNd.at())))	req.setText(nCntr->name());
			    else if((nPrm=dynamic_cast<TParamContr*>(&cNd.at())))	req.setText(nPrm->name());
			    else if((nVal=dynamic_cast<TVal*>(&cNd.at())))		req.setText(nVal->name());
			    else req.setText(cNd.at().nodeName());
			    return 0;
			}
			case AId_Descr:
			    req.setAttr("type", i2s(OpcUa_LocalizedText));
			    if((nTpDAQ=dynamic_cast<TTipDAQ*>(&cNd.at())))		req.setText(nTpDAQ->modInfo("Description"));
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
	case OpcUa_WriteRequest:
	{
	    uint32_t rez = Server::EP::reqData(reqTp, req);
	    if(rez != OpcUa_BadNodeIdUnknown) return rez;

	    NodeId nid = NodeId::fromAddr(req.attr("node"));

	    // OpenSCADA DAQ parameter's attribute
	    if(nid.ns() != NS_OpenSCADA_DAQ)	return OpcUa_BadNodeIdUnknown;
	    uint32_t aid = s2i(req.attr("aid"));

	    // Connect to DAQ node
	    int addrOff = 0;
	    string firstEl = TSYS::strParse(nid.strVal(),0,".",&addrOff);
	    AutoHD<TCntrNode> cNd = SYS->daq().at().daqAt(nid.strVal().substr(addrOff), '.', true);
	    if(cNd.freeStat()) return OpcUa_BadNodeIdUnknown;

	    if(aid != AId_Value || !dynamic_cast<TVal*>(&cNd.at())) return OpcUa_BadNothingToDo;
	    ((AutoHD<TVal>)cNd).at().setS(req.text());

	    return 0;
	}
    }

    return OpcUa_BadNodeIdUnknown;
}

void OPCEndPoint::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info")
    {
	TCntrNode::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("End point: ")+name(),RWRWR_,"root",SPRT_ID);
	if(ctrMkNode("area",opt,-1,"/ep",_("End point")))
	{
	    if(ctrMkNode("area",opt,-1,"/ep/st",_("State")))
	    {
		ctrMkNode("fld",opt,-1,"/ep/st/status",_("Status"),R_R_R_,"root",SPRT_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/ep/st/en_st",_("Enable"),RWRWR_,"root",SPRT_ID,1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/ep/st/db",_("DB"),RWRWR_,"root",SPRT_ID,4,
		    "tp","str","dest","select","select","/db/list","help",TMess::labDB());
	    }
	    if(ctrMkNode("area",opt,-1,"/ep/cfg",_("Configuration")))
	    {
		TConfig::cntrCmdMake(opt,"/ep/cfg",0,"root",SPRT_ID,RWRWR_);
		ctrMkNode("fld",opt,-1,"/ep/cfg/ServCert",cfg("ServCert").fld().descr(),RWRW__,"root",SPRT_ID,3,"tp","str","cols","90","rows","7");
		ctrMkNode("fld",opt,-1,"/ep/cfg/ServPvKey",cfg("ServPvKey").fld().descr(),RWRW__,"root",SPRT_ID,3,"tp","str","cols","90","rows","7");
		ctrRemoveNode(opt,"/ep/cfg/SecPolicies");
		if(ctrMkNode("table",opt,-1,"/ep/cfg/secPlc",cfg("SecPolicies").fld().descr(),RWRWR_,"root",SPRT_ID,1,"s_com","add,del"))
		{
		    ctrMkNode("list",opt,-1,"/ep/cfg/secPlc/0",_("Policy"),RWRWR_,"root",SPRT_ID,3,"tp","str","dest","select","sel_list","None;Basic128Rsa15;Basic256");
		    ctrMkNode("list",opt,-1,"/ep/cfg/secPlc/1",_("Message mode"),RWRWR_,"root",SPRT_ID,4,"tp","dec","dest","select","sel_id","1;2;3","sel_list",_("None;Sign;Sign&Encrypt"));
		}
	    }
	}
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/ep/st/status" && ctrChkNode(opt))	opt->setText(getStatus());
    else if(a_path == "/ep/st/en_st")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD))	opt->setText(enableStat()?"1":"0");
	if(ctrChkNode(opt,"set",RWRWR_,"root",SPRT_ID,SEC_WR))	setEnable(s2i(opt->text()));
    }
    else if(a_path == "/ep/st/db")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD))	opt->setText(DB());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SPRT_ID,SEC_WR))	setDB(opt->text());
    }
    else if(a_path == "/ep/cfg/ls_itr" && ctrChkNode(opt))
    {
	opt->childAdd("el")->setText("*");
	vector<string> sls;
	SYS->transport().at().inTrList(sls);
	for(unsigned i_s = 0; i_s < sls.size(); i_s++)
	    opt->childAdd("el")->setText(sls[i_s]);
    }
    else if(a_path == "/ep/cfg/secPlc")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD))
	{
	    XMLNode *n_pol	= ctrMkNode("list",opt,-1,"/ep/cfg/secPlc/0","",RWRWR_);
	    XMLNode *n_mm	= ctrMkNode("list",opt,-1,"/ep/cfg/secPlc/1","",RWRWR_);
	    ResAlloc res(nodeRes(), false);
	    for(unsigned i_p = 0; i_p < mSec.size(); i_p++)
	    {
		if(n_pol) n_pol->childAdd("el")->setText(mSec[i_p].policy);
		if(n_mm)  n_mm->childAdd("el")->setText(i2s(mSec[i_p].messageMode));
	    }
	    return;
	}
	ResAlloc res(nodeRes(), true);
	if(ctrChkNode(opt,"add",RWRWR_,"root",SPRT_ID,SEC_WR))
	{ mSec.push_back(UA::SecuritySetting("None",MS_None)); modif(); return; }
	int row = s2i(opt->attr("row"));
	if(row < 0 || row >= (int)mSec.size())
	    throw TError(nodePath().c_str(),_("No present selected row."));
	if(ctrChkNode(opt,"del",RWRWR_,"root",SPRT_ID,SEC_WR))
	{ mSec.erase(mSec.begin()+row); modif(); return; }
	if(ctrChkNode(opt,"set",RWRWR_,"root",SPRT_ID,SEC_WR))
	{
	    int col = s2i(opt->attr("col"));
	    if(col == 0)	mSec[row].policy = opt->text();
	    else if(col == 1)	mSec[row].messageMode = (MessageSecurityMode)s2i(opt->text());
	    modif();
	}
    }
    else if(a_path.compare(0,7,"/ep/cfg") == 0) TConfig::cntrCmdProc(opt, TSYS::pathLev(a_path,2), "root", SPRT_ID, RWRWR_);
    else TCntrNode::cntrCmdProc(opt);
}
