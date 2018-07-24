
//OpenSCADA system file: tparamcontr.cpp
/***************************************************************************
 *   Copyright (C) 2003-2018 by Roman Savochenko, <rom_as@oscada.org>      *
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
TParamContr::TParamContr( const string &name, TTipParam *tpprm ) : TConfig(tpprm), mRdPrcTm(0), mEn(false), tpParm(tpprm)
{
    cfg("SHIFR") = mId = name;	//!! For prevent ID location change on the parameter type change
    setName(name);

#if OSC_DEBUG >= 1
    SYS->cntrIter("DAQParamCntr",1);
#endif
}

TParamContr::~TParamContr( )
{
    nodeDelAll();

#if OSC_DEBUG >= 1
    SYS->cntrIter("DAQParamCntr",-1);
#endif

}

string TParamContr::objName( )	{ return TValue::objName() + ":TParamContr"; }

string TParamContr::DAQPath( )	{ return owner().DAQPath()+"."+id(); }

TCntrNode &TParamContr::operator=( const TCntrNode &node )
{
    const TParamContr *src_n = dynamic_cast<const TParamContr*>(&node);
    if(!src_n) return *this;

    //Check for parameter type and change it if different and alow
    if(type().name != src_n->type().name && owner().owner().tpPrmToId(src_n->type().name) >= 0) {
	if(enableStat()) disable();
	setType(src_n->type().name);
    }

    //Configuration copy
    exclCopy(*src_n, "SHIFR;");

    //Enable new parameter
    if(src_n->enableStat() && toEnable() && !enableStat()) {
	enable();

	//Archives creation and copy
	vector<string> a_ls;
	vlList(a_ls);
	for(unsigned iA = 0; iA < a_ls.size(); iA++) {
	    if(!src_n->vlPresent(a_ls[iA]) || src_n->vlAt(a_ls[iA]).at().arch().freeStat()) continue;

	    vlAt(a_ls[iA]).at().setArch();
	    (TCntrNode&)vlAt(a_ls[iA]).at().arch().at() = (TCntrNode&)src_n->vlAt(a_ls[iA]).at().arch().at();
	}
    }

    return *this;
}

TController &TParamContr::owner( ) const	{ return *(TController*)nodePrev(); }

string TParamContr::name( )	{ string nm = cfg("NAME").getS(); return nm.size() ? nm : id(); }

void TParamContr::setName( const string &inm )	{ cfg("NAME").setS(inm); }

string TParamContr::descr( )	{ return cfg("DESCR").getS(); }

bool TParamContr::dataActive( )	{ return owner().startStat(); }

void TParamContr::setDescr( const string &idsc ){ cfg("DESCR").setS(idsc); }

void TParamContr::postEnable( int flag )
{
    TValue::postEnable(flag);

    if(!vlCfg()) setVlCfg(this);
    if(!vlElemPresent(&SYS->daq().at().elErr()))
	vlElemAtt(&SYS->daq().at().elErr());
}

void TParamContr::preDisable(int flag)
{
    //Delete or stop the archives
    vector<string> a_ls;
    vlList(a_ls);
    for(unsigned iA = 0; iA < a_ls.size(); iA++)
	if(!vlAt(a_ls[iA]).at().arch().freeStat()) {
	    string arh_id = vlAt(a_ls[iA]).at().arch().at().id();
	    if((flag>>8) == RM_Full) SYS->archive().at().valDel(arh_id, true);
	    else SYS->archive().at().valAt(arh_id).at().stop();
	}

    if(enableStat())	disable();
}

void TParamContr::postDisable(int flag)
{
    if(flag) {
	//Delete the parameter from DB
	SYS->db().at().dataDel(owner().DB()+"."+owner().cfg(type().db).getS(), owner().owner().nodePath()+owner().cfg(type().db).getS(), *this, true);
    }
}

void TParamContr::load_( TConfig *icfg )
{
    if(!SYS->chkSelDB(owner().DB())) throw TError();

    if(icfg) *(TConfig*)this = *icfg;
    else {
	//cfgViewAll(true);
	SYS->db().at().dataGet(owner().DB()+"."+owner().cfg(type().db).getS(), owner().owner().nodePath()+owner().cfg(type().db).getS(), *this);
    }
}

void TParamContr::save_( )
{
    SYS->db().at().dataSet( owner().DB()+"."+owner().cfg(type().db).getS(),
			    owner().owner().nodePath()+owner().cfg(type().db).getS(),*this );

    //Save archives
    vector<string> a_ls;
    vlList(a_ls);
    for(unsigned iA = 0; iA < a_ls.size(); iA++)
	if(!vlAt(a_ls[iA]).at().arch().freeStat())
	    vlAt(a_ls[iA]).at().arch().at().save();
}

bool TParamContr::cfgChange( TCfg &co, const TVariant &pc )	{ modif(); return true; }

void TParamContr::enable( )
{
    mEn = true;
}

void TParamContr::disable( )
{
    mEn = false;
}

void TParamContr::vlGet( TVal &val )
{
    if(val.name() == "err") {
	if(!enableStat()) val.setS(_("1:Parameter disabled."), 0, true);
	else if(!owner().startStat()) val.setS(_("2:Acquisition stopped."), 0, true);
	else val.setS("0", 0, true);
    }
}

void TParamContr::vlArchMake( TVal &val )
{
    if(!val.arch().freeStat())	val.arch().at().setDB(owner().DB());
}

void TParamContr::setType( const string &tpId )
{
    if(enableStat() || tpId == type().name || !owner().owner().tpPrmPresent(tpId))	return;

    setNodeMode(TCntrNode::Disabled);

    try {
	//Wait for disconnect other
	while(nodeUse(true) > 1) TSYS::sysSleep(1e-3);
	//Remove from DB
	postDisable(true);

	//Create temporary structure
	TConfig tCfg(&type());
	tCfg = *(TConfig*)this;

	//Set new config structure
	tpParm = &owner().owner().tpPrmAt(owner().owner().tpPrmToId(tpId));
	setElem(tpParm);

	//Restore configurations
	*(TConfig*)this = tCfg;
    } catch(...) { }

    setNodeMode(TCntrNode::Enabled);

    setVlCfg(this);

    modif();
}

TVariant TParamContr::objFuncCall( const string &iid, vector<TVariant> &prms, const string &user )
{
    //TCntrNodeObj cntr() - get the controller node
    if(iid == "cntr")	return new TCntrNodeObj(AutoHD<TCntrNode>(&owner()), user);

    //Configuration functions call
    TVariant cfRez = objFunc(iid, prms, user);
    if(!cfRez.isNull()) return cfRez;

    return TValue::objFuncCall(iid, prms, user);
}

void TParamContr::cntrCmdProc( XMLNode *opt )
{
    string a_path = opt->attr("path");

    //Service commands process
    if(a_path.substr(0,6) == "/serv/")	{ TValue::cntrCmdProc(opt); return; }

    //Get page info
    if(opt->name() == "info") {
	TValue::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Parameter: ")+name(),RWRWR_,"root",SDAQ_ID);
	if(ctrMkNode("area",opt,0,"/prm",_("Parameter"))) {
	    if(ctrMkNode("area",opt,-1,"/prm/st",_("State"))) {
		if(!enableStat() && owner().owner().tpPrmSize() > 1)
		    ctrMkNode("fld",opt,-1,"/prm/st/type",_("Type"),RWRWR_,"root",SDAQ_ID,4,"tp","str","dest","select","select","/prm/tpLst",
			"help",_("The type changing leads to lose some data of the specific configurations."));
		else ctrMkNode("fld",opt,-1,"/prm/st/type",_("Type"),R_R_R_,"root",SDAQ_ID,1,"tp","str");
		if(owner().enableStat())
		    ctrMkNode("fld",opt,-1,"/prm/st/en",_("Enabled"),RWRWR_,"root",SDAQ_ID,1,"tp","bool");
	    }
	    if(ctrMkNode("area",opt,-1,"/prm/cfg",_("Configuration")))
		TConfig::cntrCmdMake(opt,"/prm/cfg",0,"root",SDAQ_ID,RWRWR_);
	}
        return;
    }
    //Process command to page
    if(a_path == "/prm/st/type") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(type().name);
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	setType(opt->text());
    }
    else if(a_path == "/prm/st/en") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(enableStat()?"1":"0");
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR)) {
	    if(!owner().enableStat())	throw err_sys(_("Controller is not running!"));
	    else s2i(opt->text()) ? enable() : disable();
	}
    }
    else if(a_path.compare(0,8,"/prm/cfg") == 0) TConfig::cntrCmdProc(opt,TSYS::pathLev(a_path,2),"root",SDAQ_ID,RWRWR_);
    else if(a_path == "/prm/tmplList" && ctrChkNode(opt)) {
	opt->childAdd("el")->setText("");
        vector<string> lls, ls;
	SYS->daq().at().tmplLibList(lls);
	for(unsigned iL = 0; iL < lls.size(); iL++) {
	    SYS->daq().at().tmplLibAt(lls[iL]).at().list(ls);
	    for(unsigned iT = 0; iT < ls.size(); iT++)
		opt->childAdd("el")->setText(lls[iL]+"."+ls[iT]);
	}
    }
    else if(a_path == "/prm/tpLst" && ctrChkNode(opt))
	for(unsigned iTp = 0; iTp < owner().owner().tpPrmSize(); iTp++)
	    opt->childAdd("el")->setAttr("id",owner().owner().tpPrmAt(iTp).name)->setText(owner().owner().tpPrmAt(iTp).descr);
    else TValue::cntrCmdProc(opt);
}
