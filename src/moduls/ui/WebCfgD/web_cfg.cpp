
//OpenSCADA system module UI.WebCfgD file: web_cfg.cpp
/***************************************************************************
 *   Copyright (C) 2008 by Roman Savochenko                                *
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

#include <time.h>
#include <getopt.h>
#include <string.h>
#include <string>
#include <gd.h>

#include <config.h>
#include <tsys.h>
#include <tmess.h>
#include <tsecurity.h>

#include "web_cfg.h"

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"WebCfgD"
#define MOD_NAME	"Dynamic WEB configurator"
#define MOD_TYPE	"UI"
#define VER_TYPE	VER_UI
#define SUB_TYPE	"WWW"
#define MOD_VERSION	"0.3.0"
#define AUTORS		"Roman Savochenko"
#define DESCRIPTION	"Allow the dynamic WEB based OpenSCADA system configurator. Use XHTML, CSS and JavaScript technologies."
#define LICENSE		"GPL"
//*************************************************

WebCfgD::TWEB *WebCfgD::mod;

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
	    return new WebCfgD::TWEB( source );
	return NULL;
    }
}

extern char *WebCfgDVCA_js;

using namespace WebCfgD;

//*************************************************
//* TWEB                                          *
//*************************************************
TWEB::TWEB( string name ) : mTAuth(10), lst_ses_chk(0)
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

    //- Reg export functions -
    modFuncReg( new ExpFunc("void HttpGet(const string&,string&,const string&,vector<string>&);",
	"Process Get comand from http protocol's!",(void(TModule::*)( )) &TWEB::HttpGet) );
    modFuncReg( new ExpFunc("void HttpPost(const string&,string&,const string&,vector<string>&,const string&);",
	"Process Set comand from http protocol's!",(void(TModule::*)( )) &TWEB::HttpPost) );

    //> Default CSS init
    mCSStables =
	"hr {width:100%}\n"
	"body {background-color:#818181}\n"
	"h1.head {text-align:center; color:#ffff00 }\n"
	"h2.title {text-align:center; font-style:italic; margin: 0px; padding: 0px; border-width:0px }\n"
	"table.page_head {background-color:#cccccc; border:3px ridge blue; width:100% }\n"
	"table.page_head td.tool {text-align:center; border:1px solid blue; width:120px;  white-space: nowrap }\n"
	"table.page_head td.user {text-align:left; border:1px solid blue; width:120px; white-space: nowrap }\n"
	"table.page_area {background-color:#9999ff; border:3px ridge #a9a9a9; width:100%; padding:2px }\n"
	"table.page_area tr.content {background-color:#cccccc; border:5px ridge #9999ff; padding:5px }\n"
	"table.page_auth {background-color:#9999ff; border:3px ridge #a9a9a9; padding: 2px; }\n"
	"table.page_auth tr.content {background-color:#cccccc; border:5px ridge #9999ff; padding: 5px; }\n"
	"select { font-family: Verdana,Arial,Helvetica,sans-serif; font-size:12px; }\n"
	"#popupmenu { position: absolute; border: 0; width: 150px; height: 150px; overflow : auto; }\n"
	"#popupmenu select { background-color: #E6E6E6; border: 1px solid black; padding: 1px; }\n"
	"#combomenu { position: absolute; border: 1px solid black; width: 150px; height: 120px; overflow: auto; background-color: white; padding: 1px; }\n"
	"#combomenu select { background-color: white; border: 0; }\n"
	"table.main { background-color: #E6E6E6; border: 3px ridge #FF9253; padding: 5px; width: auto; table-layout: fixed; font-family: Verdana,Arial,Helvetica,sans-serif; font-size:12px; }\n"
	"table.main input,select { font-family: Verdana,Arial,Helvetica,sans-serif; font-size:12px; }\n"
	"table.main td.title { color : blue; font-family : Arial; font-size : 30px; font-style : italic; text-align : center; }\n"
	"table.main td.tool { text-align: left; border-top: 2px solid black; }\n"
	"table.main td.tool img { height: 22px; border: 0; vertical-align: middle; }\n"
	"table.main td.status { text-align: left; }\n"
	"table.main tr.body { background-color : lightgrey; overflow : auto; }\n"
	"table.main td.tree { background-color : #F7FFF6; width: 250px; text-align: left; vertical-align : top; border: 3px ridge #F7FFF6;"
	" white-space: nowrap; color: black; }\n"
	"table.main td.tree li.select { background-color : #BDEBED; }\n"
	"table.main td.tree ul { background-color: #F7FFF6; background-repeat: repeat-y; background-attachment: scroll; background-position: left top; }\n"
	"table.main td.tree div { overflow: auto; width: 250px; padding: 5px; max-height: 500px; }\n"
	"table.main td.page { width: 600px; text-align: left; vertical-align : top; border: 3px outset #F7F7F1; background-color: #F7F7F1; }\n"
	"table.page { width: 100%; padding: 2px; }\n"
	"table.page td.title { color: black; font-family : Arial; font-size : 20px; font-style : italic; text-align : left; padding-bottom: 5px; }\n"
	"table.page td.title img { vertical-align: middle; height: 32px; }\n"
	"table.page td.tabs { }\n"
	"table.page td.tabs span { padding-left: 5px; padding-right: 5px; padding-bottom: 2px; padding-top: 2px; border: 1px solid gray; white-space: nowrap; background-color: #DEDED9; cursor: pointer; }\n"
	"table.page td.tabs span.active { background-color: #F7F7F1; cursor: default; border-bottom: 0px none; border-top: 2px solid green; }\n"
	"table.page td.content { border : 2px ridge grey; text-align : left; vertical-align : top; height: 300px; padding: 5px; }\n"
	"table.page td.content div { overflow: auto; width: auto; max-height: 440px; white-space: nowrap; }\n"
	"table.page td.content div.elem { margin-bottom: 2px; overflow : visible; width : 99%; }\n"
	"table.page td.content div.elem span.label { white-space: nowrap; padding-right: 3px; }\n"
	"table.page td.content div.elem span.const { font-weight: bold; }\n"
	"table.page td.content div.elem textarea { width: 100%; }\n"
	"table.page td.content fieldset.elem { margin : 0px; margin-bottom: 2px; padding: 3px; }\n"
	"table.page td.content .list { width: 50%; }\n"
	"table.page td.content div.elem .line { padding-right: 3px; }\n"
	"table.page td.content div.elem .line input,select { vertical-align: middle; border: 1px solid black; }\n"
	"table.page td.content div.elem .line img { vertical-align: middle; height: 18px; cursor: pointer; }\n"
	"table.page td.content div.elem .number input { text-align: right; }\n"
	"ul.tree { clear: both; list-style-image: none; list-style-position: outside; list-style-type: none; margin: 0; padding: 0; white-space: nowrap; }\n"
	"ul.tree a { text-decoration:none; color: black; }\n"
	"ul.tree a.pm { cursor: pointer; }\n"
	"ul.tree img { vertical-align:middle; }\n"
	"ul.tree ul { list-style-image: none; list-style-position: outside; list-style-type: none; margin: 0; padding: 0 0 0 17px; }\n"
	"a.active img {  cursor: pointer; }\n"
	"a.inactive img { cursor: default; }\n";
}

TWEB::~TWEB()
{

}

string TWEB::modInfo( const string &name )
{
    if( name == "SubType" ) return(SUB_TYPE);
    else return( TModule::modInfo( name) );
}

void TWEB::modInfo( vector<string> &list )
{
    TModule::modInfo(list);
    list.push_back("SubType");
}

string TWEB::optDescr( )
{
    char buf[STR_BUF_LEN];
    snprintf(buf,sizeof(buf),_(
	"======================= The module <%s:%s> options =======================\n"
	"---------- Parameters of the module section <%s> in config file ----------\n"
	"SessTimeLife <time>      Time of the sesion life, minutes (default 10).\n"
	"CSSTables    <CSS>	  CSS for creating pages.\n\n"),
	MOD_TYPE,MOD_ID,nodePath().c_str());

    return buf;
}

void TWEB::load_( )
{
    //- Load parameters from command line -
    int next_opt;
    const char *short_opt="h";
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

    //- Load parameters from config file -
    mTAuth = atoi( TBDS::genDBGet(nodePath()+"SessTimeLife",TSYS::int2str(mTAuth)).c_str() );
    mCSStables = TBDS::genDBGet(nodePath()+"CSSTables",mCSStables);
}

void TWEB::save_( )
{
    TBDS::genDBSet(nodePath()+"SessTimeLife",TSYS::int2str(mTAuth));
    TBDS::genDBSet(nodePath()+"CSSTables",mCSStables);
}

void TWEB::modStart()
{
    run_st = true;
}

void TWEB::modStop()
{
    run_st = false;
}

string TWEB::httpHead( const string &rcode, int cln, const string &cnt_tp, const string &addattr )
{
    return  "HTTP/1.0 "+rcode+"\n"
	    "Server: "+PACKAGE_STRING+"\n"
	    "Accept-Ranges: bytes\n"
	    "Content-Length: "+TSYS::int2str(cln)+"\n"
	    "Connection: close\n"
	    "Content-Type: "+cnt_tp+"\n"
	    "Cache-Control: no-cache\n"
	    "Charset="+Mess->charset()+"\n"+addattr+"\n";
}

string TWEB::pgHead( string head_els )
{
    return
	"<?xml version='1.0' ?>\n"
	"<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Transitional//EN'\n"
	"'DTD/xhtml1-transitional.dtd'>\n"
	"<html xmlns='http://www.w3.org/1999/xhtml'>\n"
	"<head>\n"
	"  <meta http-equiv='Content-Type' content='text/html; charset="+Mess->charset()+"'/>\n"
	"  <meta http-equiv='Cache-Control' content='no-store, no-cache, must-revalidate'/>\n"
	"  <meta http-equiv='Cache-Control' content='post-check=0, pre-check=0'/>\n"
	"  <meta http-equiv='Content-Script-Type' content='text/javascript'/>\n"+
	head_els+
	"  <link rel='shortcut icon' href='/"MOD_ID"/ico' type='image' />\n"
	"  <title>"PACKAGE_NAME". "+_(MOD_NAME)+"</title>\n"
	"  <style type='text/css'>\n"+mCSStables+"</style>\n"
	"</head>\n"
	"<body alink='#33ccff' link='#3366ff' text='#000000' vlink='#339999'>\n";
}

string TWEB::pgTail( )
{
    return "</body>\n</html>";
}

void TWEB::HttpGet( const string &urli, string &page, const string &sender, vector<string> &vars )
{
    map<string,string>::iterator prmEl;
    SSess ses(TSYS::strDecode(urli,TSYS::HttpURL),page,sender,vars,"");
    ses.page = pgHead();

    try
    {
	string zero_lev = TSYS::pathLev(ses.url,0);
	//> Get about module page
	if( zero_lev == "about" )	getAbout(ses);
	//> Get module icon and global image
	else if( zero_lev == "ico" || zero_lev.substr(0,4) == "img_" )
	{
	    string itp;
	    ses.page=TUIS::icoGet( zero_lev=="ico"?"UI."MOD_ID:zero_lev.substr(4), &itp );
	    //>> Check for disable icon make
	    prmEl = ses.prm.find("filtr");
	    if( !ses.page.empty() && prmEl!=ses.prm.end() && prmEl->second == "gray" )
	    {
		gdImagePtr sim = NULL;
		if( sim = gdImageCreateFromPngPtr(ses.page.size(),(char*)ses.page.data()) ) itp = "png";
		else if( sim = gdImageCreateFromJpegPtr(ses.page.size(),(char*)ses.page.data()) ) itp = "jpg";
		else if( sim = gdImageCreateFromGifPtr(ses.page.size(),(char*)ses.page.data()) ) itp = "gif";
		if( sim )
		{
		    gdImagePtr dim = gdImageCreate(gdImageSX(sim),gdImageSY(sim));
		    for( int i_y = 0; i_y < gdImageSY(sim); i_y++ )
			for( int i_x = 0; i_x < gdImageSX(sim); i_x++ )
			{
			    int c = gdImageGetPixel(sim,i_x,i_y);
			    int y = (int)(0.3*gdImageRed(sim,c)+0.59*gdImageGreen(sim,c)+0.11*gdImageBlue(sim,c));
			    c = (int)gdImageColorResolveAlpha(dim,y,y,y,gdImageAlpha(sim,c));
			    gdImageSetPixel(dim,i_x,i_y,c);
			}
		    int img_sz;
		    char *img_ptr = NULL;
		    if( itp == "png" ) img_ptr = (char *)gdImagePngPtr(dim,&img_sz);
		    else if( itp == "jpg" ) img_ptr = (char *)gdImageJpegPtr(dim,&img_sz,-1);
		    else if( itp == "gif" ) img_ptr = (char *)gdImageGifPtr(dim,&img_sz);
		    if( img_ptr )
		    {
			ses.page.assign(img_ptr,img_sz);
			gdFree(img_ptr);
		    }
		    gdImageDestroy(dim);
		    gdImageDestroy(sim);
		}
	    }
	    page = httpHead("200 OK",ses.page.size(),string("image/")+itp)+ses.page;
	    return;
	}
	else
	{
	    sesCheck( ses );
	    //> Auth dialog preparing
	    if( !ses.user.size() )
	    {
		ses.page = ses.page+"<h1 class='head'>"PACKAGE_NAME". "+_(MOD_NAME)+"</h1>\n<hr/><br/>\n";
		getAuth( ses );
	    }
	    else
	    {
		prmEl = ses.prm.find("com");
		string wp_com = (prmEl!=ses.prm.end()) ? prmEl->second : "";

		//> Main work page create.
		if( wp_com.empty() )
		    ses.page = ses.page+
			"<center>\n"
			"<table class='main' border='0' cellspacing='3px'>\n"
			" <tr><td class='title' colspan='2'>"+PACKAGE_NAME+". "+_(MOD_NAME)+"</td></tr>\n"
			" <tr><td class='tool' colspan='2'>\n"
			"  <a id='actLoad' class='inactive' title='Load'><img src='/"MOD_ID"/img_load?filtr=gray' /></a>\n"
			"  <a id='actSave' class='inactive' title='Save'><img src='/"MOD_ID"/img_save?filtr=gray' /></a>\n"
			"  <img src='/"MOD_ID"/img_line' />\n"
			"  <a id='actUp' class='inactive' title='Up'><img src='/"MOD_ID"/img_up?filtr=gray' /></a>\n"
			"  <a id='actPrevious' class='inactive' title='Previous'><img src='/"MOD_ID"/img_previous?filtr=gray' /></a>\n"
			"  <a id='actNext' class='inactive' title='Next'><img src='/"MOD_ID"/img_next?filtr=gray' /></a>\n"
			"  <img src='/"MOD_ID"/img_line'/>\n"
			"  <a id='actAddIt' class='inactive' title='Add item'><img src='/"MOD_ID"/img_it_add?filtr=gray' /></a>\n"
			"  <a id='actDelIt' class='inactive' title='Delete item'><img src='/"MOD_ID"/img_it_del?filtr=gray' /></a>\n"
			"  <img src='/"MOD_ID"/img_line'/>\n"
			"  <a id='actCopy' class='inactive' title='Copy item'><img src='/"MOD_ID"/img_editcopy?filtr=gray' /></a>\n"
			"  <a id='actCut' class='inactive' title='Cut item'><img src='/"MOD_ID"/img_editcut?filtr=gray' /></a>\n"
			"  <a id='actPaste' class='inactive' title='Paste item'><img src='/"MOD_ID"/img_editpaste?filtr=gray' /></a>\n"
			"  <img src='/"MOD_ID"/img_line' />\n"
			"  <a id='actUpdate' class='active' title='Reload item'><img src='/"MOD_ID"/img_reload?filtr=none' /></a>\n"
			"  <a id='actStart' class='active' title='Start periodic update'><img src='/"MOD_ID"/img_start?filtr=none' /></a>\n"
			"  <a id='actStop' class='inactive' title='Stop periodic update'><img src='/"MOD_ID"/img_stop?filtr=gray' /></a>\n"
			"  <img src='/"MOD_ID"/img_line' />\n"
			"  <a id='actAbout' class='active' href='/"MOD_ID"/about' title='About'><img src='/"MOD_ID"/img_help' /></a>\n"
			"  <img src='/"MOD_ID"/img_line' />\n"
			"  <span id='selPath' />\n"
			" </td></tr>\n"
			" <tr class='body'>\n"
			"  <td class='tree'><div><ul id='treeRoot' class='tree' /></div></td>\n"
			"  <td class='page'>\n"
			"   <table class='page' border='0' cellspacing='0'>\n"
			"    <tr><td class='title'><img id='pgIco' alt='' /><span id='pgTitle' /></td></tr>\n"
			"    <tr><td id='pgTabs' class='tabs'></td></tr>\n"
			"    <tr><td class='content'><div id='pgCont'></div></td></tr>\n"
			"   </table>\n"
			"  </td>\n"
			" </tr>\n"
			" <tr><td id='status' class='status' colspan='2'/></tr>\n"
			"</table>\n"
			"</center>\n"
			"<SCRIPT>\n"+WebCfgDVCA_js+"\n</SCRIPT>\n";
		//> Get node icon
		else if( wp_com == "ico" )
		{
		    XMLNode req("get");
		    req.setAttr("path",ses.url+"/%2fico");
		    if( !mod->cntrIfCmd(req,ses.user) )
		    {
			ses.page = TSYS::strDecode(req.text(),TSYS::base64);
			page = mod->httpHead("200 OK",ses.page.size(),"image/png")+ses.page;
		    }
		    else
		    {
			string itp;
			ses.page = TUIS::icoGet("disconnect",&itp);
			page = httpHead("200 OK",ses.page.size(),string("image/")+itp)+ses.page;
		    }
		    return;
		}
		//> Get node childs
		else if( wp_com == "chlds" )
		{
		    XMLNode req("chlds");
		    prmEl = ses.prm.find("grp");
		    string gbr = (prmEl!=ses.prm.end()) ? prmEl->second : "";
		    //>> Get information about allow stations
		    if( zero_lev.empty() )
		    {
			vector<string> stls;
			SYS->transport().at().extHostList(ses.user,stls);
			stls.insert(stls.begin(),SYS->id());
			for( int i_st = 0; i_st < stls.size(); i_st++ )
			{
			    XMLNode *chN;
			    if( stls[i_st] == SYS->id() )
				chN = req.childAdd("el")->setAttr("id",SYS->id())->setText(SYS->name());
			    else
			    {
				TTransportS::ExtHost host = SYS->transport().at().extHostGet(ses.user,stls[i_st]);
				chN = req.childAdd("el")->setAttr("id",host.id)->setText(host.name);
			    }
			    //>>>> Check icon
			    XMLNode reqIco("get"); reqIco.setAttr("path","/"+stls[i_st]+"/%2fico");
			    if( mod->cntrIfCmd(reqIco,ses.user) ) chN->setAttr("icoSize","1000");
			    else chN->setAttr("icoSize",TSYS::int2str(reqIco.text().size()));
			    //>>>> Process groups
			    XMLNode brReq("info"); brReq.setAttr("path","/"+SYS->id()+"/%2fbr");
			    mod->cntrIfCmd(brReq,ses.user);
			    for( int i_br = 0; brReq.childSize() && i_br < brReq.childGet(0)->childSize(); i_br++ )
			    {
				XMLNode *chB = chN->childAdd();
				*chB = *brReq.childGet(0)->childGet(i_br);
				chB->setAttr("chPresent","1");
			    }
			}
		    }
		    else
		    {
			req.setAttr("path",ses.url+"/%2fobj")->setAttr("grp",gbr)->setAttr("icoCheck","1");
			mod->cntrIfCmd(req,ses.user);
		    }
		    ses.page = req.save();
		    page = mod->httpHead("200 OK",ses.page.size(),"text/xml")+ses.page;
		    return;
		}
		else if( wp_com == "info" )
		{
		    XMLNode req("info"); req.setAttr("path",ses.url);
		    mod->cntrIfCmd(req,ses.user);
		    ses.page = req.save();
		    page = mod->httpHead("200 OK",ses.page.size(),"text/xml")+ses.page;
		    return;
		}
		else if( wp_com == "modify" )
		{
		    XMLNode req("modify"); req.setAttr("path",ses.url+"/%2fobj");
		    mod->cntrIfCmd(req,ses.user);
		    ses.page = req.save();
		    page = mod->httpHead("200 OK",ses.page.size(),"text/xml")+ses.page;
		    return;
		}
		else if( wp_com == "get" )
		{
		    XMLNode req("get"); req.setAttr("path",ses.url);
		    mod->cntrIfCmd(req,ses.user);
		    ses.page = req.save();
		    page = mod->httpHead("200 OK",ses.page.size(),"text/xml")+ses.page;
		    return;
		}
		else
		{
		    mess_warning(nodePath().c_str(),_("Unknown command: %s."),wp_com.c_str());
		    ses.page = ses.page+"<center>Call page/widget '"+ses.url+"' command: '"+wp_com+"'</center>\n<br/>";
		}
	    }
	}
    }catch( TError err )
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
    ses.page = ses.page+"<center>\n"
	"<table class='page_auth'>\n"
	"<TR><TD>"+MOD_ID+" v"+MOD_VERSION+"</TD></TR>\n"
	"<TR class='content'><TD>\n"
	"<table border='0'>\n"
	"<TR><TD><font color='Blue'>"+_("Name: ")+"</font></TD><TD align='left'>"+_(MOD_NAME)+"</TD></TR>"
	"<TR><TD><font color='Blue'>"+_("Desription: ")+"</font></TD><TD align='left'>"+_(DESCRIPTION)+"</TD></TR>"
	"<TR><TD><font color='Blue'>"+_("License: ")+"</font></TD><TD align='left'>"+_(LICENSE)+"</TD></TR>"
	"<TR><TD><font color='Blue'>"+_("Author: ")+"</font></TD><TD align='left'>"+_(AUTORS)+"</TD></TR>"
	"</table>\n"
	"</TD></TR>\n</table><br/>\n"
	"<table class='page_auth'>\n"
	"<TR><TD>"+PACKAGE+" v"+VERSION+"</TD></TR>\n"
	"<TR class='content'><TD>\n"
	"<table border='0'>\n"
	"<TR><TD><font color='Blue'>"+_("Name: ")+"</font></TD><TD align='left'>"+_(PACKAGE_DESCR)+"</TD></TR>\n"
	"<TR><TD><font color='Blue'>"+_("License: ")+"</font></TD><TD align='left'>"+PACKAGE_LICENSE+"</TD></TR>\n"
	"<TR><TD><font color='Blue'>"+_("Author: ")+"</font></TD><TD align='left'>"+_(PACKAGE_AUTHOR)+"</TD></TR>\n"
	"<TR><TD><font color='Blue'>"+_("Web site: ")+"</font></TD><TD align='left'>"+PACKAGE_SITE+"</TD></TR>\n"
	"</table>\n"
	"</TD></TR></table><br/>\n"
	"</center>\n";
}

void TWEB::getAuth( SSess &ses )
{
    ses.page = ses.page+"<center><table class='page_auth'>"
	"<tr><td><b>"+_("Enter to module")+"</b></td></tr>\n"
	"<tr class='content'> <td align='center'>\n"
	"<form method='post' action='/"+MOD_ID+ses.url+"' enctype='multipart/form-data'>\n"
	"<table cellpadding='3'>\n"
	"<tr><td><b>"+_("User name")+"</b></td><td><input type='text' name='user' size='20'/></td></tr>\n"
	"<tr><td><b>"+_("Password")+"</b></td><td><input type='password' name='pass' size='20'/></td></tr>\n"
	"<tr><td colspan='2' align='center'><input type='submit' name='auth_enter' value='"+_("Enter")+"'/>\n"
	"<input type='reset' name='clean' value='"+_("Clean")+"'/>\n"
	"</td></tr></table>\n</form>\n"
	"</td></tr></table></center>\n";
}

void TWEB::HttpPost( const string &url, string &page, const string &sender, vector<string> &vars, const string &contain )
{
    map<string,string>::iterator cntEl;
    SSess ses(TSYS::strDecode(url,TSYS::HttpURL),page,sender,vars,contain);

    //> Check for autentification POST requests
    if( ses.cnt.find("auth_enter") != ses.cnt.end() )
    {
	string pass;
	if( (cntEl=ses.cnt.find("user")) != ses.cnt.end() )	ses.user = cntEl->second;
	if( (cntEl=ses.cnt.find("pass")) != ses.cnt.end() )	pass = cntEl->second;
	if( SYS->security().at().usrPresent(ses.user) && SYS->security().at().usrAt(ses.user).at().auth(pass) )
	{
	    ses.page = pgHead("<META HTTP-EQUIV='Refresh' CONTENT='0; URL=/"MOD_ID"/"+url+"'/>")+pgTail();
	    page=httpHead("200 OK",ses.page.size(),"text/html",
		"Set-Cookie: oscdAuthQtCfgDId="+TSYS::int2str(sesOpen(ses.user))+"; path=/;\n")+ses.page;
	    return;
	}
	ses.page = pgHead()+"<h1 class='head'>"+PACKAGE_NAME+". "+_(MOD_NAME)+"</h1>\n<hr/><br/>\n";
	messPost(ses.page,nodePath(),_("Auth wrong! Retry please."),TWEB::Error);
	ses.page += "\n";
	getAuth( ses );
	ses.page += pgTail();
	page = httpHead("200 OK",ses.page.size(),"text/html")+ses.page;
	return;
    }
    //> Session check
    sesCheck( ses );
    if( !ses.user.size() || ses.cnt.find("auth_ch") != ses.cnt.end() )
    {
	ses.page = pgHead("<META HTTP-EQUIV='Refresh' CONTENT='0; URL=/"MOD_ID"/"+url+"'/>")+pgTail();
	page=httpHead("200 OK",ses.page.size(),"text/html","Set-Cookie: oscdAuthQtCfgDId=""; path=/;\n")+ses.page;
	return;
    }

    //> Commands process
    cntEl = ses.prm.find("com");
    string wp_com = (cntEl!=ses.prm.end()) ? cntEl->second : "";
    //> Attributes set
    if( wp_com == "com" )
    {
	XMLNode req(""); req.load(ses.content); req.setAttr("path",ses.url);
	mod->cntrIfCmd(req,ses.user);
	ses.page = req.save();
	page = httpHead("200 OK",ses.page.size(),"text/xml")+ses.page;
    }
}

void TWEB::messPost( string &page, const string &cat, const string &mess, MessLev type )
{
    //> Put system message.
    message(cat.c_str(), (type==Error) ? TMess::Error : (type==Warning) ? TMess::Warning : TMess::Info,"%s",mess.c_str());

    //> Prepare HTML messages
    page = page+"<table border='2' width='40%' align='center'><tbody>\n";
    if( type == Warning )	page = page+"<tr bgcolor='yellow'><td align='center'><b>Warning!</b></td></tr>\n";
    else if(type == Error )	page = page+"<tr bgcolor='red'><td align='center'><b>Error!</b></td></tr>\n";
    else page = page+"<tr bgcolor='#9999ff'><td align='center'><b>Message!</b></td></tr>\n";
    page = page+"<tr bgcolor='#cccccc'> <td align='center'>"+TSYS::strEncode(mess,TSYS::Html)+"</td></tr>\n";
    page = page+"</tbody></table>\n";
}

int TWEB::cntrIfCmd( XMLNode &node, const string &user )
{
    int path_off = 0;
    string path = node.attr("path");
    string station = TSYS::pathLev(path,0,false,&path_off);
    if( station.empty() ) station = SYS->id();
    else node.setAttr("path",path.substr(path_off));

    try
    {
	//> Check local station request
	if( station == SYS->id() )
	{
	    node.setAttr("user",user);
	    SYS->cntrCmd(&node);
	    node.setAttr("path",path);
	    return atoi(node.attr("rez").c_str());
	}
	//> Request to remote host
	TTransportS::ExtHost host = SYS->transport().at().extHostGet(user,station);
	AutoHD<TTransportOut> tr = SYS->transport().at().extHost(host,"TrCntr");
	if( !tr.at().startStat() ) tr.at().start();
	node.load(tr.at().messProtIO("0\n"+host.user+"\n"+host.pass+"\n"+node.save(),"SelfSystem"));
	node.setAttr("path",path);
    }catch( TError err )
    { node.setAttr("mcat",err.cat)->setAttr("rez","3")->setText(err.mess); }

    return atoi(node.attr("rez").c_str());
}

int TWEB::sesOpen( string name )
{
    int sess_id;
    ResAlloc res(mRes,true);

    //- Get free identifier -
    do{ sess_id = rand(); }
    while( sess_id == 0 || mAuth.find(sess_id) != mAuth.end() );

    //- Add new session authentification -
    mAuth[sess_id] = SAuth(name,time(NULL));

    return sess_id;
}

void TWEB::sesCheck( SSess &ses )
{
    time_t cur_tm = time(NULL);
    map<int,SAuth>::iterator authEl;

    ResAlloc res(mRes,false);

    //- Check for close old sessions -
    if( cur_tm > lst_ses_chk+10 )
    {
	for( authEl = mAuth.begin(); authEl != mAuth.end(); )
	    if( cur_tm > authEl->second.tAuth+mTAuth*60 )
		mAuth.erase(authEl++);
	    else authEl++;
	lst_ses_chk = cur_tm;
    }

    //- Check for session and close old sessions -
    authEl = mAuth.find(atoi(getCookie("oscdAuthQtCfgDId",ses.vars).c_str()));
    if( authEl != mAuth.end() )
    {
	ses.user = authEl->second.name;
	authEl->second.tAuth = cur_tm;
    }
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

string TWEB::cntGet( SSess &ses, const string &nm )
{
    map<string,string>::iterator prmEl = ses.cnt.find(nm);
    if( prmEl != ses.cnt.end() ) return prmEl->second;
    return "<empty>";
}

void TWEB::cntrCmdProc( XMLNode *opt )
{
    //-- Get page info --
    if( opt->name() == "info" )
    {
	TUI::cntrCmdProc(opt);
	if(ctrMkNode("area",opt,1,"/prm/cfg",_("Module options")))
	{
	    ctrMkNode("fld",opt,-1,"/prm/cfg/lf_tm",_("Life time of auth sesion(min)"),0660,"root","root",1,"tp","dec");
	    ctrMkNode("fld",opt,-1,"/prm/cfg/CSS",_("CSS"),0660,"root","root",3,"tp","str","cols","90","rows","7");
	}
	ctrMkNode("fld",opt,-1,"/help/g_help",_("Options help"),0440,"root","root",3,"tp","str","cols","90","rows","5");
        return;
    }

    //-- Process command to page --
    string a_path = opt->attr("path");
    if( a_path == "/prm/cfg/lf_tm" )
    {
	if( ctrChkNode(opt,"get",0660,"root","root",SEQ_RD) )	opt->setText( TSYS::int2str(authTime()) );
	if( ctrChkNode(opt,"set",0660,"root","root",SEQ_WR) )	setAuthTime( atoi(opt->text().c_str()) );
    }
    else if( a_path == "/prm/cfg/CSS" )
    {
	if( ctrChkNode(opt,"get",0660,"root","root",SEQ_RD) )	opt->setText( CSStables() );
	if( ctrChkNode(opt,"set",0660,"root","root",SEQ_WR) )	setCSStables( opt->text() );
    }
    else if( a_path == "/help/g_help" && ctrChkNode(opt,"get",0440) )	opt->setText(optDescr());
    else TUI::cntrCmdProc(opt);
}

//*************************************************
//* SSess                                         *
//*************************************************
SSess::SSess( const string &iurl, const string &ipage, const string &isender,
	vector<string> &ivars, const string &icontent ) :
    url(iurl), page(ipage), sender(isender), vars(ivars), content(icontent)
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
