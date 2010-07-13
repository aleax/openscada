
//OpenSCADA system module Archive.FSArch file: base.cpp
/***************************************************************************
 *   Copyright (C) 2003-2010 by Roman Savochenko                           *
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

#include <sys/stat.h>
#include <signal.h>
#include <string.h>
#include <string>

#include <tsys.h>
#include <resalloc.h>
#include <tmess.h>

#include "base.h"

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"FSArch"
#define MOD_NAME	"File system archivator"
#define MOD_TYPE	"Archive"
#define VER_TYPE	VER_ARH
#define VERSION		"1.4.0"
#define AUTORS		"Roman Savochenko"
#define DESCRIPTION	"The Archive module. Allow functions for messages and values arhiving to file system."
#define LICENSE		"GPL"
//*************************************************

FSArch::ModArch *FSArch::mod;

extern "C"
{
    TModule::SAt module( int n_mod )
    {
	if( n_mod==0 ) return TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE);
	return TModule::SAt("");
    }

    TModule *attach( const TModule::SAt &AtMod, const string &source )
    {
	if( AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE) ) 
	    return new FSArch::ModArch( source );
	return NULL;
    }
}

using namespace FSArch;

//*************************************************
//* FSArch::ModArch                               *
//*************************************************
ModArch::ModArch( const string &name) : prcSt(false)
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

    //> Create checking archivators timer
    struct sigevent sigev;
    memset(&sigev,0,sizeof(sigev));
    sigev.sigev_notify = SIGEV_THREAD;
    sigev.sigev_value.sival_ptr = this;
    sigev.sigev_notify_function = Task;
    sigev.sigev_notify_attributes = NULL;
    timer_create(CLOCK_REALTIME,&sigev,&tmId);
}

void ModArch::postEnable( int flag )
{
    TModule::postEnable( flag );

    if( flag&TCntrNode::NodeConnect )
    {
	//> Add self DB-fields for archives
	owner().messE().fldAdd( new TFld("A_PRMS",_("Addon parameters"),TFld::String,TFld::FullText,"10000") );
	owner().valE().fldAdd( new TFld("A_PRMS",_("Addon parameters"),TFld::String,TFld::FullText,"10000") );

	//> Pack files DB structure
	elPackfl.fldAdd( new TFld("FILE",_("File"),TFld::String,TCfg::Key,"100") );
	elPackfl.fldAdd( new TFld("BEGIN",_("Begin"),TFld::String,TFld::NoFlag,"20") );
	elPackfl.fldAdd( new TFld("END",_("End"),TFld::String,TFld::NoFlag,"20") );
	elPackfl.fldAdd( new TFld("PRM1",_("Parameter 1"),TFld::String,TFld::NoFlag,"20") );
	elPackfl.fldAdd( new TFld("PRM2",_("Parameter 2"),TFld::String,TFld::NoFlag,"20") );
	elPackfl.fldAdd( new TFld("PRM3",_("Parameter 3"),TFld::String,TFld::NoFlag,"20") );
    }
}

ModArch::~ModArch()
{
    try{ modStop(); }catch(...){}

    timer_delete(tmId);
}

string ModArch::filesDB()
{
    return SYS->workDB()+"."+modId()+"_Pack";
}

bool ModArch::filePack( const string &anm )
{
    if( anm.size() > 3 && anm.substr(anm.size()-3,3) == ".gz" ) return true;
    return false;
}

string ModArch::packArch( const string &anm, bool replace )
{
    string rez_nm = anm+".gz";

    //sighandler_t prevs = signal(SIGCHLD,SIG_DFL);
    int sysres = system((string("gzip -c \"")+anm+"\" > \""+rez_nm+"\"").c_str());
    //signal(SIGCHLD,prevs);
    if( sysres )
    {
	remove(rez_nm.c_str());
	throw TError(nodePath().c_str(),_("Compress error!"));
    }
    if( replace ) remove(anm.c_str());

    return rez_nm;
}

string ModArch::unPackArch( const string &anm, bool replace )
{
    string rez_nm = anm.substr(0,anm.size()-3);

    //sighandler_t prevs = signal(SIGCHLD,SIG_DFL);
    int sysres = system((string("gzip -cd \"")+anm+"\" > \""+rez_nm+"\"").c_str());
    //signal(SIGCHLD,prevs);
    if( sysres )
    {
	remove(rez_nm.c_str());
	throw TError(nodePath().c_str(),_("Decompress error: '%s'!"),anm.c_str());
    }
    if( replace ) remove(anm.c_str());

    return rez_nm;
}

void ModArch::load_( )
{
    //> Load parameters from command line

}

void ModArch::modStart( )
{
    //> Start interval timer for checking archivators
    struct itimerspec itval;
    itval.it_interval.tv_sec = itval.it_value.tv_sec = CHECK_ARH_PER;
    itval.it_interval.tv_nsec = itval.it_value.tv_nsec = 0;
    timer_settime(tmId, 0, &itval, NULL);
}

void ModArch::modStop( )
{
    //> Stop interval timer for periodic thread creating
    struct itimerspec itval;
    itval.it_interval.tv_sec = itval.it_interval.tv_nsec =
	itval.it_value.tv_sec = itval.it_value.tv_nsec = 0;
    timer_settime(tmId, 0, &itval, NULL);
    if( TSYS::eventWait( prcSt, false, nodePath()+"stop",5) )
	throw TError(nodePath().c_str(),_("Check archives thread is not stopped!"));
}

void ModArch::Task( union sigval obj )
{
    ModArch *arh = (ModArch *)obj.sival_ptr;
    if( arh->prcSt )  return;
    arh->prcSt = true;

    vector<string> a_list;
    //> Check message archivators
    arh->messList(a_list);
    for( int i_a = 0; i_a < a_list.size(); i_a++ )
	if( arh->messAt(a_list[i_a]).at().startStat( ) )
	    try{ arh->messAt(a_list[i_a]).at().checkArchivator(); }
	    catch(TError err)
	    {
		mess_err(err.cat.c_str(),"%s",err.mess.c_str());
		mess_err(arh->nodePath().c_str(),_("Check message archivator <%s> error."),a_list[i_a].c_str());
	    }

    //> Check value archivators
    arh->valList(a_list);
    for( int i_a = 0; i_a < a_list.size(); i_a++ )
	if( arh->valAt(a_list[i_a]).at().startStat( ) )
	    try{ arh->valAt(a_list[i_a]).at().checkArchivator(); }
	    catch(TError err)
	    {
		mess_err(err.cat.c_str(),"%s",err.mess.c_str());
		mess_err(arh->nodePath().c_str(),_("Check value archivator <%s> error."),a_list[i_a].c_str());
	    }

    //> Check to nopresent archive files
    struct stat file_stat;
    TConfig c_el(&mod->packFE());
    c_el.cfgViewAll(false);
    for( int fld_cnt=0; SYS->db().at().dataSeek(mod->filesDB(),mod->nodePath()+"Pack/",fld_cnt++,c_el); )
	if( stat(c_el.cfg("FILE").getS().c_str(),&file_stat) != 0 || (file_stat.st_mode&S_IFMT) != S_IFREG )
	{
	    if( !SYS->db().at().dataDel(mod->filesDB(),mod->nodePath()+"Pack/",c_el,true) )	break;
	    fld_cnt--;
	}

    arh->prcSt = false;
}

TMArchivator *ModArch::AMess(const string &iid, const string &idb)
{
    return new ModMArch(iid,idb,&owner().messE());
}


TVArchivator *ModArch::AVal(const string &iid, const string &idb)
{
    return new ModVArch(iid,idb,&owner().valE());
}
