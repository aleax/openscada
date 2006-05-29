
//OpenSCADA system file: tdaqs.cpp
/***************************************************************************
 *   Copyright (C) 2003-2006 by Roman Savochenko                           *
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

#include "tsys.h"
#include "tmess.h"
#include "tcontroller.h"
//#include "ttipdaq.h"
#include "tmodule.h"
#include "tvalue.h"
#include "tdaqs.h"


TDAQS::TDAQS( ) : TSubSYS("DAQ","Data acquisition",true), el_err("Error")
{
    //Error atributes
    el_err.fldAdd( new TFld("err",Mess->I18N("Error"),TFld::String,FLD_NWR|FLD_DRD) );
}

TDAQS::~TDAQS( )
{

}

void TDAQS::preDisable(int flag)
{
    subStop();
}

void TDAQS::subLoad( )
{
    //========== Load parameters from command line ============
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{"help"       ,0,NULL,'h'},
	{NULL         ,0,NULL,0  }
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
    
    //========== Load parameters =============
    
    //Load from DB
    try
    {
	AutoHD<TTipDAQ> wmod;
	vector<string> mod_ls, tdb_ls, db_ls;
	
	modList(mod_ls);
	for( int i_md = 0; i_md < mod_ls.size(); i_md++ )
	{
	    wmod = at(mod_ls[i_md]);
	    TConfig g_cfg(&wmod.at());
	
	    //Search and create new controllers
	    SYS->db().at().modList(tdb_ls);	
	    for( int i_tp = 0; i_tp < tdb_ls.size(); i_tp++ )
    	    {
        	SYS->db().at().at(tdb_ls[i_tp]).at().list(db_ls);
        	for( int i_db = 0; i_db < db_ls.size(); i_db++ )
        	{
		    string wbd = tdb_ls[i_tp]+"."+db_ls[i_db];
		    int fld_cnt=0;
		    while( SYS->db().at().dataSeek(wbd+"."+subId()+"_"+wmod.at().modId(),nodePath()+"DAQ/",fld_cnt++,g_cfg) )
		    {
			try
            		{
			    string m_id = g_cfg.cfg("ID").getS();
			    if( !wmod.at().present(m_id) )
				wmod.at().add(m_id,(wbd==SYS->workDB())?"*.*":wbd);
			}catch(TError err) { Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); }
            		g_cfg.cfg("ID").setS("");
		    }
		}
	    }
	    //Load present controllers
	    wmod.at().list(tdb_ls);
	    for( int i_c = 0; i_c < tdb_ls.size(); i_c++ )
		wmod.at().at(tdb_ls[i_c]).at().load();
	}
    }catch(TError err) { Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); }
    
    //Load modules
    TSubSYS::subLoad( );
}

void TDAQS::subSave(  )
{	
    //========== Save parameters =============

    //Save all controllers    
    vector<string> m_l, c_l;
    modList(m_l);
    for( unsigned i_m = 0; i_m < m_l.size(); i_m++)
    {
	at(m_l[i_m]).at().list(c_l);
	for( unsigned i_c = 0; i_c < c_l.size(); i_c++)
	{
	    try{ at(m_l[i_m]).at().at(c_l[i_c]).at().save( ); }
	    catch(TError err) { Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); }
	}
    }
    //Save modules
    TSubSYS::subSave( );					    
}

void TDAQS::subStart(  )         
{
    vector<string> m_l;
    modList(m_l);
    //Enable controllers
    for( unsigned i_m = 0; i_m < m_l.size(); i_m++)
    {
	vector<string> c_l;
	at(m_l[i_m]).at().list(c_l);
	for( unsigned i_c = 0; i_c < c_l.size(); i_c++)
	{
	    AutoHD<TController> cntr = at(m_l[i_m]).at().at(c_l[i_c]);
	    if( !cntr.at().enableStat() && cntr.at().toEnable() )
	        try{ cntr.at().enable(); }
		catch(TError err) { Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); }
	}
    }
    //Start controllers
    for( unsigned i_m = 0; i_m < m_l.size(); i_m++)
    {
	vector<string> c_l;
        at(m_l[i_m]).at().list(c_l);
	for( unsigned i_c = 0; i_c < c_l.size(); i_c++)
	{
	    AutoHD<TController> cntr = at(m_l[i_m]).at().at(c_l[i_c]);
	    if( !cntr.at().startStat() && cntr.at().toStart() )
		try{ cntr.at().start( ); }
		catch(TError err) { Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); }
	}
    }
    TSubSYS::subStart( );
}

void TDAQS::subStop( )
{
    vector<string> m_l;
    modList(m_l);
    //Stop
    for( unsigned i_m = 0; i_m < m_l.size(); i_m++)
    {
	vector<string> c_l;
	at(m_l[i_m]).at().list(c_l);
	for( unsigned i_c = 0; i_c < c_l.size(); i_c++)
	{
	    AutoHD<TController> cntr = at(m_l[i_m]).at().at(c_l[i_c]);
	    if( cntr.at().startStat() )
		try{ cntr.at().stop( ); }
		catch(TError err) { Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); }	    	
	}
    }
    //Disable
    for( unsigned i_m = 0; i_m < m_l.size(); i_m++)
    {	
	vector<string> c_l;
        at(m_l[i_m]).at().list(c_l);
	for( unsigned i_c = 0; i_c < c_l.size(); i_c++)
        {
            AutoHD<TController> cntr = at(m_l[i_m]).at().at(c_l[i_c]);
            if( cntr.at().enableStat() )
        	try{ cntr.at().disable( ); }
        	catch(TError err) { Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); }
        }
    }
    TSubSYS::subStop( );
}

string TDAQS::optDescr( )
{
    char buf[STR_BUF_LEN];
    snprintf(buf,sizeof(buf),Mess->I18N(
	"=================== Subsystem \"Data acquisition\" options ================\n"
	"------------ Parameters of section <%s> in config file -----------\n\n"
	),nodePath().c_str());

    return(buf);
}


//================== Controll functions ========================
void TDAQS::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    if( cmd==TCntrNode::Info )
    {
	TSubSYS::cntrCmd_( a_path, opt, cmd );       //Call parent

	ctrMkNode("area",opt,0,a_path.c_str(),"/bd",Mess->I18N("Subsystem"),0440);
	ctrMkNode("comm",opt,-1,a_path.c_str(),"/bd/load_bd",Mess->I18N("Load"));
	ctrMkNode("comm",opt,-1,a_path.c_str(),"/bd/upd_bd",Mess->I18N("Save"));
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/help/g_help",Mess->I18N("Options help"),0440,0,0,3,"tp","str","cols","90","rows","5");
    }
    else if( cmd==TCntrNode::Get )
    {
	if( a_path == "/help/g_help" ) ctrSetS( opt, optDescr() );       
	else TSubSYS::cntrCmd_( a_path, opt, cmd );
    }
    else if( cmd==TCntrNode::Set )
    {
	if( a_path == "/bd/load_bd" )		subLoad();
	else if( a_path == "/bd/upd_bd" )	subSave();
	else TSubSYS::cntrCmd_( a_path, opt, cmd );	
    }
}

