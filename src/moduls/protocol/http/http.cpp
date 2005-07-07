/***************************************************************************
 *   Copyright (C) 2004 by Roman Savochenko                                *
 *   rom_as@fromru.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
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

#include <config.h>
#include <tsys.h>
#include <tkernel.h>
#include <tmessage.h>
#include <tmodule.h>
#include <tuis.h>
#include "http.h"

//============ Modul info! =====================================================
#define MOD_ID      "http"
#define MOD_NAME    "HTTP"
#define MOD_TYPE    "Protocol"
#define VER_TYPE    VER_PROT
#define M_VERSION   "1.1.0"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "Allow support HTTP for WWW based UIs."
#define LICENSE     "GPL"
//==============================================================================

extern "C"
{
    TModule::SAt module( int n_mod )
    {
	TModule::SAt AtMod;

	if(n_mod==0)
	{
	    AtMod.id	= MOD_ID;
	    AtMod.type  = MOD_TYPE;
	    AtMod.t_ver = VER_TYPE;
	}
	else
	    AtMod.id	= "";

	return( AtMod );
    }

    TModule *attach( const TModule::SAt &AtMod, const string &source )
    {
	pr_http::TProt *self_addr = NULL;

	if( AtMod.id == MOD_ID && AtMod.type == MOD_TYPE && AtMod.t_ver == VER_TYPE )
    	    self_addr = new pr_http::TProt( source );

	return ( self_addr );
    }
}

using namespace pr_http;

//================================================================
//=========== TProt ==============================================
//================================================================
TProt::TProt( string name )
{
    mId 	= MOD_ID;
    mType  	= MOD_TYPE;
    mName       = MOD_NAME;
    Vers      	= M_VERSION;
    Autors    	= AUTORS;
    DescrMod  	= DESCRIPTION;
    License   	= LICENSE;
    Source    	= name;
}

TProt::~TProt()
{

}

string TProt::optDescr( )
{
    char buf[STR_BUF_LEN];
    snprintf(buf,sizeof(buf),I18N(
        "======================= The module <%s:%s> options =======================\n"
        "---------- Parameters of the module section <%s> in config file ----------\n\n"),
	MOD_TYPE,MOD_ID,MOD_ID);

    return(buf);
}

void TProt::modLoad( )
{
    //========== Load parameters from command line ============
    int next_opt;
    char *short_opt="h";
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
	    case 'h': fprintf(stdout,optDescr().c_str()); break;
	    case -1 : break;
	}
    } while(next_opt != -1);

    //========== Load parameters from config file =============
}

TProtocolIn *TProt::in_open( const string &name )
{
    return( new TProtIn(name,this) );
}

//================================================================
//=========== TProtIn ============================================
//================================================================
TProtIn::TProtIn( string name, TProtocol *owner) : TProtocolIn(name,owner), m_nofull(false)
{

}

TProtIn::~TProtIn()
{

}

string TProtIn::w_ok( )
{
    return("HTTP/1.0 200 OK\nContent-type: text/html; charset="+Mess->charset()+"\n\n");
}

char *TProtIn::bad_request_response =
    "HTTP/1.0 400 Bad Request\n"
    "Content-type: text/html\n"
    "\n"
    "<html>\n"
    " <body>\n"
    "  <h1>Bad Request</h1>\n"
    "  <p>This server did not undersand your request.</p>\n"
    " </body>\n"
    "</html>\n";
    
char *TProtIn::not_found_response_template = 
    "HTTP/1.0 404 Not Found\n"
    "Content-type: text/html\n"
    "\n"
    "<html>\n"
    " <body>\n"
    "  <h1>Not Found</h1>\n"    
    "  <p> %s </p>\n"
    " </body>\n"
    "</html>\n";

char *TProtIn::bad_method_response_template =
    "HTTP/1.0 501 Method Not Implemented\n"
    "Content-type: text/html\n"
    "\n"
    "<html>\n"
    " <body>\n"
    "  <h1>Method Not Implemented</h1>\n"
    "  <p>The method %s is not implemented by this server.</p>\n"
    " </body>\n"
    "</html>\n";


bool TProtIn::mess( const string &reqst, string &answer, const string &sender )
{
    char buf[1024];
    int hd = -1; 
    string req;
    vector<string> vars;    
    
    //Continue for full reqst
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
	request[request.size()] = '\0';
	//Mess->put("DEBUG",TMess::Debug,"Content: <%s>!",request.c_str());
	
	//Parse first record
	req     = request.substr(0,request.find("\n",0)-1);
	string method   = req.substr(pos,req.find(" ",pos)-pos); pos = req.find(" ",pos)+1;
	string url      = req.substr(pos,req.find(" ",pos)-pos); pos = req.find(" ",pos)+1;
	string protocol = req.substr(pos,req.find(" ",pos)-pos); pos = req.find(" ",pos)+1;	
	
	//Parse all next records to content
	string   bound;
	int      c_lng=-1;
	request = request.substr(request.find("\n",0)+1);	
	req     = request.substr(0,request.find("\n",0)-1);
	do
	{
	    pos = req.find(":");
    	    if( pos == string::npos ) break;
    	    string var = req.substr(0,pos++);	    
    	    if( var == "Content-Type" )
	    {	
		if( c_lng < 0 ) c_lng = 0;
		//Check full post message
		pos = req.find("boundary=",pos)+strlen("boundary=");
		bound = req.substr(pos);
	    }
	    else if( var == "Content-Length" || var == "Content-length" )
		c_lng = atoi(req.substr(pos).c_str());
	    
	    vars.push_back( req );	        	    
	    request = request.substr(request.find("\n")+1);	
    	    req     = request.substr(0,request.find("\n")-1);
	}
	while( request.size() );
	//Check content length
	if( c_lng >= 0 )
	{
	    pos = request.find(bound);		
	    if( pos == string::npos || c_lng > (request.size()-pos+2) ) m_nofull = true; 	
	}
	if( m_nofull ) return(m_nofull);
	
	//Check protocol version	
	if( protocol != "HTTP/1.0" && protocol != "HTTP/1.1" )
	{
	    answer = bad_request_response;
	    return(m_nofull);
	}
	TUIS &ui = owner().owner().owner().ui();
	if( url[0] != '/' ) url[0] = '/';
	string name_mod = url.substr(1,url.find("/",1)-1);
	
        try
	{ 
	    AutoHD<TModule> mod = ui.gmdAt(name_mod);
	    if( mod.at().modInfo("SubType") != "WWW" )
		throw TError("%s: find no WWW subtype module!",MOD_ID);
	    
    	    //Check metods
    	    int n_dir = url.find("/",1);
    	    if( n_dir == string::npos ) url = "/";
    	    else                        url = url.substr(n_dir,url.size()-n_dir);
    	    if( method == "GET" )
    	    {
		void(TModule::*HttpGet)( const string &url, string &page, const string &sender, vector<string> &vars);
	 	mod.at().modFunc("void HttpGet(const string&,string&,const string&,vector<string>&);",
		    (void (TModule::**)()) &HttpGet);
		
		answer = w_ok();
		((&mod.at())->*HttpGet)(url,answer,sender,vars);
		//Mess->put("DEBUG",TMess::Debug,"Get Content: <%s>!",request.c_str());
	    }
	    else if( method == "POST" ) 
	    {
		void(TModule::*HttpPost)( const string &url, string &page, const string &sender, vector<string> &vars, const string &contein );
	 	mod.at().modFunc("void HttpPost(const string&,string&,const string&,vector<string>&,const string&);",
		    (void (TModule::**)()) &HttpPost);		
		    
		answer = w_ok();
		((&mod.at())->*HttpPost)(url,answer,sender,vars,request);
		//owner().mPut("DEBUG",TMess::Debug,"Post Content: <%s>!",request.c_str());
	    }
	    else
	    {
		snprintf(buf,sizeof(buf),bad_method_response_template,method.c_str());
		answer = buf;
	    }
	}
	catch(TError err){ index(answer); }	
    }

    return(m_nofull);
}

string TProtIn::w_head( )
{
    return(
	"<?xml version='1.0' ?>\n"
	"<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Transitional//EN'\n"
	"'DTD/xhtml1-transitional.dtd'>\n"
	//"<!DOCTYPE html PUBLIC '-//W3C//DTD HTML 4.01 Transitional//EN'\n"
	//"'http://www.w3.org/TR/html4/strict.dtd'>\n"
	"<html xmlns='http://www.w3.org/1999/xhtml'>\n<head>\n"
    	"<meta http-equiv='Content-Type' content='text/html; charset="+Mess->charset()+"'/>\n"
    	"<title>"+PACKAGE_NAME+"!"+"</title>\n"
	"</head>\n");
}

string TProtIn::w_body( )
{
    return(string("")+"<body bgcolor='#818181' text='#000000' link='#3366ff' vlink='#339999' alink='#33ccff'>\n"
    	"<h1 align='center'><font color='#ffff00'>"+PACKAGE_NAME+"!</font></h1>\n"
    	"<hr width='100%' size='3'/><br/>\n<br/><br/>\n");
}

char *TProtIn::w_head_ =
    "</html>\n";

char *TProtIn::w_body_ =
    "<hr width='100%' size='2'/>\n"
    "</body>\n";         

void TProtIn::index( string &answer )
{ 
    answer = w_ok()+w_head()+w_body()+
	    "<table border='2' align='center' width='40%' bgcolor='#A9A9A9'>\n"
	    "<tr bgcolor='#9999ff'><td><b>"+owner().I18N("Avoid web modules")+"</b></td></tr>\n"
	    "<tr bgcolor='#cccccc'><td><ul>\n";
    vector<string> list;
    TUIS &ui = owner().owner().owner().ui();
    ui.gmdList(list);
    for( unsigned i_l = 0; i_l < list.size(); i_l++ )
    {
	AutoHD<TModule> mod = ui.gmdAt(list[i_l]);
	if( mod.at().modInfo("SubType") == "WWW" )
	    answer = answer+"<li><a href='"+list[i_l]+"'>"+mod.at().modInfo("Name")+"</a></li>\n";
    }     
    answer = answer+"</ul></td></tr></table>\n"+w_body_+w_head_;
}
