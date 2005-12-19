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

TParamContr::TParamContr( const string &name, TTipParam *tpprm ) : 
    TConfig(tpprm), tipparm(tpprm), m_en(false), m_export(false),
    m_id(cfg("SHIFR").getSd()), m_name(cfg("NAME").getSd()), m_descr(cfg("DESCR").getSd()), m_aen(cfg("EN").getBd())
{
    m_id = m_name = name;
}

TParamContr::~TParamContr( )
{

}

void TParamContr::postEnable()
{
    TValue::postEnable();
    vlCfg(this);	
    vlAttElem(&SYS->controller().at().errE());
    disable();
}

void TParamContr::preDisable(int flag)
{
    if( enableStat() )	disable();
    vlDetElem(&SYS->controller().at().errE());
}
	
void TParamContr::postDisable(int flag)
{
    if( flag )
    {
	try
	{
    	    TBDS::SName nm_bd( owner().BD().tp.c_str(), owner().BD().bd.c_str(), owner().cfg(type().BD()).getS().c_str() );
	    SYS->db().at().dataDel(nm_bd,owner().owner().nodePath()+nm_bd.tbl,*this);
	}catch(TError err) { Mess->put(nodePath().c_str(),TMess::Error,err.mess.c_str()); }
    }
}

void TParamContr::load( )
{
    TBDS::SName nm_bd( owner().BD().tp.c_str(), owner().BD().bd.c_str(), owner().cfg(type().BD()).getS().c_str() );
    SYS->db().at().dataGet(nm_bd,owner().owner().nodePath()+nm_bd.tbl,*this);
}

void TParamContr::save( )
{
    TBDS::SName nm_bd( owner().BD().tp.c_str(), owner().BD().bd.c_str(), owner().cfg(type().BD()).getS().c_str() );
    SYS->db().at().dataSet(nm_bd,owner().owner().nodePath()+nm_bd.tbl,*this);
}

TParamContr & TParamContr::operator=( TParamContr & PrmCntr )
{
    TConfig::operator=(PrmCntr);

    return *this;
}

void TParamContr::enable()
{
    m_en = true;
}

void TParamContr::disable()
{
    m_en = false;
}

void TParamContr::vlGet( TVal &val )
{
    if(val.name() == "err")
    {
	if( enableStat() )	val.setB(false,NULL,true);
	else	val.setB(true,NULL,true);
    }
    else if(val.name() == "err_mess" )
    {
	if( enableStat() )	val.setS(Mess->I18N("No errors."),NULL,true);
	else	val.setS(Mess->I18N("Parameter had disabled."),NULL,true);
    }
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
	if( owner().startStat() ) 
	    ctrMkNode("fld",opt,a_path.c_str(),"/prm/st/en",Mess->I18N("Enable"),0664,0,0,"bool");
	ctrMkNode("area",opt,a_path.c_str(),"/prm/cfg",Mess->I18N("Config"));
	ctrMkNode("comm",opt,a_path.c_str(),"/prm/cfg/load",Mess->I18N("Load"),0550);
	ctrMkNode("comm",opt,a_path.c_str(),"/prm/cfg/save",Mess->I18N("Save"),0550);
	TConfig::cntrMake(opt,a_path.c_str(),"/prm/cfg",0);
	ctrMkNode("area",opt,a_path.c_str(),"/val",Mess->I18N("Atributes"));
	TValue::cntrMake(opt,a_path.c_str(),"/val",-1);
    }
    else if( cmd==TCntrNode::Get )
    {
	if( a_path == "/prm/st/type" )        	ctrSetS( opt, type().lName() );
	else if( a_path == "/prm/st/en" ) 	ctrSetB( opt, enableStat() );
	else if( a_path.substr(0,8) == "/prm/cfg" ) 	TConfig::cntrCmd(TSYS::pathLev(a_path,2), opt, TCntrNode::Get);
	else if( a_path.substr(0,4) == "/val" ) 	TValue::cntrCmd(TSYS::pathLev(a_path,1), opt, TCntrNode::Get);
	else throw TError(nodePath().c_str(),Mess->I18N("Branch <%s> error!"),a_path.c_str());		
    }
    else if( cmd==TCntrNode::Set )
    {
	if( a_path == "/prm/st/en" ) 		ctrGetB(opt)?enable():disable();
	else if( a_path == "/prm/cfg/load" ) 	load();
	else if( a_path == "/prm/cfg/save" ) 	save();
	else if( a_path.substr(0,8) == "/prm/cfg" )	TConfig::cntrCmd(TSYS::pathLev(a_path,2), opt, TCntrNode::Set);
	else if( a_path.substr(0,4) == "/val" )	TValue::cntrCmd(TSYS::pathLev(a_path,1), opt, TCntrNode::Set);
	else throw TError(nodePath().c_str(),Mess->I18N("Branch <%s> error!"),a_path.c_str());
    }    
}

