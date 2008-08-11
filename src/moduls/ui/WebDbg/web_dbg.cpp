
//OpenSCADA system module UI.WebDbg file: web_dbg.cpp
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
 
#include <getopt.h>
#include <string>

#include <config.h>
#include <tsys.h>
#include <tmess.h>

#include "web_dbg.h"

//************************************************
//* Modul info!                                  *
#define MOD_ID	    "WebDbg"
#define MOD_NAME    "WEB debuger"
#define MOD_TYPE    "UI"
#define VER_TYPE    VER_UI
#define SUB_TYPE    "WWW"
#define MOD_VERSION "0.1.0"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "Web debug modul."
#define LICENSE     "GPL"
//************************************************

WebDbg::TWEB *WebDbg::mod;

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
	    return new WebDbg::TWEB( source );       
	return NULL;
    }    
}

using namespace WebDbg;

//************************************************
//* TWEB                                         *
//************************************************
TWEB::TWEB( string name ) : n_col(1), h_sz(800),v_sz(300), trnd_len(10), trnd_tm(time(NULL)+31104000)
{
    mId		= MOD_ID;
    mName       = MOD_NAME;
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
}

TWEB::~TWEB()
{

}

string TWEB::modInfo( const string &name )
{
    if( name == "SubType" ) return(SUB_TYPE);
    else return TModule::modInfo(name);
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
	"---------- Parameters of the module section <%s> in config file ----------\n\n"),
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
    
    //- Load parameters from config file -
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

void TWEB::save_( )
{
    //- Save parameters to config file -
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
    if(per_refr) 
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

void TWEB::HttpGet( const string &url, string &page, const string &sender, vector<string> &vars )
{
    string ntrnd = TSYS::pathLev(url,0);
    if( !ntrnd.size() )
    {
	//- Make main page -
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

void TWEB::cntrCmdProc( XMLNode *opt )
{
    //- Get page info -
    if( opt->name() == "info" )
    {
        TUI::cntrCmdProc(opt);
        if(ctrMkNode("area",opt,1,"/prm/cfg",_("Module options")))
	{
	    ctrMkNode("list",opt,-1,"/prm/cfg/trnds",_("Display parameter atributes trends"),0664,"root","root",1,"s_com","add,del");
	    ctrMkNode("fld",opt,-1,"/prm/cfg/col",_("Collums"),0664,"root","root",1,"tp","dec");
	    ctrMkNode("fld",opt,-1,"/prm/cfg/hsize",_("Horizontal trend size (pixel)"),0664,"root","root",1,"tp","dec");
	    ctrMkNode("fld",opt,-1,"/prm/cfg/vsize",_("Vertical trend size (pixel)"),0664,"root","root",1,"tp","dec");
	    ctrMkNode("fld",opt,-1,"/prm/cfg/trnd_tm",_("Trend start time (sec)"),0664,"root","root",1,"tp","time");
	    ctrMkNode("fld",opt,-1,"/prm/cfg/trnd_len",_("Trend length (sec)"),0664,"root","root",1,"tp","dec");	
	}
        ctrMkNode("fld",opt,-1,"/help/g_help",_("Options help"),0440,"root","root",3,"tp","str","cols","90","rows","5");
        return;
    }

    //- Process command to page -
    string a_path = opt->attr("path");
    if( a_path == "/prm/cfg/trnds" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )
            for( unsigned i_el=0; i_el < trnd_lst.size(); i_el++ )
		opt->childAdd("el")->setText(trnd_lst[i_el]);
	if( ctrChkNode(opt,"add",0664,"root","root",SEQ_WR) )	{ trnd_lst.push_back(opt->text()); modif(); }
	if( ctrChkNode(opt,"del",0664,"root","root",SEQ_WR) )
	    for( unsigned i_el=0; i_el < trnd_lst.size(); i_el++ )
		if( trnd_lst[i_el] == opt->text() )
		{
		    trnd_lst.erase(trnd_lst.begin()+i_el);
		    modif();
		    break;
		}
    }
    else if( a_path == "/prm/cfg/col" )
    {
	if( ctrChkNode(opt,"get",0664) )			opt->setText( TSYS::int2str(nCol()) );
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	setNCol( atoi(opt->text().c_str()) );
    }    
    else if( a_path == "/prm/cfg/hsize" )
    {
	if( ctrChkNode(opt,"get",0664) )			opt->setText( TSYS::int2str(hSize()) );
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	setHSize( atoi(opt->text().c_str()) );
    }
    else if( a_path == "/prm/cfg/vsize" )	
    {
	if( ctrChkNode(opt,"get",0664) )			opt->setText( TSYS::int2str(vSize()) );
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	setVSize( atoi(opt->text().c_str()) );
    }
    else if( a_path == "/prm/cfg/trnd_tm" )	
    {
	if( ctrChkNode(opt,"get",0664) )			opt->setText( TSYS::int2str(trndTm()) );
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	setTrndTm( atoi(opt->text().c_str()) );
    }
    else if( a_path == "/prm/cfg/trnd_len" )
    {
	if( ctrChkNode(opt,"get",0664) )			opt->setText( TSYS::int2str(trndLen()) );
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	setTrndLen( atoi(opt->text().c_str()) );
    }		
    else if( a_path == "/help/g_help" && ctrChkNode(opt,"get",0440) )	opt->setText(optDescr());
    else TUI::cntrCmdProc(opt);
}		    
