
//OpenSCADA system module DAQ.SNMP file: snmp.cpp
/***************************************************************************
 *   Copyright (C) 2006-2010 by Roman Savochenko                           *
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
#define MOD_VER		"0.4.1"
#define AUTORS		_("Roman Savochenko")
#define DESCRIPTION	_("Allow realising of SNMP client service.")
#define LICENSE		"GPL2"
//*************************************************

SNMP_DAQ::TTpContr *SNMP_DAQ::mod;  //Pointer for direct access to module

extern "C"
{
    TModule::SAt module( int n_mod )
    {
	if( n_mod==0 )	return TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE);
	return TModule::SAt("");
    }

    TModule *attach( const TModule::SAt &AtMod, const string &source )
    {
	if( AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE) )
	    return new SNMP_DAQ::TTpContr( source );
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

TTpContr::~TTpContr()
{

}

void TTpContr::load_( )
{
    //> Load parameters from command line

}

void TTpContr::postEnable( int flag )
{
    TTipDAQ::postEnable(flag);

    //> Controler's bd structure
    fldAdd( new TFld("PRM_BD",_("Parameteres table"),TFld::String,TFld::NoFlag,"30","") );
    fldAdd( new TFld("PERIOD",_("Gather data period (s)"),TFld::Integer,TFld::NoFlag,"3","1","1;100") );
    fldAdd( new TFld("PRIOR",_("Gather task priority"),TFld::Integer,TFld::NoFlag,"2","0","-1;99") );
    fldAdd( new TFld("ADDR",_("Remote host address"),TFld::String,TFld::NoFlag,"30","localhost") );
    fldAdd( new TFld("COMM",_("Server community"),TFld::String,TFld::NoFlag,"20","public") );
    fldAdd( new TFld("PATTR_LIM",_("Param's attributes limit"),TFld::Integer,TFld::NoFlag,"3","100","10;10000") );

    //> Parameter type bd structure
    int t_prm = tpParmAdd("std","PRM_BD",_("Standard"));
    tpPrmAt(t_prm).fldAdd( new TFld("OID_LS",_("OID list (next line separated)"),TFld::String,TFld::FullText|TCfg::NoVal,"100","") );
}

TController *TTpContr::ContrAttach( const string &name, const string &daq_db )
{
    return new TMdContr(name,daq_db,this);
}

//*************************************************
//* TMdContr                                      *
//*************************************************
TMdContr::TMdContr( string name_c, const string &daq_db, ::TElem *cfgelem) :
	::TController(name_c,daq_db,cfgelem), prc_st(false), endrun_req(false), tm_gath(0),
	m_per(cfg("PERIOD").getId()), m_prior(cfg("PRIOR").getId()), m_addr(cfg("ADDR").getSd()), m_comm(cfg("COMM").getSd()),
	m_pattr_lim(cfg("PATTR_LIM").getId())
{
    cfg("PRM_BD").setS("SNMPPrm_"+name_c);
}

TMdContr::~TMdContr( )
{
    if( run_st ) stop();
}

string TMdContr::getStatus( )
{
    string rez = TController::getStatus( );
    if( startStat() && !redntUse( ) ) rez += TSYS::strMess(_("Spent time: %s. "),TSYS::time2str(tm_gath).c_str());
    return rez;
}

TParamContr *TMdContr::ParamAttach( const string &name, int type )
{
    return new TMdPrm(name,&owner().tpPrmAt(type));
}

void TMdContr::start_( )
{
    //> Start the gathering data task
    if( !prc_st ) SYS->taskCreate( nodePath('.',true), m_prior, TMdContr::Task, this, &prc_st );
}

void TMdContr::stop_( )
{
    //> Stop the request and calc data task
    if( prc_st ) SYS->taskDestroy( nodePath('.',true), &prc_st, &endrun_req );
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

    int		el_cnt;
    string	soid;

    //> Start SNMP-net session
    struct snmp_session session;
    struct snmp_pdu *response;
    struct variable_list *vars;
    snmp_sess_init( &session );
    session.version = SNMP_VERSION_1;
    session.community = (u_char*)cntr.m_comm.c_str();
    session.community_len = strlen((char *)session.community);
    session.peername = (char *)cntr.m_addr.c_str();
    void *ss =  snmp_sess_open(&session);
    if( !ss )
    {
	mess_err(mod->nodePath().c_str(),"%s",_("Error SNMP session open."));
	return NULL;
    }

    cntr.endrun_req = false;
    cntr.prc_st = true;

    while(!cntr.endrun_req)
    {
	long long t_cnt = TSYS::curTime();

	//>> Update controller's data
	el_cnt = 0;
	cntr.en_res.resRequestR( );
	for( unsigned i_p = 0; i_p < cntr.p_hd.size() && !cntr.redntUse(); i_p++ )
	    try
	    {
		oid oid_root[MAX_OID_LEN], oid_next[MAX_OID_LEN];
		size_t oid_root_len = MAX_OID_LEN, oid_next_len = MAX_OID_LEN;

		TMdPrm &cprm = cntr.p_hd[i_p].at();

		for( int ioid = 0; ioid < cprm.lsOID().size(); ioid++ )
		{
		    oid_root_len = oid_next_len=cprm.lsOID()[ioid].size()/sizeof(oid);
		    memmove(oid_root,cprm.lsOID()[ioid].c_str(),oid_root_len*sizeof(oid));
		    memmove(oid_next,oid_root,oid_root_len*sizeof(oid));

		    bool running = true;
		    while( running && (el_cnt++) < cntr.pAttrLimit() )
		    {
			struct snmp_pdu *pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);
			snmp_add_null_var(pdu, oid_next, oid_next_len);
			int status = snmp_sess_synch_response(ss,pdu,&response);
			if( status == STAT_SUCCESS )
			{
			    if( response->errstat == SNMP_ERR_NOERROR )
				for( vars = response->variables; vars; vars = vars->next_variable )
				{
				    if( (vars->name_length < oid_root_len) || (memcmp(oid_root,vars->name,oid_root_len*sizeof(oid)) != 0) )
				    {
					running = 0;
					continue;
				    }
				    //>> Get or create element
				    soid = cntr.oid2str(vars->name,vars->name_length);
				    if( !cprm.elem().fldPresent(soid) )
				    {
					char tbuf[100];
					snprint_objid(tbuf,sizeof(tbuf),vars->name,vars->name_length);
					switch( vars->type )
					{
					    case ASN_OCTET_STR:
						cprm.elem().fldAdd( new TFld(soid.c_str(),tbuf,TFld::String,TFld::NoWrite) );
						break;
					    case ASN_INTEGER:
						cprm.elem().fldAdd( new TFld(soid.c_str(),tbuf,TFld::Integer,TFld::NoWrite) );
						break;
					    case ASN_COUNTER:
						cprm.elem().fldAdd( new TFld(soid.c_str(),tbuf,TFld::Real,TFld::NoWrite) );
						break;
					}
				    }
				    //>> Set value
				    switch( vars->type )
				    {
					case ASN_OCTET_STR:
					    cprm.vlAt(soid).at().setS(string((char*)vars->val.string,vars->val_len),0,true);
					    break;
					case ASN_INTEGER:
					    cprm.vlAt(soid).at().setI(*vars->val.integer,0,true);
					    break;
					case ASN_COUNTER:
					    cprm.vlAt(soid).at().setR(*(unsigned long*)vars->val.integer,0,true);
					    break;
					default:
					    //print_objid(vars->name,vars->name_length);
					    //print_value(vars->name,vars->name_length,vars);
					    break;
				    }
				    //print_variable(vars->name, vars->name_length, vars);
				    //--------------------
				    if( vars->type!=SNMP_ENDOFMIBVIEW && vars->type!=SNMP_NOSUCHOBJECT && vars->type!=SNMP_NOSUCHINSTANCE )
				    {
					memmove((char*)oid_next,(char*)vars->name,vars->name_length*sizeof(oid));
					oid_next_len = vars->name_length;
				    }
				    else running = 0;
				}
			    else running = 0;
			}
			else if( status == STAT_TIMEOUT )
			{
			    mess_err(mod->nodePath().c_str(),_("Timeout: No Response from %s."),session.peername);
			    running = 0;
			}
			else running = 0;
			if( response ) snmp_free_pdu(response);
		    }
		}
	    }
	    catch(TError err)
	    { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
	cntr.en_res.resRelease( );
	cntr.tm_gath = TSYS::curTime()-t_cnt;

	TSYS::taskSleep((long long)(1e9*cntr.period()));
    }

    snmp_sess_close(ss);

    cntr.prc_st = false;

    return NULL;
}

string TMdContr::oid2str( oid *ioid, size_t isz )
{
    string rez;
    for(int i_el = 0; i_el < isz; i_el++)
	rez=rez+"_"+TSYS::int2str(ioid[i_el]);
    return rez;
}

//*************************************************
//* TMdPrm                                        *
//*************************************************
TMdPrm::TMdPrm( string name, TTipParam *tp_prm ) :
    TParamContr(name,tp_prm), p_el("w_attr"), m_oid(cfg("OID_LS").getSd())
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

void TMdPrm::enable( )
{
    if( enableStat() )	return;

    TParamContr::enable();

    owner().prmEn( id(), true );
}

void TMdPrm::disable( )
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

    parseOIDList(m_oid);
}

void TMdPrm::parseOIDList( const string &ioid )
{
    m_oid = ioid;

    oid tmpoid[MAX_OID_LEN];
    size_t tmpoid_len = MAX_OID_LEN;

    ls_oid.clear();

    string sel;
    for( int ioff = 0; (sel=TSYS::strSepParse(m_oid,0,'\n',&ioff)).size(); )
    {
	tmpoid_len = MAX_OID_LEN;
	if( snmp_parse_oid(sel.c_str(),tmpoid,&tmpoid_len) )
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
	ctrMkNode("fld",opt,-1,"/prm/cfg/OID_LS",cfg("OID_LS").fld().descr(),enableStat()?R_R_R_:RWRWR_,"root",SDAQ_ID);
	return;
    }

    //> Process command to page
    if(a_path == "/prm/cfg/OID_LS" && ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))
    {
	if(enableStat())	throw TError(nodePath().c_str(),_("Parameter is enabled."));
	parseOIDList(opt->text());
    }
    else TParamContr::cntrCmdProc(opt);
}

void TMdPrm::vlArchMake( TVal &val )
{
    if( val.arch().freeStat() ) return;
    val.arch().at().setSrcMode(TVArchive::PassiveAttr,val.arch().at().srcData());
    val.arch().at().setPeriod((long long)(owner().period()*1000000));
    val.arch().at().setHardGrid( true );
    val.arch().at().setHighResTm( true );
}
