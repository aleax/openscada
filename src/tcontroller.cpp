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

//==== TController ====
TController::TController( const string &name_c, const TBDS::SName &bd, TTipController *tcntr, TElem *cfgelem ) : 
    m_bd(bd), m_owner(tcntr), TConfig(cfgelem), run_st(false), en_st(false), m_add_type(0),
    m_name(cfg("NAME").getS()), m_lname(cfg("LNAME").getS()), m_aen(cfg("ENABLE").getB()), m_astart(cfg("START").getB())  
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
	    TBDS &bds = owner().owner().owner().BD();
	    
	    //Delete from controllers BD
	    TConfig g_cfg((TControllerS *)(&owner().owner()));
	    g_cfg.cfg("NAME").setS(name());
	    g_cfg.cfg("MODUL").setS(owner().modName());
	    bds.open(((TControllerS &)owner().owner()).BD()).at().fieldDel(g_cfg);
	    bds.close(((TControllerS &)owner().owner()).BD());		
	    
	    //Delete from type BD
	    bds.open(BD()).at().fieldDel(*this);
	    bds.close(BD());
	    
	    //Delete parameter's tables
	    AutoHD<TBD> cbd = ((TTipBD &)bds.gmdAt(m_bd.tp).at()).at(m_bd.bd);
            for(unsigned i_tp = 0; i_tp < m_owner->tpPrmSize(); i_tp++)
	    	cbd.at().del(cfg(m_owner->tpPrmAt(i_tp).BD()).getS());
	}
    }catch(TError err)
    { owner().mPut("SYS",MESS_ERR,"%s",err.what().c_str()); }
}

void TController::load( )
{
#if OSC_DEBUG
    owner().mPut("DEBUG",MESS_INFO,"%s: Load controller's configs!",name().c_str());
#endif	

    //Update type controller bd record
    TBDS &bds = owner().owner().owner().BD();
    bds.open(m_bd).at().fieldGet(*this);
    bds.close(m_bd);
    
    //Load parameters if enabled
    if( en_st )	LoadParmCfg( );
    
    //Load for children    	    
    load_();

#if OSC_DEBUG
    owner().mPut("DEBUG",MESS_DEBUG,"%s: Load controller's configs ok!",name().c_str());
#endif	    
}

void TController::save( )
{
#if OSC_DEBUG
    owner().mPut("DEBUG",MESS_INFO,"%s: Save controller's configs!",name().c_str());
#endif

    //Update type controller bd record
    TBDS &bds = owner().owner().owner().BD();
    bds.open(m_bd,true).at().fieldSet(*this);
    bds.close(m_bd);
	    
    //Update generic controller bd record
    AutoHD<TTable> tbl = bds.open(((TControllerS &)owner().owner()).BD(), true);
    TConfig g_cfg((TControllerS *)(&owner().owner()));
    g_cfg.cfg("NAME").setS(name());
    g_cfg.cfg("MODUL").setS(owner().modName());
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
    owner().mPut("DEBUG",MESS_DEBUG,"%s: Save controller's configs ok!",name().c_str());
#endif   
} 

void TController::start( )
{   
    //Enable if no enabled
    if( !en_st ) enable();

#if OSC_DEBUG
    owner().mPut("DEBUG",MESS_INFO,"%s: Start controller!",name().c_str());
#endif

    //Start for children
    start_();
	    
#if OSC_DEBUG
    owner().mPut("DEBUG",MESS_DEBUG,"%s: Start controller ok!",name().c_str());
#endif
}

void TController::stop( )
{
    if( !run_st ) return;
    
#if OSC_DEBUG
    owner().mPut("DEBUG",MESS_INFO,"%s: Stop controller!",name().c_str());
#endif

    //Stop for children
    stop_();
    
#if OSC_DEBUG
    owner().mPut("DEBUG",MESS_DEBUG,"%s: Stop controller ok!",name().c_str());
#endif
}

void TController::enable( )
{
    if( en_st )	return;

#if OSC_DEBUG
    owner().mPut("DEBUG",MESS_INFO,"%s: Enable controller!",name().c_str());
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
    owner().mPut("DEBUG",MESS_DEBUG,"%s: Enable controller ok!",name().c_str());
#endif
}

void TController::disable( )
{
    if( !en_st ) return;
    //Stop if runed
    if( run_st ) stop();
    
#if OSC_DEBUG
    owner().mPut("DEBUG",MESS_INFO,"%s: Disable controller!",name().c_str());
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
    owner().mPut("DEBUG",MESS_DEBUG,"%s: Disable controller ok!",name().c_str());
#endif
}

void TController::LoadParmCfg(  )
{
    string      parm_bd;
    TParamContr *PrmCntr;

    TBDS    &bds  = owner().owner().owner().BD();    
    TParamS &prms = owner().owner().owner().Param();    
    
    for(unsigned i_tp = 0; i_tp < m_owner->tpPrmSize(); i_tp++)
    {	
	try
	{
    	    int fld_cnt = 0;
    	    TConfig c_el(&m_owner->tpPrmAt(i_tp));
	
     	    TBDS::SName n_bd( m_bd.tp, m_bd.bd, cfg(m_owner->tpPrmAt(i_tp).BD()).getS() );
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
    		{ owner().mPut("SYS",MESS_ERR,"%s",err.what().c_str()); }
    	    }
    	    tbl.free();
    	    bds.close( n_bd );
	}catch(TError err) 
	{ owner().mPut("SYS",MESS_ERR,"%s",err.what().c_str()); }
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
void TController::ctrStat_( XMLNode *inf )
{
    char *i_cntr = 
    	"<oscada_cntr>"
	" <area id='cntr'>"
	"  <fld id='t_bd' acs='0660' tp='str' dest='select' select='/cntr/b_mod'/>"
	"  <fld id='bd' acs='0660' tp='str'/>"
	"  <fld id='tbl' acs='0660' tp='str'/>"
	"  <area id='st'>"
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
	"  <list id='prm' s_com='add,ins,del' tp='br' mode='att' br_pref='_'/>"
	"  <comm id='load' acs='0550'/>"
	"  <comm id='save' acs='0550'/>"
	" </area>"	
	"</oscada_cntr>";
    char *dscr="dscr";
    XMLNode *t_cntr;

    inf->load( i_cntr );
    inf->text(Mess->I18Ns("Controller: ")+name());
    t_cntr = inf->childGet(0);
    t_cntr->attr(dscr,Mess->I18N("Controller"));
    t_cntr->childGet(0)->attr(dscr,Mess->I18N("Type controller BD (module:bd:table)"));    
    t_cntr = t_cntr->childGet(3);    
    t_cntr->attr(dscr,Mess->I18N("State"));    
    t_cntr->childGet(0)->attr(dscr,Mess->I18N("Enable"));
    t_cntr->childGet(1)->attr(dscr,Mess->I18N("Run"));
    
    t_cntr = inf->childGet(0)->childGet(4);    
    t_cntr->attr(dscr,Mess->I18N("Config"));
    t_cntr->childGet(0)->attr(dscr,Mess->I18N("Load from BD"));
    t_cntr->childGet(1)->attr(dscr,Mess->I18N("Save to BD"));

    cntrMake("/cntr/cfg",inf, 0);
    //t_cntr->childGet(2)->attr("acs","0444");    //No write acces to name    
    
    if( !owner().tpPrmSize() || !enableStat() ) inf->childDel(1); 
    else
    {
    	t_cntr = inf->childGet(1);
	t_cntr->attr(dscr,Mess->I18N("Parameters"));
    	t_cntr->childGet(0)->attr(dscr,Mess->I18N("Parameter type for add operation"));
	t_cntr->childGet(1)->attr(dscr,Mess->I18N("Parameters"));
	t_cntr->childGet(2)->attr(dscr,Mess->I18N("Load from BD"));
	t_cntr->childGet(3)->attr(dscr,Mess->I18N("Save to BD"));
    }    
}

void TController::ctrDinGet_( const string &a_path, XMLNode *opt )
{
    vector<string> c_list;
    
    if( a_path == "/prm/t_prm" )	ctrSetS( opt, owner().tpPrmAt(m_add_type).lName() );
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
	    ctrSetS( opt, owner().tpPrmAt(i_a).lName() ); 	
    }
    else if( a_path == "/cntr/t_bd" )	ctrSetS( opt, m_bd.tp );
    else if( a_path == "/cntr/bd" ) 	ctrSetS( opt, m_bd.bd );
    else if( a_path == "/cntr/tbl" )	ctrSetS( opt, m_bd.tbl );
    else if( a_path == "/cntr/b_mod" )
    {
	opt->childClean();
	owner().owner().owner().BD().gmdList(c_list);
	for( unsigned i_a=0; i_a < c_list.size(); i_a++ )
	    ctrSetS( opt, c_list[i_a] );
    }
    else if( a_path == "/cntr/st/en_st" )	ctrSetB( opt, en_st );
    else if( a_path == "/cntr/st/run_st" )	ctrSetB( opt, run_st );
    else if( a_path.substr(0,9) == "/cntr/cfg" )TConfig::cntrCmd(pathLev(a_path,2), opt, TCntrNode::Get );
    else throw TError("(%s) Branch %s error!",__func__,a_path.c_str());
}

void TController::ctrDinSet_( const string &a_path, XMLNode *opt )
{
    if( a_path == "/prm/t_prm" )	m_add_type = atoi(ctrGetS( opt ).c_str());
    else if( a_path.substr(0,8) == "/prm/prm" )
	for( int i_el=0; i_el < opt->childSize(); i_el++)	    
	{
	    XMLNode *t_c = opt->childGet(i_el);
	    if( t_c->name() == "el")
	    {
		if(t_c->attr("do") == "add")      add(t_c->text(),m_add_type);
		else if(t_c->attr("do") == "ins") add(t_c->text(),m_add_type,atoi(t_c->attr("id").c_str()));
		else if(t_c->attr("do") == "del") 
		{
		    AutoHD<TParamContr> prm = at(t_c->text());
		    TBDS::SName nm_bd( BD().tp, BD().bd, cfg(prm.at().type().BD()).getS() );
		    TConfig conf(&prm.at().type());
		    conf = prm.at();
		    prm.free();
		    //Delete
		    del(t_c->text());
		    //Delete from BD
		    owner().owner().owner().BD().open(nm_bd).at().fieldDel(conf);
		    owner().owner().owner().BD().close(nm_bd);
		}
	    }
	}
    else if( a_path == "/prm/load" )	LoadParmCfg();
    else if( a_path == "/prm/save" )	SaveParmCfg();
    else if( a_path == "/cntr/t_bd" )	m_bd.tp    = ctrGetS( opt );
    else if( a_path == "/cntr/bd" ) 	m_bd.bd    = ctrGetS( opt );
    else if( a_path == "/cntr/tbl" )	m_bd.tbl   = ctrGetS( opt );
    else if( a_path == "/cntr/cfg/load" )	load();
    else if( a_path == "/cntr/cfg/save" )	save();	
    else if( a_path.substr(0,9) == "/cntr/cfg" )TConfig::cntrCmd(pathLev(a_path,2), opt, TCntrNode::Set );
    else if( a_path == "/cntr/st/en_st" )	{ if( ctrGetB( opt ) ) enable(); else disable(); }
    else if( a_path == "/cntr/st/run_st" )	{ if( ctrGetB( opt ) ) start();  else stop(); }
    else throw TError("(%s) Branch %s error!",__func__,a_path.c_str());
}

AutoHD<TCntrNode> TController::ctrAt1( const string &a_path )
{
    if( a_path.substr(0,1) == "_" ) return at( pathEncode(a_path.substr(1),true) );
    else throw TError("(%s) Branch %s error!",__func__,a_path.c_str());
}


