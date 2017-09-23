
//OpenSCADA system module Protocol.HTTP file: http.cpp
/***************************************************************************
 *   Copyright (C) 2003-2017 by Roman Savochenko, <rom_as@oscada.org>      *
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
#define MOD_VER		"3.1.1"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("Provides support for the HTTP protocol for WWW-based user interfaces.")
#define LICENSE		"GPL2"
//*************************************************

PrHTTP::TProt *PrHTTP::mod;

extern "C"
{
#ifdef MOD_INCL
    TModule::SAt prot_HTTP_module( int n_mod )
#else
    TModule::SAt module( int n_mod )
#endif
    {
	if(n_mod == 0)	return TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE);
	return TModule::SAt("");
    }

#ifdef MOD_INCL
    TModule *prot_HTTP_attach( const TModule::SAt &AtMod, const string &source )
#else
    TModule *attach( const TModule::SAt &AtMod, const string &source )
#endif
    {
	if(AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE)) return new PrHTTP::TProt(source);
	return NULL;
    }
}

using namespace PrHTTP;

//*************************************************
//* TProt                                         *
//*************************************************
TProt::TProt( string name ) : TProtocol(MOD_ID),
    mDeny(dataRes()), mAllow(dataRes()), mTmpl(dataRes()), mTmplMainPage(dataRes()), mAllowUsersAuth(dataRes()),
    lstSesChk(0), mTAuth(10)
{
    mod = this;

    modInfoMainSet(MOD_NAME, MOD_TYPE, MOD_VER, AUTHORS, DESCRIPTION, LICENSE, name);

    mAllow = "*";
}

TProt::~TProt( )
{

}

string TProt::optDescr( )
{
    char buf[STR_BUF_LEN];
    snprintf(buf,sizeof(buf),_(
	"======================= Module <%s:%s> options =======================\n"
	"---------- Parameters of the module section '%s' in config-file ----------\n"
	"AuthTime <min>      Life time of the authentication, minutes (default 10).\n\n"),
	MOD_TYPE,MOD_ID,nodePath().c_str());

    return buf;
}

void TProt::load_( )
{
    //Load parameters from command line
    string argCom, argVl;
    for(int argPos = 0; (argCom=SYS->getCmdOpt(argPos,&argVl)).size(); )
	if(argCom == "h" || argCom == "help")	fprintf(stdout,"%s",optDescr().c_str());

    //Load parameters from config-file
    setDeny(TBDS::genDBGet(nodePath()+"Deny",deny()));
    setAllow(TBDS::genDBGet(nodePath()+"Allow",allow()));
    setTmpl(TBDS::genDBGet(nodePath()+"Tmpl",tmpl()));
    setTmplMainPage(TBDS::genDBGet(nodePath()+"TmplMainPage",tmplMainPage()));
    setAllowUsersAuth(TBDS::genDBGet(nodePath()+"AllowUsersAuth",allowUsersAuth()));
    setAuthTime(s2i(TBDS::genDBGet(nodePath()+"AuthTime",i2s(authTime()))));
    // Load auto-login config
    MtxAlloc res(dataRes(), true);
    XMLNode aLogNd("aLog");
    try {
	aLogNd.load(TBDS::genDBGet(nodePath()+"AutoLogin"));
	for(unsigned iN = 0, iAL = 0; iN < aLogNd.childSize(); iN++) {
	    SAutoLogin al(aLogNd.childGet(iN)->attr("addrs"),aLogNd.childGet(iN)->attr("user"));
	    for(iAL = 0; iAL < mALog.size() && !(mALog[iAL]==al); ++iAL) ;
	    if(iAL >= mALog.size()) mALog.push_back(al);
	}
    } catch(...) { }
}

void TProt::save_( )
{
    TBDS::genDBSet(nodePath()+"Deny", deny());
    TBDS::genDBSet(nodePath()+"Allow", allow());
    TBDS::genDBSet(nodePath()+"Tmpl", tmpl());
    TBDS::genDBSet(nodePath()+"TmplMainPage", tmplMainPage());
    TBDS::genDBSet(nodePath()+"AllowUsersAuth",allowUsersAuth());
    TBDS::genDBSet(nodePath()+"AuthTime", i2s(authTime()));

    //Save auto-login config
    MtxAlloc res(dataRes(), true);
    XMLNode aLogNd("aLog");
    for(unsigned i_n = 0; i_n < mALog.size(); i_n++)
	aLogNd.childAdd("it")->setAttr("addrs", mALog[i_n].addrs)->setAttr("user", mALog[i_n].user);
    TBDS::genDBSet(nodePath()+"AutoLogin", aLogNd.save());
}

TVariant TProt::objFuncCall( const string &iid, vector<TVariant> &prms, const string &user )
{
    //bool pgAccess(string URL) - Checking for page access pointed by the <URL>.
    //  URL       - URL of the checking page.
    if(iid == "pgAccess" && prms.size()) {
	// ???? Check for the allowed pages cache

	TRegExp re;
	string rules, rule;
	// Check for deny
	rules = deny();
	for(int off = 0; (rule=TSYS::strLine(rules,0,&off)).size(); ) {
	    if(rule.size() > 2 && rule[0] == '/' && rule[rule.size()-1] == '/') re.setPattern(rule.substr(1,rule.size()-2));
	    else re.setPattern(rule, "p");
	    if(re.test(prms[0].getS()))	return false;
	}
	// Check for allow
	rules = allow();
	for(int off = 0; (rule=TSYS::strLine(rules,0,&off)).size(); ) {
	    if(rule.size() > 2 && rule[0] == '/' && rule[rule.size()-1] == '/') re.setPattern(rule.substr(1,rule.size()-2));
	    else re.setPattern(rule, "p");
	    if(re.test(prms[0].getS()))	return true;
	}
	return false;
    }
    //string pgCreator( string cnt, string rcode = "200 OK", string httpattrs = "Content-Type: text/html;charset={SYS}",
    //                 string htmlHeadEls = "", string forceTmplFile = "", string lang = "" ) -
    //    Forming page or resource from content <cnt>, wrapped to HTTP result <rcode>, with HTTP additional attributes <httpattrs>,
    //    HTML additional head's element <htmlHeadEls> and forced template file <forceTmplFile>.
    //  cnt       - a page or a resource (images, XML, CSS, JavaScript, ...) content;
    //  rcode     - HTTP result code, like to "200 OK"; empty value there disables addition of the HTTP header;
    //  httpattrs - additional HTTP-attributes, mostly this is "Content-Type" which by default sets to "text/html;charset={SYS}";
    //              only for "Content-Type: text/html" will do wrapping to internal/service or force <forceTmplFile> HTML-template;
    //  htmlHeadEls   - an additional HTML-header's tag, it's mostly META with "Refresh" to pointed URL;
    //  forceTmplFile - force template file for override the internal/service template by the main-page template or other;
    //  lang - language.
    if(iid == "pgCreator" && prms.size()) {
	size_t extPos;

	string forceTmpl = (prms.size() >= 5) ? prms[4].getS() : "";
	string forceTmplExt;
	if((extPos=forceTmpl.rfind(".")) != string::npos) { forceTmplExt = forceTmpl.substr(extPos); forceTmpl = forceTmpl.substr(0, extPos); }
	//bool isForceTmpl = forceTmpl.getS().size();

	string cTmpl = mod->tmpl();
	string cTmplExt;
	if((extPos=cTmpl.rfind(".")) != string::npos)	{ cTmplExt = cTmpl.substr(extPos); cTmpl = cTmpl.substr(0, extPos); }

	string lang = (prms.size() >= 6) ? prms[5].getS() : "";
	if(lang.size() > 2)	lang = lang.substr(0, 2);

	string httpattrs = (prms.size() >= 3) ? prms[2].getS() : "";
	if(httpattrs.find("Content-Type") == string::npos)
	    httpattrs = "Content-Type: text/html;charset="+ Mess->charset() + (httpattrs.size()?"\x0D\x0A":"") + httpattrs;

	string answer;

	if(httpattrs.find("Content-Type: text/html") != string::npos) {
	    int hd = -1;
	    if(forceTmpl.size() && lang.size())		hd = ::open((forceTmpl+"_"+lang+forceTmplExt).c_str(), O_RDONLY);
	    if(hd < 0 && forceTmpl.size())		hd = ::open((forceTmpl+forceTmplExt).c_str(), O_RDONLY);
	    if(hd < 0 && cTmpl.size() && lang.size())	hd = ::open((cTmpl+"_"+lang+cTmplExt).c_str(), O_RDONLY);
	    if(hd < 0 && cTmpl.size())			hd = ::open((cTmpl+cTmplExt).c_str(), O_RDONLY);
	    if(hd >= 0) {
		char buf[STR_BUF_LEN];
		for(int len = 0; (len=read(hd,buf,sizeof(buf))) > 0; ) answer.append(buf, len);
		::close(hd);
		if(answer.find("#####CONTEXT#####") == string::npos && !forceTmpl.size()) answer.clear();
		else {
		    try {
			XMLNode tree("");
			tree.load(answer, true);
			if(prms.size() >= 4 && prms[3].getS().size()) {
			    XMLNode *headEl = tree.childGet("head", 0, true);
			    if(!headEl) headEl = tree.childGet("HEAD", 0, true);
			    if(headEl) {
				headEl->childAdd("META")->load(prms[3].getS());
				answer = tree.save(XMLNode::XHTMLHeader);
			    } else answer.clear();
			}
		    } catch(TError &err) {
			mess_err(nodePath().c_str(), _("HTML template '%s' for language '%s' load error: %s"),
				(forceTmpl.size()?(forceTmpl+forceTmplExt):(cTmpl+cTmplExt)).c_str(), lang.c_str(), err.mess.c_str());
			answer.clear();
		    }
		}
	    }
	    if(answer.empty())
		answer = "<?xml version='1.0' ?>\n"
		    "<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Transitional//EN' 'http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd'>\n"
		    "<html xmlns='http://www.w3.org/1999/xhtml'>\n"
		    " <head>\n"
		    "  <meta http-equiv='Content-Type' content='text/html; charset=" + Mess->charset() + "'/>\n" +
		    ((prms.size() >= 4) ? prms[3].getS() : "" ) +
		    "  <title>" PACKAGE_NAME ": " + SYS->id() + "</title>\n"
		    "  <link rel='shortcut icon' href='/" + SYS->id() + ".png' type='image' />\n"
		//    "  <title>" PACKAGE_NAME "!</title>\n"
		//    "  <link rel='shortcut icon' href='/" SPRT_ID "." MOD_ID ".png' type='image' />\n"
		    "  <style type='text/css'>\n"
		    "   hr { width: 95%; }\n"
		    "   p { margin: 0px; text-indent: 15px; margin-bottom: 5px; }\n"
		    "   body { background-color: #818181; margin: 0px; }\n"
		    "   div.error { width: 60%; border: 4px outset #B0B0B0; color: red; background-color: #B0B0B0; font-size: 200%; padding: 10px; }\n"
		    "   h1.head { text-align: center; color: #ffff00; }\n"
		    "   h2.title { text-align: center; font-style: italic; margin: 0px; padding: 0px; border-width: 0px; }\n"
		    "   table.work { background-color: #9999ff; border: 3px ridge #a9a9a9; padding: 2px;  }\n"
		    "   table.work td { background-color:#cccccc; text-align: left; }\n"
		    "   table.work td.content { padding: 5px; padding-bottom: 20px; }\n"
		    "   table.work td.content img { vertical-align: middle; }\n"
		    "   table.work ul { list-style-image: none; list-style-type: none; margin: 0px; padding: 0px; padding-left: 20px; }\n"
		    "  </style>\n"
		    " </head>\n"
		    " <body>\n"
		    "  <h1 class='head'>" PACKAGE_NAME ": " + SYS->id() + "</h1>\n"
		    "  <hr/><br/>\n"
		    "<center>\n" CtxTmplMark "</center>\n"
		    "  <hr/>\n"
		    " </body>\n"
		    "</html>\n";

	    size_t tmplPos = answer.find(CtxTmplMark);
	    if(tmplPos != string::npos) answer = answer.replace(tmplPos, strlen(CtxTmplMark), prms[0].getS());
	} else answer = prms[0].getS();

	if(prms.size() < 2 || !prms[1].getS().size()) return answer;

	return "HTTP/1.0 " + prms[1].getS() + "\x0D\x0A"
	   "Server: " + PACKAGE_STRING + "\x0D\x0A"
	   "Accept-Ranges: bytes\x0D\x0A"
	   "Content-Length: " + i2s(answer.size()) + "\x0D\x0A" +
	   httpattrs + "\x0D\x0A\x0D\x0A" + answer;
    }

    return TCntrNode::objFuncCall(iid, prms, user);
}

TProtocolIn *TProt::in_open( const string &name )	{ return new TProtIn(name); }

int TProt::sesOpen( const string &name, const string &srcAddr, const string &userAgent )
{
    int sess_id;
    MtxAlloc res(dataRes(), true);

    //Get free identifier
    do{ sess_id = rand(); }
    while(sess_id == 0 || mAuth.find(sess_id) != mAuth.end());

    //Add new session authentification
    mAuth[sess_id] = SAuth(name, time(NULL), srcAddr, userAgent);

    return sess_id;
}

void TProt::sesClose( int sid )
{
    MtxAlloc res(dataRes(), true);
    map<int,SAuth>::iterator authEl = mAuth.find(sid);
    if(authEl != mAuth.end()) {
	mess_info(nodePath().c_str(),_("Auth exit from user '%s'."),authEl->second.name.c_str());
	mAuth.erase(authEl);
    }
}

string TProt::sesCheck( int sid )
{
    time_t cur_tm = time(NULL);
    map<int,SAuth>::iterator authEl;

    //Check for close old sessions
    MtxAlloc res(dataRes(), true);
    if(cur_tm > lstSesChk+10) {
	for(authEl = mAuth.begin(); authEl != mAuth.end(); )
	    if(cur_tm > authEl->second.tAuth+authTime()*60) {
		mess_info(nodePath().c_str(),_("Auth session for user '%s' expired."),authEl->second.name.c_str());
		mAuth.erase(authEl++);
	    } else authEl++;
	lstSesChk = cur_tm;
    }

    //Check for session
    authEl = mAuth.find(sid);
    if(authEl != mAuth.end()) {
	authEl->second.tAuth = cur_tm;
	return authEl->second.name;
    }

    return "";
}

string TProt::autoLogGet( const string &sender )
{
    string addr;
    MtxAlloc res(dataRes(), true);
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
    bool onlyHeader = s2i(io.attr("onlyHeader"));
    if(uri.empty()) uri = "/";
    XMLNode *nd;

    MtxAlloc resN(tro.reqRes(), true);

    try {
	//Get host address from transport
	hostTr = (tro.owner().modId()=="Sockets") ? TSYS::strParse(tro.addr(),1,":")+":"+TSYS::strParse(tro.addr(),2,":") : tro.addr();

	//Set new address
	if(!host.empty() && host != hostTr) {
	    tro.stop();
	    tro.setAddr((tro.owner().modId()=="Sockets")?"TCP:"+host:host);
	    hostTr = host;
	}
	host = hostTr;

	//Prepare request
	if(io.name() == "GET") ;
	else if(io.name() == "POST") {
	    //Content process
	    bool isCnt = false;
	    for(unsigned cnt_c = 0; cnt_c < io.childSize(); cnt_c++) {
		if(io.childGet(cnt_c)->name() != "cnt") continue;
		cnt += "--" cntBnd "\x0D\x0A";
		cnt += "Content-Disposition: form-data; \""+io.childGet(cnt_c)->attr("name")+
		       "\"; filename=\""+io.childGet(cnt_c)->attr("filename")+"\"\x0D\x0A";
		// Place appended content properties
		for(unsigned ch_c = 0; ch_c < io.childGet(cnt_c)->childSize(); ch_c++)
		    if(io.childGet(cnt_c)->childGet(ch_c)->name() == "prm")
			cnt += io.childGet(cnt_c)->childGet(ch_c)->attr("id")+": "+
			   io.childGet(cnt_c)->childGet(ch_c)->text()+"\x0D\x0A";
		cnt += "\x0D\x0A"+io.childGet(cnt_c)->text();
		isCnt = true;
	    }
	    if(isCnt) {
		cnt += "--" cntBnd "--\x0D\x0A";
		io.childAdd("prm")->setAttr("id","Content-Type")->setText("multipart/form-data; boundary=" cntBnd);
	    } else cnt = io.text();
	    io.childAdd("prm")->setAttr("id","Content-Length")->setText(i2s(cnt.size()));
	}
	else throw TError(nodePath().c_str(),TSYS::strMess(_("HTTP method '%s' error or don't support."),io.name().c_str()).c_str());

	//Place HTTP head
	req = TSYS::strMess("%s %s HTTP/1.1\x0D\x0A",io.name().c_str(),uri.c_str());
	// Place main HTTP properties
	req += TSYS::strMess("Host: %s\x0D\x0A",host.c_str());
	req += "User-Agent: " PACKAGE_NAME " v" VERSION "\x0D\x0A";
	// Place appended HTTP-properties
	for(unsigned ch_c = 0; ch_c < io.childSize(); ch_c++)
	    if(io.childGet(ch_c)->name() == "prm")
		req += io.childGet(ch_c)->attr("id")+": "+io.childGet(ch_c)->text()+"\x0D\x0A";
	req += "\x0D\x0A"+cnt;

	io.childClear();

	//Start transport
	if(!tro.startStat()) tro.start();

	//Put request
	int resp_len = tro.messIO(req.c_str(), req.size(), buf, sizeof(buf));
	resp.assign(buf,resp_len);

	//Process response
	io.setText("");
	// Parse first record
	int pos = 0;
	tw = TSYS::strLine(resp,0,&pos);
	string protocol	= TSYS::strParse(tw, 0, " ");
	string rcod	= TSYS::strParse(tw, 1, " ");
	string rstr	= TSYS::strParse(tw, 2, " ");
	if((protocol != "HTTP/1.0" && protocol != "HTTP/1.1") || rcod.empty() || rstr.empty())
	    throw TError(nodePath().c_str(),_("HTTP respond error"));
	io.setAttr("Protocol",protocol)->setAttr("RezCod",rcod)->setAttr("RezStr",rstr);

	//Parse parameters
	int c_lng = -1, ch_ln = 0;
	while(true) {
	    tw = TSYS::strLine(resp, 0, &pos);
	    if(tw.empty()) break;
	    size_t sepPos = tw.find(":", 0);
	    if(sepPos == 0 || sepPos == string::npos) continue;
	    nd = io.childAdd("prm")->setAttr("id",tw.substr(0,sepPos))->setText(sTrm(tw.substr(sepPos+1)));
	    if(c_lng == -1 && strcasecmp(nd->attr("id").c_str(),"content-length") == 0) c_lng = s2i(nd->text());
	    if(c_lng != -2 && strcasecmp(nd->attr("id").c_str(),"transfer-encoding") == 0 && nd->text() == "chunked") c_lng = -2;
	}

	if(onlyHeader)	{ tro.stop(); return; }

next_ch:
	// Get chunk size
	if(c_lng == -2) {
	    if(ch_ln) pos += ch_ln+2;
	    tw = TSYS::strLine(resp,0,&pos);
	    ch_ln = strtol(tw.c_str(),NULL,16);
	}else ch_ln = c_lng;

	// Wait tail
	while(ch_ln > 0 && ((int)(resp.size()-pos) < ch_ln ||
	    (c_lng == -2 && ((int)(resp.size()-pos) < (ch_ln+5) || resp.find("\x0D\x0A",pos+ch_ln+2) == string::npos))))
	{
	    resp_len = tro.messIO(NULL, 0, buf, sizeof(buf));
	    if(!resp_len) throw TError(nodePath().c_str(), _("Not full respond."));
	    resp.append(buf, resp_len);
	}

	// Put body
	io.setText(io.text()+resp.substr(pos,ch_ln));

	// Next chunk process
	if(c_lng == -2 && ch_ln != 0) goto next_ch;
    } catch(TError &err) { io.setAttr("err",err.mess); return; }

    io.setAttr("err","");
}

void TProt::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TProtocol::cntrCmdProc(opt);
	if(ctrMkNode("area",opt,0,"/prm",_("Protocol"))) {
	    if(ctrMkNode("area",opt,1,"/prm/st",_("State")))
		ctrMkNode("list",opt,-1,"/prm/st/auths",_("Active authentication sessions"),R_R_R_,"root",SPRT_ID);
	    if(ctrMkNode("area",opt,1,"/prm/cfg",_("Module options"))) {
		ctrMkNode("fld",opt,-1,"/prm/cfg/deny",_("Deny"),RWRWR_,"root",SPRT_ID,2, "tp","str", "rows","2");
		ctrMkNode("fld",opt,-1,"/prm/cfg/allow",_("Allow"),RWRWR_,"root",SPRT_ID,2, "tp","str", "rows","2");
		ctrMkNode("fld",opt,-1,"/prm/cfg/tmpl",_("HTML template"),RWRWR_,"root",SPRT_ID,3,
		    "tp","str", "dest","sel_ed", "select","/prm/cfg/tmplList");
		ctrMkNode("fld",opt,-1,"/prm/cfg/tmplMainPage",_("HTML main page template"),RWRWR_,"root",SPRT_ID,3,
		    "tp","str", "dest","sel_ed", "select","/prm/cfg/tmplMainPageList");
		ctrMkNode("fld",opt,-1,"/prm/cfg/lf_tm",_("Life time of the authentication (min)"),RWRWR_,"root",SPRT_ID,1,"tp","dec");
		ctrMkNode("fld",opt,-1,"/prm/cfg/aUsers",_("List of users allowed for auth, separated by ';'"),RWRWR_,"root",SPRT_ID,1,"tp","str");
		if(ctrMkNode("table",opt,-1,"/prm/cfg/alog",_("Auto login"),RWRWR_,"root",SPRT_ID,2,"s_com","add,del,ins",
		    "help",_("For address field you can use address templates list, for example \"192.168.1.*;192.168.2.*\".")))
		{
		    ctrMkNode("list",opt,-1,"/prm/cfg/alog/addrs",_("Address"),RWRWR_,"root",SPRT_ID,1,"tp","str");
		    ctrMkNode("list",opt,-1,"/prm/cfg/alog/user",_("User"),RWRWR_,"root",SPRT_ID,3,"tp","str","dest","select","select","/prm/cfg/usr_ls");
		}
	    }
	}
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/st/auths" && ctrChkNode(opt)) {
	MtxAlloc res(dataRes(), true);
	for(map<int,SAuth>::iterator authEl = mAuth.begin(); authEl != mAuth.end(); ++authEl)
	    opt->childAdd("el")->setText(TSYS::strMess(_("%s %s(%s), by \"%s\""),
		atm2s(authEl->second.tAuth).c_str(),authEl->second.name.c_str(),authEl->second.addr.c_str(),authEl->second.agent.c_str()));
    }
    else if(a_path == "/prm/cfg/deny") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD))	opt->setText(deny());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SPRT_ID,SEC_WR))	setDeny(opt->text());
    }
    else if(a_path == "/prm/cfg/allow") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD))	opt->setText(allow());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SPRT_ID,SEC_WR))	setAllow(opt->text());
    }
    else if(a_path == "/prm/cfg/tmpl") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD))	opt->setText(tmpl());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SPRT_ID,SEC_WR))	setTmpl(opt->text());
    }
    else if(a_path == "/prm/cfg/tmplList" && ctrChkNode(opt))	TSYS::ctrListFS(opt, tmpl(), "html;xhtml;xml;");
    else if(a_path == "/prm/cfg/tmplMainPage") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD))	opt->setText(tmplMainPage());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SPRT_ID,SEC_WR))	setTmplMainPage(opt->text());
    }
    else if(a_path == "/prm/cfg/tmplMainPageList" && ctrChkNode(opt))	TSYS::ctrListFS(opt, tmplMainPage(), "html;xhtml;xml;");
    else if(a_path == "/prm/cfg/lf_tm") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD))	opt->setText(i2s(authTime()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SPRT_ID,SEC_WR))	setAuthTime(s2i(opt->text()));
    }
    else if(a_path == "/prm/cfg/aUsers") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD))	opt->setText(allowUsersAuth());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SPRT_ID,SEC_WR))	setAllowUsersAuth(opt->text());
    }
    else if(a_path == "/prm/cfg/alog") {
	int idrow = s2i(opt->attr("row"));
	string idcol = opt->attr("col");
	if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD)) {
	    XMLNode *n_addrs	= ctrMkNode("list",opt,-1,"/prm/cfg/alog/addrs","");
	    XMLNode *n_user	= ctrMkNode("list",opt,-1,"/prm/cfg/alog/user","");

	    MtxAlloc res(dataRes(), true);
	    for(unsigned i_a = 0; i_a < mALog.size(); i_a++) {
		if(n_addrs)	n_addrs->childAdd("el")->setText(mALog[i_a].addrs);
		if(n_user)	n_user->childAdd("el")->setText(mALog[i_a].user);
	    }
	    return;
	}
	MtxAlloc res(dataRes(), true);
	modif();
	if(ctrChkNode(opt,"add",RWRWR_,"root",SPRT_ID,SEC_WR))	mALog.push_back(SAutoLogin());
	else if(ctrChkNode(opt,"ins",RWRWR_,"root",SPRT_ID,SEC_WR) && (idrow >= 0 || idrow < (int)mALog.size()))
	    mALog.insert(mALog.begin()+idrow, SAutoLogin());
	else if(ctrChkNode(opt,"del",RWRWR_,"root",SPRT_ID,SEC_WR) && (idrow >= 0 || idrow < (int)mALog.size()))
	    mALog.erase(mALog.begin()+idrow);
	else if(ctrChkNode(opt,"set",RWRWR_,"root",SPRT_ID,SEC_WR) && (idrow >= 0 || idrow < (int)mALog.size())) {
	    if(idcol == "addrs")mALog[idrow].addrs = opt->text();
	    if(idcol == "user")	mALog[idrow].user = opt->text();
	}
    }
    else if(a_path == "/prm/cfg/usr_ls" && ctrChkNode(opt)) {
	vector<string> ls;
	SYS->security().at().usrList(ls);
	for(unsigned iL = 0; iL < ls.size(); iL++)
	    opt->childAdd("el")->setText(ls[iL]);
    }
    else TProtocol::cntrCmdProc(opt);
}

//*************************************************
//* TProtIn                                       *
//*************************************************
#undef _
#define _(mess) mod->I18N(mess, lang().c_str())

TProtIn::TProtIn( string name ) : TProtocolIn(name), mNoFull(false)
{

}

TProtIn::~TProtIn( )
{

}

string TProtIn::pgCreator( const string &cnt, const string &rcode, const string &httpattrs, const string &htmlHeadEls, const string &forceTmplFile )
{
    vector<TVariant> prms;
    prms.push_back(cnt); prms.push_back(rcode); prms.push_back(httpattrs); prms.push_back(htmlHeadEls); prms.push_back(forceTmplFile); prms.push_back(lang());

    return owner().objFuncCall("pgCreator", prms, "root").getS();
}

bool TProtIn::pgAccess( const string &URL )
{
    vector<TVariant> prms; prms.push_back(URL);
    return owner().objFuncCall("pgAccess", prms, "root").getB();
}

bool TProtIn::mess( const string &reqst, string &answer )
{
    bool KeepAlive = false;
    string req, sel, userAgent;
    int sesId = 0;
    vector<string> vars;
    string sender = TSYS::strLine(srcAddr(), 0);

    //Continue for full reqst
    if(mNoFull) {
	mBuf.append(reqst);
	mNoFull = false;
    } else mBuf = reqst;	//Save request to buffer

    string request = mBuf;

    answer = "";
    if(request.size() > 0) {
	int	pos = 0;
	if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("Content: %d:\n%s"), request.size(), request.c_str());

	//Parse first record
	req = TSYS::strLine(request,0,&pos);
	if(req == request) { mNoFull = true; return mNoFull; }	//HTTP header is not full
	string method   = TSYS::strSepParse(req, 0, ' ');
	string uris     = TSYS::strSepParse(req, 1, ' ');
	string protocol = TSYS::strSepParse(req, 2, ' ');
	string uri;

	if(!pgAccess(sender+uris)) {
	    answer = pgCreator(TSYS::strMess("<div class='error'>Access for the URL '%s' is forbidden.</div>\n",(sender+uris).c_str()), "403 Forbidden");
	    return mNoFull || KeepAlive;
	}

	prms = user = brLang = prmLang = "";

	//Parse parameters
	int c_lng = -1;
	while(true) {
	    req = TSYS::strLine(request, 0, &pos);
	    if(req.empty()) break;
	    size_t sepPos = req.find(":", 0);
	    if(sepPos == 0 || sepPos == string::npos) break;
	    string var = req.substr(0, sepPos);
	    string val = req.substr(sepPos+1);
	    vars.push_back(req);

	    if(strcasecmp(var.c_str(),"content-length") == 0)	c_lng = s2i(val);
	    else if(strcasecmp(var.c_str(),"user-agent") == 0)	userAgent = sTrm(val);
	    else if(strcasecmp(var.c_str(),"connection") == 0) {
		for(int off = 0; (sel=TSYS::strSepParse(val,0,',',&off)).size(); )
		    if(strcasecmp(sTrm(sel).c_str(),"keep-alive") == 0)
		    { KeepAlive = true; break; }
	    }
	    else if(strcasecmp(var.c_str(),"cookie") == 0) {
		size_t vpos = val.find("oscd_u_id=", 0);
		if(vpos != string::npos) user = mod->sesCheck((sesId=s2i(val.substr(vpos+10))));
	    }
	    else if(strcasecmp(var.c_str(),"accept-language") == 0)
		brLang = TSYS::strTrim(TSYS::strParse(val,0,","));
	    else if(strcasecmp(var.c_str(),"oscd_lang") == 0)	vars.pop_back();
	}

	//Check content length
	if((c_lng >= 0 && c_lng > (int)(request.size()-pos)) || (c_lng < 0 && method == "POST")) mNoFull = true;
	if(mNoFull) return mNoFull;

	//Check protocol version
	if(protocol != "HTTP/1.0" && protocol != "HTTP/1.1") {
	    answer = pgCreator("<div class='error'>Bad Request<br/>This server doesn't undersand your request.</div>\n", "400 Bad Request");
	    return mNoFull || KeepAlive;
	}

	//URL parameters parse
	size_t prmSep = uris.find("?");
	if(prmSep != string::npos) {
	    prms = uris.substr(prmSep);
	    uris = uris.substr(0, prmSep);
	    string sprm;
	    for(int iprm = 1; (sprm=TSYS::strParse(prms,0,"&",&iprm)).size(); ) {
		prmSep = sprm.find("=");
		if(prmSep != string::npos && sprm.substr(0,prmSep) == "lang")	prmLang = sprm.substr(prmSep+1);
	    }
	}

	int uri_pos = 0;
	string name_mod = TSYS::pathLev(uris, 0, false, &uri_pos);
	while(uri_pos < (int)uris.size() && uris[uri_pos] == '/') uri_pos++;
	uri = "/" + uris.substr(uri_pos);

	//Process the internal commands
	// Login
	if(name_mod == "login") {
	    if(method == "GET") {
		if(sesId) mod->sesClose(sesId);
		answer = getAuth(uri);
		return mNoFull || KeepAlive;
	    }
	    else if(method == "POST") {
		map<string,string>	cnt;
		map<string,string>::iterator cntEl;
		getCnt(vars, request.substr(pos), cnt);
		if(cnt.find("auth_enter") != cnt.end()) {
		    string pass;
		    if((cntEl=cnt.find("user")) != cnt.end())	user = cntEl->second;
		    if((cntEl=cnt.find("pass")) != cnt.end())	pass = cntEl->second;

		    if(mod->autoLogGet(sender) == user ||
			((!mod->allowUsersAuth().size() || TRegExp("(^|;)"+user+"(;|$)").test(mod->allowUsersAuth())) &&
			    SYS->security().at().usrPresent(user) && SYS->security().at().usrAt(user).at().auth(pass)))
		    {
			mess_info(owner().nodePath().c_str(), _("Auth OK from user '%s'. Host: %s. User agent: %s."),
			    user.c_str(), sender.c_str(), userAgent.c_str());
			answer = pgCreator("<h2 class='title'>"+TSYS::strMess(_("Going to the page: <b>%s</b>"),(uri+prms).c_str())+"</h2>\n", "200 OK",
				"Set-Cookie: oscd_u_id="+i2s(mod->sesOpen(user,sender,userAgent))+"; path=/;",
				"<META HTTP-EQUIV='Refresh' CONTENT='0; URL="+uri+prms+"'/>");
			return mNoFull || KeepAlive;
		    }
		}

		mess_warning(owner().nodePath().c_str(), _("Auth wrong from user '%s'. Host: %s. User agent: %s."),
		    user.c_str(), sender.c_str(), userAgent.c_str());
		answer = getAuth(uri, _("<p style='color: #CF8122;'>Auth is wrong! Retry please.</p>"));
		return mNoFull || KeepAlive;
	    }
	}
	// Logut
	else if(name_mod == "logout" && method == "GET") {
	    if(sesId) mod->sesClose(sesId);
	    answer = pgCreator("<h2 class='title'>"+TSYS::strMess(_("Going to the page: <b>%s</b>"),"/")+"</h2>\n", "200 OK",
		"Set-Cookie: oscd_u_id=0; path=/;", "<META HTTP-EQUIV='Refresh' CONTENT='0; URL=/'/>");
	    return mNoFull || KeepAlive;
	}
	// robots.txt
	else if(name_mod == "robots.txt" && method == "GET") {
	    answer = pgCreator("User-Agent: *\nDisallow: /", "200 OK", "Content-Type: text/plain;charset=us-ascii");
	    return mNoFull || KeepAlive;
	}

	//Send request to the module
	try {
	    AutoHD<TModule> wwwmod = SYS->ui().at().modAt(name_mod);
	    if(wwwmod.at().modInfo("SubType") != "WWW") throw TError(nodePath().c_str(),_("No one WWW subtype module was found!"));
	    if(s2i(wwwmod.at().modInfo("Auth")) && user.empty()) {
		// Check for auto-login
		user = mod->autoLogGet(sender);
		if(!user.empty()) {
		    mess_info(owner().nodePath().c_str(), _("Auto auth from user '%s'. Host: %s. User agent: %s."),
			user.c_str(), sender.c_str(), userAgent.c_str());
		    answer = pgCreator("<h2 class='title'>"+TSYS::strMess(_("Going to the page: <b>%s</b>"),(uri+prms).c_str())+"</h2>\n", "200 OK",
			"Set-Cookie: oscd_u_id="+i2s(mod->sesOpen(user,sender,userAgent))+"; path=/;",
			"<META HTTP-EQUIV='Refresh' CONTENT='0; URL="+uris+prms+"'/>");
		}
		else answer = pgCreator("<h2 class='title'>"+TSYS::strMess(_("Going to the page: <b>%s</b>"),("/login/"+name_mod+uri+prms).c_str())+"</h2>\n",
			"200 OK", "", "<META HTTP-EQUIV='Refresh' CONTENT='0; URL=/login/"+(name_mod+uri+prms)+"'/>");
		return mNoFull || KeepAlive;
	    }

	    vars.push_back("oscd_lang: "+lang());

	    // Check metods
	    if(method == "GET") {
		void(TModule::*HttpGet)(const string &uri, string &page, const string &sender, vector<string> &vars, const string &user);
		void(TModule::*HTTP_GET)(const string &uri, string &page, vector<string> &vars, const string &user, TProtocolIn *iprt);

		if(wwwmod.at().modFunc("void HTTP_GET(const string&,string&,vector<string>&,const string&,TProtocolIn*);",
			(void (TModule::**)()) &HTTP_GET,true))
		    ((&wwwmod.at())->*HTTP_GET)(uri+prms, answer, vars, user, this);
		else if(wwwmod.at().modFunc("void HttpGet(const string&,string&,const string&,vector<string>&,const string&);",
			(void (TModule::**)()) &HttpGet,true))
		    ((&wwwmod.at())->*HttpGet)(uri+prms, answer, sender, vars, user);
		else throw TError(nodePath().c_str(), _("No a HTTP GET function present for module '%s'!"), name_mod.c_str());
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "Get Content:\n%s", request.c_str());
	    }
	    else if(method == "POST") {
		void(TModule::*HttpPost)(const string &uri, string &page, const string &sender, vector<string> &vars, const string &user);
		void(TModule::*HTTP_POST)(const string &uri, string &page, vector<string> &vars, const string &user, TProtocolIn *iprt);

		answer = request.substr(pos);
		if(wwwmod.at().modFunc("void HTTP_POST(const string&,string&,vector<string>&,const string&,TProtocolIn*);",
			(void (TModule::**)()) &HTTP_POST,true))
		    ((&wwwmod.at())->*HTTP_POST)(uri+prms, answer, vars, user, this);
		else if(wwwmod.at().modFunc("void HttpPost(const string&,string&,const string&,vector<string>&,const string&);",
			(void (TModule::**)()) &HttpPost,true))
		    ((&wwwmod.at())->*HttpPost)(uri+prms, answer, sender, vars, user);
		else throw TError(nodePath().c_str(), _("No a HTTP POST function present for module '%s'!"), name_mod.c_str());
		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), "Post Content:\n%s", request.c_str());
	    }
	    else answer = pgCreator("<div class='error'>Method '"+method+"' isn't implemented by this server!</div>\n",
				    "501 Method Not Implemented");
	} catch(TError &err) {
	    //Check to direct file request for template
	    if(method == "GET" && mod->tmpl().size() && uris != "/") {
		int hd = -1;
		//Open file
		size_t tmplDirPos = mod->tmpl().rfind("/");
		if((tmplDirPos != string::npos && (hd=open((mod->tmpl().substr(0,tmplDirPos)+uris).c_str(),O_RDONLY)) >= 0) ||
			(hd=open(("./"+uris).c_str(),O_RDONLY)) >= 0) {	//From current folder
		    answer.clear();
		    char buf[STR_BUF_LEN];
		    for(int len = 0; (len=read(hd,buf,sizeof(buf))) > 0; ) answer.append(buf,len);
		    close(hd);
		    //Extension process
		    size_t ext_pos = uris.rfind(".");
		    string fext = (ext_pos != string::npos) ? uris.substr(ext_pos+1) : "";
		    if(fext == "png" || fext == "jpg" || fext == "ico")
			answer = pgCreator(answer, "200 OK", "Content-Type: image/"+fext+";");
		    else if(fext == "css" || fext == "html" || fext == "xml")
			answer = pgCreator(answer, "200 OK", "Content-Type: text/"+fext+";");
		    else if(fext == "js")
			answer = pgCreator(answer, "200 OK", "Content-Type: text/javascript;");
		    else answer = pgCreator("<div class='error'>Bad Request!<br/>This server doesn't undersand your request.</div>\n",
					    "400 Bad Request");
		    return mNoFull || KeepAlive;
		}
	    }
	    //Check for module's icon and other images into folder "icons/"
	    if(method == "GET" && name_mod.rfind(".") != string::npos) {
		string icoTp, ico = TUIS::icoGet(name_mod.substr(0,name_mod.rfind(".")), &icoTp);
		if(ico.size()) {
		    answer = pgCreator(ico, "200 OK", "Content-Type: "+TUIS::mimeGet(name_mod,ico,"image/"+icoTp)+";");
		    return mNoFull || KeepAlive;
		}
	    }

	    answer = getIndex(user, sender);
	}
    }

    return mNoFull || KeepAlive;
}

string TProtIn::lang( )
{
    string rez = prmLang;
    if(rez.empty() && user.size())
	try { rez = SYS->security().at().usrAt(user).at().lang(); }
	catch(...) { }
    if(rez.empty())	rez = brLang;

    return rez;
}

string TProtIn::getIndex( const string &user, const string &sender )
{
    string answer = string("<table class='work' width='50%'>\n")+
	"<tr><th>"+_("Login")+"</th></tr>"
	"<tr><td class='content'>";
	// "<p>"+_("Welcome to the Web-interfaces of OpenSCADA system.")+"</p>";
    if(!user.empty())
	answer = answer +
	    "<p style='color: green;'>"+TSYS::strMess(_("You are logged in as \"<b>%s</b>\"."),user.c_str())+"</p>"
	    "<p>"+TSYS::strMess(_("Select the necessary Web-module from the list below, <a href='%s'>logout</a> or <a href='%s'>login as an another user</a>."),("/logout"+prms).c_str(),("/login"+prms).c_str())+"</p>";
    else {
	answer = answer +
	    "<p style='color: #CF8122;'>"+_("You are not logged in the system!")+"</p>"
	    "<p>"+TSYS::strMess(_("To use some modules you must be logged in. <a href='%s'>Login now</a>."),("/login"+prms).c_str())+"</p>";
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
    for(unsigned iL = 0; iL < list.size(); iL++) {
	AutoHD<TModule> mod = owner().owner().owner().ui().at().modAt(list[iL]);
	if(mod.at().modInfo("SubType") == "WWW" && pgAccess(sender+"/"+list[iL]+"/")) {
	    string mIcoTp;
	    TUIS::icoGet("UI."+list[iL], &mIcoTp, true);
	    answer = answer+"<li>"+(mIcoTp.size()?"<img src='/UI."+list[iL]+"."+mIcoTp+"' height='32' width='32'/> ":"")+
		"<a href='/"+list[iL]+"/"+prms+"'><span title='"+mod.at().modInfo("Description:"+lang())+"'>"+mod.at().modInfo("Name:"+lang())+"</span></a></li>\n";
	}
    }

    return pgCreator(answer+"</ul></td></tr></table>\n", "200 OK", "", "", mod->tmplMainPage());
}

string TProtIn::getAuth( const string& uri, const string &mess )
{
    return pgCreator(string("<table class='work'>") +
	"<tr><th>" + _("Login to system") + "</th></tr>\n"
	"<tr><td>\n"
	"<form method='post' action='/login" + uri + prms + "' enctype='multipart/form-data'>\n"
	"<table cellpadding='3px'>\n"
	"<tr><td><b>" + _("User name") + "</b></td><td><input type='text' name='user' size='20'/></td></tr>\n"
	"<tr><td><b>" + _("Password") + "</b></td><td><input type='password' name='pass' size='20'/></td></tr>\n"
	"<tr><td colspan='2' style='text-align: center'><input type='submit' name='auth_enter' value='" + _("Enter") + "'/>&nbsp;"
	"<input type='reset' name='clean' value='" + _("Clean") + "'/></td></tr>"
	"</table>\n</form>\n"
	"</td></tr>"
	"<tr><td>" + mess + "</td></tr>"
	"</table>\n", "200 OK");
}

void TProtIn::getCnt( const vector<string> &vars, const string &content, map<string,string> &cnt )
{
    //Content parse
    string boundary;
    const char *c_bound = "boundary=";
    const char *c_term = "\x0D\x0A";
    const char *c_end = "--";
    const char *c_fd = "Content-Disposition";
    const char *c_name = "name=\"";

    for(size_t i_vr = 0, pos = 0; i_vr < vars.size() && boundary.empty(); i_vr++)
	if(vars[i_vr].compare(0,vars[i_vr].find(":",0),"Content-Type") == 0 && (pos=vars[i_vr].find(c_bound,0)) != string::npos) {
	    pos += strlen(c_bound);
	    boundary = vars[i_vr].substr(pos,vars[i_vr].size()-pos);
	}
    if(boundary.empty()) return;

    for(size_t pos = 0, spos = 0, i_bnd = 0; true; ) {
	pos = content.find(boundary,pos);
	if(pos == string::npos || content.compare(pos+boundary.size(),2,c_end) == 0) break;
	pos += boundary.size()+strlen(c_term);

	// Process properties and get name
	string p_name;
	while(pos < content.size()) {
	    string c_head = content.substr(pos, content.find(c_term,pos)-pos);
	    pos += c_head.size()+strlen(c_term);
	    if(c_head.empty()) break;
	    if((spos=c_head.find(":")) == string::npos) return;
	    if(c_head.compare(0,spos,c_fd) == 0 && (i_bnd=c_head.find(c_name,spos)) != string::npos) {
		i_bnd += strlen(c_name);
		p_name = c_head.substr(i_bnd,c_head.find("\"",i_bnd)-i_bnd);
	    }
	}
	if(pos >= content.size()) return;
	if(!p_name.empty()) cnt[p_name] = content.substr(pos,content.find(string(c_term)+c_end+boundary,pos)-pos);
    }
}
