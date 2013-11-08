
//OpenSCADA system module Protocol.HTTP file: http.cpp
/***************************************************************************
 *   Copyright (C) 2003-2010 by Roman Savochenko                           *
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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include <tsys.h>
#include <tmess.h>
#include <tmodule.h>
#include <tuis.h>
#include "http.h"

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"HTTP"
#define MOD_NAME	_("HTTP-realization")
#define MOD_TYPE	SPRT_ID
#define VER_TYPE	SPRT_VER
#define MOD_VER		"1.6.0"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("Allow support HTTP for WWW based UIs.")
#define LICENSE		"GPL2"
//*************************************************

PrHTTP::TProt *PrHTTP::mod;

extern "C"
{
#ifdef MOD_INCL
    TModule::SAt prt_HTTP_module( int n_mod )
#else
    TModule::SAt module( int n_mod )
#endif
    {
	if(n_mod==0)	return TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE);
	return TModule::SAt("");
    }

#ifdef MOD_INCL
    TModule *prt_HTTP_attach( const TModule::SAt &AtMod, const string &source )
#else
    TModule *attach( const TModule::SAt &AtMod, const string &source )
#endif
    {
	if(AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE))
	    return new PrHTTP::TProt(source);
	return NULL;
    }
}

using namespace PrHTTP;

//*************************************************
//* TProt                                         *
//*************************************************
TProt::TProt( string name ) : TProtocol(MOD_ID), mTAuth(10), lst_ses_chk(0)
{
    mod		= this;

    mType	= MOD_TYPE;
    mName	= MOD_NAME;
    mVers	= MOD_VER;
    mAuthor	= AUTHORS;
    mDescr	= DESCRIPTION;
    mLicense	= LICENSE;
    mSource	= name;
}

TProt::~TProt()
{

}

string TProt::optDescr( )
{
    char buf[STR_BUF_LEN];
    snprintf(buf,sizeof(buf),_(
	"======================= The module <%s:%s> options =======================\n"
	"---------- Parameters of the module section '%s' in config-file ----------\n"
	"AuthTime <min>      Life time of the authentication, minutes (default 10).\n\n"),
	MOD_TYPE,MOD_ID,nodePath().c_str());

    return buf;
}

void TProt::load_( )
{
    //> Load parameters from command line
    string argCom, argVl;
    for(int argPos = 0; (argCom=SYS->getCmdOpt(argPos,&argVl)).size(); )
        if(argCom == "h" || argCom == "help")	fprintf(stdout,"%s",optDescr().c_str());

    //> Load parameters from config-file
    setAuthTime(atoi(TBDS::genDBGet(nodePath()+"AuthTime",TSYS::int2str(authTime())).c_str()));
    setTmpl(TBDS::genDBGet(nodePath()+"Tmpl",tmpl()));
    //>> Load auto-login config
    ResAlloc res(nodeRes(),true);
    XMLNode aLogNd("aLog");
    try
    {
	aLogNd.load(TBDS::genDBGet(nodePath()+"AutoLogin"));
	for(unsigned i_n = 0; i_n < aLogNd.childSize(); i_n++)
	    mALog.push_back(SAutoLogin(aLogNd.childGet(i_n)->attr("addrs"),aLogNd.childGet(i_n)->attr("user")));
    }catch(...){ }
}

void TProt::save_( )
{
    TBDS::genDBSet(nodePath()+"AuthTime",TSYS::int2str(authTime()));
    TBDS::genDBSet(nodePath()+"Tmpl",tmpl());
    //>> Save auto-login config
    ResAlloc res(nodeRes(),false);
    XMLNode aLogNd("aLog");
    for(unsigned i_n = 0; i_n < mALog.size(); i_n++)
	aLogNd.childAdd("it")->setAttr("addrs",mALog[i_n].addrs)->setAttr("user",mALog[i_n].user);
    TBDS::genDBSet(nodePath()+"AutoLogin",aLogNd.save());
}

TProtocolIn *TProt::in_open( const string &name )
{
    return new TProtIn(name);
}

int TProt::sesOpen( const string &name )
{
    int sess_id;
    ResAlloc res(nodeRes(),true);

    //> Get free identifier
    do{ sess_id = rand(); }
    while(sess_id == 0 || mAuth.find(sess_id) != mAuth.end());

    //> Add new session authentification
    mAuth[sess_id] = SAuth(name,time(NULL));

    return sess_id;
}

void TProt::sesClose( int sid )
{
    ResAlloc res(nodeRes(),true);
    map<int,SAuth>::iterator authEl = mAuth.find(sid);
    if(authEl != mAuth.end())
    {
	mess_info(nodePath().c_str(),_("Auth exit from user '%s'."),authEl->second.name.c_str());
	mAuth.erase(authEl);
    }
}

string TProt::sesCheck( int sid )
{
    time_t cur_tm = time(NULL);
    map<int,SAuth>::iterator authEl;

    //> Check for close old sessions
    ResAlloc res(nodeRes(),true);
    if(cur_tm > lst_ses_chk+10)
    {
	for(authEl = mAuth.begin(); authEl != mAuth.end(); )
	    if(cur_tm > authEl->second.tAuth+authTime()*60)
	    {
		mess_info(nodePath().c_str(),_("Auth session for user '%s' expired."),authEl->second.name.c_str());
		mAuth.erase(authEl++);
	    }
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

string TProt::autoLogGet( const string &sender )
{
    string addr;
    ResAlloc res(nodeRes(),false);
    for(unsigned i_a = 0; sender.size() && i_a < mALog.size(); i_a++)
	for(int aoff = 0; (addr=TSYS::strParse(mALog[i_a].addrs,0,";",&aoff)).size(); )
	    if(TRegExp(addr, "p").test(sender)) return mALog[i_a].user;
    return "";
}

void TProt::outMess( XMLNode &io, TTransportOut &tro )
{
    string req, resp, hostTr, tw, cnt;
    char buf[1000];
    string host = io.attr("Host");
    string uri = io.attr("URI");
    if(uri.empty()) uri = "/";
    XMLNode *nd;

    ResAlloc resN(tro.nodeRes(), true);

    try
    {
	//> Get host address from transport
	if(tro.owner().modId() == "Sockets") hostTr = TSYS::strParse(tro.addr(),1,":")+":"+TSYS::strParse(tro.addr(),2,":");
	else hostTr = tro.addr();

	//> Set new address
	if(!host.empty() && host != hostTr)
	{
	    tro.stop();
	    if(tro.owner().modId() == "Sockets") tro.setAddr("TCP:"+host);
	    else tro.setAddr(host);
	    hostTr = host;
	}
	host = hostTr;

	//> Prepare request
	if(io.name() == "GET") ;
	else if(io.name() == "POST")
	{
	    //> Content process
	    bool isCnt = false;
	    for(unsigned cnt_c = 0; cnt_c < io.childSize(); cnt_c++)
	    {
		if(io.childGet(cnt_c)->name() != "cnt") continue;
		cnt += "--"cntBnd"\x0D\x0A";
		cnt += "Content-Disposition: form-data; \""+io.childGet(cnt_c)->attr("name")+
		       "\"; filename=\""+io.childGet(cnt_c)->attr("filename")+"\"\x0D\x0A";
		//>> Place appended content properties
		for(unsigned ch_c = 0; ch_c < io.childGet(cnt_c)->childSize(); ch_c++)
		    if(io.childGet(cnt_c)->childGet(ch_c)->name() == "prm")
			cnt += io.childGet(cnt_c)->childGet(ch_c)->attr("id")+": "+
			   io.childGet(cnt_c)->childGet(ch_c)->text()+"\x0D\x0A";
		cnt += "\x0D\x0A"+io.childGet(cnt_c)->text();
		isCnt = true;
	    }
	    if(isCnt)
	    {
		cnt += "--"cntBnd"--\x0D\x0A";
		io.childAdd("prm")->setAttr("id","Content-Type")->setText("multipart/form-data; boundary="cntBnd);
	    }
	    else cnt = io.text();
	    io.childAdd("prm")->setAttr("id","Content-Length")->setText(TSYS::int2str(cnt.size()));
	}
	else throw TError(nodePath().c_str(),TSYS::strMess(_("HTTP method '%s' error or don't support."),io.name().c_str()).c_str());

	//> Place HTTP head
	req = TSYS::strMess("%s %s HTTP/1.1\x0D\x0A",io.name().c_str(),uri.c_str());
	//>> Place main HTTP properties
	req += TSYS::strMess("Host: %s\x0D\x0A",host.c_str());
	req += "User-Agent: "PACKAGE_NAME" v"VERSION"\x0D\x0A";
	//>> Place appended HTTP-properties
	for(unsigned ch_c = 0; ch_c < io.childSize(); ch_c++)
	    if(io.childGet(ch_c)->name() == "prm")
		req += io.childGet(ch_c)->attr("id")+": "+io.childGet(ch_c)->text()+"\x0D\x0A";
	req += "\x0D\x0A"+cnt;

	io.childClear();

	//> Start transport
	if(!tro.startStat()) tro.start();

	//> Put request
	int resp_len = tro.messIO(req.c_str(),req.size(),buf,sizeof(buf),0,true);
	resp.assign(buf,resp_len);

	//> Process response
	io.setText("");
	//>> Parse first record
	int pos = 0;
	tw = TSYS::strLine(resp,0,&pos);
	string protocol	= TSYS::strParse(tw,0," ");
	string rcod	= TSYS::strParse(tw,1," ");
	string rstr	= TSYS::strParse(tw,2," ");
	if((protocol != "HTTP/1.0" && protocol != "HTTP/1.1") || rcod.empty() || rstr.empty())
	    throw TError(nodePath().c_str(),_("HTTP respond error"));
	io.setAttr("Protocol",protocol)->setAttr("RezCod",rcod)->setAttr("RezStr",rstr);

	//> Parse parameters
	int c_lng = -1, ch_ln = 0;
	while(true)
	{
	    tw = TSYS::strLine(resp,0,&pos);
	    if(tw.empty()) break;
	    size_t sepPos = tw.find(":",0);
	    if(sepPos == 0 || sepPos == string::npos) continue;
	    nd = io.childAdd("prm")->setAttr("id",tw.substr(0,sepPos))->setText(TSYS::strNoSpace(tw.substr(sepPos+1)));
	    if(c_lng == -1 && strcasecmp(nd->attr("id").c_str(),"content-length") == 0) c_lng = atoi(nd->text().c_str());
	    if(c_lng != -2 && strcasecmp(nd->attr("id").c_str(),"transfer-encoding") == 0 && nd->text() == "chunked") c_lng = -2;
	}

next_ch:
	//>> Get chunk size
	if(c_lng == -2)
	{
	    if(ch_ln) pos += ch_ln+2;
	    tw = TSYS::strLine(resp,0,&pos);
	    ch_ln = strtol(tw.c_str(),NULL,16);
	}
	else ch_ln = c_lng;

	//>> Wait tail
	while(ch_ln > 0 && ((int)(resp.size()-pos) < ch_ln ||
	    (c_lng == -2 && ((int)(resp.size()-pos) < (ch_ln+5) || resp.find("\x0D\x0A",pos+ch_ln+2) == string::npos))))
	{
	    resp_len = tro.messIO(NULL,0,buf,sizeof(buf),0,true);
	    if(!resp_len) throw TError(nodePath().c_str(),_("Not full respond."));
	    resp.append(buf,resp_len);
	}

	//>> Put body
	io.setText(io.text()+resp.substr(pos,ch_ln));

	//>> Next chunk process
	if(c_lng == -2 && ch_ln != 0) goto next_ch;
    }
    catch(TError err)	{ io.setAttr("err",err.mess); return; }

    io.setAttr("err","");
}

void TProt::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if(opt->name() == "info")
    {
	TProtocol::cntrCmdProc(opt);
	if(ctrMkNode("area",opt,0,"/prm",_("Protocol")))
	{
	    ctrMkNode("fld",opt,-1,"/prm/lf_tm",_("Life time of the authentication (min)"),RWRWR_,"root",SPRT_ID,1,"tp","dec");
	    ctrMkNode("fld",opt,-1,"/prm/tmpl",_("HTML-template"),RWRWR_,"root",SPRT_ID,3,"tp","str","dest","sel_ed","select","/prm/tmplList");
	    if(ctrMkNode("table",opt,-1,"/prm/alog",_("Auto login"),RWRWR_,"root",SPRT_ID,2,"s_com","add,del,ins",
		"help",_("For address field you can use address templates list, for example \"192.168.1.*;192.168.2.*\".")))
	    {
		ctrMkNode("list",opt,-1,"/prm/alog/addrs",_("Address"),RWRWR_,"root",SPRT_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/prm/alog/user",_("User"),RWRWR_,"root",SPRT_ID,3,"tp","str","dest","select","select","/prm/usr_ls");
	    }
	}
	ctrMkNode("fld",opt,-1,"/help/g_help",_("Module help"),R_R_R_,"root",SPRT_ID,3,"tp","str","cols","90","rows","5");
	return;
    }

    //> Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/lf_tm")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD))	opt->setText(TSYS::int2str(authTime()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SPRT_ID,SEC_WR))	setAuthTime(atoi(opt->text().c_str()));
    }
    else if(a_path == "/prm/alog")
    {
	int idrow = atoi(opt->attr("row").c_str());
	string idcol = opt->attr("col");
	if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD))
	{
	    XMLNode *n_addrs	= ctrMkNode("list",opt,-1,"/prm/alog/addrs","");
	    XMLNode *n_user	= ctrMkNode("list",opt,-1,"/prm/alog/user","");

	    ResAlloc res(nodeRes(),false);
	    for(unsigned i_a = 0; i_a < mALog.size(); i_a++)
	    {
		if(n_addrs)	n_addrs->childAdd("el")->setText(mALog[i_a].addrs);
		if(n_user)	n_user->childAdd("el")->setText(mALog[i_a].user);
	    }
	    return;
	}
	ResAlloc res(nodeRes(),true);
	modif();
	if(ctrChkNode(opt,"add",RWRWR_,"root",SPRT_ID,SEC_WR))	mALog.push_back(SAutoLogin());
	else if(ctrChkNode(opt,"ins",RWRWR_,"root",SPRT_ID,SEC_WR) && (idrow >= 0 || idrow < (int)mALog.size()))
	    mALog.insert(mALog.begin()+idrow, SAutoLogin());
	else if(ctrChkNode(opt,"del",RWRWR_,"root",SPRT_ID,SEC_WR) && (idrow >= 0 || idrow < (int)mALog.size()))
	    mALog.erase(mALog.begin()+idrow);
	else if(ctrChkNode(opt,"set",RWRWR_,"root",SPRT_ID,SEC_WR) && (idrow >= 0 || idrow < (int)mALog.size()))
	{
	    if(idcol == "addrs")mALog[idrow].addrs = opt->text();
	    if(idcol == "user")	mALog[idrow].user = opt->text();
	}
    }
    else if(a_path == "/prm/tmpl")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD))	opt->setText(tmpl());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SPRT_ID,SEC_WR))	setTmpl(opt->text());
    }
    else if(a_path == "/prm/tmplList" && ctrChkNode(opt))	TSYS::ctrListFS(opt, tmpl(), "html;xhtml;xml;");
    else if(a_path == "/prm/usr_ls" && ctrChkNode(opt))
    {
	vector<string> ls;
	SYS->security().at().usrList(ls);
	for(unsigned i_l = 0; i_l < ls.size(); i_l++)
	    opt->childAdd("el")->setText(ls[i_l]);
    }
    else if(a_path == "/help/g_help" && ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID))
	opt->setText(optDescr());
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

string TProtIn::httpHead( const string &rcode, int cln, const string &addattr, bool defCtx )
{
    return "HTTP/1.0 "+rcode+"\x0D\x0A"
	   "Server: "+PACKAGE_STRING+"\x0D\x0A"
	   "Accept-Ranges: bytes\x0D\x0A"
	   "Content-Length: "+TSYS::int2str(cln)+"\x0D\x0A"+
	   (defCtx?("Content-Type: text/html;charset="+Mess->charset()+"\x0D\x0A"):string(""))+addattr+"\x0D\x0A";
}

bool TProtIn::mess( const string &reqst, string &answer )
{
    bool KeepAlive = false;
    string req, sel, userAgent;
    int sesId = 0;
    vector<string> vars;
    string sender = TSYS::strLine(srcAddr(),0);

    //> Continue for full reqst
    if(m_nofull)
    {
	m_buf.append(reqst);
	m_nofull = false;
    }
    else m_buf = reqst;  //Save request to buffer

    string request = m_buf;

    answer = "";
    if( request.size() > 0 )
    {
	int	pos = 0;
	if(mess_lev() == TMess::Debug)
	    mess_debug(nodePath().c_str(), _("Content: %d:\n%s"), request.size(), request.c_str());

	//> Parse first record
	req = TSYS::strLine(request,0,&pos);
	if(req == request) { m_nofull = true; return m_nofull; }	//HTTP header is not full
	string method   = TSYS::strSepParse(req,0,' ');
	string urls     = TSYS::strSepParse(req,1,' ');
	string protocol = TSYS::strSepParse(req,2,' ');
	string user, url;

	//> Parse parameters
	int c_lng = -1;
	while( true )
	{
	    req = TSYS::strLine(request,0,&pos);
	    if(req.empty()) break;
	    size_t sepPos = req.find(":",0);
	    if(sepPos == 0 || sepPos == string::npos) break;
	    string var = req.substr(0,sepPos);
	    string val = req.substr(sepPos+1);
	    vars.push_back(req);

	    if(strcasecmp(var.c_str(),"content-length") == 0)	c_lng = atoi(val.c_str());
	    else if(strcasecmp(var.c_str(),"user-agent") == 0)	userAgent = TSYS::strNoSpace(val);
	    else if(strcasecmp(var.c_str(),"connection") == 0)
	    {
		for(int off = 0; (sel=TSYS::strSepParse(val,0,',',&off)).size(); )
		    if(strcasecmp(TSYS::strNoSpace(sel).c_str(),"keep-alive") == 0)
		    { KeepAlive = true; break; }
	    }
	    else if(strcasecmp(var.c_str(),"cookie") == 0)
	    {
		size_t vpos = val.find("oscd_u_id=",0);
		if(vpos != string::npos) user = mod->sesCheck((sesId=atoi(val.substr(vpos+10).c_str())));
	    }
	}

	//> Check content length
	if((c_lng >= 0 && c_lng > (int)(request.size()-pos)) || (c_lng < 0 && method == "POST")) m_nofull = true;
	if(m_nofull) return m_nofull;

	//> Check protocol version
	if(protocol != "HTTP/1.0" && protocol != "HTTP/1.1")
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
	string name_mod = TSYS::pathLev(urls,0,false,&url_pos);
	while(url_pos < (int)urls.size() && urls[url_pos] == '/') url_pos++;
	url = "/"+urls.substr(url_pos);

	//> Process internal commands
	if(name_mod == "login")
	{
	    if(method == "GET")
	    {
		if(sesId) mod->sesClose(sesId);
		answer = getAuth(url);
		return m_nofull||KeepAlive;
	    }
	    else if(method == "POST")
	    {
		map<string,string>	cnt;
		map<string,string>::iterator cntEl;
		getCnt(vars,request.substr(pos),cnt);
		if(cnt.find("auth_enter") != cnt.end())
		{
		    string pass;
		    if((cntEl=cnt.find("user")) != cnt.end())	user = cntEl->second;
		    if((cntEl=cnt.find("pass")) != cnt.end())	pass = cntEl->second;

		    if(mod->autoLogGet(sender) == user || (SYS->security().at().usrPresent(user) && SYS->security().at().usrAt(user).at().auth(pass)))
		    {
			mess_info(owner().nodePath().c_str(),_("Auth OK from user '%s'. Host: %s. User agent: %s."),user.c_str(),sender.c_str(),userAgent.c_str());
			answer = pgTmpl("<h2 class='title'>"+TSYS::strMess(_("Going to page: <b>%s</b>"),url.c_str())+"</h2>\n",
			    "<META HTTP-EQUIV='Refresh' CONTENT='0; URL="+url+"'/>");
			answer = httpHead("200 OK",answer.size(),"Set-Cookie: oscd_u_id="+TSYS::int2str(mod->sesOpen(user))+"; path=/;\x0D\x0A")+answer;
			return m_nofull||KeepAlive;
		    }
		}

		mess_warning(owner().nodePath().c_str(),_("Auth wrong from user '%s'. Host: %s. User agent: %s."),user.c_str(),sender.c_str(),userAgent.c_str());
		answer = getAuth(url,_("<p style='color: #CF8122;'>Auth is wrong! Retry please.</p>"));
		return m_nofull||KeepAlive;
	    }
	}
	else if(name_mod == "logout" && method == "GET")
	{
	    if(sesId) mod->sesClose(sesId);
	    answer = pgTmpl("<h2 class='title'>"+TSYS::strMess(_("Going to page: <b>%s</b>"),"/")+"</h2>\n",
		"<META HTTP-EQUIV='Refresh' CONTENT='0; URL=/'/>");
	    answer = httpHead("200 OK",answer.size(),"Set-Cookie: oscd_u_id=0; path=/;\x0D\x0A")+answer;
	    return m_nofull||KeepAlive;
	}
	else if(name_mod == "robots.txt" && method == "GET")
	{
	    answer = "User-Agent: *\nDisallow: /";
	    answer = httpHead("200 OK",answer.size(),"Content-Type: text/plain;charset=us-ascii\x0D\x0A",false)+answer;
	    return m_nofull||KeepAlive;
	}

	//> Send request to module
	try
	{
	    AutoHD<TModule> wwwmod = SYS->ui().at().modAt(name_mod);
	    if(wwwmod.at().modInfo("SubType") != "WWW") throw TError(nodePath().c_str(),_("Find no one WWW subtype module!"));
	    if(atoi(wwwmod.at().modInfo("Auth").c_str()) && user.empty())
	    {
		//>> Check for auto-login
		user = mod->autoLogGet(sender);
		if(!user.empty())
		{
		    mess_info(owner().nodePath().c_str(),_("Auto auth from user '%s'. Host: %s. User agent: %s."),user.c_str(),sender.c_str(),userAgent.c_str());
		    answer = pgTmpl("<h2 class='title'>"+TSYS::strMess(_("Going to page: <b>%s</b>"),url.c_str())+"</h2>\n",
			"<META HTTP-EQUIV='Refresh' CONTENT='0; URL="+urls+"'/>");
		    answer = httpHead("200 OK",answer.size(),"Set-Cookie: oscd_u_id="+TSYS::int2str(mod->sesOpen(user))+"; path=/;\x0D\x0A")+answer;
		}
		else
		{
		    answer = pgTmpl("<h2 class='title'>"+TSYS::strMess(_("Going to page: <b>%s</b>"),("/login/"+name_mod+url).c_str())+"</h2>\n",
			"<META HTTP-EQUIV='Refresh' CONTENT='0; URL=/login/"+(name_mod+url)+"'/>");
		    answer = httpHead("200 OK",answer.size())+answer;
		}
		return m_nofull||KeepAlive;
	    }

	    //>> Check metods
	    if( method == "GET" )
	    {
		void(TModule::*HttpGet)( const string &url, string &page, const string &sender, vector<string> &vars, const string &user );
		wwwmod.at().modFunc("void HttpGet(const string&,string&,const string&,vector<string>&,const string&);",
		    (void (TModule::**)()) &HttpGet);

		((&wwwmod.at())->*HttpGet)(url,answer,sender,vars,user);
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "Get Content:\n%s", request.c_str());
	    }
	    else if( method == "POST" )
	    {
		void(TModule::*HttpPost)( const string &url, string &page, const string &sender, vector<string> &vars, const string &user );
		wwwmod.at().modFunc("void HttpPost(const string&,string&,const string&,vector<string>&,const string&);",
		    (void (TModule::**)()) &HttpPost);

		answer = request.substr(pos);
		((&wwwmod.at())->*HttpPost)(url,answer,sender,vars,user);
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "Post Content:\n%s", request.c_str());
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
	catch(TError err)
	{
	    //> Check to direct file request for template
	    if(method == "GET" && mod->tmpl().size() && urls != "/")
	    {
		int hd = -1;
		//> Open file
		size_t tmplDirPos = mod->tmpl().rfind("/");
		if(tmplDirPos != string::npos && (hd=open((mod->tmpl().substr(0,tmplDirPos)+urls).c_str(),O_RDONLY)) != -1)
		{
		    answer.clear();
    		    char buf[STR_BUF_LEN];
    		    for(int len = 0; (len=read(hd,buf,sizeof(buf))) > 0; ) answer.append(buf,len);
    		    close(hd);
		    //> Extension process
		    size_t ext_pos = urls.rfind(".");
		    string fext = (ext_pos != string::npos) ? urls.substr(ext_pos+1) : "";
		    if(fext == "png" || fext == "jpg" || fext == "ico")	answer = httpHead("200 OK",answer.size(),"Content-Type: image/"+fext+"\x0D\x0A",false)+answer;
		    else if(fext == "css" || fext == "html" || fext == "xml") answer = httpHead("200 OK",answer.size(),"Content-Type: text/"+fext+"\x0D\x0A",false)+answer;
		    else if(fext == "js") answer = httpHead("200 OK",answer.size(),"Content-Type: text/javascript\x0D\x0A",false)+answer;
		    else answer = httpHead("200 OK",answer.size())+answer;
		    return m_nofull||KeepAlive;
		}
	    }
	    answer = getIndex(user,sender);
	}
    }

    return m_nofull||KeepAlive;
}

string TProtIn::pgHead( const string &head_els )
{
    return
	"<?xml version='1.0' ?>\n"
	"<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Transitional//EN' 'http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd'>\n"
	"<html xmlns='http://www.w3.org/1999/xhtml'>\n<head>\n"
	"<meta http-equiv='Content-Type' content='text/html; charset="+Mess->charset()+"'/>\n"+
	head_els+
	"<title>"+PACKAGE_NAME+"!"+"</title>\n"
	"<style type='text/css'>\n"
	"  hr { width: 95%; }\n"
	"  p { margin: 0px; text-indent: 15px; margin-bottom: 5px; }\n"
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

string TProtIn::pgTmpl(const string &cnt, const string &head_els)
{
    string answer;
    int hd = open(mod->tmpl().c_str(),O_RDONLY);
    if(hd != -1)
    {
        char buf[STR_BUF_LEN];
        for(int len = 0; (len=read(hd,buf,sizeof(buf))) > 0; ) answer.append(buf,len);
        close(hd);
	if(answer.find("#####CONTEXT#####") == string::npos)	answer.clear();
	else
	{
	    try
	    {
		XMLNode tree("");
		tree.load(answer, true);
		if(head_els.size())
		{
		    XMLNode *headEl = tree.childGet("head");
		    if(headEl)
		    {
			headEl->childAdd("META")->load(head_els);
			answer = tree.save(XMLNode::XHTMLHeader);
		    }
		    else answer.clear();
		}
	    }catch(TError err)
	    {
		mess_err(nodePath().c_str(),_("HTML-template '%s' load error: %s"),mod->tmpl().c_str(),err.mess.c_str());
		answer.clear();
	    }
	}
    }
    if(answer.empty())	answer = pgHead(head_els)+"<center>\n#####CONTEXT#####\n</center>\n"+pgTail();
    size_t tmplPos = answer.find("#####CONTEXT#####");

    return answer.replace(tmplPos,strlen("#####CONTEXT#####"),cnt);
}

string TProtIn::getIndex(const string &user, const string &sender)
{
    string answer = string("<table class='work' width='50%'>\n")+
	"<tr><th>"+_("Login")+"</th></tr>"
	"<tr><td class='content'>"
	"<p>"+_("Welcome to the Web-interfaces of OpenSCADA system.")+"</p>";
    if( !user.empty() )
	answer = answer +
	    "<p style='color: green;'>"+TSYS::strMess(_("You are logged in as \"<b>%s</b>\"."),user.c_str())+"</p>"
	    "<p>"+_("Select the necessary Web-module from the list below, press <a href='/logout'>here</a> to logout or <a href='/login'>here</a> to login as another user.")+"</p>";
    else
    {
	answer = answer +
	    "<p style='color: #CF8122;'>"+_("You are not logged in the system!")+"</p>"
	    "<p>"+_("To use some modules you must be logged in. To login now click <a href='/login'>here</a>.")+"</p>";
	string a_log = mod->autoLogGet(sender);
	if(!a_log.empty())
	    answer += "<p>"+TSYS::strMess(_("You can auto-login from user \"<b>%s</b>\" by simple selecting the module."),a_log.c_str())+"</p>";
    }
    answer += "</td></tr>";

    answer = answer +
	"<tr><th>"+_("Accessible web modules")+"</th></tr>\n"
	"<tr><td class='content'><ul>\n";
    vector<string> list;
    owner().owner().owner().ui().at().modList(list);
    for( unsigned i_l = 0; i_l < list.size(); i_l++ )
    {
	AutoHD<TModule> mod = owner().owner().owner().ui().at().modAt(list[i_l]);
	if( mod.at().modInfo("SubType") == "WWW" )
	    answer = answer+"<li><a href='/"+list[i_l]+"/'>"+mod.at().modInfo("Name")+"</a></li>\n";
    }
    answer = pgTmpl(answer+"</ul></td></tr></table>\n");

    return httpHead("200 OK",answer.size())+answer;
}

string TProtIn::getAuth( const string& url, const string &mess )
{
    string answer = pgTmpl(string("<table class='work'>")+
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
	"</table>\n");

    return httpHead("200 OK",answer.size())+answer;
}

void TProtIn::getCnt( const vector<string> &vars, const string &content, map<string,string> &cnt )
{
    //> Content parse
    string boundary;
    const char *c_bound = "boundary=";
    const char *c_term = "\x0D\x0A";
    const char *c_end = "--";
    const char *c_fd = "Content-Disposition";
    const char *c_name = "name=\"";

    for(size_t i_vr = 0, pos = 0; i_vr < vars.size() && boundary.empty(); i_vr++)
	if(vars[i_vr].compare(0,vars[i_vr].find(":",0),"Content-Type") == 0 && (pos=vars[i_vr].find(c_bound,0)) != string::npos)
	{
	    pos += strlen(c_bound);
	    boundary = vars[i_vr].substr(pos,vars[i_vr].size()-pos);
	}
    if(boundary.empty()) return;

    for(size_t pos = 0, spos = 0, i_bnd = 0; true; )
    {
	pos = content.find(boundary,pos);
        if(pos == string::npos || content.compare(pos+boundary.size(),2,c_end) == 0) break;
        pos += boundary.size()+strlen(c_term);

        //>> Process properties and get name
        string p_name;
        while(pos < content.size())
        {
            string c_head = content.substr(pos, content.find(c_term,pos)-pos);
            pos += c_head.size()+strlen(c_term);
            if(c_head.empty()) break;
            if((spos=c_head.find(":")) == string::npos) return;
            if(c_head.compare(0,spos,c_fd) == 0 && (i_bnd=c_head.find(c_name,spos)) != string::npos)
	    {
		i_bnd += strlen(c_name);
		p_name = c_head.substr(i_bnd,c_head.find("\"",i_bnd)-i_bnd);
	    }
        }
        if(pos >= content.size()) return;
        if(!p_name.empty()) cnt[p_name] = content.substr(pos,content.find(string(c_term)+c_end+boundary,pos)-pos);
    }
}
