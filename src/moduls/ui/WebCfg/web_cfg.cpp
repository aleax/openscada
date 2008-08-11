
//OpenSCADA system module UI.WebCfg file: web_cfg.cpp
/***************************************************************************
 *   Copyright (C) 2004-2007 by Roman Savochenko                           *
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

#include <config.h>
#include <tsys.h>
#include <tmess.h>
#include <tsecurity.h>

#include "web_cfg.h"

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"WebCfg"
#define MOD_NAME	"System configurator (WEB)"
#define MOD_TYPE	"UI"
#define VER_TYPE	VER_UI
#define SUB_TYPE	"WWW"
#define MOD_VERSION	"1.5.0"
#define AUTORS		"Roman Savochenko"
#define DESCRIPTION	"Allow the WEB based OpenSCADA system configurator."
#define LICENSE		"GPL"
//*************************************************

WebCfg::TWEB *WebCfg::mod;

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
	    return new WebCfg::TWEB( source );
	return NULL;
    }
}

using namespace WebCfg;

//*************************************************
//* TWEB                                          *
//*************************************************
TWEB::TWEB( string name ) : m_t_auth(10), lst_ses_chk(0)
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

    //- Default CSS init -
    m_CSStables =
	"hr {width:100%}\n"
	"body {background-color:#818181}\n"
	"h1.head {text-align:center; color:#ffff00 }\n"
	"h2.title {text-align:center; font-style:italic; margin: 0px; padding: 0px; border-width:0px }\n"
	"table.page_head {background-color:#cccccc; border:3px ridge blue; width:100% }\n"
	"table.page_head td.tool {text-align:center; border:1px solid blue; width:120px;  white-space: nowrap }\n"
	"table.page_head td.user {text-align:left; border:1px solid blue; width:120px; white-space: nowrap }\n"
	"table.page_area {background-color:#9999ff; border:3px ridge #a9a9a9; width:100%; padding:2px }\n"
	"table.page_area tr.content {background-color:#cccccc; border:5px ridge #9999ff; padding:5px }\n"
	"table.page_auth {background-color:#9999ff; border:3px ridge #a9a9a9; padding:2px }\n"
	"table.page_auth tr.content {background-color:#cccccc; border:5px ridge #9999ff; padding:5px }\n";
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
    m_t_auth = atoi( TBDS::genDBGet(nodePath()+"SessTimeLife",TSYS::int2str(m_t_auth)).c_str() );
    m_CSStables = TBDS::genDBGet(nodePath()+"CSSTables",m_CSStables);
}

void TWEB::save_( )
{
    TBDS::genDBSet(nodePath()+"SessTimeLife",TSYS::int2str(m_t_auth));
    TBDS::genDBSet(nodePath()+"CSSTables",m_CSStables);
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
	    "Content-type: "+cnt_tp+"\n"
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
	"  <meta http-equiv='Cache-Control' content='no-cache'/>\n"+
	head_els+
	"  <link rel='shortcut icon' href='/"+MOD_ID+"/%2fico' type='image' />\n"
	"  <title>"+PACKAGE_NAME+". "+_(MOD_NAME)+"</title>\n"
	"  <style type='text/css'>\n"+m_CSStables+"</style>\n"
	"</head>\n"
	"<body alink='#33ccff' link='#3366ff' text='#000000' vlink='#339999'>\n"
	"<h1 class='head'>"+PACKAGE_NAME+". "+_(MOD_NAME)+"</h1>\n"
	"<hr size='3'/><br/>\n";
}

string TWEB::pgTail( )
{
    return "<hr size='3'/>\n</body>\n</html>";
}

void TWEB::HttpGet( const string &urli, string &page, const string &sender, vector<string> &vars )
{
    map< string, string >::iterator prmEl;
    SSess ses(TSYS::strDecode(urli,TSYS::HttpURL),page,sender,vars,"");
    ses.page = pgHead();

    try
    {
	string zero_lev = TSYS::pathLev(ses.url,0);
	//- Get about module page -
	if( zero_lev == "about" )	getAbout(ses);
	//- Get module icon -
	else if( zero_lev.substr(0,4) == "img_" )
	{
	    //- Get global image -
	    string itp;
	    ses.page=TUIS::icoGet(zero_lev.substr(4),&itp);
	    page = httpHead("200 OK",ses.page.size(),string("image/")+itp)+ses.page;
	    return;
	}
	else
	{
	    sesCheck( ses );
	    //- Auth dialog preparing -
	    if( !ses.user.size() ) getAuth( ses );
	    else
	    {
		ses.pg_info.setName("info");
		ses.pg_info.setAttr("path",ses.url)->setAttr("user",ses.user);
		if(cntrIfCmd(ses.pg_info)) throw TError(ses.pg_info.attr("mcat").c_str(),"%s",ses.pg_info.text().c_str());
		ses.root = ses.pg_info.childGet(0);
		
		if(ses.root->name()=="img")
		{
		    //-- Transfer page image --
		    XMLNode req("get"); req.setAttr("path",ses.url)->setAttr("user",ses.user);
		    if(cntrIfCmd(req)) throw TError(req.attr("mcat").c_str(),"%s",req.text().c_str());
		    ses.page=TSYS::strDecode(req.text(),TSYS::base64);
		    page = httpHead("200 OK",ses.page.size(),string("image/")+req.attr("tp"))+ses.page;
		    return;
		}
		else
		{
		    //-- Get area --
		    getHead( ses );
		    getArea( ses, *ses.root, "/" );
		}
	    }
	}
    }catch(TError err)
    {
	ses.page = "Page <"+ses.url+"> error: "+err.mess;
	//postMess(ses.page,err.cat,err.mess,TWEB::Error); 
	page = httpHead("404 Not Found",ses.page.size())+ses.page;
	return;
    }

    colontDown( ses );
    ses.page += pgTail();

    page = httpHead("200 OK",ses.page.size())+ses.page;
}

void TWEB::getAbout( SSess &ses )
{
    ses.page = ses.page+"<center><table class='page_auth'>\n"
	"<TR><TD>"+PACKAGE+" "+VERSION+"</TD></TR>\n"
	"<TR class='content'><TD>\n"
	"<table border='0'>\n"
	"<TR><TD><font color='Blue'>"+_("Name: ")+"</font></TD><TD>"+_("Open Supervisory Control And Data Acquisition")+"</TD></TR>\n"
	"<TR><TD><font color='Blue'>"+_("License: ")+"</font></TD><TD>GPL</TD></TR>\n"
	"<TR><TD><font color='Blue'>"+_("Author: ")+"</font></TD><TD>Roman Savochenko</TD></TR>\n"
	"</table>\n"
	"</TD></TR></table><br/>\n"
	"<table class='page_auth'>\n"
	"<TR><TD>"+MOD_ID+" "+MOD_VERSION+"</TD></TR>\n"
	"<TR class='content'><TD>\n"
	"<table border='0'>\n"
	"<TR><TD><font color='Blue'>"+_("Name: ")+"</font></TD><TD>"+_(MOD_NAME)+"</TD></TR>"
	"<TR><TD><font color='Blue'>"+_("Desription: ")+"</font></TD><TD>"+_(DESCRIPTION)+"</TD></TR>"
	"<TR><TD><font color='Blue'>"+_("License: ")+"</font></TD><TD>"+_(LICENSE)+"</TD></TR>"
	"<TR><TD><font color='Blue'>"+_("Author: ")+"</font></TD><TD>"+_(AUTORS)+"</TD></TR>"
	"</table>\n"
	"</TD></TR>\n</table><br/></center>\n";
}

void TWEB::getHead( SSess &ses )
{
    string path = string("/")+MOD_ID+ses.url;

    ses.page = ses.page+"<table class='page_head'><tr>\n"
	"<td class='tool'>\n"
	"<a href='/"+MOD_ID+"'><img src='/"+MOD_ID+"/img_gohome' alt='"+_("Root page")+"' border='0'/></a>\n"
	"<a href='"+path+"'><img src='/"+MOD_ID+"/img_reload' alt='" +_("Curent page")+"' border='0'/></a>\n"
	"<a href='"+path.substr(0,path.rfind("/"))+"'><img src='/"+MOD_ID+"/img_up' alt='"+_("Previos page")+"' border='0'/></a>\n"
	"<a href='/"+MOD_ID+"/about'><img src='/"+MOD_ID+"/img_help' alt='"+_("About")+"' border='0'/></a>\n"
	"</td>\n"
	"<td>";
    if(ses.root->childGet("id","ico",true))
	ses.page=ses.page+"<img id='p_ico' src='"+path+"/%252fico' align='left' height='64' width='64'/>";
    ses.page=ses.page+"<h2 class='title'>"+ses.root->attr("dscr")+"</h2></td>\n"
	"<td class='user'";
    if(ses.user == "root") ses.page = ses.page + " bgcolor='red'";
    else                   ses.page = ses.page + " bgcolor='LawnGreen'";
    ses.page = ses.page+">"+_("user:")+" <b>"+ses.user+"</b><br/>"+_("from:")+" <b>"+ses.sender+"</b>\n"
	"<form action='"+path+"' method='post' enctype='multipart/form-data'>\n"
	"<input name='auth_ch' type='submit' value='"+_("Change")+"'/>\n"
	"</form></td>\n"
	"</tr></table><br/>\n";
}

void TWEB::getArea( SSess &ses, XMLNode &node, string a_path )
{
    unsigned	i_cf,c_cfg;
    bool	wr;
    string	area_path;
    string	path = string("/")+MOD_ID+ses.url;

    for( i_cf = 0; i_cf < node.childSize(); i_cf++)
    {
	XMLNode *t_s = node.childGet(i_cf);
	if( t_s->name() == "area" )
	{
	    wr = false;
	    area_path = a_path+t_s->attr("id")+'/';
	    if( a_path == "/" )
		ses.page = ses.page+"<table class='page_area'>\n"
		    "<tr><td><font size='+1'><b><i><u>"+t_s->attr("dscr")+
		    "</u></i></b></font><br/></td></tr>\n"
		    "<tr class='content'><td>";
	    else
		ses.page = ses.page+"<fieldset><legend>"+t_s->attr("dscr")+"</legend>\n";

	    //ses.page = ses.page+"<tr bgcolor=#CCCCCC><td>";
	    //- Get area's fields -
	    c_cfg = 0;
	    bool f_open = false;
	    for( unsigned i_el = 0; i_el < t_s->childSize(); i_el++)
	    {
		XMLNode *t_c = t_s->childGet(i_el);
		if( t_c->name() == "fld" )
		{
		    //-- First element --
		    if(c_cfg++ == 0)
		    {
			ses.page = ses.page+"<form action='"+path+"' method='post' enctype='multipart/form-data'>\n" 
			    "<table><tbody>\n";
		    }
		    //-- Start full element --
		    if( t_c->attr("dscr").size() )
		    {
			if(f_open) ses.page = ses.page+"</td></tr>";
			ses.page = ses.page+"<tr><td valign='top'>"+t_c->attr("dscr")+":</td><td>";
			f_open = true;
		    }
		    wr |= getVal(ses,*t_c,area_path+t_c->attr("id"));
		}
	    }
	    if(f_open) ses.page = ses.page+"</td></tr>";
	    if(c_cfg > 0)
	    {
		ses.page = ses.page+"</tbody></table><br/>\n";
		if(wr) ses.page = ses.page+"<input type='submit' name='apply:"+area_path+"' value='"+_("Apply")+"'/>\n";    // Submit button 
		ses.page = ses.page+"</form>\n<hr size='3'/>\n";
	    }

	    //- Get area's lists -
	    c_cfg = 0;
	    for( unsigned i_el = 0; i_el < t_s->childSize(); i_el++)
	    {
		XMLNode *t_c = t_s->childGet(i_el);
		if( t_c->name() == "list" || t_c->name() == "table" || t_c->name() == "img" )
		{
		    c_cfg++;
		    ses.page = ses.page+t_c->attr("dscr")+":<br/>";
		    getVal(ses,*t_c,area_path+t_c->attr("id"));
		}
	    }
	    //if(c_cfg > 0) ses.page = ses.page+"<hr size='3'/>\n";

	    //- Get area's tables -
	    /*c_cfg = 0;
	    for( unsigned i_el = 0; i_el < t_s->childSize(); i_el++)
	    {
		XMLNode *t_c = t_s->childGet(i_el);
		if( t_c->name() == "table" )
		{
		    c_cfg++;
		    ses.page = ses.page+t_c->attr("dscr")+":<br/>";
		    getVal(ses,*t_c,area_path+t_c->attr("id"));
		}
	    }*/
	    if(c_cfg > 0) ses.page = ses.page+"<hr size='3'/>\n";
	    //- Get area's commands -
	    for( unsigned i_el = 0; i_el < t_s->childSize(); i_el++)
	    {
		//-- First element --
		XMLNode *t_c = t_s->childGet(i_el);
		if( t_c->name() == "comm" )
		    getCmd(ses,*t_c,area_path+t_c->attr("id"));
	    }

	    getArea( ses, *t_s, area_path);
	    if( a_path == "/" ) ses.page = ses.page+"</td></tr></table>\n";
	    else	ses.page = ses.page+"</fieldset>\n";
	}
    }
}

void TWEB::getCmd( SSess &ses, XMLNode &node, string a_path )
{
    string	path = string("/")+MOD_ID+ses.url;

    ses.page = ses.page+"<form action='"+path+"' method='post' enctype='multipart/form-data'>\n"
	"<input type='submit' name='comm:"+a_path+"' value='"+node.attr("dscr")+"'/>\n";
    int f_cfg=0;
    for( unsigned i_el=0; i_el < node.childSize(); i_el++)
    {
	XMLNode *t_c = node.childGet(i_el);
	if( t_c->name() == "fld" )
	{
	    if(f_cfg++ > 0) ses.page = ses.page+", ";
	    else            ses.page = ses.page+"<br/>(";
	    ses.page = ses.page+t_c->attr("dscr")+":";
	    getVal(ses,*t_c,a_path,false);
	}
    }
    if(f_cfg > 0) ses.page = ses.page+")<br/>\n";

    ses.page = ses.page+"</form>\n"; 
}

bool TWEB::getVal( SSess &ses, XMLNode &node, string a_path, bool rd )
{
    string	path = string("/")+MOD_ID+ses.url;

    bool wr = atoi(node.attr("acs").c_str())&SEQ_WR;
    if( node.name() == "fld" )
    {
	XMLNode dt_req("get");
	if( rd )
	{
	    dt_req.setAttr("path",ses.url+"/"+TSYS::strEncode(a_path,TSYS::PathEl))->setAttr("user",ses.user);
	    if(cntrIfCmd(dt_req))
	    {
		ses.mess.push_back(dt_req.text().c_str());
		dt_req.setText("");
	    }
	}
	if( (node.attr("dest") == "select" || node.attr("dest") == "sel_ed") && wr )
	{
	    bool sel_ok = false;

	    ses.page = ses.page+ "<select name='"+TSYS::strEncode(node.attr("id"),TSYS::Html)+"'>";

	    if( node.attr("select").empty() )
	    {
		string s_nm;
		bool ind_ok = node.attr("sel_id").size();	//Index present
		for( int ls_off = 0, c_el = 0; !(s_nm=TSYS::strSepParse(node.attr("sel_list"),0,';',&ls_off)).empty(); c_el++ )
		{
		    if( ind_ok )ses.page = ses.page+"<option value='"+TSYS::strEncode(TSYS::strSepParse(node.attr("sel_id"),c_el,';'),TSYS::Html)+"'";
		    else	ses.page = ses.page+"<option value='"+TSYS::strEncode(s_nm,TSYS::Html)+"'";
		    if( (ind_ok && TSYS::strSepParse(node.attr("sel_id"),c_el,';') == dt_req.text()) ||
			(!ind_ok && s_nm == dt_req.text()) )
		    {
			sel_ok = true;
			ses.page = ses.page+" selected='selected'";
		    }
		    ses.page = ses.page+">"+s_nm+"</option>";
		}
	    }
	    else
	    {
		XMLNode x_lst("get");
		x_lst.setAttr("path",ses.url+"/"+TSYS::strEncode( node.attr("select"), TSYS::PathEl ))->
		    setAttr("user",ses.user);
		if( cntrIfCmd(x_lst) ) ses.mess.push_back(x_lst.text().c_str());

		for( unsigned i_el = 0, c_el = 0; i_el < x_lst.childSize(); i_el++ )
		    if( x_lst.childGet(i_el)->name() == "el")
		    {
			bool ind_ok = x_lst.childGet(i_el)->attr("id").size();  //Index present
			if( ind_ok )ses.page = ses.page+"<option value='"+TSYS::strEncode(x_lst.childGet(i_el)->attr("id"),TSYS::Html)+"'";
			else	ses.page = ses.page+"<option value='"+TSYS::strEncode(x_lst.childGet(i_el)->text(),TSYS::Html)+"'";
			if( (ind_ok && x_lst.childGet(i_el)->attr("id") == dt_req.text()) ||
			    (!ind_ok && x_lst.childGet(i_el)->text() == dt_req.text()) )
			{
			    sel_ok = true;
			    ses.page = ses.page+" selected='selected'";
			}
			ses.page = ses.page+">"+x_lst.childGet(i_el)->text()+"</option>";
		    }
	    }
	    //- Check no selected -
	    if( !sel_ok ) ses.page = ses.page+"<option value='' selected='selected'></option>";
	    ses.page = ses.page+ "</select>\n";
	}
	else
	{
	    if( node.attr("tp") == "bool" )
	    {
		ses.page = ses.page+"<input type='checkbox' name='"+node.attr("id")+"'";
		if( atoi(dt_req.text().c_str()) ) ses.page=ses.page+" checked='checked'";
		if( !wr ) ses.page=ses.page+" disabled='disabled'";
		ses.page = ses.page + "/>\n";
	    }
	    else
	    {
		//- Draw text area -
		if( node.attr("tp") == "str" && (node.attr("rows").size() || node.attr("cols").size()) )
		{
		    ses.page = ses.page+"<textarea name='"+node.attr("id")+"' cols='"+node.attr("cols")+"' rows='"+node.attr("rows")+"'";
		    if( !wr ) ses.page=ses.page+" readonly='readonly'";
		    ses.page = ses.page+">"+TSYS::strEncode(dt_req.text(),TSYS::Html)+"</textarea>\n";
		}
		else if( node.attr("tp") == "time" )
		{
		    struct tm *tm_tm;
		    time_t tm_t;
		    if( dt_req.text().size() ) tm_t = atoi(dt_req.text().c_str());
		    else                       tm_t = time(NULL);
		    tm_tm = localtime(&tm_t);
		    if( !wr )
		    {
			char *c_tm = ctime( &tm_t );
			for( int i_ch = 0; i_ch < strlen(c_tm); i_ch++ )
			    if( c_tm[i_ch] == '\n' ) c_tm[i_ch] = '\0';
			ses.page=ses.page+"<b>"+c_tm+"</b>";
		    }
		    else
		    {
			string s_id = node.attr("id");
			ses.page = ses.page+"<input type='text' name='"+s_id+"_d' value='"+TSYS::int2str(tm_tm->tm_mday)+"' maxlength='2' size='2'/>\n";
			ses.page = ses.page+"<input type='text' name='"+s_id+"_ms' value='"+TSYS::int2str(tm_tm->tm_mon+1)+"' maxlength='2' size='2'/>\n";
			ses.page = ses.page+"<input type='text' name='"+s_id+"_y' value='"+TSYS::int2str(tm_tm->tm_year+1900)+"' maxlength='4' size='4'/>\n";
			ses.page = ses.page+ " , ";
			ses.page = ses.page+"<input type='text' name='"+s_id+"_h' value='"+TSYS::int2str(tm_tm->tm_hour)+"' maxlength='2' size='2'/>\n";
			ses.page = ses.page+"<input type='text' name='"+s_id+"_m' value='"+TSYS::int2str(tm_tm->tm_min)+"' maxlength='2' size='2'/>\n";
			ses.page = ses.page+"<input type='text' name='"+s_id+"_s' value='"+TSYS::int2str(tm_tm->tm_sec)+"' maxlength='2' size='2'/>\n";
		    }
		}
		else
		{
		    //- Other fields -
		    if( !wr ) ses.page=ses.page+"<b>"+dt_req.text()+"</b>";
		    else
		    {
			ses.page = ses.page+"<input type='text' name='"+node.attr("id")+"' value='"+TSYS::strEncode(dt_req.text(),TSYS::Html)+"'";
			//-- addon parameters --
			int val_n = atoi(node.attr("len").c_str());
			if( val_n > 0 ) ses.page = ses.page + " maxlength='"+TSYS::int2str(val_n)+"' size='"+TSYS::int2str((val_n>50)?50:val_n)+"'";
			else
			{
			    if( node.attr("tp") == "dec" ||
				 node.attr("tp") == "hex" ||
				 node.attr("tp") == "oct" ||
				 node.attr("tp") == "real" )
				ses.page = ses.page + " size='5'";
			    else ses.page = ses.page + " size='50'";
			}
			ses.page = ses.page + "/>\n";
		    }
		}
	    }
	}
    }
    else if( node.name() == "list" )
    {
	if( node.attr("tp") == "br" || wr )
	    ses.page = ses.page+"<form action='"+path+"' method='post' enctype='multipart/form-data'>\n";

	XMLNode req("get"); 
	req.setAttr("path",ses.url+"/"+TSYS::strEncode( a_path, TSYS::PathEl ))->
	    setAttr("user",ses.user);
	if(cntrIfCmd(req)) ses.mess.push_back(req.text().c_str());

	bool ind_m = atoi(node.attr("idm").c_str());

	int p_size = req.childSize();
	p_size = (p_size > 20)?20:(p_size < 4)?4:p_size;
	ses.page = ses.page+"<select name='"+TSYS::strEncode(node.attr("id"),TSYS::Html)+"' size='"+TSYS::int2str(p_size)+"'>\n";
	for( unsigned i_lel = 0; i_lel < req.childSize(); i_lel++)
	{
	    XMLNode *t_c = req.childGet(i_lel);
	    if( t_c->name() == "el" )
		ses.page = ses.page+"<option value='"+TSYS::int2str(i_lel)+":"+TSYS::strEncode(t_c->attr("id"),TSYS::Html)+":"+
			TSYS::strEncode(t_c->text(),TSYS::Html)+"'>"+TSYS::strEncode(t_c->text(),TSYS::Html)+"</option>\n";
	}
	ses.page = ses.page+"</select><br/>\n";
	if( node.attr("tp") == "br" )
	    ses.page = ses.page+"<input type='submit' name='list:"+a_path+"/go' value='"+_("Go")+"'/><br/>\n";   //Go branch command
	if( wr && node.attr("s_com").size() )
	{
	    bool p_edit = false;
	    //ses.page = ses.page+"<br/>\n";
	    if( node.attr("s_com").find("add") != string::npos )
	    {
		ses.page = ses.page+"<input type='submit' name='list:"+a_path+"/add' value='"+_("Add")+"'/>\n";    //Add element to list
		p_edit = true;
	    }
	    if( node.attr("s_com").find("ins") != string::npos )
	    {
		ses.page = ses.page+"<input type='submit' name='list:"+a_path+"/ins' value='"+_("Insert")+"'/>\n"; //Insert element to list
		p_edit = true;
	    }
	    if( node.attr("s_com").find("edit") != string::npos )
	    {
		ses.page = ses.page+"<input type='submit' name='list:"+a_path+"/edit' value='"+_("Edit")+"'/>\n";   //Edit list element
		p_edit = true;
	    }
	    if( p_edit )
	    {
		if( ind_m )
		    ses.page = ses.page+"<input type='text' name='ener_id' value=''/>\n";
		node.setName("fld");
		node.setAttr("id","ener_f");
		getVal( ses, node, node.attr("id"), false );
		ses.page = ses.page+"<br/>\n";
	    }
	    if( node.attr("s_com").find("move") != string::npos )
		ses.page = ses.page+
		    "<input type='submit' name='list:"+a_path+"/up' value='"+_("Up")+"'/>\n"
		    "<input type='submit' name='list:"+a_path+"/down' value='"+_("Down")+"'/>\n";

	    if( node.attr("s_com").find("del") != string::npos )
		ses.page = ses.page+"<input type='submit' name='list:"+a_path+"/del' value='"+_("Del")+"'/>\n";    //Del element from list
	}
	if( node.attr("tp") == "br" || wr )
	    ses.page = ses.page+"</form>\n";
    }
    else if( node.name() == "table" )
    {
	if( wr ) ses.page = ses.page+"<form action='"+path+"' method='post' enctype='multipart/form-data'>\n";

	XMLNode req("get"); 
	req.setAttr("path",ses.url+"/"+TSYS::strEncode( a_path, TSYS::PathEl ))->
	    setAttr("user",ses.user);
	if(cntrIfCmd(req)) ses.mess.push_back(req.text().c_str());

	ses.page = ses.page + "<table border='1'>\n";
	int clm_sz[req.childSize()];

	//- Draw head -
	ses.page = ses.page + "<tr bgcolor='#FFA07A' align='center'>";
	for( int i_cl=0; i_cl < req.childSize(); i_cl++)
	{
	    XMLNode *t_lsel = req.childGet(i_cl);
	    XMLNode *t_linf = node.childGet("id",t_lsel->attr("id"),true);
	    if(!t_linf) continue;

	    ses.page = ses.page + "<td>"+TSYS::strEncode(t_linf->attr("dscr"),TSYS::Html)+"</td>";
	    //-- Calc column sizes --
	    clm_sz[i_cl] = t_linf->attr("dscr").size();
	    for( int i_rw=0; i_rw < t_lsel->childSize(); i_rw++)
	    {
		XMLNode *x_el  = t_lsel->childGet(i_rw);
		if( x_el->text().size() > clm_sz[i_cl] ) clm_sz[i_cl] = x_el->text().size();
	    }
	    if( clm_sz[i_cl] < 5 )	clm_sz[i_cl] = 5;
	    if( clm_sz[i_cl] > 40 )	clm_sz[i_cl] = 40;
	}
	if( wr ) ses.page = ses.page + "<td bgcolor='Green'>*</td>";
	ses.page = ses.page + "</tr>\n";

	for( int i_rw=0; i_rw < req.childGet(0)->childSize(); i_rw++)
	{
	    ses.page = ses.page + "<tr bgcolor='#cccccc'>";
	    for( int i_cl=0; i_cl < req.childSize(); i_cl++)
	    {
		XMLNode *t_lsel = req.childGet(i_cl);
		XMLNode *t_linf = node.childGet("id",t_lsel->attr("id"),true);
		if(!t_linf) continue;
		bool c_wr = wr && (atoi(t_linf->attr("acs").c_str())&SEQ_WR);

		XMLNode *x_el = t_lsel->childGet(i_rw);
		if( t_linf->attr("tp") == "time" )
		{
		    time_t tm_t = atoi(x_el->text().c_str());
		    char *c_tm = ctime( &tm_t );
		    for( int i_ch = 0; i_ch < strlen(c_tm); i_ch++ )
			if( c_tm[i_ch] == '\n' ) c_tm[i_ch] = '\0';
		    ses.page = ses.page+"<td nowrap='nowrap'>"+c_tm+"</td>";
		}
		else if( (t_linf->attr("dest") == "select" || t_linf->attr("dest") == "sel_ed") && c_wr )
		{
		    ses.page = ses.page+ "<td><select name='"+TSYS::int2str(i_rw)+":"+t_linf->attr("id")+"'>";

		    bool sel_ok = false;
		    bool u_ind = atoi(t_linf->attr("idm").c_str());

		    if( t_linf->attr("select").empty() )
		    {
			string s_nm;
			for( int ls_off = 0, id_off = 0, i_ls = 0; !(s_nm=TSYS::strSepParse(t_linf->attr("sel_list"),0,';',&ls_off)).empty(); i_ls++ )
			{
			    if( u_ind ) ses.page = ses.page+"<option value='"+TSYS::strSepParse(t_linf->attr("sel_id"),i_ls,';')+"'";
			    else ses.page = ses.page+"<option value='"+s_nm+"'";

			    if( (u_ind && TSYS::strSepParse(t_linf->attr("sel_id"),i_ls,';') == x_el->text()) ||
				(!u_ind && s_nm == x_el->text()) )
			    {
				sel_ok = true;
				ses.page = ses.page+" selected='selected'";
			    }
			    ses.page = ses.page+">"+s_nm+"</option>";
			}
		    }
		    else
		    {
			XMLNode x_lst("get");
			x_lst.setAttr("path",ses.url+"/"+TSYS::strEncode( t_linf->attr("select"),TSYS::PathEl))->
			setAttr("user",ses.user);
			if(cntrIfCmd(x_lst)) ses.mess.push_back(x_lst.text().c_str());

			for( int i_ls = 0; i_ls < x_lst.childSize(); i_ls++ )
			{
			    if( u_ind ) ses.page = ses.page+"<option value='"+x_lst.childGet(i_ls)->attr("id")+"'";
			    else ses.page = ses.page+"<option value='"+x_lst.childGet(i_ls)->text()+"'";
			
			    if( (u_ind && x_lst.childGet(i_ls)->attr("id") == x_el->text()) ||
				(!u_ind && x_lst.childGet(i_ls)->text() == x_el->text()) )
			    {
				sel_ok = true;
				ses.page = ses.page+" selected='selected'";
			    }
			    ses.page = ses.page+">"+x_lst.childGet(i_ls)->text()+"</option>";
			}
		    }
		    if( !sel_ok ) ses.page = ses.page+"<option value='' selected='selected'></option>";
		    ses.page = ses.page+ "</select></td>\n";
		}
		else if( t_linf->attr("tp") == "bool" )
		{
		    ses.page = ses.page+"<td><input type='checkbox' name='"+TSYS::int2str(i_rw)+":"+t_linf->attr("id")+"'";
		    if( atoi(x_el->text().c_str()) ) ses.page=ses.page+" checked='checked'";
		    if( !c_wr ) ses.page=ses.page+" disabled='disabled'";
		    ses.page = ses.page + "/></td>\n";
		}
		else
		{
		    if( !c_wr ) ses.page = ses.page+"<td>"+TSYS::strEncode(x_el->text(),TSYS::Html)+"</td>";
		    else ses.page = ses.page+"<td><input type='text' name='"+TSYS::int2str(i_rw)+
			":"+t_linf->attr("id")+"' value='"+TSYS::strEncode(x_el->text(),TSYS::Html)+"' size='"+TSYS::int2str(clm_sz[i_cl])+"'/></td>";
		}
	    }
	    //-- Add modify buttons --
	    if( wr ) ses.page = ses.page+"<td><input type='checkbox' name='row:"+TSYS::int2str(i_rw)+"'/></td>";
	    ses.page = ses.page + "</tr>\n";
	}
	ses.page = ses.page + "</table>\n";

	if( wr )
	{
	    ses.page = ses.page+"<input type='submit' name='tbl:"+a_path+"/apply' value='"+_("Apply")+"'/><br/>";
	    if( node.attr("s_com").find("add") != string::npos )
		ses.page = ses.page+"<input type='submit' name='tbl:"+a_path+"/add' value='"+_("Add row")+"'/>";
	    if( node.attr("s_com").find("ins") != string::npos )
		ses.page = ses.page+"<input type='submit' name='tbl:"+a_path+"/ins' value='"+_("Insert row")+"'/>";
	    if( node.attr("s_com").find("del") != string::npos )
		ses.page = ses.page+"<input type='submit' name='tbl:"+a_path+"/del' value='"+_("Delete row")+"'/>";
	    if( node.attr("s_com").find("move") != string::npos )
	    {
		ses.page = ses.page+"<input type='submit' name='tbl:"+a_path+"/up' value='"+_("Move up row")+"'/>";
		ses.page = ses.page+"<input type='submit' name='tbl:"+a_path+"/down' value='"+_("Move down row")+"'/>";
	    }

	    ses.page = ses.page+"</form>\n";
	}
    }
    else if( node.name() == "img" )
    {
	ses.page = ses.page + "<img src='/"+MOD_ID+TSYS::strEncode(ses.url+"/"+TSYS::strEncode(a_path,TSYS::PathEl),TSYS::HttpURL)+"'/><br/>\n";
    }

    return wr;
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
    map< string, string >::iterator cntEl;
    SSess ses(TSYS::strDecode(url,TSYS::HttpURL),page,sender,vars,contain);

    //- Check for autentification POST requests -
    if( ses.cnt.find("auth_enter") != ses.cnt.end() )
    {
	string pass;
	if( (cntEl=ses.cnt.find("user")) != ses.cnt.end() )	ses.user = cntEl->second;
	if( (cntEl=ses.cnt.find("pass")) != ses.cnt.end() )	pass = cntEl->second;
	if( SYS->security().at().usrPresent(ses.user) && SYS->security().at().usrAt(ses.user).at().auth(pass) )
	{
	    ses.page = pgHead("<META HTTP-EQUIV='Refresh' CONTENT='0; URL=/"MOD_ID"/"+url+"'/>")+pgTail();
	    page=httpHead("200 OK",ses.page.size(),"text/html",
		"Set-Cookie: oscd_u_id="+TSYS::int2str(sesOpen(ses.user))+"; path=/;\n")+ses.page;
	    return;
	}
	ses.page = pgHead();
	messPost(ses.page,nodePath(),_("Auth wrong! Retry please."),TWEB::Error);
	ses.page += "\n";
	getAuth( ses );
	ses.page += pgTail();
	page = httpHead("200 OK",ses.page.size(),"text/html")+ses.page;
	return;
    }
    //- Session check -
    sesCheck( ses );
    if( !ses.user.size() || ses.cnt.find("auth_ch") != ses.cnt.end() )
    {
	ses.page = pgHead("<META HTTP-EQUIV='Refresh' CONTENT='0; URL=/"MOD_ID"/"+url+"'/>")+pgTail();
	page=httpHead("200 OK",ses.page.size(),"text/html",
	"Set-Cookie: oscd_u_id=""; path=/;\n")+ses.page;
	return;
    }

    //- Commands process -
    ses.page = pgHead();
    try
    {
	ses.pg_info.setName("info");
	ses.pg_info.setAttr("path",ses.url)->setAttr("user",ses.user);
	if(cntrIfCmd(ses.pg_info)) throw TError(ses.pg_info.attr("mcat").c_str(),"%s",ses.pg_info.text().c_str());
	ses.root = ses.pg_info.childGet(0);

	//-- Parse post category and path to area --
	string prs_cat, prs_path;
	unsigned i_el;
 
	string prs_comm;
	for( cntEl = ses.cnt.begin(); cntEl != ses.cnt.end(); cntEl++ )
	{
	    prs_comm = cntEl->first;
	     string comm_t = TSYS::strSepParse(prs_comm,0,':');
	     if( comm_t == "apply" || comm_t == "comm" || comm_t == "list" || comm_t == "tbl" ) break;
	}
	if( cntEl != ses.cnt.end() )
	{
	    ses.cnt.erase(cntEl);
	    int kz = postArea( ses, *ses.root, prs_comm );

	    if( !(kz&0x01) ) messPost(ses.page,nodePath(),"Post request broken!",TWEB::Error);
	    else if( !(kz&0x02) )
	    {
		ses.pg_info.setName("info");
		ses.pg_info.setAttr("path",ses.url)->setAttr("user",ses.user);
		if(cntrIfCmd(ses.pg_info)) throw TError(ses.pg_info.attr("mcat").c_str(),"%s",ses.pg_info.text().c_str());
		ses.root = ses.pg_info.childGet(0);
		getHead( ses );
		getArea( ses, *ses.root, "/" );
	    }
	}
    }catch(TError err) 
    { messPost(ses.page,err.cat,err.mess,TWEB::Error); }

    colontDown( ses );
    ses.page += pgTail();

    page = httpHead("200 OK",ses.page.size(),"text/html")+ses.page;
}

int TWEB::postArea( SSess &ses, XMLNode &node, const string &prs_comm, int level )
{
    string prs_cat = TSYS::strSepParse(prs_comm,0,':');
    string prs_path = TSYS::strSepParse(prs_comm,1,':');

    if( !TSYS::pathLev(prs_path,level).size() ) return 0x00;
    try
    {
	XMLNode *t_nd = TCntrNode::ctrId(&node, TSYS::pathLev(prs_path,level));
	if( prs_cat == "apply" && !TSYS::pathLev(prs_path,level+1).size() )
	    return( postVal(ses,*t_nd,prs_path) );
	else if( prs_cat == "comm" && !TSYS::pathLev(prs_path,level+1).size() )
	    return( postCmd(ses,*t_nd,prs_path));
	else if( prs_cat == "list" && t_nd->name() == "list" )
	    return( postList(ses,*t_nd,prs_path) );
	else if( prs_cat == "tbl" && t_nd->name() == "table" )
	    return( postTable(ses,*t_nd,prs_path) );
	return( postArea( ses, *t_nd, prs_comm, ++level ));
    }
    catch(TError err)
    {
	messPost(ses.page,err.cat,err.mess,TWEB::Error);
	return 0x01|0x02;
    }
}

int  TWEB::postVal( SSess &ses, XMLNode &node, string prs_path)
{
    //- Free no changed elements (polimorfic problem fix) -
    for( unsigned i_cf = 0; i_cf < node.childSize(); i_cf++)
    {
	XMLNode *t_c = node.childGet(i_cf);
	if( (t_c->name() == "fld") && (atoi(t_c->attr("acs").c_str())&SEQ_WR) &&
		t_c->attr("tp") != "bool" && !valPrepare( ses, *t_c,prs_path, true ) )
	    ses.cnt.erase(t_c->attr("id"));
    }

    //- Check and modify -
    for( unsigned i_cf = 0; i_cf < node.childSize(); i_cf++)
    {
	XMLNode *t_c = node.childGet(i_cf);
	if( t_c->name() == "fld" && (atoi(t_c->attr("acs").c_str())&SEQ_WR) )
	{
	    if( !valPrepare( ses, *t_c,prs_path, true ) ) 
		continue;
	    mess_info(nodePath().c_str(),_("%s| Change <%s:%s> to %s"),
		ses.user.c_str(),
		t_c->attr("id").c_str(),
		t_c->attr("dscr").c_str(),
		t_c->text().c_str());
		
	    XMLNode req("set"); 
	    req.setAttr("path",ses.url+"/"+TSYS::strEncode(prs_path+t_c->attr("id"), TSYS::PathEl))->
		setAttr("user",ses.user)->setText(t_c->text());
	    if(cntrIfCmd(req)) { ses.mess.push_back(req.text().c_str()); return 0x01; }
	    continue;
	}
    }
    return 0x01;
}

int TWEB::postCmd( SSess &ses, XMLNode &node, string prs_path )
{
    //- Prepare command options -
    try
    {
	//-- Check link --
	if( node.attr("tp") == "lnk" )
	{
	    XMLNode req("get"); 
	    req.setAttr("path",ses.url+"/"+TSYS::strEncode(prs_path,TSYS::PathEl))->
		setAttr("user",ses.user);
	    if(cntrIfCmd(req)) ses.mess.push_back(req.text().c_str());

	    string url = string("/")+MOD_ID+"/"+TSYS::strEncode(req.text(),TSYS::HttpURL);

	    mess_info(nodePath().c_str(),_("%s| Go to link <%s>"),ses.user.c_str(),url.c_str());

	    ses.page = ses.page + "<meta http-equiv='Refresh' content='0; url="+url+"'>\n";
	    messPost( ses.page,nodePath(),"Go to <"+url+"> !",TWEB::Info);
	    return 0x01|0x02;  //No error. That no draw curent page
	}

	for( int i_prm = 0; i_prm < node.childSize(); i_prm++ )
	{
	    XMLNode *t_c = node.childGet(i_prm);
	    if( t_c->name() == "fld" )
	    valPrepare( ses, *t_c, "", false );
	}

	mess_info(nodePath().c_str(),_("%s| Put command <%s:%s>"),
	    ses.user.c_str(),
	    node.attr("id").c_str(),
	    node.attr("dscr").c_str());

	XMLNode req("set");
	req.setAttr("path",ses.url+"/"+TSYS::strEncode(prs_path,TSYS::PathEl))->setAttr("user",ses.user);

	//-- Copy command parameters --
	for( int i_ch = 0; i_ch < node.childSize(); i_ch++ )
	    *req.childAdd() = *node.childGet(i_ch);
	if(cntrIfCmd(req)) ses.mess.push_back(req.text().c_str());
	//return 0x01;
    }
    catch(TError err){ ses.mess.push_back( err.mess ); }
    //{
	//postMess(ses.page,err.mess,TWEB::Error);
	//return 0x01|0x02;
    //}
    return 0x01;
}

int TWEB::postList( SSess &ses, XMLNode &node, string prs_path )
{
    string	path = string("/")+MOD_ID+ses.url;
    string	f_path, l_path, nd_nm, l_com, ener_f, ener_id;

    bool ind_m = atoi(node.attr("idm").c_str());
    //- Get command name and path -
    int c_pos = 1;
    while(true)
    {
	int t_pos = prs_path.find("/",c_pos);
	if( t_pos == string::npos )
	{
	    l_com = prs_path.substr(c_pos,t_pos-c_pos);
	    break;
	}
	l_path = l_path + "/" + prs_path.substr(c_pos,t_pos-c_pos);//+"/";
	c_pos = t_pos+1;
    }
    f_path = ses.url+"/"+TSYS::strEncode(l_path,TSYS::PathEl);

    //- Get new element value -
    map< string, string >::iterator cntEl;
    if( (cntEl=ses.cnt.find("ener_id")) != ses.cnt.end() )	ener_id = cntEl->second;
    if( (cntEl=ses.cnt.find("ener_f")) != ses.cnt.end() )	ener_f = cntEl->second;

    string i_pos, i_el, l_el;

    //- Check selected item -
    if( l_com == "go" || l_com == "ins" || l_com == "edit" || 
	l_com == "del" || l_com == "up" || l_com == "down" )
    {
	if( (cntEl=ses.cnt.find(node.attr("id"))) != ses.cnt.end() )	l_el = cntEl->second;
	else
	{
	    ses.mess.push_back( "No select list element of <"+node.attr("dscr")+">!" );
	    return 0x01;
	    //postMess(ses.page,"No select list element for list <"+node.attr("dscr")+">!",TWEB::Warning);
	    //return( 0x01|0x02 );
	}
	i_pos = TSYS::strSepParse(l_el,0,':');
	i_el =  TSYS::strSepParse(l_el,1,':');
	l_el =  TSYS::strSepParse(l_el,2,':');
    }
    if( l_com == "go" )
    {
	//-- Get branch prefix --
	string br_pref = node.attr("br_pref");
	//if( node.attr("br_pref").size() )	br_pref = node.attr("br_pref");
	//else					br_pref = l_path;
	//-- Check branch type --
	string url = path+"/";

	//-- Check Index-list mode --
	if( i_el.size() )	url.append(TSYS::strEncode(TSYS::strEncode(br_pref+i_el,TSYS::PathEl),TSYS::HttpURL));
	else			url.append(TSYS::strEncode(TSYS::strEncode(br_pref+l_el,TSYS::PathEl),TSYS::HttpURL));

	//printf("GO URL: %s\n",url.c_str());

	ses.page = ses.page + "<meta http-equiv='Refresh' content='0; url="+url+"'/>\n";
	messPost( ses.page, nodePath(), "Go to <"+url+"> !",TWEB::Info);
	return 0x01|0x02;  //No error. That no draw curent page
    }

    XMLNode n_el1; n_el1.setAttr("path",f_path)->setAttr("user",ses.user);
    if( l_com == "del" )
    {
	n_el1.setName("del");
	n_el1.setAttr("pos",i_pos);
	if( ind_m ) n_el1.setAttr("id",i_el);
	else n_el1.setText(l_el);

	mess_info(nodePath().c_str(),_("%s| Delete <%s> element <%s:%s>."),
	    ses.user.c_str(), f_path.c_str(), i_el.c_str(), l_el.c_str());
    }
    else if( l_com == "ins" )
    {
	n_el1.setName("ins");
	n_el1.setAttr("pos",i_pos);
	n_el1.setAttr("p_id",(ind_m)?i_el:l_el);
	if( ind_m ) n_el1.setAttr("id",ener_id);
	n_el1.setText(ener_f);

	mess_info(nodePath().c_str(),_("%s| Insert <%s> element <%s:%s> to %s."),
	    ses.user.c_str(), f_path.c_str(), ener_id.c_str(), ener_f.c_str(), i_pos.c_str());
    }
    else if( l_com == "add" )
    {
	n_el1.setName("add");
	if( ind_m ) n_el1.setAttr("id",ener_id);
	n_el1.setText(ener_f);

	mess_info(nodePath().c_str(),_("%s| Add <%s> element <%s:%s>."),
	    ses.user.c_str(), f_path.c_str(), ener_id.c_str(), ener_f.c_str());
    }
    else if( l_com == "edit" )
    {
	n_el1.setName("edit");
	n_el1.setAttr("pos",i_pos);
	n_el1.setAttr("p_id",(ind_m)?i_el:l_el);
	if( ind_m ) n_el1.setAttr("id",ener_id);
	n_el1.setText(ener_f);

	mess_info(nodePath().c_str(),_("%s| Set <%s> element %s to <%s:%s>."),
	    ses.user.c_str(), f_path.c_str(), i_pos.c_str(), ener_id.c_str(), ener_f.c_str());
    }
    else if( l_com == "up" || l_com == "down" )
    {
	string i_pos_to;
	if( l_com == "down" )	i_pos_to = TSYS::int2str(atoi(i_pos.c_str())-1);
	else			i_pos_to = TSYS::int2str(atoi(i_pos.c_str())+1);
	n_el1.setName("move");
	n_el1.setAttr("pos",i_pos);
	n_el1.setAttr("to",i_pos_to);

	mess_info(nodePath().c_str(),_("%s| Move <%s> from %s to %s."),
	    ses.user.c_str(), f_path.c_str(), i_pos.c_str(), i_pos_to.c_str());
    }

    if(cntrIfCmd(n_el1)) ses.mess.push_back(n_el1.text().c_str());    

    return 0x01;
}

int TWEB::postTable( SSess &ses, XMLNode &node, string prs_path )
{
    XMLNode	n_el1;
    string	path = string("/")+MOD_ID+ses.url;
    string	f_path, l_path, l_com;
    int		op_cnt = 0;

    bool wr = atoi(node.attr("acs").c_str())&SEQ_WR;
    if( !wr )	return 0x01;

    //- Get command name and path -
    int c_pos = 1;
    while(true)
    {
	int t_pos = prs_path.find("/",c_pos);
	if( t_pos == string::npos )
	{
	    l_com = prs_path.substr(c_pos,t_pos-c_pos);
	    break;
	}
	l_path = l_path + "/" + prs_path.substr(c_pos,t_pos-c_pos);//+"/";
	c_pos = t_pos+1;
    }
    f_path = ses.url+"/"+TSYS::strEncode(l_path,TSYS::PathEl);

    XMLNode dt_tbl("get"); dt_tbl.setAttr("path",f_path)->setAttr("user",ses.user);
    if(cntrIfCmd(dt_tbl)) ses.mess.push_back(dt_tbl.text().c_str());

    n_el1.setAttr("path",f_path)->setAttr("user",ses.user);

    if( l_com == "apply" )
    {
	for( int i_cl=0; i_cl < dt_tbl.childSize(); i_cl++)
	{
	    XMLNode *t_lsel = dt_tbl.childGet(i_cl);
	    XMLNode *t_linf = node.childGet("id",t_lsel->attr("id"),true);
	    if(!t_linf) continue;
	    if(wr && (atoi(t_linf->attr("acs").c_str())&SEQ_WR) )
		for( int i_rw=0; i_rw < t_lsel->childSize(); i_rw++)
		{
		    XMLNode *w_el = t_lsel->childGet(i_rw);
		    string new_val = cntGet(ses,TSYS::int2str(i_rw)+":"+t_linf->attr("id"));
		    if( t_linf->attr("tp") == "bool" )
			new_val = (new_val!="<empty>")?"1":"0";
		    if( new_val != "<empty>" && new_val != w_el->text() )
		    {
			n_el1.setName("set");
			n_el1.setText(new_val);
			string row_addr;
			if( !node.attr("key").size() )
			{
			    row_addr=TSYS::int2str(i_rw);
			    n_el1.setAttr("row",row_addr)->setAttr("col",TSYS::int2str(i_cl));
			}
			else
			{
			    //-- Get Key columns --
			    string key;
			    for( int i_off = 0; (key=TSYS::strSepParse(node.attr("key"),0,',',&i_off)).size(); )
				for( int i_el = 0; i_el < dt_tbl.childSize(); i_el++ )
				    if( dt_tbl.childGet(i_el)->attr("id") == key )
				    {
					n_el1.setAttr("key_"+key,dt_tbl.childGet(i_el)->childGet(i_rw)->text());
					row_addr=row_addr+"key_"+key+"="+dt_tbl.childGet(i_el)->childGet(i_rw)->text()+",";
					break;
				    }
			    //-- Get current column id --
			    n_el1.setAttr("col",t_linf->attr("id"));
			}
			mess_info(nodePath().c_str(),_("%s| Set <%s> cell (<%s>:%s) to: %s."),
			    ses.user.c_str(), f_path.c_str(), row_addr.c_str(), t_linf->attr("id").c_str(), new_val.c_str());

			if(cntrIfCmd(n_el1)) ses.mess.push_back(n_el1.text().c_str());
		    }
		}
	}
    }
    else if( l_com == "add" )
    {
	n_el1.setName("add");
	mess_info(nodePath().c_str(),_("%s| Add <%s> record."), ses.user.c_str(), f_path.c_str() );

	if(cntrIfCmd(n_el1)) ses.mess.push_back(n_el1.text().c_str());
    }
    else if( l_com == "ins" && dt_tbl.childSize() > 0 )
    {
	for( int i_rw=0; i_rw < dt_tbl.childGet(0)->childSize(); i_rw++)
	    if( cntGet(ses,"row:"+TSYS::int2str(i_rw)) != "<empty>" )
	    {
		n_el1.setName("ins");
		n_el1.setAttr("row",TSYS::int2str(i_rw+op_cnt));
		mess_info(nodePath().c_str(),_("%s| Insert <%s> record %d."),
		    ses.user.c_str(), f_path.c_str(), i_rw+op_cnt );
		
		if(cntrIfCmd(n_el1)) ses.mess.push_back(n_el1.text().c_str());
		op_cnt++;
	    }
    }
    else if( l_com == "del" && dt_tbl.childSize() > 0 )
    {
	for( int i_rw=0; i_rw < dt_tbl.childGet(0)->childSize(); i_rw++)
	    if( cntGet(ses,"row:"+TSYS::int2str(i_rw)) != "<empty>" )
	    {
		n_el1.setName("del");
		if( !node.attr("key").size() )
		    n_el1.setAttr("row",TSYS::int2str(i_rw-op_cnt));
		else
		{
		     //-- Get Key columns --
		     string key;
		     for( int i_off = 0; (key=TSYS::strSepParse(node.attr("key"),0,',',&i_off)).size(); )
			for( int i_el = 0; i_el < dt_tbl.childSize(); i_el++ )
			    if( dt_tbl.childGet(i_el)->attr("id") == key )
			    { n_el1.setAttr("key_"+key,dt_tbl.childGet(i_el)->childGet(i_rw)->text()); break; }
		}
		mess_info(nodePath().c_str(),_("%s| Delete <%s> record %d."),
		    ses.user.c_str(), f_path.c_str(), i_rw-op_cnt );
		
		if(cntrIfCmd(n_el1)) ses.mess.push_back(n_el1.text().c_str());
		op_cnt++;
	    }
    }
    else if( (l_com == "up" || l_com == "down") && dt_tbl.childSize() > 0 )
    {
	for( int i_rw=0; i_rw < dt_tbl.childGet(0)->childSize(); i_rw++)
	    if( cntGet(ses,"row:"+TSYS::int2str(i_rw)) != "<empty>" )
	    {
		int r_new = i_rw-1;
		if( l_com == "down" )  r_new = i_rw+1;
		n_el1.setName("move");
		n_el1.setAttr("row",TSYS::int2str(i_rw))->setAttr("to",TSYS::int2str(r_new));
		mess_info(nodePath().c_str(),_("%s| Move <%s> record from %d to %d."),
		    ses.user.c_str(), f_path.c_str(), i_rw, r_new );
		
		if(cntrIfCmd(n_el1)) ses.mess.push_back(n_el1.text().c_str());
	    }
    }

    return 0x01;
}

void TWEB::messPost( string &page, const string &cat, const string &mess, MessLev type )
{
    //- Put system message. -
    message(cat.c_str(), (type==Error) ? TMess::Error :
			 (type==Warning) ? TMess::Warning :
			  TMess::Info,"%s",mess.c_str());

    //- Prepare HTML messages -
    page = page+"<table border='2' width='40%' align='center'><tbody>\n";
    if(type == Warning )
	page = page+"<tr bgcolor='yellow'><td align='center'><b>Warning!</b></td></tr>\n";
    else if(type == Error )
	page = page+"<tr bgcolor='red'><td align='center'><b>Error!</b></td></tr>\n";
    else page = page+"<tr bgcolor='#9999ff'><td align='center'><b>Message!</b></td></tr>\n";
    page = page+"<tr bgcolor='#cccccc'> <td align='center'>"+TSYS::strEncode(mess,TSYS::Html)+"</td></tr>\n";
    page = page+"</tbody></table>\n";
}

bool TWEB::valPrepare( SSess &ses, XMLNode &node, string prs_path, bool compare )
{
    unsigned i_cnt;
    string   val;
    map< string, string >::iterator cntEl;

    XMLNode req("get");
    req.setAttr("path",ses.url+"/"+TSYS::strEncode(prs_path+node.attr("id"),TSYS::PathEl))->
	setAttr("user",ses.user);

    if( node.attr("tp") == "bool" )
    {
	if( ses.cnt.find(node.attr("id")) != ses.cnt.end() ) val="1"; else val="0";
	if( compare )
	{
	    if(cntrIfCmd(req)) ses.mess.push_back(req.text().c_str());
	    if(req.text() == val) return false;   //No change fld
	}
    }
    else if( node.attr("tp") == "time" )
    {
	//time_t c_tm = time(NULL);
	struct tm tm_tm;
	//tm_tm = *localtime(&c_tm);
	string s_id = node.attr("id");
	tm_tm.tm_isdst = -1;
	if( (cntEl=ses.cnt.find(s_id+"_d")) != ses.cnt.end() )
	    tm_tm.tm_mday = atoi( cntEl->second.c_str() );
	if( (cntEl=ses.cnt.find(s_id+"_ms")) != ses.cnt.end() )
	    tm_tm.tm_mon = atoi( cntEl->second.c_str() )-1;
	if( (cntEl=ses.cnt.find(s_id+"_y")) != ses.cnt.end() )
	    tm_tm.tm_year = atoi( cntEl->second.c_str() )-1900;
	if( (cntEl=ses.cnt.find(s_id+"_h")) != ses.cnt.end() )
	    tm_tm.tm_hour = atoi( cntEl->second.c_str() );
	if( (cntEl=ses.cnt.find(s_id+"_m")) != ses.cnt.end() )
	    tm_tm.tm_min = atoi( cntEl->second.c_str() );
	if( (cntEl=ses.cnt.find(s_id+"_s")) != ses.cnt.end() )
	    tm_tm.tm_sec = atoi( cntEl->second.c_str() );
	val = TSYS::int2str(mktime(&tm_tm));
	if( compare )
	{
	    if(cntrIfCmd(req)) ses.mess.push_back(req.text().c_str());
	    if(req.text() == val) return false;   //No change time
	}
    }
    else
    {
	if( (cntEl=ses.cnt.find(node.attr("id"))) != ses.cnt.end() )
	{
	    val = cntEl->second;
	    if( compare )
	    {
		if(cntrIfCmd(req)) ses.mess.push_back(req.text().c_str());
		if(req.text() == val) return false;   //No change fld
	    }
	}
	else return false;
    }
    node.setText(val);

    return true;
}

int TWEB::cntrIfCmd( XMLNode &node )
{
    SYS->cntrCmd(&node);
    return atoi(node.attr("rez").c_str());
}

int TWEB::sesOpen( string name )
{
    int sess_id;
    ResAlloc res(m_res,true);

    //- Get free identifier -
    do{ sess_id = rand(); }
    while( sess_id == 0 || m_auth.find(sess_id) != m_auth.end() );

    //- Add new session authentification -
    m_auth[sess_id] = SAuth(name,time(NULL));

    return sess_id;
}

void TWEB::sesCheck( SSess &ses )
{
    time_t cur_tm = time(NULL);
    map< int, SAuth >::iterator authEl;

    ResAlloc res(m_res,false);

    //- Check for close old sessions -
    if( cur_tm > lst_ses_chk+10 )
    {
	for( authEl = m_auth.begin(); authEl != m_auth.end(); )
	    if( cur_tm > authEl->second.t_auth+m_t_auth*60 )
		m_auth.erase(authEl++);
	    else authEl++;
	lst_ses_chk = cur_tm;
    }

    //- Check for session and close old sessions -
    authEl = m_auth.find(atoi(getCookie( "oscd_u_id", ses.vars ).c_str()));
    if( authEl != m_auth.end() )
    {
	ses.user = authEl->second.name;
	authEl->second.t_auth = cur_tm;
    }
}

string TWEB::getCookie( string name, vector<string> &vars )
{
    for( unsigned i_var = 0; i_var < vars.size(); i_var++)
	if( vars[i_var].substr(0, vars[i_var].find(":",0)) == "Cookie" )
	{
	    int i_beg = vars[i_var].find(name+"=",0);
	    if( i_beg == string::npos ) return("");
	    i_beg += name.size()+1;
	    return( vars[i_var].substr(i_beg,vars[i_var].find(";",i_beg)-i_beg) );
	}
    return "";
}

void TWEB::colontDown( SSess &ses )
{
    //-- Draw path --
    ses.page = ses.page+"<hr size='3'/>"+ses.url+"<br/>\n";

    //-- Make warning messages --
    if( ses.mess.size() )
    {
	ses.page = ses.page+"<SCRIPT LANGUAGE='JavaScript'>\n<!--\n";
	for( int i_m = 0; i_m < ses.mess.size(); i_m++)
	    ses.page = ses.page+"alert('"+TSYS::strEncode(ses.mess[i_m],TSYS::JavaSc)+"');\n";
	ses.page = ses.page+"//-->\n</SCRIPT>\n";
    }
}

string TWEB::cntGet( SSess &ses, const string &nm )
{
    map< string, string >::iterator prmEl = ses.cnt.find(nm);
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

