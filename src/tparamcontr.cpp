
//OpenSCADA file: tparamcontr.cpp
/***************************************************************************
 *   Copyright (C) 2003-2021 by Roman Savochenko, <roman@oscada.org>       *
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

#include "tsys.h"
#include "tdaqs.h"
#include "tcontroller.h"
#include "ttypedaq.h"
#include "ttypeparam.h"
#include "tparamcontr.h"

using namespace OSCADA;

//*************************************************
//* TParamContr                                   *
//*************************************************
TParamContr::TParamContr( const string &name, TTypeParam *tpprm ) : TConfig(tpprm), mRdPrcTm(0), mPrm(-1), mEn(false), tpParm(tpprm)
{
    cfg("SHIFR") = mId = name;	//!! To prevent the ID location changing on the parameter type change

    setName(name);
    if(type().isPrmCntr) mPrm = grpAdd("prm_");
    //type().create(this);

    if(mess_lev() == TMess::Debug) SYS->cntrIter(objName(), 1);
}

TParamContr::~TParamContr( )
{
    //type().destroy(this);
    nodeDelAll();

    if(mess_lev() == TMess::Debug) SYS->cntrIter(objName(), -1);
}

string TParamContr::objName( )	{ return TValue::objName() + ":TParamContr"; }

string TParamContr::DAQPath( )
{
    string oPath = ownerPath();
    return owner().DAQPath()+(oPath.size()?".":"") + oPath + "." + id();
}

string TParamContr::ownerPath( bool inclSelf )
{
    string rez;

    if(inclSelf) rez = nodeName();
    for(TCntrNode *own = nodePrev(); dynamic_cast<TParamContr*>(own); own = own->nodePrev())
	rez = rez.empty() ? string(own->nodeName()) : (string(own->nodeName())+"."+rez);

    return rez;
}

TCntrNode &TParamContr::operator=( const TCntrNode &node )
{
    const TParamContr *src_n = dynamic_cast<const TParamContr*>(&node);
    if(!src_n) return *this;

    if(enableStat()) disable();

    //Check for parameter type and change it if different and alow
    if(type().name != src_n->type().name && owner().owner().tpPrmToId(src_n->type().name) >= 0)
	setType(src_n->type().name);

    //Configuration copy
    exclCopy(*src_n, "SHIFR;");

    //Enable new parameter
    if(src_n->enableStat() && toEnable() && !enableStat()) {
	enable();

	//Archives creation and copy
	vector<string> aLs;
	vlList(aLs);
	for(unsigned iA = 0; iA < aLs.size(); iA++) {
	    if(!src_n->vlPresent(aLs[iA]) || src_n->vlAt(aLs[iA]).at().arch().freeStat()) continue;

	    try {
		vlAt(aLs[iA]).at().setArch();
		(TCntrNode&)vlAt(aLs[iA]).at().arch().at() = (TCntrNode&)src_n->vlAt(aLs[iA]).at().arch().at();
	    } catch(TError &err) { if(err.cod != TError::Arch_Val_DblVSrc) throw; }
	}

	//Included parameters copy
	if(mPrm >= 0) {
	    vector<string> prmLs;
	    src_n->list(prmLs);
	    for(unsigned iP = 0; iP < prmLs.size(); iP++) {
		if(!owner().owner().tpPrmPresent(src_n->at(prmLs[iP]).at().type().name)) continue;
		if(!present(prmLs[iP])) add(prmLs[iP], owner().owner().tpPrmToId(src_n->at(prmLs[iP]).at().type().name));
		(TCntrNode&)at(prmLs[iP]).at() = (TCntrNode&)src_n->at(prmLs[iP]).at();
		//if(toEnable() && !enableStat()) enable();
	    }
	}
    }

    return *this;
}

TController &TParamContr::owner( ) const
{
    TCntrNode *own = nodePrev();
    while(!dynamic_cast<TController*>(own)) own = own->nodePrev();

    return *(TController*)own;
}

string TParamContr::name( )			{ string nm = cfg("NAME").getS(); return nm.size() ? nm : id(); }

void TParamContr::setName( const string &inm )	{ cfg("NAME").setS(inm); }

string TParamContr::descr( )			{ return cfg("DESCR").getS(); }

int64_t TParamContr::timeStamp( )
{
    int64_t mTimeStamp = cfg("TIMESTAMP").getI();

    vector<string> ls;
    list(ls);
    for(unsigned iL = 0; iL < ls.size(); ++iL)
	mTimeStamp = vmax(mTimeStamp, at(ls[iL]).at().timeStamp());

    return mTimeStamp;
}

bool TParamContr::dataActive( )			{ return owner().startStat(); }

void TParamContr::setDescr( const string &idsc ){ cfg("DESCR").setS(idsc); }

void TParamContr::list( vector<string> &list ) const
{
    if(mPrm < 0) return;
    chldList(mPrm, list);
}

bool TParamContr::present( const string &iid ) const
{
    if(mPrm < 0) return false;
    return chldPresent(mPrm, iid);
}

string TParamContr::add( const string &iid, unsigned type )
{
    return (mPrm < 0) ? "" : chldAdd(mPrm, owner().ParamAttach(TSYS::strEncode(sTrm(iid),TSYS::oscdID),type));
}

void TParamContr::del( const string &iid, int flags )
{
    if(mPrm < 0) return;
    chldDel(mPrm, iid, -1, flags);
}

AutoHD<TParamContr> TParamContr::at( const string &iid, const string &who ) const
{
    if(mPrm < 0) return AutoHD<TParamContr>();
    return chldAt(mPrm, iid);
}

void TParamContr::LoadParmCfg( )
{
    if(mPrm < 0) return;

    map<string, bool>	itReg;

    //Search and create new parameters
    for(unsigned iTp = 0; iTp < owner().owner().tpPrmSize(); iTp++) {
	if(owner().owner().tpPrmAt(iTp).DB(&owner()).empty()) continue;
	try {
	    TConfig cEl(&owner().owner().tpPrmAt(iTp));
	    //cEl.cfgViewAll(false);
	    cEl.cfg("OWNER").setS(ownerPath(true), TCfg::ForceUse);

	    // Search new into DB and Config-file
	    for(int fld_cnt = 0; TBDS::dataSeek(owner().DB()+"."+owner().owner().tpPrmAt(iTp).DB(&owner()),
		    owner().owner().nodePath()+owner().owner().tpPrmAt(iTp).DB(&owner()),fld_cnt++,cEl,TBDS::UseCache); )
	    {
		try {
		    string shfr = cEl.cfg("SHIFR").getS();
		    if(!present(shfr))	add(shfr, iTp);
		    at(shfr).at().load(&cEl);
		    itReg[shfr] = true;
		} catch(TError &err) {
		    mess_err(err.cat.c_str(), "%s", err.mess.c_str());
		    mess_sys(TMess::Error, _("Error adding parameter '%s'."), cEl.cfg("SHIFR").getS().c_str());
		}
	    }
	} catch(TError &err) {
	    mess_err(err.cat.c_str(), "%s", err.mess.c_str());
	    mess_sys(TMess::Error, _("Error finding and creating new parameters."));
	}
    }

    //Check for remove items removed from the DB
    if(SYS->chkSelDB(SYS->selDB(),true)) {
	vector<string> itLs;
	list(itLs);
	for(unsigned iIt = 0; iIt < itLs.size(); iIt++)
	    if(itReg.find(itLs[iIt]) == itReg.end())
		del(itLs[iIt]);
    }

    //Force load present parameters
    vector<string> prmLs;
    list(prmLs);
    for(unsigned iP = 0; iP < prmLs.size(); iP++) {
	at(prmLs[iP]).at().modifG();
	at(prmLs[iP]).at().load();
    }
}

void TParamContr::postEnable( int flag )
{
    TValue::postEnable(flag);

    if(!vlCfg()) setVlCfg(this);
    if(!vlElemPresent(&SYS->daq().at().elErr()))
	vlElemAtt(&SYS->daq().at().elErr());

    type().create(this);
}

void TParamContr::preDisable( int flag )
{
    type().destroy(this);

    //Delete or stop the archives
    vector<string> aLs;
    vlList(aLs);
    for(unsigned iA = 0; iA < aLs.size(); iA++)
	if(!vlAt(aLs[iA]).at().arch().freeStat()) {
	    string arh_id = vlAt(aLs[iA]).at().arch().at().id();
	    if(flag&NodeRemove_NoArch)
		SYS->archive().at().valAt(arh_id).at().stop();
	    else SYS->archive().at().valDel(arh_id, true);
	}

    if(enableStat())	disable();
}

void TParamContr::postDisable( int flag )
{
    if(flag&NodeRemove) {
	//Delete the parameter from DB
	cfg("OWNER") = ownerPath();
	TBDS::dataDel(owner().DB()+"."+type().DB(&owner()), owner().owner().nodePath()+type().DB(&owner()), *this, TBDS::UseAllKeys);
    }
}

void TParamContr::load_( TConfig *icfg )
{
    if(!SYS->chkSelDB(owner().DB())) throw TError();

    if(icfg) *(TConfig*)this = *icfg;
    else {
	//Checking for need to change the parameter type at loading from the configuration context
	if(SYS->cfgCtx() && !SYS->cfgCtx()->childGet("prmTp",type().name,true))
	    for(unsigned iTp = 0; iTp < owner().owner().tpPrmSize(); ++iTp)
		if(SYS->cfgCtx()->childGet("prmTp",owner().owner().tpPrmAt(iTp).name,true)) {
		    setType(owner().owner().tpPrmAt(iTp).name);
		    break;
		}

	//cfgViewAll(true);
	cfg("OWNER") = ownerPath();
	TBDS::dataGet(owner().DB()+"."+type().DB(&owner()), owner().owner().nodePath()+type().DB(&owner()), *this);
    }

    LoadParmCfg();
}

void TParamContr::save_( )
{
    cfg("OWNER") = ownerPath();
    cfg("TIMESTAMP") = (int64_t)SYS->sysTm();
    TBDS::dataSet(owner().DB()+"."+type().DB(&owner()), owner().owner().nodePath()+type().DB(&owner()), *this);
    if(SYS->cfgCtx(true)) SYS->cfgCtx(true)->setAttr("prmTp", type().name);

    //Save archives
    vector<string> aLs;
    vlList(aLs);
    for(unsigned iA = 0; iA < aLs.size(); iA++)
	if(!vlAt(aLs[iA]).at().arch().freeStat())
	    vlAt(aLs[iA]).at().arch().at().save();
}

bool TParamContr::cfgChange( TCfg &co, const TVariant &pc )
{
    if(co.getS() != pc.getS()) modif();
    return type().cfgChange(this, co);
}

void TParamContr::enable( )
{
    type().enable(this);

    bool enErr = false;
    //Enable the parameters
    vector<string> prm_list;
    list(prm_list);
    for(unsigned iPrm = 0; iPrm < prm_list.size(); iPrm++)
	if(at(prm_list[iPrm]).at().toEnable())
	    try{ at(prm_list[iPrm]).at().enable(); }
	    catch(TError &err) {
		mess_warning(err.cat.c_str(), "%s", err.mess.c_str());
		mess_sys(TMess::Warning, _("Error turning on the parameter '%s'."), prm_list[iPrm].c_str());
		enErr = true;
	    }

    mEn = true;

    if(enErr) throw err_sys(_("Error turning on some parameters."));
}

void TParamContr::disable( )
{
    //Disable parameters
    vector<string> prm_list;
    list(prm_list);
    for(unsigned iPrm = 0; iPrm < prm_list.size(); iPrm++)
	if(at(prm_list[iPrm]).at().enableStat())
	    try{ at(prm_list[iPrm]).at().disable(); }
	    catch(TError &err) {
		mess_warning(err.cat.c_str(), "%s", err.mess.c_str());
		mess_sys(TMess::Warning, _("Error turning off the parameter '%s'."), prm_list[iPrm].c_str());
	    }

    type().disable(this);
    mEn = false;
}

void TParamContr::vlGet( TVal &val )
{
    if(val.name() == "err") {
	if(!enableStat()) val.setS(_("1:Parameter disabled."), 0, true);
	else if(!owner().startStat()) val.setS(_("2:Acquisition stopped."), 0, true);
	else val.setS("0", 0, true);
    }

    type().vlGet(this, val);
}

void TParamContr::vlSet( TVal &vo, const TVariant &vl, const TVariant &pvl )
{
    type().vlSet(this, vo, vl, pvl);
}

bool TParamContr::vlSetRednt( TVal &vo, const TVariant &vl, const TVariant &pvl )
{
    if(!owner().redntUse()) return false;
    if(vl == pvl) return true;

    XMLNode req("set");
    req.setAttr("path", nodePath(0,true)+"/%2fserv%2fattr")->setAttr("reforwardRedundOff", "1")
	->childAdd("el")->setAttr("id", vo.name())->setText(vl.getS());
    SYS->daq().at().rdStRequest(owner().workId(), req);

    return true;
}

void TParamContr::vlArchMake( TVal &val )
{
    if(!val.arch().freeStat())	val.arch().at().setDB(owner().DB());
    type().vlArchMake(this, val);
}

void TParamContr::setType( const string &tpId )
{
    if(enableStat() || tpId == type().name || !owner().owner().tpPrmPresent(tpId))	return;

    //Remove all included parameters
    if(mPrm >= 0 && !owner().owner().tpPrmAt(owner().owner().tpPrmToId(tpId)).isPrmCntr) {
	vector<string> pls;
	list(pls);
	for(unsigned iP = 0; iP < pls.size(); iP++) del(pls[iP], true);
	grpDel(mPrm); mPrm = -1;
    }

    type().destroy(this);
    setNodeMode(TCntrNode::Disabled);

    try {
	//Wait for disconnect other
	while(nodeUse(true) > 1) TSYS::sysSleep(1e-3);
	//Remove from DB
	postDisable(NodeRemove);

	//Create temporary structure
	TConfig tCfg(&type());
	tCfg = *(TConfig*)this;

	//Set new config structure
	tpParm = &owner().owner().tpPrmAt(owner().owner().tpPrmToId(tpId));
	setElem(tpParm);

	//Restore configurations
	*(TConfig*)this = tCfg;
    } catch(...) { }

    if(mPrm < 0 && tpParm->isPrmCntr) mPrm = grpAdd("prm_");

    setNodeMode(TCntrNode::Enabled);

    setVlCfg(this);

    type().create(this);

    modif();
}

TVariant TParamContr::objFuncCall( const string &iid, vector<TVariant> &prms, const string &user )
{
    // TCntrNodeObj cntr() - get the controller node
    if(iid == "cntr")	return new TCntrNodeObj(AutoHD<TCntrNode>(&owner()), user);
    // bool alarmSet( string mess, int lev = -5, bool force = false ) -
    //		set alarm to message <mess> and level <lev> and omit the presence control at <force>.
    if(iid == "alarmSet" && prms.size() >= 1) {
	owner().alarmSet(prms[0].getS(), (prms.size() >= 2) ? prms[1].getI() : -TMess::Crit,
	    ownerPath(true)+"\n"+name(), (prms.size() >= 3) ? prms[2].getB() : false);
	return true;
    }

    //Configuration functions call
    TVariant cfRez = objFunc(iid, prms, user, RWRWR_, "root:" SDAQ_ID);
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
	ctrMkNode("branches",opt,-1,"/br","",R_R_R_);
	if(ctrMkNode("area",opt,0,"/prm",_("Parameter"))) {
	    if(ctrMkNode("area",opt,-1,"/prm/st",_("State"))) {
		ctrMkNode("fld",opt,-1,"/prm/st/type",_("Type"),((!enableStat()&&owner().owner().tpPrmSize()>1)?RWRWR_:R_R_R_),"root",SDAQ_ID,4,
		    "tp","str","dest","select","select","/prm/tpLst",
		    "help",_("The type changing leads to lose some data of the specific configurations."));
		/*if(!enableStat() && owner().owner().tpPrmSize() > 1)
		    ctrMkNode("fld",opt,-1,"/prm/st/type",_("Type"),RWRWR_,"root",SDAQ_ID,4,"tp","str","dest","select","select","/prm/tpLst",
			"help",_("The type changing leads to lose some data of the specific configurations."));
		else ctrMkNode("fld",opt,-1,"/prm/st/type",_("Type"),R_R_R_,"root",SDAQ_ID,1,"tp","str");*/
		if(owner().enableStat())
		    ctrMkNode("fld",opt,-1,"/prm/st/en",_("Enabled"),RWRWR_,"root",SDAQ_ID,1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/prm/st/timestamp",_("Date of modification"),R_R_R_,"root",SDAQ_ID,1,"tp","time");
	    }
	    if(ctrMkNode("area",opt,-1,"/prm/cfg",_("Configuration"))) {
		TConfig::cntrCmdMake(opt,"/prm/cfg",0,"root",SDAQ_ID,RWRWR_);
		ctrRemoveNode(opt,"/prm/cfg/OWNER");
		ctrRemoveNode(opt,"/prm/cfg/TIMESTAMP");
	    }
	}

	type().cntrCmdProc(this, opt);

	if(mPrm >= 0) {
	    ctrMkNode("grp",opt,-1,"/br/prm_",_("Parameter"),RWRWR_,"root",SDAQ_ID,2,"idm",i2s(limObjNm_SZ).c_str(),"idSz",i2s(limObjID_SZ).c_str());
	    if(ctrMkNode("area",opt,-1,"/iPrms",_("Inclusion"))) {
		ctrMkNode("fld",opt,-1,"/iPrms/nmb",_("Number"),R_R_R_,"root",SDAQ_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/iPrms/prm",_("Parameters"),RWRWR_,"root",SDAQ_ID,5,
		    "tp","br","idm",i2s(limObjNm_SZ).c_str(),"s_com","add,del","br_pref","prm_","idSz",i2s(limObjID_SZ).c_str());
	    }
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
    else if(a_path == "/prm/st/timestamp" && ctrChkNode(opt))	opt->setText(i2s(timeStamp()));
    else if(mPrm >= 0 && a_path == "/iPrms/nmb" && ctrChkNode(opt)) {
	vector<string> c_list;
	list(c_list);
	unsigned eC = 0;
	for(unsigned iA = 0; iA < c_list.size(); iA++)
	    if(at(c_list[iA]).at().enableStat()) eC++;
	opt->setText(TSYS::strMess(_("All: %d; Enabled: %d"),c_list.size(),eC));
    }
    else if((a_path == "/br/prm_" || a_path == "/iPrms/prm")) {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD)) {
	    vector<string> c_list;
	    list(c_list);
	    for(unsigned iA = 0; iA < c_list.size(); iA++) {
	        XMLNode *cN = opt->childAdd("el")->setAttr("id",c_list[iA])->setText(at(c_list[iA]).at().name());
		if(!s2i(opt->attr("recurs"))) continue;
		cN->setName(opt->name())->setAttr("path",TSYS::strEncode(opt->attr("path"),TSYS::PathEl))->setAttr("recurs","1");
		at(c_list[iA]).at().cntrCmd(cN);
		cN->setName("el")->setAttr("path","")->setAttr("rez","")->setAttr("recurs","")->setText("");
	    }
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SDAQ_ID,SEC_WR)) { opt->setAttr("id", add(opt->attr("id"))); at(opt->attr("id")).at().setName(opt->text()); }
	if(ctrChkNode(opt,"del",RWRWR_,"root",SDAQ_ID,SEC_WR))	del(opt->attr("id"),true);
    }
    else if(type().cntrCmdProc(this, opt)) /* Process OK */;
    else if(a_path.compare(0,8,"/prm/cfg") == 0) TConfig::cntrCmdProc(opt, TSYS::pathLev(a_path,2), "root", SDAQ_ID, RWRWR_);
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
