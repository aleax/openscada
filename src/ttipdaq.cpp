
//OpenSCADA system file: ttipdaq.cpp
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


#include <stdio.h>

#include "tsys.h"
#include "tmodule.h"
#include "tmess.h"
#include "tbds.h"
#include "tdaqs.h"
#include "ttiparam.h"
#include "ttipdaq.h"


TTipDAQ::TTipDAQ( ) 
{
    m_cntr = grpAdd("cntr_");
    
    fldAdd( new TFld("ID",Mess->I18N("ID"),TFld::String,FLD_KEY|FLD_NWR,"20") );
    fldAdd( new TFld("NAME",Mess->I18N("Name"),TFld::String,0,"50") );
    fldAdd( new TFld("DESCR",Mess->I18N("Description"),TFld::String,0,"300") );
    fldAdd( new TFld("ENABLE",Mess->I18N("To enable"),TFld::Bool,0,"1","false") );
    fldAdd( new TFld("START",Mess->I18N("To start"),TFld::Bool,0,"1","false") );
}

TTipDAQ::~TTipDAQ( )
{
    nodeDelAll();
    
    while(paramt.size())
    {
	delete paramt[0];
	paramt.erase(paramt.begin());	
    }
};      

void TTipDAQ::modStart( )
{
    vector<string> lst;
    //Start all controllers
    list(lst);
    for(int i_l=0; i_l < lst.size(); i_l++)
	if( at(lst[i_l]).at().toStart() )
    	    at(lst[i_l]).at().start( );
}

void TTipDAQ::modStop( )
{
    vector<string> lst;
    //Stop all controllers
    list(lst);
    for(int i_l=0; i_l < lst.size(); i_l++)
        at(lst[i_l]).at().stop( );
}
      
void TTipDAQ::add( const string &name, const string &daq_db )
{   
    if( chldPresent(m_cntr,name) ) return;
    chldAdd(m_cntr,ContrAttach( name, daq_db ));
}

TTipParam &TTipDAQ::tpPrmAt( unsigned id )
{
    if(id >= paramt.size() || id < 0) 
	throw TError(nodePath().c_str(),Mess->I18N("Id of parameter type error!"));
    return *paramt[id];
}

//const string &name_t, const string &n_fld_bd, const string &descr
int TTipDAQ::tpParmAdd( const char *id, const char *n_db, const char *name )
{
    int i_t = tpPrmToId(id);    
    if( i_t < 0 )
    {
	//add type
	i_t = paramt.size();
	paramt.push_back(new TTipParam(id, name, n_db) );
	//Add structure fields
        paramt[i_t]->fldAdd( new TFld("SHIFR",Mess->I18N("ID"),TFld::String,FLD_KEY|FLD_NWR,"20") );
	paramt[i_t]->fldAdd( new TFld("NAME",Mess->I18N("Name"),TFld::String,0,"50") );
	paramt[i_t]->fldAdd( new TFld("DESCR",Mess->I18N("Description"),TFld::String,0,"200") );
	paramt[i_t]->fldAdd( new TFld("EN",Mess->I18N("To enable"),TFld::Bool,FLD_NOVAL,"1","false") );
    }

    return i_t;
}

int TTipDAQ::tpPrmToId( const string &name_t)
{
    for(unsigned i_t=0; i_t < paramt.size(); i_t++)
	if(paramt[i_t]->name() == name_t) return(i_t);
    return -1;	
}

//================== Controll functions ========================
void TTipDAQ::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    vector<string> c_list;
    
    if( cmd==TCntrNode::Info )
    {	
	TModule::cntrCmd_( a_path, opt, cmd );

	ctrMkNode("area",opt,0,a_path.c_str(),"/tctr",Mess->I18N("Controllers"));
	ctrMkNode("list",opt,-1,a_path.c_str(),"/tctr/ctr",Mess->I18N("Controllers"),0664,0,0,4,"tp","br","idm","1","s_com","add,del","br_pref","cntr_");
    }
    else if( cmd==TCntrNode::Get )
    {
	if( a_path == "/tctr/ctr" )
	{
	    list(c_list);
	    opt->childClean();
	    for( unsigned i_a=0; i_a < c_list.size(); i_a++ )
		ctrSetS( opt, at(c_list[i_a]).at().name(), c_list[i_a].c_str() ); 	
	}
	else TModule::cntrCmd_( a_path, opt, cmd );
    }
    else if( cmd==TCntrNode::Set )
    {
	if( a_path == "/tctr/ctr" )
	{
	    if( opt->name() == "add" )		
	    {
		add(opt->attr("id"));
		at(opt->attr("id")).at().name(opt->text());				
	    }
	    else if( opt->name() == "del" )    	chldDel(m_cntr,opt->attr("id"),-1,1);
	}
	else TModule::cntrCmd_( a_path, opt, cmd );
    }
}
