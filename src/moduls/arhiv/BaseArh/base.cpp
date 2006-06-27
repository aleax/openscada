
//OpenSCADA system module Archive.BaseArh file: base.cpp
/***************************************************************************
 *   Copyright (C) 2003-2006 by Roman Savochenko                           *
 *   rom_as@fromru.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
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
#include <getopt.h>
#include <string>

#include <tsys.h>
#include <resalloc.h>
#include <tmess.h>

#include "base.h"

//============ Modul info! =====================================================
#define MOD_ID      "BaseArh"
#define MOD_NAME    "Base archivator"
#define MOD_TYPE    "Archive"
#define VER_TYPE    VER_ARH
#define VERSION     "0.9.0"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "The Archive module. Allow base functions of message and value arhiving to file system."
#define LICENSE     "GPL"
//==============================================================================

BaseArch::ModArch *BaseArch::mod;

extern "C"
{ 
    TModule::SAt module( int n_mod )
    {
	TModule::SAt AtMod;
	
	if(n_mod==0) 
	{
	    AtMod.id	= MOD_ID;
	    AtMod.type  = MOD_TYPE;
	    AtMod.t_ver = VER_TYPE;
	}
	else
	    AtMod.id	= "";
	return( AtMod );    
    }
    
    TModule *attach( const TModule::SAt &AtMod, const string &source )
    {
	BaseArch::ModArch *self_addr = NULL;
	
	if( AtMod.id == MOD_ID && AtMod.type == MOD_TYPE && AtMod.t_ver == VER_TYPE ) 
	    self_addr = BaseArch::mod = new BaseArch::ModArch( source );
	    
	return ( self_addr );
    }    
}

using namespace BaseArch;

//==============================================================================
//========================== BaseArch::ModArch =================================
//==============================================================================
ModArch::ModArch( const string &name) : prc_st(false), chk_fDB(0)
{
    mId 	= MOD_ID;
    mName	= MOD_NAME;
    mType  	= MOD_TYPE;
    mVers      	= VERSION;
    mAutor    	= AUTORS;
    mDescr  	= DESCRIPTION;
    mLicense   	= LICENSE;
    mSource    	= name;
    
    //- Create checking archivators timer -
    struct sigevent sigev;
    sigev.sigev_notify = SIGEV_THREAD;
    sigev.sigev_value.sival_ptr = this;
    sigev.sigev_notify_function = Task;
    sigev.sigev_notify_attributes = NULL;
    timer_create(CLOCK_REALTIME,&sigev,&tmId);
}

void ModArch::postEnable( )
{
    TModule::postEnable( );
    
    //Add self DB-fields for messages archive
    if( !owner().messE().fldPresent("BaseArhXML") )
	owner().messE().fldAdd( new TFld("BaseArhXML",I18N("XML archive files"),TFld::Bool,FLD_NOFLG,"1","false") );
    if( !owner().messE().fldPresent("BaseArhMSize") )
	owner().messE().fldAdd( new TFld("BaseArhMSize",I18N("Maximum archive file size (kB)"),TFld::Dec,FLD_NOFLG,"4","300") );
    if( !owner().messE().fldPresent("BaseArhNFiles") )
	owner().messE().fldAdd( new TFld("BaseArhNFiles",I18N("Maximum files number"),TFld::Dec,FLD_NOFLG,"3","10") );
    if( !owner().messE().fldPresent("BaseArhTmSize") )
	owner().messE().fldAdd( new TFld("BaseArhTmSize",I18N("File's time size (days)"),TFld::Dec,FLD_NOFLG,"3","30") );
    if( !owner().messE().fldPresent("BaseArhPackTm") )	
	owner().messE().fldAdd( new TFld("BaseArhPackTm",I18N("Pack files timeout (min)"),TFld::Dec,FLD_NOFLG,"2","10") );
    if( !owner().messE().fldPresent("BaseArhTm") )
	owner().messE().fldAdd( new TFld("BaseArhTm",I18N("Check archives period (min)"),TFld::Dec,FLD_NOFLG,"2","60") );
	
    //Add self DB-fields for value archive
    if( !owner().valE().fldPresent("BaseArhTmSize") )
	owner().valE().fldAdd( new TFld("BaseArhTmSize",I18N("File's time size (hours)"),TFld::Real,FLD_NOFLG,"4.2","24") );
    if( !owner().valE().fldPresent("BaseArhNFiles") )
    	owner().valE().fldAdd( new TFld("BaseArhNFiles",I18N("Maximum files number"),TFld::Dec,FLD_NOFLG,"3","10") );
    if( !owner().valE().fldPresent("BaseArhRound") )
	owner().valE().fldAdd( new TFld("BaseArhRound",I18N("Numberic values rounding (%)"),TFld::Real,FLD_NOFLG,"2.2","0.1","0;50") );
    if( !owner().valE().fldPresent("BaseArhPackTm") )	
	owner().valE().fldAdd( new TFld("BaseArhPackTm",I18N("Pack files timeout (min)"),TFld::Dec,FLD_NOFLG,"2","10") );    	
    if( !owner().valE().fldPresent("BaseArhTm") )
	owner().valE().fldAdd( new TFld("BaseArhTm",I18N("Check archives period (min)"),TFld::Dec,FLD_NOFLG,"2","60") );
	
    //Pack files DB structure
    el_packfl.fldAdd( new TFld("FILE",I18N("File"),TFld::String,FLD_KEY,"100") );
    el_packfl.fldAdd( new TFld("BEGIN",I18N("Begin"),TFld::String,FLD_NOFLG,"20") );
    el_packfl.fldAdd( new TFld("END",I18N("End"),TFld::String,FLD_NOFLG,"20") );
    el_packfl.fldAdd( new TFld("PRM1",I18N("Parameter 1"),TFld::String,0,"20") );
    el_packfl.fldAdd( new TFld("PRM2",I18N("Parameter 2"),TFld::String,0,"20") );
    el_packfl.fldAdd( new TFld("PRM3",I18N("Parameter 3"),TFld::String,0,"20") );	
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
    if( anm.size() > 3 && anm.substr(anm.size()-3,3) == ".gz" )
    return true;
    return false;
}
		
string ModArch::packArch( const string &anm, bool replace )
{
    string rez_nm = anm+".gz";
    if( system((string("gzip -c \"")+anm+"\" > \""+rez_nm+"\"").c_str()) )
	throw TError(nodePath().c_str(),mod->I18N("Compress error!"));
    if(replace) remove(anm.c_str());
	    
    return rez_nm;
}
					
string ModArch::unPackArch( const string &anm, bool replace )
{
    string rez_nm = anm.substr(0,anm.size()-3);
    if( system((string("gzip -cd \"")+anm+"\" > \""+rez_nm+"\"").c_str()) )
        throw TError(nodePath().c_str(),mod->I18N("Decompress error!"));
    if(replace) remove(anm.c_str());
							    
    return rez_nm;
}

string ModArch::optDescr( )
{
    char buf[STR_BUF_LEN];

    snprintf(buf,sizeof(buf),I18N(
	"======================= The module <%s:%s> options =======================\n"
	"---------- Parameters of the module section <%s> in config file ----------\n\n"),
	MOD_TYPE,MOD_ID,nodePath().c_str());

    return buf;
}

void ModArch::modLoad()
{
    //========== Load parameters from command line ============
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{"help"    ,0,NULL,'h'},
	{NULL      ,0,NULL,0  }
    };

    optind=opterr=0;
    do
    {
	next_opt=getopt_long(SYS->argc,(char * const *)SYS->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': fprintf(stdout,optDescr().c_str()); break;
	    case -1 : break;
	}
    } while(next_opt != -1);
}

void ModArch::modStart( )
{
    //- Start interval timer for checking archivators -
    struct itimerspec itval;
    itval.it_interval.tv_sec = itval.it_value.tv_sec = CHECK_ARH_PER;
    itval.it_interval.tv_nsec = itval.it_value.tv_nsec = 0;
    timer_settime(tmId, 0, &itval, NULL);
}

void ModArch::modStop( )
{
    //- Stop interval timer for periodic thread creating -
    struct itimerspec itval;
    itval.it_interval.tv_sec = itval.it_interval.tv_nsec =
	itval.it_value.tv_sec = itval.it_value.tv_nsec = 0;
    timer_settime(tmId, 0, &itval, NULL);
    if( TSYS::eventWait( prc_st, false, nodePath()+"stop",5) )
        throw TError(nodePath().c_str(),Mess->I18N("Check archives thread no stoped!"));
}

void ModArch::Task(union sigval obj)
{
    ModArch *arh = (ModArch *)obj.sival_ptr;
    if( arh->prc_st )  return;
    arh->prc_st = true;

    vector<string> a_list;
    //- Check message archivators -
    arh->messList(a_list);
    for( int i_a = 0; i_a < a_list.size(); i_a++ )
	try{ arh->messAt(a_list[i_a]).at().checkArchivator(); }
	catch(TError err)
	{ 
	    Mess->put(err.cat.c_str(),TMess::Error,"%s",err.mess.c_str());
	    Mess->put(arh->nodePath().c_str(),TMess::Error,mod->I18N("Check message archivator <%s> error."),a_list[i_a].c_str());
	}
	
    //- Check value archivators -
    arh->valList(a_list);
    for( int i_a = 0; i_a < a_list.size(); i_a++ )
	try{ arh->valAt(a_list[i_a]).at().checkArchivator(); }
	catch(TError err)
	{ 
	    Mess->put(err.cat.c_str(),TMess::Error,"%s",err.mess.c_str());
	    Mess->put(arh->nodePath().c_str(),TMess::Error,mod->I18N("Check value archivator <%s> error."),a_list[i_a].c_str()); 
	}

    //- Check to nopresent archive files -
    if( arh->chk_fDB++ > 24*60 )
    {
	arh->chk_fDB = 0;
	TConfig c_el(&mod->packFE());
	int fld_cnt=0;
	while( SYS->db().at().dataSeek(mod->filesDB(),mod->nodePath()+"Pack/",fld_cnt++,c_el) )
	{
	    struct stat file_stat;
	    if( stat(c_el.cfg("FILE").getS().c_str(),&file_stat) != 0 || (file_stat.st_mode&S_IFMT) != S_IFREG )
	    {	
		SYS->db().at().dataDel(mod->filesDB(),mod->nodePath()+"Pack/",c_el);
		fld_cnt--;
	    }	
    	    c_el.cfg("FILE").setS("");
	}	
    }

    arh->prc_st = false;
}

TMArchivator *ModArch::AMess(const string &iid, const string &idb)
{
    return new ModMArch(iid,idb,&owner().messE());
}


TVArchivator *ModArch::AVal(const string &iid, const string &idb)
{
    return new ModVArch(iid,idb,&owner().valE());
}

void ModArch::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    if( cmd==TCntrNode::Info )
    {
	TTipArchivator::cntrCmd_( a_path, opt, cmd );       //Call parent

	ctrMkNode("fld",opt,-1,a_path.c_str(),"/help/g_help",Mess->I18N("Options help"),0440,0,0,3,"tp","str","cols","90","rows","5");
    }
    else if( cmd==TCntrNode::Get )
    {
	if( a_path == "/help/g_help" )	ctrSetS( opt, optDescr() );       
	else TTipArchivator::cntrCmd_( a_path, opt, cmd );
    }
    else if( cmd==TCntrNode::Set )
	TTipArchivator::cntrCmd_( a_path, opt, cmd );
}
