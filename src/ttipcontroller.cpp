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
    SFld sc_fld[] =
    {
	{"NAME"  ,Mess->I18N("Short name") ,T_STRING|F_NWR,""     ,"20"},
	{"LNAME" ,Mess->I18N("Description"),T_STRING      ,""     ,"50"},
	{"ENABLE",Mess->I18N("To enable")  ,T_BOOL        ,"false","1" },
	{"START" ,Mess->I18N("To start")   ,T_BOOL        ,"false","1" }
    };    
    loadCfg( sc_fld, sizeof(sc_fld)/sizeof(SFld) );
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
    while(val_el.size())
    {
	delete val_el[0];
	val_el.erase(val_el.begin());
    }
};

void TTipController::add( const string &name, const SBDS &bd )
{   
    TController *cntr = ContrAttach( name, bd );
    try{ m_hd_cntr.obj_add( cntr, &cntr->name() ); }
    catch(TError err) { delete cntr; }
}

void TTipController::loadCfg( SFld *elements, int numb )
{
    for(int i = 0; i < numb; i++) elAdd(&elements[i]);
}

int TTipController::tpParmAdd( const string &name_t, const string &n_fld_bd, const string &descr)
{
    int i_t;
    
    SFld Elem_TPrm[] =
    {
	{"SHIFR" ,Mess->I18N("Short name (TAGG)")  ,T_STRING|F_NWR,""     ,"20"},
	{"NAME"  ,Mess->I18N("Description")        ,T_STRING      ,""     ,"50"},
	{"EXPORT",Mess->I18N("Put to generic list"),T_BOOL|V_NOVAL,"false","1" }
    };	

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
	tpParmLoad(i_t, Elem_TPrm,sizeof(Elem_TPrm)/sizeof(SFld));
    }

    return(i_t);
}

unsigned TTipController::tpPrmToId( const string &name_t)
{
    for(unsigned i_t=0; i_t < paramt.size(); i_t++)
	if(paramt[i_t]->name() == name_t) return(i_t);
    throw TError("(%s) The parameter type %s no avoid!",o_name,name_t.c_str());
}

void TTipController::tpParmLoad( unsigned t_prm, SFld *elements, int numb )
{
    if( t_prm >= paramt.size() ) throw TError("(%s) Type parameter %d no avoid!",o_name,t_prm);
    for(int i = 0; i < numb; i++) paramt[t_prm]->elAdd(&elements[i]);
}

void TTipController::tpValAdd( const string &name, SFld *vl_el, int number)
{
    unsigned id_elem, i_elem;
    
    for( id_elem = 0; id_elem < val_el.size(); id_elem++)
	if(val_el[id_elem]->elName() == name) break;
    if( id_elem == val_el.size()) val_el.push_back( new TElem( name ));

    for( i_elem=0; i_elem < (unsigned)number; i_elem++)
	val_el[id_elem]->elAdd(&vl_el[i_elem]);
}

void TTipController::tpValList( vector<string> & List )
{
    for(unsigned i_val=0; i_val < val_el.size(); i_val++)
	List.push_back(val_el[i_val]->elName());
}

TElem &TTipController::tpValAt( const string &name)
{
    for(unsigned i_val=0; i_val < val_el.size(); i_val++)
	if(val_el[i_val]->elName() == name) return(*val_el[i_val]); 
    throw TError("%s: value %s no avoid into controller!",o_name,name.c_str());
}

//================== Controll functions ========================
void TTipController::ctr_fill_info( XMLNode *inf )
{
    char *dscr="dscr";
    
    TModule::ctr_fill_info( inf );
    
    char *i_cntr = 
	"<area id='a_tctr'>"
	" <list id='ctr' s_com='add,del' tp='br' mode='att'/>"
	"</area>";
    
    XMLNode *n_add = inf->ins_child(0);
    n_add->load_xml(i_cntr);
    n_add->set_attr(dscr,Mess->I18N("Controllers"));
    n_add->get_child(0)->set_attr(dscr,Mess->I18N("Controllers"));
}

void TTipController::ctr_din_get_( const string &a_path, XMLNode *opt )
{   
    if( a_path == "/a_tctr/ctr" )
    {
	vector<string> c_list;
	list(c_list);
	opt->clean_childs();
	for( unsigned i_a=0; i_a < c_list.size(); i_a++ )
	    ctr_opt_setS( opt, c_list[i_a], i_a ); 	
    }
    else TModule::ctr_din_get_( a_path, opt );
}

void TTipController::ctr_din_set_( const string &a_path, XMLNode *opt )
{
    if( a_path.substr(0,11) == "/a_tctr/ctr" )
	for( int i_el=0; i_el < opt->get_child_count(); i_el++)	    
	{
	    XMLNode *t_c = opt->get_child(i_el);
	    if( t_c->get_name() == "el")
	    {
		if(t_c->get_attr("do") == "add")      add(t_c->get_text(),SBDS("","",""));
		else if(t_c->get_attr("do") == "del") del(t_c->get_text());
	    }
	}
    else TModule::ctr_din_set_( a_path, opt );
}
	
AutoHD<TContr> TTipController::ctr_at1( const string &a_path )
{
    if( a_path.substr(0,11) == "/a_tctr/ctr" ) return at(ctr_path_l(a_path,2));	
    else return TModule::ctr_at1(a_path);
}

											
