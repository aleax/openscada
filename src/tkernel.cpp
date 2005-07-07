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

TMess	*Mess;
TSYS	*SYS;

TKernel::TKernel( const string &name ) 
	: DefBDType(""), DefBDName(""), m_name(name), s_run(false), m_genDB(true)
{
    Mess->put_s("INFO",TMess::Info,"Create!");
    
    //auto_ptr<TMessage> Mess (new TMess());
    m_param    	= new TParamS(this);
    m_bd     	= new TBDS(this);    
    m_sequrity 	= new TSequrity(this);
    m_transport	= new TTransportS(this);
    m_protocol 	= new TProtocolS(this);
    m_archive  	= new TArchiveS(this);
    m_controller= new TControllerS(this);
    m_special  	= new TSpecialS(this);
    m_ui       	= new TUIS(this);
    m_func	= new TFunctionS(this);
    
    m_modschedul= new TModSchedul(this);
    //Registry modules subsystems
    modSchedul().gmdReg(m_bd);
    modSchedul().gmdReg(m_transport);
    modSchedul().gmdReg(m_protocol);
    modSchedul().gmdReg(m_controller);    
    modSchedul().gmdReg(m_archive);
    modSchedul().gmdReg(m_special);    
    modSchedul().gmdReg(m_ui);    
    
    //Load modules
    modSchedul().load();
    modSchedul().loadLibS();
}

TKernel::~TKernel()
{
    start( false );
    
    delete m_modschedul;
    delete m_ui;
    delete m_param;
    delete m_archive;
    delete m_controller;
    delete m_protocol;
    delete m_transport;
    delete m_special;
    delete m_sequrity;
    delete m_bd;
    delete m_func;
}

void TKernel::load()
{
    mPut("SYS",TMess::Info,"Load!",m_name.c_str());   

    //========== Load commandline data ==================
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
        switch(next_opt)
        {
	    case 'h': fprintf(stdout,optDescr().c_str()); break;
    	    case -1 : break;    	
	}
	//else if(next_opt == 'h') throw TError("Comandline help print!");
    } while(next_opt != -1);
    
    //========== Load options from config file ==================
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

    //================== Load subsystems and modules ============    
    sequrity().load();
    modSchedul().loadAll();
    
    mPut("SYS",TMess::Debug,"Load OK!",m_name.c_str());
}

void TKernel::start( bool val )
{
    if( val == s_run )	return;
    if( val )
    {
	mPut("SYS",TMess::Info,"Start!",m_name.c_str());
	
	func().start( val );
	modSchedul().startAll();
        modSchedul().schedStart();
	s_run = val;
	
	mPut("SYS",TMess::Debug,"Start OK!",m_name.c_str());
    }
    else
    {
	mPut("SYS",TMess::Info,"Stop!",m_name.c_str());    
	
	modSchedul().schedStop( );
	modSchedul().stopAll( );
	func().start( val );
	s_run = val;
	
	mPut("SYS",TMess::Debug,"Stop OK!",m_name.c_str());
    }    
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

string TKernel::cfgNodeName()
{
    return SYS->cfgNodeName()+m_name+"/";
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
void TKernel::cntrCmd_( const string &a_path, XMLNode *opt, int cmd )
{
    if( cmd==TCntrNode::Info )
    {
	ctrMkNode("oscada_cntr",opt,a_path.c_str(),"/",Mess->I18Ns("Kernel: ")+name());
	ctrMkNode("area",opt,a_path.c_str(),"/gen",Mess->I18N("Kernel"),0440);
	ctrMkNode("fld",opt,a_path.c_str(),"/gen/run",Mess->I18N("Run"),0664,0,0,"bool");
	ctrMkNode("fld",opt,a_path.c_str(),"/gen/self",Mess->I18N("Use generic DB"),0664,0,0,"bool");
	ctrMkNode("fld",opt,a_path.c_str(),"/gen/def_tp_bd",Mess->I18N("Default bd(module:bd)"),0664,0,0,"str")->
	    attr_("dest","select")->attr_("select","/gen/b_mod");
	ctrMkNode("fld",opt,a_path.c_str(),"/gen/def_bd","",0664,0,0,"str");
	ctrMkNode("comm",opt,a_path.c_str(),"/gen/load",Mess->I18N("Load kernel"));
	ctrMkNode("area",opt,a_path.c_str(),"/subs",Mess->I18N("Subsystems"));
	ctrMkNode("list",opt,a_path.c_str(),"/subs/br",Mess->I18N("Subsystems"),0555,0,0,"br")->
	    attr_("mode","at")->attr_("br_pref","_");
	ctrMkNode("area",opt,a_path.c_str(),"/help",Mess->I18N("Help"));
	ctrMkNode("fld",opt,a_path.c_str(),"/help/g_help",Mess->I18N("Options help"),0444,0,0,"str")->
	    attr_("cols","90")->attr_("rows","5");
    }
    else if( cmd==TCntrNode::Get )
    {
	if( a_path == "/gen/run" )		ctrSetB( opt, s_run );	
	else if( a_path == "/gen/self" )	ctrSetB( opt, m_genDB );
	else if( a_path == "/gen/def_tp_bd" )	ctrSetS( opt, DefBDType );
	else if( a_path == "/gen/def_bd" )    	ctrSetS( opt, DefBDName );     
	else if( a_path == "/gen/b_mod" )
	{
	    vector<string> list;
	    db().gmdList(list);
	    opt->childClean();
	    for( unsigned i_a=0; i_a < list.size(); i_a++ )
		ctrSetS( opt, list[i_a] );
	}
	else if( a_path.substr(0,8) == "/subs/br" )
	{
	    opt->childClean();
	    ctrSetS( opt, modSchedul().name(),modSchedul().id().c_str() );
	    ctrSetS( opt, sequrity().name(),sequrity().id().c_str() );
	    ctrSetS( opt, archive().gmdName(),archive().gmdId().c_str() );
	    ctrSetS( opt, db().gmdName(),db().gmdId().c_str() );
    	    ctrSetS( opt, controller().gmdName(),controller().gmdId().c_str() );
    	    ctrSetS( opt, protocol().gmdName(),protocol().gmdId().c_str() );
    	    ctrSetS( opt, transport().gmdName(),transport().gmdId().c_str() );
    	    ctrSetS( opt, special().gmdName(),special().gmdId().c_str() );
    	    ctrSetS( opt, param().name(),param().id().c_str() );
    	    ctrSetS( opt, ui().gmdName(),ui().gmdId().c_str() );
    	    ctrSetS( opt, func().name(),func().id().c_str() );
	}
	else if( a_path == "/help/g_help" )        ctrSetS( opt, optDescr() );
	else throw TError("(Kernel:%s)Branch %s error!",name().c_str(),a_path.c_str());
    }
    else if( cmd==TCntrNode::Set )
    {
	if( a_path == "/gen/run" )              start(ctrGetB(opt));
	else if( a_path == "/gen/self" )       	m_genDB = ctrGetB( opt );
	else if( a_path == "/gen/def_tp_bd" )	DefBDType = ctrGetS( opt );
	else if( a_path == "/gen/def_bd" )	DefBDName = ctrGetS( opt ); 
	else if( a_path == "/gen/load" )	load();	
	else throw TError("(Kernel:%s)Branch %s error!",name().c_str(),a_path.c_str());	
    }
}

TCntrNode &TKernel::ctrAt( const string &br )
{
    if(br.substr(0,1)!="_")	throw TError("(Kernel:%s)Branch %s error!",name().c_str(),br.c_str());
    
    string sub = br.substr(1);    
    if( sub == modSchedul().id() )	return modSchedul();
    else if( sub == sequrity().id() )	return sequrity();
    else if( sub == archive().gmdId() )	return archive();
    else if( sub == db().gmdId() )   	return db();
    else if( sub == controller().gmdId() )	return controller();
    else if( sub == protocol().gmdId() )	return protocol();
    else if( sub == transport().gmdId() )	return transport();
    else if( sub == special().gmdId() )	return special();
    else if( sub == param().id() )	return param();
    else if( sub == ui().gmdId() )	return ui();
    else if( sub == func().id() )	return func();
    else throw TError("(Kernel:%s)Branch %s error!",name().c_str(),br.c_str());
}

//==============================================================
//================== Message functions ========================
//==============================================================
void TKernel::mPut( const string &categ, TMess::Type level, char *fmt,  ... )
{
    char str[STR_BUF_LEN];
    va_list argptr;

    va_start (argptr,fmt);
    vsnprintf(str,sizeof(str),fmt,argptr);
    va_end(argptr);
    mPutS( categ, level, str );
}

void TKernel::mPutS( const string &categ, TMess::Type level, const string &mess )
{
    Mess->put( categ, level,"%s:%s",name().c_str(),mess.c_str() );
}

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
