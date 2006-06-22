
//OpenSCADA system file: tcontroller.cpp
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

#include "tsys.h"
#include "tmess.h"
//#include "tdaqs.h"
#include "ttiparam.h"
//#include "tparamcontr.h"
//#include "tparams.h"
#include "tcontroller.h"

//==== TController ====
TController::TController( const string &id_c, const string &daq_db, TElem *cfgelem ) :
    m_bd(daq_db), TConfig(cfgelem), run_st(false), en_st(false), m_add_type(0),
    m_id(cfg("ID").getSd()), m_name(cfg("NAME").getSd()), m_descr(cfg("DESCR").getSd()),
    m_aen(cfg("ENABLE").getBd()), m_astart(cfg("START").getBd())
{
    m_id = id_c;
    m_prm = grpAdd("prm_");
}

TController::~TController(  )
{
    nodeDelAll();
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
	    //Delete DB record
	    SYS->db().at().dataDel(BD(),owner().nodePath()+"DAQ/",*this);

	    //Delete parameter's tables
            for(unsigned i_tp = 0; i_tp < owner().tpPrmSize(); i_tp++)
	    {
		string tbl = genBD()+"."+cfg(owner().tpPrmAt(i_tp).BD()).getS();
		SYS->db().at().open(tbl);
		SYS->db().at().close(tbl,true);
	    }
	}
    }catch(TError err)
    { Mess->put(err.cat.c_str(),TMess::Error,"%s",err.mess.c_str()); }
}

string TController::name()   
{ 
    return (m_name.size())?m_name:id();
}

string TController::BD()
{
    return m_bd+"."+owner().owner().subId()+"_"+owner().modId();
}

void TController::load( )
{
#if OSC_DEBUG
    Mess->put(nodePath().c_str(),TMess::Info,Mess->I18N("Load controller's configs!"));
#endif

    SYS->db().at().dataGet(BD(),owner().nodePath()+"DAQ/",*this);

    //Load parameters if enabled
    if( en_st )	LoadParmCfg( );
}

void TController::save( )
{
#if OSC_DEBUG
    Mess->put(nodePath().c_str(),TMess::Info,Mess->I18N("Save controller's configs!"));
#endif

    //Update type controller bd record
    SYS->db().at().dataSet(BD(),owner().nodePath()+"DAQ/",*this);

    //Save parameters if enabled
    if( en_st ) SaveParmCfg( );
}

void TController::start( )
{
    //Enable if no enabled
    if( run_st ) return;
    if( !en_st ) enable();

#if OSC_DEBUG
    Mess->put(nodePath().c_str(),TMess::Info,Mess->I18N("Start controller!"));
#endif
}

void TController::stop( )
{
    if( !run_st ) return;

#if OSC_DEBUG
    Mess->put(nodePath().c_str(),TMess::Info,Mess->I18N("Stop controller!"));
#endif
}

void TController::enable( )
{
    if( en_st )	return;

#if OSC_DEBUG
    Mess->put(nodePath().c_str(),TMess::Info,Mess->I18N("Enable controller!"));
#endif
    //Load parameters
    LoadParmCfg( );

    //Enable for children
    enable_();

    //Set enable stat flag
    en_st=true;
}

void TController::disable( )
{
    if( !en_st ) return;
    //Stop if runed
    if( run_st ) stop();

#if OSC_DEBUG
    Mess->put(nodePath().c_str(),TMess::Info,Mess->I18N("Disable controller!"));
#endif

    //Disable for children
    disable_();

    //Free all parameters
    //FreeParmCfg();

    //Clear enable flag
    en_st = false;
}

void TController::LoadParmCfg(  )
{
    string      parm_bd;
    TParamContr *PrmCntr;

    //Search and create new parameters
    for( int i_tp = 0; i_tp < owner().tpPrmSize(); i_tp++ )
    {
	try
	{
    	    TConfig c_el(&owner().tpPrmAt(i_tp));
	    
	    int fld_cnt = 0;
	    while( SYS->db().at().dataSeek(genBD()+"."+cfg(owner().tpPrmAt(i_tp).BD()).getS(),
					   owner().nodePath()+cfg(owner().tpPrmAt(i_tp).BD()).getS(),fld_cnt++,c_el) )
    	    {
    		try
		{ 
		    if( !present(c_el.cfg("SHIFR").getS()) ) 
			add( c_el.cfg("SHIFR").getS(), i_tp ); 
		}
		catch(TError err) 
		{ 
		    Mess->put(err.cat.c_str(),TMess::Error,"%s",err.mess.c_str()); 
		    Mess->put(nodePath().c_str(),TMess::Error,Mess->I18N("Add parameter <%s> error."),c_el.cfg("SHIFR").getS().c_str());
		}
		c_el.cfg("SHIFR").setS("");
    	    }	    
	}catch(TError err) 
	{ 
	    Mess->put(err.cat.c_str(),TMess::Error,"%s",err.mess.c_str()); 
	    Mess->put(nodePath().c_str(),TMess::Error,Mess->I18N("Search and create new parameters error."));
	}
    }
    
    //Load present parameters
    vector<string> prm_ls;
    list(prm_ls);
    for( int i_p = 0; i_p < prm_ls.size(); i_p++ )
	at(prm_ls[i_p]).at().load( );	
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

void TController::add( const string &name, unsigned type )
{
    if( chldPresent(m_prm,name) ) return;
    chldAdd(m_prm,ParamAttach( name, type ));
}

TParamContr *TController::ParamAttach( const string &name, int type)
{
    return new TParamContr(name, &owner().tpPrmAt(type));
}

//================== Controll functions ========================
void TController::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    vector<string> c_list;

    if( cmd==TCntrNode::Info )
    {
	TCntrNode::cntrCmd_(a_path,opt,cmd);
    
    	ctrMkNode("oscada_cntr",opt,-1,a_path.c_str(),"/",Mess->I18Ns("Controller: ")+name());
	ctrMkNode("area",opt,-1,a_path.c_str(),"/cntr",Mess->I18N("Controller"));
	ctrMkNode("area",opt,-1,a_path.c_str(),"/cntr/st",Mess->I18N("State"));
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/cntr/st/en_st",Mess->I18N("Enable"),0664,0,0,1,"tp","bool");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/cntr/st/run_st",Mess->I18N("Run"),0664,0,0,1,"tp","bool");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/cntr/st/bd",Mess->I18N("Controller DB (module.db)"),0660,0,0,1,"tp","str");
	ctrMkNode("area",opt,-1,a_path.c_str(),"/cntr/cfg",Mess->I18N("Config"));
	ctrMkNode("comm",opt,-1,a_path.c_str(),"/cntr/cfg/load",Mess->I18N("Load"),0550);
	ctrMkNode("comm",opt,-1,a_path.c_str(),"/cntr/cfg/save",Mess->I18N("Save"),0550);
	cntrMake(opt,a_path.c_str(),"/cntr/cfg",0);
    	if( owner().tpPrmSize() )// && enableStat() )
	{
     	     ctrMkNode("area",opt,-1,a_path.c_str(),"/prm",Mess->I18N("Parameters"));
	     ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/t_prm",Mess->I18N("To add parameters"),0660,0,0,3,"tp","str","dest","select","select","/prm/t_lst");
	     ctrMkNode("list",opt,-1,a_path.c_str(),"/prm/prm",Mess->I18N("Parameters"),0660,0,0,4,"tp","br","idm","1","s_com","add,del","br_pref","prm_");
	     ctrMkNode("comm",opt,-1,a_path.c_str(),"/prm/load",Mess->I18N("Load"),0550);
	     ctrMkNode("comm",opt,-1,a_path.c_str(),"/prm/save",Mess->I18N("Save"),0550);
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
                ctrSetS( opt, at(c_list[i_a]).at().name(), c_list[i_a].c_str() );
	}
	else if( a_path == "/prm/t_lst" )
	{
	    opt->childClean();
	    for( unsigned i_a=0; i_a < owner().tpPrmSize(); i_a++ )
		ctrSetS( opt, owner().tpPrmAt(i_a).lName(), owner().tpPrmAt(i_a).name().c_str() );
	}
	else if( a_path == "/cntr/st/bd" )	ctrSetS( opt, m_bd );
    	else if( a_path == "/cntr/st/en_st" )	ctrSetB( opt, en_st );
	else if( a_path == "/cntr/st/run_st" )	ctrSetB( opt, run_st );
	else if( a_path.substr(0,9) == "/cntr/cfg" )	TConfig::cntrCmd(TSYS::pathLev(a_path,2), opt, TCntrNode::Get );
	else TCntrNode::cntrCmd_(a_path,opt,cmd);
    }
    else if( cmd==TCntrNode::Set )
    {
	if( a_path == "/prm/t_prm" )	m_add_type = owner().tpPrmToId(ctrGetS( opt ));
	else if( a_path == "/prm/prm" )
	{
	    if( opt->name() == "add" )
	    {
		add(opt->attr("id"),m_add_type);
		at(opt->attr("id")).at().name(opt->text());
	    }
	    else if( opt->name() == "del" )	del(opt->attr("id"),true);
	}
	else if( a_path == "/prm/load" )	LoadParmCfg();
	else if( a_path == "/prm/save" )	SaveParmCfg();
	else if( a_path == "/cntr/cfg/load" )	load();
	else if( a_path == "/cntr/cfg/save" )	save();
	else if( a_path.substr(0,9) == "/cntr/cfg" )TConfig::cntrCmd(TSYS::pathLev(a_path,2), opt, TCntrNode::Set );
	else if( a_path == "/cntr/st/en_st" )	{ if( ctrGetB( opt ) ) enable(); else disable(); }
	else if( a_path == "/cntr/st/run_st" )	{ if( ctrGetB( opt ) ) start();  else stop(); }
	else if( a_path == "/cntr/st/bd" )      m_bd = ctrGetS(opt);	
	else TCntrNode::cntrCmd_(a_path,opt,cmd);
    }
}
