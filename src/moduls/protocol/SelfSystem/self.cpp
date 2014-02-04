
//OpenSCADA system module Protocol.SelfSystem file: self.cpp
/***************************************************************************
 *   Copyright (C) 2007-2010 by Roman Savochenko                           *
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

#include <string.h>
#include <string>

#include <tsys.h>
#include <tmess.h>
#include <tmodule.h>
#include "self.h"

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"SelfSystem"
#define MOD_NAME	_("Self system OpenSCADA protocol")
#define MOD_TYPE	SPRT_ID
#define VER_TYPE	SPRT_VER
#define MOD_VER		"0.9.5"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("Self OpenSCADA protocol, support generic functions.")
#define LICENSE		"GPL2"
//*************************************************

SelfPr::TProt *SelfPr::mod;

extern "C"
{
#ifdef MOD_INCL
    TModule::SAt prt_SelfSystem_module( int n_mod )
#else
    TModule::SAt module( int n_mod )
#endif
    {
	if( n_mod==0 )	return TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE);
	return TModule::SAt("");
    }

#ifdef MOD_INCL
    TModule *prt_SelfSystem_attach( const TModule::SAt &AtMod, const string &source )
#else
    TModule *attach( const TModule::SAt &AtMod, const string &source )
#endif
    {
	if( AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE) )
	    return new SelfPr::TProt( source );
	return NULL;
    }
}

using namespace SelfPr;

//*************************************************
//* TProt                                         *
//*************************************************
TProt::TProt( string name ) : TProtocol(MOD_ID), m_t_auth(60), mComprLev(0), mComprBrd(80)
{
    mod		= this;

    mName	= MOD_NAME;
    mType	= MOD_TYPE;
    mVers	= MOD_VER;
    mAuthor	= AUTHORS;
    mDescr	= DESCRIPTION;
    mLicense	= LICENSE;
    mSource	= name;
}

TProt::~TProt( )
{
    ResAlloc res(ses_res,true);
    while(auth_lst.size()) auth_lst.erase(auth_lst.begin());
    res.release();
}

int TProt::sesOpen( const char *user,const char *pass )
{
    if(!SYS->security().at().usrPresent(user) || !SYS->security().at().usrAt(user).at().auth(pass))
	return -1;

    //> Check sesion and close old sesion
    ResAlloc res(ses_res,true);
    for(unsigned i_s = 0; i_s < auth_lst.size(); )
	if(time(NULL) > (auth_lst[i_s].t_auth+10*authTime()))
	    auth_lst.erase(auth_lst.begin() + i_s);
	else i_s++;

    //> Make new sesion
    int id_ses = rand();
    auth_lst.push_back( TProt::SAuth(time(NULL), user, id_ses) );

    return id_ses;
}

void TProt::sesClose( int id_ses )
{
    ResAlloc res( ses_res, true );
    for(unsigned i_s = 0; i_s < auth_lst.size(); )
	if(time(NULL) > (auth_lst[i_s].t_auth+10*authTime()) || auth_lst[i_s].id_ses == id_ses)
	    auth_lst.erase(auth_lst.begin() + i_s);
	else i_s++;
}

TProt::SAuth TProt::sesGet(int id_ses)
{
    ResAlloc res(ses_res,true);
    time_t cur_tm = time(NULL);
    for(unsigned i_s = 0; i_s < auth_lst.size(); )
	if(cur_tm > (auth_lst[i_s].t_auth+10*authTime())) auth_lst.erase(auth_lst.begin() + i_s);
	else if(auth_lst[i_s].id_ses == id_ses)
	{
	    auth_lst[i_s].t_auth = cur_tm;
	    return auth_lst[i_s];
	}
	else i_s++;

    return TProt::SAuth(0,"",0);
}

void TProt::load_( )
{
    //> Load parameters from command line

    //> Load parameters from config-file
    setAuthTime(atoi(TBDS::genDBGet(nodePath()+"SessTimeLife",TSYS::int2str(authTime())).c_str()));
    setComprLev(atoi(TBDS::genDBGet(nodePath()+"ComprLev",TSYS::int2str(comprLev())).c_str()));
    setComprBrd(atoi(TBDS::genDBGet(nodePath()+"ComprBrd",TSYS::int2str(comprBrd())).c_str()));
}

void TProt::save_( )
{
    TBDS::genDBSet(nodePath()+"SessTimeLife",TSYS::int2str(authTime()));
    TBDS::genDBSet(nodePath()+"ComprLev",TSYS::int2str(comprLev()));
    TBDS::genDBSet(nodePath()+"ComprBrd",TSYS::int2str(comprBrd()));
}

TProtocolIn *TProt::in_open( const string &name )
{
    return new TProtIn(name);
}

void TProt::outMess( XMLNode &io, TTransportOut &tro )
{
    char buf[1000];
    string req, resp, header;
    int rez, resp_len, off, head_end;

    ResAlloc res(tro.nodeRes(), true);

    bool   isDir = atoi(io.attr("rqDir").c_str()); io.attrDel("rqDir");
    string user = io.attr("rqUser"); io.attrDel("rqUser");
    string pass = io.attr("rqPass"); io.attrDel("rqPass");
    string data = io.save();
    io.clear();

    try
    {
	while(true)
	{
	    //> Session open
            if(!isDir && tro.prm1() < 0)
            {
                req = "SES_OPEN "+user+" "+pass+"\n";
                resp_len = tro.messIO(req.c_str(),req.size(),buf,sizeof(buf)-1,0,true);
                buf[resp_len] = 0;
                head_end = off = 0;
                header = TSYS::strLine(buf,0,&head_end);
                if(header.size() >= 5 && TSYS::strParse(header,0," ",&off) == "REZ")
                {
                    rez = atoi(TSYS::strParse(header,0," ",&off).c_str());
                    if(rez > 0 || off >= (int)header.size())
                	throw TError(nodePath().c_str(),_("Station '%s' error: %s!"),tro.id().c_str(),header.substr(off).c_str());
                    tro.setPrm1(atoi(header.substr(off).c_str()));
                } else throw TError(nodePath().c_str(),_("Station '%s' error: Respond format error!"),tro.id().c_str());
            }

	    //> Request
	    //>> Compress data
	    bool reqCompr = (comprLev() && (int)data.size() > comprBrd());
	    if(reqCompr) data = TSYS::strCompr(data,comprLev());

	    if(isDir)	req = "REQDIR "+user+" "+pass+" "+TSYS::int2str(data.size()*(reqCompr?-1:1))+"\n"+data;
	    else	req = "REQ "+TSYS::int2str(tro.prm1())+" "+TSYS::int2str(data.size()*(reqCompr?-1:1))+"\n"+data;
	    buf[0] = 0;
	    resp_len = tro.messIO(req.c_str(),req.size(),buf,sizeof(buf),0,true);
	    resp.assign(buf,resp_len);

	    //>> Get head
            head_end = off = 0;
            header = TSYS::strLine(resp,0,&head_end);
            if(header.size() < 5 || TSYS::strParse(header,0," ",&off) != "REZ")
                throw TError(nodePath().c_str(),_("Station respond '%s' error!"),tro.id().c_str());
            rez = atoi(TSYS::strParse(header,0," ",&off).c_str());
            if(rez == 1) { tro.setPrm1(-1); if(isDir) break; else continue; }
            if(rez > 0 || off >= (int)header.size())
                throw TError(nodePath().c_str(),_("Station '%s' error: %s!"),tro.id().c_str(),buf+off);
            int resp_size = atoi(header.substr(off).c_str());

	    //>> Wait tail
	    while((int)resp.size() < abs(resp_size)+head_end)
	    {
		resp_len = tro.messIO(NULL,0,buf,sizeof(buf),0,true);
		if(!resp_len) throw TError(nodePath().c_str(),_("Not full respond."));
		resp.append(buf,resp_len);
	    }

	    if(resp_size < 0) io.load(TSYS::strUncompr(resp.substr(head_end)));
	    else io.load(resp.substr(head_end));

	    return;
	}
    }
    catch(TError err) { tro.stop(); throw; }
}

void TProt::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if(opt->name() == "info")
    {
	TProtocol::cntrCmdProc(opt);
	if(ctrMkNode("area",opt,1,"/prm",_("Parameters")))
	    if(ctrMkNode("area",opt,1,"/prm/cfg",_("Module options")))
	    {
		ctrMkNode("fld",opt,-1,"/prm/cfg/lf_tm",_("Life time of auth session(min)"),RWRWR_,"root",SPRT_ID,2,"tp","dec","min","1");
		ctrMkNode("fld",opt,-1,"/prm/cfg/compr",_("Compression level"),RWRWR_,"root",SPRT_ID,4,"tp","dec","min","-1","max","9",
		    "help",_("ZLib compression level:\n"
			     "  -1  - optimal speed-size;\n"
			     "  0   - disable;\n"
			     "  1-9 - direct level."));
		ctrMkNode("fld",opt,-1,"/prm/cfg/comprBrd",_("Lower compression border"),RWRWR_,"root",SPRT_ID,3,"tp","dec","min","10",
		    "help",_("Value in bytes."));
	    }
	return;
    }

    //> Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/cfg/lf_tm")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD))	opt->setText(TSYS::int2str(authTime()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SPRT_ID,SEC_WR))	setAuthTime(atoi(opt->text().c_str()));
    }
    else if(a_path == "/prm/cfg/compr")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD))	opt->setText(TSYS::int2str(comprLev()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SPRT_ID,SEC_WR))	setComprLev(atoi(opt->text().c_str()));
    }
    else if(a_path == "/prm/cfg/comprBrd")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD))	opt->setText(TSYS::int2str(comprBrd()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SPRT_ID,SEC_WR))	setComprBrd(atoi(opt->text().c_str()));
    }
    else TProtocol::cntrCmdProc(opt);
}

//*************************************************
//* TProtIn                                       *
//*************************************************
TProtIn::TProtIn( string name ) : TProtocolIn( name ), m_nofull(false)
{

}

TProtIn::~TProtIn()
{

}

bool TProtIn::mess( const string &request, string &answer )
{
    int64_t d_tm = 0;
    int ses_id = -1;
    int req_sz = 0;
    char user[256] = "", pass[256] = "";

    //> Continue for full request
    if( m_nofull )
    {
	req_buf = req_buf+request;
	m_nofull = false;
    }
    else req_buf=request;  //Save request to buffer

    string req = req_buf.substr(0,req_buf.find("\n"));

    if( req.substr(0,8) == "SES_OPEN" )
    {
	sscanf(req.c_str(),"SES_OPEN %255s %255s",user,pass);
	ses_id = mod->sesOpen(user, pass);
	if(ses_id < 0)	answer = "REZ 1 Auth error. User or password error.\n";
	else answer = "REZ 0 "+TSYS::int2str(ses_id)+"\n";
    }
    else if( req.substr(0,9) == "SES_CLOSE" )
    {
	sscanf(req.c_str(),"SES_CLOSE %d",&ses_id);
	mod->sesClose(ses_id);
	answer = "REZ 0\n";
    }
    else if( req.substr(0,3) == "REQ" )
    {
	if(mess_lev() == TMess::Debug)
	{
	    d_tm = TSYS::curTime();
	    mess_debug(nodePath().c_str(), _("Get request: '%s': %d"), req.c_str(), req_buf.size());
	}

	TProt::SAuth auth(0,"",0);
	if( sscanf(req.c_str(),"REQ %d %d",&ses_id,&req_sz) == 2 )	auth = mod->sesGet(ses_id);
	else if( sscanf(req.c_str(),"REQDIR %255s %255s %d",user,pass,&req_sz) == 3 )
	{
	    if( SYS->security().at().usrPresent(user) && SYS->security().at().usrAt(user).at().auth(pass) )
	    { auth.t_auth = 1; auth.name = user; }
	}
	else { answer = "REZ 3 Command format error.\n"; return false; }

	if( !auth.t_auth ) { answer = "REZ 1 Auth error. Session no valid.\n"; return false; }

	try
	{
	    if( req_buf.size() < req.size()+strlen("\n")+abs(req_sz) ) { m_nofull = true; return true; }

	    //> Decompress request
	    if( req_sz < 0 )
		req_buf.replace(req.size()+strlen("\n"),abs(req_sz),
		    TSYS::strUncompr(req_buf.substr(req.size()+strlen("\n"))));

	    //> Process request
	    XMLNode req_node;
	    req_node.load(req_buf.substr(req.size()+strlen("\n")));
	    req_node.setAttr("user",auth.name);

	    if(mess_lev() == TMess::Debug)
	    {
		mess_debug(nodePath().c_str(), _("Unpack and load request: '%s': %d, time: %f ms."), req.c_str(), req_buf.size(), 1e-3*(TSYS::curTime()-d_tm));
		d_tm = TSYS::curTime();
	    }

	    SYS->cntrCmd(&req_node);

	    if(mess_lev() == TMess::Debug)
	    {
		mess_debug(nodePath().c_str(), _("Process request: '%s', time: %f ms."), req.c_str(), 1e-3*(TSYS::curTime()-d_tm));
		d_tm = TSYS::curTime();
	    }

	    string resp = req_node.save(XMLNode::MissTagEnc|XMLNode::MissAttrEnc)+"\n";

	    //> Compress respond
	    bool respCompr = (((TProt&)owner()).comprLev() && (int)resp.size() > ((TProt&)owner()).comprBrd());
	    if( respCompr ) resp = TSYS::strCompr(resp,((TProt&)owner()).comprLev());

	    if(mess_lev() == TMess::Debug)
		mess_debug(nodePath().c_str(), _("Save respond to stream and pack: '%s': %d, time: %f ms."), req.c_str(), resp.size(), 1e-3*(TSYS::curTime()-d_tm));

	    answer = "REZ 0 "+TSYS::int2str(resp.size()*(respCompr?-1:1))+"\n"+resp;
	}
	catch(TError err)
	{
	    answer = "REZ 2 "+err.cat+":"+err.mess+"\n";
	}
    }
    else answer = "REZ 3 Command format error.\n";

    return false;
}
