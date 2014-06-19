
//!!! Module name, file name and module's license. Change for your need.
//OpenSCADA system module UI.Tmpl file: module.cpp
/***************************************************************************
 *   Copyright (C) 2012 by MyName MyFamily                                 *
 *   my@email.org                                                          *
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

//!!! System's includings. Add need for your module includings.
#include <string>

//!!! OpenSCADA module's API includings. Add need for your module includings.
#include <tsys.h>
#include <tmess.h>

//!!! Self your module's includings. Add need for your module includings.
#include "module.h"

//!!! Module's meta-information. Change for your module.
//************************************************
//* Modul info!                                  *
#define MOD_ID		"Tmpl"
#define MOD_NAME	_("UI template")
#define MOD_TYPE	SUI_ID
#define VER_TYPE	SUI_VER
#define SUB_TYPE	"WWW"
#define MOD_VER		"0.0.1"
#define AUTHORS		_("MyName MyFamily")
#define DESCRIPTION	_("UI subsystem template module.")
#define LICENSE		"MyLicense"
//************************************************

ModTmpl::TWEB *ModTmpl::mod;

//!!! Required section for binding OpenSCADA kernel's to this module. Gives information and create module's root object.
//!!! Not remove this section!
extern "C"
{
#ifdef MOD_INCL
    TModule::SAt ui_Tmpl_module( int n_mod )
#else
    TModule::SAt module( int n_mod )
#endif
    {
	if( n_mod==0 )	return TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE);
	return TModule::SAt("");
    }

#ifdef MOD_INCL
    TModule *ui_Tmpl_attach( const TModule::SAt &AtMod, const string &source )
#else
    TModule *attach( const TModule::SAt &AtMod, const string &source )
#endif
    {
	if( AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE) )
	    return new ModTmpl::TWEB( source );
	return NULL;
    }
}

//!!! Include for default call into your module's namespace.
using namespace ModTmpl;

//************************************************
//* TWEB                                         *
//************************************************
//!!! Constructor for module's root object. Append into for your need.
TWEB::TWEB( string name ) : TUI(MOD_ID)
{
    //!!! Init shortcut to module's root object. No change it.
    mod		= this;

    //!!! Loading module's meta-information into root object. No change it.
    mName	= MOD_NAME;
    mType	= MOD_TYPE;
    mVers	= MOD_VER;
    mAuthor	= AUTHORS;
    mDescr	= DESCRIPTION;
    mLicense	= LICENSE;
    mSource	= name;

    //!!! Register your module's export functions. Used, for example, for call from HTTP module, and use this OpenSCADA module as HTTP user interface module.
    //> Reg export functions
    modFuncReg( new ExpFunc("void HttpGet(const string&,string&,const string&,vector<string>&);",
	"Process Get comand from http protocol's!",(void(TModule::*)( )) &TWEB::HttpGet) );
    modFuncReg( new ExpFunc("void HttpPost(const string&,string&,const string&,vector<string>&,const string&);",
	"Process Set comand from http protocol's!",(void(TModule::*)( )) &TWEB::HttpPost) );
}

//!!! Destructor for module's root object. Append into for your need.
TWEB::~TWEB( )
{

}

//!!! Create append module's info attributes. In this example add attribute "SubType" for HTTP module allow bind inform.
void TWEB::modInfo( vector<string> &list )
{
    TModule::modInfo(list);
    list.push_back("SubType");
}

//!!! Get an append module's info attribute. In this example get attribute "SubType" for HTTP module allow bind inform.
string TWEB::modInfo( const string &name )
{
    if( name == "SubType" ) return SUB_TYPE;
    else return TModule::modInfo(name);
}

//!!! Module's comandline options the print help function. Add your module commandline parameters info.
string TWEB::optDescr( )
{
    char buf[STR_BUF_LEN];

    snprintf(buf,sizeof(buf),_(
	"======================= The module <%s:%s> options =======================\n"
	"---------- Parameters of the module section '%s' in config-file ----------\n\n"),
	MOD_TYPE,MOD_ID,nodePath().c_str());

    return buf;
}

//!!! Inherited (virtual) load object's node method. Call from OpenSCADA kernel. Append your module need data loadings
void TWEB::load_( )
{
    //!!! Load self module command line parameters' values. Append your addition parameters process.
    //> Load parameters from command line
    string argCom, argVl;
    for(int argPos = 0; (argCom=SYS->getCmdOpt(argPos,&argVl)).size(); )
        if(argCom == "h" || argCom == "help")	fprintf(stdout,"%s",optDescr().c_str());

    //!!! Load addition your module specific data. For example, make loading addition module's parameters from OpenSCADA system DB or from main config-file.
    //> Load parameters from config-file
    string trnds = TBDS::genDBGet(nodePath()+"Trends"), trnd_el;
    trnd_lst.clear();
    for( int el_off = 0; (trnd_el=TSYS::strSepParse(trnds,0,';',&el_off)).size(); )
	trnd_lst.push_back(trnd_el);
    n_col = atoi(TBDS::genDBGet(nodePath()+"n_col",TSYS::int2str(n_col)).c_str());
    h_sz = atoi(TBDS::genDBGet(nodePath()+"h_sz",TSYS::int2str(h_sz)).c_str());
    v_sz = atoi(TBDS::genDBGet(nodePath()+"v_sz",TSYS::int2str(v_sz)).c_str());
    trnd_len = atoi(TBDS::genDBGet(nodePath()+"trnd_len",TSYS::int2str(trnd_len)).c_str());
    trnd_tm  = atoi(TBDS::genDBGet(nodePath()+"trnd_tm",TSYS::int2str(trnd_tm)).c_str());
}

//!!! Inherited (virtual) save object's node method. Call from OpenSCADA kernel. Append your module need data savings
void TWEB::save_( )
{
    //!!! Save addition your module specific data. For example, make saving addition module's parameters to OpenSCADA system DB.
    //> Save parameters to config-file
    string trnds;
    for(int i_el = 0; i_el < trnd_lst.size(); i_el++ )
	trnds+=trnd_lst[i_el]+";";
    TBDS::genDBSet(nodePath()+"Trends",trnds);
    TBDS::genDBSet(nodePath()+"n_col",TSYS::int2str(n_col));
    TBDS::genDBSet(nodePath()+"h_sz",TSYS::int2str(h_sz));
    TBDS::genDBSet(nodePath()+"v_sz",TSYS::int2str(v_sz));
    TBDS::genDBSet(nodePath()+"trnd_len",TSYS::int2str(trnd_len));
    TBDS::genDBSet(nodePath()+"trnd_tm",TSYS::int2str(trnd_tm));
}

//!!! OpenSCADA control interface comands process virtual function.
//!!! For example, process access from standard confifurators of OpenSCADA to individual module's parameters.
//!!! Modify for self needs
void TWEB::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if(opt->name() == "info")
    {
	TUI::cntrCmdProc(opt);
	if(ctrMkNode("area",opt,1,"/prm/cfg",_("Module options")))
	{
	    ctrMkNode("list",opt,-1,"/prm/cfg/trnds",_("Display parameter attributes trends"),RWRWR_,"root",SUI_ID,1,"s_com","add,del");
	    ctrMkNode("fld",opt,-1,"/prm/cfg/col",_("Collums"),RWRWR_,"root",SUI_ID,1,"tp","dec");
	    ctrMkNode("fld",opt,-1,"/prm/cfg/hsize",_("Horizontal trend size (pixel)"),RWRWR_,"root",SUI_ID,1,"tp","dec");
	    ctrMkNode("fld",opt,-1,"/prm/cfg/vsize",_("Vertical trend size (pixel)"),RWRWR_,"root",SUI_ID,1,"tp","dec");
	    ctrMkNode("fld",opt,-1,"/prm/cfg/trnd_tm",_("Trend start time (sec)"),RWRWR_,"root",SUI_ID,1,"tp","time");
	    ctrMkNode("fld",opt,-1,"/prm/cfg/trnd_len",_("Trend length (sec)"),RWRWR_,"root",SUI_ID,1,"tp","dec");
	}
	return;
    }

    //> Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/cfg/trnds")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))
	    for(unsigned i_el=0; i_el < trnd_lst.size(); i_el++ )
		opt->childAdd("el")->setText(trnd_lst[i_el]);
	if(ctrChkNode(opt,"add",RWRWR_,"root",SUI_ID,SEC_WR))	{ trnd_lst.push_back(opt->text()); modif(); }
	if(ctrChkNode(opt,"del",RWRWR_,"root",SUI_ID,SEC_WR))
	    for(unsigned i_el=0; i_el < trnd_lst.size(); i_el++)
		if(trnd_lst[i_el] == opt->text() )
		{
		    trnd_lst.erase(trnd_lst.begin()+i_el);
		    modif();
		    break;
		}
    }
    else if(a_path == "/prm/cfg/col")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(TSYS::int2str(nCol()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setNCol(atoi(opt->text().c_str()));
    }
    else if(a_path == "/prm/cfg/hsize")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(TSYS::int2str(hSize()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setHSize(atoi(opt->text().c_str()));
    }
    else if(a_path == "/prm/cfg/vsize")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(TSYS::int2str(vSize()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setVSize(atoi(opt->text().c_str()));
    }
    else if(a_path == "/prm/cfg/trnd_tm")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(TSYS::int2str(trndTm()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setTrndTm(atoi(opt->text().c_str()));
    }
    else if(a_path == "/prm/cfg/trnd_len")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(TSYS::int2str(trndLen()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setTrndLen(atoi(opt->text().c_str()));
    }
    else TUI::cntrCmdProc(opt);
}

//!!! Registered export function for process GET requests from OpenSCADA transport's protocol module HTTP.
void TWEB::HttpGet( const string &url, string &page, const string &sender, vector<string> &vars )
{
    string ntrnd = TSYS::pathLev(url,0);
    if( !ntrnd.size() )
    {
	//> Make main page
	page = w_head();
	int i_col = 0;
	page = page+"<table>\n";
	for(int i_el = 0; i_el < trnd_lst.size(); i_el++ )
	{
	    try
	    {
		if( (dynamic_cast<TVal*>(&SYS->nodeAt(trnd_lst[i_el],0,'.').at()) &&
		    !dynamic_cast<TVal&>(SYS->nodeAt(trnd_lst[i_el],0,'.').at()).arch().freeStat()) ||
		    dynamic_cast<TVArchive *>(&SYS->nodeAt(trnd_lst[i_el],0,'.').at()) )
		{
		    if(i_col==0) page = page+"<tr>";
		    page = page+"<td><b>"+trnd_lst[i_el]+"</b><br/>\n";
		    page = page+"<img src='/"+MOD_ID+"/"+TSYS::int2str(i_el)+"' border='0'/></td>\n";
		    if(i_col==(n_col-1)) page = page+"</tr>";
		    if(++i_col == n_col) i_col=0;
		}
	    }catch(...)	{ }
	}
	page = page+"</table>\n"+w_tail();
	page = http_head("200 OK",page.size())+page;
    }
    else
    {
	AutoHD<TVArchive> arch;
	int imgn = atoi(ntrnd.c_str());
	if( dynamic_cast<TVal *>(&SYS->nodeAt(trnd_lst[imgn],0,'.').at()) )
	    arch = dynamic_cast<TVal&>(SYS->nodeAt(trnd_lst[imgn],0,'.').at()).arch();
	else if( dynamic_cast<TVArchive *>(&SYS->nodeAt(trnd_lst[imgn],0,'.').at()) )
	    arch = SYS->nodeAt(trnd_lst[imgn],0,'.');

	if( !arch.freeStat() )
	{
	    int64_t v_beg = ((trnd_tm+trnd_len)>time(NULL))?time(NULL)-trnd_len:trnd_tm;
	    int64_t v_end = v_beg+trnd_len;

	    page = arch.at().makeTrendImg(v_beg*1000000,v_end*1000000,"",h_sz, v_sz );
	}
	page = http_head("200 OK",page.size(),string("image/png"))+page;
    }
}

//!!! Registered export function for process POST requests from OpenSCADA transport's protocol module HTTP.
void TWEB::HttpPost( const string &url, string &page, const string &sender, vector<string> &vars, const string &contein )
{

}

//!!! Your module self functions realisation.
string TWEB::http_head( const string &rcode, int cln, const string &cnt_tp, const string &addattr )
{
    return  "HTTP/1.0 "+rcode+"\n"
	"Server: "+PACKAGE_STRING+"\n"
	"Accept-Ranges: bytes\n"
	"Content-Length: "+TSYS::int2str(cln)+"\n"
	"Connection: close\n"
	"Content-type: "+cnt_tp+"\n"
	"Charset="+Mess->charset()+"\n"+addattr+"\n";
}

string TWEB::w_head( )
{
    bool per_refr = trnd_tm > time(NULL);

    string shead =
	"<?xml version='1.0' ?>\n"
	"<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Transitional//EN'\n"
	"'DTD/xhtml1-transitional.dtd'>\n"
	"<html xmlns='http://www.w3.org/1999/xhtml'>\n"
	"<head>\n"
	"  <meta http-equiv='Content-Type' content='text/html; charset="+Mess->charset()+"'/>\n";
    if( per_refr )
	shead=shead+"<meta http-equiv='Refresh' content='1'/>\n<meta http-equiv='Cache-Control' content='no-cache'/>\n";
    shead=shead+"  <title>OpenSCADA debug web modul!</title>\n"
	"</head>\n"
	"<body bgcolor='#818181' text='#000000' link='#3366ff' vlink='#339999' alink='#33ccff'>\n";
	//"<h1 align=\"center\"><font color=\"#ffff00\"> Welcome to OpenSCADA debug web modul!</font></h1>\n"
	//"<hr width=\"100%\" size=\"2\">\n"
	//"<hr width='100%' size='3'/><br/>\n";

    return shead;
}

string TWEB::w_tail( )
{
    return
	//"<hr width='100%' size='3'/>\n"
	"</body>\n"
	"</html>";
}

