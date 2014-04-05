
//OpenSCADA system module DAQ.MMS file: module.cpp
/***************************************************************************
 *   Copyright (C) 2013-2014 by Roman Savochenko                           *
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

#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#include <terror.h>
#include <tsys.h>
#include <tmess.h>
#include <ttiparam.h>
#include <tdaqs.h>

#include "module.h"

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"MMS"
#define MOD_NAME	_("MMS(IEC-9506)")
#define MOD_TYPE	SDAQ_ID
#define VER_TYPE	SDAQ_VER
#define MOD_VER		"0.5.0"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("MMS(IEC-9506) client implementation.")
#define LICENSE		"GPL2"
//*************************************************

ModMMS::TTpContr *ModMMS::mod;  //Pointer for direct access to the module

extern "C"
{
    TModule::SAt module( int n_mod )
    {
	if(n_mod == 0)	return TModule::SAt(MOD_ID, MOD_TYPE, VER_TYPE);
	return TModule::SAt("");
    }

    TModule *attach( const TModule::SAt &AtMod, const string &source )
    {
	if(AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE))
	    return new ModMMS::TTpContr(source);
	return NULL;
    }
}

using namespace ModMMS;

//*************************************************
//* TTpContr                                      *
//*************************************************
TTpContr::TTpContr( string name ) : TTipDAQ(MOD_ID)
{
    mod		= this;

    mName	= MOD_NAME;
    mType	= MOD_TYPE;
    mVers	= MOD_VER;
    mAuthor	= AUTHORS;
    mDescr	= DESCRIPTION;
    mLicense	= LICENSE;
    mSource	= name;
}

TTpContr::~TTpContr( )	{ }

void TTpContr::postEnable( int flag )
{
    TTipDAQ::postEnable(flag);

    //> Controler's bd structure
    fldAdd(new TFld("PRM_BD",_("Parameteres table"),TFld::String,TFld::NoFlag,"30",""));
    fldAdd(new TFld("SCHEDULE",_("Acquisition schedule"),TFld::String,TFld::NoFlag,"100","1"));
    fldAdd(new TFld("PRIOR",_("Gather task priority"),TFld::Integer,TFld::NoFlag,"2","0","-1;99"));
    fldAdd(new TFld("SYNCPER",_("Sync inter remote station period (s)"),TFld::Real,TFld::NoFlag,"6.2","60","0;1000"));
    fldAdd(new TFld("ADDR",_("Server address"),TFld::String,TFld::NoFlag,"50","localhost:102"));
    fldAdd(new TFld("VARS_RD_REQ",_("Variables into read request"),TFld::Integer,TFld::NoFlag,"3","100","10;9999"));

    //> Parameter type bd structure
    int t_prm = tpParmAdd("std", "PRM_BD", _("Standard"), true);
    tpPrmAt(t_prm).fldAdd(new TFld("VAR_LS",_("Variables list (next line separated)"),TFld::String,TFld::FullText|TCfg::NoVal,"100000",""));
}

TController *TTpContr::ContrAttach( const string &name, const string &daq_db )
{
    return new TMdContr(name, daq_db, this);
}

void TTpContr::load_( )	{ }

void TTpContr::save_( )	{ }

void TTpContr::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if(opt->name() == "info")
    {
	TTipDAQ::cntrCmdProc(opt);
	return;
    }
    //> Process command to page
    string a_path = opt->attr("path");
    TTipDAQ::cntrCmdProc(opt);
}

//*************************************************
//* TMdContr                                      *
//*************************************************
TMdContr::TMdContr( string name_c, const string &daq_db, ::TElem *cfgelem ) : TController(name_c,daq_db,cfgelem),
    mSched(cfg("SCHEDULE")), mPrior(cfg("PRIOR")), mSync(cfg("SYNCPER")), mAddr(cfg("ADDR")), mVarsRdReq(cfg("VARS_RD_REQ")),
    prcSt(false), callSt(false), isReload(false), tmGath(0), tmDelay(0)
{
    cfg("PRM_BD").setS("MMSPrm_"+name_c);

    //ParameterCBB
    string prms;
    setBS(prms, SupportOpts::str1);
    setBS(prms, SupportOpts::str2);
    setBS(prms, SupportOpts::vnam);
    setBS(prms, SupportOpts::valt);
    setBS(prms, SupportOpts::vadr);
    setBS(prms, SupportOpts::tpy);
    setBS(prms, SupportOpts::vlis);
    setCallParameterCBB(prms);

    //ServicesSupported
    prms = "";
    setBS(prms, SupportServs::status);
    setBS(prms, SupportServs::getNameList);
    setBS(prms, SupportServs::identify);
    setBS(prms, SupportServs::read);
    setBS(prms, SupportServs::write);
    setBS(prms, SupportServs::getVariableAccessAttributes);
    setBS(prms, SupportServs::getCapabilityList);
    setCallServicesSupported(prms);
}

TMdContr::~TMdContr( )
{
    if(run_st) stop();
}

string TMdContr::getStatus( )
{
    string rez = TController::getStatus();
    if(startStat() && !redntUse())
    {
	if(tmDelay > -1)
	{
	    rez += TSYS::strMess(_("Connection error. Restoring in %.6g s."), tmDelay);
	    rez.replace(0, 1, "10");
	}
	else
	{
	    if(callSt)	rez += TSYS::strMess(_("Call now. "));
	    if(period())rez += TSYS::strMess(_("Call by period: %s. "),TSYS::time2str(1e-3*period()).c_str());
	    else rez += TSYS::strMess(_("Call next by cron '%s'. "),TSYS::time2str(TSYS::cron(cron()),"%d-%m-%Y %R").c_str());
	    rez += TSYS::strMess(_("Spent time: %s. Requests %.6g."),TSYS::time2str(tmGath).c_str(),-tmDelay);
	}
    }
    return rez;
}

void TMdContr::regVar( const string &vl )
{
    ResAlloc res(reqRes, true);
    if(mVars.find(vl) == mVars.end()) mVars[vl] = TVariant();
}

void TMdContr::reqService( XML_N &io )
{
    ResAlloc res(nodeRes(), true);
    io.setAttr("err", "");

    try { tr.at().start(); }
    catch(TError err) { io.setAttr("err", TSYS::strMess("%s",err.mess.c_str())); return; }

    Client::reqService(io);
    if(io.attr("err").empty()) tmDelay--;
}

void TMdContr::protIO( XML_N &io )
{
    ResAlloc resN(tr.at().nodeRes(), true);
    if(messLev() == TMess::Debug) io.setAttr("debug", "1");
    try { Client::protIO(io); }
    catch(TError er) { io.setAttr("err", TSYS::strMess("%s:%s", _("Remote host error"), er.mess.c_str())); }
}

int TMdContr::messIO( const char *obuf, int len_ob, char *ibuf, int len_ib )
{
    //printf("TEST 00: req=%d: '%s'\n", len_ob, TSYS::strDecode(string(obuf,len_ob),TSYS::Bin).c_str());
    return tr.at().messIO(obuf, len_ob, ibuf, len_ib, 0, true);
}

void TMdContr::debugMess( const string &mess )
{
    mess_debug_(nodePath().c_str(), "%s", mess.c_str());
}

TParamContr *TMdContr::ParamAttach( const string &name, int type )
{
    return new TMdPrm(name, &owner().tpPrmAt(type));
}

void TMdContr::enable_( )
{
    string trName = "Sockets.out_MMS"+id();
    tr = SYS->transport().at().nodeAt(trName, 0, '.', 0, true);
    if(tr.freeStat())
    {
	SYS->transport().at().at(TSYS::strParse(trName,0,".")).at().outAdd(TSYS::strParse(trName,1,".").substr(4));
	tr = SYS->transport().at().nodeAt(trName, 0, '.');
	tr.at().setDscr(TSYS::strMess(_("MMS automatic created transport for '%s' controller."),id().c_str()));
    }
}

void TMdContr::disable_( )
{
    tr.free();
    mVars.clear();
}

void TMdContr::start_( )
{
    //> Schedule process
    mPer = TSYS::strSepParse(cron(),1,' ').empty() ? vmax(0,(int64_t)(1e9*atof(cron().c_str()))) : 0;
    tmDelay = 0;

    // Clear data blocks
    mVars.clear();

    // Reenable parameters
    try
    {
	vector<string> pls;
	list(pls);

	isReload = true;
	for(unsigned i_p = 0; i_p < pls.size(); i_p++)
	    if(at(pls[i_p]).at().enableStat()) at(pls[i_p]).at().enable();
	isReload = false;
    } catch(TError) { isReload = false; throw; }

    //> Start the gathering data task
    SYS->taskCreate(nodePath('.',true), mPrior, TMdContr::Task, this);
}

void TMdContr::stop_( )
{
    //> Stop the request and calc data task
    SYS->taskDestroy(nodePath('.',true));

    //> Set EVal
    ResAlloc res(enRes, false);
    for(unsigned i_p = 0; i_p < pHD.size(); i_p++)
	pHD[i_p].at().setEval();
}

void TMdContr::prmEn( TMdPrm *prm, bool val )
{
    unsigned i_prm;

    ResAlloc res(enRes, true);
    for(i_prm = 0; i_prm < pHD.size(); i_prm++)
	if(&pHD[i_prm].at() == prm) break;

    if(val && i_prm >= pHD.size())	pHD.push_back(prm);
    if(!val && i_prm < pHD.size())	pHD.erase(pHD.begin()+i_prm);
}

void *TMdContr::Task( void *icntr )
{
    vector<string> als;
    string	nId;
    TMdContr &cntr = *(TMdContr *)icntr;

    bool firstCall = true;
    cntr.prcSt = true;

    for(unsigned int it_cnt = cntr.pHD.size(); !TSYS::taskEndRun(); it_cnt++)
    {
	if(cntr.redntUse()) { TSYS::sysSleep(1); continue; }
	if(cntr.tmDelay > 0){ TSYS::sysSleep(1); cntr.tmDelay = vmax(0,cntr.tmDelay-1); continue; }

	int64_t t_cnt = TSYS::curTime();

	//Prepare and read block variables
	XML_N valCtr("MMS"), *value = NULL;
	valCtr.setAttr("id","read");
	ResAlloc res(cntr.reqRes, false);
	for(map<string,TVariant>::iterator vi = cntr.mVars.begin(); true; ++vi)
	{
	    // Send request
	    if(vi == cntr.mVars.end() || valCtr.childSize() >= cntr.mVarsRdReq.getI())
	    {
		if(!valCtr.childSize())	break;

		cntr.reqService(valCtr);

		//  Process result
		for(int i_ch = 0; i_ch < valCtr.childSize(); i_ch++)
		{
		    value = valCtr.childGet(i_ch);
		    nId = (value->attr("domainId").size()?value->attr("domainId"):"*")+"/"+value->attr("itemId");
		    if(!valCtr.attr("err").empty()) value = NULL;
		    if(!value) { cntr.mVars[nId] = TVariant(); continue; }
		    switch(atoi(value->attr("tp").c_str()))
		    {
			case VT_Bool: case VT_Int: case VT_UInt:
			case VT_Float:
			case VT_BitString: case VT_OctString: case VT_VisString:
			    cntr.mVars[nId] = value->text();
			    break;
			case VT_Array: case VT_Struct:		//!!!! Need for test
			{
			    vector<StackTp> stack;
			    TArrayObj *curArr = new TArrayObj();
			    cntr.mVars[nId] = curArr;
			    XML_N *curValue = value;
			    for(int i_v = 0; true; )
			    {
				if(i_v >= curValue->childSize())
				{
				    if(stack.size())
				    {
					curArr = stack.back().arr;
					curValue = stack.back().vl;
					i_v = stack.back().inPos+1;
					stack.pop_back();
					continue;
				    }
				    else break;
				}

				XML_N *itValue = curValue->childGet(i_v);
				switch(atoi(itValue->attr("tp").c_str()))
				{
				    case VT_Bool:  curArr->arSet(i_v, bool(atoi(itValue->text().c_str())));		break;
				    case VT_Int: case VT_UInt:	curArr->arSet(i_v, atoll(itValue->text().c_str()));	break;
				    case VT_Float: curArr->arSet(i_v, atof(itValue->text().c_str()));			break;
				    case VT_BitString: case VT_OctString: case VT_VisString:
					curArr->arSet(i_v, itValue->text());
					break;
				    case VT_Array: case VT_Struct:
				    {
					stack.push_back(StackTp(curArr,curValue,i_v));
					curValue = itValue;
					TArrayObj *itArr = new TArrayObj();
					curArr->arSet(i_v, itArr);
					curArr = itArr;
					i_v = 0;
					continue;
				    }
				}
				i_v++;
			    }
			    break;
			}
			//case MMS_UTC_TIME: pVal.at().setI(MmsValue_toUnixTimestamp(value), 0, true);		break;
			default: mess_err(cntr.nodePath().c_str(), _("Value type read '%s' is not implemented."), value->attr("tp").c_str());
		    }
		}

		valCtr.childClear();
		if(vi == cntr.mVars.end()) break;
	    }

	    value = valCtr.childAdd("it")->setAttr("itemId", TSYS::pathLev(vi->first,1));
	    if(TSYS::pathLev(vi->first,0) != "*") value->setAttr("domainId", TSYS::pathLev(vi->first,0));
	}
	res.release();

	//Update controller's data
	cntr.enRes.resRequestR();
	cntr.callSt = true;
	unsigned int div = cntr.period() ? (unsigned int)(cntr.syncPer()/(1e-9*cntr.period())) : 0;
	for(unsigned i_p = 0; i_p < cntr.pHD.size() && !cntr.redntUse() && !TSYS::taskEndRun(); i_p++)
	    try
	    {
		cntr.pHD[i_p].at().vlList(als);
		if(firstCall || (div && (it_cnt%div) < cntr.pHD.size()))
		{
		    printf("TEST 00: firstCall=%d; div=%d; it_cnt=%d\n",firstCall,div,it_cnt);
		    cntr.pHD[i_p].at().attrPrc();
		}

		//Attributes update
		res.request(false);
		for(unsigned i_a = 0; i_a < als.size(); i_a++)
		{
		    AutoHD<TVal> pVal = cntr.pHD[i_p].at().vlAt(als[i_a]);
		    nId = TSYS::strLine(pVal.at().fld().reserve(),0);
		    if(nId.empty()) continue;
		    //printf("TEST 01: '%s' = %s\n", nId.c_str(), cntr.mVars[nId].getS().c_str());
		    pVal.at().set(cntr.mVars[nId], 0, true);
		}
		res.release();
	    }
	    catch(TError err)	{ mess_err(err.cat.c_str(), "%s", err.mess.c_str()); }

	cntr.callSt = firstCall = false;
	cntr.enRes.resRelease();
	cntr.tmGath = TSYS::curTime()-t_cnt;

	if(TSYS::taskEndRun()) break;
	TSYS::taskSleep(cntr.period(), (cntr.period()?0:TSYS::cron(cntr.cron())));
    }

    cntr.prcSt = false;

    return NULL;
}

bool TMdContr::cfgChange( TCfg &icfg )
{
    TController::cfgChange(icfg);

    try
    {
	if(icfg.name() == "ADDR" && enableStat()) tr.at().setAddr("TCP:"+icfg.getS());
    } catch(...) { }

    return true;
}

void TMdContr::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if(opt->name() == "info")
    {
	TController::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/cntr/cfg/SCHEDULE",mSched.fld().descr(),startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,3,
	    "dest","sel_ed","sel_list",TMess::labSecCRONsel(),"help",TMess::labSecCRON());
	ctrMkNode("fld",opt,-1,"/cntr/cfg/PRIOR",mPrior.fld().descr(),startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,1,"help",TMess::labTaskPrior());
	ctrMkNode("fld",opt,-1,"/cntr/cfg/ADDR",mAddr.fld().descr(),startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID);
	return;
    }
    //> Process command to page
    string a_path = opt->attr("path");
    TController::cntrCmdProc(opt);
}

//*************************************************
//* TMdPrm                                        *
//*************************************************
TMdPrm::TMdPrm( string name, TTipParam *tp_prm ) : TParamContr(name,tp_prm), p_el("w_attr")
{

}

TMdPrm::~TMdPrm( )	{ nodeDelAll(); }

void TMdPrm::postEnable( int flag )
{
    TParamContr::postEnable(flag);
    if(!vlElemPresent(&p_el))   vlElemAtt(&p_el);
}

TMdContr &TMdPrm::owner( )	{ return (TMdContr&)TParamContr::owner(); }

void TMdPrm::enable( )
{
    if(enableStat() && !owner().isReload) return;
    TParamContr::enable();

    attrPrc();

    owner().prmEn(this, true);
}

void TMdPrm::disable( )
{
    if(!enableStat())  return;

    owner().prmEn(this, false);

    TParamContr::disable();

    setEval();
}

void TMdPrm::load_( )
{
    TParamContr::load_();
}

void TMdPrm::save_( )
{
    TParamContr::save_();
}

void TMdPrm::attrPrc( XML_N *iVal, vector<string> *iAls, const string &vid )
{
    vector<string> *als = iAls ? iAls : new vector<string>;

    bool wrMode;
    string varLs = varList(), var;
    if(tryBS(owner().parameterCBB(), (uint32_t)SupportServs::getVariableAccessAttributes))
	for(int off = 0; iVal || (var=TSYS::strLine(varLs,0,&off)).size(); )
	{
	    wrMode = (var.compare(0,2,"w:") == 0);
	    if(wrMode) var.erase(0,2);
	    XML_N *valCntr = NULL, *value = iVal;
	    if(!value)
	    {
		valCntr = new XML_N("MMS");
		value = valCntr->setAttr("id","getVariableAccessAttributes")->setAttr("itemId", TSYS::pathLev(var,1));
		if(TSYS::pathLev(var,0) != "*") value->setAttr("domainId", TSYS::pathLev(var,0));
		owner().reqService(*valCntr);
		if(!valCntr->attr("err").empty()) { delete valCntr; continue; }
	    }

	    bool srchOK = false;
	    string vName = iVal ? vid+"$"+value->attr("id") : var;
	    // Find for already presented attribute
	    for(unsigned i_a = 0; i_a < p_el.fldSize() && !srchOK; i_a++)
		if(TSYS::strLine(p_el.fldAt(i_a).reserve(),0) == vName) srchOK = true;
	    if(srchOK) { als->push_back(vName); owner().regVar(vName); }
	    else
	    {
		string aid = TSYS::strEncode(TSYS::pathLev(vName,1), TSYS::oscdID);
		if(vlPresent(aid))
		    for(int i_v = 1; true; i_v++)
			if(!vlPresent(aid+MMS::i2s(i_v)))
			{ aid += MMS::i2s(i_v); break; }

		int vtp = -1;
		switch(atoi(value->attr("tp").c_str()))
		{
		    case VT_Bool:	vtp = TFld::Boolean;		break;
		    case VT_Int: case VT_UInt: vtp = TFld::Integer;	break;
		    case VT_Float:	vtp = TFld::Real;		break;
		    case VT_BitString: case VT_VisString: case VT_OctString: vtp = TFld::String;	break;
		    case VT_Struct:
			for(int i_a = 0; i_a < value->childSize(); i_a++)
			    attrPrc(value->childGet(i_a), als, vName);
			break;
		    case VT_Array: vtp = TFld::Object;		break;
		    default: mess_err(nodePath().c_str(), _("Value type %s is not implemented for '%s'."), value->attr("tp").c_str(), vName.c_str());
		}
		if(vtp >= 0)
		{
		    p_el.fldAdd(new TFld(aid.c_str(),TSYS::pathLev(vName,1).c_str(),(TFld::Type)vtp,(wrMode?(int)TVal::DirWrite:(int)TFld::NoWrite),
			"","","","",(vName+"\n"+value->attr("tp")).c_str()));
		    als->push_back(vName);
		    owner().regVar(vName);
		}
	    }

	    if(!valCntr) break;
	    delete valCntr;
	}
    else
    {
	XML_N valCntr("MMS"), *value = valCntr.setAttr("id","read")->childAdd("it");
	for(int off = 0; (var=TSYS::strLine(varLs,0,&off)).size(); )
	{
	    wrMode = (var.compare(0,2,"w:") == 0);
	    if(wrMode) var.erase(0,2);

	    bool srchOK = false;
	    // Find for already presented attribute
	    for(unsigned i_a = 0; i_a < p_el.fldSize() && !srchOK; i_a++)
		if(TSYS::strLine(p_el.fldAt(i_a).reserve(),0) == var) srchOK = true;
	    if(srchOK) { als->push_back(var); owner().regVar(var); continue; }

	    string aid = TSYS::strEncode(TSYS::pathLev(var,1), TSYS::oscdID);
	    if(vlPresent(aid))
		for(int i_v = 1; true; i_v++)
		    if(!vlPresent(aid+MMS::i2s(i_v)))
		    { aid += MMS::i2s(i_v); break; }

	    value->setAttr("itemId", TSYS::pathLev(var,1));
	    if(TSYS::pathLev(var,0) != "*") value->setAttr("domainId", TSYS::pathLev(var,0));
	    owner().reqService(valCntr);
	    if(!valCntr.attr("err").empty()) continue;

	    int vtp = -1;
	    switch(atoi(value->attr("tp").c_str()))
	    {
		case VT_Bool:	vtp = TFld::Boolean;		break;
		case VT_Int: case VT_UInt: vtp = TFld::Integer;	break;
		case VT_Float:	vtp = TFld::Real;		break;
		case VT_BitString: case VT_VisString: case VT_OctString: vtp = TFld::String;	break;
		case VT_Struct:	vtp = TFld::Object;		break;
		case VT_Array: vtp = TFld::Object;			break;
		default: mess_err(nodePath().c_str(), _("Value type %s is not implemented for '%s'."), value->attr("tp").c_str(), var.c_str());
	    }
	    if(vtp >= 0)
	    {
		p_el.fldAdd(new TFld(aid.c_str(),TSYS::pathLev(var,1).c_str(),(TFld::Type)vtp,(wrMode?(int)TVal::DirWrite:(int)TFld::NoWrite),
		    "","","","",(var+"\n"+value->attr("tp")).c_str()));
		als->push_back(var);
		owner().regVar(var);
	    }
	}
    }

    //Find for delete attribute
    for(unsigned i_a = 0, i_p; !iVal && i_a < p_el.fldSize(); )
    {
	for(i_p = 0; i_p < als->size(); i_p++)
	    if(TSYS::strLine(p_el.fldAt(i_a).reserve(),0) == (*als)[i_p]) break;
	if(i_p >= als->size())
	    try{ p_el.fldDel(i_a); continue; } catch(TError err) { }
	i_a++;
    }

    if(!iAls) delete als;
}

void TMdPrm::setEval( )
{
    vector<string> ls;
    elem().fldList(ls);
    for(unsigned i_el = 0; i_el < ls.size(); i_el++)
	vlAt(ls[i_el]).at().setS(EVAL_STR, 0, true);
}

void TMdPrm::cntrCmdProc( XMLNode *opt )
{
    //Service commands process
    string a_path = opt->attr("path");
    if(a_path.substr(0,6) == "/serv/")	{ TParamContr::cntrCmdProc(opt); return; }

    //Get page info
    if(opt->name() == "info")
    {
	TParamContr::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/cfg/SEL_VAR",_("Variable append"),RWRW__,"root",SDAQ_ID,2,"dest","select","select","/prm/cfg/SEL_VAR_lst");
	return;
    }

    //Process command to page
    if(a_path == "/prm/cfg/SEL_VAR")
    {
	if(ctrChkNode(opt,"get")) opt->setText(TBDS::genDBGet(nodePath()+"selVAR","",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",SDAQ_ID,SEC_RD))
	{
	    if(TSYS::pathLev(opt->text(),1).size())
	    {
		string vLs = varList(), vS;
		for(int off = 0; (vS=TSYS::strLine(vLs,0,&off)).size(); )
		    if((vS.compare(0,2,"w:") == 0 && vS.compare(2,string::npos,opt->text()) == 0) || vS == opt->text())
			break;
		if(vS.empty()) setVarList(vLs+((vLs.size() && vLs[vLs.size()-1] != '\n')?"\n":"")+opt->text());
	    }
	    TBDS::genDBSet(nodePath()+"selVAR",TSYS::pathLev(opt->text(),0),opt->attr("user"));
	}
    }
    else if(a_path == "/prm/cfg/SEL_VAR_lst" && ctrChkNode(opt))
    {
	string selVAR = TSYS::pathLev(TBDS::genDBGet(nodePath()+"selVAR","",opt->attr("user")),0);
	if(selVAR.empty())	//Get domain list
	{
	    XML_N reqDom("MMS");
	    reqDom.setAttr("id", "getNameList")->setAttr("objectClass", MMS::i2s(OCL_Domain));
	    owner().reqService(reqDom);
	    opt->childAdd("el")->setText("*");
	    for(int i_d = 0; reqDom.attr("err").empty() && i_d < reqDom.childSize(); i_d++)
		opt->childAdd("el")->setText(reqDom.childGet(i_d)->text());
	}
	else			//Get names list
	{
	    opt->childAdd("el")->setText("");
	    XML_N reqVar("MMS");
	    reqVar.setAttr("moreFollows","");
	    string continueAfter;
	    do
	    {
		reqVar.clear()->setAttr("id","getNameList")->setAttr("objectClass", MMS::i2s(OCL_NmVar))->setAttr("continueAfter", continueAfter);
		if(selVAR != "*") reqVar.setAttr("domainSpecific",selVAR);
		owner().reqService(reqVar);
		for(int i_v = 0; reqVar.attr("err").empty() && i_v < reqVar.childSize(); i_v++)
		    opt->childAdd("el")->setText(selVAR+"/"+(continueAfter=reqVar.childGet(i_v)->text()));
	    }
	    while(atoi(reqVar.attr("moreFollows").c_str()));
	}
    }
    else TParamContr::cntrCmdProc(opt);
}

void TMdPrm::vlArchMake( TVal &val )
{
    TParamContr::vlArchMake(val);

    if(val.arch().freeStat()) return;
    val.arch().at().setSrcMode(TVArchive::PassiveAttr);
    val.arch().at().setPeriod((int64_t)(owner().period()*1000000));
    val.arch().at().setHardGrid(true);
    val.arch().at().setHighResTm(true);
}

void TMdPrm::vlSet( TVal &vo, const TVariant &vl, const TVariant &pvl )
{
    if(!enableStat())	vo.setS(EVAL_STR, 0, true);

    //Send to active reserve station
    if(owner().redntUse())
    {
	if(vl == pvl) return;
	XMLNode req("set");
	req.setAttr("path", nodePath(0,true)+"/%2fserv%2fattr")->childAdd("el")->setAttr("id",vo.name())->setText(vl.getS());
	SYS->daq().at().rdStRequest(owner().workId(), req);
	return;
    }

    if(vl.isEVal() || vl == pvl) return;

    int off = 0;
    string nId = TSYS::strLine(vo.fld().reserve(), 0, &off);
    int vTp = atoi(TSYS::strLine(vo.fld().reserve(),0,&off).c_str());

    XML_N valCtr("MMS");
    XML_N *value = valCtr.setAttr("id", "write")->
		    childAdd("it")->setAttr("itemId", TSYS::pathLev(nId,1))->setAttr("dataType", MMS::i2s(vTp));
    if(TSYS::pathLev(nId,0) != "*") value->setAttr("domainId", TSYS::pathLev(nId,0));
    switch(vTp)
    {
	case VT_Array: case VT_Struct:	//!!!! Need for test
	{
	    TArrayObj *curArr = NULL;
	    if(vl.type() != TVariant::Object || !(curArr=dynamic_cast<TArrayObj*>(&vl.getO().at()))) break;
	    vector<TMdContr::StackTp> stack;
	    XML_N *curValue = value;
	    for(int i_v = 0; true; )
	    {
		if(i_v >= curArr->arSize())
		{
		    if(stack.size())
		    {
			curArr = stack.back().arr;
			curValue = stack.back().vl;
			i_v = stack.back().inPos+1;
			stack.pop_back();
			continue;
		    }
		    else break;
		}

		XML_N *itValue = curValue->childAdd("it");
		TVariant itArr = curArr->arGet(i_v);
		switch(itArr.type())
		{
		    case TVariant::Boolean: itValue->setAttr("dataType",MMS::i2s(VT_Bool))->setText(itArr.getS());	break;
		    case TVariant::Integer: itValue->setAttr("dataType",MMS::i2s(VT_Int))->setText(itArr.getS());	break;
		    case TVariant::Real: itValue->setAttr("dataType",MMS::i2s(VT_Float))->setText(itArr.getS());	break;
		    case TVariant::String: itValue->setAttr("dataType",MMS::i2s(VT_VisString))->setText(itArr.getS());	break;
		    case TVariant::Object:
			if(!dynamic_cast<TArrayObj*>(&itArr.getO().at())) itValue->parent()->childDel(itValue);
			else
			{
			    stack.push_back(TMdContr::StackTp(curArr,curValue,i_v));
			    curValue = itValue;
			    curArr = (TArrayObj*)(&itArr.getO().at());
			    i_v = 0;
			    continue;
			}
		}
		i_v++;
	    }
	    break;
	}
	default: value->setText(vl.getS());
    }
    owner().reqService(valCtr);
    if(!valCtr.attr("err").empty())
    {
	vo.setS(EVAL_STR, 0, true);
	if(owner().messLev() == TMess::Debug) mess_debug_(nodePath().c_str(),_("Write to '%s' error: %s."),nId.c_str(),value->attr("err").c_str());
    }
}
