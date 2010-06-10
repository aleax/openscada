
//OpenSCADA system module Protocol.HTTP file: http.cpp
/***************************************************************************
 *   Copyright (C) 2003-2009 by Roman Savochenko                           *
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

#include <config.h>
#include <tsys.h>
#include <tmess.h>
#include <tmodule.h>
#include <tuis.h>
#include "http.h"

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"HTTP"
#define MOD_NAME	"HTTP-realisation"
#define MOD_TYPE	"Protocol"
#define VER_TYPE	VER_PROT
#define M_VERSION	"1.4.1"
#define AUTORS		"Roman Savochenko"
#define DESCRIPTION	"Allow support HTTP for WWW based UIs."
#define LICENSE		"GPL"
//*************************************************

PrHTTP::TProt *PrHTTP::mod;

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
	    return new PrHTTP::TProt( source );
	return NULL;
    }
}

using namespace PrHTTP;

//*************************************************
//* TProt                                         *
//*************************************************
TProt::TProt( string name ) : mTAuth(10), lst_ses_chk(0)
{
    mId		= MOD_ID;
    mType	= MOD_TYPE;
    mName	= MOD_NAME;
    mVers	= M_VERSION;
    mAutor	= AUTORS;
    mDescr	= DESCRIPTION;
    mLicense	= LICENSE;
    mSource	= name;

    mod		= this;
}

TProt::~TProt()
{

}

string TProt::optDescr( )
{
    char buf[STR_BUF_LEN];
    snprintf(buf,sizeof(buf),_(
	"======================= The module <%s:%s> options =======================\n"
	"---------- Parameters of the module section <%s> in config file ----------\n"
	"AuthTime <min>      Life time of the authentication, minutes (default 10).\n\n"),
	MOD_TYPE,MOD_ID,nodePath().c_str());

    return buf;
}

void TProt::load_( )
{
    //> Load parameters from command line
    int next_opt;
    const char *short_opt="h";
    struct option long_opt[] =
    {
	{"help"    ,0,NULL,'h'},
	{NULL      ,0,NULL, 0 }
    };

    optind=opterr=0;
    do
    {
	next_opt=getopt_long(SYS->argc,(char * const *)SYS->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': fprintf(stdout,"%s",optDescr().c_str()); break;
	    case -1 : break;
	}
    } while(next_opt != -1);

    //> Load parameters from config file
    mTAuth = atoi( TBDS::genDBGet(nodePath()+"AuthTime",TSYS::int2str(mTAuth)).c_str() );
}

void TProt::save_( )
{
    TBDS::genDBSet(nodePath()+"AuthTime",TSYS::int2str(mTAuth));
}

TProtocolIn *TProt::in_open( const string &name )
{
    return new TProtIn(name);
}

int TProt::sesOpen( string name )
{
    int sess_id;
    ResAlloc res(nodeRes(),true);

    //> Get free identifier
    do{ sess_id = rand(); }
    while( sess_id == 0 || mAuth.find(sess_id) != mAuth.end() );

    //> Add new session authentification
    mAuth[sess_id] = SAuth(name,time(NULL));

    return sess_id;
}

string TProt::sesCheck( int sid )
{
    time_t cur_tm = time(NULL);
    map<int,SAuth>::iterator authEl;

    //> Check for close old sessions
    ResAlloc res(nodeRes(),true);
    if( cur_tm > lst_ses_chk+10 )
    {
	for( authEl = mAuth.begin(); authEl != mAuth.end(); )
	    if( cur_tm > authEl->second.tAuth+mTAuth*60 )
		mAuth.erase(authEl++);
	    else authEl++;
	lst_ses_chk = cur_tm;
    }

    //> Check for session
    res.request(false);
    authEl = mAuth.find(sid);
    if( authEl != mAuth.end() )
    {
	authEl->second.tAuth = cur_tm;
	return authEl->second.name;
    }
    return "";
}

void TProt::outMess( XMLNode &io, TTransportOut &tro )
{
    string req, resp, hostTr, tw, cnt;
    char buf[1000];
    string host = io.attr("Host");
    string uri = io.attr("URI");
    if( uri.empty() ) uri = "/";
    XMLNode *nd;

    ResAlloc resN( tro.nodeRes(), true );

    try
    {
	//> Get host address from transport
	if( tro.owner().modId() == "Sockets" ) hostTr = TSYS::strParse(tro.addr(),1,":")+":"+TSYS::strParse(tro.addr(),2,":");
	else hostTr = tro.addr();

	//> Set new address
	if( !host.empty() && host != hostTr )
	{
	    tro.stop();
	    if( tro.owner().modId() == "Sockets" ) tro.setAddr("TCP:"+host);
	    else tro.setAddr(host);
	}
	host = hostTr;

	//> Prepare request
	if( io.name() == "GET" ) ;
	else if( io.name() == "POST" )
	{
	    //> Content process
	    bool isCnt = false;
	    for( int cnt_c = 0; cnt_c < io.childSize(); cnt_c++ )
	    {
		if( io.childGet(cnt_c)->name() != "cnt" ) continue;
		cnt += "--"cntBnd"\r\n";
		cnt += "Content-Disposition: form-data; \""+io.childGet(cnt_c)->attr("name")+
		       "\"; filename=\""+io.childGet(cnt_c)->attr("filename")+"\"\r\n";
		//>> Place appended content properties
		for( int ch_c = 0; ch_c < io.childGet(cnt_c)->childSize(); ch_c++ )
		    if( io.childGet(cnt_c)->childGet(ch_c)->name() == "prm" )
			cnt += io.childGet(cnt_c)->childGet(ch_c)->attr("id")+": "+
			   io.childGet(cnt_c)->childGet(ch_c)->text()+"\r\n";
		cnt += "\r\n"+io.childGet(cnt_c)->text();
		isCnt = true;
	    }
	    if( isCnt ) cnt += "--"cntBnd"--\r\n";
	    io.childAdd("prm")->setAttr("id","Content-Length")->setText(TSYS::int2str(cnt.size()));
	    io.childAdd("prm")->setAttr("id","Content-Type")->setText("multipart/form-data; boundary="cntBnd);
	}
	else throw TError(nodePath().c_str(),TSYS::strMess(_("HTTP method '%s' error or don't support."),io.name().c_str()).c_str());

	//> Place HTTP head
	req = TSYS::strMess("%s %s HTTP/1.1\r\n",io.name().c_str(),uri.c_str());
	//>> Place main HTTP properties
	req += TSYS::strMess("Host: %s\r\n",host.c_str());
	req += "User-Agent: "PACKAGE_NAME" v"VERSION"\r\n";
	//>> Place appended HTTP-properties
	for( int ch_c = 0; ch_c < io.childSize(); ch_c++ )
	    if( io.childGet(ch_c)->name() == "prm" )
		req += io.childGet(ch_c)->attr("id")+": "+io.childGet(ch_c)->text()+"\r\n";
	req += "\r\n"+cnt;

	io.childClear();

	//> Start transport
	if( !tro.startStat() ) tro.start();

	//> Put request
	int resp_len = tro.messIO(req.c_str(),req.size(),buf,sizeof(buf),0,true);
	resp.assign(buf,resp_len);

	//> Parse first record
	int pos = 0;
	tw = TSYS::strParse(resp,0,"\r\n",&pos);
	string protocol	= TSYS::strParse(tw,0," ");
	string rcod	= TSYS::strParse(tw,1," ");
	string rstr	= TSYS::strParse(tw,2," ");
	if( (protocol != "HTTP/1.0" && protocol != "HTTP/1.1") || rcod.empty() || rstr.empty() )
	    throw TError(nodePath().c_str(),_("HTTP respond error"));
	io.setAttr("Protocol",protocol)->setAttr("RezCod",rcod)->setAttr("RezStr",rstr);

	//> Parse parameters
	int c_lng = -1;
	while( true )
	{
	    tw = TSYS::strParse(resp,0,"\r\n",&pos);
	    if( tw.empty() ) break;
	    int sepPos = tw.find(":",0);
	    if( sepPos == 0 || sepPos == string::npos ) continue;
	    nd = io.childAdd("prm")->setAttr("id",tw.substr(0,sepPos))->setText(tw.substr(sepPos+1));
	    if( strcasecmp(nd->attr("id").c_str(),"content-length") == 0 ) c_lng = atoi(nd->text().c_str());
	}

	//>> Wait tail
	while( c_lng >= 0 && c_lng > (resp.size()-pos) )
	{
	    resp_len = tro.messIO(NULL,0,buf,sizeof(buf),0,true);
	    resp.append(buf,resp_len);
	}

	io.setText(resp.substr(pos));
    }
    catch( TError err )	{ io.setAttr("err",err.mess); return; }

    io.setAttr("err","");
}

void TProt::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if( opt->name() == "info" )
    {
	TProtocol::cntrCmdProc(opt);
	if(ctrMkNode("area",opt,0,"/prm",_("Protocol")))
	    ctrMkNode("fld",opt,-1,"/prm/lf_tm",_("Life time of the authentication (min)"),0660,"root","root",1,"tp","dec");
	ctrMkNode("fld",opt,-1,"/help/g_help",_("Options help"),0440,"root","root",3,"tp","str","cols","90","rows","5");
	return;
    }

    //> Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/prm/lf_tm" )
    {
	if( ctrChkNode(opt,"get",0660,"root","root",SEC_RD) )	opt->setText( TSYS::int2str(authTime()) );
	if( ctrChkNode(opt,"set",0660,"root","root",SEC_WR) )	setAuthTime( atoi(opt->text().c_str()) );
    }
    else if( a_path == "/help/g_help" && ctrChkNode(opt,"get",0440) )	opt->setText(optDescr());
    else TProtocol::cntrCmdProc(opt);
}


//*************************************************
//* TProtIn                                       *
//*************************************************
TProtIn::TProtIn( string name ) : TProtocolIn(name), m_nofull(false)
{

}

TProtIn::~TProtIn()
{

}

string TProtIn::httpHead( const string &rcode, int cln, const string &addattr )
{
    return  "HTTP/1.0 "+rcode+"\r\n"
	    "Server: "+PACKAGE_STRING+"\r\n"
	    "Accept-Ranges: bytes\r\n"
	    "Content-Length: "+TSYS::int2str(cln)+"\r\n"
	    "Content-Type: text/html;charset="+Mess->charset()+"\r\n"+addattr+"\r\n";
}

bool TProtIn::mess( const string &reqst, string &answer, const string &sender )
{
    bool KeepAlive = false;
    string req, sel;
    vector<string> vars;

    //> Continue for full reqst
    if( m_nofull )
    {
	m_buf = m_buf+reqst;
	m_nofull = false;
    }
    else m_buf=reqst;  //Save request to bufer

    string request = m_buf;

    answer = "";
    if( request.size() > 0 )
    {
	int	pos = 0;
#if OSC_DEBUG >= 3
	mess_debug(nodePath().c_str(),_("Content: %d:\n%s"),request.size(),request.c_str());
#endif

	//> Parse first record
	req = TSYS::strSepParse(request,0,'\n',&pos);
	if( !req.empty() ) req.resize(req.size()-1);
	string method   = TSYS::strSepParse(req,0,' ');
	string url      = TSYS::strSepParse(req,1,' ');
	string protocol = TSYS::strSepParse(req,2,' ');
	string user;

	//> Parse parameters
	int c_lng=-1;
	while( true )
	{
	    req = TSYS::strSepParse(request,0,'\n',&pos);
	    if( req.empty() ) { m_nofull=true; break; }
	    if( req == "\r" ) break;
	    req.resize(req.size()-1);
	    int sepPos = req.find(":",0);
	    if( sepPos == 0 || sepPos == string::npos ) break;
	    string var = req.substr(0,sepPos);
	    string val = req.substr(sepPos+1);
	    vars.push_back(req);

	    if( strcasecmp(var.c_str(),"content-length") == 0 )	c_lng = atoi(val.c_str());
	    else if( strcasecmp(var.c_str(),"connection") == 0 )
	    {
		for( int off = 0; (sel=TSYS::strSepParse(val,0,',',&off)).size(); )
		    if( strcasecmp(TSYS::strNoSpace(sel).c_str(),"keep-alive") == 0 )
		    { KeepAlive = true; break; }
	    }
	    else if( strcasecmp(var.c_str(),"cookie") == 0 )
	    {
		int vpos = val.find("oscd_u_id=",0);
		if( vpos != string::npos ) user = mod->sesCheck(atoi(val.substr(vpos+10).c_str()));
	    }
	}

	//> Check content length
	if( (c_lng >= 0 && c_lng > (request.size()-pos)) || (c_lng < 0 && method == "POST") ) m_nofull = true;
	if( m_nofull ) return m_nofull;

	//> Check protocol version
	if( protocol != "HTTP/1.0" && protocol != "HTTP/1.1" )
	{
	    answer = "<html>\n"
		     " <body>\n"
		     "  <h1>Bad Request</h1>\n"
		     "  <p>This server did not undersand your request.</p>\n"
		     " </body>\n"
		     "</html>\n";
	    answer = httpHead("400 Bad Request",answer.size())+answer;
	    return m_nofull||KeepAlive;
	}

	int url_pos = 0;
	string name_mod = TSYS::pathLev(url,0,false,&url_pos);
	while( url_pos < url.size() && url[url_pos] == '/' ) url_pos++;
	url = "/"+url.substr(url_pos);

	//> Process internal commands
	if( name_mod == "login" )
	{
	    if( method == "GET" )	{ answer = getAuth(url); return m_nofull||KeepAlive; }
	    else if( method == "POST" )
	    {
		map<string,string>	cnt;
		map<string,string>::iterator cntEl;
		getCnt(vars,request.substr(pos),cnt);
		if( cnt.find("auth_enter") != cnt.end() )
		{
		    string pass;
		    if( (cntEl=cnt.find("user")) != cnt.end() )	user = cntEl->second;
		    if( (cntEl=cnt.find("pass")) != cnt.end() )	pass = cntEl->second;

		    if( SYS->security().at().usrPresent(user) && SYS->security().at().usrAt(user).at().auth(pass) )
		    {
			answer = pgHead("<META HTTP-EQUIV='Refresh' CONTENT='0; URL="+url+"'/>")+
			    "<h2 class='title'>"+TSYS::strMess(_("Going to page: <b>%s</b>"),url.c_str())+"</h2>\n"+pgTail();
			answer = httpHead("200 OK",answer.size(),"Set-Cookie: oscd_u_id="+TSYS::int2str(mod->sesOpen(user))+"; path=/;\r\n")+answer;
			return m_nofull||KeepAlive;
		    }
		}

		answer = getAuth(url,_("<p style='color: #CF8122;'>Auth is wrong! Retry please.</p>"));
		return m_nofull||KeepAlive;
	    }
	}
	else if( name_mod == "logout" && method == "GET" )
	{
	    answer = pgHead("<META HTTP-EQUIV='Refresh' CONTENT='0; URL=/'/>")+
		"<h2 class='title'>"+TSYS::strMess(_("Going to page: <b>%s</b>"),"/")+"</h2>\n"+pgTail();
	    answer = httpHead("200 OK",answer.size(),"Set-Cookie: oscd_u_id=0; path=/;\r\n")+answer;
	    return m_nofull||KeepAlive;
	}

	//> Send request to module
	try
	{
	    AutoHD<TModule> wwwmod = SYS->ui().at().modAt(name_mod);
	    if( wwwmod.at().modInfo("SubType") != "WWW" ) throw TError(nodePath().c_str(),"Find no one WWW subtype module!");
	    if( atoi(wwwmod.at().modInfo("Auth").c_str()) && user.empty() )
	    {
		answer = pgHead("<META HTTP-EQUIV='Refresh' CONTENT='0; URL=/login/"+(name_mod+url)+"'/>")+
		    "<h2 class='title'>"+TSYS::strMess(_("Going to page: <b>%s</b>"),("/login/"+name_mod+url).c_str())+"</h2>\n"+pgTail();
		answer = httpHead("200 OK",answer.size())+answer;
		return m_nofull||KeepAlive;
	    }

	    //>> Check metods
	    if( method == "GET" )
	    {
		void(TModule::*HttpGet)( const string &url, string &page, const string &sender, vector<string> &vars, const string &user );
		wwwmod.at().modFunc("void HttpGet(const string&,string&,const string&,vector<string>&,const string&);",
		    (void (TModule::**)()) &HttpGet);

		((&wwwmod.at())->*HttpGet)(url,answer,sender,vars,user);
#if OSC_DEBUG >= 4
		mess_debug(nodePath().c_str(),"Get Content:\n%s",request.c_str());
#endif
	    }
	    else if( method == "POST" )
	    {
		void(TModule::*HttpPost)( const string &url, string &page, const string &sender, vector<string> &vars, const string &user );
		wwwmod.at().modFunc("void HttpPost(const string&,string&,const string&,vector<string>&,const string&);",
		    (void (TModule::**)()) &HttpPost);

		answer = request.substr(pos);
		((&wwwmod.at())->*HttpPost)(url,answer,sender,vars,user);
#if OSC_DEBUG >= 4
		mess_debug(nodePath().c_str(),"Post Content:\n%s",request.c_str());
#endif
	    }
	    else
	    {
		answer = "<html>\n"
			 " <body>\n"
			 "  <h1>Method Not Implemented</h1>\n"
			 "  <p>The method "+method+" is not implemented by this server.</p>\n"
			 " </body>\n"
			 "</html>\n";
		answer = httpHead("501 Method Not Implemented",answer.size())+answer;
	    }
	}
	catch(TError err){ answer = getIndex(user); }
    }

    return m_nofull||KeepAlive;
}

string TProtIn::pgHead( string head_els )
{
    return
	"<?xml version='1.0' ?>\n"
	"<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Transitional//EN'\n"
	"'DTD/xhtml1-transitional.dtd'>\n"
	"<html xmlns='http://www.w3.org/1999/xhtml'>\n<head>\n"
	"<meta http-equiv='Content-Type' content='text/html; charset="+Mess->charset()+"'/>\n"+
	head_els+
	"<title>"+PACKAGE_NAME+"!"+"</title>\n"
	"<style type='text/css'>\n"
	"  hr { width: 95%; }\n"
	"  p { margin: 0px; text-indent: 10px; margin-bottom: 5px; }\n"
	"  body { background-color: #818181; margin: 0px; }\n"
	"  h1.head { text-align: center; color: #ffff00; }\n"
	"  h2.title { text-align: center; font-style: italic; margin: 0px; padding: 0px; border-width: 0px; }\n"
	"  table.work { background-color: #9999ff; border: 3px ridge #a9a9a9; padding: 2px;  }\n"
	"  table.work td { background-color:#cccccc; text-align: left; }\n"
	"  table.work td.content { padding: 5px; padding-bottom: 20px; }\n"
	"  table.work ul { margin: 0px; padding: 0px; padding-left: 20px; }\n"
	"</style>\n"
	"</head>\n"
	"<body>\n"
	"<h1 class='head'>"+PACKAGE_NAME+"</h1>\n"
	"<hr/><br/>\n";
}

string TProtIn::pgTail()
{
    return
	"<hr/>\n"
	"</body>\n"
	"</html>\n";
}

string TProtIn::getIndex( const string &user )
{
    string answer = pgHead()+"<center><table class='work' width='50%'>\n"
	"<tr><th>"+_("Login")+"</th></tr>"
	"<tr><td class='content'>"
	"<p>"+_("Welcome to Web-interfaces of OpenSCADA system.")+"</p>";
    if( !user.empty() )
	answer = answer +
	    "<p style='color: green;'>"+TSYS::strMess(_("You are login as <b>'%s'</b>."),user.c_str())+"</p>"
	    "<p>"+_("Select need Web-module from list below or press <a href='/logout'>here</a> for logout or <a href='/login'>here</a> for login as other user.")+"</p>";
    else
	answer = answer +
	    "<p style='color: #CF8122;'>"+_("You are not login now!")+"</p>"
	    "<p>"+_("For use some modules you must be login. For login now click <a href='/login'>here</a>.")+"</p>";
    answer += "</td></tr>";

    answer = answer +
	"<tr><th>"+_("Present web modules")+"</th></tr>\n"
	"<tr><td class='content'><ul>\n";
    vector<string> list;
    owner().owner().owner().ui().at().modList(list);
    for( unsigned i_l = 0; i_l < list.size(); i_l++ )
    {
	AutoHD<TModule> mod = owner().owner().owner().ui().at().modAt(list[i_l]);
	if( mod.at().modInfo("SubType") == "WWW" )
	    answer = answer+"<li><a href='"+list[i_l]+"/'>"+mod.at().modInfo("Name")+"</a></li>\n";
    }
    answer = answer+"</ul></td></tr></table></center>\n"+pgTail();

    return httpHead("200 OK",answer.size())+answer;
}

string TProtIn::getAuth( const string& url, const string &mess )
{
    string answer = pgHead()+"<center><table class='work'>"
	"<tr><th>"+_("Login to system")+"</th></tr>\n"
	"<tr><td>\n"
	"<form method='post' action='/login"+url+"' enctype='multipart/form-data'>\n"
	"<table cellpadding='3px'>\n"
	"<tr><td><b>"+_("User name")+"</b></td><td><input type='text' name='user' size='20'/></td></tr>\n"
	"<tr><td><b>"+_("Password")+"</b></td><td><input type='password' name='pass' size='20'/></td></tr>\n"
	"<tr><td colspan='2' style='text-align: center'><input type='submit' name='auth_enter' value='"+_("Enter")+"'/>&nbsp;"
	"<input type='reset' name='clean' value='"+_("Clean")+"'/></td></tr>"
	"</table>\n</form>\n"
	"</td></tr>"
	"<tr><td>"+mess+"</td></tr>"
	"</table></center>\n"+pgTail();

    return httpHead("200 OK",answer.size())+answer;
}

void TProtIn::getCnt( const vector<string> &vars, const string &content, map<string,string> &cnt )
{
    //> Content parse
    int pos = 0, i_bnd;
    string boundary;
    const char *c_bound = "boundary=";
    const char *c_term = "\r\n";
    const char *c_end = "--";
    const char *c_fd = "Content-Disposition: form-data;";
    const char *c_name = "name=\"";
    const char *c_file = "filename=\"";

    for( int i_vr = 0; i_vr < vars.size(); i_vr++ )
	if( vars[i_vr].substr(0,vars[i_vr].find(":",0)) == "Content-Type" )
	{
	    int pos = vars[i_vr].find(c_bound,0)+strlen(c_bound);
	    boundary = vars[i_vr].substr(pos,vars[i_vr].size()-pos);
	}
    if( !boundary.size() ) return;

    while(true)
    {
	pos = content.find(boundary,pos);
	if( pos == string::npos || content.substr(pos+boundary.size(),2) == "--" ) break;
	    pos += boundary.size()+strlen(c_term);
	string c_head = content.substr(pos, content.find(c_term,pos)-pos);
	if( c_head.find(c_fd,0) == string::npos ) continue;

	//>> Get name
	i_bnd = c_head.find(c_name,0)+strlen(c_name);
	string c_name = c_head.substr(i_bnd,c_head.find("\"",i_bnd)-i_bnd);
	i_bnd = c_head.find(c_file,0);
	if( i_bnd == string::npos )
	{
	    //>>> Get value
	    pos += c_head.size()+(2*strlen(c_term));
	    if(pos >= content.size()) break;
	    string c_val  = content.substr(pos, content.find(string(c_term)+c_end+boundary,pos)-pos);
	    cnt[c_name] = c_val;
	}
	else
	{
	    i_bnd += strlen(c_file);
	    cnt[c_name] = c_head.substr(i_bnd,c_head.find("\"",i_bnd)-i_bnd);
	}
    }
}
