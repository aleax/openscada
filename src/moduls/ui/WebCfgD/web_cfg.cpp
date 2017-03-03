
//OpenSCADA system module UI.WebCfgD file: web_cfg.cpp
/***************************************************************************
 *   Copyright (C) 2008-2017 by Roman Savochenko, <rom_as@oscada.org>      *
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
#include <time.h>
#include <string.h>
#include <string>
#if HAVE_GD_CORE
#include <gd.h>
#endif

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
#define MOD_VER		"1.2.0"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("Provides dynamic WEB based configurator. Uses XHTML, CSS and JavaScript technology.")
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
	if(n_mod == 0)	return TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE);
	return TModule::SAt("");
    }

#ifdef MOD_INCL
    TModule *ui_WebCfgD_attach( const TModule::SAt &AtMod, const string &source )
#else
    TModule *attach( const TModule::SAt &AtMod, const string &source )
#endif
    {
	if(AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE)) return new WebCfgD::TWEB(source);
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
    mod = this;

    modInfoMainSet(MOD_NAME, MOD_TYPE, MOD_VER, AUTHORS, DESCRIPTION, LICENSE, name);

    //Export functions registration
    modFuncReg(new ExpFunc("void HTTP_GET(const string&,string&,vector<string>&,const string&,TProtocolIn*);",
	"GET command processing from HTTP protocol!",(void(TModule::*)( )) &TWEB::HTTP_GET));
    modFuncReg(new ExpFunc("void HTTP_POST(const string&,string&,vector<string>&,const string&,TProtocolIn*);",
	"POST command processing from HTTP protocol!",(void(TModule::*)( )) &TWEB::HTTP_POST));

#if HAVE_GD_CORE
    gdFTUseFontConfig(1);
#endif

    //Massages not for compile but for indexing by gettext
#if 0
    char mess[][100] = {
	_("Manual"), _("About"), _("Drag to resize the Menu"),
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

string TWEB::pgCreator( TProtocolIn *iprt, const string &cnt, const string &rcode, const string &httpattrs,
    const string &htmlHeadEls, const string &forceTmplFile )
{
    vector<TVariant> prms;
    prms.push_back(cnt); prms.push_back(rcode); prms.push_back(httpattrs); prms.push_back(htmlHeadEls); prms.push_back(forceTmplFile);

    return iprt->owner().objFuncCall("pgCreator", prms, "root").getS();
}

void TWEB::imgConvert( SSess &ses, string &vl )
{
#if HAVE_GD_CORE
    map<string,string>::iterator prmEl;
    gdImagePtr sim = NULL;
    string itp;
    int newImgH = 0, newImgW = 0;

    if(vl.empty() || (ses.prm.find("size") == ses.prm.end() && ses.prm.find("filtr") == ses.prm.end()))	return;

    if((sim=gdImageCreateFromPngPtr(vl.size(),(char*)vl.data())))		itp = "png";
    else if((sim=gdImageCreateFromJpegPtr(vl.size(),(char*)vl.data())))	itp = "jpg";
    else if((sim=gdImageCreateFromGifPtr(vl.size(),(char*)vl.data())))	itp = "gif";
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
    //Check for disable icon make
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
    //Save result
    if(sim) {
	int img_sz;
	char *img_ptr = NULL;
	gdImageSaveAlpha(sim, 1);
	if(itp == "png")	img_ptr = (char *)gdImagePngPtrEx(sim, &img_sz, 1);
	else if(itp == "jpg")	img_ptr = (char *)gdImageJpegPtr(sim, &img_sz, -1);
	else if(itp == "gif")	img_ptr = (char *)gdImageGifPtr(sim, &img_sz);
	if(img_ptr) {
	    vl.assign(img_ptr,img_sz);
	    gdFree(img_ptr);
	}
	gdImageDestroy(sim);
    }
#endif
}

void TWEB::HTTP_GET( const string &urli, string &page, vector<string> &vars, const string &user, TProtocolIn *iprt )
{
    map<string,string>::iterator prmEl;
    SSess ses(TSYS::strDecode(urli,TSYS::HttpURL), TSYS::strLine(iprt->srcAddr(),0), user, vars, "");

    try {
	string zero_lev = TSYS::pathLev(ses.url, 0);

	//Get about module page
	if(zero_lev == "about")	{	//getAbout(ses);
	    page = pgCreator(iprt, "<table class='work'>\n"
		" <tr><th><a href='"+TUIS::docGet("|" MOD_ID,NULL,TUIS::GetPathURL)+"'>" MOD_ID " v" MOD_VER "</a></th></tr>\n"
		" <tr><td class='content'>\n"
		"  <table border='0px' cellspacing='3px'>\n"
		"   <tr><td style='color: blue;'>" + _("Name: ") + "</td><td>" + _(MOD_NAME) + "</TD></TR>\n"
		"   <tr><td style='color: blue;'>" + _("Description: ") + "</td><td>" + _(DESCRIPTION) + "</td></tr>\n"
		"   <tr><td style='color: blue;'>" + _("License: ") + "</td><td>" + _(LICENSE) + "</td></tr>\n"
		"   <tr><td style='color: blue;'>" + _("Author: ") + "</td><td>" + _(AUTHORS) + "</td></tr>\n"
		"  </table>\n"
		" </td></tr>\n"
		"</table><br/>\n"
		"<table class='work'>\n"
		" <tr><th>" PACKAGE " v" VERSION "</th></tr>\n"
		" <tr><td class='content'>\n"
		"  <table border='0' cellspacing='3px'>\n"
		"   <tr><td style='color: blue;'>" + _("Name: ") + "</td><td>" + _(PACKAGE_DESCR) + "</td></tr>\n"
		"   <tr><td style='color: blue;'>" + _("License: ") + "</td><td>" PACKAGE_LICENSE "</td></tr>\n"
		"   <tr><td style='color: blue;'>" + _("Author: ") + "</td><td>" + _(PACKAGE_AUTHOR) + "</td></tr>\n"
		"   <tr><td style='color: blue;'>" + _("Web site: ") + "</td><td><a href='" PACKAGE_SITE "'>" PACKAGE_SITE "</a></td></tr>\n"
		"  </table>\n"
		" </td></tr>\n"
		"</table><br/>\n", "200 OK");
	}
	//Get module icon and global image
	else if(zero_lev == "ico" || zero_lev.substr(0,4) == "img_") {
	    string itp;
	    page = TUIS::icoGet(((zero_lev == "ico")?"UI." MOD_ID:zero_lev.substr(4)), &itp);
	    imgConvert(ses, page);
	    page = pgCreator(iprt, page, "200 OK", "Content-Type: image/"+itp+";");
	}
	else if(zero_lev == "doc") {
	    XMLNode req(zero_lev); req.setText(TUIS::docGet("|"+TSYS::pathLev(ses.url,1),NULL,TUIS::GetPathURL));
	    page = pgCreator(iprt, req.save(), "200 OK", "Content-Type: text/xml;charset=UTF-8");
	}
	else {
	    prmEl = ses.prm.find("com");
	    int hd;
	    string wp_com = (prmEl!=ses.prm.end()) ? prmEl->second : "";
	    page = "";

	    if(wp_com.empty() && zero_lev == "script.js") {
		if((hd=open("WebCfgDVCA.js",O_RDONLY)) >= 0) {
		    char buf[STR_BUF_LEN];
		    for(int len = 0; (len=read(hd,buf,sizeof(buf))) > 0; ) page.append(buf, len);
		    close(hd);
		    page = trMessReplace(page);
		}
		else page = trMessReplace(WebCfgDVCA_js);
		page = pgCreator(iprt, page, "200 OK", "Content-Type: text/javascript;");
	    }
	    //Main work page create.
	    else if(wp_com.empty()) {
		if((hd=open("WebCfgDVCA.html",O_RDONLY)) >= 0) {
		    char buf[STR_BUF_LEN];
		    for(int len = 0; (len=read(hd,buf,sizeof(buf))) > 0; ) page.append(buf, len);
		    close(hd);
		    page = trMessReplace(page);
		}
		else page = trMessReplace(WebCfgDVCA_html);
		// User replace
		size_t varPos = page.find("##USER##");
		if(varPos != string::npos)
		    page.replace(varPos, 8, TSYS::strMess("<span style=\"color: %s;\">%s</span>",((user=="root")?"red":"green"),user.c_str()));
		// Charset replace
		if((varPos=page.find("##CHARSET##")) != string::npos)
		    page.replace(varPos, 11, Mess->charset());
		// Title replace
		if((varPos=page.find("##TITLE##")) != string::npos)
		    page.replace(varPos, 9, PACKAGE_NAME " " MOD_ID ": "+SYS->name());

		page = pgCreator(iprt, page, "200 OK", "Content-Type:text/html;charset=UTF-8");
	    }
	    //Get node icon
	    else if(wp_com == "ico") {
		string itp = "png";
		XMLNode req("get"); req.setAttr("path",ses.url+"/%2fico");
		if(!mod->cntrIfCmd(req,ses.user))
		    page = TSYS::strDecode(req.text(), TSYS::base64);
		else page = TUIS::icoGet("disconnect", &itp);
		imgConvert(ses, page);
		page = pgCreator(iprt, page, "200 OK", "Content-Type: image/"+itp+";");
	    }
	    //Get node's childs
	    else if(wp_com == "chlds") {
		XMLNode req("chlds");
		prmEl = ses.prm.find("grp");
		string gbr = (prmEl!=ses.prm.end()) ? prmEl->second : "";
		// Get information about allow stations
		if(zero_lev.empty()) {
		    vector<TTransportS::ExtHost> stls;
		    SYS->transport().at().extHostList(ses.user, stls);
		    stls.insert(stls.begin(), TTransportS::ExtHost("",SYS->id()));
		    for(unsigned iSt = 0; iSt < stls.size(); iSt++) {
			XMLNode *chN = req.childAdd("el")->setAttr("id",stls[iSt].id)->
							   setText((stls[iSt].id==SYS->id())?SYS->name():stls[iSt].name);
			//  Check icon
			XMLNode reqIco("get"); reqIco.setAttr("path","/"+stls[iSt].id+"/%2fico");
			if(mod->cntrIfCmd(reqIco,ses.user)) chN->setAttr("icoSize","1000");
			else chN->setAttr("icoSize",i2s(reqIco.text().size()));
			//  Process groups
			XMLNode brReq("info"); brReq.setAttr("path","/"+SYS->id()+"/%2fbr");
			mod->cntrIfCmd(brReq,ses.user);
			for(unsigned i_br = 0; brReq.childSize() && i_br < brReq.childGet(0)->childSize(); i_br++) {
			    XMLNode *chB = chN->childAdd();
			    *chB = *brReq.childGet(0)->childGet(i_br);
			    chB->setAttr("chPresent", "1");
			}
		    }
		}
		else {
		    req.setAttr("path",ses.url+"/%2fobj")->setAttr("grp",gbr)->setAttr("icoCheck","1");
		    mod->cntrIfCmd(req, ses.user);
		}
		page = pgCreator(iprt, req.save(), "200 OK", "Content-Type: text/xml;charset=UTF-8");
	    }
	    else if(wp_com == "info" || wp_com == "get" || wp_com == "modify") {
		XMLNode req(wp_com); req.setAttr("path", ses.url);
		mod->cntrIfCmd(req, ses.user);
		page = pgCreator(iprt, req.save(), "200 OK", "Content-Type: text/xml;charset=UTF-8");
	    }
	    else if(wp_com == "img") {
		string itp = "png";
		XMLNode req("get"); req.setAttr("path", ses.url);
		if(mod->cntrIfCmd(req,ses.user) || s2i(req.attr("rez")) || req.text().empty())
		    page = TUIS::icoGet("stop", &itp);
		else {
		    page = TSYS::strDecode(req.text(),TSYS::base64);
		    if(req.attr("tp").size()) itp = req.attr("tp");
		}
		page = pgCreator(iprt, page, "200 OK", "Content-Type: image/"+itp+";");
	    }
	    else {
		mess_warning(nodePath().c_str(), _("Unknown command: %s."), wp_com.c_str());
		page = pgCreator(iprt, "<div class='error'>"+TSYS::strMess(_("Call page/widget '%s' command '%s'!"),
								ses.url.c_str(),wp_com.c_str())+"</div>\n",
				       "200 OK");
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
    SSess ses(TSYS::strDecode(url,TSYS::HttpURL), TSYS::strLine(iprt->srcAddr(),0), user, vars, page);

    //Commands process
    cntEl = ses.prm.find("com");
    string wp_com = (cntEl!=ses.prm.end()) ? cntEl->second : "";
    //Attributes set
    if(wp_com == "com") {
	XMLNode req(""); req.load(ses.content); req.setAttr("path", ses.url);
	mod->cntrIfCmd(req, ses.user);
	page = pgCreator(iprt, req.save(XMLNode::XMLHeader), "200 OK", "Content-Type: text/xml;charset=UTF-8");
    }
    //Full request to control interface
    else if(wp_com == "req") {
	XMLNode req(""); req.load(ses.content);
	mod->cntrIfCmd(req,ses.user);
	page = pgCreator(iprt, req.save(XMLNode::XMLHeader), "200 OK", "Content-Type: text/xml;charset=UTF-8");
    }
    else if(wp_com == "img") {
	if((cntEl=ses.cnt.find("name")) != ses.cnt.end() && !ses.files[cntEl->second].empty()) {
	    XMLNode req("set"); req.setAttr("path",ses.url)->setText(TSYS::strEncode(ses.files[cntEl->second],TSYS::base64));
	    mod->cntrIfCmd(req,ses.user);
	}
	page = pgCreator(iprt, string("<div class='error'>")+_("No a content.")+"</div>\n", "204 No Content");
    }
}

string TWEB::trMessReplace( const string &tsrc )
{
    string trez; trez.reserve(tsrc.size());

    unsigned txtBeg = 0, i_s, i_r;
    for(i_s = 0; i_s < tsrc.size(); i_s++)
	if(tsrc[i_s] == '#' && tsrc.substr(i_s,3) == "###" && (i_s+3) < tsrc.size() && tsrc[i_s+3] != '#') {
	    for(i_r = i_s+3; i_r < tsrc.size(); i_r++)
		if((tsrc[i_r] == '#' && tsrc.substr(i_r,3) == "###" && ((i_r+3)>=tsrc.size() || tsrc[i_r+3] != '#')) ||
			tsrc[i_r] == '\n')
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

int TWEB::cntrIfCmd( XMLNode &node, const string &user )
{
    //Mark commands in "primaryCmd", for redundant hosts mostly transfer
    // !!! Move further to the command's source
    if(node.name() == "set" || node.name() == "add" || node.name() == "ins" || node.name() == "del" || node.name() == "move" ||
	    node.name() == "load" || node.name() == "save" || node.name() == "copy")
	node.setAttr("primaryCmd", "1");
    try { return SYS->transport().at().cntrIfCmd(node,"UIWebCfg",user); }
    catch(TError &err) { node.setAttr("mcat",err.cat)->setAttr("rez","10")->setText(err.mess); }

    return s2i(node.attr("rez"));
}

void TWEB::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TUI::cntrCmdProc(opt);
	if(ctrMkNode("area",opt,1,"/prm/cfg",_("Module options")))
	    ctrMkNode("comm",opt,-1,"/prm/cfg/host_lnk",_("Go to remote stations list configuration"),RWRW__,"root",SUI_ID,1,"tp","lnk");
	return;
    }

    //Process command to page
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
    const char *c_file = "filename=\"";

    for(size_t i_vr = 0, pos = 0; i_vr < vars.size() && boundary.empty(); i_vr++)
	if(vars[i_vr].compare(0,vars[i_vr].find(":",0),"Content-Type") == 0 &&
		(pos=vars[i_vr].find(c_bound,0)) != string::npos) {
	    pos += strlen(c_bound);
	    boundary = vars[i_vr].substr(pos,vars[i_vr].size()-pos);
	}
    if(boundary.empty()) return;

    for(size_t pos = 0, spos = 0, i_bnd = 0; true; ) {
	pos = content.find(boundary,pos);
	if(pos == string::npos || content.compare(pos+boundary.size(),2,c_end) == 0) break;
	pos += boundary.size()+strlen(c_term);

	// Process properties and get name
	string p_name, f_name;
	while(pos < content.size()) {
	    string c_head = content.substr(pos, content.find(c_term,pos)-pos);
	    pos += c_head.size()+strlen(c_term);
	    if(c_head.empty()) break;
	    if((spos=c_head.find(":")) == string::npos) return;
	    if(c_head.compare(0,spos,c_fd) == 0) {
		if((i_bnd=c_head.find(c_name,spos)) != string::npos) {
		    i_bnd += strlen(c_name);
		    p_name = c_head.substr(i_bnd,c_head.find("\"",i_bnd)-i_bnd);
		}
		if((i_bnd=c_head.find(c_file,spos)) != string::npos) {
		    i_bnd += strlen(c_file);
		    f_name = c_head.substr(i_bnd,c_head.find("\"",i_bnd)-i_bnd);
		}
	    }
	}
	if(pos >= content.size()) return;
	if(!p_name.empty()) {
	    cnt[p_name] = content.substr(pos,content.find(string(c_term)+c_end+boundary,pos)-pos);
	    if(!f_name.empty()) {
		files[f_name] = cnt[p_name];
		cnt[p_name] = f_name;
	    }
	}
    }
}
