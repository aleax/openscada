
//OpenSCADA system module DAQ.Comedi file: module.cpp
/***************************************************************************
 *   Copyright (C) 2012-2014 by Roman Savochenko                           *
 *   rom_as@oscada.org                                                     *
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
#include <dirent.h>

#include <terror.h>
#include <tsys.h>
#include <tmess.h>
#include <ttiparam.h>
#include <tdaqs.h>

#include "module.h"

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"Comedi"
#define MOD_NAME	_("DAQ boards by Comedi")
#define MOD_TYPE	SDAQ_ID
#define VER_TYPE	SDAQ_VER
#define MOD_VER		"0.5.0"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("ISA, PCI, PCMCIA, USB DAQ boards collection by Comedi(http://www.comedi.org).")
#define LICENSE		"GPL2"
//*************************************************

ModComedi::TTpContr *ModComedi::mod;  //Pointer for direct access to the module

extern "C"
{
    TModule::SAt module( int n_mod )
    {
	if(n_mod == 0)	return TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE);
	return TModule::SAt("");
    }

    TModule *attach( const TModule::SAt &AtMod, const string &source )
    {
	if(AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE))
	    return new ModComedi::TTpContr(source);
	return NULL;
    }
}

using namespace ModComedi;

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

void TTpContr::load_( )	{ }

void TTpContr::save_( )	{ }

void TTpContr::postEnable( int flag )
{
    TTipDAQ::postEnable(flag);

    //Controler's bd structure
    fldAdd(new TFld("PRM_BD",_("Parameteres table"),TFld::String,TFld::NoFlag,"30",""));
    fldAdd(new TFld("SCHEDULE",_("Acquisition schedule"),TFld::String,TFld::NoFlag,"100","1"));
    fldAdd(new TFld("PRIOR",_("Gather task priority"),TFld::Integer,TFld::NoFlag,"2","0","-1;99"));

    //Parameter type bd structure
    int t_prm = tpParmAdd("std","PRM_BD",_("Standard"));
    tpPrmAt(t_prm).fldAdd(new TFld("ADDR",_("Board's device address"),TFld::String,TCfg::NoVal,"100",""));
    tpPrmAt(t_prm).fldAdd(new TFld("ASYNCH_RD",_("Asynchronous read"),TFld::Boolean,TCfg::NoVal,"1","0"));
    tpPrmAt(t_prm).fldAdd(new TFld("PRMS",_("Addition parameters"),TFld::String,TFld::FullText|TCfg::NoVal,"1000"));
}

TController *TTpContr::ContrAttach( const string &name, const string &daq_db )
{
    return new TMdContr(name,daq_db,this);
}

//*************************************************
//* TMdContr                                      *
//*************************************************
TMdContr::TMdContr( string name_c, const string &daq_db, ::TElem *cfgelem) :
	::TController(name_c,daq_db,cfgelem), mPrior(cfg("PRIOR").getId()), mSched(cfg("SCHEDULE")),
	mPer(1000000000), prcSt(false), call_st(false), endRunReq(false), tm_gath(0)
{
    cfg("PRM_BD").setS("ComediPrm_"+name_c);
}

TMdContr::~TMdContr( )
{
    if(startStat()) stop();
}

string TMdContr::getStatus( )
{
    string val = TController::getStatus();

    if(startStat() && !redntUse())
    {
	if(call_st)	val += TSYS::strMess(_("Call now. "));
	if(period())	val += TSYS::strMess(_("Call by period: %s. "), tm2s(1e-3*period()).c_str());
	else val += TSYS::strMess(_("Call next by cron '%s'. "), tm2s(TSYS::cron(cron()),"%d-%m-%Y %R").c_str());
	val += TSYS::strMess(_("Spent time: %s. "), tm2s(tm_gath).c_str());
    }

    return val;
}

TParamContr *TMdContr::ParamAttach( const string &name, int type )
{
    return new TMdPrm(name,&owner().tpPrmAt(type));
}

void TMdContr::start_( )
{
    if(prcSt)	return;

    //Schedule process
    mPer = TSYS::strSepParse(cron(),1,' ').empty() ? vmax(0,(int64_t)(1e9*atof(cron().c_str()))) : 0;

    //Start the gathering data task
    SYS->taskCreate(nodePath('.',true), mPrior, TMdContr::Task, this, 10);
}

void TMdContr::stop_( )
{
    //Stop the request and calc data task
    SYS->taskDestroy(nodePath('.',true), &endRunReq);
}

void TMdContr::prmEn( const string &id, bool val )
{
    ResAlloc res(en_res, true);

    unsigned i_prm;
    for(i_prm = 0; i_prm < p_hd.size(); i_prm++)
	if(p_hd[i_prm].at().id() == id) break;

    if(val && i_prm >= p_hd.size())	p_hd.push_back(at(id));
    if(!val && i_prm < p_hd.size())	p_hd.erase(p_hd.begin()+i_prm);
}

void *TMdContr::Task( void *icntr )
{
    TMdContr &cntr = *(TMdContr*)icntr;

    cntr.endRunReq = false;

    try
    {
	while(!cntr.endRunReq)
	{
	    if(!cntr.redntUse())
	    {
		cntr.call_st = true;
		int64_t t_cnt = TSYS::curTime();

		//Update controller's data
		ResAlloc res( cntr.en_res, false );
		for(unsigned i_p = 0; i_p < cntr.p_hd.size(); i_p++) cntr.p_hd[i_p].at().getVals();
		res.release();

		//Calc acquisition process time
		cntr.tm_gath = TSYS::curTime()-t_cnt;
		cntr.call_st = false;
	    }

	    cntr.prcSt = true;

	    //Calc next work time and sleep
	    TSYS::taskSleep(cntr.period(), (cntr.period()?0:TSYS::cron(cntr.cron())));
	}
    }
    catch(TError err) { mess_err( err.cat.c_str(), err.mess.c_str() ); }

    cntr.prcSt = false;

    return NULL;
}

void TMdContr::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info")
    {
	TController::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/cntr/cfg/SCHEDULE",cfg("SCHEDULE").fld().descr(),RWRWR_,"root",SDAQ_ID,4,
	    "tp","str","dest","sel_ed","sel_list",TMess::labSecCRONsel(),"help",TMess::labSecCRON());
	return;
    }
    //Process command to page
    TController::cntrCmdProc(opt);
}


//*************************************************
//* TMdPrm                                        *
//*************************************************
TMdPrm::TMdPrm( string name, TTipParam *tp_prm ) :
    TParamContr(name,tp_prm), p_el("w_attr"), asynchRd(cfg("ASYNCH_RD").getBd()), devH(NULL), aiTm(5)
{

}

TMdPrm::~TMdPrm( )	{ nodeDelAll(); }

void TMdPrm::postEnable( int flag )
{
    TParamContr::postEnable(flag);
    if(!vlElemPresent(&p_el))   vlElemAtt(&p_el);
}

void TMdPrm::vlGet( TVal &val )
{
    if(!enableStat() || !owner().startStat())
    {
	if(val.name() == "err")
	{
	    if(!enableStat()) val.setS(_("1:Parameter is disabled."),0,true);
	    else if(!owner().startStat()) val.setS(_("2:Acquisition is stopped."),0,true);
	}
	else val.setS(EVAL_STR,0,true);
	return;
    }

    if(owner().redntUse()) return;

    ResAlloc res(dev_res, true);
    if(val.name() == "err") val.setS("0", 0, true);
    else if(!asynchRd) getVals(val.name());
}

void TMdPrm::vlSet( TVal &vo, const TVariant &vl, const TVariant &pvl )
{
    if(!enableStat()) vo.setS(EVAL_STR, 0, true);

    if(vl.isEVal() || vl == pvl) return;

    //Send to active reserve station
    if(owner().redntUse())
    {
	XMLNode req("set");
	req.setAttr("path",nodePath(0,true)+"/%2fserv%2fattr")->childAdd("el")->setAttr("id",vo.name())->setText(vl.getS());
	SYS->daq().at().rdStRequest(owner().workId(),req);
	return;
    }

    //Direct write
    ResAlloc res(dev_res, true);
    int off = 0,
	i_sd  = atoi(TSYS::strParse(vo.fld().reserve(),0,".",&off).c_str()),
	i_chnl= atoi(TSYS::strParse(vo.fld().reserve(),0,".",&off).c_str()),
	i_rng = atoi(TSYS::strParse(vo.fld().reserve(),0,".",&off).c_str());
    if(vo.name().compare(0,2,"ao") == 0)
    {
	int rez = comedi_data_write(devH, i_sd, i_chnl, i_rng, 0, vmax(0,vl.getI()));
	if(rez == -1) vo.setR(EVAL_REAL, 0, true);
    }
    else if(vo.name().compare(0,2,"do") == 0)
    {
	int rez = comedi_dio_write(devH, i_sd, i_chnl, vl.getB());
	if(rez == -1) vo.setB(EVAL_BOOL, 0, true);
    }
}

TMdContr &TMdPrm::owner( )	{ return (TMdContr&)TParamContr::owner(); }

void TMdPrm::enable()
{
    if(enableStat()) return;

    ResAlloc res(dev_res, true);
    devH = comedi_open(cfg("ADDR").getS().c_str());
    if(!devH)	throw TError(nodePath().c_str(), _("Comedi device file open: %s"), comedi_strerror(comedi_errno()));

    TParamContr::enable();

    string chnId, chnNm;
    vector<string> als;
    p_el.fldAdd(new TFld("info",_("Information"),TFld::String,TFld::NoWrite|TVal::DirRead));
    als.push_back("info");
    int nSubDev = comedi_get_n_subdevices(devH);

    //Attributes create
    for(int i_sd = 0, i_ai = 0, i_ao = 0, i_di = 0, i_do = 0, i_dio = 0; i_sd < nSubDev; i_sd++)
	switch(comedi_get_subdevice_type(devH, i_sd))
	{
	    case COMEDI_SUBD_AI:
		aiTm = atoi(modPrm("aiTm").c_str());
		for(int i_n = 0; i_n < comedi_get_n_channels(devH,i_sd); i_n++, i_ai++)
		{
		    chnId = TSYS::strMess("ai%d",i_ai); chnNm = TSYS::strMess(_("Analog input %d"),i_ai);
		    als.push_back(chnId);
		    p_el.fldAt(p_el.fldAdd(new TFld(chnId.c_str(),chnNm.c_str(),TFld::Real,TFld::NoWrite|TVal::DirRead))).
			setReserve(TSYS::strMess("%d.%d.%d",i_sd,i_n,atoi(modPrm(TSYS::strMess("rng.%d_%d",i_sd,i_n)).c_str())).c_str());
		}
		break;
	    case COMEDI_SUBD_AO:
		for(int i_n = 0; i_n < comedi_get_n_channels(devH,i_sd); i_n++, i_ao++)
		{
		    chnId = TSYS::strMess("ao%d",i_ao); chnNm = TSYS::strMess(_("Analog output %d"),i_ao);
		    als.push_back(chnId);
		    p_el.fldAt(p_el.fldAdd(new TFld(chnId.c_str(),chnNm.c_str(),TFld::Real,TVal::DirWrite))).
			setReserve(TSYS::strMess("%d.%d.%d",i_sd,i_n,atoi(modPrm(TSYS::strMess("rng.%d_%d",i_sd,i_n)).c_str())).c_str());
		}
		break;
	    case COMEDI_SUBD_DI:
		for(int i_n = 0; i_n < comedi_get_n_channels(devH,i_sd); i_n++, i_di++)
		{
		    chnId = TSYS::strMess("di%d",i_di); chnNm = TSYS::strMess(_("Digital input %d"),i_di);
		    als.push_back(chnId);
		    p_el.fldAt(p_el.fldAdd(new TFld(chnId.c_str(),chnNm.c_str(),TFld::Boolean,TFld::NoWrite|TVal::DirRead))).
			setReserve(TSYS::strMess("%d.%d.%d",i_sd,i_n,atoi(modPrm(TSYS::strMess("rng.%d_%d",i_sd,i_n)).c_str())).c_str());
		}
		break;
	    case COMEDI_SUBD_DO:
		for(int i_n = 0; i_n < comedi_get_n_channels(devH,i_sd); i_n++, i_do++)
		{
		    chnId = TSYS::strMess("do%d",i_do); chnNm = TSYS::strMess(_("Digital output %d"),i_do);
		    als.push_back(chnId);
		    p_el.fldAt(p_el.fldAdd(new TFld(chnId.c_str(),chnNm.c_str(),TFld::Boolean,TVal::DirWrite))).
			setReserve(TSYS::strMess("%d.%d.%d",i_sd,i_n,atoi(modPrm(TSYS::strMess("rng.%d_%d",i_sd,i_n)).c_str())).c_str());
		}
		break;
	    case COMEDI_SUBD_DIO:
		for(int i_n = 0; i_n < comedi_get_n_channels(devH,i_sd); i_n++, i_dio++)
		{
		    chnId = TSYS::strMess("dio%d",i_dio); chnNm = TSYS::strMess(_("Digital input-output %d"),i_dio);
		    als.push_back(chnId);
		    p_el.fldAt(p_el.fldAdd(new TFld(chnId.c_str(),chnNm.c_str(),TFld::Boolean,TVal::DirRead|TVal::DirWrite))).
			setReserve(TSYS::strMess("%d.%d.%d",i_sd,i_n,atoi(modPrm(TSYS::strMess("rng.%d_%d",i_sd,i_n)).c_str())).c_str());
		}
		break;
	    default: continue;
	}

    //Check for delete DAQ parameter's attributes
    for(int i_p = 0; i_p < (int)p_el.fldSize(); i_p++)
    {
	unsigned i_l;
	for(i_l = 0; i_l < als.size(); i_l++)
	    if(p_el.fldAt(i_p).name() == als[i_l])
		break;
	if(i_l >= als.size())
	    try{ p_el.fldDel(i_p); i_p--; }
	    catch(TError err){ mess_warning(err.cat.c_str(),err.mess.c_str()); }
    }

    owner().prmEn(id(), true);
}

void TMdPrm::disable()
{
    if(!enableStat()) return;

    owner().prmEn(id(), false);

    TParamContr::disable();

    //Set EVAL to parameter attributes
    vector<string> ls;
    elem().fldList(ls);
    for(unsigned i_el = 0; i_el < ls.size(); i_el++)
	vlAt(ls[i_el]).at().setS(EVAL_STR,0,true);

    ResAlloc res(dev_res, true);
    if(devH) comedi_close(devH);
}

void TMdPrm::getVals( const string &atr )
{
    vector<string> als;
    if(atr.empty())
    {
	if(!asynchRd) return;
	vlList(als);
    }
    else als.push_back(atr);

    ResAlloc res(dev_res,true);
    for(unsigned i_a = 0; i_a < als.size(); i_a++)
    {
	AutoHD<TVal> val = vlAt(als[i_a]);
	int off = 0,
	    i_sd  = atoi(TSYS::strParse(val.at().fld().reserve(),0,".",&off).c_str()),
	    i_chnl= atoi(TSYS::strParse(val.at().fld().reserve(),0,".",&off).c_str()),
	    i_rng = atoi(TSYS::strParse(val.at().fld().reserve(),0,".",&off).c_str());

	if(als[i_a] == "info")
	    val.at().setS(TSYS::strMess("%s (%s) 0x%06x",comedi_get_driver_name(devH),comedi_get_board_name(devH),comedi_get_version_code(devH)),0,true);
	else if(als[i_a].compare(0,2,"ai") == 0)
	{
	    lsampl_t data = 0xFFFF;
	    int rez = comedi_data_read_delayed(devH, i_sd, i_chnl, i_rng, 1, &data, (aiTm > 20) ? 0 : aiTm*1000);
	    if(rez != -1 && aiTm > 20)
	    {
		TSYS::sysSleep((float)aiTm*1e-6);
		comedi_data_read_delayed(devH, i_sd, i_chnl, i_rng, 1, &data, 0);
	    }
	    comedi_range *rng = comedi_get_range(devH, i_sd, i_chnl, i_rng);
	    int maxVal = comedi_get_maxdata(devH, i_sd, i_chnl);
	    double dVal = vmax(rng->min,vmin(rng->max,rng->min+((double)data/(double)maxVal)*(rng->max-rng->min)));
	    val.at().setR((rez == -1 || isnan(dVal)) ? EVAL_REAL : dVal, 0, true);
	}
	else if(als[i_a].compare(0,2,"di") == 0 || als[i_a].compare(0,2,"do") == 0)
	{
	    unsigned int bit = EVAL_BOOL;
	    comedi_dio_read(devH, i_sd, i_chnl, &bit);
	    val.at().setB(bit, 0, true);
	}
    }
}

string TMdPrm::modPrm( const string &prm )
{
    XMLNode prmNd;
    try
    {
	prmNd.load(cfg("PRMS").getS());
	string sobj = TSYS::strParse(prm,0,":"), sa = TSYS::strParse(prm,1,":");
	if(!sa.size())  return prmNd.attr(prm);

	//Internal node
	for(unsigned i_n = 0; i_n < prmNd.childSize(); i_n++)
	    if(prmNd.childGet(i_n)->name() == sobj)
		return prmNd.childGet(i_n)->attr(sa);
    } catch(...){ }

    return "";
}

void TMdPrm::setModPrm( const string &prm, const string &val )
{
    XMLNode prmNd("ModCfg");
    try { prmNd.load(cfg("PRMS").getS()); } catch(...){ }

    if(modPrm(prm) != val) modif();
    string sobj = TSYS::strParse(prm,0,":"), sa = TSYS::strParse(prm,1,":");
    if(!sa.size()) prmNd.setAttr(prm,val);

    //Internal node
    else
    {
	unsigned i_n;
	for(i_n = 0; i_n < prmNd.childSize(); i_n++)
	    if(prmNd.childGet(i_n)->name() == sobj)
	    { prmNd.childGet(i_n)->setAttr(sa,val); break; }
	if(i_n >= prmNd.childSize())
	    prmNd.childAdd(sobj)->setAttr(sa,val);
    }

    cfg("PRMS").setS(prmNd.save(XMLNode::BrAllPast));
}

void TMdPrm::load_( )	{ TParamContr::load_(); }

void TMdPrm::save_( )	{ TParamContr::save_(); }

void TMdPrm::cntrCmdProc( XMLNode *opt )
{
    //Service commands process
    string a_path = opt->attr("path");
    if(a_path.substr(0,6) == "/serv/")	{ TParamContr::cntrCmdProc(opt); return; }

    //Get page info
    if(opt->name() == "info")
    {
	TParamContr::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/cfg/ADDR",cfg("ADDR").fld().descr(),enableStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,
	    2,"dest","sel_ed","select","/prm/cfg/devLst");
	ctrRemoveNode(opt,"/prm/cfg/PRMS");
	// Configuration page: ranges
	ResAlloc res(dev_res,true);
	comedi_t *tmpDevH = comedi_open(cfg("ADDR").getS().c_str());
	if(tmpDevH && ctrMkNode("area",opt,-1,"/cfg",_("Configuration")))
	{
	    ctrMkNode("fld",opt,-1,"/cfg/aiTm",_("Analog input settle timeout (us)"),enableStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,3,"tp","dec","min","0","max","1000000");

	    int cfgIts = 0;
	    for(int iSDev = 0; iSDev < comedi_get_n_subdevices(tmpDevH); iSDev++)
	    {
		bool rChnSpec = (comedi_range_is_chan_specific(tmpDevH,iSDev)==1);
		int nRanges = rChnSpec ? 0 : comedi_get_n_ranges(tmpDevH,iSDev,0);
		for(int i_cn = 0; i_cn < comedi_get_n_channels(tmpDevH,iSDev); i_cn++)
		{
		    if(rChnSpec) nRanges = comedi_get_n_ranges(tmpDevH,iSDev,i_cn);
		    if(nRanges <= 1) continue;
		    string rngIdLs, rngNmLs;
		    for(int iRng = 0; iRng < nRanges; iRng++)
		    {
			rngIdLs += i2s(iRng)+";";
			comedi_range *rng = comedi_get_range(tmpDevH, iSDev, (rChnSpec?i_cn:0), iRng);
			rngNmLs += TSYS::strMess("[%g, %g]",rng->min, rng->max)+";";
		    }
		    ctrMkNode("fld",opt,-1,TSYS::strMess("/cfg/chn%d_%d",iSDev,i_cn).c_str(),TSYS::strMess(_("Channel %d.%d range"),iSDev,i_cn).c_str(),
			enableStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,4,"dest","select","tp","dec","sel_id",rngIdLs.c_str(),"sel_list",rngNmLs.c_str());
		    cfgIts++;
		}
	    }
	    if(!cfgIts) ctrRemoveNode(opt,"/cfg");
	}
	if(tmpDevH) comedi_close(tmpDevH);
	return;
    }

    //Process command to page
    if(a_path == "/prm/cfg/ADDR" && ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR) && enableStat())
	throw TError(nodePath().c_str(),"Parameter is enabled.");
    else if(a_path == "/prm/cfg/devLst" && ctrChkNode(opt))
    {
	DIR *IdDir = opendir("/dev");
	if(IdDir)
	{
	    dirent scan_dirent, *scan_rez = NULL;
	    while(readdir_r(IdDir,&scan_dirent,&scan_rez) == 0 && scan_rez)
	    {
		string nFile = string("/dev/")+scan_rez->d_name;
		if(!strcmp("..",scan_rez->d_name) || !strcmp(".",scan_rez->d_name) ||
		    strncmp("comedi",scan_rez->d_name,6) || access(nFile.c_str(),F_OK|R_OK) != 0) continue;
		opt->childAdd("el")->setText(nFile);
	    }
	    closedir(IdDir);
	}
    }
    else if(a_path == "/cfg/aiTm")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(i2s(atoi(modPrm("aiTm").c_str())));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	setModPrm("aiTm",opt->text());
    }
    else if(a_path.compare(0,8,"/cfg/chn") == 0)
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(i2s(atoi(modPrm("rng."+a_path.substr(8)).c_str())));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	setModPrm("rng."+a_path.substr(8),opt->text());
    }
    else TParamContr::cntrCmdProc(opt);
}

void TMdPrm::vlArchMake( TVal &val )
{
    TParamContr::vlArchMake(val);

    if(val.arch().freeStat()) return;
    if(asynchRd)
    {
	val.arch().at().setSrcMode(TVArchive::PassiveAttr);
	val.arch().at().setPeriod(owner().period() ? (int64_t)owner().period()/1000 : 1000000);
    }
    else
    {
	val.arch().at().setSrcMode(TVArchive::ActiveAttr);
	val.arch().at().setPeriod(SYS->archive().at().valPeriod()*1000);
    }
    val.arch().at().setHardGrid(true);
    val.arch().at().setHighResTm(true);
}
