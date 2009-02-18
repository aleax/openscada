
//OpenSCADA system module Protocol.HTTP file: http.cpp
/***************************************************************************
 *   Copyright (C) 2003-2008 by Roman Savochenko                           *
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
#include <string.h>

#include <config.h>
#include <tsys.h>
#include <tmess.h>
#include <tmodule.h>
#include <tuis.h>
#include "http.h"

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"HTTP"
#define MOD_NAME	"HTTP-realisation"
#define MOD_TYPE	"Protocol"
#define VER_TYPE	VER_PROT
#define M_VERSION	"1.3.2"
#define AUTORS		"Roman Savochenko"
#define DESCRIPTION	"Allow support HTTP for WWW based UIs."
#define LICENSE		"GPL"
//*************************************************

PrHTTP::TProt *PrHTTP::mod;

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
	    return new PrHTTP::TProt( source );
	return NULL;
    }
}

using namespace PrHTTP;

//*************************************************
//* TProt                                         *
//*************************************************
TProt::TProt( string name )
{
    mId		= MOD_ID;
    mType	= MOD_TYPE;
    mName	= MOD_NAME;
    mVers	= M_VERSION;
    mAutor	= AUTORS;
    mDescr	= DESCRIPTION;
    mLicense	= LICENSE;
    mSource	= name;

    mod		= this;
}

TProt::~TProt()
{

}

string TProt::optDescr( )
{
    char buf[STR_BUF_LEN];
    snprintf(buf,sizeof(buf),_(
	"======================= The module <%s:%s> options =======================\n"
	"---------- Parameters of the module section <%s> in config file ----------\n\n"),
	MOD_TYPE,MOD_ID,nodePath().c_str());

    return(buf);
}

void TProt::load_( )
{
    //- Load parameters from command line -
    int next_opt;
    const char *short_opt="h";
    struct option long_opt[] =
    {
	{"help"    ,0,NULL,'h'},
	{NULL      ,0,NULL, 0 }
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

    //- Load parameters from config file -
}

TProtocolIn *TProt::in_open( const string &name )
{
    return new TProtIn(name);
}

//*************************************************
//* TProtIn                                       *
//*************************************************
TProtIn::TProtIn( string name ) : TProtocolIn(name), m_nofull(false)
{

}

TProtIn::~TProtIn()
{

}

string TProtIn::http_head( const string &rcode, int cln, const string &addattr )
{
    return  "HTTP/1.0 "+rcode+"\r\n"
	    "Server: "+PACKAGE_STRING+"\r\n"
	    "Accept-Ranges: bytes\r\n"
	    "Content-Length: "+TSYS::int2str(cln)+"\r\n"
	    "Keep-Alive: timeout=5, max=100\r\n"
	    "Connection: Keep-Alive\r\n"
	    "Content-type: text/html\r\n"
	    "Charset="+Mess->charset()+"\r\n"+addattr+"\r\n";
}

bool TProtIn::mess( const string &reqst, string &answer, const string &sender )
{
    bool KeepAlive = false;
    string req, sel;
    vector<string> vars;

    //> Continue for full reqst
    if( m_nofull )
    {
	m_buf = m_buf+reqst;
	m_nofull = false;
    }
    else m_buf=reqst;  //Save request to bufer

    string request = m_buf;

    answer = "";
    if( request.size() > 0 )
    {
	int    pos = 0;
#if OSC_DEBUG >= 3
	mess_debug(nodePath().c_str(),_("Content:\n%s"),request.c_str());
#endif

	//> Parse first record
	req = TSYS::strSepParse(request,0,'\n',&pos);
	if( !req.empty() ) req.resize(req.size()-1);
	string method   = TSYS::strSepParse(req,0,' ');
	string url      = TSYS::strSepParse(req,1,' ');
	string protocol = TSYS::strSepParse(req,2,' ');

	//> Parse parameters
	int c_lng=-1;
	while( true )
	{
	    req = TSYS::strSepParse(request,0,'\n',&pos);
	    if( req.empty() ) { m_nofull=true; break; }
	    if( req == "\r" ) break;
	    req.resize(req.size()-1);
	    int sepPos = req.find(":",0);
	    if( sepPos == 0 || sepPos == string::npos ) break;
	    string var = req.substr(0,sepPos);
	    if( strcasecmp(var.c_str(),"content-length") == 0 )	c_lng = atoi(req.substr(sepPos+1).c_str());
	    else if( strcasecmp(var.c_str(),"connection") == 0 )
		for( int off = 0; (sel=TSYS::strSepParse(req.substr(sepPos+1),0,',',&off)).size(); )
		    if( strcasecmp(TSYS::strNoSpace(sel).c_str(),"keep-alive") == 0 )
		    { KeepAlive = true; break; }
	    vars.push_back( req );
	}

	//> Check content length
	if( (c_lng >= 0 && c_lng > (request.size()-pos)) || (c_lng < 0 && method == "POST") ) m_nofull = true;
	if( m_nofull ) return m_nofull;

	//> Check protocol version
	if( protocol != "HTTP/1.0" && protocol != "HTTP/1.1" )
	{
	    answer = "<html>\n"
		     " <body>\n"
		     "  <h1>Bad Request</h1>\n"
		     "  <p>This server did not undersand your request.</p>\n"
		     " </body>\n"
		     "</html>\n";
	    answer = http_head("400 Bad Request",answer.size())+answer;
	    return m_nofull;
	}

	//> Send request to module
	int url_pos = 0;
	string name_mod = TSYS::pathLev(url,0,false,&url_pos);
	while( url_pos < url.size() && url[url_pos] == '/' ) url_pos++;
	url = "/"+url.substr(url_pos);
	//if( url.empty() )	url = "/";
	try
	{
	    AutoHD<TModule> mod = SYS->ui().at().modAt(name_mod);
	    if( mod.at().modInfo("SubType") != "WWW" ) throw TError(nodePath().c_str(),"Find no one WWW subtype module!");

	    //>> Check metods
	    if( method == "GET" )
	    {
		void(TModule::*HttpGet)( const string &url, string &page, const string &sender, vector<string> &vars);
		mod.at().modFunc("void HttpGet(const string&,string&,const string&,vector<string>&);",
		    (void (TModule::**)()) &HttpGet);

		((&mod.at())->*HttpGet)(url,answer,sender,vars);
#if OSC_DEBUG >= 4
		mess_debug(nodePath().c_str(),"Get Content:\n%s",request.c_str());
#endif
	    }
	    else if( method == "POST" )
	    {
		void(TModule::*HttpPost)( const string &url, string &page, const string &sender, vector<string> &vars, const string &contain );
		mod.at().modFunc("void HttpPost(const string&,string&,const string&,vector<string>&,const string&);",
		    (void (TModule::**)()) &HttpPost);

		((&mod.at())->*HttpPost)(url,answer,sender,vars,request.substr(pos));
#if OSC_DEBUG >= 4
		mess_debug(nodePath().c_str(),"Post Content:\n%s",request.c_str());
#endif
	    }
	    else
	    {
		answer = "<html>\n"
			 " <body>\n"
			 "  <h1>Method Not Implemented</h1>\n"
			 "  <p>The method "+method+" is not implemented by this server.</p>\n"
			 " </body>\n"
			 "</html>\n";
		answer = http_head("501 Method Not Implemented",answer.size())+answer;
	    }
	}
	catch(TError err){ index(answer); }
    }

    return m_nofull||KeepAlive;
}

string TProtIn::w_head( )
{
    return
	"<?xml version='1.0' ?>\n"
	"<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Transitional//EN'\n"
	"'DTD/xhtml1-transitional.dtd'>\n"
	"<html xmlns='http://www.w3.org/1999/xhtml'>\n<head>\n"
	"<meta http-equiv='Content-Type' content='text/html; charset="+Mess->charset()+"'/>\n"
	"<title>"+PACKAGE_NAME+"!"+"</title>\n"
	"</head>\n"
	"<body bgcolor='#818181' text='#000000' link='#3366ff' vlink='#339999' alink='#33ccff'>\n"
	"<h1 align='center'><font color='#ffff00'>"+PACKAGE_NAME+"!</font></h1>\n"
	"<hr width='100%' size='3'/><br/>\n<br/><br/>\n";
}

string TProtIn::w_tail()
{
    return
	"<hr width='100%' size='2'/>\n"
	"</body>\n"
	"</html>\n";
}

void TProtIn::index( string &answer )
{
    answer = w_head()+
	    "<table border='2' align='center' width='40%' bgcolor='#A9A9A9'>\n"
	    "<tr bgcolor='#9999ff'><td><b>"+_("Present web modules")+"</b></td></tr>\n"
	    "<tr bgcolor='#cccccc'><td><ul>\n";
    vector<string> list;
    owner().owner().owner().ui().at().modList(list);
    for( unsigned i_l = 0; i_l < list.size(); i_l++ )
    {
	AutoHD<TModule> mod = owner().owner().owner().ui().at().modAt(list[i_l]);
	if( mod.at().modInfo("SubType") == "WWW" )
	    answer = answer+"<li><a href='"+list[i_l]+"/'>"+mod.at().modInfo("Name")+"</a></li>\n";
    }
    answer = answer+"</ul></td></tr></table>\n"+w_tail();
    answer = http_head("200 OK",answer.size())+answer;
}
