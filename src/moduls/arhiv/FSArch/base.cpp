
//OpenSCADA system module Archive.FSArch file: base.cpp
/***************************************************************************
 *   Copyright (C) 2003-2016 by Roman Savochenko, <rom_as@oscada.org>      *
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
#define MOD_VER		"2.8.9"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("The archiver module. Provides functions for messages and values archiving to file system.")
#define LICENSE		"GPL2"
//*************************************************

FSArch::ModArch *FSArch::mod;

extern "C"
{
#ifdef MOD_INCL
    TModule::SAt arh_FSArch_module( int n_mod )
#else
    TModule::SAt module( int n_mod )
#endif
    {
	if(n_mod == 0) return TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE);
	return TModule::SAt("");
    }

#ifdef MOD_INCL
    TModule *arh_FSArch_attach( const TModule::SAt &AtMod, const string &source )
#else
    TModule *attach( const TModule::SAt &AtMod, const string &source )
#endif
    {
	if(AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE)) return new FSArch::ModArch(source);
	return NULL;
    }
}

using namespace FSArch;

//*************************************************
//* FSArch::ModArch                               *
//*************************************************
ModArch::ModArch( const string &name) : TTypeArchivator(MOD_ID), noArchLimit(false)
{
    mod = this;

    modInfoMainSet(MOD_NAME, MOD_TYPE, MOD_VER, AUTHORS, DESCRIPTION, LICENSE, name);
}

void ModArch::postEnable( int flag )
{
    TModule::postEnable(flag);

    if(flag&TCntrNode::NodeConnect) {
	//Add self DB-fields for archives
	owner().messE().fldAdd(new TFld("A_PRMS","Addon parameters",TFld::String,TFld::FullText,"10000"));
	owner().valE().fldAdd(new TFld("A_PRMS","Addon parameters",TFld::String,TFld::FullText,"10000"));

	//Pack files DB structure
	elPackfl.fldAdd(new TFld("FILE","File",TFld::String,TCfg::Key,"100"));
	elPackfl.fldAdd(new TFld("BEGIN","Begin",TFld::String,TFld::NoFlag,"20"));
	elPackfl.fldAdd(new TFld("END","End",TFld::String,TFld::NoFlag,"20"));
	elPackfl.fldAdd(new TFld("PRM1","Parameter 1",TFld::String,TFld::NoFlag,"20"));
	elPackfl.fldAdd(new TFld("PRM2","Parameter 2",TFld::String,TFld::NoFlag,"20"));
	elPackfl.fldAdd(new TFld("PRM3","Parameter 3",TFld::String,TFld::NoFlag,"20"));
    }
}

ModArch::~ModArch( )
{
    try { modStop(); } catch(...) { }
}

string ModArch::filesDB( )	{ return SYS->workDB()+"."+modId()+"_Pack"; }

bool ModArch::filePack( const string &anm )	{ return (anm.size() > 3 && anm.substr(anm.size()-3,3) == ".gz"); }

string ModArch::packArch( const string &anm, bool replace )
{
    string rez_nm = anm + ".gz";

    //sighandler_t prevs = signal(SIGCHLD,SIG_DFL);
    int sysres = system((string("gzip -c \"")+anm+"\" > \""+rez_nm+"\"").c_str());
    //signal(SIGCHLD,prevs);
    if(sysres) {
	remove(rez_nm.c_str());
	throw err_sys(_("Compress error!"));
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
    if(sysres) {
	remove(rez_nm.c_str());
	throw err_sys(_("Decompress error: '%s'!"), anm.c_str());
    }
    if(replace) remove(anm.c_str());

    return rez_nm;
}

string ModArch::optDescr( )
{
    return TSYS::strMess(_(
	"======================= The module <%s:%s> options =======================\n"
	"    --noArchLimit        Disable archives limit to the file number. Use for see archives mode, not work.\n"
	"\n"),MOD_TYPE,MOD_ID);
}

void ModArch::load_( )
{
    //Load parameters from command line
    string argCom, argVl;
    for(int argPos = 0; (argCom=SYS->getCmdOpt(argPos,&argVl)).size(); )
        if(argCom == "h" || argCom == "help")	fprintf(stdout, "%s", optDescr().c_str());
	else if(argCom == "noArchLimit")	noArchLimit = true;
}

void ModArch::perSYSCall( unsigned int cnt )
{
    try {
	if(cnt%60) return;

	vector<string> a_list;
	time_t end_tm = time(NULL)+STD_WAIT_TM;

	//Check message archivators
	messList(a_list);
	for(unsigned i_a = 0; time(NULL) < end_tm && i_a < a_list.size(); i_a++)
	    if(messAt(a_list[i_a]).at().startStat())
		try{ messAt(a_list[i_a]).at().checkArchivator(); }
		catch(TError &err) {
		    mess_err(err.cat.c_str(), "%s", err.mess.c_str());
		    mess_sys(TMess::Error, _("Check message archivator '%s' error."), a_list[i_a].c_str());
		}

	//Check value archivators
	valList(a_list);
	for(unsigned i_a = 0; time(NULL) < end_tm && i_a < a_list.size(); i_a++)
	    if(valAt(a_list[i_a]).at().startStat())
		try{ valAt(a_list[i_a]).at().checkArchivator(); }
		catch(TError &err) {
		    mess_err(err.cat.c_str(), "%s", err.mess.c_str());
		    mess_sys(TMess::Error, _("Check value archivator '%s' error."), a_list[i_a].c_str());
		}

	//Check to nopresent archive files
	struct stat file_stat;
	vector<vector<string> > full;
	TConfig cEl(&mod->packFE());
	cEl.cfgViewAll(false);
	for(int fldCnt = 0; time(NULL) < end_tm && SYS->db().at().dataSeek(mod->filesDB(),mod->nodePath()+"Pack",fldCnt++,cEl,false,&full); )
	    if(stat(cEl.cfg("FILE").getS().c_str(),&file_stat) != 0 || (file_stat.st_mode&S_IFMT) != S_IFREG) {
		if(!SYS->db().at().dataDel(mod->filesDB(),mod->nodePath()+"Pack",cEl,true,false,true))	break;
		if(full.empty()) fldCnt--;
	    }
    } catch(TError &err) { mess_sys(TMess::Error, "%s", err.mess.c_str()); }
}

TMArchivator *ModArch::AMess( const string &iid, const string &idb )	{ return new ModMArch(iid,idb,&owner().messE()); }

TVArchivator *ModArch::AVal( const string &iid, const string &idb )	{ return new ModVArch(iid,idb,&owner().valE()); }
