
//OpenSCADA system module DAQ.SNMP file: snmp.cpp
/***************************************************************************
 *   Copyright (C) 2006-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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

#include <sys/times.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

#undef PACKAGE_BUGREPORT
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION

#include <terror.h>
#include <tsys.h>
#include <tmess.h>
#include <ttiparam.h>
#include <tdaqs.h>

#include "snmp_client.h"

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"SNMP"
#define MOD_NAME	_("SNMP client")
#define MOD_TYPE	SDAQ_ID
#define VER_TYPE	SDAQ_VER
#define MOD_VER		"0.7.0"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("Allow realization of SNMP client service.")
#define LICENSE		"GPL2"
//*************************************************

SNMP_DAQ::TTpContr *SNMP_DAQ::mod;  //Pointer for direct access to the module

extern "C"
{
    TModule::SAt module( int n_mod )
    {
	if(n_mod == 0)	return TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE);
	return TModule::SAt("");
    }

    TModule *attach( const TModule::SAt &AtMod, const string &source )
    {
	if(AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE)) return new SNMP_DAQ::TTpContr(source);
	return NULL;
    }
}

using namespace SNMP_DAQ;

//*************************************************
//* TTpContr                                      *
//*************************************************
TTpContr::TTpContr( string name ) : TTipDAQ(MOD_ID)
{
    mod		= this;

    mName	= MOD_NAME;
    mType	= MOD_TYPE;
    mVers	= MOD_VER;
    mAuthor	= AUTHORS;
    mDescr	= DESCRIPTION;
    mLicense	= LICENSE;
    mSource	= name;

    //Once init of Net-SNMP
    init_snmp("OpenSCADA SNMP client");
}

TTpContr::~TTpContr( )	{ }

void TTpContr::load_( )
{
    //Load parameters from command line

}

void TTpContr::postEnable( int flag )
{
    TTipDAQ::postEnable(flag);

    //Controler's bd structure
    fldAdd(new TFld("PRM_BD",_("Parameteres table"),TFld::String,TFld::NoFlag,"30",""));
    fldAdd(new TFld("SCHEDULE",_("Acquisition schedule"),TFld::String,TFld::NoFlag,"100","1"));
    fldAdd(new TFld("PRIOR",_("Gather task priority"),TFld::Integer,TFld::NoFlag,"2","0","-1;99"));
    fldAdd(new TFld("ADDR",_("Remote host address"),TFld::String,TFld::NoFlag,"30","localhost"));
    fldAdd(new TFld("RETR",_("Retries"),TFld::Integer,TFld::NoFlag,"1","1","0;10"));
    fldAdd(new TFld("TM",_("Timeout (sec)"),TFld::Integer,TFld::NoFlag,"1","3","1;10"));
    fldAdd(new TFld("VER",_("SNMP version"),TFld::String,TFld::Selected,"2","1","1;2c;2u;3","SNMPv1;SNMPv2c;SNMPv2u;SNMPv3"));
    fldAdd(new TFld("COMM",_("Server community/user"),TFld::String,TFld::NoFlag,"20","public"));
    fldAdd(new TFld("V3",_("V3 parameters"),TFld::String,TFld::NoFlag,"50","authNoPriv:MD5::DES:"));
    fldAdd(new TFld("PATTR_LIM",_("Parameter's attributes limit"),TFld::Integer,TFld::NoFlag,"3","100","10;10000"));

    //Parameter type bd structure
    int t_prm = tpParmAdd("std", "PRM_BD", _("Standard"), true);
    tpPrmAt(t_prm).fldAdd(new TFld("OID_LS",_("OID list (next line separated)"),TFld::String,TFld::FullText|TCfg::NoVal,"100000",""));
}

TController *TTpContr::ContrAttach( const string &name, const string &daq_db )	{ return new TMdContr(name, daq_db, this); }

//*************************************************
//* TMdContr                                      *
//*************************************************
TMdContr::TMdContr(string name_c, const string &daq_db, TElem *cfgelem) :
    TController(name_c,daq_db,cfgelem),
    mPrior(cfg("PRIOR").getId()), mPattrLim(cfg("PATTR_LIM").getId()), mRetr(cfg("RETR").getId()), mTm(cfg("TM").getId()),
    prcSt(false), callSt(false), endrunReq(false), prmEnErr(false), tmGath(0), acqErr(dataRes)
{
    pthread_mutexattr_t attrM;
    pthread_mutexattr_init(&attrM);
    pthread_mutexattr_settype(&attrM, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&enRes, &attrM);
    pthread_mutex_init(&dataRes, &attrM);
    pthread_mutexattr_destroy(&attrM);

    cfg("PRM_BD").setS("SNMPPrm_"+name_c);
}

TMdContr::~TMdContr( )
{
    if(run_st) stop();

    pthread_mutex_destroy(&enRes);
    pthread_mutex_destroy(&dataRes);
}

string TMdContr::getStatus( )
{
    string rez = TController::getStatus();
    if(startStat() && !redntUse()) {
	if(!acqErr.getVal().empty())	rez = acqErr.getVal();
	else {
	    if(callSt)	rez += TSYS::strMess(_("Call now. "));
	    if(period())rez += TSYS::strMess(_("Call by period: %s. "),tm2s(1e-3*period()).c_str());
	    else rez += TSYS::strMess(_("Call next by cron '%s'. "),tm2s(TSYS::cron(cron()),"%d-%m-%Y %R").c_str());
	    rez += TSYS::strMess(_("Spent time: %s."),tm2s(tmGath).c_str());
	}
    }

    return rez;
}

struct snmp_session *TMdContr::getSess( )
{
    //Session init
    snmp_sess_init(&session);
    session.version = SNMP_VERSION_1;
    if(ver() == "1")		session.version = SNMP_VERSION_1;
    else if(ver() == "2c")	session.version = SNMP_VERSION_2c;
    else if(ver() == "2u")	session.version = SNMP_VERSION_2u;
    else if(ver() == "3")	session.version = SNMP_VERSION_3;
    wAddr = TSYS::strParse(cfg("ADDR").getS(), 0, ":");
    session.peername = (char *)wAddr.c_str();
    session.retries = mRetr;
    session.timeout = mTm*1000000;
    if(session.version != SNMP_VERSION_3) {
	wComm = cfg("COMM").getS();
	session.community = (u_char*)wComm.c_str();
	session.community_len = wComm.size();
    }
    else {
	wComm = cfg("COMM").getS();
	session.securityName = (char*)wComm.c_str();
	session.securityNameLen = strlen(session.securityName);

	session.securityLevel = SNMP_SEC_LEVEL_NOAUTH;
	if(secLev() == "authNoPriv")	session.securityLevel = SNMP_SEC_LEVEL_AUTHNOPRIV;
	else if(secLev() == "authPriv")	session.securityLevel = SNMP_SEC_LEVEL_AUTHPRIV;

	if(session.securityLevel != SNMP_SEC_LEVEL_NOAUTH) {
	    if(secAuthProto() == "SHA") {
		session.securityAuthProto = usmHMACSHA1AuthProtocol;
		session.securityAuthProtoLen = sizeof(usmHMACSHA1AuthProtocol)/sizeof(oid);
	    }
	    else {
		session.securityAuthProto = usmHMACMD5AuthProtocol;
		session.securityAuthProtoLen = sizeof(usmHMACMD5AuthProtocol)/sizeof(oid);
	    }
	    session.securityAuthKeyLen = USM_AUTH_KU_LEN;

	    string w_sauth = secAuthPass();
	    if(generate_Ku(session.securityAuthProto, session.securityAuthProtoLen, (u_char*)w_sauth.data(), w_sauth.size(),
		    session.securityAuthKey, &session.securityAuthKeyLen) != SNMPERR_SUCCESS)
		throw TError(nodePath().c_str(),_("Error generating Ku from authentication pass phrase."));
	}

	if(session.securityLevel == SNMP_SEC_LEVEL_AUTHPRIV) {
#ifdef USM_PRIV_PROTO_AES_LEN
	    if(secPrivProto() == "AES") {
		session.securityPrivProto = usmAESPrivProtocol;
		session.securityPrivProtoLen = sizeof(usmAESPrivProtocol)/sizeof(oid);
	    }
	    else
#endif
	    {
		session.securityPrivProto = usmDESPrivProtocol;
		session.securityPrivProtoLen = sizeof(usmDESPrivProtocol)/sizeof(oid);
	    }
	    session.securityPrivKeyLen = USM_PRIV_KU_LEN;

	    string w_spriv = secPrivPass();
	    if(generate_Ku(session.securityPrivProto, session.securityPrivProtoLen, (u_char*)w_spriv.data(), w_spriv.size(),
		    session.securityPrivKey, &session.securityPrivKeyLen) != SNMPERR_SUCCESS)
		throw TError(nodePath().c_str(),_("Error generating Ku from privacy pass phrase."));
	}
    }

    return &session;
}

string TMdContr::secLev( )	{ return TSYS::strParse(cfg("V3").getS(), 0, ":"); }

void TMdContr::setSecLev( const string &vl )
{
    cfg("V3").setS(vl+":"+secAuthProto()+":"+secAuthPass()+":"+secPrivProto()+":"+secPrivPass());
}

string TMdContr::secAuthProto( ){ return TSYS::strParse(cfg("V3").getS(), 1, ":"); }

void TMdContr::setSecAuthProto( const string &vl )
{
    cfg("V3").setS(secLev()+":"+vl+":"+secAuthPass()+":"+secPrivProto()+":"+secPrivPass());
}

string TMdContr::secAuthPass( )	{ return TSYS::strParse(cfg("V3").getS(), 2, ":"); }

void TMdContr::setSecAuthPass( const string &vl )
{
    cfg("V3").setS(secLev()+":"+secAuthProto()+":"+vl+":"+secPrivProto()+":"+secPrivPass());
}

string TMdContr::secPrivProto( ){ return TSYS::strParse(cfg("V3").getS(), 3, ":"); }

void TMdContr::setSecPrivProto( const string &vl )
{
    cfg("V3").setS(secLev()+":"+secAuthProto()+":"+secAuthPass()+":"+vl+":"+secPrivPass());
}

string TMdContr::secPrivPass( )	{ return TSYS::strParse(cfg("V3").getS(), 4, ":"); }

void TMdContr::setSecPrivPass( const string &vl )
{
    cfg("V3").setS(secLev()+":"+secAuthProto()+":"+secAuthPass()+":"+secPrivProto()+":"+vl);
}

TParamContr *TMdContr::ParamAttach( const string &name, int type ) { return new TMdPrm(name,&owner().tpPrmAt(type)); }

void TMdContr::enable_( )	{ prmEnErr = false; }

void TMdContr::start_( )
{
    //Schedule process
    mPer = TSYS::strSepParse(cron(), 1, ' ').empty() ? vmax(0,(int64_t)(1e9*s2r(cron()))) : 0;

    getSess();

    //Start the gathering data task
    if(!prcSt) SYS->taskCreate(nodePath('.',true), mPrior, TMdContr::Task, this);
}

void TMdContr::stop_( )
{
    //Stop the request and calc data task
    if(prcSt) SYS->taskDestroy(nodePath('.',true), &endrunReq);
}

void TMdContr::prmEn( TMdPrm *prm, bool val )
{
    unsigned i_prm;

    MtxAlloc res(enRes, true);
    for(i_prm = 0; i_prm < pHd.size(); i_prm++)
	if(&pHd[i_prm].at() == prm) break;

    if(val && i_prm >= pHd.size()) pHd.push_back(prm);
    if(!val && i_prm < pHd.size()) pHd.erase(pHd.begin()+i_prm);
}

void *TMdContr::Task( void *icntr )
{
    TMdContr &cntr = *(TMdContr *)icntr;

    string	daqerr;

    //Start SNMP-net session
    void *ss =  snmp_sess_open(cntr.getSess());
    if(!ss) { mess_err(mod->nodePath().c_str(), "%s", _("Error SNMP session open.")); return NULL; }

    cntr.endrunReq = false;
    cntr.prcSt = true;

    while(!cntr.endrunReq) {
	cntr.callSt = true;
	int64_t t_cnt = TSYS::curTime();

	// Update controller's data
	daqerr.clear();
	MtxAlloc res(cntr.enRes, true);
	for(unsigned i_p = 0; i_p < cntr.pHd.size() && !cntr.redntUse(); i_p++)
	    try { cntr.pHd[i_p].at().upVal(ss); }
	    catch(TError err) { daqerr = err.mess; }
	res.unlock();
	cntr.tmGath = TSYS::curTime()-t_cnt;
	cntr.callSt = false;

	cntr.acqErr.setVal(daqerr);

	TSYS::taskSleep(cntr.period(), cntr.period()?0:TSYS::cron(cntr.cron()));
    }

    snmp_sess_close(ss);

    cntr.prcSt = false;

    return NULL;
}

string TMdContr::oid2str( oid *ioid, size_t isz, const string &sep )
{
    string rez;
    for(unsigned i_el = 0; i_el < isz; i_el++)
	rez += sep+i2s(ioid[i_el]);

    return rez;
}

void TMdContr::str2oid( const string &str, oid *ioid, size_t &isz, const string &sep )
{
    string sb;
    unsigned n = 0;
    for(int off = 0; ((sb=TSYS::strParse(str,0,sep,&off)).size() || off < (int)str.size()) && n < isz; )
	if(sb.size()) ioid[n++] = s2i(sb);
    isz = n;
}

void TMdContr::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TController::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/cntr/cfg/SCHEDULE",cfg("SCHEDULE").fld().descr(),startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,4,
	    "tp","str","dest","sel_ed","sel_list",TMess::labSecCRONsel(),"help",TMess::labSecCRON());
	ctrMkNode("fld",opt,-1,"/cntr/cfg/PRIOR",cfg("PRIOR").fld().descr(),startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,1,"help",TMess::labTaskPrior());
	ctrMkNode("fld",opt,-1,"/cntr/cfg/ADDR",cfg("ADDR").fld().descr(),startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,2,"tp","str",
	    "help",_("SNMP agent host in IP address or domain host name.\nAlso you can set port like \"localhost:161\""));
	ctrMkNode("fld",opt,-1,"/cntr/cfg/TM",cfg("TM").fld().descr(),startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID);
	ctrMkNode("fld",opt,-1,"/cntr/cfg/VER",cfg("VER").fld().descr(),startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID);
	ctrMkNode("fld",opt,-1,"/cntr/cfg/COMM",cfg("COMM").fld().descr(),startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,2,"tp","str",
	    "help",_("Community group or user."));
	ctrMkNode("fld",opt,-1,"/cntr/cfg/PATTR_LIM",cfg("PATTR_LIM").fld().descr(),startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID);
	ctrRemoveNode(opt,"/cntr/cfg/V3");
	if(ver() == "3") {
	    ctrMkNode("fld",opt,-1,"/cntr/cfg/SecLev",_("Security level"),startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,5,"tp","str","idm","1","dest","select",
		"sel_id","noAurhNoPriv;authNoPriv;authPriv","sel_list",_("No auth/No privacy;Auth/No privacy;Auth/Privacy"));
	    if(secLev() != "noAurhNoPriv") {
		ctrMkNode("fld",opt,-1,"/cntr/cfg/AuthProto",_("Auth"),startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,3,"tp","str","dest","select","sel_list","MD5;SHA");
		ctrMkNode("fld",opt,-1,"/cntr/cfg/AuthPass","",startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,1,"tp","str");
	    }
	    if(secLev() == "authPriv") {
#ifdef USM_PRIV_PROTO_AES_LEN
		const char *prtLs = "DES;AES";
#else
		const char *prtLs = "DES";
#endif
		ctrMkNode("fld",opt,-1,"/cntr/cfg/PrivProto",_("Privacy"),startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,3,"tp","str","dest","select","sel_list",prtLs);
		ctrMkNode("fld",opt,-1,"/cntr/cfg/PrivPass","",startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,1,"tp","str");
	    }
	}
        return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/cntr/cfg/SecLev") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(secLev());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	setSecLev(opt->text());
    }
    else if(a_path == "/cntr/cfg/AuthProto") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(secAuthProto());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	setSecAuthProto(opt->text());
    }
    else if(a_path == "/cntr/cfg/AuthPass") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(string(secAuthPass().size(),'*'));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	setSecAuthPass(opt->text());
    }
    else if(a_path == "/cntr/cfg/PrivProto") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(secPrivProto());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	setSecPrivProto(opt->text());
    }
    else if(a_path == "/cntr/cfg/PrivPass") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(string(secPrivPass().size(),'*'));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	setSecPrivPass(opt->text());
    }
    else TController::cntrCmdProc(opt);
}


//*************************************************
//* TMdPrm                                        *
//*************************************************
TMdPrm::TMdPrm(string name, TTipParam *tp_prm) :
    TParamContr(name,tp_prm), p_el("w_attr")
{

}

TMdPrm::~TMdPrm( )	{ nodeDelAll(); }

void TMdPrm::postEnable( int flag )
{
    TParamContr::postEnable(flag);
    if(!vlElemPresent(&p_el)) vlElemAtt(&p_el);
}

TMdContr &TMdPrm::owner( )	{ return (TMdContr&)TParamContr::owner(); }

void TMdPrm::enable( )
{
    if(enableStat()) return;

    TParamContr::enable();

    owner().prmEn(this, true);

    parseOIDList(OIDList());

    //Init attributes call
    // Start SNMP-net session
    if(owner().enableStat() || !owner().prmEnErr) {
	void *ss =  snmp_sess_open(owner().getSess());
	if(ss) {
	    try { upVal(ss,true); }
	    catch(TError err) {
		owner().prmEnErr = true;
		mess_err(nodePath().c_str(),"%s",err.mess.c_str());
	    }
	    snmp_sess_close(ss);
	}
    }
}

void TMdPrm::disable( )
{
    if(!enableStat())  return;

    owner().prmEn(this, false);

    TParamContr::disable();

    //Set EVAL to parameter attributes
    vector<string> ls;
    elem().fldList(ls);
    for(unsigned i_el = 0; i_el < ls.size(); i_el++)
	vlAt(ls[i_el]).at().setS(EVAL_STR,0,true);
}

void TMdPrm::upVal( void *ss, bool onlyInit )
{
    int		el_cnt = 0;
    string	soid;
    char	tbuf[100];
    struct snmp_pdu *response;
    struct variable_list *var;
    oid oid_root[MAX_OID_LEN], oid_next[MAX_OID_LEN];
    size_t oid_root_len = MAX_OID_LEN, oid_next_len = MAX_OID_LEN;

    vector<string> als;

    for(unsigned ioid = 0; ioid < lsOID().size(); ioid++) {
	oid_root_len = oid_next_len = lsOID()[ioid].size()/sizeof(oid);
	memmove(oid_root,lsOID()[ioid].data(),oid_root_len*sizeof(oid));
	memmove(oid_next,oid_root,oid_root_len*sizeof(oid));

	bool isScalar = oid_root_len && (oid_root[oid_root_len-1] == 0);
	bool running = true;
	while(running && (el_cnt++) < owner().pAttrLimit()) {
	    struct snmp_pdu *pdu = snmp_pdu_create(isScalar ? SNMP_MSG_GET : SNMP_MSG_GETNEXT);
	    snmp_add_null_var(pdu, oid_next, oid_next_len);
	    int status = snmp_sess_synch_response(ss, pdu, &response);
	    if(status == STAT_SUCCESS && response && response->errstat == SNMP_ERR_NOERROR)
		for(var = response->variables; var; var = var->next_variable) {
		    if((var->name_length < oid_root_len) || (memcmp(oid_root,var->name,oid_root_len*sizeof(oid)) != 0))
		    {
			running = false;
			continue;
		    }
		    // Get or create element
		    soid = owner().oid2str(var->name,var->name_length);
		    als.push_back(soid);
		    if(!elem().fldPresent(soid)) {
			TFld::Type tp = (TFld::Type)-1;
			unsigned flg = TVal::DirWrite;
			string selIds, selLabs;

			snprint_objid(tbuf,sizeof(tbuf),var->name,var->name_length);

			struct tree *subtree = get_tree_head();
			if(subtree) subtree = get_tree(var->name, var->name_length, subtree);
			if(subtree) {
			    if(!(subtree->access == MIB_ACCESS_READWRITE || subtree->access == MIB_ACCESS_WRITEONLY))
				flg |= TFld::NoWrite;
			    if(subtree->enums) {
				flg |= TFld::Selected;
				for(struct enum_list *enums = subtree->enums; enums; enums = enums->next)
				{
				    selIds += i2s(enums->value)+";";
				    selLabs += string(enums->label)+";";
				}
			    }
			}

			switch(var->type)
			{
			    case ASN_BOOLEAN:		tp = TFld::Boolean;	break;
			    case ASN_INTEGER:		tp = TFld::Integer;	break;
			    case ASN_OPAQUE_FLOAT:
			    case ASN_OPAQUE_DOUBLE:	tp = TFld::Real;	break;
			    case ASN_GAUGE:
			    case ASN_COUNTER:
			    case ASN_TIMETICKS:
			    case ASN_UINTEGER:		tp = TFld::Real;	break;
			    case ASN_OCTET_STR:
			    case ASN_OPAQUE:
			    case ASN_IPADDRESS:
			    case ASN_OBJECT_ID:
			    case ASN_COUNTER64:
			    case ASN_OPAQUE_COUNTER64:
			    case ASN_OPAQUE_U64:
			    case ASN_OPAQUE_I64:
			    case ASN_BIT_STR:		tp = TFld::String;	break;
			    case SNMP_ENDOFMIBVIEW:
			    case SNMP_NOSUCHOBJECT:
			    case SNMP_NOSUCHINSTANCE:	running = false;	break;
			}
			if(tp >= 0)
			    elem().fldAdd(new TFld(soid.c_str(),tbuf,tp,flg,"","",
				selIds.c_str(),selLabs.c_str(),(i2s(var->type)+"\n"+(subtree->hint?subtree->hint:"")).c_str()));
		    }
		    // Set value
		    if(!onlyInit) {
			AutoHD<TVal> attr = vlAt(soid);
			
			switch(var->type)
			{
			    case ASN_BOOLEAN: attr.at().setB((bool)*var->val.integer, 0, true);	break;
			    case ASN_INTEGER: attr.at().setI(*var->val.integer, 0, true);	break;
			    case ASN_GAUGE:
			    case ASN_COUNTER:
			    case ASN_TIMETICKS:
			    case ASN_UINTEGER: attr.at().setR(*(unsigned long*)var->val.integer, 0, true);	break;
			    case ASN_OCTET_STR:
			    case ASN_OPAQUE: {
				string hint = TSYS::strLine(attr.at().fld().reserve(),1);
				size_t fndPos;
				if((fndPos=hint.find("x")) != string::npos)
				    hint = TSYS::strDecode(string((char*)var->val.string,var->val_len), TSYS::Bin,
					((fndPos+1)<hint.size())?hint.substr(fndPos+1,1):"");
				else hint = string((char*)var->val.string,var->val_len);
				attr.at().setS(hint, 0, true);
				break;
			    }
			    case ASN_IPADDRESS: {
				u_char *ip = (u_char*)var->val.string;
				attr.at().setS(TSYS::strMess("%d.%d.%d.%d",ip[0],ip[1],ip[2],ip[3]), 0, true);
				break;
			    }
			    case ASN_OBJECT_ID:
				snprint_objid(tbuf, sizeof(tbuf), (oid*)(var->val.objid), var->val_len/sizeof(oid));
				attr.at().setS(tbuf, 0, true);
				break;
			    case ASN_COUNTER64:
			    case ASN_OPAQUE_COUNTER64:
			    case ASN_OPAQUE_U64:
				printU64(tbuf,(struct counter64*)var->val.counter64);
				attr.at().setS(tbuf,0,true);
				break;
			    case ASN_OPAQUE_I64:
				printI64(tbuf,(struct counter64 *)var->val.counter64);
				attr.at().setS(tbuf,0,true);
				break;
			    case ASN_BIT_STR:
				snprint_bitstring(tbuf, sizeof(tbuf), var, NULL, NULL, NULL);
				attr.at().setS(tbuf,0,true);
				break;
			    case ASN_OPAQUE_FLOAT:
				if(var->val.floatVal) attr.at().setR(*var->val.floatVal,0,true);
				break;
			    case ASN_OPAQUE_DOUBLE:
				if(var->val.doubleVal) attr.at().setR(*var->val.doubleVal,0,true);
				break;
			    default:
				mess_warning(owner().nodePath().c_str(),_("ASN type '%d' do not handled."),var->type);
				//print_objid(var->name,var->name_length);
				//print_value(var->name,var->name_length,var);
				break;
			}
		    }
		    if(isScalar) { running = false; break; }
		    if(running) {
			memmove((char*)oid_next, (char*)var->name, var->name_length*sizeof(oid));
			oid_next_len = var->name_length;
		    }
		}
	    else if(status == STAT_TIMEOUT)
		throw TError(owner().nodePath().c_str(),TSYS::strMess(_("10:Timeout: No Response from %s."),owner().session.peername).c_str());
	    else running = 0;
	    if(response) snmp_free_pdu(response);
	}
    }

    //Check for delete DAQ parameter's attributes
    for(int i_p = 0; onlyInit && i_p < (int)elem().fldSize(); i_p++)
    {
	unsigned i_l;
	for(i_l = 0; i_l < als.size(); i_l++)
	    if(elem().fldAt(i_p).name() == als[i_l])
		break;
	if(i_l >= als.size())
	    try{ elem().fldDel(i_p); i_p--; }
	    catch(TError err){ mess_warning(err.cat.c_str(),err.mess.c_str()); }
    }
}

void TMdPrm::load_( )	{ TParamContr::load_(); }

void TMdPrm::parseOIDList(const string &ioid)
{
    cfg("OID_LS").setS(ioid);

    oid tmpoid[MAX_OID_LEN];
    size_t tmpoid_len = MAX_OID_LEN;

    ls_oid.clear();

    string sel;
    for(int ioff = 0; (sel=TSYS::strSepParse(OIDList(),0,'\n',&ioff)).size(); )
    {
	if(sel[0] == '#') continue;
	tmpoid_len = MAX_OID_LEN;
	if(snmp_parse_oid(sel.c_str(),tmpoid,&tmpoid_len))
	    ls_oid.push_back(string((char*)tmpoid,tmpoid_len*sizeof(oid)));
    }
}

void TMdPrm::cntrCmdProc( XMLNode *opt )
{
    //Service commands process
    string a_path = opt->attr("path");
    if(a_path.substr(0,6) == "/serv/")	{ TParamContr::cntrCmdProc(opt); return; }

    //Get page info
    if(opt->name() == "info") {
	TParamContr::cntrCmdProc(opt);
	ctrMkNode2("fld",opt,-1,"/prm/cfg/OID_LS",cfg("OID_LS").fld().descr(),enableStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,"SnthHgl","1","rows","8",
	    "help",_("SNMP OID list, include directories for get all subitems. OID can write in the methods:\n"
		"  \".1.3.6.1.2.1.1\" - direct code addressing for object \"System\";\n"
		"  \".iso.org.dod.internet.mgmt.mib-2.system\" - full symbol to direct code addressing for object \"System\";\n"
		"  \"system.sysDescr.0\" - simple, not full path, addressing from root alias (object \"System\");\n"
		"  \"SNMPv2-MIB::sysDescr.0\" - addressing from MIB base by module name for \"system.sysDescr.0\"."),NULL);
	if(get_tree_head())
	    ctrMkNode2("fld",opt,-1,"/prm/cfg/MIB",_("MIB Tree"),enableStat()?0:RWRW__,"root",SDAQ_ID,"dest","select","select","/prm/cfg/MIB_lst",NULL);
	return;
    }

    //Process command to page
    if(a_path == "/prm/cfg/OID_LS" && ctrChkNode(opt,"SnthHgl",RWRWR_,"root",SDAQ_ID,SEC_RD))
	opt->childAdd("rule")->setAttr("expr","^#[^\n]*")->setAttr("color","gray")->setAttr("font_italic","1");
    else if(a_path == "/prm/cfg/MIB") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SDAQ_ID,SEC_RD)) opt->setText(TBDS::genDBGet(nodePath()+"selOID","",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",SDAQ_ID,SEC_WR)) {
	    if(opt->text() == _("<<Append current>>")) {
		oid oidn[MAX_OID_LEN];
		size_t oidn_len = MAX_OID_LEN;
		string baseIt = TBDS::genDBGet(nodePath()+"selOID","",opt->attr("user"));
		if(snmp_parse_oid(baseIt.c_str(),oidn,&oidn_len)) {
		    string vLs = OIDList(), vS;
		    for(int off = 0; (vS=TSYS::strLine(vLs,0,&off)).size() && vS != baseIt; ) ;
		    if(vS.empty()) setOIDList(vLs+((vLs.size() && vLs[vLs.size()-1] != '\n')?"\n":"")+baseIt);
		}
	    }
	    else TBDS::genDBSet(nodePath()+"selOID", opt->text(), opt->attr("user"));
	}
    }
    else if(a_path == "/prm/cfg/MIB_lst" && ctrChkNode(opt)) {
	oid oidn[MAX_OID_LEN];
	size_t oidn_len = MAX_OID_LEN;
	string baseIt = TBDS::genDBGet(nodePath()+"selOID","",opt->attr("user")), baseIt_, baseIt_s;
	struct tree *subTr = get_tree_head();
	if(snmp_parse_oid(baseIt.c_str(),oidn,&oidn_len)) subTr = get_tree(oidn, oidn_len, subTr);
	else baseIt = ".iso";

	opt->childAdd("el")->setText(_("<<Append current>>"));

	//Previous
	for(int off = 1; (baseIt_=TSYS::strParse(baseIt,0,".",&off)).size(); )
	    opt->childAdd("el")->setText((baseIt_s+="."+baseIt_));

	//Childs
	for(struct tree *chtree = subTr->child_list; chtree; chtree = chtree->next_peer)
	    opt->childAdd("el")->setText(baseIt+"."+chtree->label);
    }
    else TParamContr::cntrCmdProc(opt);
}

void TMdPrm::vlSet( TVal &vo, const TVariant &vl, const TVariant &pvl )
{
    if(!enableStat() || !owner().startStat()) { vo.setS(EVAL_STR, 0, true); return; }

    //Send to active reserve station
    if(owner().redntUse()) {
	if(vl == pvl) return;
	XMLNode req("set");
	req.setAttr("path",nodePath(0,true)+"/%2fserv%2fattr")->childAdd("el")->setAttr("id",vo.name())->setText(vl.getS());
	SYS->daq().at().rdStRequest(owner().workId(),req);
	return;
    }

    //Direct write
    void *ss;
    char vtp = 0;
    oid oidn[MAX_OID_LEN];
    size_t oidn_len = MAX_OID_LEN;
    struct snmp_pdu *response = NULL;

    struct snmp_pdu *pdu = snmp_pdu_create(SNMP_MSG_SET);
    owner().str2oid(vo.name(), oidn, oidn_len);

    TVariant wVl = vl;
    switch(s2i(vo.fld().reserve()))
    {
	case ASN_INTEGER:	vtp = 'i';	break;
	case ASN_GAUGE:		vtp = 'u';	break;
	case ASN_COUNTER:	vtp = 'c';	break;
	case ASN_TIMETICKS:	vtp = 't';	break;
	case ASN_UINTEGER:	vtp = '3';	break;
	case ASN_OCTET_STR:
	case ASN_OPAQUE:	vtp = 's';	break;
	case ASN_IPADDRESS:	vtp = 'a';	break;
	case ASN_OBJECT_ID:	vtp = 'o';	break;
	case ASN_COUNTER64:
	case ASN_OPAQUE_COUNTER64:
	case ASN_OPAQUE_U64:	vtp = 'C';	break;
	case ASN_OPAQUE_I64:	break;
	case ASN_BIT_STR:	vtp = 'b';	break;
	case ASN_OPAQUE_FLOAT:	break;
	case ASN_OPAQUE_DOUBLE:	break;
	default:		break;
    }
    if(vtp) {
	if(!(ss=snmp_sess_open(&owner().session))) return;

	snmp_add_var(pdu, oidn, oidn_len, vtp, vl.getS().c_str());
	int status = snmp_sess_synch_response(ss, pdu, &response);
	if(status == STAT_TIMEOUT)
	    owner().acqErr.setVal(TSYS::strMess(_("10:Timeout: No Response from %s."),owner().session.peername).c_str());
	else if(response && response->errstat == SNMP_ERR_NOSUCHNAME)
	    owner().acqErr.setVal(TSYS::strMess(_("11:No authorized name.")));
	if(response) snmp_free_pdu(response);
	snmp_sess_close(ss);
    }
}

void TMdPrm::vlArchMake(TVal &val)
{
    TParamContr::vlArchMake(val);

    if(val.arch().freeStat()) return;
    val.arch().at().setSrcMode(TVArchive::PassiveAttr);
    val.arch().at().setPeriod(owner().period() ? owner().period()/1000 : 1000000);
    val.arch().at().setHardGrid(true);
    val.arch().at().setHighResTm(true);
}
