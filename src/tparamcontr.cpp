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

TParamContr::TParamContr( const string &name, TTipParam *tpprm, TController *contr ) : 
    m_owner(contr), TConfig(tpprm), TValue(this), tipparm(tpprm), m_export(false), 
    m_name(cfg("SHIFR").getS()), m_lname(cfg("NAME").getS()), m_aexport(cfg("EXPORT").getB())
{
    m_name = name;
}

TParamContr::~TParamContr( )
{
}

void TParamContr::load( )
{
    TBDS &bds  = owner().owner().owner().owner().BD();
    TBDS::SName nm_bd( owner().BD().tp, owner().BD().bd, owner().cfg(type().BD()).getS() );
    bds.open(nm_bd).at().fieldGet(*this);
    bds.close(nm_bd);
}

void TParamContr::save( )
{
    TBDS &bds  = owner().owner().owner().owner().BD();
    TBDS::SName nm_bd( owner().BD().tp, owner().BD().bd, owner().cfg(type().BD()).getS() );
    bds.open(nm_bd,true).at().fieldSet(*this);
    bds.close(nm_bd);
}

TParamContr & TParamContr::operator=( TParamContr & PrmCntr )
{
    TConfig::operator=(PrmCntr);

    return(*this);
}

void TParamContr::enable()
{
    vector<string> list;
    vlList(list);
    for(unsigned i_val = 0; i_val < list.size(); i_val++)
	vlAt(list[i_val]).at().valid(true);
}

void TParamContr::disable()
{
    vector<string> list;
    vlList(list);
    for(unsigned i_val = 0; i_val < list.size(); i_val++)
	vlAt(list[i_val]).at().valid(false);
}

void TParamContr::exportPrm( )
{
    TKernel &kern = owner().owner().owner().owner();    
    kern.Param().add( TControllerS::SName( owner().owner().modName(), owner().name()), name() );
    m_export = true;
}

void TParamContr::unExportPrm( )
{
    TKernel &kern = owner().owner().owner().owner();
    kern.Param().del( TControllerS::SName( owner().owner().modName(), owner().name()), name());
    m_export = false;
}

//================== Controll functions ========================
void TParamContr::ctrStat_( XMLNode *inf )
{
    char *i_cntr = 
    	"<oscada_cntr>"
	" <area id='prm'>"
	"  <area id='a_st'>"
	"   <fld id='type' acs='0444' tp='str'/>"
	"   <fld id='exp_st' acs='0664' tp='bool'/>"
	"  </area>"
	"  <area id='cfg'>"
	"   <comm id='load' acs='0550'/>"
	"   <comm id='save' acs='0550'/>"    
	"  </area>"    
	"  <area id='val'>"
	"  </area>"    
	" </area>"
	"</oscada_cntr>";
    char buf[STR_BUF_LEN];
    char *dscr="dscr";
    
    inf->load( i_cntr );
    snprintf(buf,sizeof(buf),Mess->I18N("Parameter: %s"),name().c_str());
    inf->text(buf);
    XMLNode *t_cntr = inf->childGet(0);
    t_cntr->attr(dscr,Mess->I18N("Parameter"));
    t_cntr = t_cntr->childGet(0);    
    t_cntr->attr(dscr,Mess->I18N("State"));    
    t_cntr->childGet(0)->attr(dscr,Mess->I18N("Type"));
    t_cntr->childGet(1)->attr(dscr,Mess->I18N("In generic list"));
    t_cntr = inf->childGet(0)->childGet(1);    
    t_cntr->attr(dscr,Mess->I18N("Config"));
    t_cntr->childGet(0)->attr(dscr,Mess->I18N("Load from BD"));
    t_cntr->childGet(1)->attr(dscr,Mess->I18N("Save to BD"));
    t_cntr = inf->childGet(0)->childGet(2);    
    t_cntr->attr(dscr,Mess->I18N("Value atributes"));   
    
    TConfig::cntrMake("/prm/cfg",inf,0);
    TValue::cntrMake("/prm/val",inf,-1);
}

void TParamContr::ctrDinGet_( const string &a_path, XMLNode *opt )
{    
    if( a_path == "/prm/a_st/type" )        ctrSetS( opt, type().lName() );
    else if( a_path == "/prm/a_st/exp_st" ) ctrSetB( opt, m_export );
    else if( a_path.substr(0,8) == "/prm/cfg" ) TConfig::cntrCmd(pathLev(a_path,2), opt, TCntrNode::Get);
    else if( a_path.substr(0,8) == "/prm/val" ) TValue::cntrCmd(pathLev(a_path,2), opt, TCntrNode::Get);
    else throw TError("(%s) Branch %s error!",o_name,a_path.c_str());
}

void TParamContr::ctrDinSet_( const string &a_path, XMLNode *opt )
{
    if( a_path == "/prm/a_st/exp_st" )
    {
	if( ctrGetB( opt ) ) exportPrm();
	else                 unExportPrm();
    }
    else if( a_path == "/prm/cfg/load" )        load();
    else if( a_path == "/prm/cfg/save" )        save();    
    else if( a_path.substr(0,8) == "/prm/cfg" ) TConfig::cntrCmd(pathLev(a_path,2), opt, TCntrNode::Set);
    else if( a_path.substr(0,8) == "/prm/val" ) TValue::cntrCmd(pathLev(a_path,2), opt, TCntrNode::Set);
    else throw TError("(%s) Branch %s error!",o_name,a_path.c_str());
}
