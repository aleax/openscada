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

#include "tsys.h"
#include "tkernel.h"
#include "tmessage.h"
#include "tcontrollers.h"
#include "ttiparam.h"
#include "tparamcontr.h"
#include "tparams.h"
#include "tcontroller.h"

//==== TController ====
TController::TController( const string &name_c, const TBDS::SName &bd, TTipController *tcntr, TElem *cfgelem ) : 
    m_bd(bd), m_owner(tcntr), TConfig(cfgelem), run_st(false), en_st(false), m_add_type(0),
    m_name(cfg("NAME").getSd()), m_lname(cfg("LNAME").getSd()), m_aen(cfg("ENABLE").getBd()), m_astart(cfg("START").getBd())  
{
    m_prm = grpAdd();
    
    m_name = name_c; 
}

TController::~TController(  )
{
    delAll();
}

void TController::preDisable(int flag)
{
    //Disable controller if it enabled
    if( en_st )  disable( );
}

void TController::postDisable(int flag)
{
    try
    {
	if( flag )
	{
	    TBDS &bds = owner().owner().owner().db();
	    
	    //Delete from controllers BD
	    TConfig g_cfg((TControllerS *)(&owner().owner()));
	    g_cfg.cfg("NAME").setS(name());
	    g_cfg.cfg("MODUL").setS(owner().modId());
	    bds.open(((TControllerS &)owner().owner()).BD()).at().fieldDel(g_cfg);
	    bds.close(((TControllerS &)owner().owner()).BD());		
	    
	    //Delete from type BD
	    bds.open(BD()).at().fieldDel(*this);
	    bds.close(BD());
	    
	    //Delete parameter's tables
	    AutoHD<TBD> cbd = ((TTipBD &)bds.gmdAt(BD().tp).at()).at(BD().bd);
            for(unsigned i_tp = 0; i_tp < m_owner->tpPrmSize(); i_tp++)
	    	cbd.at().del(cfg(m_owner->tpPrmAt(i_tp).BD()).getS());
	}
    }catch(TError err)
    { owner().mPut("SYS",TMess::Error,"%s",err.what().c_str()); }
}

TBDS::SName TController::BD()
{ 
    return owner().owner().owner().nameDBPrep(m_bd);
}

void TController::load( )
{
#if OSC_DEBUG
    owner().mPut("DEBUG",TMess::Info,"%s: Load controller's configs!",name().c_str());
#endif	

    //Update type controller bd record
    TBDS &bds = owner().owner().owner().db();
    bds.open(BD()).at().fieldGet(*this);
    bds.close(BD());
    
    //Load parameters if enabled
    if( en_st )	LoadParmCfg( );
    
    //Load for children    	    
    load_();

#if OSC_DEBUG
    owner().mPut("DEBUG",TMess::Debug,"%s: Load controller's configs ok!",name().c_str());
#endif	    
}

void TController::save( )
{
#if OSC_DEBUG
    owner().mPut("DEBUG",TMess::Info,"%s: Save controller's configs!",name().c_str());
#endif

    //Update type controller bd record
    TBDS &bds = owner().owner().owner().db();
    bds.open(BD(),true).at().fieldSet(*this);
    bds.close(BD());
	    
    //Update generic controller bd record
    AutoHD<TTable> tbl = bds.open(((TControllerS &)owner().owner()).BD(), true);
    TConfig g_cfg((TControllerS *)(&owner().owner()));
    g_cfg.cfg("NAME").setS(name());
    g_cfg.cfg("MODUL").setS(owner().modId());
    g_cfg.cfg("BDTYPE").setS(m_bd.tp);
    g_cfg.cfg("BDNAME").setS(m_bd.bd);
    g_cfg.cfg("TABLE").setS(m_bd.tbl);
    tbl.at().fieldSet(g_cfg);
    tbl.free();
    bds.close(((TControllerS &)owner().owner()).BD());
    
    //Save parameters if enabled
    if( en_st ) SaveParmCfg( );

    //Save for children
    save_();

#if OSC_DEBUG
    owner().mPut("DEBUG",TMess::Debug,"%s: Save controller's configs ok!",name().c_str());
#endif   
} 

void TController::start( )
{   
    //Enable if no enabled
    if( !en_st ) enable();

#if OSC_DEBUG
    owner().mPut("DEBUG",TMess::Info,"%s: Start controller!",name().c_str());
#endif

    //Start for children
    start_();
	    
#if OSC_DEBUG
    owner().mPut("DEBUG",TMess::Debug,"%s: Start controller ok!",name().c_str());
#endif
}

void TController::stop( )
{
    if( !run_st ) return;
    
#if OSC_DEBUG
    owner().mPut("DEBUG",TMess::Info,"%s: Stop controller!",name().c_str());
#endif

    //Stop for children
    stop_();
    
#if OSC_DEBUG
    owner().mPut("DEBUG",TMess::Debug,"%s: Stop controller ok!",name().c_str());
#endif
}

void TController::enable( )
{
    if( en_st )	return;

#if OSC_DEBUG
    owner().mPut("DEBUG",TMess::Info,"%s: Enable controller!",name().c_str());
#endif
    //Load parameters
    LoadParmCfg( );
    
    //Export parameters
    vector<string> c_list;
    list(c_list);
    for( unsigned i_ls = 0; i_ls < c_list.size(); i_ls++)
    {
        AutoHD<TParamContr> prm = at(c_list[i_ls],"self_exp");
        if( prm.at().toExport() && !prm.at().exportStat() ) prm.at().exportPrm();
    }
    
    //Enable for children
    enable_();
    
    //Set enable stat flag
    en_st=true;    

#if OSC_DEBUG
    owner().mPut("DEBUG",TMess::Debug,"%s: Enable controller ok!",name().c_str());
#endif
}

void TController::disable( )
{
    if( !en_st ) return;
    //Stop if runed
    if( run_st ) stop();
    
#if OSC_DEBUG
    owner().mPut("DEBUG",TMess::Info,"%s: Disable controller!",name().c_str());
#endif

    //Disable for children
    disable_();	    
    
    //Unexport parameters
    vector<string> c_list;
    list(c_list);
    for( unsigned i_ls = 0; i_ls < c_list.size(); i_ls++)
    {
        AutoHD<TParamContr> prm = at(c_list[i_ls],"self_exp");
        if( prm.at().exportStat() ) prm.at().unExportPrm();
    }
    
    //Free all parameters
    FreeParmCfg();
		
    //Clear enable flag    
    en_st = false;							    

#if OSC_DEBUG
    owner().mPut("DEBUG",TMess::Debug,"%s: Disable controller ok!",name().c_str());
#endif
}

void TController::LoadParmCfg(  )
{
    string      parm_bd;
    TParamContr *PrmCntr;

    TBDS    &bds  = owner().owner().owner().db();    
    TParamS &prms = owner().owner().owner().param();    
    
    for(unsigned i_tp = 0; i_tp < m_owner->tpPrmSize(); i_tp++)
    {	
	try
	{
    	    int fld_cnt = 0;
    	    TConfig c_el(&m_owner->tpPrmAt(i_tp));
	    
     	    TBDS::SName n_bd( BD().tp.c_str(), BD().bd.c_str(), cfg(m_owner->tpPrmAt(i_tp).BD()).getS().c_str() );
    	    AutoHD<TTable> tbl = bds.open(n_bd);
    	    while( tbl.at().fieldSeek(fld_cnt++,c_el) )
    	    {		
    		try
    		{
    		    string name = c_el.cfg("SHIFR").getS();
    		    if( !avoid(name) )
    		    {
    			add( name, i_tp );
    			((TConfig &)at(name).at()) = c_el;			    
    		    }
    		    else at(name).at().load();
    		}catch(TError err)
    		{ owner().mPut("SYS",TMess::Error,"%s",err.what().c_str()); }
    	    }
    	    tbl.free();
    	    bds.close( n_bd );
	}catch(TError err) 
	{ owner().mPut("SYS",TMess::Error,"%s",err.what().c_str()); }
    }
}

void TController::SaveParmCfg(  )
{
    vector<string> c_list;
    
    list(c_list);		    
    for( unsigned i_ls = 0, i_bd=0; i_ls < c_list.size(); i_ls++)
        at(c_list[i_ls]).at().save();
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
    if( chldAvoid(m_prm,name) ) return;
    chldAdd(m_prm,ParamAttach( name, type )); 
}

TParamContr *TController::ParamAttach( const string &name, int type)
{ 
    return(new TParamContr(name, &m_owner->tpPrmAt(type), this)); 
}

//================== Controll functions ========================
void TController::cntrCmd_( const string &a_path, XMLNode *opt, int cmd )
{
    vector<string> c_list;
	
    if( cmd==TCntrNode::Info )
    {
    	ctrMkNode("oscada_cntr",opt,a_path.c_str(),"/",Mess->I18Ns("Controller: ")+name());
	ctrMkNode("area",opt,a_path.c_str(),"/cntr",Mess->I18N("Controller"));
	if( owner().owner().owner().genDB( ) )
	    ctrMkNode("fld",opt,a_path.c_str(),"/cntr/tbl",Mess->I18N("Type controller table"),0660,0,0,"str");
	else
	{
	    ctrMkNode("fld",opt,a_path.c_str(),"/cntr/t_bd",Mess->I18N("Type controller BD (module:bd:table)"),0660,0,0,"str")->
	     	attr_("dest","select")->attr_("select","/cntr/b_mod");
	    ctrMkNode("fld",opt,a_path.c_str(),"/cntr/bd","",0660,0,0,"str");
	    ctrMkNode("fld",opt,a_path.c_str(),"/cntr/tbl","",0660,0,0,"str");
	}
	ctrMkNode("area",opt,a_path.c_str(),"/cntr/st",Mess->I18N("State"));
	ctrMkNode("fld",opt,a_path.c_str(),"/cntr/st/en_st",Mess->I18N("Enable"),0664,0,0,"bool");
	ctrMkNode("fld",opt,a_path.c_str(),"/cntr/st/run_st",Mess->I18N("Run"),0664,0,0,"bool");
	ctrMkNode("area",opt,a_path.c_str(),"/cntr/cfg",Mess->I18N("Config"));
	ctrMkNode("comm",opt,a_path.c_str(),"/cntr/cfg/load",Mess->I18N("Load from BD"),0550);
	ctrMkNode("comm",opt,a_path.c_str(),"/cntr/cfg/save",Mess->I18N("Save to BD"),0550);
	cntrMake(opt,a_path.c_str(),"/cntr/cfg",0);
    	if( owner().tpPrmSize() && enableStat() )
	{
	    ctrMkNode("area",opt,a_path.c_str(),"/prm",Mess->I18N("Parameters"));
	    ctrMkNode("fld",opt,a_path.c_str(),"/prm/t_prm",Mess->I18N("To add parameters"),0660,0,0,"str")->
	     	attr_("dest","select")->attr_("select","/prm/t_lst");
	    ctrMkNode("list",opt,a_path.c_str(),"/prm/prm",Mess->I18N("Parameters"),0660,0,0,"br")->
		attr_("s_com","add,del")->attr_("mode","att")->attr_("br_pref","_");
	    ctrMkNode("comm",opt,a_path.c_str(),"/prm/load",Mess->I18N("Load from BD"),0550);
	    ctrMkNode("comm",opt,a_path.c_str(),"/prm/save",Mess->I18N("Save to BD"),0550);
	}	
    }
    else if( cmd==TCntrNode::Get )
    {    
	if( a_path == "/prm/t_prm" )	ctrSetS( opt, owner().tpPrmAt(m_add_type).name() );
	else if( a_path == "/prm/prm" )
	{
	    list(c_list);
	    opt->childClean();
	    for( unsigned i_a=0; i_a < c_list.size(); i_a++ )
		ctrSetS( opt, c_list[i_a] ); 	
	}
	else if( a_path == "/prm/t_lst" )
	{
	    opt->childClean();
	    for( unsigned i_a=0; i_a < owner().tpPrmSize(); i_a++ )
		ctrSetS( opt, owner().tpPrmAt(i_a).lName(), owner().tpPrmAt(i_a).name().c_str() );
	}
	else if( a_path == "/cntr/t_bd" )	ctrSetS( opt, m_bd.tp );
	else if( a_path == "/cntr/bd" ) 	ctrSetS( opt, m_bd.bd );
	else if( a_path == "/cntr/tbl" )	ctrSetS( opt, m_bd.tbl );
	else if( a_path == "/cntr/b_mod" )
	{
	    opt->childClean();
	    owner().owner().owner().db().gmdList(c_list);
	    ctrSetS( opt, "" );
	    for( unsigned i_a=0; i_a < c_list.size(); i_a++ )
		ctrSetS( opt, c_list[i_a] );
	}
	else if( a_path == "/cntr/st/en_st" )	ctrSetB( opt, en_st );
	else if( a_path == "/cntr/st/run_st" )	ctrSetB( opt, run_st );
	else if( a_path.substr(0,9) == "/cntr/cfg" )TConfig::cntrCmd(TSYS::pathLev(a_path,2), opt, TCntrNode::Get );
	else throw TError("(%s) Branch %s error!",__func__,a_path.c_str());
    }
    else if( cmd==TCntrNode::Set )
    {
	if( a_path == "/prm/t_prm" )	m_add_type = owner().tpPrmToId(ctrGetS( opt ));
	else if( a_path == "/prm/prm" )
	{
	    if( opt->name() == "add" )	add(opt->text(),m_add_type);
	    else if( opt->name() == "del" )
	    {
    		AutoHD<TParamContr> prm = at(opt->text());
		TBDS::SName nm_bd( BD().tp.c_str(), BD().bd.c_str(), cfg(prm.at().type().BD()).getS().c_str() );
		TConfig conf(&prm.at().type());
		conf = prm.at();
		prm.free();
		//Delete
		del(opt->text());
		//Delete from BD
		owner().owner().owner().db().open(nm_bd).at().fieldDel(conf);
		owner().owner().owner().db().close(nm_bd);
	    }
	}
	else if( a_path == "/prm/load" )	LoadParmCfg();
	else if( a_path == "/prm/save" )	SaveParmCfg();
	else if( a_path == "/cntr/t_bd" )	m_bd.tp    = ctrGetS( opt );
	else if( a_path == "/cntr/bd" ) 	m_bd.bd    = ctrGetS( opt );
	else if( a_path == "/cntr/tbl" )	m_bd.tbl   = ctrGetS( opt );
	else if( a_path == "/cntr/cfg/load" )	load();
	else if( a_path == "/cntr/cfg/save" )	save();	
	else if( a_path.substr(0,9) == "/cntr/cfg" )TConfig::cntrCmd(TSYS::pathLev(a_path,2), opt, TCntrNode::Set );
	else if( a_path == "/cntr/st/en_st" )	{ if( ctrGetB( opt ) ) enable(); else disable(); }
	else if( a_path == "/cntr/st/run_st" )	{ if( ctrGetB( opt ) ) start();  else stop(); }
	else throw TError("(%s) Branch %s error!",__func__,a_path.c_str());	    
    }
}

AutoHD<TCntrNode> TController::ctrAt1( const string &a_path )
{
    if( a_path.substr(0,1) == "_" ) return at( TSYS::strEncode(a_path.substr(1),TSYS::PathEl) );
    else throw TError("(%s) Branch %s error!",__func__,a_path.c_str());
}
