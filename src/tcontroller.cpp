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

//==== TController ====
TController::TController( const string &name_c, const SBDS &bd, TTipController *tcntr, TElem *cfgelem ) : 
    m_bd(bd), owner(tcntr), TConfig(cfgelem), run_st(false), en_st(false), m_hd(o_name), m_add_type(0),
    m_name(cfg("NAME").getS()), m_lname(cfg("LNAME").getS()), m_aen(cfg("ENABLE").getB()), m_astart(cfg("START").getB())  
{
    m_name = name_c; 
    TBDS &bds = Owner().Owner().Owner().BD();
    
    try
    {
	cfLoadValBD("NAME",bds.open(m_bd).at());
	bds.close(m_bd);
    }catch(...){ }    
}

TController::~TController(  )
{
    vector<string> c_list;

    if( run_st ) stop( );
    m_hd.lock();
    if( en_st )  disable( );
}

//TParamContr &TController::at( unsigned hd )
//{ 
//    return( *(TParamContr *)m_hd.hd_at( hd ) ); 
//}

void TController::load( bool self )
{
    TBDS &bds = Owner().Owner().Owner().BD();
    if( en_st || self ) 
    {
#if OSC_DEBUG
    	Owner().m_put("DEBUG",MESS_INFO,"%s: Load controller's configs!",name().c_str());	
#endif   

	cfLoadValBD("NAME",bds.open(m_bd).at());
	bds.close(m_bd);
	
	if( !self ) LoadParmCfg( );
	
	load_();        //Module load	
#if OSC_DEBUG
    	Owner().m_put("DEBUG",MESS_DEBUG,"%s: Load controller's configs ok!",name().c_str());	
#endif  
    }
    else throw TError("(%s) Controller %s no enable!",o_name,name().c_str());    
}

void TController::save( bool self )
{
    TBDS &bds = Owner().Owner().Owner().BD();
    if( en_st || self ) 
    {
#if OSC_DEBUG
	Owner().m_put("DEBUG",MESS_INFO,"%s: Save controller's configs!",name().c_str());	
#endif 	
	save_();	//Module save

	if( !self ) SaveParmCfg( );
	
	//Update type controller bd record
	AutoHD<TTable> tbl = bds.open(m_bd, true);
	owner->elUpdateBDAttr( tbl.at() );
	cfSaveValBD("NAME",tbl.at());
	tbl.at().save();
	tbl.free();
	bds.close(m_bd);
	
	//Update generic controller bd record
	tbl = bds.open(((TControllerS &)Owner().Owner()).BD(), true);
	TConfig *g_cfg = new TConfig((TControllerS *)(&Owner().Owner()));
	g_cfg->cfg("NAME").setS(name());
	g_cfg->cfg("MODUL").setS(Owner().modName());
	g_cfg->cfg("BDTYPE").setS(m_bd.tp);
	g_cfg->cfg("BDNAME").setS(m_bd.bd);
	g_cfg->cfg("TABLE").setS(m_bd.tbl);
	g_cfg->cfSaveValBD("NAME",tbl.at());
	delete g_cfg;
	tbl.at().save();
	tbl.free();
	bds.close(((TControllerS &)Owner().Owner()).BD());	
	
#if OSC_DEBUG
	Owner().m_put("DEBUG",MESS_DEBUG,"%s: Save controller's configs ok!",name().c_str());	
#endif 
    }
    else throw TError("(%s) Controller %s no enable!",o_name,name().c_str());    
} 

void TController::start( )
{        
    if( en_st && !run_st )
    {
#if OSC_DEBUG
	Owner().m_put("DEBUG",MESS_INFO,"%s: Start controller!",name().c_str());
#endif 	
	start_();          //Module start	
#if OSC_DEBUG
	Owner().m_put("DEBUG",MESS_DEBUG,"%s: Start controller ok!",name().c_str());
#endif 	
    }
    else if( run_st ) throw TError("(%s) Controller %s has started!",o_name,name().c_str());
    else throw TError("(%s) Controller %s no enable!",o_name,name().c_str());
}

void TController::stop( )
{
    if( run_st )
    {
#if OSC_DEBUG
	Owner().m_put("DEBUG",MESS_INFO,"%s: Stop controller!",name().c_str());
#endif	
	stop_();          //Module stop	
#if OSC_DEBUG
	Owner().m_put("DEBUG",MESS_DEBUG,"%s: Stop controller ok!",name().c_str());
#endif	
    }
    else throw TError("(%s) Controller %s is not starting!",o_name,name().c_str());
}

void TController::enable( )
{
    vector<string> c_list;    
    if( !en_st )
    {
#if OSC_DEBUG
	Owner().m_put("DEBUG",MESS_INFO,"%s: Enable controller!",name().c_str());
#endif
	en_st=true;
	try{ LoadParmCfg( ); } catch(...){ }
	//Export auto exported parameters
	list(c_list);
	for( unsigned i_ls = 0; i_ls < c_list.size(); i_ls++)
	{
	    AutoHD<TParamContr> prm = at(c_list[i_ls],"self_exp");
	    if( prm.at().toExport() && !prm.at().exportStat() ) prm.at().exportPrm();
	}
#if OSC_DEBUG
	Owner().m_put("DEBUG",MESS_DEBUG,"%s: Enable controller ok!",name().c_str());
#endif
    }
    else throw TError("(%s) Controller %s has enabled!",o_name,name().c_str());
}

void TController::disable( )
{
    vector<string> c_list;    
    if( en_st )
    {
#if OSC_DEBUG
	Owner().m_put("DEBUG",MESS_INFO,"%s: Disable controller!",name().c_str());
#endif	
	if( run_st ) stop();
	//UnExport all parameters
	list(c_list);
	for( unsigned i_ls = 0; i_ls < c_list.size(); i_ls++)
	{
	    AutoHD<TParamContr> prm = at(c_list[i_ls],"self_exp");
	    if( prm.at().exportStat() ) prm.at().unExportPrm();
	}
	//Free all parameters
	FreeParmCfg();
	
	en_st = false;	
#if OSC_DEBUG
	Owner().m_put("DEBUG",MESS_DEBUG,"%s: Disable controller ok!",name().c_str());
#endif
    }
    else throw TError("(%s) Controller %s is not enabling!",o_name,name().c_str());
}

void TController::LoadParmCfg(  )
{
    string      parm_bd;
    TParamContr *PrmCntr;

    TBDS    &bds  = Owner().Owner().Owner().BD();    
    TParamS &prms = Owner().Owner().Owner().Param();    
    
    for(unsigned i_tp = 0; i_tp < owner->tpPrmSize(); i_tp++)
    {
	SBDS n_bd( m_bd.tp, m_bd.bd, cfg(owner->tpPrmAt(i_tp).BD()).getS() );
	AutoHD<TTable> tbl = bds.open( n_bd );
	for(unsigned i=0; i < (unsigned)tbl.at().nLines( ); i++)
	{
	    try
	    {
		string n_prm = tbl.at().getCellS( tbl.at().columNameToId("SHIFR"),i);
		try{ add( n_prm, i_tp ); } catch(...){ }   //If already avoid
		at(n_prm,"self_ld").at().cfLoadValBD(i,tbl.at());
	    }catch(TError err) 
	    { Owner().m_put("SYS",MESS_ERR,"%s: %s!",name().c_str(),err.what().c_str()); }
	}
	tbl.free();
	bds.close( n_bd );
    }
}

void TController::SaveParmCfg(  )
{
    TBDS    &bds  = Owner().Owner().Owner().BD();    
    for(unsigned i_tp = 0; i_tp < owner->tpPrmSize(); i_tp++)
    {
	SBDS n_bd( m_bd.tp, m_bd.bd, cfg(owner->tpPrmAt(i_tp).BD()).getS() );
        AutoHD<TTable> tbl = bds.open( n_bd, true );
       	tbl.at().clean();
	//Update BD (resize, change atributes ..
    	owner->tpPrmAt(i_tp).elUpdateBDAttr(tbl.at());

	vector<string> c_list;    
    	list(c_list);
	for( unsigned i_ls = 0, i_bd=0; i_ls < c_list.size(); i_ls++)
	{
	    AutoHD<TParamContr> prm = at(c_list[i_ls],"self_sv");
	    if(prm.at().type().name() == owner->tpPrmAt(i_tp).name()) 
		prm.at().cfSaveValBD(i_bd++,tbl.at());
	}
	
	tbl.at().save( );
	tbl.free();
	bds.close(n_bd);
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
    try{ m_hd.obj_add( PrmCntr, &PrmCntr->name(), pos ); }
    catch(TError err) { delete PrmCntr; }
}

void TController::del( const string &name )
{ 
    delete (TParamContr *)m_hd.obj_del( name, 5 ); 
}

TParamContr *TController::ParamAttach( const string &name, int type)
{ 
    return(new TParamContr(name, &owner->tpPrmAt(type), this)); 
}

//================== Controll functions ========================
void TController::ctr_fill_info( XMLNode *inf )
{
    char *i_cntr = 
    	"<oscada_cntr>"
	" <area id='cntr'>"
	"  <fld id='t_bd' acs='0660' tp='str' dest='select' select='/cntr/b_mod'/>"
	"  <fld id='bd' acs='0660' tp='str'/>"
	"  <fld id='tbl' acs='0660' tp='str'/>"
	"  <list id='b_mod' tp='str' hide='1'/>"
	"  <area id='a_st'>"
	"   <fld id='en_st' acs='0664' tp='bool'/>"
	"   <fld id='run_st' acs='0664' tp='bool'/>"
	"  </area>"
	"  <area id='cfg'>"    
	"   <comm id='load' acs='0550'/>"
	"   <comm id='save' acs='0550'/>"
	"  </area>"
	" </area>"
	" <area id='prm'>"
	"  <fld id='t_prm' acs='0660' tp='str' dest='select' select='/prm/t_lst'/>"
	"  <list id='t_lst' tp='str' mode='st' hide='1'/>"
	"  <list id='prm' s_com='add,ins,del' tp='br' mode='att'/>"
	"  <comm id='load' acs='0550'/>"
	"  <comm id='save' acs='0550'/>"
	" </area>"	
	"</oscada_cntr>";
    char *dscr="dscr";
    XMLNode *t_cntr;

    inf->load_xml( i_cntr );
    inf->set_text(Mess->I18Ns("Controller: ")+name());
    t_cntr = inf->get_child(0);
    t_cntr->set_attr(dscr,Mess->I18N("Controller"));
    t_cntr->get_child(0)->set_attr(dscr,Mess->I18N("Type controller BD (module:bd:table)"));    
    t_cntr = t_cntr->get_child(4);    
    t_cntr->set_attr(dscr,Mess->I18N("State"));    
    t_cntr->get_child(0)->set_attr(dscr,Mess->I18N("Enable"));
    t_cntr->get_child(1)->set_attr(dscr,Mess->I18N("Run"));
    
    t_cntr = inf->get_child(0)->get_child(5);    
    t_cntr->set_attr(dscr,Mess->I18N("Config"));
    t_cntr->get_child(0)->set_attr(dscr,Mess->I18N("Load"));
    t_cntr->get_child(1)->set_attr(dscr,Mess->I18N("Save"));    

    ctr_cfg_parse("/cntr/cfg",inf, 0, this);  //Generate individual controller config from TConfig 
    //t_cntr->get_child(2)->set_attr("acs","0444");    //No write acces to name    
    
    if( !Owner().tpPrmSize() || !enableStat() ) inf->del_child(0); 
    else
    {
    	t_cntr = inf->get_child(1);
	t_cntr->set_attr(dscr,Mess->I18N("Parameters"));
    	t_cntr->get_child(0)->set_attr(dscr,Mess->I18N("Parameter type for add operation"));
	t_cntr->get_child(2)->set_attr(dscr,Mess->I18N("Parameters"));
	t_cntr->get_child(3)->set_attr(dscr,Mess->I18N("Load"));
	t_cntr->get_child(4)->set_attr(dscr,Mess->I18N("Save"));
    }    
}

void TController::ctr_din_get_( const string &a_path, XMLNode *opt )
{
    vector<string> c_list;
    
    if( a_path == "/prm/t_prm" )	ctr_opt_setS( opt, Owner().tpPrmAt(m_add_type).lName() );
    else if( a_path == "/prm/prm" )
    {
	list(c_list);
	opt->clean_childs();
	for( unsigned i_a=0; i_a < c_list.size(); i_a++ )
	    ctr_opt_setS( opt, c_list[i_a], i_a ); 	
    }
    else if( a_path == "/prm/t_lst" )
    {
	opt->clean_childs();
	for( unsigned i_a=0; i_a < Owner().tpPrmSize(); i_a++ )
	    ctr_opt_setS( opt, Owner().tpPrmAt(i_a).lName(), i_a ); 	
    }
    else if( a_path == "/cntr/t_bd" )	ctr_opt_setS( opt, m_bd.tp );
    else if( a_path == "/cntr/bd" ) 	ctr_opt_setS( opt, m_bd.bd );
    else if( a_path == "/cntr/tbl" )	ctr_opt_setS( opt, m_bd.tbl );
    else if( a_path == "/cntr/b_mod" )
    {
	opt->clean_childs();
	Owner().Owner().Owner().BD().gmdList(c_list);
	for( unsigned i_a=0; i_a < c_list.size(); i_a++ )
	    ctr_opt_setS( opt, c_list[i_a], i_a );
    }
    else if( a_path == "/cntr/a_st/en_st" )	ctr_opt_setB( opt, en_st );
    else if( a_path == "/cntr/a_st/run_st" )	ctr_opt_setB( opt, run_st );
    else if( a_path.substr(0,9) == "/cntr/cfg" )	ctr_cfg_set( ctr_path_l(a_path,2), opt, this );
    else throw TError("(%s) Branch %s error!",o_name,a_path.c_str());
}

void TController::ctr_din_set_( const string &a_path, XMLNode *opt )
{
    if( a_path == "/prm/t_prm" )	m_add_type = atoi(ctr_opt_getS( opt ).c_str());
    else if( a_path.substr(0,8) == "/prm/prm" )
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
    else if( a_path == "/cntr/t_bd" )		m_bd.tp    = ctr_opt_getS( opt );
    else if( a_path == "/cntr/bd" )    		m_bd.bd    = ctr_opt_getS( opt );
    else if( a_path == "/cntr/tbl" )   		m_bd.tbl   = ctr_opt_getS( opt );
    else if( a_path.substr(0,9) == "/cntr/cfg" ) 	ctr_cfg_get( ctr_path_l(a_path,2), opt, this );
    else if( a_path == "/cntr/a_st/en_st" )	{ if( ctr_opt_getB( opt ) ) enable(); else disable(); }
    else if( a_path == "/cntr/a_st/run_st" )	{ if( ctr_opt_getB( opt ) ) start();  else stop(); }
    else throw TError("(%s) Branch %s error!",o_name,a_path.c_str());
}

void TController::ctr_cmd_go_( const string &a_path, XMLNode *fld, XMLNode *rez )
{
    if( a_path == "/prm/load" )      		LoadParmCfg();
    else if( a_path == "/prm/save" ) 		SaveParmCfg();
    else if( a_path == "/cntr/cfg/load" )	load(true);
    else if( a_path == "/cntr/cfg/save" )	save(true);
    else throw TError("(%s) Branch %s error!",o_name,a_path.c_str());
}

AutoHD<TContr> TController::ctr_at1( const string &a_path )
{
    if( a_path.substr(0,8) == "/prm/prm" ) return at( ctr_path_l(a_path,2) );
    else throw TError("(%s) Branch %s error!",o_name,a_path.c_str());
}


