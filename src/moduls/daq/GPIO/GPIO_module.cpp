
//OpenSCADA system module DAQ.GPIO file: GPIO_module.cpp
/***************************************************************************
 *   Copyright (C) 2016-2018 by Roman Savochenko, <rom_as@oscada.org>      *
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

#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#include <terror.h>
#include <tsys.h>
#include <tmess.h>
#include <ttypeparam.h>
#include <tdaqs.h>

#include "da_BCM2835.h"
#include "da_SUNXI.h"
#include "GPIO_module.h"

//*************************************************
//* Module info!                                  *
#define MOD_ID		"GPIO"
#define MOD_NAME	"GPIO"
#define MOD_TYPE	SDAQ_ID
#define VER_TYPE	SDAQ_VER
#define MOD_VER		"2.1.0"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("Broadcom BCM 2835 GPIO, mostly for and used in Raspberry Pi. ...")
#define LICENSE		"GPL2"
//*************************************************

ModGPIO::TTpContr *ModGPIO::mod;	//Pointer for direct access to the module

extern "C"
{
    TModule::SAt module( int nMod ) {
	if(nMod == 0)	return TModule::SAt(MOD_ID, MOD_TYPE, VER_TYPE);
	return TModule::SAt("");
    }

    TModule *attach( const TModule::SAt &AtMod, const string &source ) {
	if(AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE))
	    return new ModGPIO::TTpContr(source);
	return NULL;
    }
}

using namespace ModGPIO;

//*************************************************
//* TTpContr                                      *
//*************************************************
TTpContr::TTpContr( string name ) : TTypeDAQ(MOD_ID)
{
    mod = this;

    modInfoMainSet(MOD_NAME, MOD_TYPE, MOD_VER, AUTHORS, DESCRIPTION, LICENSE, name);
}

TTpContr::~TTpContr( )
{

}

void TTpContr::postEnable( int flag )
{
    TTypeDAQ::postEnable(flag);

    //Parameter types appending
    tpParmAdd(new BCM2835());
    tpParmAdd(new SUNXI());
}

TController *TTpContr::ContrAttach( const string &name, const string &daq_db )
{
    return new TMdContr(name, daq_db, this);
}

//*************************************************
//* TMdContr                                      *
//*************************************************
TMdContr::TMdContr( string name_c, const string &daq_db, ::TElem *cfgelem ) : TController(name_c, daq_db, cfgelem)
{

}

TMdContr::~TMdContr( )
{
    if(startStat()) stop();
}

TParamContr *TMdContr::ParamAttach( const string &name, int type )
{
    return new TMdPrm(name, &owner().tpPrmAt(type));
}

void TMdContr::enable_( )
{

}

void TMdContr::disable_( )
{

}

void TMdContr::start_( )
{

}

void TMdContr::stop_( )
{

}

//*************************************************
//* TMdPrm                                     *
//*************************************************
TMdPrm::TMdPrm( string name, TTypeParam *tp_prm ) : TParamContr(name, tp_prm)
{
    mFnc = grpAdd("fnc_", true);
}

TMdPrm::~TMdPrm( )
{
    disable();
    nodeDelAll();
}

void TMdPrm::postEnable( int flag )
{
    TParamContr::postEnable(flag);

    if(flag&TCntrNode::NodeRestore) return;

    //Attributes populate
    if(!vlElemPresent(&elem())) vlElemAtt(&elem());
}

TMdContr &TMdPrm::owner( ) const	{ return (TMdContr&)TParamContr::owner(); }

void TMdPrm::enable( )
{
    if(enableStat())	return;

    TParamContr::enable();

    //Functions starting
    vector<string> ls;
    fList(ls);
    for(unsigned iL = 0; iL < ls.size(); iL++)
	fAt(ls[iL]).at().setStart(true);
}

void TMdPrm::disable( )
{
    if(!enableStat())	return;

    vector<string> ls;
    fList(ls);
    // Check the functions for busy, before
    for(unsigned iF = 0; iF < ls.size(); iF++)
	if(fAt(ls[iF]).at().nodeUse() > 1)
	    throw TError(nodePath().c_str(), _("Function '%s' is busy by %d connections."), ls[iF].c_str(), fAt(ls[iF]).at().nodeUse()-1);
    //Functions stop and unregister
    for(unsigned iF = 0; iF < ls.size(); iF++) {
	fAt(ls[iF]).at().setStart(false);
	fUnreg(ls[iF]);
    }

    TParamContr::disable();

    //Set EVAL to the parameter attributes
    elem().fldList(ls);
    for(unsigned iEl = 0; iEl < ls.size(); iEl++)
	vlAt(ls[iEl]).at().setS(EVAL_STR, 0, true);
}

string TMdPrm::modPrm( const string &prm, const string &def )
{
    string rez;
    XMLNode prmNd;
    try {
	prmNd.load(cfg("MOD_PRMS").getS());
	string sobj = TSYS::strParse(prm,0,":"), sa = TSYS::strParse(prm,1,":");
	if(!sa.size())	return (rez=prmNd.attr(prm)).empty() ? def : rez;
	//Internal node
	for(unsigned iN = 0; iN < prmNd.childSize(); iN++)
	    if(prmNd.childGet(iN)->name() == sobj)
		return (rez=prmNd.childGet(iN)->attr(sa)).empty() ? def : rez;
    } catch(...) { }

    return def;
}

void TMdPrm::setModPrm( const string &prm, const string &val )
{
    XMLNode prmNd("ModCfg");
    try { prmNd.load(cfg("MOD_PRMS").getS()); } catch(...) { }

    if(modPrm(prm) != val) modif();
    string sobj = TSYS::strParse(prm,0,":"), sa = TSYS::strParse(prm,1,":");
    if(!sa.size()) prmNd.setAttr(prm,val);
    //Internal node
    else {
	unsigned iN;
	for(iN = 0; iN < prmNd.childSize(); iN++)
	    if(prmNd.childGet(iN)->name() == sobj)
	    { prmNd.childGet(iN)->setAttr(sa,val); break; }
	if(iN >= prmNd.childSize())
	    prmNd.childAdd(sobj)->setAttr(sa,val);
    }

    cfg("MOD_PRMS").setS(prmNd.save(XMLNode::BrAllPast));
}

void TMdPrm::vlGet( TVal &vo )
{
    if((!enableStat() || !owner().startStat()) && vo.name() != "err") {
	vo.setR(EVAL_REAL, 0, true);
	return;
    }

    TParamContr::vlGet(vo);
    //type().vlGet(this, vo);
}

void TMdPrm::vlSet( TVal &vo, const TVariant &vl, const TVariant &pvl )
{
    if(!enableStat() || !owner().startStat())	{ vo.setR(EVAL_REAL, 0, true); return; }
    if(vl.isEVal() || vl == pvl) return;

    TParamContr::vlSet(vo, vl, pvl);
    //type().vlSet(this, vo, vl, pvl);
}

void TMdPrm::vlArchMake( TVal &val )
{
    TParamContr::vlArchMake(val);

    if(val.arch().freeStat()) return;
    val.arch().at().setSrcMode(TVArchive::ActiveAttr);
    val.arch().at().setPeriod(1000000);	//One second
    val.arch().at().setHardGrid(true);
    //val.arch().at().setHighResTm(true);
}

void TMdPrm::cntrCmdProc( XMLNode *opt )
{
    string rez;

    //Get the page information
    if(opt->name() == "info") {
	TParamContr::cntrCmdProc(opt);
	ctrMkNode("grp",opt,-1,"/br/fnc_",_("Function"),R_R_R_,"root",SDAQ_ID,1,"idm","1");
	ctrMkNode("list",opt,-1,"/prm/st/func",_("Functions"),R_R_R_,"root",SDAQ_ID,3,"tp","br","idm","1","br_pref","fnc_");
	ctrRemoveNode(opt, "/prm/cfg/MOD_PRMS");
	if(ctrMkNode("area",opt,-1,"/cfg",_("Configuration"))) {
	    vector<string> ls;
	    elem().fldList(ls);
	    for(unsigned iL = 0; iL < ls.size(); iL++) {
		if(ls[iL].compare(0,4,"gpio") != 0) continue;
		string pin = ls[iL].substr(4);
		ctrMkNode("fld", opt, -1, ("/cfg/mode_"+pin).c_str(), TSYS::strMess(_("GPIO '%s' output and reverse"),pin.c_str()).c_str(),
		    enableStat()?R_R_R_:RWRWR_, "root", SDAQ_ID, 4, "tp","dec", "dest","select",
		    "sel_id","0;1;2;3;4", "sel_list",_("Disabled;Input;Input (pull up);Input (pull down);Output"));
		ctrMkNode("fld", opt, -1, ("/cfg/rev_"+pin).c_str(), "", enableStat()?R_R_R_:RWRWR_, "root", SDAQ_ID, 1, "tp","bool");
	    }
	}
	
	return;
    }

    //Process commands to the page
    string a_path = opt->attr("path");
    if((a_path == "/br/fnc_" || a_path == "/prm/st/func") && ctrChkNode(opt)) {
	vector<string> ls;
	fList(ls);
	for(unsigned iF = 0; iF < ls.size(); iF++)
	    opt->childAdd("el")->setAttr("id",ls[iF])->setText(fAt(ls[iF]).at().name());
    }
    else if(a_path.compare(0,10,"/cfg/mode_") == 0) {
	string pin = a_path.substr(10);
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(modPrm("GPIOmode"+pin,"0"));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	setModPrm("GPIOmode"+pin, opt->text());
    }
    else if(a_path.compare(0,9,"/cfg/rev_") == 0) {
	string pin = a_path.substr(9);
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(modPrm("GPIOrev"+pin));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	setModPrm("GPIOrev"+pin, opt->text());
    }
    else TParamContr::cntrCmdProc(opt);
}
