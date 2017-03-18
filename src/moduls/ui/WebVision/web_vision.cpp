
//OpenSCADA system module UI.WebVision file: web_vision.cpp
/***************************************************************************
 *   Copyright (C) 2007-2017 by Roman Savochenko, <rom_as@oscada.org>      *
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

#include <fcntl.h>
#include <signal.h>
#include <string.h>

#include <tsys.h>
#include <tmess.h>

#include "web_vision.h"

//************************************************
//* Modul info!                                  *
#define MOD_ID		"WebVision"
#define MOD_NAME	_("Operation user interface (WEB)")
#define MOD_TYPE	SUI_ID
#define VER_TYPE	SUI_VER
#define SUB_TYPE	"WWW"
#define MOD_VER		"1.10.0"
#define AUTHORS		_("Roman Savochenko, Lysenko Maxim (2008-2012), Yashina Kseniya (2007)")
#define DESCRIPTION	_("Visual operation user interface, based on WEB - front-end to VCA engine.")
#define LICENSE		"GPL2"
//************************************************

WebVision::TWEB *WebVision::mod;

extern "C"
{
#ifdef MOD_INCL
    TModule::SAt ui_WebVision_module( int n_mod )
#else
    TModule::SAt module( int n_mod )
#endif
    {
	if(n_mod == 0)	return TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE);
	return TModule::SAt("");
    }

#ifdef MOD_INCL
    TModule *ui_WebVision_attach( const TModule::SAt &AtMod, const string &source )
#else
    TModule *attach( const TModule::SAt &AtMod, const string &source )
#endif
    {
	if(AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE)) return new WebVision::TWEB(source);
	return NULL;
    }
}

extern char *WebVisionVCA_js;

using namespace WebVision;

//************************************************
//* TWEB                                         *
//************************************************
TWEB::TWEB( string name ) : TUI(MOD_ID), mTSess(10), mSessLimit(5), mPNGCompLev(1)
{
    mod = this;

    modInfoMainSet(MOD_NAME, MOD_TYPE, MOD_VER, AUTHORS, DESCRIPTION, LICENSE, name);

    id_vcases	= grpAdd("ses_");

    //Reg export functions
    modFuncReg(new ExpFunc("void HTTP_GET(const string&,string&,vector<string>&,const string&,TProtocolIn*);",
	"GET command processing from HTTP protocol!",(void(TModule::*)( )) &TWEB::HTTP_GET));
    modFuncReg(new ExpFunc("void HTTP_POST(const string&,string&,vector<string>&,const string&,TProtocolIn*);",
	"POST command processing from HTTP protocol!",(void(TModule::*)( )) &TWEB::HTTP_POST));

    gdFTUseFontConfig(1);

    //Create named colors' container
    colors["aliceblue"]		= rgb(240, 248, 255);
    colors["antiquewhite"]	= rgb(250, 235, 215);
    colors["aqua"]		= rgb(0, 255, 255);
    colors["aquamarine"]	= rgb(127, 255, 212);
    colors["azure"]		= rgb(240, 255, 255);
    colors["beige"]		= rgb(245, 245, 220);
    colors["bisque"]		= rgb(255, 228, 196);
    colors["black"]		= rgb(0, 0, 0);
    colors["blanchedalmond"]	= rgb(255, 235, 205);
    colors["blue"]		= rgb(0, 0, 255);
    colors["blueviolet"]	= rgb(138, 43, 226);
    colors["brown"]		= rgb(165, 42, 42);
    colors["burlywood"]		= rgb(222, 184, 135);
    colors["cadetblue"]		= rgb(95, 158, 160);
    colors["chartreuse"]	= rgb(127, 255, 0);
    colors["chocolate"]		= rgb(210, 105, 30);
    colors["coral"]		= rgb(255, 127, 80);
    colors["cornflowerblue"]	= rgb(100, 149, 237);
    colors["cornsilk"]		= rgb(255, 248, 220);
    colors["crimson"]		= rgb(220, 20, 60);
    colors["cyan"]		= rgb(0, 255, 255);
    colors["darkblue"]		= rgb(0, 0, 139);
    colors["darkcyan"]		= rgb(0, 139, 139);
    colors["darkgoldenrod"]	= rgb(184, 134, 11);
    colors["darkgray"]		= rgb(169, 169, 169);
    colors["darkgreen"]		= rgb(0, 100, 0);
    colors["darkgrey"]		= rgb(169, 169, 169);
    colors["darkkhaki"]		= rgb(189, 183, 107);
    colors["darkmagenta"]	= rgb(139, 0, 139);
    colors["darkolivegreen"]	= rgb(85, 107, 47);
    colors["darkorange"]	= rgb(255, 140, 0);
    colors["darkorchid"]	= rgb(153, 50, 204);
    colors["darkred"]		= rgb(139, 0, 0);
    colors["darksalmon"]	= rgb(233, 150, 122);
    colors["darkseagreen"]	= rgb(143, 188, 143);
    colors["darkslateblue"]	= rgb(72, 61, 139);
    colors["darkslategray"]	= rgb(47, 79, 79);
    colors["darkslategrey"]	= rgb(47, 79, 79);
    colors["darkturquoise"]	= rgb(0, 206, 209);
    colors["darkviolet"]	= rgb(148, 0, 211);
    colors["deeppink"]		= rgb(255, 20, 147);
    colors["deepskyblue"]	= rgb(0, 191, 255);
    colors["dimgray"]		= rgb(105, 105, 105);
    colors["dimgrey"]		= rgb(105, 105, 105);
    colors["dodgerblue"]	= rgb(30, 144, 255);
    colors["firebrick"]		= rgb(178, 34, 34);
    colors["floralwhite"]	= rgb(255, 250, 240);
    colors["forestgreen"]	= rgb(34, 139, 34);
    colors["fuchsia"]		= rgb(255, 0, 255);
    colors["gainsboro"]		= rgb(220, 220, 220);
    colors["ghostwhite"]	= rgb(248, 248, 255);
    colors["gold"]		= rgb(255, 215, 0);
    colors["goldenrod"]		= rgb(218, 165, 32);
    colors["gray"]		= rgb(128, 128, 128);
    colors["grey"]		= rgb(128, 128, 128);
    colors["green"]		= rgb(0, 128, 0);
    colors["greenyellow"]	= rgb(173, 255, 47);
    colors["honeydew"]		= rgb(240, 255, 240);
    colors["hotpink"]		= rgb(255, 105, 180);
    colors["indianred"]		= rgb(205, 92, 92);
    colors["indigo"]		= rgb(75, 0, 130);
    colors["ivory"]		= rgb(255, 255, 240);
    colors["khaki"]		= rgb(240, 230, 140);
    colors["lavender"]		= rgb(230, 230, 250);
    colors["lavenderblush"]	= rgb(255, 240, 245);
    colors["lawngreen"]		= rgb(124, 252, 0);
    colors["lemonchiffon"]	= rgb(255, 250, 205);
    colors["lightblue"]		= rgb(173, 216, 230);
    colors["lightcoral"]	= rgb(240, 128, 128);
    colors["lightcyan"]		= rgb(224, 255, 255);
    colors["lightgoldenrodyellow"] = rgb(250, 250, 210);
    colors["lightgray"]		= rgb(211, 211, 211);
    colors["lightgreen"]	= rgb(144, 238, 144);
    colors["lightgrey"]		= rgb(211, 211, 211);
    colors["lightpink"]		= rgb(255, 182, 193);
    colors["lightsalmon"]	= rgb(255, 160, 122);
    colors["lightseagreen"]	= rgb(32, 178, 170);
    colors["lightskyblue"]	= rgb(135, 206, 250);
    colors["lightslategray"]	= rgb(119, 136, 153);
    colors["lightslategrey"]	= rgb(119, 136, 153);
    colors["lightsteelblue"]	= rgb(176, 196, 222);
    colors["lightyellow"]	= rgb(255, 255, 224);
    colors["lime"]		= rgb(0, 255, 0);
    colors["limegreen"]		= rgb(50, 205, 50);
    colors["linen"]		= rgb(250, 240, 230);
    colors["magenta"]		= rgb(255, 0, 255);
    colors["maroon"]		= rgb(128, 0, 0);
    colors["mediumaquamarine"]	= rgb(102, 205, 170);
    colors["mediumblue"]	= rgb(0, 0, 205);
    colors["mediumorchid"]	= rgb(186, 85, 211);
    colors["mediumpurple"]	= rgb(147, 112, 219);
    colors["mediumseagreen"]	= rgb(60, 179, 113);
    colors["mediumslateblue"]	= rgb(123, 104, 238);
    colors["mediumspringgreen"]	= rgb(0, 250, 154);
    colors["mediumturquoise"]	= rgb(72, 209, 204);
    colors["mediumvioletred"]	= rgb(199, 21, 133);
    colors["midnightblue"]	= rgb(25, 25, 112);
    colors["mintcream"]		= rgb(245, 255, 250);
    colors["mistyrose"]		= rgb(255, 228, 225);
    colors["moccasin"]		= rgb(255, 228, 181);
    colors["navajowhite"]	= rgb(255, 222, 173);
    colors["navy"]		= rgb(0, 0, 128);
    colors["oldlace"]		= rgb(253, 245, 230);
    colors["olive"]		= rgb(128, 128, 0);
    colors["olivedrab"]		= rgb(107, 142, 35);
    colors["orange"]		= rgb(255, 165, 0);
    colors["orangered"]		= rgb(255, 69, 0);
    colors["orchid"]		= rgb(218, 112, 214);
    colors["palegoldenrod"]	= rgb(238, 232, 170);
    colors["palegreen"]		= rgb(152, 251, 152);
    colors["paleturquoise"]	= rgb(175, 238, 238);
    colors["palevioletred"]	= rgb(219, 112, 147);
    colors["papayawhip"]	= rgb(255, 239, 213);
    colors["peachpuff"]		= rgb(255, 218, 185);
    colors["peru"]		= rgb(205, 133, 63);
    colors["pink"]		= rgb(255, 192, 203);
    colors["plum"]		= rgb(221, 160, 221);
    colors["powderblue"]	= rgb(176, 224, 230);
    colors["purple"]		= rgb(128, 0, 128);
    colors["red"]		= rgb(255, 0, 0);
    colors["rosybrown"]		= rgb(188, 143, 143);
    colors["royalblue"]		= rgb(65, 105, 225);
    colors["saddlebrown"]	= rgb(139, 69, 19);
    colors["salmon"]		= rgb(250, 128, 114);
    colors["sandybrown"]	= rgb(244, 164, 96);
    colors["seagreen"]		= rgb(46, 139, 87);
    colors["seashell"]		= rgb(255, 245, 238);
    colors["sienna"]		= rgb(160, 82, 45);
    colors["silver"]		= rgb(192, 192, 192);
    colors["skyblue"]		= rgb(135, 206, 235);
    colors["slateblue"]		= rgb(106, 90, 205);
    colors["slategray"]		= rgb(112, 128, 144);
    colors["slategrey"]		= rgb(112, 128, 144);
    colors["snow"]		= rgb(255, 250, 250);
    colors["springgreen"]	= rgb(0, 255, 127);
    colors["steelblue"]		= rgb(70, 130, 180);
    colors["tan"]		= rgb(210, 180, 140);
    colors["teal"]		= rgb(0, 128, 128);
    colors["thistle"]		= rgb(216, 191, 216);
    colors["tomato"]		= rgb(255, 99, 71);
    colors["turquoise"]		= rgb(64, 224, 208);
    colors["violet"]		= rgb(238, 130, 238);
    colors["wheat"]		= rgb(245, 222, 179);
    colors["white"]		= rgb(255, 255, 255);
    colors["whitesmoke"]	= rgb(245, 245, 245);
    colors["yellow"]		= rgb(255, 255, 0);
    colors["yellowgreen"]	= rgb(154, 205, 50);

#if 0
    char mess[][100] = { _("Date and time"), _("Level"), _("Category"), _("Message"), _("mcsec"), _("Ready"),
	_("Today"), _("Mon"), _("Tue"), _("Wed"), _("Thr"), _("Fri"), _("Sat"), _("Sun"),
	_("January"), _("February"), _("March"), _("April"), _("May"), _("June"), _("July"),
	_("August"), _("September"), _("October"), _("November"), _("December") };
#endif
}

TWEB::~TWEB( )
{

}

string TWEB::modInfo( const string &name )
{
    if(name == "SubType")	return SUB_TYPE;
    if(name == "Auth")		return "1";
    return TModule::modInfo(name);
}

void TWEB::modInfo( vector<string> &list )
{
    TModule::modInfo(list);
    list.push_back("SubType");
    list.push_back("Auth");
}

void TWEB::vcaSesAdd( const string &name, bool isCreate )
{
    if(vcaSesPresent(name))	return;
    chldAdd(id_vcases, new VCASess(name,isCreate));
}

string TWEB::optDescr( )
{
    char buf[STR_BUF_LEN];

    snprintf(buf,sizeof(buf),_(
	"======================= The module <%s:%s> options =======================\n"
	"---------- Parameters of the module section '%s' in config-file ----------\n"
	"SessTimeLife <time>      Time of the session life, minutes (default 10).\n\n"),
	MOD_TYPE,MOD_ID,nodePath().c_str());

    return buf;
}

void TWEB::load_( )
{
    //Load parameters from command line
    string argCom, argVl;
    for(int argPos = 0; (argCom=SYS->getCmdOpt(argPos,&argVl)).size(); )
	if(argCom == "h" || argCom == "help")	fprintf(stdout,"%s",optDescr().c_str());

    //Load parameters from config-file
    setSessTime(s2i(TBDS::genDBGet(nodePath()+"SessTimeLife",i2s(sessTime()))));
    setSessLimit(s2i(TBDS::genDBGet(nodePath()+"SessLimit",i2s(sessLimit()))));
    setPNGCompLev(s2i(TBDS::genDBGet(nodePath()+"PNGCompLev",i2s(PNGCompLev()))));
}

void TWEB::save_( )
{
    TBDS::genDBSet(nodePath()+"SessTimeLife",i2s(sessTime()));
    TBDS::genDBSet(nodePath()+"SessLimit",i2s(sessLimit()));
    TBDS::genDBSet(nodePath()+"PNGCompLev",i2s(PNGCompLev()));
}

void TWEB::modStart( )	{ runSt = true; }

void TWEB::modStop( )	{ runSt = false; }

void TWEB::perSYSCall( unsigned int cnt )
{
    try {
	//Check for opened sessions timeout close
	time_t cur_tm = time(NULL);

	vector<string> list;
	vcaSesList(list);
	for(unsigned i_s = 0; i_s < list.size(); i_s++)
	    if(cur_tm > vcaSesAt(list[i_s]).at().lstReq()+sessTime()*60)
		vcaSesDel(list[i_s]);
    } catch(TError &err) { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
}

string TWEB::pgCreator( TProtocolIn *iprt, const string &cnt, const string &rcode, const string &httpattrs,
    const string &htmlHeadEls, const string &forceTmplFile )
{
    if(httpattrs.size() && httpattrs.find("Content-Type: text/html") == string::npos)
	return "HTTP/1.0 " + rcode + "\x0D\x0A"
	    "Server: " + PACKAGE_STRING + "\x0D\x0A"
	    "Accept-Ranges: bytes\x0D\x0A"
	    "Content-Length: " + i2s(cnt.size()) + "\x0D\x0A" +
//	    "Connection: close\x0D\x0A" +
	    httpattrs + "\x0D\x0A\x0D\x0A" + cnt;

    vector<TVariant> prms;
    prms.push_back(cnt); prms.push_back(rcode); prms.push_back(httpattrs); prms.push_back(htmlHeadEls); prms.push_back(forceTmplFile);

    return iprt->owner().objFuncCall("pgCreator", prms, "root").getS();
}

void TWEB::HTTP_GET( const string &url, string &page, vector<string> &vars, const string &user, TProtocolIn *iprt )
{
    string sender = TSYS::strLine(iprt->srcAddr(), 0);
    SSess ses(TSYS::strDecode(url,TSYS::HttpURL), sender, user, vars, "", iprt);

    try {
	string zero_lev = TSYS::pathLev(ses.url, 0);
	//Get the icon and the global image of the module
	if(zero_lev == "ico" || zero_lev.compare(0,4,"img_") == 0) {
	    string itp = "png";
	    //Session's and project's icons request processing
	    map<string,string>::iterator prmEl = ses.prm.find("it");
	    if(prmEl != ses.prm.end()) {
		XMLNode req("get");
		req.setAttr("path", prmEl->second+"/%2fico");
		mod->cntrIfCmd(req, ses.user);
		page = TSYS::strDecode(req.text(), TSYS::base64);
	    }
	    else page = TUIS::icoGet(zero_lev=="ico"?"UI." MOD_ID:zero_lev.substr(4), &itp);
	    page = pgCreator(iprt, page, "200 OK", "Content-Type: image/"+itp+";");
	}
	//Check for main JavaScript code
	else if(zero_lev == "script.js") {
	    int hd; page = "";
	    if((hd=open("WebVisionVCA.js",O_RDONLY)) >= 0) {
		char buf[STR_BUF_LEN];
		for(int len = 0; (len=read(hd,buf,sizeof(buf))) > 0; ) page.append(buf, len);
		close(hd);
	    }
	    else page = WebVisionVCA_js;
	    page = trMessReplace(page);
	    page = mod->pgCreator(iprt, page, "200 OK", "Content-Type: text/javascript;");
	}
	else {
	    //Session selection or a new session for the project creation
	    if(zero_lev.empty()) {
		bool sesPrjOk = false;
		page = "<table class='work'>\n";
		// Get present sessions list
		string self_prjSess, prjSesEls = "";
		XMLNode req("get");
		req.setAttr("path","/%2fses%2fses")->setAttr("chkUserPerm","1");
		cntrIfCmd(req, ses.user);
		ResAlloc sesRes(mSesRes, false);
		for(unsigned iCh = 0; iCh < req.childSize(); iCh++) {
		    if(!SYS->security().at().access(user,SEC_WR,"root","root",RWRWR_) &&
			    (req.childGet(iCh)->attr("user") != user ||
			    (vcaSesPresent(req.childGet(iCh)->text()) && vcaSesAt(req.childGet(iCh)->text()).at().sender() != sender)))
			continue;
		    prjSesEls += "<tr><td><img src='/" MOD_ID "/ico?it=/ses_" + req.childGet(iCh)->text() + "' height='32' width='32'/> "
			"<a href='/" MOD_ID "/ses_" + req.childGet(iCh)->text() + "/'>" + req.childGet(iCh)->text()+"</a>";
		    if(req.childGet(iCh)->attr("user") != user) prjSesEls += " - "+req.childGet(iCh)->attr("user");
		    if(vcaSesPresent(req.childGet(iCh)->text()) && vcaSesAt(req.childGet(iCh)->text()).at().sender() != sender)
			prjSesEls += " - "+vcaSesAt(req.childGet(iCh)->text()).at().sender();
		    prjSesEls += "</td></tr>";
		    self_prjSess += req.childGet(iCh)->attr("proj")+";";
		}
		if(!prjSesEls.empty()) {
		    page = page +
			"<tr><th>" + _("Connect to opened session") + "</th></tr>\n"
			"<tr><td class='content'>\n"
			"<table border='0' cellspacing='3px' width='100%'>\n" +
			prjSesEls +
			"</table></td></tr>\n";
		    sesPrjOk = true;
		}
		// Get present projects list
		prjSesEls = "";
		req.clear()->setAttr("path","/%2fprm%2fcfg%2fprj")->setAttr("chkUserPerm","1");
		cntrIfCmd(req,ses.user);
		for(unsigned iCh = 0; iCh < req.childSize(); iCh++) {
		    if(!SYS->security().at().access(user,SEC_WR,"root","root",RWRWR_) && self_prjSess.find(req.childGet(iCh)->attr("id")+";") != string::npos)
			continue;
		    prjSesEls += "<tr><td><img src='/" MOD_ID "/ico?it=/prj_" + req.childGet(iCh)->attr("id") + "' height='32' width='32'/> "
			"<a href='/" MOD_ID "/prj_" + req.childGet(iCh)->attr("id") + "/'>" + req.childGet(iCh)->text() + "</a></td></tr>";
		}
		if(!prjSesEls.empty()) {
		    page = page +
			"<tr><th>"+_("Create new session for present project")+"</th></tr>\n"
			"<tr><td class='content'>\n"
			"<table border='0' cellspacing='3px' width='100%'>\n"+
			prjSesEls+
			"</table></td></tr>\n";
		    sesPrjOk = true;
		}
		page += "</table>";

		if(!sesPrjOk) page = messPost(nodePath(), _("No one sessions and projects of VCA engine are present for user!"), TWEB::Warning);
		page = pgCreator(iprt, page, "200 OK");
	    }
	    //New session create
	    else if(zero_lev.compare(0,4,"prj_") == 0) {
		string sName;
		// Find for early created session for the user and the sender
		XMLNode req("get");
		req.setAttr("path","/%2fses%2fses")->setAttr("chkUserPerm","1");
		cntrIfCmd(req, ses.user);
		ResAlloc sesRes(mSesRes, false);
		if(!SYS->security().at().access(user,SEC_WR,"root","root",RWRWR_))
		    for(unsigned iCh = 0; iCh < req.childSize(); iCh++)
			if(req.childGet(iCh)->attr("user") == user && req.childGet(iCh)->attr("proj") == zero_lev.substr(4) &&
			    vcaSesPresent(req.childGet(iCh)->text()) && vcaSesAt(req.childGet(iCh)->text()).at().sender() == sender)
			{ sName = req.childGet(iCh)->text(); break; }
		if(sName.empty()) {
		    vector<string> vcaLs;
		    vcaSesList(vcaLs);
		    if((int)vcaLs.size() >= mod->sessLimit())
			page = messPost(nodePath(), _("Sorry, opened sessions number reach limit!"), TWEB::Warning);
		    else {
			sesRes.request(true);
			req.setName("connect")->setAttr("path","/%2fserv%2fsess")->setAttr("prj",zero_lev.substr(4));
			if(cntrIfCmd(req,ses.user))
			    page = messPost(req.attr("mcat").c_str(), req.text().c_str(), TWEB::Error);
			else {
			    sName = req.attr("sess");
			    vcaSesAdd(sName, true);
			    vcaSesAt(sName).at().senderSet(sender);
			}
		    }
		}
		if(!sName.empty())
		    page = pgCreator(iprt, TSYS::strMess(_("Go to session '%s' for project '%s' ..."),sName.c_str(),zero_lev.substr(4).c_str()),
			"200 OK", "", "<META HTTP-EQUIV='Refresh' CONTENT='0; URL=/" MOD_ID "/ses_" + sName + "/'/>");
		else page = pgCreator(iprt, page, "200 OK");
	    }
	    //Main session page data prepare
	    else if(zero_lev.compare(0,4,"ses_") == 0) {
		ses.url = Mess->codeConvIn("UTF-8", ses.url);	//Internal data into UTF-8
		string sesnm = zero_lev.substr(4);
		// Check for session present
		if(!ses.prm.size()) {
		    XMLNode req("get"); req.setAttr("path",ses.url+"/%2fobj%2fst%2fen");
		    if(cntrIfCmd(req,ses.user) || !s2i(req.text()))	{ HTTP_GET("", page, vars, user, iprt); return; }
		}
		// Call to session
		ResAlloc sesRes(mSesRes, false);
		try { vcaSesAt(sesnm).at().getReq(ses); }
		catch(...) {
		    if(!vcaSesPresent(sesnm)) {
			sesRes.request(true);
			vcaSesAdd(sesnm, false);
			vcaSesAt(sesnm).at().senderSet(sender);
			vcaSesAt(sesnm).at().getReq(ses);
		    } else throw;
		}
		page = ses.page;
	    }
	    else {
		page = pgCreator(iprt, "<div class='error'>"+TSYS::strMess(_("Pointed project/session '%s' is wrong!"),zero_lev.c_str())+"</div>\n",
				       "404 Not Found");
	    }
	}
    } catch(TError &err) {
	page = pgCreator(iprt, "<div class='error'>"+TSYS::strMess(_("Page '%s' error: %s"),ses.url.c_str(),err.mess.c_str())+"</div>\n",
			       "404 Not Found");
    }
}

void TWEB::HTTP_POST( const string &url, string &page, vector<string> &vars, const string &user, TProtocolIn *iprt )
{
    map<string,string>::iterator cntEl;
    SSess ses(TSYS::strDecode(url,TSYS::HttpURL), TSYS::strLine(iprt->srcAddr(),0), user, vars, page, iprt);

    try {
	ses.url = Mess->codeConvIn("UTF-8", ses.url);	//Internal data into UTF-8
	//To control interface request
	if((cntEl=ses.prm.find("com"))!=ses.prm.end() && cntEl->second == "com") {
	    XMLNode req(""); req.load(ses.content); req.setAttr("path", ses.url);
	    cntrIfCmd(req, ses.user, false);
	    page = pgCreator(iprt, req.save(), "200 OK", "Content-Type: text/xml;charset=UTF-8");
	    return;
	}

	//Post command to the session
	string sesnm = TSYS::pathLev(ses.url, 0);
	if(sesnm.size() <= 4 || sesnm.compare(0,4,"ses_") != 0)
	    throw TError(nodePath().c_str(), "%s", TSYS::strMess(_("Wrong session '%s'."), sesnm.c_str()).c_str());
	else {
	    ResAlloc sesRes(mSesRes, false);
	    vcaSesAt(sesnm.substr(4)).at().postReq(ses);
	    page = ses.page;
	}
    } catch(TError &err) {
	page = pgCreator(iprt, "<div class='error'>"+TSYS::strMess(_("Page '%s' error: %s"),url.c_str(),err.mess.c_str())+"</div>\n",
	    "404 Not Found");
    }
}

string TWEB::messPost( const string &cat, const string &mess, MessLev type )
{
    string page;

    //Put system message
    message(cat.c_str(), (type==Error) ? TMess::Error : (type==Warning) ? TMess::Warning : TMess::Info,"%s",mess.c_str());

    //Prepare HTML messages
    page += "<table border='2' width='40%' align='center'><tbody>\n";
    if(type == Warning )	page += "<tr bgcolor='yellow'><td align='center'><b>Warning!</b></td></tr>\n";
    else if(type == Error )	page += "<tr bgcolor='red'><td align='center'><b>Error!</b></td></tr>\n";
    else page += "<tr bgcolor='#9999ff'><td align='center'><b>Message!</b></td></tr>\n";
    page += "<tr bgcolor='#cccccc'> <td align='center'>"+TSYS::strEncode(mess,TSYS::Html)+"</td></tr>\n";
    page += "</tbody></table>\n";

    return page;
}

int TWEB::cntrIfCmd( XMLNode &node, const string &user, bool VCA )
{
    node.setAttr("user", user);
    if(VCA)	node.setAttr("path","/UI/VCAEngine"+node.attr("path"));
    SYS->cntrCmd(&node);
    return s2i(node.attr("rez"));
}

void TWEB::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TUI::cntrCmdProc(opt);
	if(ctrMkNode("area",opt,1,"/prm/cfg",_("Module options"),R_R_R_)) {
	    ctrMkNode("fld", opt, -1, "/prm/cfg/lf_tm", _("Life time of session (min)"), RWRWR_, "root", SUI_ID, 1, "tp","dec");
	    ctrMkNode("fld", opt, -1, "/prm/cfg/sesLimit", _("Sessions limit"), RWRWR_, "root", SUI_ID, 1, "tp","dec");
	    ctrMkNode("fld", opt, -1, "/prm/cfg/PNGCompLev", _("PNG compression level"), RWRWR_, "root", SUI_ID, 4,
		"tp","dec", "min","-1", "max","9", "help",_("PNG (ZLib) compression level:\n"
			    "  -1  - optimal speed-size;\n"
			    "  0   - disable;\n"
			    "  1-9 - direct level."));
	}
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/cfg/lf_tm") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(i2s(sessTime()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setSessTime(s2i(opt->text()));
    }
    else if(a_path == "/prm/cfg/sesLimit") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(i2s(sessLimit()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setSessLimit(s2i(opt->text()));
    }
    else if(a_path == "/prm/cfg/PNGCompLev") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(i2s(PNGCompLev()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setPNGCompLev(s2i(opt->text()));
    }
    else TUI::cntrCmdProc(opt);
}

void TWEB::imgConvert( SSess &ses )
{
    map<string,string>::iterator prmEl;
    gdImagePtr sim = NULL;
    string itp;
    int newImgH = 0, newImgW = 0;

    if(ses.page.empty() || (ses.prm.find("size") == ses.prm.end() && ses.prm.find("filtr") == ses.prm.end()))   return;

    if((sim=gdImageCreateFromPngPtr(ses.page.size(),(char*)ses.page.data())))		itp = "png";
    else if((sim=gdImageCreateFromJpegPtr(ses.page.size(),(char*)ses.page.data())))	itp = "jpg";
    else if((sim=gdImageCreateFromGifPtr(ses.page.size(),(char*)ses.page.data())))	itp = "gif";
    //if(sim) gdImageAlphaBlending(sim, 0);

    //Check for resize icon
    if(sim && (prmEl=ses.prm.find("size")) != ses.prm.end() && (newImgH=s2i(prmEl->second)) > 0 && gdImageSY(sim) > newImgH) {
	newImgW = gdImageSX(sim)*newImgH/gdImageSY(sim);
	gdImagePtr dim = gdImageCreateTrueColor(newImgW,newImgH);
	gdImageAlphaBlending(dim,0);
	gdImageFilledRectangle(dim,0,0,newImgW-1,newImgH-1,gdImageColorResolveAlpha(dim,0,0,0,127));
	gdImageCopyResampled(dim,sim,0,0,0,0,newImgW,newImgH,gdImageSX(sim),gdImageSY(sim));
	gdImageDestroy(sim);
	sim = dim;
    }

    // Check for disable icon make
    if(sim && (prmEl = ses.prm.find("filtr")) != ses.prm.end() && (prmEl->second == "gray" || prmEl->second == "unact")) {
	gdImagePtr dim = gdImageCreateTrueColor(gdImageSX(sim),gdImageSY(sim));
	gdImageAlphaBlending(dim,0);
	bool isUnAct = (prmEl->second == "unact");
	for(int i_y = 0; i_y < gdImageSY(sim); i_y++)
	    for(int i_x = 0; i_x < gdImageSX(sim); i_x++) {
		int c = gdImageGetPixel(sim,i_x,i_y);
		int y = (int)(0.3*gdImageRed(sim,c)+0.59*gdImageGreen(sim,c)+0.11*gdImageBlue(sim,c));
		if(isUnAct) y = 255-(255-y)/2;
		c = (int)gdImageColorResolveAlpha(dim,y,y,y,gdImageAlpha(sim,c));
		gdImageSetPixel(dim,i_x,i_y,c);
	    }
	gdImageDestroy(sim);
	sim = dim;
    }

    // Save result
    if(sim) {
	int img_sz;
	char *img_ptr = NULL;
	gdImageSaveAlpha(sim, 1);
	if(itp == "png")	img_ptr = (char *)gdImagePngPtrEx(sim, &img_sz, PNGCompLev());
	else if(itp == "jpg")	img_ptr = (char *)gdImageJpegPtr(sim, &img_sz,-1);
	else if(itp == "gif")	img_ptr = (char *)gdImageGifPtr(sim, &img_sz);
	if(img_ptr) {
	    ses.page.assign(img_ptr,img_sz);
	    gdFree(img_ptr);
	}
	gdImageDestroy(sim);
    }
}

int TWEB::colorParse( const string &tclr )
{
    string clr = tclr;
    int alpha;
    size_t found = clr.find("-");
    if(found != string::npos) {
	clr = tclr.substr(0,found);
	alpha =  s2i(tclr.substr(found+1));
    } else alpha = 255;

    if(clr.size() >= 4 && clr[0] == '#') {
	int el_sz = clr.size()/3;
	return ((int)vmin(127,(alpha/2+0.5))<<24)+
		(strtol(clr.substr(1,el_sz).c_str(),NULL,16)<<16)+
		(strtol(clr.substr(1+el_sz,el_sz).c_str(),NULL,16)<<8)+
		strtol(clr.substr(1+2*el_sz,el_sz).c_str(),NULL,16);
    }
    else if(clr.size()) {
	map<string,int>::iterator iclr = colors.find(clr);
	if(iclr != colors.end()) return ((int)vmin(127,(alpha/2+0.5))<<24)+ iclr->second;
    }

    return -1;
}

int TWEB::colorResolve( gdImagePtr im, int clr )
{
    return gdImageColorResolveAlpha(im, (uint8_t)(clr>>16), (uint8_t)(clr>>8), (uint8_t)clr, 127-(uint8_t)(clr>>24));
}

string TWEB::trMessReplace( const string &tsrc )
{
    string trez; trez.reserve(tsrc.size());

    unsigned txtBeg = 0, i_s, i_r;
    for(i_s = 0; i_s < tsrc.size(); i_s++)
	if(tsrc[i_s] == '#' && tsrc.substr(i_s,3) == "###" && (i_s+3)<tsrc.size() && tsrc[i_s+3] != '#') {
	    for(i_r = i_s+3; i_r < tsrc.size(); i_r++)
	    if((tsrc[i_r] == '#' && tsrc.substr(i_r,3) == "###" && ((i_r+3)>=tsrc.size() || tsrc[i_r+3] != '#')) || tsrc[i_r] == '\n')
		break;
	    if(i_r < tsrc.size() && tsrc[i_r] != '\n') {
		trez.append(tsrc.substr(txtBeg,i_s-txtBeg));
		trez.append(_(tsrc.substr(i_s+3,i_r-i_s-3).c_str()));
		i_s = i_r+2;
		txtBeg = i_r+3;
		continue;
	    }
	}
    if(txtBeg < i_s) trez.append(tsrc.substr(txtBeg,i_s-txtBeg));

    return trez;
}

//*************************************************
//* SSess                                         *
//*************************************************
SSess::SSess( const string &iurl, const string &isender, const string &iuser, vector<string> &ivars,
	const string &icontent, TProtocolIn *iprt ) :
    url(iurl), sender(isender), user(iuser), content(icontent), vars(ivars), prt(iprt)
{
    //URL parameters parse
    size_t prmSep = iurl.find("?");
    if(prmSep != string::npos) {
	url = iurl.substr(0,prmSep);
	string prms = iurl.substr(prmSep+1);
	string sprm;
	for(int iprm = 0; (sprm=TSYS::strSepParse(prms,0,'&',&iprm)).size(); )
	    if((prmSep=sprm.find("=")) == string::npos) prm[sprm] = "true";
	    else prm[sprm.substr(0,prmSep)] = sprm.substr(prmSep+1);
    }

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
