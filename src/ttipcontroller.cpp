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

#include "tmodule.h"
#include "tkernel.h"
#include "tmessage.h"
#include "tbds.h"
#include "tcontrollers.h"
#include "ttiparam.h"
#include "ttipcontroller.h"


const char *TTipController::o_name = "TTipController";

TTipController::TTipController( ) : TElem(""), m_hd_cntr(o_name) 
{
    fldAdd( new TFld("NAME",Mess->I18N("Short name"),T_STRING|F_KEY|F_NWR,"20") );
    fldAdd( new TFld("LNAME",Mess->I18N("Description"),T_STRING,"50") );
    fldAdd( new TFld("ENABLE",Mess->I18N("To enable"),T_BOOL,"1","false") );
    fldAdd( new TFld("START",Mess->I18N("To start"),T_BOOL,"1","false") );
}

TTipController::~TTipController( )
{
    vector<string> c_list;

    m_hd_cntr.lock();
    list(c_list);
    //First, disable all controllers
    for( unsigned i_ls = 0; i_ls < c_list.size(); i_ls++)
    	at(c_list[i_ls]).at().disable();
    //Second, delete controllers
    for( unsigned i_ls = 0; i_ls < c_list.size(); i_ls++)
    	del(c_list[i_ls]);
    
    while(paramt.size())
    {
	delete paramt[0];
	paramt.erase(paramt.begin());	
    }
};      
      
void TTipController::add( const string &name, const TBDS::SName &bd )
{   
    if( m_hd_cntr.objAvoid(name) ) return;
    TController *cntr = ContrAttach( name, bd );
    try{ m_hd_cntr.objAdd( cntr, &cntr->name() ); }
    catch(TError err) { delete cntr; throw; }
}

int TTipController::tpParmAdd( const string &name_t, const string &n_fld_bd, const string &descr)
{
    int i_t;    

    //search type
    try
    { 
	i_t = tpPrmToId(name_t); 
	throw TError("(%s) Parameter %s already avoid!",o_name,name_t.c_str());
    }
    catch(TError err)
    {
	//add type
	i_t = paramt.size();
	paramt.push_back(new TTipParam(name_t, descr, n_fld_bd) );
	//Add structure fields
        paramt[i_t]->fldAdd( new TFld("SHIFR",Mess->I18N("Short name (TAGG)"),T_STRING|F_KEY|F_NWR,"20") );
	paramt[i_t]->fldAdd( new TFld("NAME",Mess->I18N("Description"),T_STRING,"50") );
	paramt[i_t]->fldAdd( new TFld("EXPORT",Mess->I18N("Put to generic list"),T_BOOL|F_NOVAL,"1","false") );		    
    }

    return(i_t);
}

unsigned TTipController::tpPrmToId( const string &name_t)
{
    for(unsigned i_t=0; i_t < paramt.size(); i_t++)
	if(paramt[i_t]->name() == name_t) return(i_t);
    throw TError("(%s) The parameter type %s no avoid!",o_name,name_t.c_str());
}

//================== Controll functions ========================
void TTipController::ctrStat_( XMLNode *inf )
{
    char *dscr="dscr";
    
    TModule::ctrStat_( inf );
    
    char *i_cntr = 
	"<area id='tctr'>"
	" <list id='ctr' s_com='add,del' tp='br' mode='att'/>"
	"</area>";
    
    XMLNode *n_add = inf->childIns(0);
    n_add->load(i_cntr);
    n_add->attr(dscr,Mess->I18N("Controllers"));
    n_add->childGet(0)->attr(dscr,Mess->I18N("Controllers"));
}

void TTipController::ctrDinGet_( const string &a_path, XMLNode *opt )
{   
    if( a_path == "/tctr/ctr" )
    {
	vector<string> c_list;
	list(c_list);
	opt->childClean();
	for( unsigned i_a=0; i_a < c_list.size(); i_a++ )
	    ctrSetS( opt, c_list[i_a], i_a ); 	
    }
    else TModule::ctrDinGet_( a_path, opt );
}

void TTipController::ctrDinSet_( const string &a_path, XMLNode *opt )
{
    if( a_path.substr(0,9) == "/tctr/ctr" )
	for( int i_el=0; i_el < opt->childSize(); i_el++)	    
	{
	    XMLNode *t_c = opt->childGet(i_el);
	    if( t_c->name() == "el")
	    {
		if(t_c->attr("do") == "add")      add(t_c->text(),TBDS::SName("","",""));
		else if(t_c->attr("do") == "del")
		{
		    TConfig conf(this);
		    conf = at(t_c->text()).at();
		    TBDS::SName nm_bd = at(t_c->text()).at().BD();
		    //Delete
		    del(t_c->text());
		    //Delete from controllers BD
		    TConfig g_cfg((TControllerS *)(&owner()));
		    g_cfg.cfg("NAME").setS(t_c->text());
		    owner().owner().BD().open(((TControllerS &)owner()).BD()).at().fieldDel(g_cfg);
		    owner().owner().BD().close(((TControllerS &)owner()).BD());		    
		    //Delete from type BD
		    owner().owner().BD().open(nm_bd).at().fieldDel(conf);
		    owner().owner().BD().close(nm_bd);		    
		}
	    }
	}
    else TModule::ctrDinSet_( a_path, opt );
}
	
AutoHD<TContr> TTipController::ctrAt1( const string &a_path )
{
    if( a_path.substr(0,9) == "/tctr/ctr" ) return at(pathLev(a_path,2));	
    else return TModule::ctrAt1(a_path);
}

											
