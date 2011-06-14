
//OpenSCADA system module DAQ.SNMP file: snmp.cpp
/***************************************************************************
 *   Copyright (C) 2006-2011 by Roman Savochenko                           *
 *   rom_as@fromru.com                                                     *
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
#include <getopt.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

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
#define MOD_VER		"0.6.0"
#define AUTORS		_("Roman Savochenko")
#define DESCRIPTION	_("Allow realising of SNMP client service.")
#define LICENSE		"GPL2"
//*************************************************

SNMP_DAQ::TTpContr *SNMP_DAQ::mod;  //Pointer for direct access to module

extern "C"
{
    TModule::SAt module(int n_mod)
    {
	if(n_mod == 0)	return TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE);
	return TModule::SAt("");
    }

    TModule *attach(const TModule::SAt &AtMod, const string &source)
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
    mAutor	= AUTORS;
    mDescr	= DESCRIPTION;
    mLicense	= LICENSE;
    mSource	= name;

    //> Once init of Net-SNMP
    init_snmp("OpenSCADA SNMP client");
}

TTpContr::~TTpContr( )
{

}

void TTpContr::load_( )
{
    //> Load parameters from command line

}

void TTpContr::postEnable(int flag)
{
    TTipDAQ::postEnable(flag);

    //> Controler's bd structure
    fldAdd(new TFld("PRM_BD",_("Parameteres table"),TFld::String,TFld::NoFlag,"30",""));
    fldAdd(new TFld("SCHEDULE",_("Acquisition schedule"),TFld::String,TFld::NoFlag,"100","1"));
    fldAdd(new TFld("PRIOR",_("Gather task priority"),TFld::Integer,TFld::NoFlag,"2","0","-1;99"));
    fldAdd(new TFld("ADDR",_("Remote host address"),TFld::String,TFld::NoFlag,"30","localhost"));
    fldAdd(new TFld("RETR",_("Retries"),TFld::Integer,TFld::NoFlag,"1","1","0;10"));
    fldAdd(new TFld("TM",_("Timeout (sec)"),TFld::Integer,TFld::NoFlag,"1","3","1;10"));
    fldAdd(new TFld("VER",_("SNMP version"),TFld::String,TFld::Selected,"2","1","1;2c;2u;3","SNMPv1;SNMPv2c;SNMPv2u;SNMPv3"));
    fldAdd(new TFld("COMM",_("Server community/user"),TFld::String,TFld::NoFlag,"20","public"));
    fldAdd(new TFld("V3",_("V3 parameters"),TFld::String,TFld::NoFlag,"50","authNoPriv:MD5::DES:"));
    fldAdd(new TFld("PATTR_LIM",_("Param's attributes limit"),TFld::Integer,TFld::NoFlag,"3","100","10;10000"));

    //> Parameter type bd structure
    int t_prm = tpParmAdd("std", "PRM_BD", _("Standard"));
    tpPrmAt(t_prm).fldAdd(new TFld("OID_LS",_("OID list (next line separated)"),TFld::String,TFld::FullText|TCfg::NoVal,"100",""));
}

TController *TTpContr::ContrAttach(const string &name, const string &daq_db)
{
    return new TMdContr(name,daq_db,this);
}

//*************************************************
//* TMdContr                                      *
//*************************************************
TMdContr::TMdContr(string name_c, const string &daq_db, ::TElem *cfgelem) :
    ::TController(name_c,daq_db,cfgelem),
    m_prior(cfg("PRIOR").getId()), m_pattr_lim(cfg("PATTR_LIM").getId()), m_retr(cfg("RETR").getId()), m_tm(cfg("TM").getId()),
    mSched(cfg("SCHEDULE").getSd()), m_addr(cfg("ADDR").getSd()), m_ver(cfg("VER").getSd()), m_comm(cfg("COMM").getSd()),
    m_V3(cfg("V3").getSd()), prc_st(false), endrun_req(false), tm_gath(0)
{
    cfg("PRM_BD").setS("SNMPPrm_"+name_c);
}

TMdContr::~TMdContr( )
{
    if(run_st) stop();
}

string TMdContr::getStatus( )
{
    string rez = TController::getStatus();
    if(startStat() && !redntUse())
    {
	if(!acq_err.getVal().empty())	rez = acq_err.getVal();
	else
	{
	    if(period()) rez += TSYS::strMess(_("Call by period: %s. "),TSYS::time2str(1e-3*period()).c_str());
            else rez += TSYS::strMess(_("Call next by cron '%s'. "),TSYS::time2str(TSYS::cron(cron()),"%d-%m-%Y %R").c_str());
            rez += TSYS::strMess(_("Spent time: %s."),TSYS::time2str(tm_gath).c_str());
        }
    }
    return rez;
}

string TMdContr::secLev( )
{
    return TSYS::strParse(m_V3, 0, ":");
}

void TMdContr::setSecLev(const string &vl)
{
    m_V3 = vl+":"+secAuthProto()+":"+secAuthPass()+":"+secPrivProto()+":"+secPrivPass();
    modif();
}

string TMdContr::secAuthProto( )
{
    return TSYS::strParse(m_V3, 1, ":");
}

void TMdContr::setSecAuthProto(const string &vl)
{
    m_V3 = secLev()+":"+vl+":"+secAuthPass()+":"+secPrivProto()+":"+secPrivPass();
    modif();
}

string TMdContr::secAuthPass( )
{
    return TSYS::strParse(m_V3, 2, ":");
}

void TMdContr::setSecAuthPass(const string &vl)
{
    m_V3 = secLev()+":"+secAuthProto()+":"+vl+":"+secPrivProto()+":"+secPrivPass();
    modif();
}

string TMdContr::secPrivProto( )
{
    return TSYS::strParse(m_V3, 3, ":");
}

void TMdContr::setSecPrivProto(const string &vl)
{
    m_V3 = secLev()+":"+secAuthProto()+":"+secAuthPass()+":"+vl+":"+secPrivPass();
    modif();
}

string TMdContr::secPrivPass( )
{
    return TSYS::strParse(m_V3, 4, ":");
}

void TMdContr::setSecPrivPass(const string &vl)
{
    m_V3 = secLev()+":"+secAuthProto()+":"+secAuthPass()+":"+secPrivProto()+":"+vl;
    modif();
}

TParamContr *TMdContr::ParamAttach(const string &name, int type)
{
    return new TMdPrm(name,&owner().tpPrmAt(type));
}

void TMdContr::start_( )
{
    //> Schedule process
    mPer = TSYS::strSepParse(mSched, 1, ' ').empty() ? vmax(0,(int64_t)(1e9*atof(mSched.getVal().c_str()))) : 0;

    //> Session init
    snmp_sess_init(&session);
    session.version = SNMP_VERSION_1;
    if(m_ver.getVal() == "1")		session.version = SNMP_VERSION_1;
    else if(m_ver.getVal() == "2c")	session.version = SNMP_VERSION_2c;
    else if(m_ver.getVal() == "2u")	session.version = SNMP_VERSION_2u;
    else if(m_ver.getVal() == "3")	session.version = SNMP_VERSION_3;
    w_addr = TSYS::strParse(m_addr, 0, ":");
    session.peername = (char *)w_addr.c_str();
    session.retries = m_retr;
    session.timeout = m_tm*1000000;
    if(session.version != SNMP_VERSION_3)
    {
	w_comm = m_comm;
	session.community = (u_char*)w_comm.c_str();
	session.community_len = w_comm.size();
    }
    else
    {
	w_comm = m_comm;
	session.securityName = (char*)w_comm.c_str();
	session.securityNameLen = strlen(session.securityName);

	session.securityLevel = SNMP_SEC_LEVEL_NOAUTH;
	if(secLev() == "authNoPriv")	session.securityLevel = SNMP_SEC_LEVEL_AUTHNOPRIV;
	else if(secLev() == "authPriv")	session.securityLevel = SNMP_SEC_LEVEL_AUTHPRIV;

	if(session.securityLevel != SNMP_SEC_LEVEL_NOAUTH)
	{
	    if(secAuthProto() == "SHA")
	    {
		session.securityAuthProto = usmHMACSHA1AuthProtocol;
		session.securityAuthProtoLen = sizeof(usmHMACSHA1AuthProtocol)/sizeof(oid);
	    }
	    else
	    {
		session.securityAuthProto = usmHMACMD5AuthProtocol;
		session.securityAuthProtoLen = sizeof(usmHMACMD5AuthProtocol)/sizeof(oid);
	    }
	    session.securityAuthKeyLen = USM_AUTH_KU_LEN;

	    string w_sauth = secAuthPass();
	    if(generate_Ku(session.securityAuthProto, session.securityAuthProtoLen, (u_char*)w_sauth.data(), w_sauth.size(),
		    session.securityAuthKey, &session.securityAuthKeyLen) != SNMPERR_SUCCESS)
		throw TError(nodePath().c_str(),_("Error generating Ku from authentication pass phrase."));
	}

	if(session.securityLevel == SNMP_SEC_LEVEL_AUTHPRIV)
	{
	    if(secPrivProto() == "AES")
	    {
		session.securityPrivProto = usmAESPrivProtocol;
		session.securityPrivProtoLen = sizeof(usmAESPrivProtocol)/sizeof(oid);
	    }
	    else
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

    //> Start the gathering data task
    if(!prc_st) SYS->taskCreate(nodePath('.',true), m_prior, TMdContr::Task, this, &prc_st);
}

void TMdContr::stop_( )
{
    //> Stop the request and calc data task
    if(prc_st) SYS->taskDestroy(nodePath('.',true), &prc_st, &endrun_req);
}

void TMdContr::prmEn(const string &id, bool val)
{
    ResAlloc res(en_res,true);

    unsigned i_prm;
    for(i_prm = 0; i_prm < p_hd.size(); i_prm++)
	if(p_hd[i_prm].at().id() == id) break;

    if(val && i_prm >= p_hd.size()) p_hd.push_back(at(id));
    if(!val && i_prm < p_hd.size()) p_hd.erase(p_hd.begin()+i_prm);
}

void *TMdContr::Task(void *icntr)
{
    TMdContr &cntr = *(TMdContr *)icntr;

    char	tbuf[100];
    int		el_cnt;
    string	soid, daqerr;
    struct snmp_pdu *response;
    struct variable_list *var;
    oid oid_root[MAX_OID_LEN], oid_next[MAX_OID_LEN];
    size_t oid_root_len = MAX_OID_LEN, oid_next_len = MAX_OID_LEN;

    //> Start SNMP-net session
    void *ss =  snmp_sess_open(&cntr.session);
    if(!ss) { mess_err(mod->nodePath().c_str(), "%s", _("Error SNMP session open.")); return NULL; }

    cntr.endrun_req = false;
    cntr.prc_st = true;

    while(!cntr.endrun_req)
    {
	int64_t t_cnt = TSYS::curTime();

	//>> Update controller's data
	daqerr.clear();
	el_cnt = 0;
	cntr.en_res.resRequestR( );
	for(unsigned i_p = 0; i_p < cntr.p_hd.size() && !cntr.redntUse(); i_p++)
	    try
	    {
		AutoHD<TMdPrm> cprm = cntr.p_hd[i_p];
		for(unsigned ioid = 0; ioid < cprm.at().lsOID().size(); ioid++)
		{
		    oid_root_len = oid_next_len = cprm.at().lsOID()[ioid].size()/sizeof(oid);
		    memmove(oid_root,cprm.at().lsOID()[ioid].c_str(),oid_root_len*sizeof(oid));
		    memmove(oid_next,oid_root,oid_root_len*sizeof(oid));

		    bool running = true;
		    while(running && (el_cnt++) < cntr.pAttrLimit())
		    {
			struct snmp_pdu *pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);
			snmp_add_null_var(pdu, oid_next, oid_next_len);
			int status = snmp_sess_synch_response(ss, pdu, &response);
			if(status == STAT_SUCCESS && response && response->errstat == SNMP_ERR_NOERROR)
			    for(var = response->variables; var; var = var->next_variable)
			    {
				if((var->name_length < oid_root_len) || (memcmp(oid_root,var->name,oid_root_len*sizeof(oid)) != 0))
				{
				    running = false;
				    continue;
				}
				//>> Get or create element
				soid = cntr.oid2str(var->name,var->name_length);
				if(!cprm.at().elem().fldPresent(soid))
				{
				    snprint_objid(tbuf,sizeof(tbuf),var->name,var->name_length);
				    switch(var->type)
				    {
					case ASN_BOOLEAN:
					    cprm.at().elem().fldAdd(new TFld(soid.c_str(),tbuf,TFld::Boolean,TFld::NoWrite));
					    break;
					case ASN_INTEGER:
				    	    cprm.at().elem().fldAdd(new TFld(soid.c_str(),tbuf,TFld::Integer,TVal::DirWrite,"","","","",TSYS::int2str(var->type).c_str()));
			    		    break;
		    			case ASN_OPAQUE_FLOAT:
	    				case ASN_OPAQUE_DOUBLE:
    					    cprm.at().elem().fldAdd(new TFld(soid.c_str(),tbuf,TFld::Real,TVal::DirWrite,"","","","",TSYS::int2str(var->type).c_str()));
					    break;
					case ASN_GAUGE:
					case ASN_COUNTER:
					case ASN_TIMETICKS:
					case ASN_UINTEGER:
					    cprm.at().elem().fldAdd(new TFld(soid.c_str(),tbuf,TFld::Real,TVal::DirWrite,"","","","",TSYS::int2str(var->type).c_str()));
					    break;
					case ASN_OCTET_STR:
					case ASN_OPAQUE:
					case ASN_IPADDRESS:
					case ASN_OBJECT_ID:
					case ASN_COUNTER64:
					case ASN_OPAQUE_COUNTER64:
					case ASN_OPAQUE_U64:
					case ASN_OPAQUE_I64:
					case ASN_BIT_STR:
					    cprm.at().elem().fldAdd(new TFld(soid.c_str(),tbuf,TFld::String,TVal::DirWrite,"","","","",TSYS::int2str(var->type).c_str()));
					    break;
				    }
				}
				//>> Set value
				AutoHD<TVal> attr = cprm.at().vlAt(soid);
				switch(var->type)
				{
				    case ASN_BOOLEAN:
					attr.at().setB((bool)*var->val.integer,0,true);
					break;
				    case ASN_INTEGER:
					attr.at().setI(*var->val.integer,0,true);
					break;
				    case ASN_GAUGE:
				    case ASN_COUNTER:
				    case ASN_TIMETICKS:
				    case ASN_UINTEGER:
					attr.at().setR(*(unsigned long*)var->val.integer,0,true);
					break;
				    case ASN_OCTET_STR:
				    case ASN_OPAQUE:
					attr.at().setS(string((char*)var->val.string,var->val_len),0,true);
					break;
				    case ASN_IPADDRESS:
				    {
    					u_char *ip = (u_char*)var->val.string;
					attr.at().setS(TSYS::strMess("%d.%d.%d.%d",ip[0], ip[1], ip[2], ip[3]),0,true);
					break;
				    }
				    case ASN_OBJECT_ID:
					snprint_objid(tbuf, sizeof(tbuf), (oid*)(var->val.objid), var->val_len/sizeof(oid));
					attr.at().setS(tbuf,0,true);
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
				    case SNMP_ENDOFMIBVIEW:
				    case SNMP_NOSUCHOBJECT:
				    case SNMP_NOSUCHINSTANCE:
					running = false;
					break;
				    default:
					mess_warning(cntr.nodePath().c_str(),_("ASN type '%d' do not handled."),var->type);
					//print_objid(var->name,var->name_length);
					//print_value(var->name,var->name_length,var);
					break;
				}
				if(running)
				{
				    memmove((char*)oid_next, (char*)var->name, var->name_length*sizeof(oid));
				    oid_next_len = var->name_length;
				}
			    }
			else if(status == STAT_TIMEOUT)
			    throw TError(cntr.nodePath().c_str(),TSYS::strMess(_("10:Timeout: No Response from %s."),cntr.session.peername).c_str());
			else running = 0;
			if(response) snmp_free_pdu(response);
		    }
		}
	    }
	    catch(TError err) { daqerr = err.mess; }
	cntr.en_res.resRelease();
	cntr.tm_gath = TSYS::curTime()-t_cnt;

	cntr.acq_err.setVal(daqerr);

	TSYS::taskSleep(cntr.period(),cntr.period()?0:TSYS::cron(cntr.cron()));
    }

    snmp_sess_close(ss);

    cntr.prc_st = false;

    return NULL;
}

string TMdContr::oid2str(oid *ioid, size_t isz)
{
    string rez;
    for(unsigned i_el = 0; i_el < isz; i_el++)
	rez += "_"+TSYS::int2str(ioid[i_el]);
    return rez;
}

void TMdContr::str2oid( const string &str, oid *ioid, size_t &isz )
{
    string sb;
    unsigned n = 0;
    for(int off = 0; ((sb=TSYS::strParse(str,0,"_",&off)).size() || off < (int)str.size()) && n < isz; )
	if(sb.size()) ioid[n++] = atoi(sb.c_str());
    isz = n;
}

void TMdContr::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if(opt->name() == "info")
    {
        TController::cntrCmdProc(opt);
        ctrMkNode("fld",opt,-1,"/cntr/cfg/ADDR",cfg("ADDR").fld().descr(),RWRWR_,"root",SDAQ_ID,2,"tp","str",
	    "help",_("SNMP agent host in IP address or domain host name.\nAlso you can set port like \"localhost:161\""));
        ctrMkNode("fld",opt,-1,"/cntr/cfg/SCHEDULE",cfg("SCHEDULE").fld().descr(),RWRWR_,"root",SDAQ_ID,4,
            "tp","str","dest","sel_ed","sel_list",TMess::labSecCRONsel(),"help",TMess::labSecCRON());
        ctrMkNode("fld",opt,-1,"/cntr/cfg/COMM",cfg("COMM").fld().descr(),RWRWR_,"root",SDAQ_ID,2,"tp","str",
    	    "help",_("Community group or user."));
	ctrRemoveNode(opt,"/cntr/cfg/V3");
	if(m_ver.getVal() == "3")
	{
    	    ctrMkNode("fld",opt,-1,"/cntr/cfg/SecLev",_("Security level"),RWRWR_,"root",SDAQ_ID,5,"tp","str","idm","1","dest","select",
        	"sel_id","noAurhNoPriv;authNoPriv;authPriv","sel_list",_("No auth/No privacy;Auth/No privacy;Auth/Privacy"));
	    if(secLev() != "noAurhNoPriv")
	    {
		ctrMkNode("fld",opt,-1,"/cntr/cfg/AuthProto",_("Auth"),RWRWR_,"root",SDAQ_ID,3,"tp","str","dest","select","sel_list","MD5;SHA");
		ctrMkNode("fld",opt,-1,"/cntr/cfg/AuthPass","",RWRWR_,"root",SDAQ_ID,1,"tp","str");
	    }
	    if(secLev() == "authPriv")
	    {
		ctrMkNode("fld",opt,-1,"/cntr/cfg/PrivProto",_("Privacy"),RWRWR_,"root",SDAQ_ID,3,"tp","str","dest","select","sel_list","DES;AES");
		ctrMkNode("fld",opt,-1,"/cntr/cfg/PrivPass","",RWRWR_,"root",SDAQ_ID,1,"tp","str");
	    }
	}
        return;
    }

    //> Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/cntr/cfg/SecLev")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(secLev());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	setSecLev(opt->text());
    }
    else if(a_path == "/cntr/cfg/AuthProto")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(secAuthProto());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	setSecAuthProto(opt->text());
    }
    else if(a_path == "/cntr/cfg/AuthPass")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(string(secAuthPass().size(),'*'));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	setSecAuthPass(opt->text());
    }
    else if(a_path == "/cntr/cfg/PrivProto")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(secPrivProto());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	setSecPrivProto(opt->text());
    }
    else if(a_path == "/cntr/cfg/PrivPass")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(string(secPrivPass().size(),'*'));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	setSecPrivPass(opt->text());
    }
    else TController::cntrCmdProc(opt);
}


//*************************************************
//* TMdPrm                                        *
//*************************************************
TMdPrm::TMdPrm(string name, TTipParam *tp_prm) :
    TParamContr(name,tp_prm), m_oid(cfg("OID_LS").getSd()), p_el("w_attr")
{

}

TMdPrm::~TMdPrm( )
{
    nodeDelAll();
}

void TMdPrm::postEnable(int flag)
{
    TParamContr::postEnable(flag);
    if(!vlElemPresent(&p_el)) vlElemAtt(&p_el);
}

TMdContr &TMdPrm::owner( )	{ return (TMdContr&)TParamContr::owner(); }

void TMdPrm::enable( )
{
    if(enableStat()) return;

    TParamContr::enable();

    owner().prmEn(id(), true);

    //> Remove all OID's attributes
    for(unsigned i_p = 0; i_p < p_el.fldSize(); )
	try{ p_el.fldDel(i_p); }
        catch(TError err) { i_p++; }

    parseOIDList(m_oid);
}

void TMdPrm::disable( )
{
    if(!enableStat())  return;

    owner().prmEn(id(), false);

    TParamContr::disable();

    //> Set EVAL to parameter attributes
    vector<string> ls;
    elem().fldList(ls);
    for(unsigned i_el = 0; i_el < ls.size(); i_el++)
	vlAt(ls[i_el]).at().setS(EVAL_STR,0,true);
}

void TMdPrm::load_( )
{
    TParamContr::load_();
}

void TMdPrm::parseOIDList(const string &ioid)
{
    m_oid = ioid;

    oid tmpoid[MAX_OID_LEN];
    size_t tmpoid_len = MAX_OID_LEN;

    ls_oid.clear();

    string sel;
    for(int ioff = 0; (sel=TSYS::strSepParse(m_oid,0,'\n',&ioff)).size(); )
    {
	tmpoid_len = MAX_OID_LEN;
	if(snmp_parse_oid(sel.c_str(),tmpoid,&tmpoid_len))
	    ls_oid.push_back(string((char*)tmpoid,tmpoid_len*sizeof(oid)));
    }
}

void TMdPrm::cntrCmdProc( XMLNode *opt )
{
    //> Service commands process
    string a_path = opt->attr("path");
    if(a_path.substr(0,6) == "/serv/")	{ TParamContr::cntrCmdProc(opt); return; }

    //> Get page info
    if(opt->name() == "info")
    {
	TParamContr::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/cfg/OID_LS",cfg("OID_LS").fld().descr(),RWRWR_,"root",SDAQ_ID,2,"rows","8",
            "help",_("SNMP OID list, include directories for get all subitems. OID can write in the methods:\n"
		"  \".1.3.6.1.2.1.1\" - direct code addressing for object \"System\";\n"
		"  \"1.3.0\" - short code addressing, from root alias (object \"System.sysDescr\");\n"
		"  \".iso.org.dod.internet.mgmt.mib-2.system\" - full symbol to direct code addressing for object \"System\";\n"
		"  \"system.sysDescr\" - simple, not full path, addressing from root alias (object \"System\");\n"
		"  \"SNMPv2-MIB::sysDescr\" - addressing from MIB base by module name for \"system.sysDescr\"."));
	return;
    }

    //> Process command to page
    TParamContr::cntrCmdProc(opt);
}

void TMdPrm::vlSet( TVal &valo, const TVariant &pvl )
{
    if(!enableStat() || !owner().startStat()) valo.setS(EVAL_STR, 0, true);

    //> Send to active reserve station
    if(owner().redntUse())
    {
        if(valo.getS(NULL,true) == pvl.getS()) return;
        XMLNode req("set");
        req.setAttr("path",nodePath(0,true)+"/%2fserv%2fattr")->childAdd("el")->setAttr("id",valo.name())->setText(valo.getS(NULL,true));
        SYS->daq().at().rdStRequest(owner().workId(),req);
        return;
    }

    //> Direct write
    void *ss;
    char vtp = 0;
    oid oidn[MAX_OID_LEN];
    size_t oidn_len = MAX_OID_LEN;
    struct snmp_pdu *response = NULL;

    struct snmp_pdu *pdu = snmp_pdu_create(SNMP_MSG_SET);
    owner().str2oid(valo.name(), oidn, oidn_len);

    switch(atoi(valo.fld().reserve().c_str()))
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
    if(vtp)
    {
	if(!(ss=snmp_sess_open(&owner().session))) return;

	snmp_add_var(pdu, oidn, oidn_len, vtp, valo.getS().c_str());
	int status = snmp_sess_synch_response(ss, pdu, &response);
	if(status == STAT_TIMEOUT)
	    owner().acq_err.setVal(TSYS::strMess(_("10:Timeout: No Response from %s."),owner().session.peername).c_str());
	else if(response && response->errstat == SNMP_ERR_NOSUCHNAME)
	    owner().acq_err.setVal(TSYS::strMess(_("11:No authorized name.")));
	if(response) snmp_free_pdu(response);
	snmp_sess_close(ss);
    }
}

void TMdPrm::vlArchMake(TVal &val)
{
    if(val.arch().freeStat()) return;
    val.arch().at().setSrcMode(TVArchive::PassiveAttr,val.arch().at().srcData());
    val.arch().at().setPeriod(owner().period() ? owner().period()/1000 : 1000000);
    val.arch().at().setHardGrid(true);
    val.arch().at().setHighResTm(true);
}
