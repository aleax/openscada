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
 
#include "tbds.h"
#include "tsys.h"
#include "tkernel.h"
#include "tmessage.h"
#include "tcontrollers.h"
#include "tcontroller.h"
#include "ttipcontroller.h"
#include "tparams.h"
#include "ttiparam.h"
#include "tparamcontr.h"

const char *TParamContr::o_name = "TParamContr";
const char *TParamContr::i_cntr = 
    "<oscada_cntr>"
    " <area id='a_prm'>"
    "  <area id='a_st'>"
    "   <fld id='type' acs='0444' tp='str'/>"
    "   <fld id='exp_st' acs='0664' tp='bool'/>"
    "  </area>"
    "  <area id='a_cfg'>"
    "   <comm id='load' acs='0550'/>"
    "   <comm id='save' acs='0550'/>"    
    "  </area>"    
    "  <area id='a_val'>"
    "  </area>"    
    " </area>"
    "</oscada_cntr>";

TParamContr::TParamContr( const string &name, TTipParam *tpprm, TController *contr ) : 
    owner(contr), TConfig(tpprm), TValue(this), tipparm(tpprm), m_export(false), 
    m_name(cfg("SHIFR").getS()), m_lname(cfg("NAME").getS()), m_aexport(cfg("EXPORT").getB())
{
    m_name = name;
}

TParamContr::~TParamContr( )
{

}

void TParamContr::Load( )
{
    TBDS &bds  = Owner().Owner().Owner().Owner().BD();
    SHDBD t_hd = bds.open( SBDS( Owner().BD().tp, Owner().BD().bd, Owner().cfg(Type().BD()).getS() ) );	
    cfLoadValBD("SHIFR",bds.at(t_hd));
    bds.close(t_hd);
}

void TParamContr::Save( )
{
    TBDS &bds  = Owner().Owner().Owner().Owner().BD();
    SHDBD t_hd = bds.open( SBDS( Owner().BD().tp, Owner().BD().bd, Owner().cfg(Type().BD()).getS() ), true );	
    cfConfElem().elUpdateBDAttr( bds.at(t_hd) );
    cfSaveValBD("SHIFR",bds.at(t_hd));
    bds.at(t_hd).Save(); 
    bds.close(t_hd);
}

TParamContr & TParamContr::operator=( TParamContr & PrmCntr )
{
    TConfig::operator=(PrmCntr);

    return(*this);
}

/*
void TParamContr::UpdateVAL()
{    
    //vlElem( &Owner().Owner().at_TpVal(cfg("TYPE").getS()) );
}                    
*/

void TParamContr::Enable()
{
    vector<string> list;
    vlList(list);
    for(unsigned i_val = 0; i_val < list.size(); i_val++)
	vlAt(list[i_val]).at().valid(true);
}

void TParamContr::Disable()
{
    vector<string> list;
    vlList(list);
    for(unsigned i_val = 0; i_val < list.size(); i_val++)
	vlAt(list[i_val]).at().valid(false);
}

/*
TValue &TParamContr::val()
{ 
    return(*this); 
}
*/

void TParamContr::Export( )
{
    TKernel &kern = Owner().Owner().Owner().Owner();    
    kern.Param().add( SCntrS( Owner().Owner().mod_Name(), Owner().Name()), Name() );
    m_export = true;
}

void TParamContr::UnExport( )
{
    TKernel &kern = Owner().Owner().Owner().Owner();
    kern.Param().del( SCntrS( Owner().Owner().mod_Name(), Owner().Name()), Name());
    m_export = false;
}

//================== Controll functions ========================
void TParamContr::ctr_fill_info( XMLNode *inf )
{
    char buf[STR_BUF_LEN];
    char *dscr="dscr";
    
    inf->load_xml( i_cntr );
    snprintf(buf,sizeof(buf),Mess->I18N("Parameter: %s"),Name().c_str());
    inf->set_text(buf);
    XMLNode *t_cntr = inf->get_child(0);
    t_cntr->set_attr(dscr,Mess->I18N("Parameter control"));
    t_cntr = t_cntr->get_child(0);    
    t_cntr->set_attr(dscr,Mess->I18N("Parameter stat"));    
    t_cntr->get_child(0)->set_attr(dscr,Mess->I18N("The parameter type"));
    t_cntr->get_child(1)->set_attr(dscr,Mess->I18N("Into generic list"));
    t_cntr = inf->get_child(0)->get_child(1);    
    t_cntr->set_attr(dscr,Mess->I18N("Parameter config"));
    t_cntr->get_child(0)->set_attr(dscr,Mess->I18N("Load parameter"));
    t_cntr->get_child(1)->set_attr(dscr,Mess->I18N("Save parameter"));
    t_cntr = inf->get_child(0)->get_child(2);    
    t_cntr->set_attr(dscr,Mess->I18N("Parameter value atributes"));   
    
    ctr_cfg_parse("/a_prm/a_cfg",inf, this);  //Generate individual controller config from TConfig 
    ctr_val_parse("/a_prm/a_val",inf, this);  //Generate value from TValue 
}

void TParamContr::ctr_din_get_( const string &a_path, XMLNode *opt )
{    
    string t_id = ctr_path_l(a_path,0);
    if( t_id == "a_prm" )
    {
	t_id = ctr_path_l(a_path,1);
	if( t_id == "a_st" )
	{	
	    t_id = ctr_path_l(a_path,2);
	    if( t_id == "type" )        ctr_opt_setS( opt, Type().Descr() );
	    else if( t_id == "exp_st" ) ctr_opt_setB( opt, m_export );
	}
	else if( t_id == "a_cfg" ) ctr_cfg_set( ctr_path_l(a_path,2), opt, this );
	else if( t_id == "a_val" ) ctr_val_set( ctr_path_l(a_path,2), opt, this );
    }
}

void TParamContr::ctr_din_set_( const string &a_path, XMLNode *opt )
{
    string t_id = ctr_path_l(a_path,0);
    if( t_id == "a_prm" )
    {
	t_id = ctr_path_l(a_path,1);
	if( t_id == "a_st" )
	{	
	    t_id = ctr_path_l(a_path,2);
	    if( t_id == "exp_st" )
		if( ctr_opt_getB( opt ) ) Export();
		else                      UnExport();
	}
	else if( t_id == "a_cfg" ) ctr_cfg_get( ctr_path_l(a_path,2), opt, this );
	else if( t_id == "a_val" ) ctr_val_get( ctr_path_l(a_path,2), opt, this );
    }
}

void TParamContr::ctr_cmd_go_( const string &a_path, XMLNode *fld, XMLNode *rez )
{
    string t_id = ctr_path_l(a_path,0);    
    if( t_id == "a_prm" )
    {
    	t_id = ctr_path_l(a_path,1);    
	if( t_id == "a_cfg" )
	{
	    t_id = ctr_path_l(a_path,2);    
	    if( t_id == "load" )      Load();
	    else if( t_id == "save" ) Save();
	}
    }
}

