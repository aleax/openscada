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
#include "tmess.h"
#include "tcontrollers.h"
#include "tcontroller.h"
#include "ttipcontroller.h"
#include "tparams.h"
#include "ttiparam.h"
#include "tparamcontr.h"

const char *TParamContr::o_name = "TParamContr";

TParamContr::TParamContr( const string &name, TTipParam *tpprm ) : 
    TConfig(tpprm), TValue(this), tipparm(tpprm), m_en(false), m_export(false), m_sw_atr(false),
    m_name(cfg("SHIFR").getSd()), m_lname(cfg("NAME").getSd()), 
    m_aexport(cfg("EXPORT").getBd()), m_aen(cfg("EN").getBd())
{
    m_name = name;
}

TParamContr::~TParamContr( )
{
}

void TParamContr::load( )
{
    AutoHD<TBDS> bds  = owner().owner().owner().owner().db();
    TBDS::SName nm_bd( owner().BD().tp.c_str(), owner().BD().bd.c_str(), owner().cfg(type().BD()).getS().c_str() );
    bds.at().open(nm_bd).at().fieldGet(*this);
    bds.at().close(nm_bd);
}

void TParamContr::save( )
{
    AutoHD<TBDS> bds  = owner().owner().owner().owner().db();
    TBDS::SName nm_bd( owner().BD().tp.c_str(), owner().BD().bd.c_str(), owner().cfg(type().BD()).getS().c_str() );
    bds.at().open(nm_bd,true).at().fieldSet(*this);
    bds.at().close(nm_bd);
}

TParamContr & TParamContr::operator=( TParamContr & PrmCntr )
{
    TConfig::operator=(PrmCntr);

    return(*this);
}

void TParamContr::enable()
{
    vector<string> list;
    
    if( m_en )	return;    
    vlList(list);
    for(unsigned i_val = 0; i_val < list.size(); i_val++)
	vlAt(list[i_val]).at().valid(true);    
    m_en = true;	
}

void TParamContr::disable()
{
    vector<string> list;
    
    if( !m_en )  return;    
    vlList(list);
    for(unsigned i_val = 0; i_val < list.size(); i_val++)
	vlAt(list[i_val]).at().valid(false);
    m_en = false;
}

void TParamContr::exportPrm( )
{    
    if( m_export )	return;
    
    owner().owner().owner().owner().param().at().add( TControllerS::SName( owner().owner().modId().c_str(), owner().name().c_str()), name() );
    m_export = true;
}

void TParamContr::unExportPrm( )
{
    if( !m_export )      return;
    
    owner().owner().owner().owner().param().at().del( TControllerS::SName( owner().owner().modId().c_str(), owner().name().c_str()), name());
    m_export = false;
}

//================== Controll functions ========================
void TParamContr::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    if( cmd==TCntrNode::Info )
    {
	ctrMkNode("oscada_cntr",opt,a_path.c_str(),"/",Mess->I18Ns("Parameter: ")+name());
	ctrMkNode("area",opt,a_path.c_str(),"/prm",Mess->I18N("Parameter"));
	ctrMkNode("area",opt,a_path.c_str(),"/prm/st",Mess->I18N("State"));
	ctrMkNode("fld",opt,a_path.c_str(),"/prm/st/type",Mess->I18N("Type"),0444,0,0,"str");
	ctrMkNode("fld",opt,a_path.c_str(),"/prm/st/exp_st",Mess->I18N("In generic list"),0664,0,0,"bool");
	if( owner().startStat() ) 
	    ctrMkNode("fld",opt,a_path.c_str(),"/prm/st/en",Mess->I18N("Enable"),0664,0,0,"bool");
	if( enableStat() )	    
	    ctrMkNode("fld",opt,a_path.c_str(),"/prm/st/atr_sw",Mess->I18N("Show atributes"),0664,0,0,"bool");
	ctrMkNode("area",opt,a_path.c_str(),"/prm/cfg",Mess->I18N("Config"));
	ctrMkNode("comm",opt,a_path.c_str(),"/prm/cfg/load",Mess->I18N("Load from BD"),0550);
	ctrMkNode("comm",opt,a_path.c_str(),"/prm/cfg/save",Mess->I18N("Save to BD"),0550);
	TConfig::cntrMake(opt,a_path.c_str(),"/prm/cfg",0);
	if( enableStat() && m_sw_atr )    
	{    
	    ctrMkNode("area",opt,a_path.c_str(),"/prm/val",Mess->I18N("Value atributes"));
	    TValue::cntrMake(opt,a_path.c_str(),"/prm/val",-1);
	}
    }
    else if( cmd==TCntrNode::Get )
    {
	if( a_path == "/prm/st/type" )        	ctrSetS( opt, type().lName() );
	else if( a_path == "/prm/st/en" ) 	ctrSetB( opt, enableStat() );
	else if( a_path == "/prm/st/exp_st" ) 	ctrSetB( opt, exportStat() );
	else if( a_path == "/prm/st/atr_sw" )		ctrSetB( opt, m_sw_atr );
	else if( a_path.substr(0,8) == "/prm/cfg" ) 	TConfig::cntrCmd(TSYS::pathLev(a_path,2), opt, TCntrNode::Get);
	else if( a_path.substr(0,8) == "/prm/val" ) 	TValue::cntrCmd(TSYS::pathLev(a_path,2), opt, TCntrNode::Get);
	else throw TError(name().c_str(),"Branch <%s> error!",a_path.c_str());		
    }
    else if( cmd==TCntrNode::Set )
    {
	if( a_path == "/prm/st/en" ) 		ctrGetB(opt)?enable():disable();
	else if( a_path == "/prm/st/exp_st" ) 	ctrGetB(opt)?exportPrm():unExportPrm();
	else if( a_path == "/prm/st/atr_sw" ) 	m_sw_atr = ctrGetB(opt);
	else if( a_path == "/prm/cfg/load" ) 	load();
	else if( a_path == "/prm/cfg/save" ) 	save();    
	else if( a_path.substr(0,8) == "/prm/cfg" )	TConfig::cntrCmd(TSYS::pathLev(a_path,2), opt, TCntrNode::Set);
	else if( a_path.substr(0,8) == "/prm/val" )	TValue::cntrCmd(TSYS::pathLev(a_path,2), opt, TCntrNode::Set);
	else throw TError(name().c_str(),"Branch <%s> error!",a_path.c_str());
    }    
}

