
//OpenSCADA system module UI.WebVision file: web_vision.cpp
/***************************************************************************
 *   Copyright (C) 2007-2008 by Roman Savochenko                           *
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

#include <getopt.h>
#include <signal.h>
#include <string.h>

#include <config.h>
#include <tsys.h>
#include <tmess.h>

#include "web_vision.h"

//************************************************
//* Modul info!                                  *
#define MOD_ID		"WebVision"
#define MOD_NAME	"Operation user interface (WEB)"
#define MOD_TYPE	"UI"
#define VER_TYPE	VER_UI
#define SUB_TYPE	"WWW"
#define MOD_VERSION	"0.6.10"
#define AUTORS		"Roman Savochenko"
#define DEVELOPERS	"Roman Savochenko, Lysenko Maxim, Yashina Kseniya"
#define DESCRIPTION	"Web operation user interface for visual control area (VCA) projects playing."
#define LICENSE		"GPL"
//************************************************

WebVision::TWEB *WebVision::mod;

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
	    return new WebVision::TWEB( source );
	return NULL;
    }
}

using namespace WebVision;

//************************************************
//* TWEB                                         *
//************************************************
TWEB::TWEB( string name ) : mTSess(10), chck_st(false)
{
    mId		= MOD_ID;
    mName	= MOD_NAME;
    mType	= MOD_TYPE;
    mVers	= MOD_VERSION;
    mAutor	= AUTORS;
    mDescr	= DESCRIPTION;
    mLicense	= LICENSE;
    mSource	= name;

    mod		= this;
    id_vcases	= grpAdd("ses_");

    //- Reg export functions -
    modFuncReg( new ExpFunc("void HttpGet(const string&,string&,const string&,vector<string>&,const string&);",
        "Process Get comand from http protocol's!",(void(TModule::*)( )) &TWEB::HttpGet) );
    modFuncReg( new ExpFunc("void HttpPost(const string&,string&,const string&,vector<string>&,const string&);",
        "Process Set comand from http protocol's!",(void(TModule::*)( )) &TWEB::HttpPost) );

    //- Create check sessions timer -
    struct sigevent sigev;
    memset(&sigev,0,sizeof(sigev));
    sigev.sigev_notify = SIGEV_THREAD;
    sigev.sigev_value.sival_ptr = this;
    sigev.sigev_notify_function = TaskSessCheck;
    sigev.sigev_notify_attributes = NULL;
    timer_create(CLOCK_REALTIME,&sigev,&chkSessTm);

    //- Create named colors' container -
    colors["aliceblue"] = rgb(240, 248, 255);
    colors["antiquewhite"] = rgb(250, 235, 215);
    colors["aqua"] = rgb( 0, 255, 255);
    colors["aquamarine"] = rgb(127, 255, 212);
    colors["azure"] = rgb(240, 255, 255);
    colors["beige"] = rgb(245, 245, 220);
    colors["bisque"] = rgb(255, 228, 196);
    colors["black"] = rgb( 0, 0, 0);
    colors["blanchedalmond"] = rgb(255, 235, 205);
    colors["blue"] = rgb( 0, 0, 255);
    colors["blueviolet"] = rgb(138, 43, 226);
    colors["brown"] = rgb(165, 42, 42);
    colors["burlywood"] = rgb(222, 184, 135);
    colors["cadetblue"] = rgb( 95, 158, 160);
    colors["chartreuse"] = rgb(127, 255, 0);
    colors["chocolate"] = rgb(210, 105, 30);
    colors["coral"] = rgb(255, 127, 80);
    colors["cornflowerblue"] = rgb(100, 149, 237);
    colors["cornsilk"] = rgb(255, 248, 220);
    colors["crimson"] = rgb(220, 20, 60);
    colors["cyan"] = rgb( 0, 255, 255);
    colors["darkblue"] = rgb( 0, 0, 139);
    colors["darkcyan"] = rgb( 0, 139, 139);
    colors["darkgoldenrod"] = rgb(184, 134, 11);
    colors["darkgray"] = rgb(169, 169, 169);
    colors["darkgreen"] = rgb( 0, 100, 0);
    colors["darkgrey"] = rgb(169, 169, 169);
    colors["darkkhaki"] = rgb(189, 183, 107);
    colors["darkmagenta"] = rgb(139, 0, 139);
    colors["darkolivegreen"] = rgb( 85, 107, 47);
    colors["darkorange"] = rgb(255, 140, 0);
    colors["darkorchid"] = rgb(153, 50, 204);
    colors["darkred"] = rgb(139, 0, 0);
    colors["darksalmon"] = rgb(233, 150, 122);
    colors["darkseagreen"] = rgb(143, 188, 143);
    colors["darkslateblue"] = rgb( 72, 61, 139);
    colors["darkslategray"] = rgb( 47, 79, 79);
    colors["darkslategrey"] = rgb( 47, 79, 79);
    colors["darkturquoise"] = rgb( 0, 206, 209);
    colors["darkviolet"] = rgb(148, 0, 211);
    colors["deeppink"] = rgb(255, 20, 147);
    colors["deepskyblue"] = rgb( 0, 191, 255);
    colors["dimgray"] = rgb(105, 105, 105);
    colors["dimgrey"] = rgb(105, 105, 105);
    colors["dodgerblue"] = rgb( 30, 144, 255);
    colors["firebrick"] = rgb(178, 34, 34);
    colors["floralwhite"] = rgb(255, 250, 240);
    colors["forestgreen"] = rgb( 34, 139, 34);
    colors["fuchsia"] = rgb(255, 0, 255);
    colors["gainsboro"] = rgb(220, 220, 220);
    colors["ghostwhite"] = rgb(248, 248, 255);
    colors["gold"] = rgb(255, 215, 0);
    colors["goldenrod"] = rgb(218, 165, 32);
    colors["gray"] = rgb(128, 128, 128);
    colors["grey"] = rgb(128, 128, 128);
    colors["green"] = rgb( 0, 128, 0);
    colors["greenyellow"] = rgb(173, 255, 47);
    colors["honeydew"] = rgb(240, 255, 240);
    colors["hotpink"] = rgb(255, 105, 180);
    colors["indianred"] = rgb(205, 92, 92);
    colors["indigo"] = rgb( 75, 0, 130);
    colors["ivory"] = rgb(255, 255, 240);
    colors["khaki"] = rgb(240, 230, 140);
    colors["lavender"] = rgb(230, 230, 250);
    colors["lavenderblush"] = rgb(255, 240, 245);
    colors["lawngreen"] = rgb(124, 252, 0);
    colors["lemonchiffon"] = rgb(255, 250, 205);
    colors["lightblue"] = rgb(173, 216, 230);
    colors["lightcoral"] = rgb(240, 128, 128);
    colors["lightcyan"] = rgb(224, 255, 255);
    colors["lightgoldenrodyellow"] = rgb(250, 250, 210);
    colors["lightgray"] = rgb(211, 211, 211);
    colors["lightgreen"] = rgb(144, 238, 144);
    colors["lightgrey"] = rgb(211, 211, 211);
    colors["lightpink"] = rgb(255, 182, 193);
    colors["lightsalmon"] = rgb(255, 160, 122);
    colors["lightseagreen"] = rgb( 32, 178, 170);
    colors["lightskyblue"] = rgb(135, 206, 250);
    colors["lightslategray"] = rgb(119, 136, 153);
    colors["lightslategrey"] = rgb(119, 136, 153);
    colors["lightsteelblue"] = rgb(176, 196, 222);
    colors["lightyellow"] = rgb(255, 255, 224);
    colors["lime"] = rgb( 0, 255, 0);
    colors["limegreen"] = rgb( 50, 205, 50);
    colors["linen"] = rgb(250, 240, 230);
    colors["magenta"] = rgb(255, 0, 255);
    colors["maroon"] = rgb(128, 0, 0);
    colors["mediumaquamarine"] = rgb(102, 205, 170);
    colors["mediumblue"] = rgb( 0, 0, 205);
    colors["mediumorchid"] = rgb(186, 85, 211);
    colors["mediumpurple"] = rgb(147, 112, 219);
    colors["mediumseagreen"] = rgb( 60, 179, 113);
    colors["mediumslateblue"] = rgb(123, 104, 238);
    colors["mediumspringgreen"] = rgb( 0, 250, 154);
    colors["mediumturquoise"] = rgb( 72, 209, 204);
    colors["mediumvioletred"] = rgb(199, 21, 133);
    colors["midnightblue"] = rgb( 25, 25, 112);
    colors["mintcream"] = rgb(245, 255, 250);
    colors["mistyrose"] = rgb(255, 228, 225);
    colors["moccasin"] = rgb(255, 228, 181);
    colors["navajowhite"] = rgb(255, 222, 173);
    colors["navy"] = rgb( 0, 0, 128);
    colors["oldlace"] = rgb(253, 245, 230);
    colors["olive"] = rgb(128, 128, 0);
    colors["olivedrab"] = rgb(107, 142, 35);
    colors["orange"] = rgb(255, 165, 0);
    colors["orangered"] = rgb(255, 69, 0);
    colors["orchid"] = rgb(218, 112, 214);
    colors["palegoldenrod"] = rgb(238, 232, 170);
    colors["palegreen"] = rgb(152, 251, 152);
    colors["paleturquoise"] = rgb(175, 238, 238);
    colors["palevioletred"] = rgb(219, 112, 147);
    colors["papayawhip"] = rgb(255, 239, 213);
    colors["peachpuff"] = rgb(255, 218, 185);
    colors["peru"] = rgb(205, 133, 63);
    colors["pink"] = rgb(255, 192, 203);
    colors["plum"] = rgb(221, 160, 221);
    colors["powderblue"] = rgb(176, 224, 230);
    colors["purple"] = rgb(128, 0, 128);
    colors["red"] = rgb(255, 0, 0);
    colors["rosybrown"] = rgb(188, 143, 143);
    colors["royalblue"] = rgb( 65, 105, 225);
    colors["saddlebrown"] = rgb(139, 69, 19);
    colors["salmon"] = rgb(250, 128, 114);
    colors["sandybrown"] = rgb(244, 164, 96);
    colors["seagreen"] = rgb( 46, 139, 87);
    colors["seashell"] = rgb(255, 245, 238);
    colors["sienna"] = rgb(160, 82, 45);
    colors["silver"] = rgb(192, 192, 192);
    colors["skyblue"] = rgb(135, 206, 235);
    colors["slateblue"] = rgb(106, 90, 205);
    colors["slategray"] = rgb(112, 128, 144);
    colors["slategrey"] = rgb(112, 128, 144);
    colors["snow"] = rgb(255, 250, 250);
    colors["springgreen"] = rgb( 0, 255, 127);
    colors["steelblue"] = rgb( 70, 130, 180);
    colors["tan"] = rgb(210, 180, 140);
    colors["teal"] = rgb( 0, 128, 128);
    colors["thistle"] = rgb(216, 191, 216);
    colors["tomato"] = rgb(255, 99, 71);
    colors["turquoise"] = rgb( 64, 224, 208);
    colors["violet"] = rgb(238, 130, 238);
    colors["wheat"] = rgb(245, 222, 179);
    colors["white"] = rgb(255, 255, 255);
    colors["whitesmoke"] = rgb(245, 245, 245);
    colors["yellow"] = rgb(255, 255, 0);
    colors["yellowgreen"] = rgb(154, 205, 50);

    //> Default CSS init
    mCSStables =
	"hr { width: 95%; }\n"
	"body { background-color: #818181; margin: 0px; }\n"
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
	"#popupmenu select { background-color: #E6E6E6; border: 1px solid black; padding: 1px; }\n";

#if 0
    char mess[][100] = { "Time", "Level", "Category", "Message", "mcsec", "Ready" };
#endif
}

TWEB::~TWEB()
{

}

string TWEB::modInfo( const string &name )
{
    if( name == "SubType" )		return SUB_TYPE;
    else if( name == "Auth" )		return "1";
    else if( name == _("Developers") )	return DEVELOPERS;
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
    if( vcaSesPresent(name) )	return;
    chldAdd( id_vcases, new VCASess(name,isCreate) );
}

string TWEB::optDescr( )
{
    char buf[STR_BUF_LEN];

    snprintf(buf,sizeof(buf),_(
	"======================= The module <%s:%s> options =======================\n"
	"---------- Parameters of the module section <%s> in config file ----------\n"
	"SessTimeLife <time>      Time of the session life, minutes (default 10).\n\n"),
	MOD_TYPE,MOD_ID,nodePath().c_str());

    return buf;
}

void TWEB::load_( )
{
    //> Load parameters from command line
    int next_opt;
    const char *short_opt="h";
    struct option long_opt[] =
    {
	{"help"      ,0,NULL,'h'},
	{NULL        ,0,NULL,0  }
    };

    optind=opterr=0;
    do
    {
	next_opt=getopt_long(SYS->argc,(char * const *)SYS->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': fprintf(stdout,TSYS::strEncode(optDescr(),TSYS::FormatPrint).c_str()); break;
	    case -1 : break;
	}
    } while(next_opt != -1);

    //> Load parameters from config file
    mTSess = atoi( TBDS::genDBGet(nodePath()+"SessTimeLife",TSYS::int2str(mTSess)).c_str() );
}

void TWEB::save_( )
{
    TBDS::genDBSet(nodePath()+"SessTimeLife",TSYS::int2str(mTSess));
}

void TWEB::modStart( )
{
    //- Start interval timer for periodic thread creating of DB syncing -
    struct itimerspec itval;
    itval.it_interval.tv_sec = itval.it_value.tv_sec = 10;
    itval.it_interval.tv_nsec = itval.it_value.tv_nsec = 0;
    timer_settime(chkSessTm, 0, &itval, NULL);

    run_st = true;
}

void TWEB::modStop( )
{
    //- Stop interval timer for periodic thread creating -
    struct itimerspec itval;
    itval.it_interval.tv_sec = itval.it_interval.tv_nsec =
    itval.it_value.tv_sec = itval.it_value.tv_nsec = 0;
    timer_settime(chkSessTm, 0, &itval, NULL);
    if( TSYS::eventWait( chck_st, false, nodePath()+"chck_stop",5) )
	throw TError(nodePath().c_str(),_("Checking if session is not stopped!"));

    run_st = false;
}

void TWEB::TaskSessCheck( union sigval obj )
{
    TWEB *web = (TWEB *)obj.sival_ptr;
    if( web->chck_st )  return;
    web->chck_st = true;

#if OSC_DEBUG >= 2
    mess_debug(web->nodePath().c_str(),_("Timer's thread <%u> call. TID: %ld"),pthread_self(),(long int)syscall(224));
#endif

    time_t cur_tm = time(NULL);

    //> Check for opened sessions timeout close
    vector<string> list;
    web->vcaSesList( list );
    for( int i_s = 0; i_s < list.size(); i_s++ )
	if( cur_tm > web->vcaSesAt(list[i_s]).at().lstReq()+web->sessTime()*60 )
	    web->vcaSesDel(list[i_s]);

    web->chck_st = false;
}

string TWEB::httpHead( const string &rcode, int cln, const string &cnt_tp, const string &addattr )
{
    return "HTTP/1.0 "+rcode+"\r\n"
	"Server: "+PACKAGE_STRING+"\r\n"
	"Accept-Ranges: bytes\r\n"
	"Content-Length: "+TSYS::int2str(cln)+"\r\n"
	"Connection: close\r\n"
	"Content-type: "+cnt_tp+"\r\n"
	"Charset="+Mess->charset()+"\r\n"+addattr+"\r\n";
}

string TWEB::pgHead( const string &head_els, const string &title )
{
    string shead =
	"<?xml version='1.0' ?>\n"
	"<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Transitional//EN'\n"
	"'DTD/xhtml1-transitional.dtd'>\n"
	"<html xmlns='http://www.w3.org/1999/xhtml'>\n"
	"<head>\n"
	"  <meta http-equiv='Content-Type' content='text/html; charset="+Mess->charset()+"'/>\n"
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

string TWEB::pgTail( )
{
    return "</body>\n</html>";
}

void TWEB::HttpGet( const string &url, string &page, const string &sender, vector<string> &vars, const string &user )
{
    SSess ses(TSYS::strDecode(url,TSYS::HttpURL),sender,user,vars,"");
    ses.page = pgHead();

    try
    {
	string zero_lev = TSYS::pathLev(ses.url,0);
	//> Get about module page
	if( zero_lev == "about" )       getAbout(ses);
	//> Get module icon and global image
	else if( zero_lev == "ico" || zero_lev.substr(0,4) == "img_" )
	{
	    string itp;
	    ses.page=TUIS::icoGet( zero_lev=="ico"?"UI."MOD_ID:zero_lev.substr(4), &itp );
	    page = httpHead("200 OK",ses.page.size(),string("image/")+itp)+ses.page;
	    return;
	}
	else
	{
	    //> Session select or new session for project creation
	    if( zero_lev.empty() )
	    {
		bool sesPrjOk = false;
		ses.page = ses.page+
		    "<h1 class='head'>"+PACKAGE_NAME+". "+_(MOD_NAME)+"</h1>\n<hr/><br/>\n"
		    "<center><table class='work'>\n";
		//>> Get present sessions list
		XMLNode req("get");
		req.setAttr("path","/%2fses%2fses")->setAttr("chkUserPerm","1");
		cntrIfCmd(req,ses.user);
		string self_sess;
		if( req.childSize() )
		{
		    ses.page = ses.page+
			"<tr><th>"+_("Connect to opened session")+"</th></tr>\n"
			"<tr><td class='content'>\n"
			"<table border='0' cellspacing='3px' width='100%'>\n";
		    for( int i_ch = 0; i_ch < req.childSize(); i_ch++ )
		    {
			ses.page += "<tr><td style='text-align: center;'><a href='/"MOD_ID"/ses_"+req.childGet(i_ch)->text()+"/'>"+
			    req.childGet(i_ch)->text()+"</a></td></tr>";
			if( req.childGet(i_ch)->attr("user") == user ) self_sess += req.childGet(i_ch)->text()+";";
		    }
		    ses.page += "</table></td></tr>\n";
		    sesPrjOk = true;
		}
		//>> Get present projects list
		req.clear()->setAttr("path","/%2fprm%2fcfg%2fprj")->setAttr("chkUserPerm","1");
		cntrIfCmd(req,ses.user);
		if( req.childSize() )
		{
		    ses.page = ses.page +
			"<tr><th>"+_("Create new session for present project")+"</th></tr>\n"
			"<tr><td class='content'>\n"
			"<table border='0' cellspacing='3px' width='100%'>\n";
		    for( int i_ch = 0; i_ch < req.childSize(); i_ch++ )
		    {
			if( !SYS->security().at().access(user,SEQ_WR,"root","root",RWRWR_) && self_sess.find(req.childGet(i_ch)->text()+";",0) != string::npos )
			    continue;
			ses.page += "<tr><td style='text-align: center;'><a href='/"MOD_ID"/prj_"+req.childGet(i_ch)->attr("id")+"/'>"+
			    req.childGet(i_ch)->text()+"</a></td></tr>";
		    }
		    ses.page += "</table></td></tr>\n";
		    sesPrjOk = true;
		}
		ses.page += "</table></center>";
		if( !sesPrjOk )	messPost(ses.page,nodePath(),_("No one sessions and projects of VCA engine are present for user!"),TWEB::Warning);
	    }
	    //> New session create
	    else if( zero_lev.size() > 4 && zero_lev.substr(0,4) == "prj_" )
	    {
		XMLNode req("connect");
		req.setAttr("path","/%2fserv%2fsess")->setAttr("prj",zero_lev.substr(4));
		if( cntrIfCmd(req,ses.user) )
		    messPost(ses.page,req.attr("mcat").c_str(),req.text().c_str(),TWEB::Error);
		else
		{
		    ses.page = pgHead("<META HTTP-EQUIV='Refresh' CONTENT='0; URL=/"MOD_ID"/ses_"+req.attr("sess")+"/'/>")+
			"<center>Open new session '"+req.attr("sess")+"' for project: '"+zero_lev.substr(4)+"'</center>\n<br/>";
		    vcaSesAdd(req.attr("sess"),true);
		}
	    }
	    //> Main session page data prepare
	    else if( zero_lev.size() > 4 && zero_lev.substr(0,4) == "ses_" )
	    {
		string sesnm = zero_lev.substr(4);
		//>> Check for session present
		if( !ses.prm.size() )
		{
		    XMLNode req("get"); req.setAttr("path",ses.url+"/%2fobj%2fst%2fen");
		    if( cntrIfCmd(req,ses.user) || !atoi(req.text().c_str()) )	{ HttpGet( "", page, sender, vars, user ); return; }
		}
		//>> Call to session
		try{ vcaSesAt(sesnm).at().getReq(ses); }
		catch(...)
		{
		    if( !vcaSesPresent(sesnm) )
		    {
			vcaSesAdd(sesnm,false);
			vcaSesAt(sesnm).at().getReq(ses);
		    }
		    else throw;
		}
		page = ses.page;
		return;
	    }
            else mess_err(nodePath().c_str(),_("An unaccessible request is received: '%s'"),zero_lev.c_str());
	}
    }catch(TError err)
    {
	ses.page = "Page <"+ses.url+"> error: "+err.mess;
	page = httpHead("404 Not Found",ses.page.size())+ses.page;
	return;
    }

    ses.page += pgTail();
    page = httpHead("200 OK",ses.page.size())+ses.page;
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
	"<TR><TD>"MOD_ID" "MOD_VERSION"</TD></TR>\n"
	"<TR class='content'><TD>\n"
	"<table border='0'>\n"
	"<TR><TD><font color='Blue'>"+_("Name: ")+"</font></TD><TD>"+_(MOD_NAME)+"</TD></TR>"
	"<TR><TD><font color='Blue'>"+_("Desription: ")+"</font></TD><TD>"+_(DESCRIPTION)+"</TD></TR>"
	"<TR><TD><font color='Blue'>"+_("License: ")+"</font></TD><TD>"+_(LICENSE)+"</TD></TR>"
	"<TR><TD><font color='Blue'>"+_("Author: ")+"</font></TD><TD>"+_(AUTORS)+"</TD></TR>"
	"</table>\n"
	"</TD></TR>\n</table><br/></center>\n";
}

void TWEB::getAuth( SSess &ses )
{
    ses.page = ses.page+"<center><table class='page_auth'>"
	"<tr><td><b>"+_("Enter to module")+"</b></td></tr>\n"
	"<tr class='content'> <td align='center'>\n"
	"<form method='post' action='/"MOD_ID+ses.url+"' enctype='multipart/form-data'>\n"
	"<table cellpadding='3px'>\n"
	"<tr><td><b>"+_("User name")+"</b></td><td><input type='text' name='user' size='20'/></td></tr>\n"
	"<tr><td><b>"+_("Password")+"</b></td><td><input type='password' name='pass' size='20'/></td></tr>\n"
	"<tr><td colspan='2' align='center'><input type='submit' name='auth_enter' value='"+_("Enter")+"'/>\n"
	"<input type='reset' name='clean' value='"+_("Clean")+"'/>\n"
	"</td></tr></table>\n</form>\n"
	"</td></tr></table></center>\n";
}

string TWEB::getCookie( string name, vector<string> &vars )
{
    for( unsigned i_var = 0; i_var < vars.size(); i_var++)
	if( vars[i_var].substr(0, vars[i_var].find(":",0)) == "Cookie" )
	{
	    int i_beg = vars[i_var].find(name+"=",0);
	    if( i_beg == string::npos ) return "";
	    i_beg += name.size()+1;
	    return vars[i_var].substr(i_beg,vars[i_var].find(";",i_beg)-i_beg);
	}
    return "";
}

void TWEB::HttpPost( const string &url, string &page, const string &sender, vector<string> &vars, const string &user )
{
    map<string,string>::iterator cntEl;
    SSess ses(TSYS::strDecode(url,TSYS::HttpURL),sender,user,vars,page);

    try
    {
	//> To control interface request
	if( (cntEl=ses.prm.find("com"))!=ses.prm.end() && cntEl->second == "com" )
	{
	    XMLNode req(""); req.load(ses.content); req.setAttr("path",ses.url);
	    cntrIfCmd(req,ses.user,false);
	    ses.page = req.save();
	    page = httpHead("200 OK",ses.page.size(),"text/xml; charset=UTF-8")+ses.page;
	    return;
	}

	//> Post command to session
	string sesnm = TSYS::pathLev(ses.url,0);
	if( sesnm.size() <= 4 || sesnm.substr(0,4) != "ses_" ) page = httpHead("404 Not Found");
	else
	{
	    vcaSesAt(sesnm.substr(4)).at().postReq(ses);
	    page = ses.page;
	}
    }
    catch(...) { page = httpHead("404 Not Found"); }
}

void TWEB::messPost( string &page, const string &cat, const string &mess, MessLev type )
{
    //> Put system message.
    message(cat.c_str(), (type==Error) ? TMess::Error : (type==Warning) ? TMess::Warning : TMess::Info,"%s",mess.c_str());

    //> Prepare HTML messages
    page = page+"<table border='2' width='40%' align='center'><tbody>\n";
    if(type == Warning )	page = page+"<tr bgcolor='yellow'><td align='center'><b>Warning!</b></td></tr>\n";
    else if(type == Error )	page = page+"<tr bgcolor='red'><td align='center'><b>Error!</b></td></tr>\n";
    else page = page+"<tr bgcolor='#9999ff'><td align='center'><b>Message!</b></td></tr>\n";
    page = page+"<tr bgcolor='#cccccc'> <td align='center'>"+TSYS::strEncode(mess,TSYS::Html)+"</td></tr>\n";
    page = page+"</tbody></table>\n";
}

int TWEB::cntrIfCmd( XMLNode &node, const string &user, bool VCA )
{
    node.setAttr("user",user);
    if( VCA )	node.setAttr("path","/UI/VCAEngine"+node.attr("path"));
    SYS->cntrCmd(&node);
    return atoi(node.attr("rez").c_str());
}

void TWEB::cntrCmdProc( XMLNode *opt )
{
    //- Get page info -
    if( opt->name() == "info" )
    {
	TUI::cntrCmdProc(opt);
	if(ctrMkNode("area",opt,1,"/prm/cfg",_("Module options")))
	    ctrMkNode("fld",opt,-1,"/prm/cfg/lf_tm",_("Life time of session (min)"),0660,"root","root",1,"tp","dec");
	ctrMkNode("fld",opt,-1,"/help/g_help",_("Options help"),0440,"root","root",3,"tp","str","cols","90","rows","5");
	return;
    }

    //- Process command to page -
    string a_path = opt->attr("path");
    if( a_path == "/prm/cfg/lf_tm" )
    {
	if( ctrChkNode(opt,"get",0660,"root","root",SEQ_RD) )   opt->setText( TSYS::int2str(sessTime()) );
	if( ctrChkNode(opt,"set",0660,"root","root",SEQ_WR) )   setSessTime( atoi(opt->text().c_str()) );
    }
    else if( a_path == "/help/g_help" && ctrChkNode(opt,"get",0440) )   opt->setText(optDescr());
    else TUI::cntrCmdProc(opt);
}

int TWEB::colorParse( const string &tclr )
{
    string clr = tclr;
    int alpha;
    size_t found = clr.find("-");
    if (found != string::npos)
    {
        clr = tclr.substr(0,found);
        alpha =  atoi(tclr.substr(found+1).c_str());
    }
    else alpha = 255;

    if( clr.size() >= 4 && clr[0] == '#' )
    {
	int el_sz = clr.size()/3;
        return ((int)vmin(127,(alpha/2+0.5))<<24)+
               (strtol(clr.substr(1,el_sz).c_str(),NULL,16)<<16)+
               (strtol(clr.substr(1+el_sz,el_sz).c_str(),NULL,16)<<8)+
		strtol(clr.substr(1+2*el_sz,el_sz).c_str(),NULL,16);
    }
    else if( clr.size() )
    {
	map<string,int>::iterator iclr = colors.find(clr);
        if( iclr != colors.end() )
            return ((int)vmin(127,(alpha/2+0.5))<<24)+ iclr->second;
    }
    return -1;
}

string TWEB::trMessReplace( const string &tsrc )
{
    string trez; trez.reserve(tsrc.size());

    int txtBeg = 0, i_s;
    for( i_s = 0; i_s < tsrc.size(); i_s++ )
	if( tsrc[i_s] == '#' && tsrc.substr(i_s,3) == "###" && (i_s+3)<tsrc.size() && tsrc[i_s+3] != '#' )
	{
	    int i_r;
	    for( i_r = i_s+3; i_r < tsrc.size(); i_r++ )
	    if( (tsrc[i_r] == '#' && tsrc.substr(i_r,3) == "###" && ((i_r+3)>=tsrc.size() || tsrc[i_r+3] != '#')) || tsrc[i_r] == '\n' )
		break;
	    if( i_r < tsrc.size() && tsrc[i_r] != '\n' )
	    {
		trez.append(tsrc.substr(txtBeg,i_s-txtBeg));
		trez.append(_(tsrc.substr(i_s+3,i_r-i_s-3).c_str()));
		i_s = i_r+2;
		txtBeg = i_r+3;
		continue;
	    }
	}
    if( txtBeg < i_s ) trez.append(tsrc.substr(txtBeg,i_s-txtBeg));

    return trez;
}

//*************************************************
//* SSess                                         *
//*************************************************
SSess::SSess( const string &iurl, const string &isender, const string &iuser, vector<string> &ivars, const string &icontent ) :
    url(iurl), sender(isender), user(iuser), vars(ivars), content(icontent)
{
    //- URL parameters parse -
    int prmSep = iurl.find("?");
    if( prmSep != string::npos )
    {
	url = iurl.substr(0,prmSep);
	string prms = iurl.substr(prmSep+1);
	string sprm;
	for( int iprm = 0; (sprm=TSYS::strSepParse(prms,0,'&',&iprm)).size(); )
	{
	    prmSep = sprm.find("=");
	    if( prmSep == string::npos ) prm[sprm] = "true";
	    else prm[sprm.substr(0,prmSep)] = sprm.substr(prmSep+1);
	}
    }

    //- Content parse -
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

	//-- Get name --
	i_bnd = c_head.find(c_name,0)+strlen(c_name);
	string c_name = c_head.substr(i_bnd,c_head.find("\"",i_bnd)-i_bnd);
	i_bnd = c_head.find(c_file,0);
	if( i_bnd == string::npos )
	{
	    //--- Get value ---
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
