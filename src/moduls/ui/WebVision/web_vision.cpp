
//OpenSCADA system module UI.WebVision file: web_vision.cpp
/***************************************************************************
 *   Copyright (C) 2007-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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
#define MOD_VER		"1.0.1"
#define AUTHORS		_("Roman Savochenko")
#define DEVELOPERS	_("Roman Savochenko, Lysenko Maxim, Yashina Kseniya")
#define DESCRIPTION	_("Web operation user interface for visual control area (VCA) projects playing.")
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

using namespace WebVision;

//************************************************
//* TWEB                                         *
//************************************************
TWEB::TWEB( string name ) : TUI(MOD_ID), mTSess(10), mSessLimit(5), mPNGCompLev(1)
{
    mod		= this;

    mName	= MOD_NAME;
    mType	= MOD_TYPE;
    mVers	= MOD_VER;
    mAuthor	= AUTHORS;
    mDescr	= DESCRIPTION;
    mLicense	= LICENSE;
    mSource	= name;

    id_vcases	= grpAdd("ses_");

    //Reg export functions
    modFuncReg(new ExpFunc("void HttpGet(const string&,string&,const string&,vector<string>&,const string&);",
	"Process Get comand from http protocol's!",(void(TModule::*)( )) &TWEB::HttpGet));
    modFuncReg(new ExpFunc("void HttpPost(const string&,string&,const string&,vector<string>&,const string&);",
	"Process Set comand from http protocol's!",(void(TModule::*)( )) &TWEB::HttpPost));

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

    //Default CSS init
    mCSStables =
	"hr { width: 95%; }\n"
	"body { background-color: #B0B0B0; margin: 0px; }\n"
	"h1.head { text-align:center; color:#ffff00; }\n"
	"h2.title { text-align:center; font-style: italic; margin: 0px; padding: 0px; border-width:0; }\n"
	"table.work { background-color: #9999ff; border: 3px ridge #a9a9a9; padding: 2px; }\n"
	"table.work td { background-color:#cccccc; text-align: left; }\n"
	"table.work td.content { padding: 5px; padding-bottom: 20px; }\n"
	"table.work ul { margin: 0px; padding: 0px; padding-left: 20px; }\n"
	".vertalign { display: table-cell; text-align: center; vertical-align: middle; }\n"
	".vertalign * { vertical-align: middle; }\n"
	"table.prot { border: 1px solid black; border-collapse: collapse; empty-cells: show; }\n"
	"table.prot th { border: 1px solid black; background-color: #E6E6E6; text-align: center; white-space: nowrap; }\n"
	"table.prot td { border: 1px solid black; white-space: nowrap; }\n"
	"table.prot td.hd { background-color: #E6E6E6; font-weight: bold; text-align: center; }\n"
	"#popupmenu { position: absolute; border: 0; width: 150px; height: 150px; overflow : auto; z-index: 1000; }\n"
	"#popupmenu select { background-color: #E6E6E6; border: 1px solid black; padding: 1px; }\n"
	"input[type=\"checkbox\"] { margin: 0 4px 0 0; float: left; }\n";

#if 0
    char mess[][100] = { _("Date and time"), _("Level"), _("Category"), _("Message"), _("mcsec"), _("Ready") };
#endif
}

TWEB::~TWEB( )
{

}

string TWEB::modInfo( const string &name )
{
    if(name == "SubType")		return SUB_TYPE;
    else if(name == "Auth")		return "1";
    else if(name == _("Developers"))	return DEVELOPERS;
    else return TModule::modInfo(name);
}

void TWEB::modInfo( vector<string> &list )
{
    TModule::modInfo(list);
    list.push_back("SubType");
    list.push_back("Auth");
    list.push_back(_("Developers"));
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

void TWEB::modStart( )	{ run_st = true; }

void TWEB::modStop( )	{ run_st = false; }

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
    }
    catch(TError err){ mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
}

string TWEB::httpHead( const string &rcode, int cln, const string &cnt_tp, const string &addattr, const string &charset )
{
    return "HTTP/1.0 "+rcode+"\x0D\x0A"
	"Server: "+PACKAGE_STRING+"\x0D\x0A"
	"Accept-Ranges: bytes\x0D\x0A"
	"Content-Length: "+i2s(cln)+"\x0D\x0A"
	"Connection: close\x0D\x0A"
	"Content-Type: "+cnt_tp+"; charset="+charset+"\x0D\x0A"+addattr+"\x0D\x0A";
}

string TWEB::pgHead( const string &head_els, const string &title, const string &charset )
{
    string shead =
	"<?xml version='1.0' ?>\n"
	"<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Transitional//EN'\n"
	"'DTD/xhtml1-transitional.dtd'>\n"
	"<html xmlns='http://www.w3.org/1999/xhtml'>\n"
	"<head>\n"
	"  <meta http-equiv='Content-Type' content='text/html; charset="+charset+"'/>\n"
	"  <meta http-equiv='Cache-Control' content='no-store, no-cache, must-revalidate'/>\n"
	"  <meta http-equiv='Cache-Control' content='post-check=0, pre-check=0'/>\n"
	"  <meta http-equiv='Content-Script-Type' content='text/javascript'/>\n"
	"  <link rel='shortcut icon' href='/"MOD_ID"/ico' type='image' />\n"
	"  <title>"+(title.empty()?(string(PACKAGE_NAME)+". "+_(MOD_NAME)):title)+"</title>\n"
	"  <style type='text/css'>\n"+mCSStables+"</style>\n"+
	head_els+
	"</head>\n"
	"<body alink='#33ccff' link='#3366ff' text='#000000' vlink='#339999'>\n";

    return shead;
}

string TWEB::pgTail( )	{ return "</body>\n</html>"; }

void TWEB::HttpGet( const string &url, string &page, const string &sender, vector<string> &vars, const string &user )
{
    SSess ses(TSYS::strDecode(url,TSYS::HttpURL),sender,user,vars,"");
    ses.page = pgHead();

    try {
	string zero_lev = TSYS::pathLev(ses.url,0);
	//Get about module page
	if(zero_lev == "about")	getAbout(ses);
	//Get module icon and global image
	else if(zero_lev == "ico" || zero_lev.compare(0,4,"img_") == 0) {
	    string itp;
	    ses.page = TUIS::icoGet(zero_lev=="ico"?"UI."MOD_ID:zero_lev.substr(4), &itp);
	    page = httpHead("200 OK",ses.page.size(),string("image/")+itp)+ses.page;
	    return;
	}
	else {
	    //Session select or new session for project creation
	    if(zero_lev.empty()) {
		bool sesPrjOk = false;
		ses.page = ses.page+
		    "<h1 class='head'>"+PACKAGE_NAME+". "+_(MOD_NAME)+"</h1>\n<hr/><br/>\n"
		    "<center><table class='work'>\n";
		// Get present sessions list
		string self_prjSess, prjSesEls = "";
		XMLNode req("get");
		req.setAttr("path","/%2fses%2fses")->setAttr("chkUserPerm","1");
		cntrIfCmd(req,ses.user);
		ResAlloc sesRes(nodeRes(),false);
		for(unsigned i_ch = 0; i_ch < req.childSize(); i_ch++) {
		    if(!SYS->security().at().access(user,SEC_WR,"root","root",RWRWR_) &&
			    (req.childGet(i_ch)->attr("user") != user ||
			    (vcaSesPresent(req.childGet(i_ch)->text()) && vcaSesAt(req.childGet(i_ch)->text()).at().sender() != sender)))
			continue;
		    prjSesEls += "<tr><td style='text-align: center;'><a href='/"MOD_ID"/ses_"+req.childGet(i_ch)->text()+"/'>"+
			req.childGet(i_ch)->text()+"</a>";
		    if(req.childGet(i_ch)->attr("user") != user) prjSesEls += " - "+req.childGet(i_ch)->attr("user");
		    if(vcaSesPresent(req.childGet(i_ch)->text()) && vcaSesAt(req.childGet(i_ch)->text()).at().sender() != sender)
			prjSesEls += " - "+vcaSesAt(req.childGet(i_ch)->text()).at().sender();
		    prjSesEls += "</td></tr>";
		    self_prjSess += req.childGet(i_ch)->attr("proj")+";";
		}
		if(!prjSesEls.empty()) {
		    ses.page = ses.page+
			"<tr><th>"+_("Connect to opened session")+"</th></tr>\n"
			"<tr><td class='content'>\n"
			"<table border='0' cellspacing='3px' width='100%'>\n"+
			prjSesEls+
			"</table></td></tr>\n";
		    sesPrjOk = true;
		}
		// Get present projects list
		prjSesEls = "";
		req.clear()->setAttr("path","/%2fprm%2fcfg%2fprj")->setAttr("chkUserPerm","1");
		cntrIfCmd(req,ses.user);
		for(unsigned i_ch = 0; i_ch < req.childSize(); i_ch++) {
		    if(!SYS->security().at().access(user,SEC_WR,"root","root",RWRWR_) && self_prjSess.find(req.childGet(i_ch)->attr("id")+";") != string::npos)
			continue;
		    prjSesEls += "<tr><td style='text-align: center;'><a href='/"MOD_ID"/prj_"+req.childGet(i_ch)->attr("id")+"/'>"+
			req.childGet(i_ch)->text()+"</a></td></tr>";
		}
		if(!prjSesEls.empty()) {
		    ses.page = ses.page +
			"<tr><th>"+_("Create new session for present project")+"</th></tr>\n"
			"<tr><td class='content'>\n"
			"<table border='0' cellspacing='3px' width='100%'>\n"+
			prjSesEls+
			"</table></td></tr>\n";
		    sesPrjOk = true;
		}
		ses.page += "</table></center>";

		if(!sesPrjOk) messPost(ses.page,nodePath(),_("No one sessions and projects of VCA engine are present for user!"),TWEB::Warning);
	    }
	    //New session create
	    else if(zero_lev.compare(0,4,"prj_") == 0) {
		string sName;
		// Find for early created session for user and sender
		XMLNode req("get");
		req.setAttr("path","/%2fses%2fses")->setAttr("chkUserPerm","1");
		cntrIfCmd(req,ses.user);
		ResAlloc sesRes(nodeRes(),false);
		for(unsigned i_ch = 0; i_ch < req.childSize(); i_ch++)
		    if(req.childGet(i_ch)->attr("user") == user && req.childGet(i_ch)->attr("proj") == zero_lev.substr(4) &&
			    vcaSesPresent(req.childGet(i_ch)->text()) && vcaSesAt(req.childGet(i_ch)->text()).at().sender() == sender)
		    { sName = req.childGet(i_ch)->text(); break; }
		if(sName.empty()) {
		    vector<string> vcaLs;
		    vcaSesList(vcaLs);
		    if((int)vcaLs.size() >= mod->sessLimit())
		        messPost(ses.page,nodePath(),_("Sorry, opened sessions number reach limit!"),TWEB::Warning);
		    else {
			sesRes.request(true);
			req.setName("connect")->setAttr("path","/%2fserv%2fsess")->setAttr("prj",zero_lev.substr(4));
			if(cntrIfCmd(req,ses.user))
			    messPost(ses.page,req.attr("mcat").c_str(),req.text().c_str(),TWEB::Error);
			else {
			    sName = req.attr("sess");
			    vcaSesAdd(sName,true);
			    vcaSesAt(sName).at().senderSet(sender);
			}
		    }
		}
		if(!sName.empty())
		    ses.page = pgHead("<META HTTP-EQUIV='Refresh' CONTENT='0; URL=/"MOD_ID"/ses_"+sName+"/'/>")+
			"<center>Go to session '"+sName+"' for project: '"+zero_lev.substr(4)+"'</center>\n<br/>";
	    }
	    //Main session page data prepare
	    else if(zero_lev.compare(0,4,"ses_") == 0) {
		ses.url = Mess->codeConvIn("UTF-8", ses.url);	//Internal data into UTF-8
		string sesnm = zero_lev.substr(4);
		// Check for session present
		if(!ses.prm.size()) {
		    XMLNode req("get"); req.setAttr("path",ses.url+"/%2fobj%2fst%2fen");
		    if(cntrIfCmd(req,ses.user) || !s2i(req.text()))	{ HttpGet("", page, sender, vars, user); return; }
		}
		// Call to session
		ResAlloc sesRes(nodeRes(),false);
		try { vcaSesAt(sesnm).at().getReq(ses); }
		catch(...) {
		    if(!vcaSesPresent(sesnm)) {
			sesRes.request(true);
			vcaSesAdd(sesnm,false);
			vcaSesAt(sesnm).at().senderSet(sender);
			vcaSesAt(sesnm).at().getReq(ses);
		    }
		    else throw;
		}
		page = ses.page;
		return;
	    }
	    else mess_err(nodePath().c_str(),_("An inaccessible request is received: '%s'"),zero_lev.c_str());
	}
    }
    catch(TError err) {
	ses.page = "Page <" + ses.url + "> error: " + err.mess;
	page = httpHead("404 Not Found",ses.page.size()) + ses.page;
	return;
    }

    ses.page += pgTail();
    page = httpHead("200 OK", ses.page.size()) + ses.page;
}

void TWEB::getAbout( SSess &ses )
{
    ses.page = ses.page+"<center><table class='page_auth'>\n"
	"<TR><TD>"+PACKAGE+" "+VERSION+"</TD></TR>\n"
	"<TR class='content'><TD>\n"
	"<table border='0'>\n"
	"<TR><TD><font color='Blue'>"+_("Name: ")+"</font></TD><TD>OpenSCADA</TD></TR>\n"
	"<TR><TD><font color='Blue'>"+_("License: ")+"</font></TD><TD>GPL</TD></TR>\n"
	"<TR><TD><font color='Blue'>"+_("Author: ")+"</font></TD><TD>Roman Savochenko</TD></TR>\n"
	"</table>\n"
	"</TD></TR></table><br/>\n"
	"<table class='page_auth'>\n"
	"<TR><TD>"MOD_ID" "MOD_VER"</TD></TR>\n"
	"<TR class='content'><TD>\n"
	"<table border='0'>\n"
	"<TR><TD><font color='Blue'>"+_("Name: ")+"</font></TD><TD>"+_(MOD_NAME)+"</TD></TR>"
	"<TR><TD><font color='Blue'>"+_("Description: ")+"</font></TD><TD>"+_(DESCRIPTION)+"</TD></TR>"
	"<TR><TD><font color='Blue'>"+_("License: ")+"</font></TD><TD>"+_(LICENSE)+"</TD></TR>"
	"<TR><TD><font color='Blue'>"+_("Author: ")+"</font></TD><TD>"+_(AUTHORS)+"</TD></TR>"
	"</table>\n"
	"</TD></TR>\n</table><br/></center>\n";
}

void TWEB::HttpPost( const string &url, string &page, const string &sender, vector<string> &vars, const string &user )
{
    map<string,string>::iterator cntEl;
    SSess ses(TSYS::strDecode(url,TSYS::HttpURL),sender,user,vars,page);

    try {
	ses.url = Mess->codeConvIn("UTF-8", ses.url);	//Internal data into UTF-8
	//To control interface request
	if((cntEl=ses.prm.find("com"))!=ses.prm.end() && cntEl->second == "com") {
	    XMLNode req(""); req.load(ses.content); req.setAttr("path",ses.url);
	    cntrIfCmd(req,ses.user,false);
	    ses.page = req.save();
	    page = httpHead("200 OK",ses.page.size(),"text/xml","","UTF-8")+ses.page;
	    return;
	}

	//Post command to session
	string sesnm = TSYS::pathLev(ses.url,0);
	if(sesnm.size() <= 4 || sesnm.compare(0,4,"ses_") != 0) page = httpHead("404 Not Found");
	else {
	    ResAlloc sesRes(nodeRes(),false);
	    vcaSesAt(sesnm.substr(4)).at().postReq(ses);
	    page = ses.page;
	}
    }
    catch(...) { page = httpHead("404 Not Found"); }
}

void TWEB::messPost( string &page, const string &cat, const string &mess, MessLev type )
{
    //Put system message.
    message(cat.c_str(), (type==Error) ? TMess::Error : (type==Warning) ? TMess::Warning : TMess::Info,"%s",mess.c_str());

    //Prepare HTML messages
    page += "<table border='2' width='40%' align='center'><tbody>\n";
    if(type == Warning )	page += "<tr bgcolor='yellow'><td align='center'><b>Warning!</b></td></tr>\n";
    else if(type == Error )	page += "<tr bgcolor='red'><td align='center'><b>Error!</b></td></tr>\n";
    else page += "<tr bgcolor='#9999ff'><td align='center'><b>Message!</b></td></tr>\n";
    page += "<tr bgcolor='#cccccc'> <td align='center'>"+TSYS::strEncode(mess,TSYS::Html)+"</td></tr>\n";
    page += "</tbody></table>\n";
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

void TWEB::imgConvert(SSess &ses)
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
    if(sim && (prmEl=ses.prm.find("size")) != ses.prm.end() && (newImgH=s2i(prmEl->second)) > 0 && gdImageSY(sim) > newImgH)
    {
	newImgW = gdImageSX(sim)*newImgH/gdImageSY(sim);
	gdImagePtr dim = gdImageCreateTrueColor(newImgW,newImgH);
	gdImageAlphaBlending(dim,0);
	gdImageFilledRectangle(dim,0,0,newImgW-1,newImgH-1,gdImageColorResolveAlpha(dim,0,0,0,127));
	gdImageCopyResampled(dim,sim,0,0,0,0,newImgW,newImgH,gdImageSX(sim),gdImageSY(sim));
	gdImageDestroy(sim);
	sim = dim;
    }

    // Check for disable icon make
    if(sim && (prmEl = ses.prm.find("filtr")) != ses.prm.end() && (prmEl->second == "gray" || prmEl->second == "unact"))
    {
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
    }
    else alpha = 255;

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
	if(tsrc[i_s] == '#' && tsrc.substr(i_s,3) == "###" && (i_s+3)<tsrc.size() && tsrc[i_s+3] != '#')
	{
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
SSess::SSess( const string &iurl, const string &isender, const string &iuser, vector<string> &ivars, const string &icontent ) :
    url(iurl), sender(isender), user(iuser), content(icontent), vars(ivars)
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
	if(vars[i_vr].compare(0,vars[i_vr].find(":",0),"Content-Type") == 0 && (pos=vars[i_vr].find(c_bound,0)) != string::npos)
	{
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
