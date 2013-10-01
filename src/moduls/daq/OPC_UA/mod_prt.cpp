
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

int OPCEndPoint::reqData( int reqTp, XML_N &req )
{
    cntReq++;

    switch(reqTp)
    {
	case OpcUa_BrowseRequest: case OpcUa_BrowseNextRequest:
	{
	    int rez = Server::EP::reqData(reqTp, req);
	    //if(rez != OpcUa_BadBrowseNameInvalid) return rez;

	    NodeId nid = NodeId::fromAddr(req.attr("node"));
	    int rPn = atoi(req.attr("rPn").c_str());

	    //> Check for DAQ subsystem data
	    if(nid.ns() != 1 || TSYS::strParse(nid.strVal(),0,".") != SYS->daq().at().subId() || (rPn && req.childSize() >= rPn)) return rez;
	    NodeId rtId = NodeId::fromAddr(req.attr("RefTpId"));
            uint32_t bd = atoi(req.attr("BrDir").c_str());
            uint32_t nClass = atoi(req.attr("ClassMask").c_str());
	    string lstNd = req.attr("LastNode"); req.setAttr("LastNode","");

	    vector<string> chLs;

	    //>> Connect to DAQ node
	    AutoHD<TCntrNode> cNd = SYS->daq();
	    string sel;
	    int nLev = 0;
	    for(int off = 0; (sel=TSYS::strParse(nid.strVal(),off?0:1,".",&off)).size(); nLev++)
		try { cNd = cNd.at().nodeAt(sel); }
		catch(TError err) { return OpcUa_BadBrowseNameInvalid; }

	    //> typeDefinition reference browse
	    if(lstNd.empty() && nLev && rtId.numbVal() == OpcUa_References && (bd == BD_FORWARD || bd == BD_BOTH))
	    {
		map<string, XML_N*>::iterator ndTpDef;
		switch(nLev)
		{
		    case 1: ndTpDef = ndMap.find(NodeId("DAQModuleObjectType",1).toAddr());	break;
		    case 2: ndTpDef = ndMap.find(NodeId("DAQControllerObjectType",1).toAddr());	break;
		    case 3: ndTpDef = ndMap.find(NodeId("DAQParameterObjectType",1).toAddr());	break;
		    case 4: ndTpDef = ndMap.find(NodeId(OpcUa_BaseDataVariableType).toAddr());	break;
		}
		if(ndTpDef != ndMap.end())
		{
		    unsigned cnClass = atoi(ndTpDef->second->attr("NodeClass").c_str());
		    if(!nClass || nClass == cnClass)
			req.childAdd("ref")->setAttr("NodeId", ndTpDef->second->attr("NodeId"))->
                            setAttr("referenceTypeId", ndTpDef->second->attr("referenceTypeId"))->
                            setAttr("dir", "1")->setAttr("name", ndTpDef->second->attr("name"))->
                            setAttr("NodeClass", i2s(cnClass))->setAttr("typeDefinition", ndTpDef->second->attr("typeDefinition"));
		}
	    }
	    //>> Inverse browse
	    if(lstNd.empty() && (!nClass || nClass == NC_Object) && (bd == BD_INVERSE || bd == BD_BOTH) && nid.strVal() != "DAQ")
		req.childAdd("ref")->setAttr("NodeId", NodeId(nid.strVal().substr(0,nid.strVal().rfind(".")),1).toAddr())->
                    setAttr("referenceTypeId", i2s(OpcUa_Organizes))->
                    setAttr("dir", "0")->setAttr("name", TSYS::strParse(nid.strVal(),nLev,"."))->
                    setAttr("NodeClass", i2s(NC_Object))->setAttr("typeDefinition", i2s(OpcUa_FolderType));
	    //>> Forward browse
	    if((!nClass || nClass == NC_Object) && (bd == BD_FORWARD || bd == BD_BOTH))
	    {
		NodeId	tDef, refTpId = OpcUa_Organizes;
		uint32_t nCl = NC_Object;
		bool	lstOK = lstNd.empty() ? true : false;
		switch(nLev)
		{
		    case 0: tDef = NodeId("DAQModuleObjectType",1); ((AutoHD<TDAQS>)cNd).at().modList(chLs);	break;
		    case 1: tDef = NodeId("DAQControllerObjectType",1); ((AutoHD<TTipDAQ>)cNd).at().list(chLs);	break;
		    case 2: tDef = NodeId("DAQParameterObjectType",1); ((AutoHD<TController>)cNd).at().list(chLs);	break;
		    case 3:
			tDef = OpcUa_BaseDataVariableType, refTpId = OpcUa_HasComponent, nCl = NC_Variable;
			((AutoHD<TParamContr>)cNd).at().vlList(chLs);
			break;
		}
		for(unsigned i_ch = 0; i_ch < chLs.size(); i_ch++)
		{
		    if(!lstOK) { lstOK = (lstNd==NodeId(nid.strVal()+"."+chLs[i_ch],1).toAddr()); continue; }
		    req.childAdd("ref")->setAttr("NodeId", NodeId(nid.strVal()+"."+chLs[i_ch],1).toAddr())->
                        setAttr("referenceTypeId", refTpId.toAddr())->
                        setAttr("dir", "1")->setAttr("name", chLs[i_ch])->
                        setAttr("NodeClass", i2s(nCl))->setAttr("typeDefinition", tDef.toAddr());
                    if(rPn && req.childSize() >= rPn && (i_ch+1) < chLs.size())
                    {
			req.setAttr("LastNode", NodeId(nid.strVal()+"."+chLs[i_ch],1).toAddr());
			break;
                    }
                }
	    }
	    return 0;
	}
	case OpcUa_ReadRequest:
	{
	    int rez = Server::EP::reqData(reqTp, req);
	    if(rez != OpcUa_BadNodeIdUnknown) return rez;

	    NodeId nid = NodeId::fromAddr(req.attr("node"));

	    //OpenSCADA DAQ parameter's attribute
	    if(nid.ns() != 1)	return OpcUa_BadNodeIdUnknown;
	    uint32_t aid = atoi(req.attr("aid").c_str());

	    //>> Connect to DAQ node
	    AutoHD<TCntrNode> cNd = SYS->daq();
	    string sel;
	    int nLev = 0;
	    for(int off = 0; (sel=TSYS::strParse(nid.strVal(),off?0:1,".",&off)).size(); nLev++)
		try { cNd = cNd.at().nodeAt(sel); } catch(TError err) { break; }

	    if(!sel.empty()) return OpcUa_BadNodeIdUnknown;
	    switch(aid)
	    {
		case AId_NodeId: req.setAttr("type", i2s(OpcUa_NodeId))->setText(nid.toAddr());				return 0;
		case AId_BrowseName: req.setAttr("type", i2s(OpcUa_QualifiedName))->setText(cNd.at().nodeName());	return 0;
		case AId_InverseName: req.setAttr("type", i2s(OpcUa_LocalizedText))->setText("");			return 0;
		case AId_WriteMask: case AId_UserWriteMask: req.setAttr("type", i2s(OpcUa_UInt32))->setText("0");	return 0;
		default:
		{
		    if(dynamic_cast<TVal*>(&cNd.at()))
		    {
			AutoHD<TVal> val = cNd;
			//>>> Variable
			switch(aid)
			{
			    case AId_NodeClass: req.setAttr("type", i2s(OpcUa_Int32))->setText(i2s(NC_Variable));		return 0;
			    case AId_DisplayName: req.setAttr("type", i2s(OpcUa_LocalizedText))->setText(val.at().name());	return 0;
			    case AId_Descr: req.setAttr("type", i2s(OpcUa_String))->setText(val.at().fld().descr());		return 0;
			    case AId_Value:
				switch(val.at().fld().type())
				{
				    case TFld::Boolean: req.setAttr("type", i2s(OpcUa_Boolean))->setText(val.at().getS());	return 0;
				    case TFld::Integer: req.setAttr("type", i2s(OpcUa_Int32))->setText(val.at().getS());	return 0;
				    case TFld::Real: req.setAttr("type", i2s(OpcUa_Double))->setText(val.at().getS());		return 0;
				    case TFld::String: req.setAttr("type", i2s(OpcUa_String))->setText(val.at().getS());	return 0;
				    default: break;
				}
				break;
			    case AId_DataType:
				switch(val.at().fld().type())
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
				req.setAttr("type", i2s(OpcUa_Byte))->setText(i2s(ACS_Read | (((AutoHD<TVal>)cNd).at().fld().flg()&TFld::NoWrite ? 0 : ACS_Write)));
				return 0;
			    case AId_MinimumSamplingInterval: req.setAttr("type", i2s(OpcUa_Double))->setText("0");		return 0;
			    case AId_Historizing: req.setAttr("type", i2s(OpcUa_Boolean))->setText("0");			return 0;
			    default: return OpcUa_BadAttributeIdInvalid;
			}
		    }
		    //>>> Objects
		    else switch(aid)
		    {
			case AId_NodeClass: req.setAttr("type", i2s(OpcUa_Int32))->setText(i2s(NC_Object));			return 0;
			case AId_DisplayName:
			    if(dynamic_cast<TModule*>(&cNd.at()))
				req.setAttr("type", i2s(OpcUa_LocalizedText))->setText(((AutoHD<TModule>)cNd).at().modName());
			    else if(dynamic_cast<TController*>(&cNd.at()))
				req.setAttr("type", i2s(OpcUa_LocalizedText))->setText(((AutoHD<TController>)cNd).at().name());
			    else if(dynamic_cast<TParamContr*>(&cNd.at()))
				req.setAttr("type", i2s(OpcUa_LocalizedText))->setText(((AutoHD<TParamContr>)cNd).at().name());
			    else req.setAttr("type", i2s(OpcUa_LocalizedText))->setText(cNd.at().nodeName());
			    return 0;
			case AId_Descr:
			    if(dynamic_cast<TModule*>(&cNd.at()))
				req.setAttr("type", i2s(OpcUa_LocalizedText))->setText(((AutoHD<TModule>)cNd).at().modInfo("Description"));
			    else if(dynamic_cast<TController*>(&cNd.at()))
				req.setAttr("type", i2s(OpcUa_LocalizedText))->setText(((AutoHD<TController>)cNd).at().descr());
			    else if(dynamic_cast<TParamContr*>(&cNd.at()))
				req.setAttr("type", i2s(OpcUa_LocalizedText))->setText(((AutoHD<TParamContr>)cNd).at().descr());
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
	    int rez = Server::EP::reqData(reqTp, req);
            if(rez != OpcUa_BadNodeIdUnknown) return rez;

	    NodeId nid = NodeId::fromAddr(req.attr("node"));
	    //OpenSCADA DAQ parameter's attribute
	    if(nid.ns() != 1)	return OpcUa_BadNodeIdUnknown;
	    uint32_t aid = atoi(req.attr("aid").c_str());
	    //>> Connect to DAQ node
	    AutoHD<TCntrNode> cNd = SYS->daq();
	    string sel;
	    int nLev = 0;
	    for(int off = 0; (sel=TSYS::strParse(nid.strVal(),off?0:1,".",&off)).size(); nLev++)
		try { cNd = cNd.at().nodeAt(sel); } catch(TError err) { break; }

	    if(!sel.empty()) return OpcUa_BadNodeIdUnknown;
	    if(aid != AId_Value || !dynamic_cast<TVal*>(&cNd.at())) return OpcUa_BadNothingToDo;
	    ((AutoHD<TVal>)cNd).at().setS(req.text());

	    return 0;
	}
    }

    return OpcUa_BadNodeIdUnknown;
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
