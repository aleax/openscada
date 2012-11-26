
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
#define MOD_NAME	_("File system archivator")
#define MOD_TYPE	SARH_ID
#define VER_TYPE	SARH_VER
#define MOD_VER		"1.5.1"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("The Archive module. Allow functions for messages and values archiving to file system.")
#define LICENSE		"GPL2"
//*************************************************

FSArch::ModArch *FSArch::mod;

extern "C"
{
#ifdef MOD_INCL
    TModule::SAt arh_FSArch_module(int n_mod)
#else
    TModule::SAt module( int n_mod )
#endif
    {
	if( n_mod==0 ) return TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE);
	return TModule::SAt("");
    }

#ifdef MOD_INCL
    TModule *arh_FSArch_attach( const TModule::SAt &AtMod, const string &source )
#else
    TModule *attach( const TModule::SAt &AtMod, const string &source )
#endif
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
ModArch::ModArch( const string &name) : TTipArchivator(MOD_ID), noArchLimit(false), copyErrValFiles(false)
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

void ModArch::postEnable( int flag )
{
    TModule::postEnable(flag);

    if(flag&TCntrNode::NodeConnect)
    {
	//> Add self DB-fields for archives
	owner().messE().fldAdd(new TFld("A_PRMS","Addon parameters",TFld::String,TFld::FullText,"10000"));
	owner().valE().fldAdd(new TFld("A_PRMS","Addon parameters",TFld::String,TFld::FullText,"10000"));

	//> Pack files DB structure
	elPackfl.fldAdd(new TFld("FILE","File",TFld::String,TCfg::Key,"100"));
	elPackfl.fldAdd(new TFld("BEGIN","Begin",TFld::String,TFld::NoFlag,"20"));
	elPackfl.fldAdd(new TFld("END","End",TFld::String,TFld::NoFlag,"20"));
	elPackfl.fldAdd(new TFld("PRM1","Parameter 1",TFld::String,TFld::NoFlag,"20"));
	elPackfl.fldAdd(new TFld("PRM2","Parameter 2",TFld::String,TFld::NoFlag,"20"));
	elPackfl.fldAdd(new TFld("PRM3","Parameter 3",TFld::String,TFld::NoFlag,"20"));
    }
}

ModArch::~ModArch()
{
    try{ modStop(); }catch(...){}
}

string ModArch::filesDB()
{
    return SYS->workDB()+"."+modId()+"_Pack";
}

bool ModArch::filePack( const string &anm )
{
    if(anm.size() > 3 && anm.substr(anm.size()-3,3) == ".gz") return true;
    return false;
}

string ModArch::packArch( const string &anm, bool replace )
{
    string rez_nm = anm+".gz";

    //sighandler_t prevs = signal(SIGCHLD,SIG_DFL);
    int sysres = system((string("gzip -c \"")+anm+"\" > \""+rez_nm+"\"").c_str());
    //signal(SIGCHLD,prevs);
    if(sysres)
    {
	remove(rez_nm.c_str());
	throw TError(nodePath().c_str(),_("Compress error!"));
    }
    if(replace) remove(anm.c_str());

    return rez_nm;
}

string ModArch::unPackArch( const string &anm, bool replace )
{
    string rez_nm = anm.substr(0,anm.size()-3);

    //sighandler_t prevs = signal(SIGCHLD,SIG_DFL);
    int sysres = system((string("gzip -cd \"")+anm+"\" > \""+rez_nm+"\"").c_str());
    //signal(SIGCHLD,prevs);
    if(sysres)
    {
	remove(rez_nm.c_str());
	throw TError(nodePath().c_str(),_("Decompress error: '%s'!"),anm.c_str());
    }
    if(replace) remove(anm.c_str());

    return rez_nm;
}

string ModArch::optDescr( )
{
    return TSYS::strMess(_(
	"======================= The module <%s:%s> options =======================\n"
	"    --noArchLimit        Disable archives limit to file number. Use for see archives mode, not work.\n"
	"    --copyErrValFiles    Copy sourced error value archive's files before it restore.\n"
	"                         Used for debug value archive's errors and correct restore.\n"
        "\n"),MOD_TYPE,MOD_ID);
}

void ModArch::load_( )
{
    //> Load parameters from command line
    string argCom, argVl;
    for(int argPos = 0; (argCom=SYS->getCmdOpt(argPos,&argVl)).size(); )
        if(argCom == "h" || argCom == "help")	fprintf(stdout, "%s", optDescr().c_str());
	else if(argCom == "noArchLimit")	noArchLimit = true;
	else if(argCom == "copyErrValFiles")	copyErrValFiles = true;
}

void ModArch::perSYSCall( unsigned int cnt )
{
    try
    {
	if(cnt%60) return;

	vector<string> a_list;
	time_t end_tm = time(NULL)+STD_WAIT_TM;

	//> Check message archivators
	messList(a_list);
	for(unsigned i_a = 0; time(NULL) < end_tm && i_a < a_list.size(); i_a++)
	    if(messAt(a_list[i_a]).at().startStat())
		try{ messAt(a_list[i_a]).at().checkArchivator(); }
		catch(TError err)
		{
		    mess_err(err.cat.c_str(),"%s",err.mess.c_str());
		    mess_err(nodePath().c_str(),_("Check message archivator '%s' error."),a_list[i_a].c_str());
		}

	//> Check value archivators
	valList(a_list);
	for(unsigned i_a = 0; time(NULL) < end_tm && i_a < a_list.size(); i_a++)
	    if(valAt(a_list[i_a]).at().startStat())
		try{ valAt(a_list[i_a]).at().checkArchivator(); }
		catch(TError err)
		{
		    mess_err(err.cat.c_str(),"%s",err.mess.c_str());
		    mess_err(nodePath().c_str(),_("Check value archivator '%s' error."),a_list[i_a].c_str());
		}

	//> Check to nopresent archive files
	struct stat file_stat;
	TConfig c_el(&mod->packFE());
	c_el.cfgViewAll(false);
	for(int fld_cnt = 0; time(NULL) < end_tm && SYS->db().at().dataSeek(mod->filesDB(),mod->nodePath()+"Pack",fld_cnt++,c_el); )
	    if(stat(c_el.cfg("FILE").getS().c_str(),&file_stat) != 0 || (file_stat.st_mode&S_IFMT) != S_IFREG)
	    {
		if(!SYS->db().at().dataDel(mod->filesDB(),mod->nodePath()+"Pack",c_el,true))	break;
		fld_cnt--;
	    }
    }
    catch(TError err) { mess_err(nodePath().c_str(),"%s",err.mess.c_str()); }
}

TMArchivator *ModArch::AMess(const string &iid, const string &idb)
{
    return new ModMArch(iid,idb,&owner().messE());
}


TVArchivator *ModArch::AVal(const string &iid, const string &idb)
{
    return new ModVArch(iid,idb,&owner().valE());
}
