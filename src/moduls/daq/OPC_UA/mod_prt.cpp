
//OpenSCADA system module DAQ.OPC_UA file: mod_prt.cpp
/***************************************************************************
 *   Copyright (C) 2009-2013 by Roman Savochenko                           *
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

    //> Node DB structure
    mEndPntEl.fldAdd(new TFld("ID",_("ID"),TFld::String,TCfg::Key|TFld::NoWrite,"20"));
    mEndPntEl.fldAdd(new TFld("NAME",_("Name"),TFld::String,TCfg::TransltText,"50"));
    mEndPntEl.fldAdd(new TFld("DESCR",_("Description"),TFld::String,TFld::FullText|TCfg::TransltText,"300"));
    mEndPntEl.fldAdd(new TFld("EN",_("To enable"),TFld::Boolean,0,"1","0"));
    mEndPntEl.fldAdd(new TFld("SerialzType",_("Serializer type"),TFld::Integer,TFld::Selected,"1","0","0",_("Binary")));
    mEndPntEl.fldAdd(new TFld("URL",_("URL"),TFld::String,0,"50","opc.tcp://localhost:4841"));
    mEndPntEl.fldAdd(new TFld("SecPolicies",_("Security policies"),TFld::String,TFld::FullText,"100","None:0\nBasic128Rsa15:1"));
    mEndPntEl.fldAdd(new TFld("ServCert",_("Server certificate (PEM)"),TFld::String,TFld::FullText,"10000"));
    mEndPntEl.fldAdd(new TFld("ServPvKey",_("Server private key (PEM)"),TFld::String,TFld::FullText,"10000"));
}

TProt::~TProt()
{
    nodeDelAll();
}

string TProt::applicationUri( )		{ return SYS->host()+"/OpenSCADA/DAQ.OPC_UA"; }

string TProt::productUri( )		{ return PACKAGE_SITE; }

string TProt::applicationName( )	{ return "OpenSCADA.OPC-UA Server"; }

void TProt::epAdd( const string &iid, const string &db )
{
    chldAdd(mEndPnt, new OPCEndPoint(iid,db,&endPntEl()));
}

bool TProt::debug( )	{ return (mess_lev()==TMess::Debug); }

void TProt::debugMess( const string &mess, const string &data )
{
    mess_debug(nodePath().c_str(), "%s: %s", mess.c_str(), TSYS::strDecode(data,TSYS::Bin).c_str());
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

bool TProt::inReq( string &request, string &answer, const string &sender )
{
    ResAlloc res(en_res, false);
    return Server::inReq(request, answer, sender);
}

void TProt::discoveryUrls( vector<string> &ls )
{
    ls.clear();
    //>> Get allowed enpoints list
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
    ResAlloc res(en_res, true);

    unsigned i_ep;
    for(i_ep = 0; i_ep < ep_hd.size(); i_ep++)
	if(ep_hd[i_ep].at().id() == id) break;

    if(val && i_ep >= ep_hd.size()) ep_hd.push_back(epAt(id));
    if(!val && i_ep < ep_hd.size()) ep_hd.erase(ep_hd.begin()+i_ep);
}

void TProt::load_( )
{
    //> Load DB
    try
    {
	TConfig g_cfg(&endPntEl());
	g_cfg.cfgViewAll(false);
	vector<string> db_ls;
	map<string, bool> itReg;

	//>> Search into DB
	SYS->db().at().dbList(db_ls,true);
	db_ls.push_back("<cfg>");
	for(unsigned i_db = 0; i_db < db_ls.size(); i_db++)
	    for(int fld_cnt = 0; SYS->db().at().dataSeek(db_ls[i_db]+"."+modId()+"_ep",nodePath()+modId()+"_ep",fld_cnt++,g_cfg); )
	    {
		string id = g_cfg.cfg("ID").getS();
		if(!epPresent(id)) epAdd(id,(db_ls[i_db]==SYS->workDB())?"*.*":db_ls[i_db]);
		itReg[id] = true;
	    }

	//>>> Check for remove items removed from DB
	if(!SYS->selDB().empty())
	{
	    epList(db_ls);
	    for(unsigned i_it = 0; i_it < db_ls.size(); i_it++)
		if(itReg.find(db_ls[i_it]) == itReg.end() && SYS->chkSelDB(epAt(db_ls[i_it]).at().DB()))
		    epDel(db_ls[i_it]);
	}
    }catch(TError err)
    {
	mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	mess_err(nodePath().c_str(),_("Search and create new server error."));
    }
}

void TProt::save_( )
{

}

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

TProtocolIn *TProt::in_open( const string &name )
{
    return new TProtIn(name);
}

void TProt::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if(opt->name() == "info")
    {
	TProtocol::cntrCmdProc(opt);
	ctrMkNode("grp", opt, -1, "/br/ep_", _("End point"), RWRWR_, "root", SPRT_ID, 2, "idm","1", "idSz","20");
	if(ctrMkNode("area",opt,0,"/ep",_("End points")))
	    ctrMkNode("list", opt, -1, "/ep/ep", _("End points"), RWRWR_, "root", SPRT_ID, 5,
		"tp","br", "idm","1", "s_com","add,del", "br_pref","ep_", "idSz","20");
	return;
    }

    //> Process command to page
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
TProtIn::TProtIn( string name ) : TProtocolIn(name)
{

}

TProtIn::~TProtIn( )
{

}

TProt &TProtIn::owner( )	{ return *(TProt*)nodePrev(); }

bool TProtIn::mess( const string &reqst, string &answ, const string &sender )
{
    mBuf += reqst;
    return owner().inReq(mBuf, answ, sender);
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

    //> Copy parameters
    exclCopy(*src_n, "ID;");
    setDB(src_n->DB());

    return *this;
}

void OPCEndPoint::postDisable( int flag )
{
    try
    {
	if(flag) SYS->db().at().dataDel(fullDB(), owner().nodePath()+tbl(), *this, true);
    }
    catch(TError err) { mess_err(err.cat.c_str(), "%s", err.mess.c_str()); }
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

string OPCEndPoint::secPolicy( int isec )
{
    ResAlloc res(nodeRes(), false);
    return Server::EP::secPolicy(isec);
}

MessageSecurityMode OPCEndPoint::secMessageMode( int isec )
{
    ResAlloc res(nodeRes(), false);
    return Server::EP::secMessageMode(isec);
}

int OPCEndPoint::sessCreate( const string &iName, double iTInact )
{
    ResAlloc res(nodeRes(), true);
    return Server::EP::sessCreate(iName, iTInact);
}

void OPCEndPoint::sessServNonceSet( int sid, const string &servNonce )
{
    ResAlloc res(nodeRes(), false);
    return Server::EP::sessServNonceSet(sid, servNonce);
}

bool OPCEndPoint::sessActivate( int sid, uint32_t secCnl, bool check )
{
    ResAlloc res( nodeRes(), true );
    return Server::EP::sessActivate(sid, secCnl, check);
}

void OPCEndPoint::sessClose( int sid )
{
    ResAlloc res(nodeRes(), true);
    return Server::EP::sessClose(sid);
}

Server::Sess OPCEndPoint::sessGet( int sid )
{
    ResAlloc res(nodeRes(), false);
    return Server::EP::sessGet(sid);
}

bool OPCEndPoint::cfgChange( TCfg &ce )
{
    modif();
    return true;
}

void OPCEndPoint::load_( )
{
    if(!SYS->chkSelDB(DB())) return;
    cfgViewAll(true);
    SYS->db().at().dataGet(fullDB(), owner().nodePath()+tbl(), *this);

    //Security policies parse
    string sp = cfg("SecPolicies").getS();
    string spi;
    ResAlloc res(nodeRes(), true);
    mSec.clear();
    for(int off = 0; (spi=TSYS::strParse(sp,0,"\n",&off)).size(); )
	mSec.push_back(UA::SecuritySetting(TSYS::strParse(spi,0,":"),atoi(TSYS::strParse(spi,1,":").c_str())));
}

void OPCEndPoint::save_( )
{
    //Security policies store
    string sp;
    ResAlloc res(nodeRes(), false);
    for(unsigned i_p = 0; i_p < mSec.size(); i_p++)
	sp += mSec[i_p].policy + ":" + TSYS::int2str(mSec[i_p].messageMode)+"\n";
    cfg("SecPolicies").setS(sp);

    SYS->db().at().dataSet(fullDB(),owner().nodePath()+tbl(),*this);
}

void OPCEndPoint::setEnable( bool vl )
{
    if(vl == enableStat()) return;
    owner().epEn(id(), vl);
    Server::EP::setEnable(vl);
    if(vl)
	nodeReg(OpcUa_ObjectsFolder,NodeId(SYS->daq().at().subId(),1),SYS->daq().at().subId(),NC_Object,OpcUa_Organizes,OpcUa_FolderType)->setAttr("DisplayName",SYS->daq().at().subName());
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

string OPCEndPoint::reqData( int reqTp, const string &rb )
{
    string respEp;
    int off = 0;
    switch(reqTp)
    {
	case OpcUa_BrowseRequest:
	{
	    //>> Request
						//> view
	    UA::iNodeId(rb, off);		//viewId
	    UA::iTm(rb, off);			//timestamp
	    UA::iNu(rb, off, 4);		//viewVersion

	    unsigned rPn = UA::iNu(rb, off, 4);	//requestedMax ReferencesPerNode
						//> nodesToBrowse
	    uint32_t nc = UA::iNu(rb, off, 4);	//Nodes

	    //>> Respond
						//> results []
	    UA::oNu(respEp, nc, 4);		//Nodes

	    //>> Process limit for "rPn"
	    //????

	    //>>> Nodes list processing
	    for(uint32_t i_c = 0; i_c < nc; i_c++)
	    {
		NodeId nid = UA::iNodeId(rb, off);	//nodeId
		uint32_t bd = UA::iNu(rb, off, 4);	//browseDirection
		NodeId rtId = UA::iNodeId(rb, off);	//referenceTypeId
		UA::iNu(rb, off, 1);			//includeSubtypes
		uint32_t nClass = UA::iNu(rb, off, 4);	//nodeClassMask
		uint32_t resMask = UA::iNu(rb, off, 4);	//resultMask

		uint32_t stCode = 0, refNumb = 0;
		int stCodeOff = respEp.size(); UA::oNu(respEp, stCode, 4);	//statusCode
		UA::oS(respEp, "");						//continuationPoint ????
		int refNumbOff = respEp.size(); UA::oNu(respEp, refNumb, 4);	//References [] = 0

		if(rtId.numbVal() != OpcUa_HierarchicalReferences && rtId.numbVal() != OpcUa_References) continue;

		map<string, XML_N*>::iterator ndX = ndMap.find(nid.toAddr());
		if(ndX == ndMap.end())
		{
		    stCode = OpcUa_BadBrowseNameInvalid;
		    if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "Browse request to unknown node: %s", nid.toAddr().c_str());
		}
		else
		{
		    //> typeDefinition reference process
		    if(rtId.numbVal() == OpcUa_References && (bd == BD_FORWARD || bd == BD_BOTH))
		    {
			map<string, XML_N*>::iterator ndTpDef = ndMap.find(ndX->second->attr("typeDefinition"));
			if(ndTpDef != ndMap.end())
			{
			    unsigned cnClass = atoi(ndTpDef->second->attr("NodeClass").c_str());
			    if(!nClass || nClass == cnClass)
			    {
				UA::oRef(respEp, resMask, NodeId::fromAddr(ndTpDef->second->attr("NodeId")),
				    NodeId::fromAddr(ndTpDef->second->attr("referenceTypeId")), 1,
				    ndTpDef->second->attr("name"), cnClass,
				    NodeId::fromAddr(ndTpDef->second->attr("typeDefinition")));
				refNumb++;
			    }
			}
		    }
		    //> Forward hierarchical references process
		    for(unsigned i_ch = 0; (bd == BD_FORWARD || bd == BD_BOTH) && i_ch < ndX->second->childSize(); i_ch++)
		    {
			XML_N *chNd = ndX->second->childGet(i_ch);
			unsigned cnClass = atoi(chNd->attr("NodeClass").c_str());
			if(nClass && nClass != cnClass) continue;
			UA::oRef(respEp, resMask, NodeId::fromAddr(chNd->attr("NodeId")),
			    NodeId::fromAddr(chNd->attr("referenceTypeId")), 1, chNd->attr("name"), cnClass,
			    NodeId::fromAddr(chNd->attr("typeDefinition")));
			refNumb++;
		    }
		    //> Inverse hierarchical references process
		    if((bd == BD_INVERSE || bd == BD_BOTH) && ndX->second->parent())
		    {
			XML_N *chNd = ndX->second->parent();
			unsigned cnClass = atoi(chNd->attr("NodeClass").c_str());
			if(!nClass || nClass == cnClass)
			{
			    UA::oRef(respEp, resMask, NodeId::fromAddr(chNd->attr("NodeId")),
				NodeId::fromAddr(chNd->attr("referenceTypeId")), 0, chNd->attr("name"), cnClass,
				NodeId::fromAddr(chNd->attr("typeDefinition")));
			    refNumb++;
			}
		    }
		}
		//> Check for DAQ subsystem data
		if(nid.ns() == 1 && TSYS::strParse(nid.strVal(),0,".") == SYS->daq().at().subId())
		{
		    vector<string> chLs;
		    stCode = 0;
		    //>> Connect to DAQ node
		    AutoHD<TCntrNode> cNd = SYS->daq();
		    string sel;
		    int nLev = 0;
		    for(int off = 0; (sel=TSYS::strParse(nid.strVal(),off?0:1,".",&off)).size(); nLev++)
			try { cNd = cNd.at().nodeAt(sel); }
			catch(TError err) { stCode = OpcUa_BadBrowseNameInvalid; break; }
		    if(!stCode)
		    {
			//> typeDefinition reference browse
			if(nLev && rtId.numbVal() == OpcUa_References && (bd == BD_FORWARD || bd == BD_BOTH))
			{
			    map<string, XML_N*>::iterator ndTpDef;
			    switch(nLev)
			    {
				case 1:	ndTpDef = ndMap.find(NodeId("DAQModuleObjectType",1).toAddr());		break;
				case 2:	ndTpDef = ndMap.find(NodeId("DAQControllerObjectType",1).toAddr());	break;
				case 3:	ndTpDef = ndMap.find(NodeId("DAQParameterObjectType",1).toAddr());	break;
				case 4:	ndTpDef = ndMap.find(NodeId(OpcUa_BaseDataVariableType).toAddr());	break;
			    }
			    if(ndTpDef != ndMap.end())
			    {
				unsigned cnClass = atoi(ndTpDef->second->attr("NodeClass").c_str());
				if(!nClass || nClass == cnClass)
				{
				    UA::oRef(respEp, resMask, NodeId::fromAddr(ndTpDef->second->attr("NodeId")),
					NodeId::fromAddr(ndTpDef->second->attr("referenceTypeId")), 1,
					ndTpDef->second->attr("name"), cnClass,
					NodeId::fromAddr(ndTpDef->second->attr("typeDefinition")));
				    refNumb++;
				}
			    }
			}
			//>> Forward browse
			if((!nClass || nClass == NC_Object) && (bd == BD_FORWARD || bd == BD_BOTH))
			{
			    switch(nLev)
			    {
				case 0:		//>>> Subsystem
				    ((AutoHD<TDAQS>)cNd).at().modList(chLs);
				    for(unsigned i_ch = 0; i_ch < chLs.size(); i_ch++, refNumb++)
					UA::oRef(respEp, resMask, NodeId(nid.strVal()+"."+chLs[i_ch],1), OpcUa_Organizes,
					    true, chLs[i_ch], NC_Object, NodeId("DAQModuleObjectType",1));
				    break;
				case 1:		//>>> Module
				    ((AutoHD<TTipDAQ>)cNd).at().list(chLs);
				    for(unsigned i_ch = 0; i_ch < chLs.size(); i_ch++, refNumb++)
					UA::oRef(respEp, resMask, NodeId(nid.strVal()+"."+chLs[i_ch],1), OpcUa_Organizes,
					    true, chLs[i_ch], NC_Object, NodeId("DAQControllerObjectType",1));
				    break;
				case 2:		//>>> Controller
				    ((AutoHD<TController>)cNd).at().list(chLs);
				    for(unsigned i_ch = 0; i_ch < chLs.size(); i_ch++, refNumb++)
					UA::oRef(respEp, resMask, NodeId(nid.strVal()+"."+chLs[i_ch],1), OpcUa_Organizes,
					    true, chLs[i_ch], NC_Object, NodeId("DAQParameterObjectType",1));
				    break;
				case 3:		//>>> Parameter
				    ((AutoHD<TParamContr>)cNd).at().vlList(chLs);
				    for(unsigned i_ch = 0; i_ch < chLs.size(); i_ch++, refNumb++)
					UA::oRef(respEp, resMask, NodeId(nid.strVal()+"."+chLs[i_ch],1), OpcUa_HasComponent,
					    true, chLs[i_ch], NC_Variable, OpcUa_BaseDataVariableType);
				    break;
			    }
			}
			//>> Inverse browse
			if((!nClass || nClass == NC_Object) && (bd == BD_INVERSE || bd == BD_BOTH) && nid.strVal() != "DAQ")
			{
			    UA::oRef(respEp, resMask, NodeId(nid.strVal().substr(0,nid.strVal().rfind(".")),1), OpcUa_Organizes,
				false, TSYS::strParse(nid.strVal(),nLev,"."), NC_Object, OpcUa_FolderType);
			    refNumb++;
			}
		    }
		}
		if(stCode)	UA::oNu(respEp, stCode, 4, stCodeOff);
		if(refNumb)	UA::oNu(respEp, refNumb, 4, refNumbOff);
	    }
	    UA::oS(respEp, "");		//diagnosticInfos []
	    break;
	}
	case OpcUa_ReadRequest:
	{
	    //>> Request
	    UA::iR(rb, off, 8);				//maxAge
	    uint32_t tmStRet = UA::iNu(rb, off, 4);	//timestampsTo Return
							//> nodesToRead []
	    uint32_t nc = UA::iNu(rb, off, 4);		//nodes
	    uint8_t eMsk = 0x01;
	    switch(tmStRet)
	    {
		case TS_SOURCE:	eMsk |= 0x04;	break;
		case TS_SERVER:	eMsk |= 0x08;	break;
		case TS_BOTH:	eMsk |= 0x0C;	break;
	    }

	    //>> Respond
	    UA::oNu(respEp, nc, 4);			//Numbers

	    //>>> Nodes list processing
	    for(uint32_t i_c = 0; i_c < nc; i_c++)
	    {
		NodeId nid = UA::iNodeId(rb, off);	//nodeId
		uint32_t aid = UA::iNu(rb, off, 4);	//attributeId
		UA::iS(rb, off);			//indexRange
		UA::iSqlf(rb, off);			//dataEncoding

		//> Get node from objects tree
		map<string, XML_N*>::iterator ndX = ndMap.find(nid.toAddr());
		if(ndX != ndMap.end())
		{
		    switch(aid)
		    {
			case AId_NodeId: modPrt->oDataValue(respEp, eMsk, nid.toAddr(), OpcUa_NodeId);				continue;
			case AId_NodeClass: modPrt->oDataValue(respEp, eMsk, ndX->second->attr("NodeClass"), OpcUa_Int32);	continue;
			case AId_BrowseName: modPrt->oDataValue(respEp, eMsk, ndX->second->attr("name"), OpcUa_QualifiedName);	continue;
			case AId_DisplayName:
			    modPrt->oDataValue(respEp, eMsk, ndX->second->attr(ndX->second->attr("DisplayName").empty()?"name":"DisplayName"),OpcUa_LocalizedText);
			    continue;
			case AId_Descr: modPrt->oDataValue(respEp, eMsk, ndX->second->attr("Descr"), OpcUa_LocalizedText);	continue;
			case AId_WriteMask: case AId_UserWriteMask: modPrt->oDataValue(respEp, eMsk, 0, OpcUa_UInt32);		continue;
			case AId_IsAbstract:
			    modPrt->oDataValue(respEp, eMsk, atoi(ndX->second->attr("IsAbstract").c_str()), OpcUa_Boolean);
			    continue;
			case AId_Symmetric:
			    modPrt->oDataValue(respEp, eMsk, atoi(ndX->second->attr("Symmetric").c_str()), OpcUa_Boolean);
			    continue;
			case AId_InverseName:
			    modPrt->oDataValue(respEp, eMsk, ndX->second->attr("InverseName"), OpcUa_LocalizedText);
			    continue;
			case AId_EventNotifier:
			    modPrt->oDataValue(respEp, eMsk, atoi(ndX->second->attr("EventNotifier").c_str()), OpcUa_Byte);
			    continue;
			default:
			{
			    string dtType = ndX->second->attr("DataType");
			    if(dtType.empty())	break;
			    switch(aid)
			    {
				case AId_Value: modPrt->oDataValue(respEp, eMsk, ndX->second->attr("Value"), atoi(dtType.c_str()));	continue;
				case AId_DataType: modPrt->oDataValue(respEp, eMsk, (atoi(dtType.c_str())&(~0x80)), OpcUa_NodeId);	continue;
				case AId_ValueRank:
				{
				    string val = ndX->second->attr("ValueRank");
				    modPrt->oDataValue(respEp, eMsk, val.empty() ? -1 : atoi(val.c_str()), OpcUa_Int32);
				    continue;
				}
				case AId_ArrayDimensions:
				{
				    string val = ndX->second->attr("Value");
				    int cnt = 0;
				    if(atoi(dtType.c_str())&0x80)
					for(int off = 0; TSYS::strLine(val, 0, &off).size(); cnt++) ;
				    modPrt->oDataValue(respEp, eMsk, cnt, 0x80|OpcUa_Int32);
				    continue;
				}
				case AId_AccessLevel:
				{
				    string val = ndX->second->attr("AccessLevel");
				    modPrt->oDataValue(respEp, eMsk, (val.empty() ? ACS_Read : atoi(val.c_str())), OpcUa_Byte);
				    continue;
				}
				case AId_UserAccessLevel:
				{
				    string val = ndX->second->attr("UserAccessLevel");
				    modPrt->oDataValue(respEp, eMsk, (val.empty() ? ACS_Read|ACS_Write : atoi(val.c_str())), OpcUa_Byte);
				    continue;
				}
				case AId_MinimumSamplingInterval:
				{
				    string val = ndX->second->attr("MinimumSamplingInterval");
				    modPrt->oDataValue(respEp, eMsk, val.empty() ? -1 : atoi(val.c_str()), OpcUa_Double);
				    continue;
				}
				case AId_Historizing:
				    modPrt->oDataValue(respEp, eMsk, atoi(ndX->second->attr("Historizing").c_str()), OpcUa_Boolean);
				    continue;
			    }
			}
		    }
		    modPrt->oDataValue(respEp, 0x02, (int)OpcUa_BadAttributeIdInvalid);
		    continue;
		}

		//OpenSCADA DAQ parameter's attribute
		if(nid.ns() == 1)
		{
		    //>> Connect to DAQ node
		    AutoHD<TCntrNode> cNd = SYS->daq();
		    string sel;
		    int nLev = 0;
		    for(int off = 0; (sel=TSYS::strParse(nid.strVal(),off?0:1,".",&off)).size(); nLev++)
			try { cNd = cNd.at().nodeAt(sel); } catch(TError err) { break; }

		    if(sel.empty())
		    {
			switch(aid)
			{
			    case AId_NodeId: modPrt->oDataValue(respEp, eMsk, nid.toAddr(), OpcUa_NodeId);	break;
			    case AId_BrowseName: modPrt->oDataValue(respEp, eMsk, cNd.at().nodeName(), OpcUa_QualifiedName);	break;
			    case AId_InverseName: modPrt->oDataValue(respEp, eMsk, "", OpcUa_LocalizedText);	break;
			    case AId_WriteMask: case AId_UserWriteMask: modPrt->oDataValue(respEp, eMsk, 0, OpcUa_UInt32);	break;
			    default:
				//>>> Variable
				if(dynamic_cast<TVal*>(&cNd.at()))
				    switch(aid)
				    {
					case AId_NodeClass: modPrt->oDataValue(respEp, eMsk, NC_Variable, OpcUa_Int32);	break;
					case AId_DisplayName: modPrt->oDataValue(respEp, eMsk, cNd.at().nodeName(), OpcUa_LocalizedText);	break;
					case AId_Descr: modPrt->oDataValue(respEp, eMsk, ((AutoHD<TVal>)cNd).at().fld().descr(), OpcUa_String);	break;
					case AId_Value:
					    switch(((AutoHD<TVal>)cNd).at().fld().type())
					    {
						case TFld::Boolean:
						    modPrt->oDataValue(respEp, eMsk, ((AutoHD<TVal>)cNd).at().getB(), OpcUa_Boolean);
						    break;
						case TFld::Integer:
						    modPrt->oDataValue(respEp, eMsk, ((AutoHD<TVal>)cNd).at().getI(), OpcUa_Int32);
						    break;
						case TFld::Real:
						    modPrt->oDataValue(respEp, eMsk, ((AutoHD<TVal>)cNd).at().getR(), OpcUa_Double);
						    break;
						case TFld::String:
						    modPrt->oDataValue(respEp, eMsk, ((AutoHD<TVal>)cNd).at().getS(), OpcUa_String);
						    break;
						default: break;
					    }
					    break;
					case AId_DataType:
					{
					    NodeId dt;
					    switch(((AutoHD<TVal>)cNd).at().fld().type())
					    {
						case TFld::Boolean: dt.setNumbVal(OpcUa_Boolean);	break;
						case TFld::Integer: dt.setNumbVal(OpcUa_Int32);		break;
						case TFld::Real:    dt.setNumbVal(OpcUa_Double);	break;
						case TFld::String:  dt.setNumbVal(OpcUa_String);	break;
						default: break;
					    }
					    modPrt->oDataValue(respEp, eMsk, dt.toAddr(), OpcUa_NodeId);
					    break;
					}
					case AId_ValueRank: modPrt->oDataValue(respEp, eMsk, -1, OpcUa_Int32);			break;
					case AId_ArrayDimensions: modPrt->oDataValue(respEp, eMsk, "", 0x80|OpcUa_Int32);	break;
					case AId_AccessLevel: case AId_UserAccessLevel:
					    modPrt->oDataValue(respEp, eMsk, ACS_Read | (((AutoHD<TVal>)cNd).at().fld().flg()&TFld::NoWrite ? 0 : ACS_Write), OpcUa_Byte);
					    break;
					case AId_MinimumSamplingInterval: modPrt->oDataValue(respEp, eMsk, 0, OpcUa_Double);	break;
					case AId_Historizing: modPrt->oDataValue(respEp, eMsk, false, OpcUa_Boolean);		break;
					default: modPrt->oDataValue(respEp, 0x02, (int)OpcUa_BadAttributeIdInvalid);
				    }
				//>>> Objects
				else
				    switch(aid)
				    {
					case AId_NodeClass: modPrt->oDataValue(respEp, eMsk, NC_Object, OpcUa_Int32);	break;
					case AId_DisplayName:
					    if(dynamic_cast<TModule*>(&cNd.at()))
						modPrt->oDataValue(respEp, eMsk, ((AutoHD<TModule>)cNd).at().modName(), OpcUa_LocalizedText);
					    else if(dynamic_cast<TController*>(&cNd.at()))
						modPrt->oDataValue(respEp, eMsk, ((AutoHD<TController>)cNd).at().name(), OpcUa_LocalizedText);
					    else if(dynamic_cast<TParamContr*>(&cNd.at()))
						modPrt->oDataValue(respEp, eMsk, ((AutoHD<TParamContr>)cNd).at().name(), OpcUa_LocalizedText);
					    else modPrt->oDataValue(respEp, eMsk, cNd.at().nodeName(), OpcUa_LocalizedText);
					    break;
					case AId_Descr:
					    if(dynamic_cast<TModule*>(&cNd.at()))
						modPrt->oDataValue(respEp, eMsk, ((AutoHD<TModule>)cNd).at().modInfo("Description"), OpcUa_LocalizedText);
					    else if(dynamic_cast<TController*>(&cNd.at()))
						modPrt->oDataValue(respEp, eMsk, ((AutoHD<TController>)cNd).at().descr(), OpcUa_LocalizedText);
					    else if(dynamic_cast<TParamContr*>(&cNd.at()))
						modPrt->oDataValue(respEp, eMsk, ((AutoHD<TParamContr>)cNd).at().descr(), OpcUa_LocalizedText);
					    else modPrt->oDataValue(respEp, 0x02, (int)OpcUa_BadAttributeIdInvalid);
					    break;
					case AId_EventNotifier: modPrt->oDataValue(respEp, eMsk, 0, OpcUa_Byte);	break;
					default: modPrt->oDataValue(respEp, 0x02, (int)OpcUa_BadAttributeIdInvalid);
				    }
			}
			continue;
		    }
		}

		modPrt->oDataValue(respEp, 0x02, (int)OpcUa_BadNodeIdUnknown);
	    }
	    UA::oS(respEp, "");		//diagnosticInfos []
	    break;
	}
	case OpcUa_WriteRequest:
	{
	    //>> Request
							//> nodesToWrite []
	    uint32_t nc = UA::iNu(rb, off, 4);		//nodes

	    //>> Respond
	    UA::oNu(respEp, nc, 4);			//Numbers
	    for(unsigned i_n = 0; i_n < nc; i_n++)
	    {
		uint32_t rezSt = 0;
		NodeId nid = UA::iNodeId(rb, off);	//nodeId
		uint32_t aid = UA::iNu(rb, off, 4);	//attributeId (Value)
		UA::iS(rb, off);			//indexRange
		XML_N nVal;
		UA::iDataValue(rb, off, nVal);		//value

		//> Get node from objects tree
		map<string,XML_N*>::iterator ndX = ndMap.find(nid.toAddr());
		if(ndX != ndMap.end()) rezSt = OpcUa_BadNothingToDo;
		else if(nid.ns() == 1)
		{
		    //>> Connect to DAQ node
		    AutoHD<TCntrNode> cNd = SYS->daq();
		    string sel;
		    int nLev = 0;
		    for(int off = 0; (sel=TSYS::strParse(nid.strVal(),off?0:1,".",&off)).size(); nLev++)
			try { cNd = cNd.at().nodeAt(sel); } catch(TError err) { break; }

		    if(!sel.empty())	rezSt = OpcUa_BadNodeIdUnknown;
		    else if(aid != AId_Value || !dynamic_cast<TVal*>(&cNd.at())) rezSt = OpcUa_BadNothingToDo;
		    else ((AutoHD<TVal>)cNd).at().setS(nVal.text());
		}

		//>>> Write result status code
		UA::oNu(respEp, rezSt, 4);		//StatusCode
	    }
	    UA::oS(respEp, "");		//diagnosticInfos []
	    break;
	}
    }

    cntReq++;

    return respEp;
}

void OPCEndPoint::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
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
    //> Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/ep/st/status" && ctrChkNode(opt))	opt->setText(getStatus());
    else if(a_path == "/ep/st/en_st")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD))	opt->setText(enableStat()?"1":"0");
	if(ctrChkNode(opt,"set",RWRWR_,"root",SPRT_ID,SEC_WR))	setEnable(atoi(opt->text().c_str()));
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
		if(n_mm)  n_mm->childAdd("el")->setText(TSYS::int2str(mSec[i_p].messageMode));
	    }
	    return;
	}
	ResAlloc res(nodeRes(), true);
	if(ctrChkNode(opt,"add",RWRWR_,"root",SPRT_ID,SEC_WR))
	{ mSec.push_back(UA::SecuritySetting("None",MS_None)); modif(); return; }
	int row = atoi(opt->attr("row").c_str());
	if(row < 0 || row >= (int)mSec.size())
	    throw TError(nodePath().c_str(),_("No present selected row."));
	if(ctrChkNode(opt,"del",RWRWR_,"root",SPRT_ID,SEC_WR))
	{ mSec.erase(mSec.begin()+row); modif(); return; }
	if(ctrChkNode(opt,"set",RWRWR_,"root",SPRT_ID,SEC_WR))
	{
	    int col = atoi(opt->attr("col").c_str());
	    if(col == 0)	mSec[row].policy = opt->text();
	    else if(col == 1)	mSec[row].messageMode = (MessageSecurityMode)atoi(opt->text().c_str());
	    modif();
	}
    }
    else if(a_path.compare(0,7,"/ep/cfg") == 0) TConfig::cntrCmdProc(opt, TSYS::pathLev(a_path,2), "root", SPRT_ID, RWRWR_);
    else TCntrNode::cntrCmdProc(opt);
}
