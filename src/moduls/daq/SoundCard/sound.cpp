
//OpenSCADA system module DAQ.SoundCard file: sound.cpp
/***************************************************************************
 *   Copyright (C) 2008-2009 by Roman Savochenko                           *
 *   rom_as@fromru.com                                                     *
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

#include <signal.h>
#include <portaudio.h>

#include <tsys.h>
#include <ttiparam.h>

#include <sound.h>

//*************************************************
//* Modul info!                                   *
#define MOD_ID      "SoundCard"
#define MOD_NAME    "Sound card"
#define MOD_TYPE    "DAQ"
#define VER_TYPE    VER_CNTR
#define VERSION     "0.6.0"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "Allow access to sound card data."
#define LICENSE     "GPL"
//*************************************************

SoundCard::TTpContr *SoundCard::mod;

extern "C"
{
    TModule::SAt module( int n_mod )
    {
	if( n_mod==0 )	return TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE);
	return TModule::SAt("");
    }

    TModule *attach( const TModule::SAt &AtMod, const string &source )
    {
	if( AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE) )
	    return new SoundCard::TTpContr( source );
	return NULL;
    }
}

using namespace SoundCard;

//*************************************************
//* TTpContr                                      *
//*************************************************
TTpContr::TTpContr( string name )
{
    mId		= MOD_ID;
    mName	= MOD_NAME;
    mType	= MOD_TYPE;
    mVers	= VERSION;
    mAutor	= AUTORS;
    mDescr	= DESCRIPTION;
    mLicense	= LICENSE;
    mSource	= name;

    mod		= this;
}

TTpContr::~TTpContr()
{
    PaError err = Pa_Terminate();
    if( err != paNoError ) mess_err(nodePath().c_str(),"Pa_Terminate: %s",Pa_GetErrorText(err));
}

void TTpContr::postEnable( int flag )
{
    TTipDAQ::postEnable( flag );

    PaError err = Pa_Initialize();
    if( err != paNoError ) mess_err(nodePath().c_str(),"Pa_Initialize: %s",Pa_GetErrorText(err));

    //> Controler's bd structure
    fldAdd( new TFld("PRM_BD",_("Parameters' table"),TFld::String,0,"30") );
    fldAdd( new TFld("CARD",_("Card device"),TFld::String,0,"20","<default>") );
    fldAdd( new TFld("SMPL_RATE",_("Card sample rate (Hz)"),TFld::Integer,0,"5","8000") );
    fldAdd( new TFld("SMPL_TYPE",_("Card sample type"),TFld::Integer,TFld::Selected,"5",TSYS::int2str(paFloat32).c_str(),
	(TSYS::int2str(paFloat32)+";"+TSYS::int2str(paInt32)+";"+TSYS::int2str(paInt16)).c_str(),_("Float 32;Int 32;Int 16")) );

    //> Parameter type bd structure
    int t_prm = tpParmAdd("std","PRM_BD",_("Standard"));
    tpPrmAt(t_prm).fldAdd( new TFld("CHANNEL",_("Channel"),TFld::Integer,TCfg::NoVal,"2","0","0;100") );
}

TController *TTpContr::ContrAttach( const string &name, const string &daq_db )
{
    return new TMdContr(name,daq_db,this);
}

//*************************************************
//* TMdContr                                      *
//*************************************************
TMdContr::TMdContr( string name_c, const string &daq_db, ::TElem *cfgelem) :
    TController(name_c,daq_db,cfgelem), prcSt(false), endrunReq(false), pEl("w_attr"), stream(NULL), numChan(0), smplSize(0), acqSize(0),
    mCard(cfg("CARD").getSd()), mSmplRate(cfg("SMPL_RATE").getId()), mSmplType(cfg("SMPL_TYPE").getId())
{
    cfg("PRM_BD").setS("SoundCard_"+name_c);

    pEl.fldAdd( new TFld("val",_("Value"),((mSmplType==paFloat32)?TFld::Real:TFld::Integer),TFld::NoWrite,"",
					  ((mSmplType==paFloat32)?TSYS::real2str(EVAL_REAL).c_str():TSYS::int2str(EVAL_INT).c_str())) );
}

TMdContr::~TMdContr()
{

}

TTpContr &TMdContr::owner( )	{ return (TTpContr&)TController::owner(); }

string TMdContr::getStatus( )
{
    string val = TController::getStatus( );
    if( !startStat( ) ) val += TSYS::strMess(_("Allowed %d input channels"),channelAllow());
    else if( !redntUse( ) ) val+= TSYS::strMess(_("Gathering from %d channels. Recieved %.2f MB."),numChan,acqSize);

    return val;
}

int TMdContr::channelAllow( )
{
    int chann = 0;
    if( mCard == "<default>" && Pa_GetDefaultInputDevice() >= 0 )
	chann = Pa_GetDeviceInfo(Pa_GetDefaultInputDevice())->maxInputChannels;
    else
	for( int i_d = 0; i_d < Pa_GetDeviceCount(); i_d++ )
	    if( mCard == Pa_GetDeviceInfo(i_d)->name )
	    {
		chann = Pa_GetDeviceInfo(i_d)->maxInputChannels;
		break;
	    }

    return chann;
}

TParamContr *TMdContr::ParamAttach( const string &name, int type )
{
    return new TMdPrm(name,&owner().tpPrmAt(type));
}

void TMdContr::load_( )
{
    TController::load_( );
}

void TMdContr::save_( )
{
    TController::save_();
}

void TMdContr::start_( )
{
    if( startStat( ) )	return;

    endrunReq = false;
    numChan = 0;
    acqSize = 0;

    //> Former proccess parameters list
    vector<string> list_p;
    list(list_p);
    for( int i_prm = 0; i_prm < list_p.size(); i_prm++ )
	if( at(list_p[i_prm]).at().enableStat() )
	{
	    prmEn( list_p[i_prm], true );
	    numChan = vmax(numChan,at(list_p[i_prm]).at().iCnl()+1);
	}

    wTm = TSYS::curTime( );
    sdTm = 1000000/mSmplRate;
    switch(mSmplType)
    {
	case paFloat32:	smplSize = sizeof(float);	break;
	case paInt32:	smplSize = sizeof(int32_t);	break;
	case paInt16:	smplSize = sizeof(int16_t);	break;
	default:	smplSize = 0;
    }

    //> Start input stream
    PaStreamParameters iParam;
    iParam.device = -1;

    if( mCard == "<default>" )	iParam.device = Pa_GetDefaultInputDevice();
    else
	for( int i_d = 0; i_d < Pa_GetDeviceCount(); i_d++ )
	    if( Pa_GetDeviceInfo(i_d)->maxInputChannels && mCard == Pa_GetDeviceInfo(i_d)->name )
	    { iParam.device = i_d; break; }
    if( iParam.device < 0 ) throw TError(nodePath().c_str(),_("Selected device '%s' is error or default device no allow."),mCard.c_str());
    if( !numChan ) throw TError(nodePath().c_str(),_("No one channel is configured for acquisition."));
    if( !smplSize ) throw TError(nodePath().c_str(),_("Sample type set is error."));

    iParam.channelCount = numChan;
    iParam.sampleFormat = mSmplType;
    iParam.suggestedLatency = Pa_GetDeviceInfo(iParam.device)->defaultLowInputLatency;
    iParam.hostApiSpecificStreamInfo = NULL;

    PaError err = Pa_OpenStream( &stream, &iParam, NULL, mSmplRate, mSmplRate/2, paClipOff, recordCallback, this );
    if( err != paNoError ) throw TError(nodePath().c_str(),"Pa_OpenStream: %s",Pa_GetErrorText(err));

    err = Pa_StartStream( stream );
    if( err != paNoError ) throw TError(nodePath().c_str(),"Pa_StartStream: %s",Pa_GetErrorText(err));
}

void TMdContr::stop_( )
{
    if( !startStat( ) )	return;

    //> Close and stop stream
    endrunReq = true;
    if( TSYS::eventWait(prcSt,false,nodePath()+"stream_stop",5) )
	throw TError(nodePath().c_str(),_("Sound input stream is not stopped!"));
    PaError err = Pa_CloseStream( stream );
    if( err != paNoError ) throw TError(nodePath().c_str(),"Pa_CloseStream: %s",Pa_GetErrorText(err));

    //> Clear proccess parameters list
    pHd.clear();
}

void TMdContr::prmEn( const string &id, bool val )
{
    int i_prm;

    ResAlloc res(nodeRes( ),true);
    for( i_prm = 0; i_prm < pHd.size(); i_prm++)
	if( pHd[i_prm].at().id() == id ) break;

    if( val && i_prm >= pHd.size() )	pHd.push_back(at(id));
    if( !val && i_prm < pHd.size() )	pHd.erase(pHd.begin()+i_prm);
}

int TMdContr::recordCallback( const void *iBuf, void *oBuf, unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData )
{
    TMdContr &cntr = *(TMdContr*)userData;
    cntr.prcSt = true;
    const char *bptr = (const char*)iBuf;

    if( cntr.redntUse( ) ) return cntr.endrunReq;

    //> Check for current time correction
    long long cTm = TSYS::curTime();
    if( fabs((cntr.wTm+framesPerBuffer*cntr.sdTm)-cTm) > 1e6 )
    {
	cntr.wTm = cTm - framesPerBuffer*cntr.sdTm;
	mess_warning(cntr.nodePath().c_str(),_("Sound card's counter run from system time is corrected."));
    }

    //> Input buffer process
    ResAlloc res(cntr.nodeRes(),false);
    for( int i_p = 0; i_p < cntr.pHd.size(); i_p++ )
    {
	const char *rptr = (const char*)iBuf;
	int  chn = cntr.pHd[i_p].at().iCnl();
	AutoHD<TVal> val = cntr.pHd[i_p].at().vlAt("val");
	bool archAllow = (!val.at().arch().freeStat() && val.at().arch().at().srcMode() == TVArchive::PassiveAttr);
	switch(cntr.mSmplType)
	{
	    case paFloat32:
		if( archAllow )
		    for( int i_s = 0; i_s < framesPerBuffer; i_s++, rptr += cntr.numChan*cntr.smplSize )
			val.at().arch().at().setR(*(float*)(rptr+(chn*cntr.smplSize)),cntr.wTm+(cntr.sdTm*i_s));
		val.at().setR(*(float*)(bptr+(framesPerBuffer-1)*cntr.numChan*cntr.smplSize+(chn*cntr.smplSize)),cntr.wTm+(framesPerBuffer-1)*cntr.sdTm,true);
		break;
	    case paInt32:
		if( archAllow )
		    for( int i_s = 0; i_s < framesPerBuffer; i_s++, rptr += cntr.numChan*cntr.smplSize )
			val.at().arch().at().setR(*(int32_t*)(rptr+(chn*cntr.smplSize)),cntr.wTm+(cntr.sdTm*i_s));
		val.at().setI(*(int32_t*)(bptr+(framesPerBuffer-1)*cntr.numChan*cntr.smplSize+(chn*cntr.smplSize)),cntr.wTm+(framesPerBuffer-1)*cntr.sdTm,true);
		break;
	    case paInt16:
		if( archAllow )
		    for( int i_s = 0; i_s < framesPerBuffer; i_s++, rptr += cntr.numChan*cntr.smplSize )
			val.at().arch().at().setR(*(int16_t*)(rptr+(chn*cntr.smplSize)),cntr.wTm+(cntr.sdTm*i_s));
		val.at().setI(*(int16_t*)(bptr+(framesPerBuffer-1)*cntr.numChan*cntr.smplSize+(chn*cntr.smplSize)),cntr.wTm+(framesPerBuffer-1)*cntr.sdTm,true);
		break;
	}
    }

    cntr.wTm += framesPerBuffer*cntr.sdTm;

    cntr.acqSize += (float)(framesPerBuffer*cntr.smplSize*cntr.numChan)/1048576;
//    if( timeInfo ) printf("Samle time %f (%f).\n",timeInfo->currentTime,timeInfo->inputBufferAdcTime);

    cntr.prcSt = false;

    return cntr.endrunReq;
}

void TMdContr::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if( opt->name() == "info" )
    {
	TController::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/cntr/cfg/CARD",cfg("CARD").fld().descr(),0664,"root","root",3,"tp","str","dest","select","select","/cntr/cfg/lst_SMPL_RATE");
	ctrMkNode("fld",opt,-1,"/cntr/cfg/SMPL_RATE",cfg("SMPL_RATE").fld().descr(),0664,"root","root",
	    3,"tp","str","dest","sel_ed","sel_list","8000;16000;22050;44100;48000;96000");
	return;
    }

    //> Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/cntr/cfg/lst_SMPL_RATE" && ctrChkNode(opt) )
    {
	for( int i_d = 0; i_d < Pa_GetDeviceCount(); i_d++ )
	    if( Pa_GetDeviceInfo(i_d)->maxInputChannels )
		opt->childAdd("el")->setText(Pa_GetDeviceInfo(i_d)->name);
    }
    else TController::cntrCmdProc(opt);
}


//*************************************************
//* TMdPrm                                        *
//*************************************************
TMdPrm::TMdPrm( string name, TTipParam *tp_prm ) :
    TParamContr(name,tp_prm), mCnl(cfg("CHANNEL").getId())
{

}

TMdPrm::~TMdPrm( )
{
    nodeDelAll();
}

void TMdPrm::postEnable( int flag )
{
    TParamContr::postEnable( flag );

    if( !vlElemPresent(&owner().prmEL()) ) vlElemAtt(&owner().prmEL());
}

TMdContr &TMdPrm::owner( )	{ return (TMdContr&)TParamContr::owner(); }

void TMdPrm::load_( )
{
    TParamContr::load_();
}

void TMdPrm::enable()
{
    if( enableStat() )	return;

    TParamContr::enable();

    //> Set to process
    if(owner().startStat())	owner().prmEn( id(), true );
}

void TMdPrm::disable()
{
    if( !enableStat() )	return;

    TParamContr::disable();

    //> Set to process
    if(owner().startStat())	owner().prmEn( id(), false );
}

void TMdPrm::vlArchMake( TVal &val )
{
    if( val.arch().freeStat() ) return;
    val.arch().at().setSrcMode(TVArchive::PassiveAttr,val.arch().at().srcData());
    val.arch().at().setPeriod(1000000/owner().mSmplRate);
    val.arch().at().setHardGrid( true );
    val.arch().at().setHighResTm( true );
}

void TMdPrm::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if( opt->name() == "info" )
    {
	TParamContr::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/cfg/CHANNEL",cfg("CHANNEL").fld().descr(),0664,"root","root",3,"tp","str","dest","select","select","/prm/cfg/lst_CHANNEL");
	return;
    }

    //> Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/prm/cfg/lst_CHANNEL" && ctrChkNode(opt) )
    {
	for( int i_c = 0; i_c < owner().channelAllow( ); i_c++ )
	    opt->childAdd("el")->setText(TSYS::int2str(i_c));
    }
    else TParamContr::cntrCmdProc(opt);
}
