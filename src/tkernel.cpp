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
 
#include <stdarg.h>
#include <stdlib.h>
#include <langinfo.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <string>
#include <iostream>
#include <new>

#include "../config.h"
#include "terror.h"
#include "tmessage.h"
#include "tbds.h"
#include "ttransports.h"
#include "tprotocols.h"
#include "tarchives.h"
#include "tcontrollers.h"
#include "tspecials.h"
#include "tparams.h"
#include "tuis.h"
#include "tmodschedul.h"
#include "tsequrity.h"
#include "tfunctions.h"
#include "tsys.h"
#include "tkernel.h"

TMessage   *Mess;
TSYS       *SYS;

const char *TKernel::o_name = "TKernel";

TKernel::TKernel( const string &name ) 
	: DefBDType(""), DefBDName(""), m_name(name), s_run(false), m_genDB(true)
{
    mPutS("INFO",MESS_INFO,"Create!");
    
    //auto_ptr<TMessage> Mess (new TMessage());
    param    	= new TParamS(this);
    bd 	     	= new TBDS(this);    
    sequrity 	= new TSequrity(this);
    transport 	= new TTransportS(this);
    protocol 	= new TProtocolS(this);
    archive  	= new TArchiveS(this);
    controller  = new TControllerS(this);
    special  	= new TSpecialS(this);
    ui       	= new TUIS(this);
    m_func	= new TFunctionS(this);
    
    modschedul  = new TModSchedul(this);
    ModSchedul().gmdReg(bd);
    ModSchedul().gmdReg(transport);
    ModSchedul().gmdReg(protocol);
    ModSchedul().gmdReg(controller);    
    ModSchedul().gmdReg(archive);
    ModSchedul().gmdReg(special);    
    ModSchedul().gmdReg(ui);    
}

TKernel::~TKernel()
{
    mPutS("INFO",MESS_INFO,"Destroy!");    
    
    vector<string> m_l;
    //Stop all controllers   //????
    Controller().gmdList(m_l);
    for( unsigned i_m = 0; i_m < m_l.size(); i_m++)
    {
	vector<string> c_l;
	((TTipController &)Controller().gmdAt(m_l[i_m]).at()).list(c_l);
	for( unsigned i_c = 0; i_c < c_l.size(); i_c++)
	{
	    AutoHD<TController> cntr = ((TTipController &)Controller().gmdAt(m_l[i_m]).at()).at(c_l[i_c]);
	    if(cntr.at().startStat()) cntr.at().stop();
	}
    }							    
    
    //Disable all controllers   //????
    for( unsigned i_m = 0; i_m < m_l.size(); i_m++)
    {
	vector<string> c_l;
	((TTipController &)Controller().gmdAt(m_l[i_m]).at()).list(c_l);
	for( unsigned i_c = 0; i_c < c_l.size(); i_c++)
	{
	    AutoHD<TController> cntr = ((TTipController &)Controller().gmdAt(m_l[i_m]).at()).at(c_l[i_c]);
	    if(cntr.at().enableStat()) cntr.at().disable();
	}
    }
    
    delete modschedul;
    delete ui;
    delete param;
    delete archive;
    delete controller;
    delete protocol;
    delete transport;
    delete special;
    delete sequrity;
    delete bd;
    delete m_func;
}

int TKernel::run()
{
    if(s_run) throw TError("(%s) Kernel already started!",o_name);
    
    mPut("INFO",MESS_INFO,"Start!",m_name.c_str());

    try
    {
	checkCommandLine();
	updateOpt();
	
	ModSchedul().gmdLoadAll();
	checkCommandLine(true);   //check help, error and exit
	
	Sequrity().init();
	ModSchedul().gmdInitAll();		
	ModSchedul().gmdStartAll();	
	ModSchedul().schedStart();	
    } 
    catch(TError error) 
    { 
	mPut("SYS",MESS_CRIT,"Run exception: %s",error.what().c_str()); 
	return(-1); 
    }
    catch(...)
    { return(-2); }

    s_run = true;
    //Start signal listen
    return(0);
}


string TKernel::optDescr( )
{
    char buf[STR_BUF_LEN];
    snprintf(buf,sizeof(buf),Mess->I18N(
	"========================= The kernel options ==============================\n"
    	"------------- The config file kernel <%s> parameters ------------\n"
    	"DefaultBD <type:name>  set default bd type and bd name (next, may use only table name);\n\n"
	),name().c_str());

    return(buf);
}


void TKernel::checkCommandLine( bool mode )
{
#if OSC_DEBUG
    mPutS("DEBUG",MESS_INFO,"Read commandline options!");
#endif
	
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{"help"     ,0,NULL,'h'},
	{NULL       ,0,NULL,0  }
    };

    optind=opterr=0;	 
    do
    {
	next_opt=getopt_long(SYS->argc,( char *const * ) SYS->argv,short_opt,long_opt,NULL);
	if(mode==false)
	{
    	    switch(next_opt)
    	    {
    		case 'h': fprintf(stdout,optDescr().c_str()); break;
    		case -1 : break;
    	    }
	}
	else if(next_opt == 'h') throw TError("Comandline help print!");
    } while(next_opt != -1);

    if( mode == false )
    {
	Sequrity().checkCommandLine();
	ModSchedul().checkCommandLine(); 
	ModSchedul().checkCommandLineMod(); 
    }
    
#if OSC_DEBUG
    mPutS("DEBUG",MESS_DEBUG,"Read commandline options ok!");
#endif
}

void TKernel::updateOpt()
{
#if OSC_DEBUG
    mPutS("DEBUG",MESS_INFO,"Read config options!");
#endif
    string opt;        
    try
    {
	opt = cfgNode()->childGet("id","DefaultBD")->text();
	if( opt.size() )
    	{
    	    int pos = 0;
    	    DefBDType = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
    	    DefBDName = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
	}
    }
    catch(...) {  }

    Sequrity().updateOpt();
    ModSchedul().updateOpt();
    ModSchedul().updateOptMod();    
    
#if OSC_DEBUG
    mPutS("DEBUG",MESS_DEBUG,"Read config options ok!");
#endif
}

XMLNode *TKernel::cfgNode()
{    
    int i_k = 0;
    while(true)
    {
	XMLNode *t_n = SYS->cfgNode()->childGet("kernel",i_k++); 
	if( t_n->attr("id") == m_name) return( t_n );
    }
}

//==============================================================
//================== Controll functions ========================
//==============================================================
void TKernel::ctrStat_( XMLNode *inf )
{
    char *i_cntr = 
    	"<oscada_cntr>"
	 "<area id='gen' acs='0440'>"
	  "<fld id='self' tp='bool'/>"
	  "<fld id='def_tp_bd' tp='str' dest='select' select='/gen/b_mod'/>"
	  "<fld id='def_bd' tp='str'/>"
	  "<comm id='run'/>"
	  "<comm id='upd_opt'/>"
	 "</area>"
         "<area id='subs'>"
          "<list id='br' tp='br' mode='at' br_pref='_' acs='0555'/>"
         "</area>"				
         "<area id='help'>"
	  "<fld id='g_help' acs='0444' tp='str' cols='90' rows='5'/>"
         "</area>"			
	"</oscada_cntr>";
    char *dscr = "dscr";
    
    inf->load( i_cntr );
    inf->text(Mess->I18Ns("Kernel: ")+name());
    //gen
    XMLNode *c_nd = inf->childGet(0);
    c_nd->attr(dscr,Mess->I18N("Kernel"));
    c_nd->childGet(0)->attr(dscr,Mess->I18N("Use generic DB"));
    c_nd->childGet(1)->attr(dscr,Mess->I18N("Default bd(module:bd)"));
    if( !s_run ) c_nd->childGet(3)->attr(dscr,Mess->I18N("Run"));
    else c_nd->childGet(3)->attr("acs","0");
    c_nd->childGet(4)->attr(dscr,Mess->I18N("Update options(from config)"));    
    c_nd = inf->childGet(1);
    c_nd->attr(dscr,Mess->I18N("Subsystems"));
    c_nd->childGet(0)->attr(dscr,Mess->I18N("Subsystems"));
    c_nd = inf->childGet(2);
    c_nd->attr(dscr,Mess->I18N("Help"));
    c_nd->childGet(0)->attr(dscr,Mess->I18N("Options help"));	       
}

void TKernel::ctrDinGet_( const string &a_path, XMLNode *opt )
{    
    if( a_path == "/gen/self" ) 		ctrSetB( opt, m_genDB );
    else if( a_path == "/gen/def_tp_bd" )	ctrSetS( opt, DefBDType );
    else if( a_path == "/gen/def_bd" )    	ctrSetS( opt, DefBDName );     
    else if( a_path == "/gen/b_mod" )
    {
	vector<string> list;
	BD().gmdList(list);
	opt->childClean();
	for( unsigned i_a=0; i_a < list.size(); i_a++ )
	    ctrSetS( opt, list[i_a] );
    }
    else if( a_path.substr(0,8) == "/subs/br" )
    {
	opt->childClean();
	ctrSetS( opt, ModSchedul().name(),"0" );
	ctrSetS( opt, Sequrity().name()  ,"1" );
	ctrSetS( opt, Archive().name()   ,"2" );
	ctrSetS( opt, BD().name()        ,"3" );
	ctrSetS( opt, Controller().name(),"4" );
	ctrSetS( opt, Protocol().name()  ,"5" );
	ctrSetS( opt, Transport().name() ,"6" );
	ctrSetS( opt, Special().name()   ,"7" );
	ctrSetS( opt, Param().name()     ,"8" );
	ctrSetS( opt, UI().name()        ,"9" );
	ctrSetS( opt, func().name()      ,"10" );
    }
    else if( a_path == "/help/g_help" )        ctrSetS( opt, optDescr() );
    else throw TError("(%s) Branch %s error!",o_name,a_path.c_str());
} 

void TKernel::ctrDinSet_( const string &a_path, XMLNode *opt )
{
    if( a_path == "/gen/self" )                 m_genDB = ctrGetB( opt );
    else if( a_path == "/gen/def_tp_bd" )	DefBDType = ctrGetS( opt );
    else if( a_path == "/gen/def_bd" )	DefBDName = ctrGetS( opt ); 
    else if( a_path == "/gen/run" )	run();
    else if( a_path == "/gen/upd_opt" ) updateOpt();	
    else throw TError("(%s) Branch %s error!",o_name,a_path.c_str());
}

TCntrNode &TKernel::ctrAt( const string &br )
{
    if(br.substr(0,1)!="_")	throw TError("<{%s}> Branch %s error!",__func__,br.c_str());
    
    switch( atoi(br.substr(1).c_str()) )
    {
	case 0: return( ModSchedul() );
	case 1: return( Sequrity() );
	case 2: return( Archive() );
	case 3: return( BD() ) ;
	case 4: return( Controller() );
	case 5: return( Protocol() );
	case 6: return( Transport() );
	case 7: return( Special() );
	case 8: return( Param() );
	case 9: return( UI() );
	case 10: return( func() );
	default: throw TError("<{%s}> Branch %s error!",__func__,br.c_str());
    }
}

//==============================================================
//================== Message functions ========================
//==============================================================
void TKernel::mPut( const string &categ, int level, char *fmt,  ... )
{
    char str[STR_BUF_LEN];
    va_list argptr;

    va_start (argptr,fmt);
    vsnprintf(str,sizeof(str),fmt,argptr);
    va_end(argptr);
    mPutS( categ, level, str );
}

void TKernel::mPutS( const string &categ, int level, const string &mess )
{
    Mess->put_s( categ, level, name()+":"+mess );
}

//DB 
TBDS::SName TKernel::nameDBPrep( const TBDS::SName &nbd )
{
    TBDS::SName bd = nbd;
    
    if( !bd.tp.size() || !bd.bd.size() || genDB() )
    {
	bd.tp = DefBDType;
	bd.bd = DefBDName;
    }
    
    return bd;
}
