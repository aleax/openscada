
//OpenSCADA system file: tparamcontr.cpp
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
 
#include "tbds.h"
#include "tsys.h"
#include "tmess.h"
#include "tdaqs.h"
#include "tcontroller.h"
#include "ttipdaq.h"
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

string TParamContr::name()
{ 
    return (m_name.size())?m_name:m_id;
}

void TParamContr::postEnable()
{
    TValue::postEnable();
    vlCfg(this);	
    vlAttElem(&SYS->daq().at().errE());
    disable();
}

void TParamContr::preDisable(int flag)
{
    if( flag )
    {
	//Delete archives
	vector<string> a_ls;
	vlList(a_ls);
	for(int i_a = 0; i_a < a_ls.size(); i_a++)
	    if( !vlAt(a_ls[i_a]).at().arch().freeStat() )
	    {
		string arh_id = vlAt(a_ls[i_a]).at().arch().at().id();
		SYS->archive().at().valDel(arh_id,true);
	    }
    }    

    if( enableStat() )	disable();
    vlDetElem(&SYS->daq().at().errE());
}
	
void TParamContr::postDisable(int flag)
{
    if( flag )
    {	
	try
	{
	    SYS->db().at().dataDel(owner().genBD()+"."+owner().cfg(type().BD()).getS(),
		    		   owner().owner().nodePath()+owner().cfg(type().BD()).getS(),*this);
	}catch(TError err) { Mess->put(nodePath().c_str(),TMess::Error,err.mess.c_str()); }
    }
}

void TParamContr::load( )
{
    SYS->db().at().dataGet(owner().genBD()+"."+owner().cfg(type().BD()).getS(),
	    		   owner().owner().nodePath()+owner().cfg(type().BD()).getS(),*this);
}

void TParamContr::save( )
{
    SYS->db().at().dataSet(owner().genBD()+"."+owner().cfg(type().BD()).getS(),
	    		   owner().owner().nodePath()+owner().cfg(type().BD()).getS(),*this);
    
    //Save archives
    vector<string> a_ls;
    vlList(a_ls);
    for(int i_a = 0; i_a < a_ls.size(); i_a++)
        if( !vlAt(a_ls[i_a]).at().arch().freeStat() )
            vlAt(a_ls[i_a]).at().arch().at().save();				    
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
    if(val.name() == "err" )
    {
	if( enableStat() ) val.setS("0",0,true);
	else val.setS(Mess->I18N("1:Parameter had disabled."),0,true);
    }
}

//================== Controll functions ========================
void TParamContr::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    if( cmd==TCntrNode::Info )
    {
	TValue::cntrCmd_(a_path,opt,cmd);
    
	ctrMkNode("oscada_cntr",opt,a_path.c_str(),"/",Mess->I18Ns("Parameter: ")+name());
	ctrInsNode("area",0,opt,a_path.c_str(),"/prm",Mess->I18N("Parameter"));
	ctrMkNode("area",opt,a_path.c_str(),"/prm/st",Mess->I18N("State"));
	ctrMkNode("fld",opt,a_path.c_str(),"/prm/st/type",Mess->I18N("Type"),0444,0,0,"str");
	if( owner().startStat() ) 
	    ctrMkNode("fld",opt,a_path.c_str(),"/prm/st/en",Mess->I18N("Enable"),0664,0,0,"bool");
	ctrMkNode("area",opt,a_path.c_str(),"/prm/cfg",Mess->I18N("Config"));
	ctrMkNode("comm",opt,a_path.c_str(),"/prm/cfg/load",Mess->I18N("Load"),0550);
	ctrMkNode("comm",opt,a_path.c_str(),"/prm/cfg/save",Mess->I18N("Save"),0550);
	TConfig::cntrMake(opt,a_path.c_str(),"/prm/cfg",0);
    }
    else if( cmd==TCntrNode::Get )
    {
	if( a_path == "/prm/st/type" )        	ctrSetS( opt, type().lName() );
	else if( a_path == "/prm/st/en" ) 	ctrSetB( opt, enableStat() );
	else if( a_path.substr(0,8) == "/prm/cfg" ) 	TConfig::cntrCmd(TSYS::pathLev(a_path,2), opt, TCntrNode::Get);
	else TValue::cntrCmd_(a_path,opt,cmd);
    }
    else if( cmd==TCntrNode::Set )
    {
	if( a_path == "/prm/st/en" ) 		ctrGetB(opt)?enable():disable();
	else if( a_path == "/prm/cfg/load" ) 	load();
	else if( a_path == "/prm/cfg/save" ) 	save();
	else if( a_path.substr(0,8) == "/prm/cfg" )	TConfig::cntrCmd(TSYS::pathLev(a_path,2), opt, TCntrNode::Set);
	else TValue::cntrCmd_(a_path,opt,cmd);
    }    
}

