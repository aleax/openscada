
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
    fldAdd( new TFld("ADDR",_("Transport address"),TFld::String,TFld::NoFlag,"30","") );
    fldAdd( new TFld("EndPoint",_("End point"),TFld::String,TFld::NoFlag,"50","opc.tcp://localhost:4841") );
    fldAdd( new TFld("SecPolicy",_("Security policy"),TFld::String,TFld::Selected,"20","None","None;Basic128;Basic128Rsa15;Basic256",_("None;Basic128;Basic128Rsa15;Basic256")) );
    fldAdd( new TFld("SecMessMode",_("Message security mode"),TFld::Integer,TFld::Selected,"1","0","0;1;2",_("None;Sign;Sign&Encrypt")) );
    fldAdd( new TFld("Cert",_("Certificate (PEM)"),TFld::String,TFld::FullText,"10000") );
    fldAdd( new TFld("AttrsLimit",_("Parameter attributes number limit"),TFld::Integer,TFld::NoFlag,"3","100") );

    //> Parameter type bd structure
    int t_prm = tpParmAdd("std","PRM_BD",_("Standard"));
    tpPrmAt(t_prm).fldAdd( new TFld("VAR_LS",_("Variables list"),TFld::String,TFld::FullText|TCfg::NoVal,"1000","") );
}

TController *TTpContr::ContrAttach( const string &name, const string &daq_db )
{
    return new TMdContr(name,daq_db,this);
}

//*************************************************
//* TMdContr                                      *
//*************************************************
TMdContr::TMdContr( string name_c, const string &daq_db, ::TElem *cfgelem) :
	::TController(name_c,daq_db,cfgelem), prc_st(false), endrun_req(false), tm_gath(0), mBrwsVar("Root folder (0:84)"),
	mSched(cfg("SCHEDULE").getSd()), mPrior(cfg("PRIOR").getId()), mAddr(cfg("ADDR").getSd()),
	mEndPoint(cfg("EndPoint").getSd()), mSecPolicy(cfg("SecPolicy").getSd()), mPAttrLim(cfg("AttrsLimit").getId())
{
    cfg("PRM_BD").setS("OPC_UA_Prm_"+name_c);

    sess.secChnl = sess.secToken = 0;
    sess.sqNumb = 33;
    sess.sqReqId = 1;
    sess.reqHndl = 0;
    sess.secRevTime = 0;
    sess.sesId = sess.authTkId = 0;
    sess.sesAccess = 0;
    sess.sesActiveTime = 1.2e6;
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

    AutoHD<TTransportOut> tr = SYS->transport().at().at(TSYS::strSepParse(mAddr,0,'.')).at().outAt(TSYS::strSepParse(mAddr,1,'.'));
    try { tr.at().start(); }
    catch( TError err )
    { io.setAttr("err",TSYS::strMess("0x%x:%s",OpcUa_BadCommunicationError,err.mess.c_str())); return; }

    XMLNode req("opc.tcp");
    if( !sess.secChnl || !sess.secToken )
    {
	//>> Send HELLO message
	req.setAttr("id","HEL")->setAttr("EndPoint",endPoint());
	tr.at().messProtIO(req,"OPC_UA");
	if( !req.attr("err").empty() )	{ io.setAttr("err",req.attr("err")); return; }

	//>> Send Open SecureChannel message
	req.setAttr("id","OPN")->setAttr("SecPolicy",secPolicy())->setAttr("SecLifeTm","300000")->
	    setAttr("SeqNumber","51")->setAttr("SeqReqId","1")->setAttr("ReqHandle","0");
	tr.at().messProtIO(req,"OPC_UA");
	if( !req.attr("err").empty() )	{ io.setAttr("err",req.attr("err")); return; }
	sess.sqNumb = 51;
	sess.sqReqId = 1;
	sess.reqHndl = 0;
	sess.secChnl = strtoul(req.attr("SecChnId").c_str(),NULL,10);
	sess.secToken = strtoul(req.attr("SecTokenId").c_str(),NULL,10);
	sess.secRevTime = atoi(req.attr("SecLifeTm").c_str());
    }
    io.setAttr("SecChnId",TSYS::uint2str(sess.secChnl))->setAttr("SecTokenId",TSYS::uint2str(sess.secToken));

    string ireq = io.attr("id");
    if( ireq != "FindServers" && ireq != "GetEndpoints" && 
	(!sess.authTkId || 1e-3*(TSYS::curTime()-sess.sesAccess) > sess.sesActiveTime) )
    {
	//>> Send CreateSession message
	req.setAttr("id","CreateSession")->setAttr("EndPoint",endPoint())->setAttr("sesTm","1.2e6")->
	    setAttr("SecChnId",TSYS::uint2str(sess.secChnl))->setAttr("SecTokenId",TSYS::uint2str(sess.secToken))->
	    setAttr("SeqNumber",TSYS::uint2str(sess.sqNumb++))->setAttr("SeqReqId",TSYS::uint2str(sess.sqReqId++))->
	    setAttr("ReqHandle",TSYS::uint2str(sess.reqHndl++))->childAdd("ClientCert")->setText(cert());
	tr.at().messProtIO(req,"OPC_UA");
	if( !req.attr("err").empty() )	{ io.setAttr("err",req.attr("err")); return; }
	sess.sesId = atoi(req.attr("sesId").c_str());
	sess.authTkId = atoi(req.attr("authTokenId").c_str());
	sess.sesActiveTime = atof(req.attr("sesTm").c_str());

	//>> Send ActivateSession message
	req.setAttr("id","ActivateSession");
	tr.at().messProtIO(req,"OPC_UA");
	if( !req.attr("err").empty() )	{ io.setAttr("err",req.attr("err")); return; }
    }

    io.setAttr("authTokenId",TSYS::int2str(sess.authTkId))->setAttr("ReqHandle",TSYS::uint2str(sess.reqHndl++))->
	setAttr("SeqNumber",TSYS::uint2str(sess.sqNumb++))->setAttr("SeqReqId",TSYS::uint2str(sess.sqReqId++));
    tr.at().messProtIO(io,"OPC_UA");
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

    //> Establish OPC OA connection
    //>> Send HELLO message
    /*XMLNode req("opc.tcp"); req.setAttr("id","HEL")->setAttr("EndPoint",endPoint());
    tr.at().messProtIO(req,"OPC_UA");
    if( !req.attr("err").empty() ) throw TError(nodePath().c_str(),_("HELLO request error: %s"),req.attr("err").c_str());

    //>> Send Open SecureChannel message
    req.setAttr("id","OPN")->setAttr("SecPolicy",secPolicy())->setAttr("SecLifeTm","300000");
    tr.at().messProtIO(req,"OPC_UA");
    if( !req.attr("err").empty() ) throw TError(nodePath().c_str(),_("Open SecureChannel request error: %s"),req.attr("err").c_str());

    //>> Send FindServers message
    req.setAttr("id","FindServers");
    tr.at().messProtIO(req,"OPC_UA");
    if( !req.attr("err").empty() ) throw TError(nodePath().c_str(),_("FindServers request error: %s"),req.attr("err").c_str());
    req.childClear();

    //>> Send CreateSession message
    req.setAttr("id","CreateSession")->setAttr("sesTm","1.2e6")->childAdd("ClientCert")->setText(cert());
    tr.at().messProtIO(req,"OPC_UA");
    if( !req.attr("err").empty() ) throw TError(nodePath().c_str(),_("CreateSession request error: %s"),req.attr("err").c_str());

    //>> Send ActivateSession message
    req.setAttr("id","ActivateSession");
    tr.at().messProtIO(req,"OPC_UA");
    if( !req.attr("err").empty() ) throw TError(nodePath().c_str(),_("ActivateSession request error: %s"),req.attr("err").c_str());

    //>> Read ServerStatus_State
    req.setAttr("id","Read");
    tr.at().messProtIO(req,"OPC_UA");
    if( !req.attr("err").empty() ) throw TError(nodePath().c_str(),_("Read request error: %s"),req.attr("err").c_str());

    //>> Browse RootFolder
    req.setAttr("id","Browse");
    tr.at().messProtIO(req,"OPC_UA");
    if( !req.attr("err").empty() ) throw TError(nodePath().c_str(),_("Browse request error: %s"),req.attr("err").c_str());*/

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
}

void *TMdContr::Task( void *icntr )
{
    TMdContr &cntr = *(TMdContr *)icntr;

#if OSC_DEBUG >= 2
    mess_debug(cntr.nodePath().c_str(),_("Thread <%u> is started. TID: %ld"),pthread_self(),(long int)syscall(224));
#endif

    cntr.endrun_req = false;
    cntr.prc_st = true;

    while( !cntr.endrun_req )
    {
	long long t_cnt = TSYS::curTime();

	//> Update controller's data
	/*cntr.en_res.resRequestR( );
	for( unsigned i_p=0; i_p < cntr.p_hd.size() && !cntr.redntUse(); i_p++ )
	    try
	    {


		//????
	    }
	    catch(TError err)
	    { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
	cntr.en_res.resRelease( );*/

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
	    //if( ctrMkNode("table",opt,-1,"/ndBrws/nd",_("Node"),RWRWR_,"root","DAQ",3,"tp","str","dest","select","select","/ndBrws/ndLst");
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
	if( ctrChkNode(opt,"get",RWRWR_,"root","DAQ",SEQ_RD) )	opt->setText(mBrwsVar);
	if( ctrChkNode(opt,"set",RWRWR_,"root","DAQ",SEQ_WR) )	mBrwsVar = opt->text();
    }
    else if( enableStat() && a_path == "/ndBrws/ndLst" && ctrChkNode(opt) )
    {
	//>> Get current node references by call browse
	string cNodeId = "0:84";
	int stP = mBrwsVar.find("(");
	int stC = mBrwsVar.find(")",stP);;
	if( stP != string::npos && stC != string::npos ) cNodeId = mBrwsVar.substr(stP+1,stC-stP-1);
	XMLNode req("opc.tcp"); req.setAttr("id","Browse")->childAdd("node")->setAttr("nodeId",cNodeId)->setAttr("browseDirection",TSYS::int2str(TProt::BOTH_2));
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
TMdPrm::TMdPrm( string name, TTipParam *tp_prm ) :
    TParamContr(name,tp_prm), p_el("w_attr")
{

}

TMdPrm::~TMdPrm( )
{
    nodeDelAll();
}

void TMdPrm::postEnable( int flag )
{
    TParamContr::postEnable(flag);
    if(!vlElemPresent(&p_el))   vlElemAtt(&p_el);
}

TMdContr &TMdPrm::owner( )	{ return (TMdContr&)TParamContr::owner(); }

void TMdPrm::enable()
{
    if( enableStat() )	return;

    TParamContr::enable();

    owner().prmEn( id(), true );
}

void TMdPrm::disable()
{
    if( !enableStat() )  return;

    owner().prmEn( id(), false );

    TParamContr::disable();

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
	ctrMkNode("fld",opt,-1,"/prm/cfg/VAR_LS",cfg("VAR_LS").fld().descr(),RWRWR_,"root","DAQ",1,
	    "help",_("Variables and it containers (Objects) list. All variables will put into the parameter attributes list.\n"
		"Variables writed by separated lines into format: [ns:id].\n"
		"Where:\n"
		"  ns - name space number;\n"
		"  id - node identifier into number or string.\n"
		"Example:\n"
		"  '0:84' - root folder;\n"
		"  '0:0x54' - root folder in hex number;\n"
		"  '3:BasicDevices2' - basic devices node into namespace 3."));
	return;
    }

    TParamContr::cntrCmdProc(opt);
}

void TMdPrm::vlArchMake( TVal &val )
{
    if( val.arch().freeStat() ) return;
    val.arch().at().setSrcMode(TVArchive::PassiveAttr,val.arch().at().srcData());
    val.arch().at().setPeriod((long long)(owner().period()*1000000));
    val.arch().at().setHardGrid( true );
    val.arch().at().setHighResTm( true );
}
