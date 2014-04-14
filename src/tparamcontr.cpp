
//OpenSCADA system file: tparamcontr.cpp
/***************************************************************************
 *   Copyright (C) 2003-2013 by Roman Savochenko                           *
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
TParamContr::TParamContr( const string &name, TTipParam *tpprm ) : TConfig(tpprm), mPrm(-1), mEn(false), tipparm(tpprm)
{
    cfg("SHIFR") = mId = name;	//!! For prevent ID location change on parameter type change

    setName(name);
    if(type().isPrmCntr) mPrm = grpAdd("prm_");
    type().create(this);

    if(mess_lev() == TMess::Debug) SYS->cntrIter("DAQParamCntr",1);
}

TParamContr::~TParamContr( )
{
    type().destroy(this);
    nodeDelAll();

    if(mess_lev() == TMess::Debug) SYS->cntrIter("DAQParamCntr",-1);
}

string TParamContr::objName( )	{ return TValue::objName()+":TParamContr"; }

string TParamContr::DAQPath( )
{
    string oPath = ownerPath();
    return owner().DAQPath()+(oPath.size()?".":"")+oPath+"."+id();
}

string TParamContr::ownerPath( bool inclSelf )
{
    string rez;

    if(inclSelf) rez = nodeName();
    for(TCntrNode *own = nodePrev(); dynamic_cast<TParamContr*>(own); own = own->nodePrev())
	rez = rez.empty() ? string(own->nodeName()) : (string(own->nodeName())+"."+rez);

    return rez;
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
    exclCopy(*src_n, "SHIFR;");

    //> Enable new parameter
    if(src_n->enableStat() && toEnable() && !enableStat())
    {
	enable();

	//> Archives creation and copy
        vector<string> a_ls;
	vlList(a_ls);
	for(unsigned i_a = 0; i_a < a_ls.size(); i_a++)
	{
	    if(!src_n->vlPresent(a_ls[i_a]) || src_n->vlAt(a_ls[i_a]).at().arch().freeStat()) continue;

	    vlAt(a_ls[i_a]).at().setArch();
	    (TCntrNode&)vlAt(a_ls[i_a]).at().arch().at() = (TCntrNode&)src_n->vlAt(a_ls[i_a]).at().arch().at();
	}

	//> Parameters copy
	if(mPrm >= 0)
	{
	    vector<string> prm_ls;
	    src_n->list(prm_ls);
	    for(unsigned i_p = 0; i_p < prm_ls.size(); i_p++)
	    {
		if(!owner().owner().tpPrmPresent(src_n->at(prm_ls[i_p]).at().type().name)) continue;
		if(!present(prm_ls[i_p])) add(prm_ls[i_p], owner().owner().tpPrmToId(src_n->at(prm_ls[i_p]).at().type().name));
		(TCntrNode&)at(prm_ls[i_p]).at() = (TCntrNode&)src_n->at(prm_ls[i_p]).at();
		//if(toEnable() && !enableStat()) enable();
	    }
	}
    }

    return *this;
}

TController &TParamContr::owner( )
{
    TCntrNode *own = nodePrev();
    while(!dynamic_cast<TController*>(own)) own = own->nodePrev();

    return *(TController*)own;
}

string TParamContr::name( )			{ string nm = cfg("NAME").getS(); return nm.size() ? nm : id(); }

void TParamContr::setName( const string &inm )	{ cfg("NAME").setS(inm); }

string TParamContr::descr( )			{ return cfg("DESCR").getS(); }

bool TParamContr::dataActive( )			{ return owner().startStat(); }

void TParamContr::setDescr( const string &idsc ){ cfg("DESCR").setS(idsc); }

void TParamContr::list( vector<string> &list )
{
    if(mPrm < 0) return;
    chldList(mPrm,list);
}

bool TParamContr::present( const string &name )
{
    if(mPrm < 0) return false;
    return chldPresent(mPrm, name);
}

void TParamContr::add( const string &name, unsigned type )
{
    if(mPrm < 0) return;
    chldAdd(mPrm, owner().ParamAttach(name,type));
}

void TParamContr::del( const string &name, bool full )
{
    if(mPrm < 0) return;
    chldDel(mPrm,name,-1,full);
}

AutoHD<TParamContr> TParamContr::at( const string &name, const string &who )
{
    if(mPrm < 0) return AutoHD<TParamContr>();
    return chldAt(mPrm, name);
}

void TParamContr::LoadParmCfg( )
{
    if(mPrm < 0) return;

    map<string, bool>	itReg;

    //> Search and create new parameters
    for(unsigned i_tp = 0; i_tp < owner().owner().tpPrmSize(); i_tp++)
    {
	if(owner().owner().tpPrmAt(i_tp).DB(&owner()).empty()) continue;
	try
	{
	    TConfig c_el(&owner().owner().tpPrmAt(i_tp));
	    c_el.cfgViewAll(false);
	    c_el.cfg("OWNER").setS(ownerPath(true), TCfg::ForceUse);

	    //>>> Search new into DB and Config-file
	    for(int fld_cnt = 0; SYS->db().at().dataSeek(owner().DB()+"."+owner().owner().tpPrmAt(i_tp).DB(&owner()),
		    owner().owner().nodePath()+owner().owner().tpPrmAt(i_tp).DB(&owner()),fld_cnt++,c_el); )
	    {
		try
		{
		    string shfr = c_el.cfg("SHIFR").getS();
		    if(!present(shfr))	add(shfr, i_tp);
		    itReg[shfr] = true;
		}
		catch(TError err)
		{
		    mess_err(err.cat.c_str(),"%s",err.mess.c_str());
		    mess_err(nodePath().c_str(),_("Add parameter '%s' error."),c_el.cfg("SHIFR").getS().c_str());
		}
	    }
	}
	catch(TError err)
	{
	    mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	    mess_err(nodePath().c_str(),_("Search and create new parameters error."));
	}
    }

    //>>> Check for remove items removed from DB
    if(!SYS->selDB().empty())
    {
	vector<string> it_ls;
	list(it_ls);
	for(unsigned i_it = 0; i_it < it_ls.size(); i_it++)
	    if(itReg.find(it_ls[i_it]) == itReg.end())
		del(it_ls[i_it]);
    }

    //> Force load present parameters
    vector<string> prm_ls;
    list(prm_ls);
    for(unsigned i_p = 0; i_p < prm_ls.size(); i_p++)
    {
	at(prm_ls[i_p]).at().modifG();
	at(prm_ls[i_p]).at().load();
    }
}

void TParamContr::postEnable(int flag)
{
    TValue::postEnable(flag);

    if(!vlCfg())  setVlCfg(this);
    if(!vlElemPresent(&SYS->daq().at().errE()))
	vlElemAtt(&SYS->daq().at().errE());
}

void TParamContr::preDisable( int flag )
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

void TParamContr::postDisable( int flag )
{
    if(flag)
    {
	//> Delete parameter from DB
	try
	{
	    cfg("OWNER") = ownerPath();
	    SYS->db().at().dataDel(owner().DB()+"."+type().DB(&owner()), owner().owner().nodePath()+type().DB(&owner()), *this, true);
	}catch(TError err) { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
    }
}

void TParamContr::load_( )
{
    if(!SYS->chkSelDB(owner().DB())) return;

    cfgViewAll(true);
    cfg("OWNER") = ownerPath();
    SYS->db().at().dataGet(owner().DB()+"."+type().DB(&owner()), owner().owner().nodePath()+type().DB(&owner()), *this);

    LoadParmCfg();
}

void TParamContr::save_( )
{
    cfg("OWNER") = ownerPath();
    SYS->db().at().dataSet(owner().DB()+"."+type().DB(&owner()), owner().owner().nodePath()+type().DB(&owner()), *this);

    //> Save archives
    vector<string> a_ls;
    vlList(a_ls);
    for(unsigned i_a = 0; i_a < a_ls.size(); i_a++)
	if(!vlAt(a_ls[i_a]).at().arch().freeStat())
	    vlAt(a_ls[i_a]).at().arch().at().save();
}

bool TParamContr::cfgChange( TCfg &cfg )
{
    modif();
    return type().cfgChange(this, cfg);
}

TParamContr &TParamContr::operator=( TParamContr & PrmCntr )
{
    TConfig::operator=(PrmCntr);

    return *this;
}

void TParamContr::enable( )
{
    type().enable(this);

    bool enErr = false;
    //> Enable parameters
    vector<string> prm_list;
    list(prm_list);
    for(unsigned i_prm = 0; i_prm < prm_list.size(); i_prm++)
	if(at(prm_list[i_prm]).at().toEnable())
	    try{ at(prm_list[i_prm]).at().enable(); }
	    catch(TError err)
	    {
		mess_warning(err.cat.c_str(), "%s", err.mess.c_str());
		mess_warning(nodePath().c_str(), _("Enable parameter '%s' error."), prm_list[i_prm].c_str());
		enErr = true;
	    }

    mEn = true;

    if(enErr) throw TError(nodePath().c_str(), _("Some parameters enable error."));
}

void TParamContr::disable()
{
    //> Disable parameters
    vector<string> prm_list;
    list(prm_list);
    for(unsigned i_prm = 0; i_prm < prm_list.size(); i_prm++)
	if(at(prm_list[i_prm]).at().enableStat())
	    try{ at(prm_list[i_prm]).at().disable(); }
	    catch(TError err)
	    {
		mess_warning(err.cat.c_str(), "%s", err.mess.c_str());
		mess_warning(nodePath().c_str(), _("Disable parameter '%s' error."), prm_list[i_prm].c_str());
	    }

    type().disable(this);
    mEn = false;
}

void TParamContr::vlGet( TVal &val )
{
    if(val.name() == "err")
    {
	if(!enableStat()) val.setS(_("1:Parameter is disabled."),0,true);
	else if(!owner().startStat()) val.setS(_("2:Controller is stopped."),0,true);
	else val.setS("0",0,true);
    }

    type().vlGet(this, val);
}

void TParamContr::vlSet( TVal &vo, const TVariant &vl, const TVariant &pvl )
{
    type().vlSet(this, vo, vl, pvl);
}

void TParamContr::vlArchMake( TVal &val )
{
    if(!val.arch().freeStat())	val.arch().at().setDB(owner().DB());
    type().vlArchMake(this, val);
}

void TParamContr::setType( const string &tpId )
{
    if(enableStat() || tpId == type().name || !owner().owner().tpPrmPresent(tpId))	return;

    //> Remove all included parameters
    if(mPrm >= 0 && !owner().owner().tpPrmAt(owner().owner().tpPrmToId(tpId)).isPrmCntr)
    {
	vector<string> pls;
	list(pls);
	for(unsigned i_p = 0; i_p < pls.size(); i_p++) del(pls[i_p], true);
	grpDel(mPrm); mPrm = -1;
    }

    type().destroy(this);
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

    if(mPrm < 0 && tipparm->isPrmCntr) mPrm = grpAdd("prm_");

    setNodeMode(TCntrNode::Enable);

    setVlCfg(this);

    type().create(this);

    modif();
}

TVariant TParamContr::objFuncCall( const string &iid, vector<TVariant> &prms, const string &user )
{
    // TCntrNodeObj cntr() - get the controller node
    if(iid == "cntr")	return new TCntrNodeObj(AutoHD<TCntrNode>(&owner()), user);

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
	ctrMkNode("branches",opt,-1,"/br","",R_R_R_);
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
	    {
		TConfig::cntrCmdMake(opt,"/prm/cfg",0,"root",SDAQ_ID,RWRWR_);
		ctrRemoveNode(opt,"/prm/cfg/OWNER");
	    }
	}

	type().cntrCmdProc(this, opt);

	if(mPrm >= 0)
	{
	    ctrMkNode("grp",opt,-1,"/br/prm_",_("Parameter"),RWRWR_,"root",SDAQ_ID,2,"idm",OBJ_NM_SZ,"idSz",OBJ_ID_SZ);
	    if(ctrMkNode("area",opt,-1,"/iPrms",_("Inclusion")))
	    {
		ctrMkNode("fld",opt,-1,"/iPrms/nmb",_("Number"),R_R_R_,"root",SDAQ_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/iPrms/prm",_("Parameters"),RWRWR_,"root",SDAQ_ID,5,"tp","br","idm",OBJ_NM_SZ,"s_com","add,del","br_pref","prm_","idSz",OBJ_ID_SZ);
	    }
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
    else if(mPrm >= 0 && a_path == "/iPrms/nmb" && ctrChkNode(opt))
    {
	vector<string> c_list;
	list(c_list);
	unsigned e_c = 0;
	for(unsigned i_a = 0; i_a < c_list.size(); i_a++)
	    if(at(c_list[i_a]).at().enableStat()) e_c++;
	opt->setText(TSYS::strMess(_("All: %d; Enabled: %d"),c_list.size(),e_c));
    }
    else if((a_path == "/br/prm_" || a_path == "/iPrms/prm"))
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))
	{
	    vector<string> c_list;
	    list(c_list);
	    for(unsigned i_a = 0; i_a < c_list.size(); i_a++)
	        opt->childAdd("el")->setAttr("id",c_list[i_a])->setText(at(c_list[i_a]).at().name());
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SDAQ_ID,SEC_WR))
	{
	    string vid = TSYS::strEncode(opt->attr("id"),TSYS::oscdID);
	    add(vid);
	    at(vid).at().setName(opt->text());
	}
	if(ctrChkNode(opt,"del",RWRWR_,"root",SDAQ_ID,SEC_WR))	del(opt->attr("id"),true);
    }
    else if(type().cntrCmdProc(this, opt)) /* Process OK */;
    else if(a_path.compare(0,8,"/prm/cfg") == 0) TConfig::cntrCmdProc(opt,TSYS::pathLev(a_path,2),"root",SDAQ_ID,RWRWR_);
    else if(a_path == "/prm/tmplList" && ctrChkNode(opt))
    {
	opt->childAdd("el")->setText("");
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
