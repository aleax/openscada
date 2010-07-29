
//OpenSCADA system module UI.WebCfgD file: web_cfg.cpp
/***************************************************************************
 *   Copyright (C) 2008-2010 by Roman Savochenko                           *
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
#define MOD_VERSION	"0.6.4"
#define AUTORS		"Roman Savochenko"
#define DESCRIPTION	"Allow the dynamic WEB based OpenSCADA system configurator. Use XHTML, CSS and JavaScript technologies."
#define LICENSE		"GPL2"
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

extern char *WebCfgDVCA_html;
extern char *WebCfgDVCA_js;

using namespace WebCfgD;

//*************************************************
//* TWEB                                          *
//*************************************************
TWEB::TWEB( string name )
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

    //> Reg export functions
    modFuncReg( new ExpFunc("void HttpGet(const string&,string&,const string&,vector<string>&,const string&);",
	"Process Get comand from http protocol's!",(void(TModule::*)( )) &TWEB::HttpGet) );
    modFuncReg( new ExpFunc("void HttpPost(const string&,string&,const string&,vector<string>&,const string&);",
	"Process Set comand from http protocol's!",(void(TModule::*)( )) &TWEB::HttpPost) );

    //> Massages not for compile but for indexing by gettext
#if 0
    char mess[][100] =
    {
	"OpenSCADA. Dynamic WEB configurator", "About",
	"Load", "Save", "Up", "Previous", "Next", "Add item","Delete item",
	"Copy item", "Cut item", "Paste item",
	"Reload item and tree", "Start periodic update", "Stop periodic update",
	"Ready",
	"Go", "Add", "Insert", "Edit", "Delete", "Item up", "Item down",
	"Add new element.", "Insert new element.", "Rename element.",
	"Select image file for download to picture field.",
	"Add row", "Insert row", "Delete row", "Up row", "Down row",
	"No one editable container present.", "Add item to node '%1'.", "Item '%1' already present.",
	"You sure for delete node '%1'?",
	"Copy is imposible.", "Selected", "Move node '%1' to '%2'.", "Copy node '%1' to '%2'.", "Node '%1' already present. Continue?",
	"Element type:", "ID:", "Name:", "Ok", "Close",
	"Page loaded."
    };
#endif
}

TWEB::~TWEB()
{

}

string TWEB::modInfo( const string &name )
{
    if( name == "SubType" )	return SUB_TYPE;
    else if( name == "Auth" )	return "1";
    else return TModule::modInfo(name);
}

void TWEB::modInfo( vector<string> &list )
{
    TModule::modInfo(list);
    list.push_back("SubType");
    list.push_back("Auth");
}

void TWEB::load_( )
{
    //> Load parameters from command line

    //> Load parameters from config file
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
    return  "HTTP/1.0 "+rcode+"\r\n"
	    "Server: "+PACKAGE_STRING+"\r\n"
	    "Accept-Ranges: bytes\r\n"
	    "Content-Length: "+TSYS::int2str(cln)+"\r\n"
	    "Content-Type: "+cnt_tp+";charset="+Mess->charset()+"\r\n"
	    "Cache-Control: no-cache\r\n"+addattr+"\r\n";
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
	"  <style type='text/css'>\n"
	"    hr { width: 95%; }\n"
	"    body { background-color: #818181; }\n"
	"    h1.head { text-align: center; color:#ffff00; }\n"
	"    h2.title { text-align: center; font-style: italic; margin: 0px; padding: 0px; border-width: 0px; }\n"
	"    table.work { background-color: #9999ff; border: 3px ridge #a9a9a9; padding: 2px; }\n"
	"    table.work td { background-color:#cccccc; text-align: left; }\n"
	"    table.work td.content { padding: 5px; padding-bottom: 20px; }\n"
	"    table.work ul { margin: 0px; padding: 0px; padding-left: 20px; }\n"
	"  </style>\n"
	"</head>\n"
	"<body>\n";
}

string TWEB::pgTail( )
{
    return "</body>\n</html>";
}

void TWEB::HttpGet( const string &urli, string &page, const string &sender, vector<string> &vars, const string &user )
{
    map<string,string>::iterator prmEl;
    SSess ses(TSYS::strDecode(urli,TSYS::HttpURL),sender,user,vars,"");
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
	    prmEl = ses.prm.find("com");
	    string wp_com = (prmEl!=ses.prm.end()) ? prmEl->second : "";

	    if( wp_com.empty() && zero_lev == "script.js" )
	    {
		ses.page = trMessReplace(WebCfgDVCA_js);
		page = httpHead("200 OK",ses.page.size(),"text/javascript")+ses.page;
		return;
	    }
	    //> Main work page create.
	    else if( wp_com.empty() )
	    {
		ses.page = trMessReplace(WebCfgDVCA_html);
		page = httpHead("200 OK",ses.page.size(),"text/html")+ses.page;
		return;
	    }
	    //> Get node icon
	    else if( wp_com == "ico" )
	    {
		string itp = "png";
		XMLNode req("get"); req.setAttr("path",ses.url+"/%2fico");
		if( !mod->cntrIfCmd(req,ses.user) )
		    ses.page = TSYS::strDecode(req.text(),TSYS::base64);
		else ses.page = TUIS::icoGet("disconnect",&itp);
		page = httpHead("200 OK",ses.page.size(),string("image/")+itp)+ses.page;
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
		page = mod->httpHead("200 OK",ses.page.size(),"text/xml; charset=UTF-8")+ses.page;
		return;
	    }
	    else if( wp_com == "info" )
	    {
		XMLNode req("info"); req.setAttr("path",ses.url);
		mod->cntrIfCmd(req,ses.user);
		ses.page = req.save();
		page = mod->httpHead("200 OK",ses.page.size(),"text/xml; charset=UTF-8")+ses.page;
		return;
	    }
	    else if( wp_com == "modify" )
	    {
		XMLNode req("modify"); req.setAttr("path",ses.url+"/%2fobj");
		mod->cntrIfCmd(req,ses.user);
		ses.page = req.save();
		page = mod->httpHead("200 OK",ses.page.size(),"text/xml; charset=UTF-8")+ses.page;
		return;
	    }
	    else if( wp_com == "get" )
	    {
		XMLNode req("get"); req.setAttr("path",ses.url);
		mod->cntrIfCmd(req,ses.user);
		ses.page = req.save();
		page = mod->httpHead("200 OK",ses.page.size(),"text/xml; charset=UTF-8")+ses.page;
		return;
	    }
	    else if( wp_com == "img" )
	    {
		string itp = "png";
		XMLNode req("get"); req.setAttr("path",ses.url);
		if( mod->cntrIfCmd(req,ses.user) || atoi(req.attr("rez").c_str()) || req.text().empty() )
		    ses.page = TUIS::icoGet("stop",&itp);
		else ses.page = TSYS::strDecode(req.text(),TSYS::base64);
		page = mod->httpHead("200 OK",ses.page.size(),"image/"+itp)+ses.page;
		return;
	    }
	    else
	    {
		mess_warning(nodePath().c_str(),_("Unknown command: %s."),wp_com.c_str());
		ses.page = ses.page+"<center>Call page/widget '"+ses.url+"' command: '"+wp_com+"'</center>\n<br/>";
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
	"<table class='work'>\n"
	"<TR><th>"+MOD_ID+" v"+MOD_VERSION+"</th></TR>\n"
	"<TR><TD class='content'>\n"
	"<table border='0px' cellspacing='3px'>\n"
	"<TR><TD style='color: blue;'>"+_("Name: ")+"</TD><TD>"+_(MOD_NAME)+"</TD></TR>\n"
	"<TR><TD style='color: blue;'>"+_("Desription: ")+"</TD><TD>"+_(DESCRIPTION)+"</TD></TR>\n"
	"<TR><TD style='color: blue;'>"+_("License: ")+"</TD><TD>"+_(LICENSE)+"</TD></TR>\n"
	"<TR><TD style='color: blue;'>"+_("Author: ")+"</TD><TD>"+_(AUTORS)+"</TD></TR>\n"
	"</table>\n"
	"</TD></TR>\n</table><br/>\n"
	"<table class='work'>\n"
	"<TR><th>"+PACKAGE+" v"+VERSION+"</th></TR>\n"
	"<TR><TD class='content'>\n"
	"<table border='0' cellspacing='3px'>\n"
	"<TR><TD style='color: blue;'>"+_("Name: ")+"</TD><TD>"+_(PACKAGE_DESCR)+"</TD></TR>\n"
	"<TR><TD style='color: blue;'>"+_("License: ")+"</TD><TD>"+PACKAGE_LICENSE+"</TD></TR>\n"
	"<TR><TD style='color: blue;'>"+_("Author: ")+"</TD><TD>"+_(PACKAGE_AUTHOR)+"</TD></TR>\n"
	"<TR><TD style='color: blue;'>"+_("Web site: ")+"</TD><TD>"+PACKAGE_SITE+"</TD></TR>\n"
	"</table>\n"
	"</TD></TR></table><br/>\n"
	"</center>\n";
}

void TWEB::HttpPost( const string &url, string &page, const string &sender, vector<string> &vars, const string &user )
{
    map<string,string>::iterator cntEl;
    SSess ses(TSYS::strDecode(url,TSYS::HttpURL),sender,user,vars,page);

    //> Commands process
    cntEl = ses.prm.find("com");
    string wp_com = (cntEl!=ses.prm.end()) ? cntEl->second : "";
    //> Attributes set
    if( wp_com == "com" )
    {
	XMLNode req(""); req.load(ses.content); req.setAttr("path",ses.url);
	mod->cntrIfCmd(req,ses.user);
	ses.page = req.save();
	page = httpHead("200 OK",ses.page.size(),"text/xml; charset=UTF-8")+ses.page;
    }
    else if( wp_com == "img" )
    {
	if( (cntEl=ses.cnt.find("name")) != ses.cnt.end() && !ses.files[cntEl->second].empty() )
	{
	    XMLNode req("set"); req.setAttr("path",ses.url)->setText(TSYS::strEncode(ses.files[cntEl->second],TSYS::base64));
	    mod->cntrIfCmd(req,ses.user);
	}
	page = httpHead("204 No Content",0,"");
    }
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

int TWEB::cntrIfCmd( XMLNode &node, const string &user )
{
    try
    {
	return SYS->transport().at().cntrIfCmd(node,"UIWebCfg",user);
    }catch( TError err )
    { node.setAttr("mcat",err.cat)->setAttr("rez","10")->setText(err.mess); }

    return atoi(node.attr("rez").c_str());
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
    //> Get page info
    if( opt->name() == "info" )
    {
	TUI::cntrCmdProc(opt);
	if(ctrMkNode("area",opt,1,"/prm/cfg",_("Module options")))
	    ctrMkNode("comm",opt,-1,"/prm/cfg/host_lnk",_("Go to remote stations list configuration"),0660,"root","root",1,"tp","lnk");
        return;
    }

    //> Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/prm/cfg/host_lnk" && ctrChkNode(opt,"get",0660,"root","root",SEC_RD) )
    {
	SYS->transport().at().setSysHost(false);
	opt->setText("/Transport");
    }
    else TUI::cntrCmdProc(opt);
}

//*************************************************
//* SSess                                         *
//*************************************************
SSess::SSess( const string &iurl, const string &isender, const string &iuser,
	vector<string> &ivars, const string &icontent ) :
    url(iurl), sender(isender), user(iuser), vars(ivars), content(icontent)
{
    //> URL parameters parse
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
    if( boundary.empty() ) return;

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
	    if( pos >= content.size() ) break;
	    cnt[c_name] = content.substr(pos, content.find(string(c_term)+c_end+boundary,pos)-pos);
	}
	else
	{
	    i_bnd += strlen(c_file);
	    cnt[c_name] = c_head.substr(i_bnd,c_head.find("\"",i_bnd)-i_bnd);

	    //>>>> Get file data
	    pos += c_head.size()+strlen(c_term);
	    if( pos >= content.size() ) break;
	    if( content.substr(pos,12) == "Content-Type" )
	    {
		string imgType = content.substr(pos,content.find(c_term,pos)-pos);
		pos += imgType.size()+(2*strlen(c_term));
		if( pos >= content.size() ) break;
	    }else pos += strlen(c_term);
	    files[cnt[c_name]] = content.substr(pos,content.find(string(c_term)+c_end+boundary,pos)-pos);
	}
    }
}
