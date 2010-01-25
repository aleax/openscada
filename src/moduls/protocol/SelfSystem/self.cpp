
//OpenSCADA system module Protocol.SelfSystem file: self.cpp
/***************************************************************************
 *   Copyright (C) 2007-2009 by Roman Savochenko                           *
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
#include <string.h>
#include <string>

#include <tsys.h>
#include <tmess.h>
#include <tmodule.h>
#include "self.h"

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"SelfSystem"
#define MOD_NAME	"Self system OpenSCADA protocol"
#define MOD_TYPE	"Protocol"
#define VER_TYPE	VER_PROT
#define VERSION		"0.9.2"
#define AUTORS		"Roman Savochenko"
#define DESCRIPTION	"Self OpenSCADA protocol, support generic functions."
#define LICENSE		"GPL"
//*************************************************

SelfPr::TProt *SelfPr::mod;

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
	    return new SelfPr::TProt( source );
	return NULL;
    }
}

using namespace SelfPr;

//*************************************************
//* TProt                                         *
//*************************************************
TProt::TProt( string name ) : m_t_auth(60), mComprLev(0), mComprBrd(80)
{
    mId		= MOD_ID;
    mName	= MOD_NAME;
    mType	= MOD_TYPE;
    mVers	= VERSION;
    mAutor	= AUTORS;
    mDescr	= DESCRIPTION;
    mLicense	= LICENSE;
    mSource	= name;

    mod		= this;
}

TProt::~TProt( )
{
    ResAlloc res(ses_res,true);
    while( auth_lst.size() )	auth_lst.erase(auth_lst.begin());
    res.release();
}

int TProt::sesOpen( const char *user,const char *pass )
{
    if( !SYS->security().at().usrPresent(user) || !SYS->security().at().usrAt(user).at().auth(pass) )
	return -1;

    //> Check sesion and close old sesion
    ResAlloc res(ses_res,true);
    int i_s = 0;
    while( i_s < auth_lst.size() )
	if( time(NULL) > (auth_lst[i_s].t_auth+10*authTime()) )
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
    int i_s = 0;
    while( i_s < auth_lst.size() )
	if( time(NULL) > (auth_lst[i_s].t_auth+10*authTime()) || auth_lst[i_s].id_ses == id_ses )
	    auth_lst.erase(auth_lst.begin() + i_s);
	else i_s++;
}

TProt::SAuth TProt::sesGet(int id_ses)
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

void TProt::load_( )
{
    //> Load parameters from command line

    //> Load parameters from config file
    setAuthTime( atoi(TBDS::genDBGet(nodePath()+"SessTimeLife",TSYS::int2str(authTime())).c_str()) );
    setComprLev( atoi(TBDS::genDBGet(nodePath()+"ComprLev",TSYS::int2str(comprLev())).c_str()) );
    setComprBrd( atoi(TBDS::genDBGet(nodePath()+"ComprBrd",TSYS::int2str(comprBrd())).c_str()) );
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
    char buf[1000], buf1[256];
    string req, resp;
    int rez, resp_len;

    ResAlloc res( tro.nodeRes(), true );

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
	    if( !isDir && tro.prm1() < 0 )
	    {
		req = "SES_OPEN "+user+" "+pass+"\n";
		resp_len = tro.messIO(req.c_str(),req.size(),buf,sizeof(buf)-1,0,true);
		buf[resp_len] = 0;
		buf1[0] = 0;
		sscanf(buf,"REZ %d %255s\n",&rez,buf1);
		if(rez == 1)	throw TError(nodePath().c_str(),_("Station <%s> auth error: %s!"),tro.id().c_str(),buf1);
		else if(rez > 0)throw TError(nodePath().c_str(),_("Station <%s> error: %s!"),tro.id().c_str(),buf1);
		tro.setPrm1(atoi(buf1));
	    }
	    //> Request
	    //>> Compress data
	    bool reqCompr = comprLev() && data.size() > comprBrd();
	    if( reqCompr )	data = TSYS::strCompr(data,comprLev());

	    if(isDir)	req = "REQDIR "+user+" "+pass+" "+TSYS::int2str(data.size()*(reqCompr?-1:1))+"\n"+data;
	    else	req = "REQ "+TSYS::int2str(tro.prm1())+" "+TSYS::int2str(data.size()*(reqCompr?-1:1))+"\n"+data;
	    buf[0] = 0;
	    resp_len = tro.messIO(req.c_str(),req.size(),buf,sizeof(buf),0,true);
	    resp.assign(buf,resp_len);

	    //>> Get head
	    buf1[0] = 0;
	    if( sscanf(resp.c_str(),"REZ %d %255s\n",&rez,buf1) <= 0 )
		throw TError(nodePath().c_str(),_("Station respond <%s> error!"),tro.id().c_str());
	    if( rez == 1 )	{ tro.setPrm1(-1); if( isDir ) break; else continue; }
	    if( rez > 0 )	throw TError(nodePath().c_str(),_("Station <%s> error: %d:%s!"),tro.id().c_str(),rez,buf1);
	    int head_end = resp.find("\n",0);
	    if( head_end == string::npos )
		throw TError(nodePath().c_str(),_("Station <%s> error: Respond is broken!"),tro.id().c_str());
	    int resp_size = atoi(buf1);

	    //>> Wait tail
	    while( resp.size() < abs(resp_size)+head_end+sizeof('\n') )
	    {
		resp_len = tro.messIO(NULL,0,buf,sizeof(buf),0,true);
		resp.append(buf,resp_len);
	    }

	    if( resp_size < 0 )	io.load(TSYS::strUncompr(resp.substr(head_end+sizeof('\n'))));
	    else io.load(resp.substr(head_end+sizeof('\n')));

	    return;
	}
    }
    catch( TError err )	{ tro.stop(); throw; }
}

void TProt::cntrCmdProc( XMLNode *opt )
{
    //- Get page info -
    if( opt->name() == "info" )
    {
	TProtocol::cntrCmdProc(opt);
	if(ctrMkNode("area",opt,1,"/prm",_("Parameters")))
	    if(ctrMkNode("area",opt,1,"/prm/cfg",_("Module options")))
	    {
		ctrMkNode("fld",opt,-1,"/prm/cfg/lf_tm",_("Life time of auth sesion(min)"),0664,"root","root",1,"tp","dec");
		ctrMkNode("fld",opt,-1,"/prm/cfg/compr",_("Compression level"),0664,"root","root",4,"tp","dec","min","-1","max","9",
		    "help",_("ZLib compression level:\n"
			     "  -1  - optimal speed-size;\n"
			     "  0   - disable;\n"
			     "  1-9 - direct level."));
		ctrMkNode("fld",opt,-1,"/prm/cfg/comprBrd",_("Lower compression border"),0664,"root","root",4,"tp","dec","min","10","max","1000",
		    "help",_("Value in bytes."));
	    }
	return;
    }

    //- Process command to page -
    string a_path = opt->attr("path");
    if( a_path == "/prm/cfg/lf_tm" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText( TSYS::int2str(authTime()) );
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	setAuthTime( atoi(opt->text().c_str()) );
    }
    else if( a_path == "/prm/cfg/compr" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText( TSYS::int2str(comprLev()) );
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	setComprLev( atoi(opt->text().c_str()) );
    }
    else if( a_path == "/prm/cfg/comprBrd" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText( TSYS::int2str(comprBrd()) );
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	setComprBrd( atoi(opt->text().c_str()) );
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

bool TProtIn::mess( const string &request, string &answer, const string &sender )
{
    int ses_id = -1;
    int req_sz = 0;
    char user[256] = "", pass[256] = "";

    //> Continue for full request
    if( m_nofull )
    {
	req_buf = req_buf+request;
	m_nofull = false;
    }
    else req_buf=request;  //Save request to bufer

    string req = req_buf.substr(0,req_buf.find("\n"));

    if( req.substr(0,8) == "SES_OPEN" )
    {
	sscanf(req.c_str(),"SES_OPEN %255s %255s",user,pass);
	ses_id = mod->sesOpen( user, pass );
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
#if OSC_DEBUG >= 3
	long long w_tm = TSYS::curTime();
	mess_debug(nodePath().c_str(),_("Get request: '%s': %d"),req.c_str(),req_buf.size());
#endif

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

#if OSC_DEBUG >= 3
	    mess_debug(nodePath().c_str(),_("Unpack and load request: '%s': %d, time: %f ms."),req.c_str(),req_buf.size(),1e-3*(TSYS::curTime()-w_tm));
	    w_tm = TSYS::curTime();
#endif

	    SYS->cntrCmd(&req_node);

#if OSC_DEBUG >= 3
	    mess_debug(nodePath().c_str(),_("Process request: '%s', time: %f ms."),req.c_str(),1e-3*(TSYS::curTime()-w_tm));
	    w_tm = TSYS::curTime();
#endif

	    string resp = req_node.save(XMLNode::MissTagEnc|XMLNode::MissAttrEnc)+"\n";

	    //> Compress respond
	    bool respCompr = ((TProt&)owner()).comprLev() && resp.size() > ((TProt&)owner()).comprBrd();
	    if( respCompr ) resp = TSYS::strCompr(resp,((TProt&)owner()).comprLev());

#if OSC_DEBUG >= 3
	    mess_debug(nodePath().c_str(),_("Save respond to stream and pack: '%s': %d, time: %f ms."),req.c_str(),resp.size(),1e-3*(TSYS::curTime()-w_tm));
#endif

	    answer="REZ 0 "+TSYS::int2str(resp.size()*(respCompr?-1:1))+"\n"+resp;
	}
	catch(TError err)
	{
	    answer="REZ 2 "+err.cat+":"+err.mess+"\n";
	}
    }
    else answer = "REZ 3 Command format error.\n";

    return false;
}
