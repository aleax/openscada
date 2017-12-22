
//OpenSCADA system file: tmodschedul.cpp
/***************************************************************************
 *   Copyright (C) 2003-2017 by Roman Savochenko, <rom_as@oscada.org>      *
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
#include <sys/types.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include <dirent.h>
#include <stddef.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#include <string>

#include "tsys.h"
#include "tmodschedul.h"

using namespace OSCADA;

//*************************************************
//* TModSchedul                                   *
//*************************************************
TModSchedul::TModSchedul( ) : TSubSYS(SMSH_ID,_("Modules Scheduler"),false), mAllow("*"), mPer(10), schM(true)
{

}

TModSchedul::~TModSchedul( )
{

}

void TModSchedul::preDisable( int flag )
{
    //All opened modules closing
    MtxAlloc res(schM, true);
    for(unsigned iSh = 0; iSh < schHD.size(); iSh++)
	while(schHD[iSh].hd && schHD[iSh].use.size()) {
	    owner().at(TSYS::strSepParse(schHD[iSh].use[0],0,'.')).at().modDel(TSYS::strSepParse(schHD[iSh].use[0],1,'.'));
	    schHD[iSh].use.erase(schHD[iSh].use.begin());
	}

    //All shared libraries detach
    for(int iSh = 0; iSh < (int)schHD.size(); iSh++)
	if(schHD[iSh].hd) { dlclose(schHD[iSh].hd); schHD.erase(schHD.begin()+(iSh--)); }
}

string TModSchedul::optDescr( )
{
    char buf[STR_BUF_LEN];
    snprintf(buf,sizeof(buf),_(
	"=================== Subsystem \"Module scheduler\" options =================\n"
	"    --modPath=<path>   Modules <path> (/var/os/modules/).\n"
	"------------ Parameters of section '%s' in config-file -----------\n"
	"ModPath  <path>        Path to shared libraries(modules).\n"
	"ModAllow <list>        List of shared libraries allowed for automatic loading, attaching and starting (bd_DBF.so;daq_JavaLikeCalc.so).\n"
	"                       Use '*' value to allow all modules.\n"
	"ModDeny  <list>        List of shared libraries deny for automatic loading, attaching and starting (bd_DBF.so;daq_JavaLikeCalc.so).\n"
	"ChkPer   <sec>         Period of checking at new shared libraries(modules).\n\n"
	),nodePath().c_str());

    return buf;
}

void TModSchedul::setChkPer( int per )	{ mPer = vmax(0,per); modif(); }

int TModSchedul::loadLibS( )	{ return libLoad(SYS->modDir(), false); }

void TModSchedul::load_( )
{
    //Load parameters from command line
    string argVl;
    if(s2i(SYS->cmdOpt("h")) || s2i(SYS->cmdOpt("help"))) fprintf(stdout, "%s", optDescr().c_str());
    if((argVl=SYS->cmdOpt("modPath")).size()) SYS->setModDir(argVl, true);

    //Load parameters from command line
    setChkPer(s2i(TBDS::genDBGet(nodePath()+"ChkPer",i2s(chkPer()))));
    SYS->setModDir(TBDS::genDBGet(nodePath()+"ModPath",SYS->modDir()), true);
    setAllowList(TBDS::genDBGet(nodePath()+"ModAllow",allowList(),"root",TBDS::OnlyCfg));
    setDenyList(TBDS::genDBGet(nodePath()+"ModDeny",denyList(),"root",TBDS::OnlyCfg));
}

void TModSchedul::unload( )
{
    //Unload all modules and pass for holded
    MtxAlloc res(schM, true);
    for(unsigned iSh = 0; iSh < schHD.size(); iSh++)
	while(schHD[iSh].hd && schHD[iSh].use.size())
	    try {
		owner().at(TSYS::strSepParse(schHD[iSh].use[0],0,'.')).at().modDel(TSYS::strSepParse(schHD[iSh].use[0],1,'.'));
		schHD[iSh].use.erase(schHD[iSh].use.begin());
	    } catch(...) { break; }

    //All shared libraries detach
    for(int iSh = 0; iSh < (int)schHD.size(); iSh++)
	if(!schHD[iSh].hd || !schHD[iSh].use.size()) {
	    if(schHD[iSh].hd) dlclose(schHD[iSh].hd);
	    schHD.erase(schHD.begin()+(iSh--));
    }

    TSubSYS::unload();

    mAllow = "*"; mDeny = "";
}

void TModSchedul::save_( )
{
    TBDS::genDBSet(nodePath()+"ChkPer", i2s(chkPer()));
    TBDS::genDBSet(nodePath()+"ModAllow", allowList(), "root", TBDS::OnlyCfg);
    TBDS::genDBSet(nodePath()+"ModDeny", denyList(), "root", TBDS::OnlyCfg);
}

void TModSchedul::dirScan( const string &paths, vector<string> &files )
{
    string modNm, path;
    TRegExp re;

    files.clear();

    //Check and append present files
    dirent  *scanRez = NULL,
	    *scanDirent = (dirent*)malloc(offsetof(dirent,d_name) + NAME_MAX + 1);
    for(int off = 0; (path=TSYS::strParse(paths,0,";",&off)).size(); ) {
	DIR *dirId = opendir(path.c_str());
	if(dirId == NULL) {
	    //Try get a pattern of files of the modules
	    size_t tPos = path.rfind("/");
	    if(tPos == string::npos || tPos == (path.size()-1)) continue;
	    re.setPattern(path.substr(tPos+1), "p"); path = path.substr(0, tPos);
	    if(!(dirId=opendir(path.c_str()))) continue;
	} else re.setPattern("", "p");

	while(readdir_r(dirId,scanDirent,&scanRez) == 0 && scanRez) {
	    if(strcmp("..",scanRez->d_name) == 0 || strcmp(".",scanRez->d_name) == 0 || !re.test(scanRez->d_name)) continue;
	    modNm = path+"/"+scanRez->d_name;
	    if(fileCheck(modNm)) files.push_back(modNm);
	}
	closedir(dirId);
    }
    free(scanDirent);
}

bool TModSchedul::fileCheck( const string &iname )
{
    struct stat file_stat;
    string modNm;

    stat(iname.c_str(),&file_stat);

    if(iname.size() <= 3 || iname.compare(iname.size()-3,3,".so") != 0 || (file_stat.st_mode&S_IFMT) != S_IFREG) return false;
    if(access(iname.c_str(),F_OK|R_OK) != 0)  return false;
    modNm = iname;

    /*void *h_lib = dlopen(iname.c_str(),RTLD_LAZY|RTLD_LOCAL);
    if( h_lib == NULL )
    {
	//mess_sys(TMess::Warning, _("Module '%s' error: %s !"), iname.c_str(), dlerror());
	return false;
    }
    else dlclose(h_lib);*/

    MtxAlloc res(schM, true);
    for(unsigned iSh = 0; iSh < schHD.size(); iSh++)
	if(schHD[iSh].name == iname) {
	    if(file_stat.st_mtime > schHD[iSh].tm) return true;
	    else return false;
	}

    return true;
}

int TModSchedul::libReg( const string &name )
{
    struct stat file_stat;

    MtxAlloc res(schM, true);
    if(name[0] == '*') file_stat.st_mtime = time(NULL);
    else stat(name.c_str(),&file_stat);

    unsigned iSh;
    for(iSh = 0; iSh < schHD.size(); iSh++)
	if(schHD[iSh].name == name) break;
    if(iSh >= schHD.size())	schHD.push_back(SHD(NULL,file_stat.st_mtime,name));
    else schHD[iSh].tm = file_stat.st_mtime;

    return iSh;
}

void TModSchedul::libUnreg( const string &iname )
{
    MtxAlloc res(schM, true);
    for(unsigned iSh = 0; iSh < schHD.size(); iSh++)
	if(schHD[iSh].name == iname) {
	    if(schHD[iSh].hd) libDet(iname);
	    schHD.erase(schHD.begin()+iSh);
	    return;
	}
    throw err_sys(_("SO '%s' is not present!"), iname.c_str());
}

void TModSchedul::libAtt( const string &iname, bool full )
{
    const char *dlErr = NULL, *dlNm = NULL;
    MtxAlloc res(schM, true);
    for(unsigned iSh = 0; iSh < schHD.size(); iSh++)
	if(schHD[iSh].name == iname) {
	    if(schHD[iSh].hd) throw err_sys(_("SO '%s' is already attached!"), iname.c_str());

	    if(iname[0] != '*')	dlNm = iname.c_str();
#ifdef CoreLibNmToDlOpen
	    else dlNm = CoreLibNmToDlOpen;
#else
	    else dlNm = NULL;
#endif
	    void *h_lib = dlopen(dlNm, RTLD_LAZY|RTLD_LOCAL);
	    if(!h_lib) {
		schHD[iSh].err = dlerror();
		throw err_sys(_("SO '%s' error: %s !"), iname.c_str(), schHD[iSh].err.c_str());
	    }

	    //Connect to module function
	    TModule::SAt (*module)( int );
	    module = (TModule::SAt (*)(int)) dlsym(h_lib,(iname[0]!='*')?"module":(iname.substr(1)+"_module").c_str());
	    if((dlErr=dlerror()) != NULL) {
		schHD[iSh].err = dlErr;
		dlclose(h_lib);
		throw err_sys(_("SO '%s' error: %s !"), iname.c_str(), schHD[iSh].err.c_str());
	    }

	    //Connect to attach function
	    TModule *(*attach)( const TModule::SAt &, const string & );
	    attach = (TModule * (*)(const TModule::SAt &, const string &)) dlsym(h_lib,(iname[0]!='*')?"attach":(iname.substr(1)+"_attach").c_str());
	    if((dlErr=dlerror()) != NULL) {
		schHD[iSh].err = dlErr;
		dlclose(h_lib);
		throw err_sys(_("SO '%s' error: %s !"), iname.c_str(), schHD[iSh].err.c_str());
	    }

	    //Get allow modules from library and start it
	    int n_mod = 0, add_mod = 0;
	    TModule::SAt AtMod;
	    while((AtMod=(module)(n_mod++)).id.size()) {
		vector<string> list;
		owner().list(list);
		for(unsigned i_sub = 0; i_sub < list.size(); i_sub++) {
		    if(owner().at(list[i_sub]).at().subModule() && AtMod.type == owner().at(list[i_sub]).at().subId()) {
			// Check type module version
			if(AtMod.tVer != owner().at(list[i_sub]).at().subVer()) {
			    mess_sys(TMess::Warning, _("%s for type '%s' doesn't support module version: %d!"),
				AtMod.id.c_str(), AtMod.type.c_str(), AtMod.tVer);
			    break;
			}
			// Check module present
			if(owner().at(list[i_sub]).at().modPresent(AtMod.id))
			    mess_sys(TMess::Warning, _("Module '%s' is already present!"), AtMod.id.c_str());
			else {
			    // Attach new module
			    TModule *LdMod = (attach)(AtMod, iname);
			    if(LdMod == NULL) {
				mess_sys(TMess::Warning, _("Attach module '%s' error!"), AtMod.id.c_str());
				break;
			    }
			    // Add atached module
			    owner().at(list[i_sub]).at().modAdd(LdMod);
			    schHD[iSh].use.push_back(list[i_sub]+"."+LdMod->modId());
			    if(full) {
				owner().at(list[i_sub]).at().modifG();
				owner().at(list[i_sub]).at().load();
				owner().at(list[i_sub]).at().subStart();
			    }
			    add_mod++;
			    break;
			}
		    }
		}
	    }
	    if(add_mod == 0) dlclose(h_lib);
	    else schHD[iSh].hd = h_lib;
	    return;
	}
    throw err_sys(_("SO '%s' is not present!"), iname.c_str());
}

void TModSchedul::libDet( const string &iname )
{
    MtxAlloc res(schM, true);
    for(unsigned iSh = 0; iSh < schHD.size(); iSh++)
	if(schHD[iSh].name == iname) {
	    if(!schHD[iSh].hd) return;
	    try {
		//Stop all modules
		for(unsigned i_m = 0; i_m < schHD[iSh].use.size(); i_m++)
		    owner().at(TSYS::strSepParse(schHD[iSh].use[i_m],0,'.')).at().
			modAt(TSYS::strSepParse(schHD[iSh].use[i_m],1,'.')).at().modStop();
		//Delete all modules
		while(schHD[iSh].use.size()) {
		    owner().at(TSYS::strSepParse(schHD[iSh].use[0],0,'.')).at().
			modDel(TSYS::strSepParse(schHD[iSh].use[0],1,'.'));
		    schHD[iSh].use.erase(schHD[iSh].use.begin());
		}
	    } catch(TError &err) {
		//owner().at(schHD[iSh]->use[0].mod_sub).at().modAt(schHD[iSh]->use[0].n_mod).at().load();
		//Start all modules
		for(unsigned i_m = 0; i_m < schHD[iSh].use.size(); i_m++)
		    owner().at(TSYS::strSepParse(schHD[iSh].use[i_m],0,'.')).at().
			modAt(TSYS::strSepParse(schHD[iSh].use[i_m],1,'.')).at().modStart();
		throw;
	    }
	    dlclose(schHD[iSh].hd);
	    schHD[iSh].hd = NULL;
	    return;
	}
    throw err_sys(_("SO '%s' is not present!"), iname.c_str());
}

bool TModSchedul::chkAllowMod( const string &name )
{
    string sel, nmFile;
    int path_els = 0;

    for(int off = 0; (sel=TSYS::pathLev(name,0,true,&off)).size(); path_els++) nmFile = sel;

    if(allowList().empty())	return false;
    if(sTrm(allowList()) != "*") {
	for(int off = 0; (sel=TSYS::strSepParse(allowList(),0,';',&off)).size(); )
	    if(sTrm(sel) == name || sTrm(sel) == nmFile) break;
	if(sel.empty()) return false;
    }

    for(int off = 0; (sel=TSYS::strSepParse(denyList(),0,';',&off)).size(); )
	if(sTrm(sel) == name || sTrm(sel) == nmFile) return false;

    return true;
}

void TModSchedul::libList( vector<string> &list )
{
    list.clear();

    MtxAlloc res(schM, true);
    for(unsigned iSh = 0; iSh < schHD.size(); iSh++)
	list.push_back(schHD[iSh].name);
}

TModSchedul::SHD TModSchedul::lib( const string &iname )
{
    MtxAlloc res(schM, true);
    for(unsigned iSh = 0; iSh < schHD.size(); iSh++)
	if(schHD[iSh].name == iname)
	    return schHD[iSh];
    throw err_sys(_("SO '%s' is not present!"), iname.c_str());
}

int TModSchedul::libLoad( const string &iname, bool full )
{
    int ldCnt = 0;
    vector<string> files, llist;

    //Check and register builtin modules
    string smod;
    for(int off = 0; (smod=TSYS::strParse(MODS_INCL,0," ",&off)).size(); ) {
	unsigned iSh;
	MtxAlloc res(schM, true);
	for(iSh = 0; iSh < schHD.size(); iSh++)
	    if(schHD[iSh].name == ("*"+smod)) break;
	if(iSh >= schHD.size()) {
	    res.unlock();
	    libReg("*"+smod);
	    try{ libAtt("*"+smod,full); ldCnt++; }
	    catch(TError &err) { mess_warning(err.cat.c_str(), "%s", err.mess.c_str()); }
	}
    }

    //Get new or changed modules list
    dirScan(iname, files);

    //Add and process allowed modules
    for(unsigned iF = 0; iF < files.size(); iF++) {
	unsigned iSh;
	bool st_auto = chkAllowMod(files[iF]);
	MtxAlloc res(schM, true);
	for(iSh = 0; iSh < schHD.size(); iSh++)
	    if(schHD[iSh].name == files[iF]) break;
	if(iSh < schHD.size()) {
	    try {
		res.unlock();
		if(st_auto) libDet(files[iF]);
	    } catch(TError &err) {
		mess_warning(err.cat.c_str(), "%s", err.mess.c_str());
		mess_sys(TMess::Warning, _("Can't detach library '%s'."), files[iF].c_str());
		continue;
	    }
	}
	res.unlock();

	libReg(files[iF]);

	if(st_auto) {
	    try{ libAtt(files[iF],full); ldCnt++; }
	    catch(TError &err) { mess_warning(err.cat.c_str(), "%s", err.mess.c_str()); }
	}
    }

    return ldCnt;
}

void TModSchedul::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TSubSYS::cntrCmdProc(opt);
	if(ctrMkNode("area",opt,0,"/ms",_("Subsystem"),R_R_R_,"root",SMSH_ID)) {
	    ctrMkNode("fld",opt,-1,"/ms/mod_path",_("Path to shared libs(modules)"),R_R_R_,"root",SMSH_ID,1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/ms/mod_deny",_("Denied modules"),RWRWR_,"root",SMSH_ID,2,"tp","str",
		"help",_("List of shared libs(modules) deny for auto connection.\n"
		         "Elements separated by symbol ';'."));
	    ctrMkNode("fld",opt,-1,"/ms/mod_allow",_("Allowed modules"),RWRWR_,"root",SMSH_ID,2,"tp","str",
		"help",_("List of shared libs(modules) allowed for auto connection.\n"
		         "Elements separated by symbol ';'.\n"
		         "Value '*' used for allow all modules."));
	    ctrMkNode("fld",opt,-1,"/ms/chk_per",_("Check modules period, seconds"),RWRWR_,"root",SMSH_ID,1,"tp","dec");
	    ctrMkNode("comm",opt,-1,"/ms/chk_now",_("Check modules now."),RWRW__,"root",SMSH_ID);
	    if(ctrMkNode("table",opt,-1,"/ms/libs",_("Shared libs(modules)"),RWRWR_,"root",SMSH_ID,1,"key","path")) {
		ctrMkNode("list",opt,-1,"/ms/libs/path",_("Path"),R_R_R_,"root",SMSH_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/ms/libs/tm",_("Time"),R_R_R_,"root",SMSH_ID,1,"tp","time");
		ctrMkNode("list",opt,-1,"/ms/libs/mods",_("Modules"),R_R_R_,"root",SMSH_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/ms/libs/en",_("Enabled"),RWRWR_,"root",SMSH_ID,1,"tp","bool");
	    }
	}
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/ms/mod_path" && ctrChkNode(opt,"get"))	opt->setText(SYS->modDir());
    else if(a_path == "/ms/mod_deny") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SMSH_ID,SEC_RD))	opt->setText(denyList());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SMSH_ID,SEC_WR))	setDenyList(opt->text());
    }
    else if(a_path == "/ms/mod_allow") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SMSH_ID,SEC_RD))	opt->setText(allowList());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SMSH_ID,SEC_WR))	setAllowList(opt->text());
    }
    else if(a_path == "/ms/chk_per") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SMSH_ID,SEC_RD))	opt->setText(i2s(chkPer()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SMSH_ID,SEC_WR))	setChkPer(s2i(opt->text()));
    }
    else if(a_path == "/ms/chk_now" && ctrChkNode(opt,"set",RWRW__,"root",SMSH_ID,SEC_WR))	libLoad(SYS->modDir(),true);
    else if(a_path == "/ms/libs") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SMSH_ID,SEC_RD)) {
	    XMLNode *n_nm	= ctrMkNode("list",opt,-1,"/ms/libs/path","");
	    XMLNode *n_tm	= ctrMkNode("list",opt,-1,"/ms/libs/tm","");
	    XMLNode *n_mods	= ctrMkNode("list",opt,-1,"/ms/libs/mods","");
	    XMLNode *n_en	= ctrMkNode("list",opt,-1,"/ms/libs/en","");
	    MtxAlloc res(schM, true);
	    for(unsigned i_sl = 0; i_sl < schHD.size(); i_sl++) {
		if(n_nm)	n_nm->childAdd("el")->setText(schHD[i_sl].name);
		if(n_tm)	n_tm->childAdd("el")->setText(i2s(schHD[i_sl].tm));
		if(n_mods) {
		    string useLs;
		    if(!schHD[i_sl].hd && !schHD[i_sl].err.empty()) useLs = schHD[i_sl].err;
		    else
			for(unsigned i_el = 0; i_el < schHD[i_sl].use.size(); i_el++)
			    useLs += schHD[i_sl].use[i_el]+"; ";
		    n_mods->childAdd("el")->setText(useLs);
		}
		if(n_en)	n_en->childAdd("el")->setText(i2s((bool)schHD[i_sl].hd));
	    }
	}
	if(ctrChkNode(opt,"set",RWRWR_,"root",SMSH_ID,SEC_WR)) {
	    if(opt->attr("col") == "en") {
		if(s2i(opt->text())) libAtt(opt->attr("key_path"),true);
		else libDet(opt->attr("key_path"));
	    }
	}
    }
    else TSubSYS::cntrCmdProc(opt);
}
