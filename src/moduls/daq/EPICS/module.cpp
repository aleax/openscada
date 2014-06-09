
//OpenSCADA system module DAQ.EPICS file: module.cpp
/***************************************************************************
 *   Copyright (C) 2014 by Roman Savochenko, <rom_as@oscada.org>           *
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
#define MOD_ID		"EPICS"
#define MOD_NAME	_("EPICS CA")
#define MOD_TYPE	SDAQ_ID
#define VER_TYPE	SDAQ_VER
#define MOD_VER		"0.1.0"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("Channel Access, by EPICS.")
#define LICENSE		"GPL2"
//*************************************************

ModEPICS::TTpContr *ModEPICS::mod;  //Pointer for direct access to the module

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
	    return new ModEPICS::TTpContr(source);
	return NULL;
    }
}

using namespace ModEPICS;

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

TTpContr::~TTpContr( )
{

}

void TTpContr::postEnable( int flag )
{
    TTipDAQ::postEnable(flag);

    //> Controler's bd structure
    fldAdd(new TFld("PRM_BD",_("Parameteres table"),TFld::String,TFld::NoFlag,"30",""));
    fldAdd(new TFld("SCHEDULE",_("Acquisition schedule"),TFld::String,TFld::NoFlag,"100","1"));
    fldAdd(new TFld("PRIOR",_("Gather task priority"),TFld::Integer,TFld::NoFlag,"2","0","-1;99"));
    fldAdd(new TFld("SYNCPER",_("Sync inter remote station period (s)"),TFld::Real,TFld::NoFlag,"6.2","60","0;1000"));
    fldAdd(new TFld("ADDR",_("Server address"),TFld::String,TFld::NoFlag,"50","localhost:102"));

    //> Parameter type bd structure
    int t_prm = tpParmAdd("std", "PRM_BD", _("Standard"), true);
    tpPrmAt(t_prm).fldAdd(new TFld("VAR_LS",_("Variables list (next line separated)"),TFld::String,TFld::FullText|TCfg::NoVal,"100000",""));
}

TController *TTpContr::ContrAttach( const string &name, const string &daq_db )
{
    return new TMdContr(name, daq_db, this);
}

void TTpContr::load_( )
{

}

void TTpContr::save_( )
{

}

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
    mSched(cfg("SCHEDULE")), mPrior(cfg("PRIOR")), mSync(cfg("SYNCPER")), mAddr(cfg("ADDR")),
    prcSt(false), callSt(false), tmGath(0), tmDelay(0)//, con(NULL)
{
    cfg("PRM_BD").setS("EPICSPrm_"+name_c);
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
	    if(period()) rez += TSYS::strMess(_("Call by period: %s. "), tm2s(1e-3*period()).c_str());
	    else rez += TSYS::strMess(_("Call next by cron '%s'. "), tm2s(TSYS::cron(cron()),"%d-%m-%Y %R").c_str());
	    rez += TSYS::strMess(_("Spent time: %s. Requests %.6g."), tm2s(tmGath).c_str(),-tmDelay);
	}
    }
    return rez;
}

TParamContr *TMdContr::ParamAttach( const string &name, int type )
{
    return new TMdPrm(name, &owner().tpPrmAt(type));
}

void TMdContr::enable_( )
{
    try{ connectServer(); }
    catch(TError err) { mess_err(nodePath().c_str(),"%s",err.mess.c_str()); }
}

void TMdContr::disable_( )
{
    disconnectServer( );
}

void TMdContr::start_( )
{
    //> Schedule process
    mPer = TSYS::strSepParse(cron(),1,' ').empty() ? vmax(0,(int64_t)(1e9*atof(cron().c_str()))) : 0;
    tmDelay = 0;

    //> Start the gathering data task
    SYS->taskCreate(nodePath('.',true), mPrior, TMdContr::Task, this);
}

void TMdContr::stop_( )
{
    //> Stop the request and calc data task
    SYS->taskDestroy(nodePath('.',true));

    //> Set EVal
    ResAlloc res(enRes, false);
    for(unsigned i_p = 0; i_p < p_hd.size(); i_p++)
	p_hd[i_p].at().setEval();
}

void TMdContr::connectServer( )
{
    int status = ca_create_channel("", 0, 0, 0, &chan);

    /*if(con) disconnectServer();

    con = MmsConnection_create();
    MmsError mmsError;
    bool rez = MmsConnection_connect(con, &mmsError, (char*)TSYS::strParse(addr(),0,":").c_str(), atoi(TSYS::strParse(addr(),1,":").c_str()));
    if(!rez)
    {
	MmsConnection_destroy(con);
	con = NULL;
	throw TError(nodePath().c_str(), _("Connect to server failed!"));
    }*/
}

void TMdContr::disconnectServer( )
{
    /*if(!con)	return;

    MmsConnection_destroy(con);
    con = NULL;*/
}

void TMdContr::prmEn( TMdPrm *prm, bool val )
{
    unsigned i_prm;

    ResAlloc res(enRes, true);
    for(i_prm = 0; i_prm < p_hd.size(); i_prm++)
	if(&p_hd[i_prm].at() == prm) break;

    if(val && i_prm >= p_hd.size())	p_hd.push_back(prm);
    if(!val && i_prm < p_hd.size())	p_hd.erase(p_hd.begin()+i_prm);
}

void *TMdContr::Task( void *icntr )
{
    //MmsError	error;
    vector<string> als;
    string	nId;
    TMdContr &cntr = *(TMdContr *)icntr;

    bool firstCall = true;
    cntr.prcSt = true;

    for(unsigned int it_cnt = cntr.p_hd.size(); !TSYS::taskEndRun(); it_cnt++)
    {
	if(cntr.redntUse()) { TSYS::sysSleep(1); continue; }
	if(cntr.tmDelay > 0){ TSYS::sysSleep(1); cntr.tmDelay = vmax(0,cntr.tmDelay-1); continue; }
	//try{ if(!cntr.con) cntr.connectServer(); }
	//catch(TError err) { cntr.tmDelay = cntr.syncPer(); continue; }
	firstCall = (cntr.tmDelay == 0);

	int64_t t_cnt = TSYS::curTime();

	//> Update controller's data
	/*cntr.enRes.resRequestR();
	cntr.callSt = true;
	unsigned int div = cntr.period() ? (unsigned int)(cntr.syncPer()/(1e-9*cntr.period())) : 0;
	for(unsigned i_p = 0; i_p < cntr.p_hd.size() && !cntr.redntUse() && !TSYS::taskEndRun(); i_p++)
	    try
	    {
		cntr.p_hd[i_p].at().vlList(als);
		if(firstCall || (div && (it_cnt%div) < cntr.p_hd.size())) cntr.p_hd[i_p].at().attrPrc();
		for(unsigned i_a = 0; i_a < als.size(); i_a++)
		{
		    AutoHD<TVal> pVal = cntr.p_hd[i_p].at().vlAt(als[i_a]);
		    nId = TSYS::strLine(pVal.at().fld().reserve(),0);
		    if(nId.empty()) continue;
		    MmsValue* value = NULL;
		    if(cntr.con)
		    {
			ResAlloc res(cntr.nodeRes(), true);
			value = MmsConnection_readVariable(cntr.con, &error, (char*)TSYS::pathLev(nId,0).c_str(), (char*)TSYS::pathLev(nId,1).c_str());
			res.release();
		    }
		    if(!value)
		    {
			pVal.at().setR(EVAL_REAL,0,true);
			if(error == MMS_ERROR_CONNECTION_LOST) cntr.disconnectServer();
			continue;
		    }
		    switch(value->type)
		    {
			case MMS_BOOLEAN: pVal.at().setB(MmsValue_getBoolean(value), 0, true);			break;
			case MMS_INTEGER: case MMS_UNSIGNED: pVal.at().setI(MmsValue_toInt64(value), 0, true);	break;
			case MMS_FLOAT:	  pVal.at().setR(MmsValue_toDouble(value), 0, true);			break;
			case MMS_BIT_STRING:
			{
			    string vl;
			    for(int i_vl = 0; i_vl < value->value.bitString.size; i_vl++)
				vl += ((value->value.bitString.buf[i_vl/8]>>(i_vl%8))&1) ? "1" : "0";
			    pVal.at().setS(vl, 0, true);
			    break;
			}
			case MMS_OCTET_STRING:
			{
			    string vl;
			    for(unsigned i_vl = 0; i_vl < value->value.octetString.size; i_vl++)
				vl += TSYS::strMess("%0.2x",value->value.octetString.buf[i_vl]);
			    pVal.at().setS(vl, 0, true);
			    break;
			}
			case MMS_VISIBLE_STRING: pVal.at().setS(value->value.visibleString, 0, true);		break;
			//case MMS_STRING: break;
			case MMS_UTC_TIME: pVal.at().setI(MmsValue_toUnixTimestamp(value), 0, true);		break;
			default: mess_err(cntr.nodePath().c_str(), _("Value type read %d is not implemented."), value->type);
		    }

		    MmsValue_delete(value);
		    cntr.tmDelay--;
		}
	    }
	    catch(TError err)	{ mess_err(err.cat.c_str(), "%s", err.mess.c_str()); }*/
	cntr.callSt = false;
	cntr.enRes.resRelease();
	cntr.tmGath = TSYS::curTime()-t_cnt;

	if(TSYS::taskEndRun()) break;
	TSYS::taskSleep(cntr.period(), (cntr.period()?0:TSYS::cron(cntr.cron())));
    }

    cntr.prcSt = false;

    return NULL;
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

TMdPrm::~TMdPrm( )
{
    nodeDelAll();
}

void TMdPrm::postEnable( int flag )
{
    TParamContr::postEnable(flag);
    if(!vlElemPresent(&p_el))   vlElemAtt(&p_el);
}

TMdContr &TMdPrm::owner( )	{ return (TMdContr&)TParamContr::owner(); }

void TMdPrm::enable( )
{
    if(enableStat())	return;
    TParamContr::enable();

    //attrPrc();

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

/*void TMdPrm::attrPrc( MmsVariableSpecification *iVal, vector<string> *iAls, const string &vid )
{
    if(!owner().con) return;

    vector<string> *als = iAls ? iAls : new vector<string>;
    MmsError error;

    string varLs = varList(), var;
    ResAlloc res(owner().nodeRes(), true);
    for(int off = 0; iVal || (var=TSYS::strLine(varLs,0,&off)).size(); )
    {
	MmsVariableSpecification* value = iVal ? iVal :
	    MmsConnection_getVariableAccessAttributes(owner().con, &error, (char*)TSYS::pathLev(var,0).c_str(), (char*)TSYS::pathLev(var,1).c_str());
	if(!value) continue;
	bool srchOK = false;
	string vName = iVal ? vid+"$"+MmsVariableSpecification_getName(value) : var;
	//>> Find for already presented attribute
	for(unsigned i_a = 0; i_a < p_el.fldSize() && !srchOK; i_a++)
	    if(TSYS::strLine(p_el.fldAt(i_a).reserve(),0) == vName) srchOK = true;
	if(srchOK) als->push_back(vName);
	else
	{
	    string aid = TSYS::strEncode(TSYS::pathLev(vName,1), TSYS::oscdID);
	    if(vlPresent(aid))
		for(int i_v = 1; true; i_v++)
		    if(!vlPresent(aid+i2s(i_v)))
		    { aid += i2s(i_v); break; }

	    int vtp = -1;
	    switch(value->type)
	    {
		case MMS_BOOLEAN: vtp = TFld::Boolean;				break;
		case MMS_INTEGER: case MMS_UNSIGNED: vtp = TFld::Integer;	break;
		case MMS_FLOAT:   vtp = TFld::Real;				break;
		case MMS_BIT_STRING: case MMS_STRING: case MMS_OCTET_STRING: case MMS_VISIBLE_STRING:
		    vtp = TFld::String;	break;
		case MMS_UTC_TIME: vtp = TFld::Integer;				break;
		case MMS_STRUCTURE:
		    for(int i_a = 0; i_a < MmsVariableSpecification_getSize(value); i_a++)
			attrPrc(MmsVariableSpecification_getChildSpecificationByIndex(value,i_a), als, vName);
		    break;
		//case MMS_ARRAY:
		default: mess_err(nodePath().c_str(), _("Value type %d is not implemented for '%s'."), value->type, vName.c_str());
	    }
	    if(vtp >= 0)
	    {
		p_el.fldAdd(new TFld(aid.c_str(),TSYS::pathLev(vName,1).c_str(),(TFld::Type)vtp,TVal::DirWrite,
		    "","","","",(vName+"\n"+i2s(value->type)).c_str()));
		als->push_back(vName);
	    }
	}
	if(iVal) break;
	MmsVariableSpecification_destroy(value);
    }

    //> Find for delete attribute
    for(unsigned i_a = 0, i_p; !iVal && i_a < p_el.fldSize(); )
    {
	for(i_p = 0; i_p < als->size(); i_p++)
	    if(TSYS::strLine(p_el.fldAt(i_a).reserve(),0) == (*als)[i_p]) break;
	if(i_p >= als->size())
	    try{ p_el.fldDel(i_a); continue; } catch(TError err) { }
	i_a++;
    }

    if(!iAls) delete als;
}*/

void TMdPrm::setEval( )
{
    vector<string> ls;
    elem().fldList(ls);
    for(unsigned i_el = 0; i_el < ls.size(); i_el++)
	vlAt(ls[i_el]).at().setS(EVAL_STR, 0, true);
}

void TMdPrm::cntrCmdProc( XMLNode *opt )
{
    //> Service commands process
    string a_path = opt->attr("path");
    if(a_path.substr(0,6) == "/serv/")	{ TParamContr::cntrCmdProc(opt); return; }

    //> Get page info
    if(opt->name() == "info")
    {
	TParamContr::cntrCmdProc(opt);
	//ctrMkNode("fld",opt,-1,"/prm/cfg/SEL_VAR",_("Variable"),RWRW__,"root",SDAQ_ID,3,"dest","select","select","/prm/cfg/SEL_VAR_lst");
	return;
    }

    //> Process command to page
    /*if(a_path == "/prm/cfg/SEL_VAR")
    {
	if(ctrChkNode(opt,"get")) opt->setText(owner().con?_("Select for append"):_("No connection"));
	if(ctrChkNode(opt,"set",RWRW__,"root",SDAQ_ID,SEC_RD))
	{
	    string vLs = varList();
	    setVarList(vLs+((vLs.size() && vLs[vLs.size()-1] != '\n')?"\n":"")+opt->text());
	}
    }*/
    else if(a_path == "/prm/cfg/SEL_VAR_lst" && ctrChkNode(opt) /*&& owner().con*/)
    {
	//MmsError mmsError;
	/*ResAlloc res(owner().nodeRes(), true);
	LinkedList nameList = MmsConnection_getDomainNames(owner().con, &mmsError);
	if(nameList)
	{
	    LinkedList eld = nameList;
	    while((eld=LinkedList_getNext(eld)) != NULL)
	    {
		LinkedList varList = MmsConnection_getDomainVariableNames(owner().con, &mmsError, (char*)eld->data);
		if(!varList) continue;
		LinkedList elv = varList;
		while((elv=LinkedList_getNext(elv)) != NULL)
		    opt->childAdd("el")->setText(string((char*)eld->data)+"/"+(char*)elv->data);
		LinkedList_destroy(varList);
	    }
	    LinkedList_destroy(nameList);
	}*/
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

    //> Send to active reserve station
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
    int vTp = atoi(TSYS::strLine(vo.fld().reserve(), 0, &off).c_str());

    /*MmsError error;
    MmsValue *value = NULL;
    switch(vTp)
    {
	case MMS_BOOLEAN: value = MmsValue_newBoolean(vl.getB());		break;
	case MMS_INTEGER:
	case MMS_UNSIGNED: value = MmsValue_newIntegerFromInt64(vl.getI());	break;
	case MMS_FLOAT: value = MmsValue_newDouble(vl.getR());			break;
	//case MMS_BIT_STRING:
	//case MMS_OCTET_STRING:
	case MMS_VISIBLE_STRING: value = MmsValue_newVisibleString((char*)vl.getS().c_str());	break;
    }

    if(value)
    {
	ResAlloc res(owner().nodeRes(), true);
	if(MmsConnection_writeVariable(owner().con,&error,(char*)TSYS::pathLev(nId,0).c_str(),(char*)TSYS::pathLev(nId,1).c_str(),value) != MMS_OK)
	{
	    vo.setS(EVAL_STR, 0, true);
	    if(owner().messLev() == TMess::Debug) mess_debug_(nodePath().c_str(),_("Write to '%s' error: %d."),nId.c_str(),error);
	}
	res.release();
	MmsValue_delete(value);
    }
    else
    {
	vo.setS(EVAL_STR, 0, true);
	if(owner().messLev() == TMess::Debug) mess_debug_(nodePath().c_str(),_("Write for type %d is not supported yet."),vTp);
    }*/
}
