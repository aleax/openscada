
//OpenSCADA system module DAQ.SoundCard file: sound.cpp
/***************************************************************************
 *   Copyright (C) 2008 by Roman Savochenko                                *
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
#include <alsa/asoundlib.h>

#include <tsys.h>
#include <ttiparam.h>

#include "sound.h"

//*************************************************
//* Modul info!                                   *
#define MOD_ID      "SoundCard"
#define MOD_NAME    "Sound card"
#define MOD_TYPE    "DAQ"
#define VER_TYPE    VER_CNTR
#define VERSION     "0.0.1"
#define AUTORS      "??"
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

    /*printf("ALSA library version: %s\n",SND_LIB_VERSION_STR);

    printf("\nPCM stream types:\n");
    for( int val = 0; val <= SND_PCM_STREAM_LAST; val++ )
    printf("  %s\n",snd_pcm_stream_name((snd_pcm_stream_t)val));

    printf("\nPCM access types:\n");
    for( int val = 0; val <= SND_PCM_ACCESS_LAST; val++ )
    printf("  %s\n",snd_pcm_access_name((snd_pcm_access_t)val));

    printf("\nPCM formats:\n");
    for( int val = 0; val <= SND_PCM_FORMAT_LAST; val++ )
	if( snd_pcm_format_name((snd_pcm_format_t)val) != NULL )
	    printf("  %s (%s)\n", snd_pcm_format_name((snd_pcm_format_t)val), snd_pcm_format_description((snd_pcm_format_t)val));

    printf("\nPCM subformats:\n");
    for( int val = 0; val <= SND_PCM_SUBFORMAT_LAST; val++ )
	printf("  %s (%s)\n", snd_pcm_subformat_name((snd_pcm_subformat_t)val), snd_pcm_subformat_description((snd_pcm_subformat_t)val));

    printf("\nPCM states:\n");
    for( int val = 0; val <= SND_PCM_STATE_LAST; val++ )
	printf("  %s\n", snd_pcm_state_name((snd_pcm_state_t)val));*/
}

TTpContr::~TTpContr()
{

}

void TTpContr::postEnable( int flag )
{
    TModule::postEnable( flag );

    //- Controler's bd structure -
    fldAdd( new TFld("PRM_BD",_("Parameters' table"),TFld::String,0,"30") );
    fldAdd( new TFld("CARD",_("Card device"),TFld::String,0,"20","/dev/") );

    //- Parameter type bd structure -
    int t_prm = tpParmAdd("std","PRM_BD",_("Standard"));
    tpPrmAt(t_prm).fldAdd( new TFld("INPUT",_("Input"),TFld::String,TCfg::NoVal,"20","") );
}

TController *TTpContr::ContrAttach( const string &name, const string &daq_db )
{
    return new TMdContr(name,daq_db,this);
}

//*************************************************
//* TMdContr                                      *
//*************************************************
TMdContr::TMdContr( string name_c, const string &daq_db, ::TElem *cfgelem) :
    TController(name_c,daq_db,cfgelem), prc_st(false), endrun_req(false), m_card(cfg("CARD").getSd())
{
    cfg("PRM_BD").setS("SoundCard_"+name_c);
}

TMdContr::~TMdContr()
{

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
    //- Create sound interrupt process task -
    pthread_attr_t pthr_attr;
    pthread_attr_init(&pthr_attr);
    struct sched_param prior;
    pthread_attr_setschedpolicy(&pthr_attr,SCHED_OTHER);
    pthread_create(&procPthr,&pthr_attr,Task,this);
    pthread_attr_destroy(&pthr_attr);
    if( TSYS::eventWait(prc_st, true, nodePath()+"task_start",5) )
	throw TError(nodePath().c_str(),_("Sound input task is not started!"));
}

void TMdContr::stop_( )
{
    //- Close sound interrupt process task -
    if( prc_st )
    {
	endrun_req = true;
	pthread_kill( procPthr, SIGALRM );
	if( TSYS::eventWait(prc_st,false,nodePath()+"task_stop",5) )
	    throw TError(nodePath().c_str(),_("Sound input task is not stopped!"));
	pthread_join( procPthr, NULL );
    }
}

void *TMdContr::Task( void *param )
{
    long long work_tm, last_tm = 0;
    struct timespec get_tm;
    TMdContr &cntr = *(TMdContr *)param;

#if OSC_DEBUG >= 2
    mess_debug(cntr.nodePath().c_str(),_("Thread <%u> is started. TID: %ld"),pthread_self(),(long int)syscall(224));
#endif

    cntr.endrun_req = false;
    cntr.prc_st = true;

    while( !cntr.endrun_req )
    {
	//- Sound card acquisition procedure -
	//????
	printf("TEST 00\n");

	usleep(STD_WAIT_DELAY*1000);
    }

    cntr.prc_st = false;

    return NULL;
}

//*************************************************
//* TMdPrm                                        *
//*************************************************
TMdPrm::TMdPrm( string name, TTipParam *tp_prm ) :
    TParamContr(name,tp_prm), p_el("w_attr"), m_in(cfg("INPUT").getSd())
{

}

TMdPrm::~TMdPrm( )
{
    nodeDelAll();
}

void TMdPrm::postEnable( int flag )
{
    TParamContr::postEnable( flag );

    if( !vlElemPresent(&p_el) ) vlElemAtt(&p_el);

    p_el.fldAdd( new TFld( "val", _("Value"), TFld::Real, TFld::NoWrite, "", TSYS::real2str(EVAL_REAL).c_str(), "-1:1" ) );
}

void TMdPrm::load_( )
{
    TParamContr::load_();
}

void TMdPrm::vlArchMake( TVal &val )
{
    if( val.arch().freeStat() ) return;
    val.arch().at().setSrcMode(TVArchive::PassiveAttr,val.arch().at().srcData());
    val.arch().at().setPeriod(1000000);
    val.arch().at().setHardGrid( true );
    val.arch().at().setHighResTm( true );
}
