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

#include "tkernel.h"
#include "tmessage.h"
#include "tcontrollers.h"
#include "ttiparam.h"
#include "tparamcontr.h"
#include "tparams.h"
#include "tcontroller.h"

const char *TController::o_name = "TController";
const char *TController::i_cntr = 
    "<oscada_cntr>"
    " <area id='a_prm'>"
    "  <fld id='t_prm' acs='0660' tp='str' dest='select' select='a_prm/t_lst'/>"
    "  <list id='t_lst' tp='str' mode='st' hide='1'/>"
    "  <list id='prm' s_com='add,ins,del' tp='br' mode='att'/>"
    "  <comm id='load' acs='0550'/>"
    "  <comm id='save' acs='0550'/>"
    " </area>"
    " <area id='a_cntr'>"
    "  <fld id='t_bd' acs='0660' tp='str' dest='select' select='a_cntr/b_mod'/>"
    "  <fld id='bd' acs='0660' tp='str'/>"
    "  <fld id='tbl' acs='0660' tp='str'/>"
    "  <list id='b_mod' tp='str' hide='1'/>"
    "  <area id='a_st'>"
    "   <fld id='en_st' acs='0664' tp='bool'/>"
    "   <fld id='run_st' acs='0664' tp='bool'/>"
    "  </area>"
    "  <area id='a_cfg'>"    
    "   <comm id='load' acs='0550'/>"
    "   <comm id='save' acs='0550'/>"
    "  </area>"
    " </area>"
    "</oscada_cntr>";

//==== TController ====
TController::TController( const string &name_c, const SBDS &bd, TTipController *tcntr, TElem *cfgelem ) : 
    m_bd(bd), owner(tcntr), TConfig(cfgelem), run_st(false), en_st(false), m_hd(o_name), m_add_type(0),
    m_name(cfg("NAME").getS()), m_lname(cfg("LNAME").getS()), m_aen(cfg("ENABLE").getB()), m_astart(cfg("START").getB())  
{
    m_name = name_c; 
    
    TBDS &bds = Owner().Owner().Owner().BD();
    try
    {
	SHDBD i_hd = bds.open( m_bd );
	cfLoadValBD("NAME",bds.at(i_hd));
	bds.close(i_hd);	
    }catch(...){ }    
}

TController::~TController(  )
{
    vector<string> c_list;

    if( run_st ) Stop( );
    m_hd.lock();
    if( en_st )  Disable( );
}

TParamContr &TController::at( unsigned hd )
{ 
    return( *(TParamContr *)m_hd.hd_at( hd ) ); 
}

void TController::Load( bool self )
{
    TBDS &bds = Owner().Owner().Owner().BD();
    if( en_st || self ) 
    {
#if OSC_DEBUG
    	Owner().m_put("DEBUG",MESS_INFO,"%s: Load controller's configs!",Name().c_str());	
#endif   

	SHDBD i_hd = bds.open( m_bd );
	cfLoadValBD("NAME",bds.at(i_hd));
	bds.close(i_hd);	
	
	if( !self ) LoadParmCfg( );
	
	Load_();        //Module load	
#if OSC_DEBUG
    	Owner().m_put("DEBUG",MESS_DEBUG,"%s: Load controller's configs ok!",Name().c_str());	
#endif  
    }
    else throw TError("(%s) Controller %s no enable!",o_name,Name().c_str());    
}

void TController::Save( bool self )
{
    TBDS &bds = Owner().Owner().Owner().BD();
    if( en_st || self ) 
    {
#if OSC_DEBUG
	Owner().m_put("DEBUG",MESS_INFO,"%s: Save controller's configs!",Name().c_str());	
#endif 	
	Save_();	//Module save

	if( !self ) SaveParmCfg( );
	
	//Update type controller bd record
	SHDBD b_hd = bds.open( m_bd, true );
	owner->elUpdateBDAttr( bds.at(b_hd) );
	cfSaveValBD("NAME",bds.at(b_hd));
	bds.at(b_hd).Save();
	bds.close(b_hd);
	
	//Update generic controller bd record
	b_hd = bds.open( ((TControllerS &)(Owner().Owner())).BD(), true );
	TConfig *g_cfg = new TConfig((TControllerS *)(&Owner().Owner()));
	g_cfg->cfg("NAME").setS(Name());
	g_cfg->cfg("MODUL").setS(Owner().mod_Name());
	g_cfg->cfg("BDTYPE").setS(m_bd.tp);
	g_cfg->cfg("BDNAME").setS(m_bd.bd);
	g_cfg->cfg("TABLE").setS(m_bd.tbl);
	g_cfg->cfSaveValBD("NAME",bds.at(b_hd));
	delete g_cfg;
	bds.at(b_hd).Save();
	bds.close(b_hd);	
	
#if OSC_DEBUG
	Owner().m_put("DEBUG",MESS_DEBUG,"%s: Save controller's configs ok!",Name().c_str());	
#endif 
    }
    else throw TError("(%s) Controller %s no enable!",o_name,Name().c_str());    
} 

void TController::Start( )
{        
    if( en_st && !run_st )
    {
#if OSC_DEBUG
	Owner().m_put("DEBUG",MESS_INFO,"%s: Start controller!",Name().c_str());
#endif 	
	Start_();          //Module start	
#if OSC_DEBUG
	Owner().m_put("DEBUG",MESS_DEBUG,"%s: Start controller ok!",Name().c_str());
#endif 	
    }
    else if( run_st ) throw TError("(%s) Controller %s has started!",o_name,Name().c_str());
    else throw TError("(%s) Controller %s no enable!",o_name,Name().c_str());
}

void TController::Stop( )
{
    if( run_st )
    {
#if OSC_DEBUG
	Owner().m_put("DEBUG",MESS_INFO,"%s: Stop controller!",Name().c_str());
#endif	
	Stop_();          //Module stop	
#if OSC_DEBUG
	Owner().m_put("DEBUG",MESS_DEBUG,"%s: Stop controller ok!",Name().c_str());
#endif	
    }
    else throw TError("(%s) Controller %s is not starting!",o_name,Name().c_str());
}

void TController::Enable( )
{
    vector<string> c_list;    
    if( !en_st )
    {
#if OSC_DEBUG
	Owner().m_put("DEBUG",MESS_INFO,"%s: Enable controller!",Name().c_str());
#endif
	en_st=true;
	try{ LoadParmCfg( ); } catch(...){ }
	//Export auto exported parameters
	list(c_list);
	for( unsigned i_ls = 0; i_ls < c_list.size(); i_ls++)
	{
	    int hd = att(c_list[i_ls],"self_exp");
	    if( at(hd).auto_export() && !at(hd).st_export() ) at(hd).Export();
	    det(hd);
	}
#if OSC_DEBUG
	Owner().m_put("DEBUG",MESS_DEBUG,"%s: Enable controller ok!",Name().c_str());
#endif
    }
    else throw TError("(%s) Controller %s has enabled!",o_name,Name().c_str());
}

void TController::Disable( )
{
    vector<string> c_list;    
    if( en_st )
    {
#if OSC_DEBUG
	Owner().m_put("DEBUG",MESS_INFO,"%s: Disable controller!",Name().c_str());
#endif	
	if( run_st ) Stop();
	//UnExport all parameters
	list(c_list);
	for( unsigned i_ls = 0; i_ls < c_list.size(); i_ls++)
	{
	    int hd = att(c_list[i_ls],"self_uxp");
	    if( at(hd).st_export() ) at(hd).UnExport();
	    det(hd);
	}
	//Free all parameters
	FreeParmCfg();
	
	en_st = false;	
#if OSC_DEBUG
	Owner().m_put("DEBUG",MESS_DEBUG,"%s: Disable controller ok!",Name().c_str());
#endif
    }
    else throw TError("(%s) Controller %s is not enabling!",o_name,Name().c_str());
}

void TController::LoadParmCfg(  )
{
    string      parm_bd;
    TParamContr *PrmCntr;

    TBDS    &bds  = Owner().Owner().Owner().BD();    
    TParamS &prms = Owner().Owner().Owner().Param();    
    
    for(unsigned i_tp = 0; i_tp < owner->SizeTpPrm(); i_tp++)
    {
	SHDBD t_hd = bds.open( SBDS( m_bd.tp, m_bd.bd, cfg(owner->at_TpPrm(i_tp).BD()).getS() ) );	
	for(unsigned i=0; i < (unsigned)bds.at(t_hd).NLines( ); i++)
	{
	    try
	    {
		string n_prm = bds.at(t_hd).GetCellS( bds.at(t_hd).ColumNameToId("SHIFR"),i);
		try{ add( n_prm, i_tp ); } catch(...){ }   //If already avoid
		at(n_prm,"self_ld").at().cfLoadValBD(i,bds.at(t_hd));
	    }catch(TError err) 
	    { Owner().m_put("SYS",MESS_ERR,"%s: %s!",Name().c_str(),err.what().c_str()); }
	}
	bds.close(t_hd);
    }
}

void TController::SaveParmCfg(  )
{
    TBDS    &bds  = Owner().Owner().Owner().BD();    
    for(unsigned i_tp = 0; i_tp < owner->SizeTpPrm(); i_tp++)
    {
    	string parm_tbl = cfg(owner->at_TpPrm(i_tp).BD()).getS();
    
	SHDBD t_hd = bds.open( SBDS(m_bd.tp, m_bd.bd, parm_tbl),true );	
       	bds.at(t_hd).Clean();
	//Update BD (resize, change atributes ..
    	owner->at_TpPrm(i_tp).elUpdateBDAttr(bds.at(t_hd));

	vector<string> c_list;    
    	list(c_list);
	for( unsigned i_ls = 0, i_bd=0; i_ls < c_list.size(); i_ls++)
	{
	    int p_hd = att(c_list[i_ls],"self_sv");
	    if(at(p_hd).Type().Name() == owner->at_TpPrm(i_tp).Name()) 
		at(p_hd).cfSaveValBD(i_bd++,bds.at(t_hd));
	    det(p_hd);		
	}
	
	bds.at(t_hd).Save( );
	bds.close(t_hd);
    }
}

void TController::FreeParmCfg(  )
{
    vector<string> c_list;    
    list(c_list);
    for( unsigned i_ls = 0; i_ls < c_list.size(); i_ls++)
	del( c_list[i_ls] );
}

void TController::add( const string &name, unsigned type, int pos )
{
    TParamContr *PrmCntr = ParamAttach( name, type );
    try{ m_hd.obj_add( PrmCntr, &PrmCntr->Name(), pos ); }
    catch(TError err) { delete PrmCntr; }
}

void TController::del( const string &name )
{ 
    delete (TParamContr *)m_hd.obj_del( name, 5 ); 
}

TParamContr *TController::ParamAttach( const string &name, int type)
{ 
    return(new TParamContr(name, &owner->at_TpPrm(type), this)); 
}

//================== Controll functions ========================
void TController::ctr_fill_info( XMLNode *inf )
{
    char *dscr="dscr";
    XMLNode *t_cntr;

    inf->load_xml( i_cntr );
    inf->set_text(Mess->I18Ns("Controller: ")+Name());
    t_cntr = inf->get_child(1);
    t_cntr->set_attr(dscr,Mess->I18N("Controller control"));
    t_cntr->get_child(0)->set_attr(dscr,Mess->I18N("Type controller BD (module:bd:table)"));    
    t_cntr = t_cntr->get_child(4);    
    t_cntr->set_attr(dscr,Mess->I18N("Controller stat"));    
    t_cntr->get_child(0)->set_attr(dscr,Mess->I18N("Enable stat"));
    t_cntr->get_child(1)->set_attr(dscr,Mess->I18N("Run stat"));
    t_cntr = inf->get_child(1)->get_child(5);    
    t_cntr->set_attr(dscr,Mess->I18N("Controller config"));
    t_cntr->get_child(0)->set_attr(dscr,Mess->I18N("Load controller"));
    t_cntr->get_child(1)->set_attr(dscr,Mess->I18N("Save controller"));    

    ctr_cfg_parse("/a_cntr/a_cfg",inf, this);  //Generate individual controller config from TConfig 
    //t_cntr->get_child(2)->set_attr("acs","0444");    //No write acces to name    
    
    if( !Owner().SizeTpPrm() || !st_enable() ) inf->del_child(0); 
    else
    {
    	t_cntr = inf->get_child(0);
	t_cntr->set_attr(dscr,Mess->I18N("Controller parameters"));
    	t_cntr->get_child(0)->set_attr(dscr,Mess->I18N("Parameter type for add operation"));
	t_cntr->get_child(2)->set_attr(dscr,Mess->I18N("Parameters"));
	t_cntr->get_child(3)->set_attr(dscr,Mess->I18N("Load BD"));
	t_cntr->get_child(4)->set_attr(dscr,Mess->I18N("Update BD"));
    }    
}

void TController::ctr_din_get_( const string &a_path, XMLNode *opt )
{
    vector<string> c_list;
    
    string t_id = ctr_path_l(a_path,0);
    if( t_id == "a_prm" )
    {
	t_id = ctr_path_l(a_path,1);	
	if( t_id == "t_prm" ) ctr_opt_setS( opt, Owner().at_TpPrm(m_add_type).Descr() );
	else if( t_id == "prm" )
	{
	    list(c_list);
	    for( unsigned i_a=0; i_a < c_list.size(); i_a++ )
		ctr_opt_setS( opt, c_list[i_a], i_a ); 	
	}
	else if( t_id == "t_lst" )
	    for( unsigned i_a=0; i_a < Owner().SizeTpPrm(); i_a++ )
		ctr_opt_setS( opt, Owner().at_TpPrm(i_a).Descr(), i_a ); 	
    }
    else if( t_id == "a_cntr" )
    {
	string t_id = ctr_path_l(a_path,1);
	if( t_id == "t_bd" )     ctr_opt_setS( opt, m_bd.tp );
	else if( t_id == "bd" )  ctr_opt_setS( opt, m_bd.bd );
	else if( t_id == "tbl" ) ctr_opt_setS( opt, m_bd.tbl );
	else if( t_id == "b_mod" )
	{
	    vector<string> c_list;
	    Owner().Owner().Owner().BD().gmd_list(c_list);
	    for( unsigned i_a=0; i_a < c_list.size(); i_a++ )
		ctr_opt_setS( opt, c_list[i_a], i_a );
	}
	else if( t_id == "a_st" )
	{	
	    string t_id = ctr_path_l(a_path,2);
	    if( t_id == "en_st" )       ctr_opt_setB( opt, en_st );
	    else if( t_id == "run_st" ) ctr_opt_setB( opt, run_st );
	}
	else if( t_id == "a_cfg" ) ctr_cfg_set( ctr_path_l(a_path,2), opt, this );
    }
}

void TController::ctr_din_set_( const string &a_path, XMLNode *opt )
{
    string t_id = ctr_path_l(a_path,0);
    if( t_id == "a_prm" )
    {
	t_id = ctr_path_l(a_path,1);	
	if( t_id == "t_prm" ) m_add_type = atoi(ctr_opt_getS( opt ).c_str());
	else if( t_id == "prm" )
	    for( int i_el=0; i_el < opt->get_child_count(); i_el++)	    
	    {
		XMLNode *t_c = opt->get_child(i_el);
		if( t_c->get_name() == "el")
		{
		    if(t_c->get_attr("do") == "add")      add(t_c->get_text(),m_add_type);
		    else if(t_c->get_attr("do") == "ins") add(t_c->get_text(),m_add_type,atoi(t_c->get_attr("id").c_str()));
		    else if(t_c->get_attr("do") == "del") del(t_c->get_text());
		}
	    }
    }    
    else if( t_id == "a_cntr" )
    {
	t_id = ctr_path_l(a_path,1);
	if( t_id == "t_bd" )       m_bd.tp    = ctr_opt_getS( opt );
	else if( t_id == "bd" )    m_bd.bd    = ctr_opt_getS( opt );
	else if( t_id == "tbl" )   m_bd.tbl   = ctr_opt_getS( opt );
	else if( t_id == "a_cfg" ) ctr_cfg_get( ctr_path_l(a_path,2), opt, this );
	else if( t_id == "a_st" )  
	{
	    t_id = ctr_path_l(a_path,2);
	    if( t_id == "en_st" )       { if( ctr_opt_getB( opt ) ) Enable(); else Disable(); }
	    else if( t_id == "run_st" ) { if( ctr_opt_getB( opt ) ) Start();  else Stop(); }
	}	
    }
}

void TController::ctr_cmd_go_( const string &a_path, XMLNode *fld, XMLNode *rez )
{
    string t_id = ctr_path_l(a_path,0);
    
    if( t_id == "a_prm" )
    {
    	t_id = ctr_path_l(a_path,1);    
	if( t_id == "load" )      LoadParmCfg();
	else if( t_id == "save" ) SaveParmCfg();
    }
    else if( t_id == "a_cntr" )
    {
    	t_id = ctr_path_l(a_path,1);    
	if( t_id == "a_cfg" )
	{
	    t_id = ctr_path_l(a_path,2);    
	    if( t_id == "load" )         Load(true);
	    else if( t_id == "save" )    Save(true);
	}
    }
}

AutoHD<TContr> TController::ctr_at1( const string &a_path )
{
    if( ctr_path_l(a_path,0) == "a_prm" )
    {
	string t_id = ctr_path_l(a_path,1);
	if( t_id == "prm" ) return at(ctr_path_l(a_path,2));
    }
    throw TError("(%s) Branch %s error",o_name,a_path.c_str());
}


