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


#include <stdio.h>

#include "tsys.h"
#include "tmodule.h"
#include "tmess.h"
#include "tbds.h"
#include "tcontrollers.h"
#include "ttiparam.h"
#include "ttipcontroller.h"


TTipController::TTipController( ) 
{
    m_cntr = grpAdd();
    
    fldAdd( new TFld("NAME",Mess->I18N("Short name"),TFld::String,FLD_KEY|FLD_NWR,"20") );
    fldAdd( new TFld("LNAME",Mess->I18N("Description"),TFld::String,0,"50") );
    fldAdd( new TFld("ENABLE",Mess->I18N("To enable"),TFld::Bool,0,"1","false") );
    fldAdd( new TFld("START",Mess->I18N("To start"),TFld::Bool,0,"1","false") );
}

TTipController::~TTipController( )
{
    nodeDelAll();
    
    while(paramt.size())
    {
	delete paramt[0];
	paramt.erase(paramt.begin());	
    }
};      
      
void TTipController::add( const string &name, const TBDS::SName &bd )
{   
    if( chldPresent(m_cntr,name) ) return;
    chldAdd(m_cntr,ContrAttach( name, bd )); 
}

//const string &name_t, const string &n_fld_bd, const string &descr
int TTipController::tpParmAdd( const char *id, const char *n_db, const char *name )
{
    int i_t;    

    //search type
    try
    { 
	i_t = tpPrmToId(id); 
	throw TError(nodePath().c_str(),"Parameter <%s> already present!",id);
    }
    catch(TError err)
    {
	//add type
	i_t = paramt.size();
	paramt.push_back(new TTipParam(id, name, n_db) );
	//Add structure fields
        paramt[i_t]->fldAdd( new TFld("SHIFR",Mess->I18N("Short name (TAGG)"),TFld::String,FLD_KEY|FLD_NWR,"20") );
	paramt[i_t]->fldAdd( new TFld("NAME",Mess->I18N("Description"),TFld::String,0,"50") );
	paramt[i_t]->fldAdd( new TFld("EN",Mess->I18N("To enable"),TFld::Bool,FLD_NOVAL,"1","false") );
	paramt[i_t]->fldAdd( new TFld("EXPORT",Mess->I18N("Put to generic list"),TFld::Bool,FLD_NOVAL,"1","false") );
    }

    return(i_t);
}

unsigned TTipController::tpPrmToId( const string &name_t)
{
    for(unsigned i_t=0; i_t < paramt.size(); i_t++)
	if(paramt[i_t]->name() == name_t) return(i_t);
    throw TError(nodePath().c_str(),"Parameter type <%s> no present!",name_t.c_str());
}

//================== Controll functions ========================
void TTipController::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    vector<string> c_list;
    
    if( cmd==TCntrNode::Info )
    {	
	TModule::cntrCmd_( a_path, opt, cmd );

	ctrInsNode("area",0,opt,a_path.c_str(),"/tctr",Mess->I18N("Controllers"));
	ctrMkNode("list",opt,a_path.c_str(),"/tctr/ctr",Mess->I18N("Controllers"),0664,0,0,"br")->
	    attr_("s_com","add,del")->attr_("mode","att")->attr_("br_pref","_");
    }
    else if( cmd==TCntrNode::Get )
    {
	if( a_path == "/tctr/ctr" )
	{
	    list(c_list);
	    opt->childClean();
	    for( unsigned i_a=0; i_a < c_list.size(); i_a++ )
		ctrSetS( opt, c_list[i_a] ); 	
	}
	else TModule::cntrCmd_( a_path, opt, cmd );
    }
    else if( cmd==TCntrNode::Set )
    {
	if( a_path == "/tctr/ctr" )
	{
	    if( opt->name() == "add" )		add(opt->text(),TBDS::SName("","",modId().c_str()));
	    else if( opt->name() == "del" )    	chldDel(m_cntr,opt->text(),-1,1);
	}
	else TModule::cntrCmd_( a_path, opt, cmd );
    }
}

AutoHD<TCntrNode> TTipController::ctrAt( const string &a_path )
{
    if( a_path.substr(0,1) == "_" ) return at(TSYS::strEncode(a_path.substr(1),TSYS::PathEl));
    else return TModule::ctrAt(a_path);
}

											
