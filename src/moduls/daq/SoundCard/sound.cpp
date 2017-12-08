
//OpenSCADA system module DAQ.SoundCard file: sound.cpp
/***************************************************************************
 *   Copyright (C) 2008-2017 by Roman Savochenko, <rom_as@oscada.org>      *
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

#include <syscall.h>
#include <sys/resource.h>
#include <signal.h>
#include <errno.h>
#include <portaudio.h>

#include <tsys.h>
#include <ttypeparam.h>

#include <sound.h>

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"SoundCard"
#define MOD_NAME	_("Sound card")
#define MOD_TYPE	SDAQ_ID
#define VER_TYPE	SDAQ_VER
#define MOD_VER		"0.8.0"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("Provides an access to the sound card.")
#define LICENSE		"GPL2"
//*************************************************

SoundCard::TTpContr *SoundCard::mod;

extern "C"
{
    TModule::SAt module( int n_mod )
    {
	if(n_mod == 0)	return TModule::SAt(MOD_ID, MOD_TYPE, VER_TYPE);
	return TModule::SAt("");
    }

    TModule *attach( const TModule::SAt &AtMod, const string &source )
    {
	if(AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE)) return new SoundCard::TTpContr(source);
	return NULL;
    }
}

using namespace SoundCard;

//*************************************************
//* TTpContr                                      *
//*************************************************
TTpContr::TTpContr( string name ) : TTypeDAQ(MOD_ID)
{
    mod = this;

    modInfoMainSet(MOD_NAME, MOD_TYPE, MOD_VER, AUTHORS, DESCRIPTION, LICENSE, name);
}

TTpContr::~TTpContr()
{
    PaError err = Pa_Terminate();
    if(err != paNoError) mess_err(nodePath().c_str(),"Pa_Terminate: %s",Pa_GetErrorText(err));
}

void TTpContr::postEnable( int flag )
{
    TTypeDAQ::postEnable(flag);

    PaError err = Pa_Initialize();
    if(err != paNoError) mess_err(nodePath().c_str(),"Pa_Initialize: %s",Pa_GetErrorText(err));

    //Controler's bd structure
    fldAdd(new TFld("PRM_BD",_("Parameters' table"),TFld::String,0,"30"));
    fldAdd(new TFld("CARD",_("Card device"),TFld::String,0,"100","<default>"));
    fldAdd(new TFld("SMPL_RATE",_("Card sample rate (Hz)"),TFld::Integer,0,"5","8000","1;200000"));
    fldAdd(new TFld("SMPL_TYPE",_("Card sample type"),TFld::Integer,TFld::Selected,"5",i2s(paFloat32).c_str(),
	TSYS::strMess("%d;%d;%d",paFloat32,paInt32,paInt16).c_str(),_("Float 32;Int 32;Int 16")));
    fldAdd(new TFld("PRIOR",_("Gather task priority"),TFld::Integer,TFld::NoFlag,"2","0","-1;199"));

    //Parameter type bd structure
    int t_prm = tpParmAdd("std","PRM_BD",_("Standard"));
    tpPrmAt(t_prm).fldAdd(new TFld("CHANNEL",_("Channel"),TFld::Integer,TCfg::NoVal,"2","0","0;100"));
}

TController *TTpContr::ContrAttach( const string &name, const string &daq_db ) { return new TMdContr(name,daq_db,this); }

//*************************************************
//* TMdContr                                      *
//*************************************************
TMdContr::TMdContr( string name_c, const string &daq_db, ::TElem *cfgelem) :
    TController(name_c,daq_db,cfgelem), pEl("w_attr"),
    mSmplRate(cfg("SMPL_RATE").getId()), mSmplType(cfg("SMPL_TYPE").getId()), mPrior(cfg("PRIOR").getId()),
    prcSt(false), endrunReq(false), firstCall(false), numChan(0), smplSize(0), stream(NULL), acqSize(0), cntCor(0)
{
    for(int iC = 0; iC < sizeof(curTm)/sizeof(time_t); iC++) curTm[iC] = 0;

    cfg("PRM_BD").setS("SoundCard_"+name_c);

    pEl.fldAdd(new TFld("val",_("Value"),((mSmplType==paFloat32)?TFld::Real:TFld::Integer),TFld::NoWrite,"",
					 ((mSmplType==paFloat32)?r2s(EVAL_REAL).c_str():ll2s(EVAL_INT).c_str())));
}

TMdContr::~TMdContr( )
{
    nodeDelAll();
}

TTpContr &TMdContr::owner( ) const	{ return (TTpContr&)TController::owner(); }

string TMdContr::getStatus( )
{
    string val = TController::getStatus();
    if(!startStat()) val += TSYS::strMess(_("Allowed %d input channels"),channelAllow());
    else if(!redntUse())
	val += TSYS::strMess(_("Gathering from %d channels, recieved %.2g MB, samplerate corrections %g and adjusted value %d."),
	    numChan, acqSize, cntCor, sRt);

    return val;
}

int TMdContr::channelAllow( )
{
    int chann = 0;
    if(card() == "<default>" && Pa_GetDefaultInputDevice() >= 0)
	chann = fmin(sizeof(curTm)/sizeof(time_t), Pa_GetDeviceInfo(Pa_GetDefaultInputDevice())->maxInputChannels);
    else
	for(int i_d = 0; i_d < Pa_GetDeviceCount(); i_d++)
	    if(card() == Pa_GetDeviceInfo(i_d)->name) {
		chann = fmin(sizeof(curTm)/sizeof(time_t), Pa_GetDeviceInfo(i_d)->maxInputChannels);
		break;
	    }

    return chann;
}

string TMdContr::sampleRates( )
{
    string rez;
    static unsigned standardSampleRates[] = { 8000, 9600, 11025, 12000, 16000, 22050, 24000, 32000, 44100, 48000, 88200, 96000, 192000, 0 };
    PaStreamParameters iParam;
    iParam.device = -1;

    if(card() == "<default>")	iParam.device = Pa_GetDefaultInputDevice();
    else
	for(int i_d = 0; i_d < Pa_GetDeviceCount(); i_d++)
	    if(Pa_GetDeviceInfo(i_d)->maxInputChannels && card() == Pa_GetDeviceInfo(i_d)->name)
	    { iParam.device = i_d; break; }
    if(iParam.device < 0) return rez;
    iParam.channelCount = fmin(sizeof(curTm)/sizeof(time_t), Pa_GetDeviceInfo(iParam.device)->maxInputChannels);
    iParam.sampleFormat = mSmplType;
    iParam.suggestedLatency = 0;
    iParam.hostApiSpecificStreamInfo = NULL;

    for(int i_s = 0; standardSampleRates[i_s]; i_s++)
	if(startStat() || Pa_IsFormatSupported(&iParam, NULL, standardSampleRates[i_s]) == paFormatIsSupported)
	    rez += i2s(standardSampleRates[i_s])+";";

    return rez;
}

TParamContr *TMdContr::ParamAttach( const string &name, int type ) { return new TMdPrm(name,&owner().tpPrmAt(type)); }

void TMdContr::start_( )
{
    if(startStat())	return;

    endrunReq = false;
    numChan = 0;
    acqSize = cntCor = 0;

    //Former proccess parameters list
    vector<string> list_p;
    list(list_p);
    for(unsigned iPrm = 0; iPrm < list_p.size(); iPrm++)
	if(at(list_p[iPrm]).at().enableStat()) {
	    prmEn(list_p[iPrm], true);
	    numChan = vmax(numChan,at(list_p[iPrm]).at().iCnl()+1);
	}

    wTm = TSYS::curTime( );
    sRt = mSmplRate;
    switch(mSmplType) {
	case paFloat32:	smplSize = sizeof(float);	break;
	case paInt32:	smplSize = sizeof(int32_t);	break;
	case paInt16:	smplSize = sizeof(int16_t);	break;
	default:	smplSize = 0;
    }

    //Start input stream
    PaStreamParameters iParam;
    iParam.device = -1;

    if(card() == "<default>") iParam.device = Pa_GetDefaultInputDevice();
    else
	for(int i_d = 0; i_d < Pa_GetDeviceCount(); i_d++)
	    if(Pa_GetDeviceInfo(i_d)->maxInputChannels && card() == Pa_GetDeviceInfo(i_d)->name)
	    { iParam.device = i_d; break; }
    if(iParam.device < 0) throw TError(nodePath().c_str(),_("Selected device '%s' is error or default device no allow."),card().c_str());
    if(!numChan) throw TError(nodePath().c_str(),_("No one channel is configured for acquisition."));
    if(!smplSize) throw TError(nodePath().c_str(),_("Sample type set is error."));

    iParam.channelCount = numChan;
    iParam.sampleFormat = mSmplType;
    iParam.suggestedLatency = 0.2; //Pa_GetDeviceInfo(iParam.device)->defaultLowInputLatency;
    iParam.hostApiSpecificStreamInfo = NULL;

    PaError err = Pa_OpenStream(&stream, &iParam, NULL, mSmplRate, 0/*mSmplRate/2*/, paClipOff, recordCallback, this);
    if(err != paNoError) throw TError(nodePath().c_str(),"Pa_OpenStream: %s",Pa_GetErrorText(err));

    corTm = SYS->sysTm();
    firstCall = true;
    err = Pa_StartStream(stream);
    if(err != paNoError) throw TError(nodePath().c_str(),"Pa_StartStream: %s",Pa_GetErrorText(err));
}

void TMdContr::stop_( )
{
    if(!startStat())	return;

    //Close and stop stream
    endrunReq = true;
    if(TSYS::eventWait(prcSt,false,nodePath()+"stream_stop",5))
	throw TError(nodePath().c_str(),_("Sound input stream is not stopped!"));
    PaError err = Pa_CloseStream(stream);
    if(err != paNoError) throw TError(nodePath().c_str(),"Pa_CloseStream: %s",Pa_GetErrorText(err));

    //Clear proccess parameters list
    pHd.clear();
}

void TMdContr::prmEn( const string &id, bool val )
{
    ResAlloc res(nodeRes(), true);

    unsigned iPrm;
    for(iPrm = 0; iPrm < pHd.size(); iPrm++)
	if(pHd[iPrm].at().id() == id) break;

    if(val && iPrm >= pHd.size()) pHd.push_back(at(id));
    if(!val && iPrm < pHd.size()) pHd.erase(pHd.begin()+iPrm);
}

int TMdContr::recordCallback( const void *iBuf, void *oBuf, unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData )
{
    TMdContr &cntr = *(TMdContr*)userData;
    cntr.prcSt = true;
    const char *bptr = (const char*)iBuf;

    if(cntr.redntUse()) { cntr.prcSt = false; return cntr.endrunReq; }

    //Set priority for call task
    if(cntr.firstCall) {
	int policy = SCHED_OTHER;
	struct sched_param prior;

	pthread_getschedparam(pthread_self(), &policy, &prior);
#if __GLIBC_PREREQ(2,4)
	if(cntr.prior() < 0)	policy = SCHED_BATCH;
#endif
	if(cntr.prior() > 0)	{ policy = SCHED_RR; prior.sched_priority = cntr.prior(); }
	if(pthread_setschedparam(pthread_self(), SCHED_FIFO, &prior) == EPERM)
	    setpriority(PRIO_PROCESS, syscall(SYS_gettid), -cntr.prior()/5);
	cntr.firstCall = false;
    }

    //Check for current time correction
    int64_t tSz = (1000000ll*framesPerBuffer)/cntr.sRt;
     if(abs(SYS->sysTm()-cntr.wTm/1000000) >= 3) {
	time_t corDt = SYS->sysTm()-cntr.wTm/1000000;
	corDt -= (corDt>=0?2:-2);
	if(cntr.messLev() == TMess::Debug)
	    mess_debug_(cntr.nodePath().c_str(), _("CallBack: Sample rate correction from %g to %g for fix error %ds on time %ds."),
		cntr.sRt, cntr.sRt/vmax(0.9,vmin(1.1,((float)corDt/(SYS->sysTm()-cntr.corTm)+1))), corDt, SYS->sysTm()-cntr.corTm);
	cntr.sRt = cntr.sRt / vmax(0.9,vmin(1.1,((float)corDt/(SYS->sysTm()-cntr.corTm)+1)));
	cntr.corTm = SYS->sysTm();
	cntr.wTm = 1000000ll*cntr.corTm;
	cntr.cntCor++;
    }

    //Input buffer process
    ResAlloc res(cntr.nodeRes(), false);
    for(unsigned iP = 0; iP < cntr.pHd.size(); iP++) {
	int  chn = cntr.pHd[iP].at().iCnl();
	AutoHD<TVal> val = cntr.pHd[iP].at().vlAt("val");
	AutoHD<TVArchive> arch = val.at().arch();
	bool archAllow = (!arch.freeStat() && arch.at().srcMode() == TVArchive::PassiveAttr);
	switch(cntr.mSmplType) {
	    case paFloat32:
		if(archAllow)
		    for(int64_t iT = 0; iT < tSz; iT += arch.at().period())
			arch.at().setR(*(float*)(bptr+cntr.smplSize*((iT*framesPerBuffer/tSz)*cntr.numChan+chn)), cntr.wTm+iT);
		if(SYS->sysTm() > cntr.curTm[chn]) {
		    val.at().setR(*(float*)(bptr+cntr.smplSize*((framesPerBuffer-1)*cntr.numChan+chn)),cntr.wTm+(1000000ll*(framesPerBuffer-1))/cntr.sRt, true);
		    cntr.curTm[chn] = SYS->sysTm();
		}
		break;
	    case paInt32:
		if(archAllow)
		    for(int64_t iT = 0; iT < tSz; iT += arch.at().period())
			arch.at().setI(*(int32_t*)(bptr+cntr.smplSize*((iT*framesPerBuffer/tSz)*cntr.numChan+chn)), cntr.wTm+iT);
		if(SYS->sysTm() > cntr.curTm[chn]) {
		    val.at().setI(*(int32_t*)(bptr+cntr.smplSize*((framesPerBuffer-1)*cntr.numChan+chn)),cntr.wTm+(1000000ll*(framesPerBuffer-1))/cntr.sRt,true);
		    cntr.curTm[chn] = SYS->sysTm();
		}
		break;
	    case paInt16:
		if(archAllow)
		    for(int64_t iT = 0; iT < tSz; iT += arch.at().period())
			arch.at().setI(*(int16_t*)(bptr+cntr.smplSize*((iT*framesPerBuffer/tSz)*cntr.numChan+chn)), cntr.wTm+iT);
		if(SYS->sysTm() > cntr.curTm[chn]) {
		    val.at().setI(*(int16_t*)(bptr+cntr.smplSize*((framesPerBuffer-1)*cntr.numChan+chn)),cntr.wTm+(1000000ll*(framesPerBuffer-1))/cntr.sRt,true);
		    cntr.curTm[chn] = SYS->sysTm();
		}
		break;
	}
    }

    cntr.wTm += tSz;

    cntr.acqSize += (float)(framesPerBuffer*cntr.smplSize*cntr.numChan)/1048576;

    cntr.prcSt = false;

    return cntr.endrunReq;
}

bool TMdContr::cfgChange( TCfg &co, const TVariant &pc )
{
    TController::cfgChange(co, pc);

    if(startStat() && (co.name() == "CARD" || co.name() == "SMPL_RATE" || co.name() == "SMPL_TYPE")) stop();

    return true;
}

void TMdContr::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TController::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/cntr/cfg/CARD",EVAL_STR,startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,
	    2,"dest","select","select","/cntr/cfg/lsDEVS");
	ctrMkNode("fld",opt,-1,"/cntr/cfg/SMPL_RATE",EVAL_STR,startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,
	    2,"dest","sel_ed","sel_list",sampleRates().c_str());
	ctrMkNode("fld",opt,-1,"/cntr/cfg/SMPL_TYPE",EVAL_STR,startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID);
	ctrMkNode("fld",opt,-1,"/cntr/cfg/PRIOR",EVAL_STR,startStat()?R_R_R_:RWRWR_,"root",SDAQ_ID,1,"help",TMess::labTaskPrior());
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/cntr/cfg/lsDEVS" && ctrChkNode(opt)) {
	for(int i_d = 0; i_d < Pa_GetDeviceCount(); i_d++)
	    if(Pa_GetDeviceInfo(i_d)->maxInputChannels)
		opt->childAdd("el")->setText(Pa_GetDeviceInfo(i_d)->name);
    }
    else TController::cntrCmdProc(opt);
}


//*************************************************
//* TMdPrm                                        *
//*************************************************
TMdPrm::TMdPrm( string name, TTypeParam *tp_prm ) : TParamContr(name,tp_prm), mCnl(cfg("CHANNEL").getId())
{

}

TMdPrm::~TMdPrm( )
{
    disable();
    nodeDelAll();
}

void TMdPrm::postEnable( int flag )
{
    TParamContr::postEnable(flag);

    if(!vlElemPresent(&owner().prmEL())) vlElemAtt(&owner().prmEL());
}

TMdContr &TMdPrm::owner( ) const	{ return (TMdContr&)TParamContr::owner(); }

void TMdPrm::enable( )
{
    if(enableStat())	return;

    TParamContr::enable();

    //Set to process
    //if(owner().startStat())
    owner().prmEn(id(), true);
}

void TMdPrm::disable( )
{
    if(!enableStat())	return;

    TParamContr::disable();

    //Set to process
    //if(owner().startStat())
    owner().prmEn(id(), false);
}

void TMdPrm::vlArchMake( TVal &val )
{
    TParamContr::vlArchMake(val);

    if(val.arch().freeStat()) return;
    val.arch().at().setSrcMode(TVArchive::PassiveAttr);
    val.arch().at().setPeriod(1000000/owner().mSmplRate);
    val.arch().at().setHardGrid(true);
    val.arch().at().setHighResTm(true);
    val.arch().at().setCombMode(TVArchive::LastVal);
}

void TMdPrm::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TParamContr::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/cfg/CHANNEL",EVAL_STR,RWRWR_,"root",SDAQ_ID,2,"dest","select","select","/prm/cfg/lst_CHANNEL");
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/cfg/lst_CHANNEL" && ctrChkNode(opt))
	for(int i_c = 0; i_c < owner().channelAllow(); i_c++)
	    opt->childAdd("el")->setText(i2s(i_c));
    else TParamContr::cntrCmdProc(opt);
}
