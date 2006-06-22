
//OpenSCADA system module UI.WebDbg file: web_dbg.cpp
/***************************************************************************
 *   Copyright (C) 2004-2006 by Roman Savochenko                           *
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
#include <string>

#include <config.h>
#include <tsys.h>
#include <tmess.h>

#include "web_dbg.h"

//============ Modul info! =====================================================
#define MOD_ID	    "WebDbg"
#define MOD_NAME    "WEB debuger"
#define MOD_TYPE    "UI"
#define VER_TYPE    VER_UI
#define SUB_TYPE    "WWW"
#define MOD_VERSION "0.0.3"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "Web debug modul."
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
	WebDbg::TWEB *self_addr = NULL;

	if( AtMod.id == MOD_ID && AtMod.type == MOD_TYPE && AtMod.t_ver == VER_TYPE )
	    self_addr = new WebDbg::TWEB( source );       

	return ( self_addr );
    }    
}

using namespace WebDbg;

//==============================================================================
//================ WebDbg::TWEB ================================================
//==============================================================================
TWEB::TWEB( string name ) : h_sz(800),v_sz(300), trnd_len(10), trnd_tm(time(NULL)+31104000)
{
    mId		= MOD_ID;
    mName       = MOD_NAME;
    mType	= MOD_TYPE;
    mVers	= MOD_VERSION;
    mAutor	= AUTORS;
    mDescr	= DESCRIPTION;
    mLicense	= LICENSE;
    mSource	= name;
    
    //Reg export functions
    modFuncReg( new ExpFunc("void HttpGet(const string&,string&,const string&,vector<string>&);",
        "Process Get comand from http protocol's!",(void(TModule::*)( )) &TWEB::HttpGet) );
    modFuncReg( new ExpFunc("void HttpPost(const string&,string&,const string&,vector<string>&,const string&);",
        "Process Set comand from http protocol's!",(void(TModule::*)( )) &TWEB::HttpPost) );				
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

    snprintf(buf,sizeof(buf),I18N(
	"======================= The module <%s:%s> options =======================\n"
	"---------- Parameters of the module section <%s> in config file ----------\n\n"),
	MOD_TYPE,MOD_ID,nodePath().c_str());

    return(buf);
}

void TWEB::modLoad( )
{
    //========== Load parameters from command line ============
    int next_opt;
    char *short_opt="h";
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
	    case 'h': fprintf(stdout,optDescr().c_str()); break;
	    case -1 : break;
	}
    } while(next_opt != -1);    
    
    //========== Load parameters from config file =============
    string trnds = TBDS::genDBGet(nodePath()+"Trends");
    string trnd_el;
    int el_cnt = 0;
    while( (trnd_el=TSYS::strSepParse(trnds,el_cnt++,';')).size())
	trnd_lst.push_back(trnd_el);
    h_sz = atoi(TBDS::genDBGet(nodePath()+"h_sz",TSYS::int2str(h_sz)).c_str());
    v_sz = atoi(TBDS::genDBGet(nodePath()+"v_sz",TSYS::int2str(v_sz)).c_str());
    trnd_len = atoi(TBDS::genDBGet(nodePath()+"trnd_len",TSYS::int2str(trnd_len)).c_str());
    trnd_tm  = atoi(TBDS::genDBGet(nodePath()+"trnd_tm",TSYS::int2str(trnd_tm)).c_str());
}

void TWEB::modSave( )
{
    //========== Save parameters to config file =============
    string trnds;
    for(int i_el = 0; i_el < trnd_lst.size(); i_el++ )
        trnds+=trnd_lst[i_el]+";";
    TBDS::genDBSet(nodePath()+"Trends",trnds);
    TBDS::genDBSet(nodePath()+"h_sz",TSYS::int2str(h_sz));
    TBDS::genDBSet(nodePath()+"v_sz",TSYS::int2str(v_sz));
    TBDS::genDBSet(nodePath()+"trnd_len",TSYS::int2str(trnd_len));
    TBDS::genDBSet(nodePath()+"trnd_tm",TSYS::int2str(trnd_tm));
}

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
    return
	"<?xml version='1.0' ?>\n"
	"<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Transitional//EN'\n"
	"'DTD/xhtml1-transitional.dtd'>\n"
	"<html xmlns='http://www.w3.org/1999/xhtml'>\n<head>\n"
        "<meta http-equiv='Content-Type' content='text/html; charset="+Mess->charset()+"'/>\n"
	"  <title>OpenSCADA debug web modul!</title>\n"
        "</head>\n"
        "<body bgcolor='#818181' text='#000000' link='#3366ff' vlink='#339999' alink='#33ccff'>\n"
	"  <h1 align=\"center\"><font color=\"#ffff00\"> Welcome to OpenSCADA debug web modul!</font></h1>\n"
	"  <hr width=\"100%\" size=\"2\">\n"	
        "<hr width='100%' size='3'/><br/>\n";
}

string TWEB::w_tail( )
{
    return
        "<hr width='100%' size='3'/>\n"
        "</body>\n"
        "</html>";
}

void TWEB::HttpGet( const string &url, string &page, const string &sender, vector<string> &vars )
{
    string ntrnd = TSYS::pathLev(url,0);
    if( !ntrnd.size() )
    {
	//Make main page
	page = w_head();
	for(int i_el = 0; i_el < trnd_lst.size(); i_el++ )
	{ 
	    try
	    {		
		if( (dynamic_cast<TVal *>(&SYS->nodeAt(trnd_lst[i_el],0,'.').at()) &&
		    !dynamic_cast<TVal&>(SYS->nodeAt(trnd_lst[i_el],0,'.').at()).arch().freeStat()) ||
		    dynamic_cast<TVArchive *>(&SYS->nodeAt(trnd_lst[i_el],0,'.').at()) )
		{
		    page = page+"<b>"+trnd_lst[i_el]+"</b><br/>\n";
		    page = page+"<img src='/"+MOD_ID+"/"+TSYS::int2str(i_el)+"' border='0'/><br/>\n";
		}
	    }catch(...)	{ }
	}	
	page = page+w_tail();
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
	    long long v_beg = ((trnd_tm+trnd_len)>time(NULL))?time(NULL)-trnd_len:trnd_tm;
	    long long v_end = v_beg+trnd_len;
	
	    page = arch.at().makeTrendImg(v_beg*1000000,v_end*1000000,"",h_sz, v_sz );
	}
	page = http_head("200 OK",page.size(),string("image/png"))+page;
    }    
}

void TWEB::HttpPost( const string &url, string &page, const string &sender, vector<string> &vars, const string &contein )
{

}

//================== Controll functions ========================
void TWEB::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    if( cmd==TCntrNode::Info )
    {
	TUI::cntrCmd_( a_path, opt, cmd );
		
        ctrMkNode("area",opt,1,a_path.c_str(),"/prm/cfg",I18N("Module options"));
	ctrMkNode("list",opt,-1,a_path.c_str(),"/prm/cfg/trnds",Mess->I18N("Display parameter atributes trends"),0664,0,0,1,"s_com","add,del");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/cfg/hsize",I18N("Horizontal trend size (pixel)"),0664,0,0,1,"tp","dec");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/cfg/vsize",I18N("Vertical trend size (pixel)"),0664,0,0,1,"tp","dec");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/cfg/trnd_tm",I18N("Trend start time (sec)"),0664,0,0,1,"tp","time");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/cfg/trnd_len",I18N("Trend length (sec)"),0664,0,0,1,"tp","dec");	
        ctrMkNode("comm",opt,-1,a_path.c_str(),"/prm/cfg/load",I18N("Load"));
        ctrMkNode("comm",opt,-1,a_path.c_str(),"/prm/cfg/save",I18N("Save"));
        ctrMkNode("fld",opt,-1,a_path.c_str(),"/help/g_help",I18N("Options help"),0440,0,0,3,"tp","str","cols","90","rows","5");
    }
    else if( cmd==TCntrNode::Get )
    {
	if( a_path == "/prm/cfg/trnds" )
	{
            opt->childClean();
            for( unsigned i_el=0; i_el < trnd_lst.size(); i_el++ )
                ctrSetS( opt, trnd_lst[i_el] );
        }
	else if( a_path == "/prm/cfg/hsize" )	ctrSetI( opt, h_sz );
	else if( a_path == "/prm/cfg/vsize" )	ctrSetI( opt, v_sz );
	else if( a_path == "/prm/cfg/trnd_tm" )	ctrSetI( opt, trnd_tm );
	else if( a_path == "/prm/cfg/trnd_len" )ctrSetI( opt, trnd_len );		
        else if( a_path == "/help/g_help" )     ctrSetS( opt, optDescr() );
        else TUI::cntrCmd_( a_path, opt, cmd );
    }
    else if( cmd==TCntrNode::Set )
    {
	if( a_path == "/prm/cfg/trnds" )
        {
	    if( opt->name() == "add" )	trnd_lst.push_back(opt->text());
	    else if( opt->name() == "del" )
		for( unsigned i_el=0; i_el < trnd_lst.size(); i_el++ )
		    if( trnd_lst[i_el] == opt->text() )
		    {
			trnd_lst.erase(trnd_lst.begin()+i_el);
			break;
		    }	    
	}
	else if( a_path == "/prm/cfg/hsize" )	h_sz = ctrGetI( opt );
        else if( a_path == "/prm/cfg/vsize" )	v_sz = ctrGetI( opt );
	else if( a_path == "/prm/cfg/trnd_tm" )	trnd_tm = ctrGetI( opt );	
        else if( a_path == "/prm/cfg/trnd_len" )trnd_len = ctrGetI( opt );	
	else if( a_path == "/prm/cfg/load" )    modLoad();
        else if( a_path == "/prm/cfg/save" )    modSave();
	else TUI::cntrCmd_( a_path, opt, cmd );
    }
}
