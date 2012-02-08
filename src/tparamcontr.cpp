
//OpenSCADA system file: tparamcontr.cpp
/***************************************************************************
 *   Copyright (C) 2003-2010 by Roman Savochenko                           *
 *   rom_as@oscada.org, rom_as@fromru.com                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
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
#include "ttiparam.h"
#include "tparamcontr.h"

using namespace OSCADA;

//*************************************************
//* TParamContr                                   *
//*************************************************
TParamContr::TParamContr( const string &name, TTipParam *tpprm ) : TConfig(tpprm), m_en(false), tipparm(tpprm)
{
    cfg("SHIFR") = mId = name;	//!! For prevent ID location change on parameter type change
    setName(name);
}

TParamContr::~TParamContr( )
{
    nodeDelAll();
}

TCntrNode &TParamContr::operator=( TCntrNode &node )
{
    TParamContr *src_n = dynamic_cast<TParamContr*>(&node);
    if(!src_n) return *this;

    //> Check for parameter type and change it if different and alow
    if(type().name != src_n->type().name && owner().owner().tpPrmToId(src_n->type().name) >= 0)
    {
	if(enableStat()) disable();
	setType(src_n->type().name);
    }

    //> Configuration copy
    exclCopy(*src_n, "ID;");

    //> Enable new parameter
    if(src_n->enableStat() && toEnable() && !enableStat()) enable();

    return *this;
}

TController &TParamContr::owner( )	{ return *(TController*)nodePrev(); }

string TParamContr::name( )	{ string nm = cfg("NAME").getS(); return nm.size() ? nm : id(); }

void TParamContr::setName( const string &inm )	{ cfg("NAME").setS(inm); }

string TParamContr::descr( )	{ return cfg("DESCR").getS(); }

void TParamContr::setDescr( const string &idsc ){ cfg("DESCR").setS(idsc); }

void TParamContr::postEnable(int flag)
{
    TValue::postEnable(flag);

    if(!vlCfg())  setVlCfg(this);
    if(!vlElemPresent(&SYS->daq().at().errE()))
	vlElemAtt(&SYS->daq().at().errE());
}

void TParamContr::preDisable(int flag)
{
    //> Delete or stop archives
    vector<string> a_ls;
    vlList(a_ls);

    for(unsigned i_a = 0; i_a < a_ls.size(); i_a++)
	if(!vlAt(a_ls[i_a]).at().arch().freeStat())
	{
	    string arh_id = vlAt(a_ls[i_a]).at().arch().at().id();
	    if(flag) SYS->archive().at().valDel(arh_id,true);
	    else SYS->archive().at().valAt(arh_id).at().stop();
	}

    if(enableStat())	disable();
}

void TParamContr::postDisable(int flag)
{
    if(flag)
    {
	//> Delete parameter from DB
	try
	{
	    SYS->db().at().dataDel(owner().DB()+"."+owner().cfg(type().db).getS(),
				   owner().owner().nodePath()+owner().cfg(type().db).getS(),*this,true);
	}catch(TError err) { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
    }
}

void TParamContr::load_( )
{
    if(!SYS->chkSelDB(owner().DB())) return;

    cfgViewAll(true);
    SYS->db().at().dataGet(owner().DB()+"."+owner().cfg(type().db).getS(),
			   owner().owner().nodePath()+owner().cfg(type().db).getS(),*this);
}

void TParamContr::save_( )
{
    SYS->db().at().dataSet( owner().DB()+"."+owner().cfg(type().db).getS(),
			    owner().owner().nodePath()+owner().cfg(type().db).getS(),*this );

    //> Save archives
    vector<string> a_ls;
    vlList(a_ls);
    for(unsigned i_a = 0; i_a < a_ls.size(); i_a++)
	if(!vlAt(a_ls[i_a]).at().arch().freeStat())
	    vlAt(a_ls[i_a]).at().arch().at().save();
}

bool TParamContr::cfgChange( TCfg &cfg )	{ modif( ); return true; }

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
    if( val.name() == "err" )
    {
	if( !enableStat() ) val.setS(_("1:Parameter is disabled."),0,true);
	else if( !owner().startStat( ) ) val.setS(_("2:Controller is stopped."),0,true);
	else val.setS("0",0,true);
    }
}

void TParamContr::setType( const string &tpId )
{
    if(enableStat() || tpId == type().name || !owner().owner().tpPrmPresent(tpId))	return;

    setNodeMode(TCntrNode::Disable);

    try
    {
	//> Wait for disconnect other
	while(nodeUse(true) > 1) TSYS::sysSleep(1e-3);
	//> Remove from DB
	postDisable(true);

	//> Create temporary structure
	TConfig tCfg(&type());
	tCfg = *(TConfig*)this;

	//> Set new config structure
	tipparm = &owner().owner().tpPrmAt(owner().owner().tpPrmToId(tpId));
	setElem(tipparm);

	//> Restore configurations
	*(TConfig*)this = tCfg;
    }catch(...) { }

    setNodeMode(TCntrNode::Enable);

    setVlCfg(this);

    modif();
}

TVariant TParamContr::objFuncCall( const string &iid, vector<TVariant> &prms, const string &user )
{
    //> Configuration functions call
    TVariant cfRez = objFunc(iid, prms, user);
    if(!cfRez.isNull()) return cfRez;

    return TValue::objFuncCall(iid, prms, user);
}

void TParamContr::cntrCmdProc( XMLNode *opt )
{
    string a_path = opt->attr("path");

    //> Service commands process
    if(a_path.substr(0,6) == "/serv/")	{ TValue::cntrCmdProc(opt); return; }

    //> Get page info
    if(opt->name() == "info")
    {
	TValue::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Parameter: ")+name(),RWRWR_,"root",SDAQ_ID);
	if(ctrMkNode("area",opt,0,"/prm",_("Parameter")))
	{
	    if(ctrMkNode("area",opt,-1,"/prm/st",_("State")))
	    {
		if(!enableStat() && owner().owner().tpPrmSize() > 1)
		    ctrMkNode("fld",opt,-1,"/prm/st/type",_("Type"),RWRWR_,"root",SDAQ_ID,4,"tp","str","dest","select","select","/prm/tpLst",
			"help",_("Change type lead to data lost for specific configurations."));
		else ctrMkNode("fld",opt,-1,"/prm/st/type",_("Type"),R_R_R_,"root",SDAQ_ID,1,"tp","str");
		if(owner().enableStat())
		    ctrMkNode("fld",opt,-1,"/prm/st/en",_("Enable"),RWRWR_,"root",SDAQ_ID,1,"tp","bool");
	    }
	    if(ctrMkNode("area",opt,-1,"/prm/cfg",_("Configuration")))
		TConfig::cntrCmdMake(opt,"/prm/cfg",0,"root",SDAQ_ID,RWRWR_);
	}
        return;
    }
    //> Process command to page
    if(a_path == "/prm/st/type")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(type().name);
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	setType(opt->text());
    }
    else if(a_path == "/prm/st/en")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(enableStat()?"1":"0");
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))
	{
	    if(!owner().enableStat())	throw TError(nodePath().c_str(),_("Controller is not started!"));
	    else atoi(opt->text().c_str())?enable():disable();
	}
    }
    else if(a_path.substr(0,8) == "/prm/cfg") TConfig::cntrCmdProc(opt,TSYS::pathLev(a_path,2),"root",SDAQ_ID,RWRWR_);
    else if(a_path == "/prm/tmplList" && ctrChkNode(opt))
    {
        vector<string> lls, ls;
	SYS->daq().at().tmplLibList(lls);
	for(unsigned i_l = 0; i_l < lls.size(); i_l++)
	{
	    SYS->daq().at().tmplLibAt(lls[i_l]).at().list(ls);
	    for(unsigned i_t = 0; i_t < ls.size(); i_t++)
		opt->childAdd("el")->setText(lls[i_l]+"."+ls[i_t]);
	}
    }
    else if(a_path == "/prm/tpLst" && ctrChkNode(opt))
	for(unsigned i_tp = 0; i_tp < owner().owner().tpPrmSize(); i_tp++)
	    opt->childAdd("el")->setAttr("id",owner().owner().tpPrmAt(i_tp).name)->setText(owner().owner().tpPrmAt(i_tp).descr);
    else TValue::cntrCmdProc(opt);
}
