
//OpenSCADA system module DAQ.OPC_UA file: mod_daq.cpp
/***************************************************************************
 *   Copyright (C) 2009 by Roman Savochenko                                *
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

#include <getopt.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#include <terror.h>
#include <tsys.h>
#include <tmess.h>
#include <ttiparam.h>
#include <tdaqs.h>

#include "mod_prt.h"
#include "mod_daq.h"

OPC_UA::TTpContr *OPC_UA::mod;  //Pointer for direct access to DAQ module

using namespace OPC_UA;

//*************************************************
//* TTpContr                                      *
//*************************************************
TTpContr::TTpContr( string name )
{
    mId		= DAQ_ID;
    mName	= DAQ_NAME;
    mType	= DAQ_TYPE;
    mVers	= DAQ_MVER;
    mAutor	= DAQ_AUTOR;
    mDescr	= DAQ_DESCR;
    mLicense	= DAQ_LICENSE;
    mSource	= name;

    mod		= this;
}

TTpContr::~TTpContr()
{

}

void TTpContr::load_( )
{
    //> Load parameters from command line

}

void TTpContr::save_( )
{

}

void TTpContr::postEnable( int flag )
{
    TTipDAQ::postEnable(flag);

    //> Controler's bd structure
    fldAdd( new TFld("PRM_BD",_("Parameteres table"),TFld::String,TFld::NoFlag,"30","") );
    fldAdd( new TFld("SCHEDULE",_("Calc schedule"),TFld::String,TFld::NoFlag,"100","1") );
    fldAdd( new TFld("PRIOR",_("Gather task priority"),TFld::Integer,TFld::NoFlag,"2","0","-1;99") );
    fldAdd( new TFld("SYNCPER",_("Sync inter remote station period (s)"),TFld::Real,TFld::NoFlag,"6.2","60","0;1000") );
    fldAdd( new TFld("ADDR",_("Transport address"),TFld::String,TFld::NoFlag,"30","") );
    fldAdd( new TFld("EndPoint",_("End point"),TFld::String,TFld::NoFlag,"50","opc.tcp://localhost:4841") );
    fldAdd( new TFld("SecPolicy",_("Security policy"),TFld::String,TFld::Selected,"20","None","None;Basic128;Basic128Rsa15;Basic256",_("None"/*;Basic128;Basic128Rsa15;Basic256"*/)) );
    fldAdd( new TFld("SecMessMode",_("Message security mode"),TFld::Integer,TFld::Selected,"1","0","0;1;2",_("None;Sign;Sign&Encrypt")) );
    fldAdd( new TFld("Cert",_("Certificate (PEM)"),TFld::String,TFld::FullText,"10000") );
    fldAdd( new TFld("AttrsLimit",_("Parameter attributes number limit"),TFld::Integer,TFld::NoFlag,"3","100") );

    //> Parameter type bd structure
    int t_prm = tpParmAdd("std","PRM_BD",_("Standard"));
    tpPrmAt(t_prm).fldAdd( new TFld("ND_LS",_("Nodes list"),TFld::String,TFld::FullText|TCfg::NoVal,"1000","") );
}

TController *TTpContr::ContrAttach( const string &name, const string &daq_db )
{
    return new TMdContr(name,daq_db,this);
}

//*************************************************
//* TMdContr                                      *
//*************************************************
TMdContr::TMdContr( string name_c, const string &daq_db, ::TElem *cfgelem) :
	::TController(name_c,daq_db,cfgelem), prc_st(false), endrun_req(false), tm_gath(0), mBrwsVar("Root folder (0:84)"), mPCfgCh(false),
	mSched(cfg("SCHEDULE").getSd()), mPrior(cfg("PRIOR").getId()), mSync(cfg("SYNCPER").getRd()), mAddr(cfg("ADDR").getSd()),
	mEndPoint(cfg("EndPoint").getSd()), mSecPolicy(cfg("SecPolicy").getSd()), mPAttrLim(cfg("AttrsLimit").getId())
{
    cfg("PRM_BD").setS("OPC_UA_Prm_"+name_c);

    sess.secChnl = sess.secToken = 0;
    sess.sqNumb = 33;
    sess.sqReqId = 1;
    sess.reqHndl = 0;
    sess.secLifeTime = 0;
    sess.sesId = sess.authTkId = 0;
    sess.sesAccess = 0;
    sess.sesLifeTime = 1.2e6;
}

TMdContr::~TMdContr( )
{
    if( run_st ) stop();
}

string TMdContr::cert( )	{ return cfg("Cert").getS(); }

string TMdContr::getStatus( )
{
    string rez = TController::getStatus( );
    if( startStat() && !redntUse( ) ) rez += TSYS::strMess(_("Gather data time %.6g ms. "),tm_gath);
    return rez;
}

void TMdContr::reqOPC( XMLNode &io )
{
    ResAlloc res( nodeRes(), true );
    io.setAttr("err","");

    AutoHD<TTransportOut> tr = SYS->transport().at().at(TSYS::strSepParse(mAddr,0,'.')).at().outAt(TSYS::strSepParse(mAddr,1,'.'));
    try { tr.at().start(); }
    catch( TError err )
    { io.setAttr("err",TSYS::strMess("0x%x:%s",OpcUa_BadCommunicationError,err.mess.c_str())); return; }

    nextTry:
    XMLNode req("opc.tcp");
    if( !sess.secChnl || !sess.secToken || (1e-3*(TSYS::curTime()-sess.sesAccess) >= sess.secLifeTime) )
    {
	if( sess.secChnl && sess.secToken ) sess.authTkId = 0;
	//>> Send HELLO message
	req.setAttr("id","HEL")->setAttr("EndPoint",endPoint());
	tr.at().messProtIO(req,"OPC_UA");
	if( !req.attr("err").empty() )	{ io.setAttr("err",req.attr("err")); return; }

	//>> Send Open SecureChannel message
	req.setAttr("id","OPN")->setAttr("SecChnId","0"/*TSYS::uint2str(sess.secChnl)*/)->
	    setAttr("SecPolicy",secPolicy())->setAttr("SecLifeTm","300000")->
	    setAttr("SeqNumber","51")->setAttr("SeqReqId","1")->setAttr("ReqHandle","0");
	tr.at().messProtIO(req,"OPC_UA");
	if( !req.attr("err").empty() )	{ io.setAttr("err",req.attr("err")); return; }
	sess.sqNumb = 51;
	sess.sqReqId = 1;
	sess.reqHndl = 0;
	sess.secChnl = strtoul(req.attr("SecChnId").c_str(),NULL,10);
	sess.secToken = strtoul(req.attr("SecTokenId").c_str(),NULL,10);
	sess.secLifeTime = atoi(req.attr("SecLifeTm").c_str());
    }
    io.setAttr("SecChnId",TSYS::uint2str(sess.secChnl))->setAttr("SecTokenId",TSYS::uint2str(sess.secToken));

    string ireq = io.attr("id");
    if( ireq != "FindServers" && ireq != "GetEndpoints" &&
	(!sess.authTkId || 1e-3*(TSYS::curTime()-sess.sesAccess) >= sess.sesLifeTime) )
    {
	//>> Send CreateSession message
	req.setAttr("id","CreateSession")->setAttr("EndPoint",endPoint())->setAttr("sesTm","1.2e6")->
	    setAttr("SecChnId",TSYS::uint2str(sess.secChnl))->setAttr("SecTokenId",TSYS::uint2str(sess.secToken))->
	    setAttr("SeqNumber",TSYS::uint2str(sess.sqNumb++))->setAttr("SeqReqId",TSYS::uint2str(sess.sqReqId++))->
	    setAttr("ReqHandle",TSYS::uint2str(sess.reqHndl++))->setAttr("authTokenId",TSYS::int2str(sess.authTkId))->
	    childAdd("ClientCert")->setText(cert());
	tr.at().messProtIO(req,"OPC_UA");
	if( !req.attr("err").empty() )	{ io.setAttr("err",req.attr("err")); return; }
	sess.sesId = atoi(req.attr("sesId").c_str());
	sess.authTkId = atoi(req.attr("authTokenId").c_str());
	sess.sesLifeTime = atof(req.attr("sesTm").c_str());

	//>> Send ActivateSession message
	req.setAttr("id","ActivateSession");
	tr.at().messProtIO(req,"OPC_UA");
	if( !req.attr("err").empty() )	{ io.setAttr("err",req.attr("err")); return; }
    }

    sess.sesAccess = TSYS::curTime();
    io.setAttr("authTokenId",TSYS::int2str(sess.authTkId))->setAttr("ReqHandle",TSYS::uint2str(sess.reqHndl++))->
	setAttr("SeqNumber",TSYS::uint2str(sess.sqNumb++))->setAttr("SeqReqId",TSYS::uint2str(sess.sqReqId++));
    tr.at().messProtIO(io,"OPC_UA");
    if( strtoul(io.attr("err").c_str(),NULL,0) == OpcUa_BadInvalidArgument )
    {
	sess.secChnl = sess.authTkId = 0;
	goto nextTry;
    }
}

TParamContr *TMdContr::ParamAttach( const string &name, int type )
{
    return new TMdPrm(name,&owner().tpPrmAt(type));
}

void TMdContr::start_( )
{
    //> Establish connection
    AutoHD<TTransportOut> tr = SYS->transport().at().at(TSYS::strSepParse(mAddr,0,'.')).at().outAt(TSYS::strSepParse(mAddr,1,'.'));
    try { tr.at().start(); }
    catch( TError err ){ mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }

    //> Schedule process
    mPer = TSYS::strSepParse(mSched,1,' ').empty() ? vmax(0,(long long)(1e9*atof(mSched.c_str()))) : 0;

    //> Start the gathering data task
    if( !prc_st ) SYS->taskCreate( nodePath('.',true), mPrior, TMdContr::Task, this, &prc_st );
}

void TMdContr::stop_( )
{
    //> Stop the request and calc data task
    SYS->taskDestroy( nodePath('.',true), &prc_st, &endrun_req );
}

void TMdContr::prmEn( const string &id, bool val )
{
    int i_prm;

    ResAlloc res(en_res,true);
    for( i_prm = 0; i_prm < p_hd.size(); i_prm++)
	if( p_hd[i_prm].at().id() == id ) break;

    if( val && i_prm >= p_hd.size() )	p_hd.push_back(at(id));
    if( !val && i_prm < p_hd.size() )	p_hd.erase(p_hd.begin()+i_prm);
    if( startStat() ) mPCfgCh = true;
}

void *TMdContr::Task( void *icntr )
{
    vector<string>	als;
    string		nId;
    TMdContr &cntr = *(TMdContr *)icntr;

#if OSC_DEBUG >= 2
    mess_debug(cntr.nodePath().c_str(),_("Thread <%u> is started. TID: %ld"),pthread_self(),(long int)syscall(224));
#endif

    cntr.endrun_req = false;
    cntr.prc_st = true;

    XMLNode req("opc.tcp"); req.setAttr("id","Read")->setAttr("timestampsToReturn",TSYS::int2str(TProt::TS_NEITHER));

    for( unsigned int it_cnt = cntr.p_hd.size(); !cntr.endrun_req; it_cnt++ )
    {
	if( cntr.redntUse() )	{ sleep(1); continue; }

	long long t_cnt = TSYS::curTime();
	unsigned int div = cntr.period() ? (unsigned int)(cntr.mSync/(1e-9*cntr.period())) : 0;

	ResAlloc res(cntr.en_res,false);
	if( !req.childSize() || cntr.mPCfgCh || (div && (it_cnt%div) < cntr.p_hd.size()) )
	{
	    if( div && (it_cnt%div) < cntr.p_hd.size() ) cntr.p_hd[it_cnt%div].at().attrPrc();

	    //> Prepare nodes list
	    req.childClear();
	    for( unsigned i_p=0; i_p < cntr.p_hd.size(); i_p++ )
	    {
		cntr.p_hd[i_p].at().vlList(als);
		for( int i_a = 0; i_a < als.size(); i_a++ )
		{
		    nId = cntr.p_hd[i_p].at().vlAt(als[i_a]).at().fld().reserve();
		    if( nId.empty() ) continue;
		    req.childAdd("node")->setAttr("prmId",cntr.p_hd[i_p].at().id())->setAttr("prmAttr",als[i_a])->setAttr("nodeId",nId)->setAttr("attributeId","13");
		}
	    }
	    cntr.mPCfgCh = false;
	}
	res.release();

	cntr.reqOPC(req);

	//> Place results
	if( req.attr("err").empty() )
	{
	    res.request(false);
	    for( int i_c = 0, i_p = 0; i_c < req.childSize() && i_p < cntr.p_hd.size(); i_c++ )
	    {
		XMLNode *cnX = req.childGet(i_c);
		while( cnX->attr("prmId") != cntr.p_hd[i_p].at().id() ) i_p++;
		if( cntr.p_hd[i_p].at().vlPresent(cnX->attr("prmAttr")) )
		    cntr.p_hd[i_p].at().vlAt(cnX->attr("prmAttr")).at().setS(cnX->text(),0,true);
	    }
	    res.release();
	}

	cntr.tm_gath = 1e-3*(TSYS::curTime()-t_cnt);

	TSYS::taskSleep(cntr.period(),cntr.period()?0:TSYS::cron(cntr.cron()));
    }

    cntr.prc_st = false;

    return NULL;
}

bool TMdContr::cfgChange( TCfg &icfg )
{
    TController::cfgChange(icfg);

    if( icfg.name() == "SecPolicy" )
    {
	if( icfg.getS() == "None" && cfg("SecMessMode").getI() ) cfg("SecMessMode").setI(0);
	if( icfg.getS() != "None" && !cfg("SecMessMode").getI() ) cfg("SecMessMode").setI(1);
    }
    else if( icfg.name() == "SecMessMode" &&
	    ((icfg.getI() && cfg("SecPolicy").getS() == "None") || (!icfg.getI() && cfg("SecPolicy").getS() != "None")) )
	return false;

    return true;
}

void TMdContr::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if( opt->name() == "info" )
    {
	TController::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/cntr/cfg/ADDR",cfg("ADDR").fld().descr(),0664,"root","DAQ",3,"tp","str","dest","select","select","/cntr/cfg/trLst");
	ctrMkNode("fld",opt,-1,"/cntr/cfg/SCHEDULE",cfg("SCHEDULE").fld().descr(),0664,"root","DAQ",4,"tp","str","dest","sel_ed",
	    "sel_list",_("1;1e-3;* * * * *;10 * * * *;10-20 2 */2 * *"),
	    "help",_("Schedule is writed in seconds periodic form or in standard Cron form.\n"
		"Seconds form is one real number (1.5, 1e-3).\n"
		"Cron it is standard form '* * * * *'. Where:\n"
		"  - minutes (0-59);\n"
		"  - hours (0-23);\n"
		"  - days (1-31);\n"
		"  - month (1-12);\n"
		"  - week day (0[sunday]-6)."));
	ctrMkNode("fld",opt,-1,"/cntr/cfg/Cert",cfg("Cert").fld().descr(),0660,"root","DAQ",3,"tp","str","cols","90","rows","7");
	if( enableStat() && ctrMkNode("area",opt,-1,"/ndBrws",_("Server nodes browser")) )
	{
	    ctrMkNode("fld",opt,-1,"/ndBrws/nd",_("Node"),RWRWR_,"root","DAQ",3,"tp","str","dest","select","select","/ndBrws/ndLst");
	    if( ctrMkNode("table",opt,-1,"/ndBrws/attrs",_("Attributes"),R_R_R_,"root","DAQ") )
	    {
		ctrMkNode("list",opt,-1,"/ndBrws/attrs/0",_("Attribute"),R_R_R_,"root","DAQ",1,"tp","str");
		ctrMkNode("list",opt,-1,"/ndBrws/attrs/1",_("Value"),R_R_R_,"root","DAQ",1,"tp","str");
	    }
	}
	return;
    }
    //> Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/cntr/cfg/trLst" && ctrChkNode(opt) )
    {
	vector<string> sls;
	SYS->transport().at().outTrList(sls);
	for( int i_s = 0; i_s < sls.size(); i_s++ )
	    opt->childAdd("el")->setText(sls[i_s]);
    }
    else if( enableStat() && a_path == "/ndBrws/nd" )
    {
	if( ctrChkNode(opt,"get",RWRWR_,"root","DAQ",SEC_RD) )	opt->setText(mBrwsVar);
	if( ctrChkNode(opt,"set",RWRWR_,"root","DAQ",SEC_WR) )	mBrwsVar = opt->text();
    }
    else if( enableStat() && a_path == "/ndBrws/attrs" && ctrChkNode(opt) )
    {
	XMLNode *n_attr	= ctrMkNode("list",opt,-1,"/ndBrws/attrs/0","",R_R_R_);
	XMLNode *n_val	= ctrMkNode("list",opt,-1,"/ndBrws/attrs/1","",R_R_R_);

	//>> Prepare request for all typical
	string cNodeId = "0:84";
	int stP = mBrwsVar.find("(");
	int stC = mBrwsVar.find(")",stP);;
	if( stP != string::npos && stC != string::npos ) cNodeId = mBrwsVar.substr(stP+1,stC-stP-1);

	XMLNode req("opc.tcp"); req.setAttr("id","Read")->setAttr("timestampsToReturn",TSYS::int2str(TProt::TS_NEITHER));
	for( int i_a = 1; i_a <= 22; i_a++ )
	    req.childAdd("node")->setAttr("nodeId",cNodeId)->setAttr("attributeId",TSYS::int2str(i_a));
	reqOPC(req);
	if( !req.attr("err").empty() ) throw TError(nodePath().c_str(),"%s",req.attr("err").c_str());

	//>> Get result
	for( int i_a = 0; i_a < req.childSize(); i_a++ )
	{
	    if( strtol(req.childGet(i_a)->attr("Status").c_str(),NULL,0) ) continue;
	    string nANm = _("Unknown");
	    string nAVl = req.childGet(i_a)->text();
	    switch( i_a+1 )
	    {
		case TProt::AId_NodeId:	nANm = _("NodeId");	break;
		case TProt::AId_NodeClass:
		    nANm = _("NodeClass");
		    switch(atoi(nAVl.c_str()))
		    {
			case TProt::NC_Object:		nAVl = _("Object");	break;
			case TProt::NC_Variable:	nAVl = _("Variable");	break;
			case TProt::NC_Method:		nAVl = _("Method");	break;
			case TProt::NC_ObjectType:	nAVl = _("ObjectType");	break;
			case TProt::NC_VariableType:	nAVl = _("VariableType");	break;
			case TProt::NC_ReferenceType:	nAVl = _("ReferenceType");	break;
			case TProt::NC_DataType:	nAVl = _("DataType");	break;
			case TProt::NC_View:		nAVl = _("View");	break;
		    }
		    break;
		case TProt::AId_BrowseName:	nANm = _("BrowseName");	break;
		case TProt::AId_DisplayName:	nANm = _("DisplayName");break;
		case TProt::AId_Descr:		nANm = _("Description");break;
		case TProt::AId_WriteMask:	nANm = _("WriteMask");	break;
		case TProt::AId_UserWriteMask:	nANm = _("UserWriteMask");	break;
		case TProt::AId_IsAbstract:	nANm = _("IsAbstract");	break;
		case TProt::AId_Symmetric:	nANm = _("Symmetric");	break;
		case TProt::AId_InverseName:	nANm = _("InverseName");	break;
		case TProt::AId_ContainsNoLoops:nANm = _("ContainsNoLoops");	break;
		case TProt::AId_EventNotifier:	nANm = _("EventNotifier");	break;
		case TProt::AId_Value:		nANm = _("Value");	break;
		case TProt::AId_DataType:
		{
		    nANm = _("DataType");
		    XMLNode reqTp("opc.tcp");
		    reqTp.setAttr("id","Read")->setAttr("timestampsToReturn",TSYS::int2str(TProt::TS_NEITHER))->
			childAdd("node")->setAttr("nodeId",nAVl)->setAttr("attributeId","3");
		    reqOPC(reqTp);
		    if( reqTp.attr("err").empty() && reqTp.childSize() ) nAVl = reqTp.childGet(0)->text();
		    break;
		}
		case TProt::AId_ValueRank:	nANm = _("ValueRank");	break;
		case TProt::AId_ArrayDimensions:nANm = _("ArrayDimensions");	break;
		case TProt::AId_AccessLevel:
		{
		    nANm = _("AccessLevel");
		    char cRW = atoi(nAVl.c_str());
		    nAVl = "";
		    if( cRW & TProt::ACS_Read )		nAVl += _("Readable, ");
		    if( cRW & TProt::ACS_Write )	nAVl += _("Writible, ");
		    if( cRW & TProt::ACS_HistRead )	nAVl += _("History readable, ");
		    if( cRW & TProt::ACS_HistWrite )	nAVl += _("History writible, ");
		    if( cRW & TProt::ACS_SemChange )	nAVl += _("Semantic change, ");
		    break;
		}
		case TProt::AId_UserAccessLevel:
		{
		    nANm = _("UserAccessLevel");
		    char cRW = atoi(nAVl.c_str());
		    nAVl = "";
		    if( cRW & TProt::ACS_Read )		nAVl += _("Readable, ");
		    if( cRW & TProt::ACS_Write )	nAVl += _("Writible, ");
		    if( cRW & TProt::ACS_HistRead )	nAVl += _("History readable, ");
		    if( cRW & TProt::ACS_HistWrite )	nAVl += _("History writible, ");
		    break;
		}
		case TProt::AId_MinimumSamplingInterval:nANm = _("MinimumSamplingInterval");	break;
		case TProt::AId_Historizing:		nANm = _("Historizing");	break;
		case TProt::AId_Executable:		nANm = _("Executable");	break;
		case TProt::AId_UserExecutable:		nANm = _("UserExecutable");	break;
	    }
	    if( n_attr )	n_attr->childAdd("el")->setText(nANm);
	    if( n_val )		n_val->childAdd("el")->setText(nAVl);
	}
    }
    else if( enableStat() && a_path == "/ndBrws/ndLst" && ctrChkNode(opt) )
    {
	//>> Get current node references by call browse
	string cNodeId = "0:84";
	int stP = mBrwsVar.find("(");
	int stC = mBrwsVar.find(")",stP);;
	if( stP != string::npos && stC != string::npos ) cNodeId = mBrwsVar.substr(stP+1,stC-stP-1);
	XMLNode req("opc.tcp"); req.setAttr("id","Browse");
	req.childAdd("node")->setAttr("nodeId",cNodeId)->
			      setAttr("browseDirection",TSYS::int2str(TProt::BD_BOTH))->
			      setAttr("resultMask",TSYS::int2str(0x3f/*TProt::RdRm_IsForward|TProt::RdRm_BrowseName*/));
	reqOPC(req);
	if( !req.attr("err").empty() || !req.childSize() ) throw TError(nodePath().c_str(),"%s",req.attr("err").c_str());
	XMLNode *rn = req.childGet(0);

	//>> Process inverse references
	for( int i_n = 0; i_n < rn->childSize(); i_n++ )
	{
	    if( atoi(rn->childGet(i_n)->attr("isForward").c_str()) )	continue;
	    opt->childAdd("el")->setText(rn->childGet(i_n)->attr("browseName")+" ("+rn->childGet(i_n)->attr("nodeId")+")");
	}
	//>> Append self address
	opt->childAdd("el")->setText(mBrwsVar);
	//>> Process forward references
	for( int i_n = 0; i_n < rn->childSize(); i_n++ )
	{
	    if( !atoi(rn->childGet(i_n)->attr("isForward").c_str()) )	continue;
	    opt->childAdd("el")->setText(rn->childGet(i_n)->attr("browseName")+" ("+rn->childGet(i_n)->attr("nodeId")+")");
	}
    }
    else TController::cntrCmdProc(opt);
}


//*************************************************
//* TMdPrm                                        *
//*************************************************
TMdPrm::TMdPrm( string name, TTipParam *tp_prm ) : TParamContr(name,tp_prm), p_el("w_attr"), mNdLst(cfg("ND_LS").getSd())
{

}

TMdPrm::~TMdPrm( )
{
    nodeDelAll();
}

void TMdPrm::postEnable( int flag )
{
    TParamContr::postEnable(flag);
    if( !vlElemPresent(&p_el) )	vlElemAtt(&p_el);
}

TMdContr &TMdPrm::owner( )	{ return (TMdContr&)TParamContr::owner(); }

void TMdPrm::enable()
{
    if( enableStat() )	return;

    TParamContr::enable();

    attrPrc();

    owner().prmEn( id(), true );
}

void TMdPrm::disable()
{
    if( !enableStat() )  return;

    owner().prmEn( id(), false );

    TParamContr::disable();

    //> Delete no used fields
    for( int i_fld = 0; i_fld < p_el.fldSize(); i_fld++ )
    {
	try{ p_el.fldDel(i_fld); i_fld--; }
	catch( TError err )	{  }
    }

    //> Set EVAL to parameter attributes
    vector<string> ls;
    elem().fldList(ls);
    for(int i_el = 0; i_el < ls.size(); i_el++)
	vlAt(ls[i_el]).at().setS(EVAL_STR,0,true);
}

void TMdPrm::load_( )
{
    TParamContr::load_();
}

string TMdPrm::attrPrc( )
{
    ResAlloc res( nodeRes(), true );

    bool srchOK = false;
    XMLNode req("opc.tcp");
    vector<string> als;

    //> Nodes list process and parameter's attributes creation
    string snd;
    for( int off = 0; (snd=TSYS::strParse(mNdLst,0,"\n",&off)).size(); )
    {
	//>> Request for node class request
	req.clear()->setAttr("id","Read")->setAttr("timestampsToReturn",TSYS::int2str(TProt::TS_NEITHER));
	req.childAdd("node")->setAttr("nodeId",snd)->setAttr("attributeId",TSYS::int2str(TProt::AId_NodeClass));
	req.childAdd("node")->setAttr("nodeId",snd)->setAttr("attributeId",TSYS::int2str(TProt::AId_BrowseName));
	req.childAdd("node")->setAttr("nodeId",snd)->setAttr("attributeId",TSYS::int2str(TProt::AId_DisplayName));
	req.childAdd("node")->setAttr("nodeId",snd)->setAttr("attributeId",TSYS::int2str(TProt::AId_Value));
	req.childAdd("node")->setAttr("nodeId",snd)->setAttr("attributeId",TSYS::int2str(TProt::AId_AccessLevel));
	owner().reqOPC(req);
	if( !req.attr("err").empty() ) return req.attr("err");
	if( strtol(req.childGet(0)->attr("Status").c_str(),NULL,0) )	continue;

	//>> Variable node's attribute creation
	if( atoi(req.childGet(0)->text().c_str()) == TProt::NC_Variable && atoi(req.childGet(4)->text().c_str())&TProt::ACS_Read )
	{
	    srchOK = false;
	    //>> Find for already presented attribute
	    for( int i_a = 0; i_a < p_el.fldSize() && !srchOK; i_a++ )
		if( p_el.fldAt(i_a).reserve() == snd ) srchOK = true;

	    //>> Create new attribute
	    if( !srchOK )
	    {
		//>>> Prepare attribute id
		string aid = TSYS::strEncode(req.childGet(1)->text(),TSYS::oscdID);
		if( vlPresent(aid) )
		    for( int i_v = 1; true; i_v++ )
			if( !vlPresent(aid+TSYS::int2str(i_v)) )
			{ aid += TSYS::int2str(i_v); break; }

		//>>> Value type prepare
		TFld::Type vtp = TFld::String;
		switch( atoi(req.childGet(3)->attr("EncodingMask").c_str()) & 0x3F )
		{
		    case OpcUa_Boolean:
			vtp = TFld::Boolean;	break;
		    case OpcUa_SByte: case OpcUa_Byte: case OpcUa_Int16: case OpcUa_UInt16: case OpcUa_Int32: case OpcUa_UInt32:
			vtp = TFld::Integer;	break;
		    case OpcUa_Int64: case OpcUa_UInt64: case OpcUa_Float: case OpcUa_Double:
			vtp = TFld::Real;	break;
		}

		//>> Browse name
		string aNm = req.childGet(2)->text();
		int nmPos = aNm.find(":");
		if( nmPos!=string::npos ) aNm.erase(0,nmPos+1);

		//>>> Flags prepare
		unsigned vflg = TVal::DirWrite;
		if( !(atoi(req.childGet(4)->text().c_str())&TProt::ACS_Write) )	vflg |= TFld::NoWrite;

		p_el.fldAdd( new TFld(aid.c_str(),aNm.c_str(),vtp,vflg,req.childGet(3)->attr("EncodingMask").c_str(),"","","",snd.c_str()) );
	    }
	}

	//>> Browse request for get child nodes
	/*req.setAttr("id","Browse")->childAdd("node")->setAttr("nodeId",cNodeId)->setAttr("browseDirection",TSYS::int2str(TProt::BD_BOTH));
	reqOPC(req);
	if( !req.attr("err").empty() || !req.childSize() ) throw TError(nodePath().c_str(),"%s",req.attr("err").c_str());
	XMLNode *rn = req.childGet(0);*/
    }
    return "";
}

void TMdPrm::save_( )
{
    TParamContr::save_();
}

void TMdPrm::cntrCmdProc( XMLNode *opt )
{
    //> Service commands process
    string a_path = opt->attr("path");
    if( a_path.substr(0,6) == "/serv/" )  { TParamContr::cntrCmdProc(opt); return; }

    //> Get page info
    if( opt->name() == "info" )
    {
	TParamContr::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/cfg/ND_LS",cfg("ND_LS").fld().descr(),RWRWR_,"root","DAQ",1,
	    "help",_("Variables and it containers (Objects) list. All variables will put into the parameter attributes list.\n"
		"Variables writed by separated lines into format: [ns:id].\n"
		"Where:\n"
		"  ns - name space number;\n"
		"  id - node identifier into number or string.\n"
		"Example:\n"
		"  '0:84' - root folder;\n"
		"  '3:BasicDevices2' - basic devices node into namespace 3."));
	return;
    }

    TParamContr::cntrCmdProc(opt);
}

void TMdPrm::vlSet( TVal &valo, const TVariant &pvl )
{
    if( !enableStat() )	valo.setS( EVAL_STR, 0, true );

    //> Send to active reserve station
    if( owner().redntUse( ) )
    {
	if( valo.getS(NULL,true) == pvl.getS() ) return;
	XMLNode req("set");
	req.setAttr("path",nodePath(0,true)+"/%2fserv%2fattr")->childAdd("el")->setAttr("id",valo.name())->setText(valo.getS(NULL,true));
	SYS->daq().at().rdStRequest(owner().workId(),req);
	return;
    }

    string vl = valo.getS(NULL,true);
    if( vl == EVAL_STR || vl == pvl.getS() ) return;

    //> Direct write
    XMLNode req("opc.tcp");
    req.setAttr("id","Write")->
	childAdd("node")->setAttr("nodeId",valo.fld().reserve())->
			  setAttr("attributeId",TSYS::int2str(TProt::AId_Value))->
			  setAttr("EncodingMask",TSYS::int2str(valo.fld().len()))->
			  setText(vl);
    owner().reqOPC(req);
}

void TMdPrm::vlArchMake( TVal &val )
{
    if( val.arch().freeStat() ) return;
    val.arch().at().setSrcMode(TVArchive::PassiveAttr,val.arch().at().srcData());
    val.arch().at().setPeriod((long long)(owner().period()*1000000));
    val.arch().at().setHardGrid( true );
    val.arch().at().setHighResTm( true );
}
