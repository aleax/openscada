
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
#include <string.h>
#include <string>
#include <gd.h>

#include <tsys.h>
#include <tmess.h>
#include <tsecurity.h>

#include "web_cfg.h"

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"WebCfgD"
#define MOD_NAME	_("Dynamic WEB configurator")
#define MOD_TYPE	SUI_ID
#define VER_TYPE	SUI_VER
#define SUB_TYPE	"WWW"
#define MOD_VER		"0.8.0"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("Allow the dynamic WEB based OpenSCADA system configurator. Use XHTML, CSS and JavaScript technologies.")
#define LICENSE		"GPL2"
//*************************************************

WebCfgD::TWEB *WebCfgD::mod;

extern "C"
{
#ifdef MOD_INCL
    TModule::SAt ui_WebCfgD_module( int n_mod )
#else
    TModule::SAt module( int n_mod )
#endif
    {
	if( n_mod==0 )	return TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE);
	return TModule::SAt("");
    }

#ifdef MOD_INCL
    TModule *ui_WebCfgD_attach( const TModule::SAt &AtMod, const string &source )
#else
    TModule *attach( const TModule::SAt &AtMod, const string &source )
#endif
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
TWEB::TWEB( string name ) : TUI(MOD_ID)
{
    mod		= this;

    mName	= MOD_NAME;
    mType	= MOD_TYPE;
    mVers	= MOD_VER;
    mAuthor	= AUTHORS;
    mDescr	= DESCRIPTION;
    mLicense	= LICENSE;
    mSource	= name;

    //> Reg export functions
    modFuncReg( new ExpFunc("void HttpGet(const string&,string&,const string&,vector<string>&,const string&);",
	"Process Get comand from http protocol's!",(void(TModule::*)( )) &TWEB::HttpGet) );
    modFuncReg( new ExpFunc("void HttpPost(const string&,string&,const string&,vector<string>&,const string&);",
	"Process Set comand from http protocol's!",(void(TModule::*)( )) &TWEB::HttpPost) );

    gdFTUseFontConfig(1);

    //> Massages not for compile but for indexing by gettext
#if 0
    char mess[][100] =
    {
	_("OpenSCADA. Dynamic WEB configurator"), _("About"),
	_("Load"), _("Save"), _("Up"), _("Previous"), _("Next"), _("Add item"),_("Delete item"),
	_("Copy item"), _("Cut item"), _("Paste item"),
	_("Reload item and tree"), _("Start periodic update"), _("Stop periodic update"),
	_("Ready"),
	_("Go"), _("Add"), _("Insert"), _("Edit"), _("Delete"), _("Item up"), _("Item down"),
	_("Add new element."), _("Insert new element."), _("Rename element."),
	_("Select image file for download to picture field."),
	_("Add row"), _("Insert row"), _("Delete row"), _("Up row"), _("Down row"),
	_("No one editable container present."), _("Add item to node '%1'."), _("Item '%1' already present."),
	_("You sure for delete node '%1'?"),
	_("Copy is impossible."), _("Selected"), _("Move node '%1' to '%2'."), _("Copy node '%1' to '%2'."), _("Node '%1' already present. Continue?"),
	_("Element type:", "ID:"), _("Name:"), _("Ok"), _("Close"),
	_("Page loaded."),
	_("Node id and/or name select"),
	_("Item name set"),
	_("Image set"),
	_("Add node"),
	_("Move or copy node"),
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

    //> Load parameters from config-file
}

void TWEB::modStart()
{
    run_st = true;
}

void TWEB::modStop()
{
    run_st = false;
}

string TWEB::httpHead( const string &rcode, int cln, const string &cnt_tp, const string &addattr, const string &charset )
{
    return  "HTTP/1.0 "+rcode+"\x0D\x0A"
	    "Server: "+PACKAGE_STRING+"\x0D\x0A"
	    "Accept-Ranges: bytes\x0D\x0A"
	    "Content-Length: "+TSYS::int2str(cln)+"\x0D\x0A"
	    "Content-Type: "+cnt_tp+"; charset="+charset+"\x0D\x0A"
	    "Cache-Control: no-cache\x0D\x0A"+addattr+"\x0D\x0A";
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

void TWEB::imgConvert(SSess &ses)
{
    map<string,string>::iterator prmEl;
    gdImagePtr sim = NULL;
    string itp;
    int newImgH = 0, newImgW = 0;

    if(ses.page.empty() || (ses.prm.find("size") == ses.prm.end() && ses.prm.find("filtr") == ses.prm.end()))	return;

    if((sim=gdImageCreateFromPngPtr(ses.page.size(),(char*)ses.page.data())))		itp = "png";
    else if((sim=gdImageCreateFromJpegPtr(ses.page.size(),(char*)ses.page.data())))	itp = "jpg";
    else if((sim=gdImageCreateFromGifPtr(ses.page.size(),(char*)ses.page.data())))	itp = "gif";
    //if(sim) gdImageAlphaBlending(sim, 0);

    //>> Check for resize icon
    if(sim && (prmEl=ses.prm.find("size")) != ses.prm.end() && (newImgH=atoi(prmEl->second.c_str())) > 0 && gdImageSY(sim) > newImgH)
    {
	newImgW = gdImageSX(sim)*newImgH/gdImageSY(sim);
	gdImagePtr dim = gdImageCreateTrueColor(newImgW,newImgH);
	gdImageAlphaBlending(dim,0);
	gdImageFilledRectangle(dim,0,0,newImgW-1,newImgH-1,gdImageColorResolveAlpha(dim,0,0,0,127));
	gdImageCopyResampled(dim,sim,0,0,0,0,newImgW,newImgH,gdImageSX(sim),gdImageSY(sim));
	gdImageDestroy(sim);
	sim = dim;
    }
    //>> Check for disable icon make
    if(sim && (prmEl = ses.prm.find("filtr")) != ses.prm.end() && (prmEl->second == "gray" || prmEl->second == "unact"))
    {
        gdImagePtr dim = gdImageCreateTrueColor(gdImageSX(sim),gdImageSY(sim));
	gdImageAlphaBlending(dim,0);
	bool isUnAct = (prmEl->second == "unact");
	for(int i_y = 0; i_y < gdImageSY(sim); i_y++)
	    for(int i_x = 0; i_x < gdImageSX(sim); i_x++)
	    {
		int c = gdImageGetPixel(sim,i_x,i_y);
		int y = (int)(0.3*gdImageRed(sim,c)+0.59*gdImageGreen(sim,c)+0.11*gdImageBlue(sim,c));
		if(isUnAct) y = 255-(255-y)/2;
		c = (int)gdImageColorResolveAlpha(dim,y,y,y,gdImageAlpha(sim,c));
		gdImageSetPixel(dim,i_x,i_y,c);
	    }
	gdImageDestroy(sim);
	sim = dim;
    }
    //>> Save result
    if(sim)
    {
	int img_sz;
	char *img_ptr = NULL;
	gdImageSaveAlpha(sim, 1);
	if(itp == "png")	img_ptr = (char *)gdImagePngPtrEx(sim, &img_sz, 1);
	else if(itp == "jpg")	img_ptr = (char *)gdImageJpegPtr(sim, &img_sz, -1);
	else if(itp == "gif")	img_ptr = (char *)gdImageGifPtr(sim, &img_sz);
	if(img_ptr)
	{
	    ses.page.assign(img_ptr,img_sz);
	    gdFree(img_ptr);
	}
	gdImageDestroy(sim);
    }
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
	    ses.page = TUIS::icoGet(((zero_lev == "ico")?"UI."MOD_ID:zero_lev.substr(4)), &itp);
	    imgConvert(ses);
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
		//>> User replace
		size_t varPos = ses.page.find("##USER##");
		if(varPos != string::npos)
		    ses.page.replace(varPos,8,TSYS::strMess("<span style=\"color: %s;\">%s</span>",((user=="root")?"red":"green"),user.c_str()));
		//>> Charset replace
		if((varPos=ses.page.find("##CHARSET##")) != string::npos) ses.page.replace(varPos,11,Mess->charset());

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
		imgConvert(ses);
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
		    for(unsigned i_st = 0; i_st < stls.size(); i_st++)
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
			for(unsigned i_br = 0; brReq.childSize() && i_br < brReq.childGet(0)->childSize(); i_br++)
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
		page = mod->httpHead("200 OK",ses.page.size(),"text/xml","","UTF-8")+ses.page;
		return;
	    }
	    else if(wp_com == "info" || wp_com == "get" || wp_com == "modify")
	    {
		XMLNode req(wp_com); req.setAttr("path",ses.url);
		mod->cntrIfCmd(req,ses.user);
		ses.page = req.save();
		page = mod->httpHead("200 OK",ses.page.size(),"text/xml","","UTF-8")+ses.page;
		return;
	    }
	    else if( wp_com == "img" )
	    {
		string itp = "png";
		XMLNode req("get"); req.setAttr("path",ses.url);
		if( mod->cntrIfCmd(req,ses.user) || atoi(req.attr("rez").c_str()) || req.text().empty() )
		    ses.page = TUIS::icoGet("stop",&itp);
		else
		{
		    ses.page = TSYS::strDecode(req.text(),TSYS::base64);
		    if(req.attr("tp").size()) itp = req.attr("tp");
		}
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
	"<TR><th>"+MOD_ID+" v"+MOD_VER+"</th></TR>\n"
	"<TR><TD class='content'>\n"
	"<table border='0px' cellspacing='3px'>\n"
	"<TR><TD style='color: blue;'>"+_("Name: ")+"</TD><TD>"+_(MOD_NAME)+"</TD></TR>\n"
	"<TR><TD style='color: blue;'>"+_("Description: ")+"</TD><TD>"+_(DESCRIPTION)+"</TD></TR>\n"
	"<TR><TD style='color: blue;'>"+_("License: ")+"</TD><TD>"+_(LICENSE)+"</TD></TR>\n"
	"<TR><TD style='color: blue;'>"+_("Author: ")+"</TD><TD>"+_(AUTHORS)+"</TD></TR>\n"
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
	ses.page = req.save(XMLNode::XMLHeader);
	page = httpHead("200 OK",ses.page.size(),"text/xml","","UTF-8")+ses.page;
    }
    //> Full request to control interface
    else if( wp_com == "req" )
    {
	XMLNode req(""); req.load(ses.content);
	mod->cntrIfCmd(req,ses.user);
	ses.page = req.save(XMLNode::XMLHeader);
	page = httpHead("200 OK",ses.page.size(),"text/xml","","UTF-8")+ses.page;
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

    unsigned txtBeg = 0, i_s, i_r;
    for(i_s = 0; i_s < tsrc.size(); i_s++)
	if( tsrc[i_s] == '#' && tsrc.substr(i_s,3) == "###" && (i_s+3)<tsrc.size() && tsrc[i_s+3] != '#' )
	{
	    for(i_r = i_s+3; i_r < tsrc.size(); i_r++)
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
    try { return SYS->transport().at().cntrIfCmd(node,"UIWebCfg",user); }
    catch(TError err) { node.setAttr("mcat",err.cat)->setAttr("rez","10")->setText(err.mess); }

    return atoi(node.attr("rez").c_str());
}

string TWEB::getCookie( string name, vector<string> &vars )
{
    for( unsigned i_var = 0; i_var < vars.size(); i_var++)
	if( vars[i_var].substr(0, vars[i_var].find(":",0)) == "Cookie" )
	{
	    size_t i_beg = vars[i_var].find(name+"=",0);
	    if( i_beg == string::npos ) return "";
	    i_beg += name.size()+1;
	    return vars[i_var].substr(i_beg,vars[i_var].find(";",i_beg)-i_beg);
	}
    return "";
}

string TWEB::cntGet( SSess &ses, const string &nm )
{
    map<string,string>::iterator prmEl = ses.cnt.find(nm);
    if(prmEl != ses.cnt.end()) return prmEl->second;
    return "<empty>";
}

void TWEB::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if(opt->name() == "info")
    {
	TUI::cntrCmdProc(opt);
	if(ctrMkNode("area",opt,1,"/prm/cfg",_("Module options")))
	    ctrMkNode("comm",opt,-1,"/prm/cfg/host_lnk",_("Go to remote stations list configuration"),RWRW__,"root",SUI_ID,1,"tp","lnk");
	return;
    }

    //> Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/cfg/host_lnk" && ctrChkNode(opt,"get",RWRW__,"root",SUI_ID,SEC_RD)) opt->setText("/Transport");
    else TUI::cntrCmdProc(opt);
}

//*************************************************
//* SSess                                         *
//*************************************************
SSess::SSess( const string &iurl, const string &isender, const string &iuser, vector<string> &ivars, const string &icontent ) :
    url(iurl), sender(isender), user(iuser), content(icontent), vars(ivars)
{
    //> URL parameters parse
    size_t prmSep = iurl.find("?");
    if(prmSep != string::npos)
    {
	url = iurl.substr(0,prmSep);
	string prms = iurl.substr(prmSep+1);
	string sprm;
	for(int iprm = 0; (sprm=TSYS::strSepParse(prms,0,'&',&iprm)).size(); )
	    if((prmSep=sprm.find("=")) == string::npos) prm[sprm] = "true";
	    else prm[sprm.substr(0,prmSep)] = sprm.substr(prmSep+1);
    }

    //> Content parse
    string boundary;
    const char *c_bound = "boundary=";
    const char *c_term = "\x0D\x0A";
    const char *c_end = "--";
    const char *c_fd = "Content-Disposition";
    const char *c_name = "name=\"";
    const char *c_file = "filename=\"";

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
        string p_name, f_name;
        while(pos < content.size())
        {
            string c_head = content.substr(pos, content.find(c_term,pos)-pos);
            pos += c_head.size()+strlen(c_term);
            if(c_head.empty()) break;
            if((spos=c_head.find(":")) == string::npos) return;
            if(c_head.compare(0,spos,c_fd) == 0)
            {
        	if((i_bnd=c_head.find(c_name,spos)) != string::npos)
        	{
            	    i_bnd += strlen(c_name);
            	    p_name = c_head.substr(i_bnd,c_head.find("\"",i_bnd)-i_bnd);
        	}
        	if((i_bnd=c_head.find(c_file,spos)) != string::npos)
        	{
        	    i_bnd += strlen(c_file);
        	    f_name = c_head.substr(i_bnd,c_head.find("\"",i_bnd)-i_bnd);
        	}
	    }
        }
        if(pos >= content.size()) return;
        if(!p_name.empty())
        {
	    cnt[p_name] = content.substr(pos,content.find(string(c_term)+c_end+boundary,pos)-pos);
	    if(!f_name.empty())
	    {
		files[f_name] = cnt[p_name];
		cnt[p_name] = f_name;
	    }
	}
    }
}
