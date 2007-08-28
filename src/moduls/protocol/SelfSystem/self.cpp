
//OpenSCADA system module Protocol.SelfSystem file: self.cpp
/***************************************************************************
 *   Copyright (C) 2003-2006 by Roman Savochenko                           *
 *   rom_as@fromru.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
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
#include <string>

#include <tsys.h>
#include <tmess.h>
#include <tmodule.h>
#include "self.h"

//============ Modul info! =====================================================
#define MOD_ID      "SelfSystem"
#define MOD_NAME    "Self system OpenSCADA protocol"
#define MOD_TYPE    "Protocol"
#define VER_TYPE    VER_PROT
#define VERSION     "0.5.0"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "Self OpenSCADA protocol, support generic functions."
#define LICENSE     "GPL"
//==============================================================================

SelfPr::TProt *SelfPr::mod;

extern "C"
{
    TModule::SAt module( int n_mod )
    {
	TModule::SAt AtMod;

	if(n_mod==0)
    	{
	    AtMod.id	= MOD_ID;
	    AtMod.type  = MOD_TYPE;
	    AtMod.t_ver = VER_TYPE;
	}
    	else
    	    AtMod.id	= "";

	return( AtMod );
    }

    TModule *attach( const TModule::SAt &AtMod, const string &source )
    {
	SelfPr::TProt *self_addr = NULL;

    	if( AtMod.id == MOD_ID && AtMod.type == MOD_TYPE && AtMod.t_ver == VER_TYPE )
	    self_addr = SelfPr::mod = new SelfPr::TProt( source );

	return self_addr;
    }
}

using namespace SelfPr;

TProt::TProt( string name ) : m_t_auth(60)
{
    mId 	= MOD_ID;
    mName       = MOD_NAME;
    mType  	= MOD_TYPE;
    mVers      	= VERSION;
    mAutor    	= AUTORS;
    mDescr  	= DESCRIPTION;
    mLicense   	= LICENSE;
    mSource    	= name;
}

TProt::~TProt()
{
    ResAlloc res(ses_res,true);
    while( auth_lst.size() )	auth_lst.erase(auth_lst.begin());
    res.release();
}

int TProt::ses_open(const char *user,const char *pass)
{
    if( !SYS->security().at().usrPresent(user) || !SYS->security().at().usrAt(user).at().auth(pass) )
	return -1;
    
    //Check sesion and close old sesion
    ResAlloc res(ses_res,true);
    int i_s = 0;
    while( i_s < auth_lst.size() )    
	if( time(NULL) > (auth_lst[i_s].t_auth+10*authTime()) )
    	    auth_lst.erase(auth_lst.begin() + i_s);
	else i_s++;

    //Make new sesion
    int id_ses = rand();
    auth_lst.push_back( TProt::SAuth(time(NULL), user, id_ses) );
    
    return id_ses;
}

void TProt::ses_close(int id_ses)
{
    ResAlloc res(ses_res,true);
    int i_s = 0;
    while( i_s < auth_lst.size() )
	if( time(NULL) > (auth_lst[i_s].t_auth+10*authTime()) || auth_lst[i_s].id_ses == id_ses )
    	    auth_lst.erase(auth_lst.begin() + i_s);
    	else i_s++;
}

TProt::SAuth TProt::ses_get(int id_ses)
{
    ResAlloc res(ses_res,true);
    time_t cur_tm = time(NULL);
    int i_s = 0;
    while( i_s < auth_lst.size() )
	if( cur_tm > (auth_lst[i_s].t_auth+10*authTime()) )
    	    auth_lst.erase(auth_lst.begin() + i_s);
	else if( auth_lst[i_s].id_ses == id_ses )
	{
	    auth_lst[i_s].t_auth = cur_tm;
	    return auth_lst[i_s];
	}
	else i_s++;

    return TProt::SAuth(0,"",0);
}

string TProt::optDescr( )
{
    char buf[STR_BUF_LEN];

    snprintf(buf,sizeof(buf),_(
	"======================= The module <%s:%s> options =======================\n"
	"---------- Parameters of the module section <%s> in config file ----------\n\n"),
	MOD_TYPE,MOD_ID,nodePath().c_str());

    return(buf);
}			

void TProt::modLoad( )
{
    //========== Load parameters from command line ============
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{"help"    ,0,NULL,'h'},
	{NULL      ,0,NULL,0  }
    };

    optind=opterr=0;
    do
    {
	next_opt=getopt_long(SYS->argc,(char * const *)SYS->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': fprintf(stdout,optDescr().c_str()); break;
	    case -1 : break;
	}
    } while(next_opt != -1);    
    
    //========== Load parameters from config file =============
    m_t_auth = atoi( TBDS::genDBGet(nodePath()+"SessTimeLife",TSYS::int2str(m_t_auth)).c_str() );
}

void TProt::modSave( )
{
    TBDS::genDBSet(nodePath()+"SessTimeLife",TSYS::int2str(m_t_auth));
}

TProtocolIn *TProt::in_open( const string &name )
{
    return new TProtIn(name);
}

void TProt::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
        TProtocol::cntrCmdProc(opt);
	if(ctrMkNode("area",opt,1,"/prm",_("Parameters")))
    	    if(ctrMkNode("area",opt,1,"/prm/cfg",_("Module options")))
	    {
    		ctrMkNode("fld",opt,-1,"/prm/cfg/lf_tm",_("Life time of auth sesion(min)"),0660,"root","root",1,"tp","dec");
    		ctrMkNode("comm",opt,-1,"/prm/cfg/load",_("Load"),0660);
    		ctrMkNode("comm",opt,-1,"/prm/cfg/save",_("Save"),0660);
	    }
        ctrMkNode("fld",opt,-1,"/help/g_help",_("Options help"),0440,"root","root",3,"tp","str","cols","90","rows","5");
        return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/prm/cfg/lf_tm" )
    {
        if( ctrChkNode(opt,"get",0660,"root","root",SEQ_RD) )   opt->setText(TSYS::int2str(m_t_auth));
        if( ctrChkNode(opt,"set",0660,"root","root",SEQ_WR) )   m_t_auth = atoi(opt->text().c_str());
    }
    else if( a_path == "/help/g_help" && ctrChkNode(opt,"get",0440) )   opt->setText(optDescr());
    else if( a_path == "/prm/cfg/load" && ctrChkNode(opt,"set",0660,"root","root",SEQ_WR) )  modLoad();
    else if( a_path == "/prm/cfg/save" && ctrChkNode(opt,"set",0660,"root","root",SEQ_WR) )  modSave();
    else TProtocol::cntrCmdProc(opt);
}

//================================================================
//=========== TProtIn ============================================
//================================================================
TProtIn::TProtIn( string name ) : TProtocolIn( name ), m_nofull(false)
{

}

TProtIn::~TProtIn()
{

}

bool TProtIn::mess( const string &request, string &answer, const string &sender )
{
    int ses_id = -1;
    int req_sz = -1;
    
    //Continue for full request
    if( m_nofull )
    {
        req_buf = req_buf+request;
        m_nofull = false;
    }
    else req_buf=request;  //Save request to bufer
    
    string req = req_buf.substr(0,req_buf.find("\n"));
    
    if( req.substr(0,8) == "SES_OPEN" )
    {
	char user[256] = "", 
	     pass[256] = "";
	sscanf(req.c_str(),"SES_OPEN %255s %255s",user,pass);
	ses_id = mod->ses_open(user,pass);
	if(ses_id < 0)	answer = "REZ 1 Auth error. User or password error.\n";
	else answer = "REZ 0 "+TSYS::int2str(ses_id)+"\n";
    }
    else if( req.substr(0,9) == "SES_CLOSE" )
    {
	sscanf(req.c_str(),"SES_CLOSE %d",&ses_id);
	mod->ses_close(ses_id);
	answer = "REZ 0\n";
    }
    else if( req.substr(0,3) == "REQ" )
    {
	sscanf(req.c_str(),"REQ %d %d",&ses_id,&req_sz);
	TProt::SAuth auth = mod->ses_get(ses_id);
	if( !auth.t_auth ) answer = "REZ 1 Auth error. Session no valid.\n";
	else
	{
	    try
	    {
		XMLNode req_node;
		if(req_buf.size() < req.size()+strlen("\n")+((req_sz>=0)?req_sz:0))
		{ m_nofull = true; return true; }
		req_node.load(req_buf.substr(req.size()));
		req_node.setAttr("user",auth.name);
		SYS->cntrCmd(&req_node);
		string resp = req_node.save()+"\n";
	        answer="REZ 0 "+TSYS::int2str(resp.size())+"\n"+resp;
	    }
	    catch(TError err)
	    {
		answer="REZ 2 "+err.cat+":"+err.mess+"\n";
	    }
	}
    }
    else answer = "REZ 3 Command format error.\n";
	
    return false;
}


