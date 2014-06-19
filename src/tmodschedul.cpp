
//OpenSCADA system file: tmodschedul.cpp
/***************************************************************************
 *   Copyright (C) 2003-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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
TModSchedul::TModSchedul( ) : TSubSYS(SMSH_ID,_("Modules scheduler"),false), mAllow("*"), mPer(10)
{

}

void TModSchedul::preDisable(int flag)
{
    //> Detach all share libs
    ResAlloc res(nodeRes(),true);
    for(unsigned i_sh = 0; i_sh < SchHD.size(); i_sh++)
	if(SchHD[i_sh].hd)
	{
	    while(SchHD[i_sh].use.size())
	    {
		owner().at(TSYS::strSepParse(SchHD[i_sh].use[0],0,'.')).at().
			modDel(TSYS::strSepParse(SchHD[i_sh].use[0],1,'.'));
		SchHD[i_sh].use.erase(SchHD[i_sh].use.begin());
	    }
	    dlclose(SchHD[i_sh].hd);
	}
    res.release();
}

string TModSchedul::optDescr( )
{
    char buf[STR_BUF_LEN];
    snprintf(buf,sizeof(buf),_(
	"=================== Subsystem \"Module scheduler\" options =================\n"
	"    --ModPath=<path>   Modules <path> (/var/os/modules/).\n"
	"------------ Parameters of section '%s' in config-file -----------\n"
	"ModPath  <path>        Path to shared libraries(modules).\n"
	"ModAllow <list>        List of shared libraries allowed for automatic loading, attaching and starting (bd_DBF.so;daq_JavaLikeCalc.so).\n"
	"                       Use '*' value for allow all modules.\n"
	"ModDeny  <list>        List of shared libraries deny for automatic loading, attaching and starting (bd_DBF.so;daq_JavaLikeCalc.so).\n"
	"ChkPer   <sec>         Period of checking at new shared libraries(modules).\n\n"
	),nodePath().c_str());

    return(buf);
}

void TModSchedul::setChkPer( int per )	{ mPer = vmax(0,per); modif(); }

int TModSchedul::loadLibS( )
{
    return libLoad(SYS->modDir(),false);
}

void TModSchedul::load_( )
{
    //> Load parameters from command line
    string argCom, argVl;
    for(int argPos = 0; (argCom=SYS->getCmdOpt(argPos,&argVl)).size(); )
        if(argCom == "h" || argCom == "help")	fprintf(stdout,"%s",optDescr().c_str());
        else if(argCom == "ModPath")	SYS->setModDir(optarg, true);

    //> Load parameters from command line
    setChkPer(atoi(TBDS::genDBGet(nodePath()+"ChkPer",TSYS::int2str(chkPer())).c_str()));
    SYS->setModDir(TBDS::genDBGet(nodePath()+"ModPath",SYS->modDir()), true);
    setAllowList(TBDS::genDBGet(nodePath()+"ModAllow",allowList(),"root",TBDS::OnlyCfg));
    setDenyList(TBDS::genDBGet(nodePath()+"ModDeny",denyList(),"root",TBDS::OnlyCfg));
}

void TModSchedul::save_( )
{
    TBDS::genDBSet(nodePath()+"ChkPer",TSYS::int2str(chkPer()));
    TBDS::genDBSet(nodePath()+"ModAllow",allowList(),"root",TBDS::OnlyCfg);
    TBDS::genDBSet(nodePath()+"ModDeny",denyList(),"root",TBDS::OnlyCfg);
}

void TModSchedul::ScanDir( const string &Paths, vector<string> &files )
{
    string NameMod, Path;

    files.clear();

    //> Check and append present files
    for(int off = 0; (Path=TSYS::strParse(Paths,0,",",&off)).size(); )
    {
	dirent scan_dirent, *scan_rez = NULL;
	DIR *IdDir = opendir(Path.c_str());
	if(IdDir == NULL) continue;

	while(readdir_r(IdDir,&scan_dirent,&scan_rez) == 0 && scan_rez)
	{
	    if(strcmp("..",scan_rez->d_name) == 0 || strcmp(".",scan_rez->d_name) == 0) continue;
	    NameMod = Path+"/"+scan_rez->d_name;
	    if(CheckFile(NameMod)) files.push_back(NameMod);
	}
	closedir(IdDir);
    }
}

bool TModSchedul::CheckFile( const string &iname )
{
    struct stat file_stat;
    string NameMod;

    stat(iname.c_str(),&file_stat);

    if(iname.size() <= 3 || iname.compare(iname.size()-3,3,".so") != 0 || (file_stat.st_mode&S_IFMT) != S_IFREG) return false;
    if(access(iname.c_str(),F_OK|R_OK) != 0)  return false;
    NameMod = iname;

    /*void *h_lib = dlopen(iname.c_str(),RTLD_LAZY|RTLD_LOCAL);
    if( h_lib == NULL )
    {
	//mess_warning(nodePath().c_str(),_("Module '%s' error: %s !"),iname.c_str(),dlerror());
	return false;
    }
    else dlclose(h_lib);*/

    ResAlloc res(nodeRes(),false);
    for(unsigned i_sh=0; i_sh < SchHD.size(); i_sh++)
	if(SchHD[i_sh].name == iname)
	{
	    if(file_stat.st_mtime > SchHD[i_sh].tm) return true;
	    else return false;
	}

    return true;
}

int TModSchedul::libReg( const string &name )
{
    struct stat file_stat;

    ResAlloc res(nodeRes(),true);
    if(name[0] == '*') file_stat.st_mtime = time(NULL);
    else stat(name.c_str(),&file_stat);

    unsigned i_sh;
    for(i_sh = 0; i_sh < SchHD.size(); i_sh++)
	if(SchHD[i_sh].name == name) break;
    if(i_sh >= SchHD.size())	SchHD.push_back(SHD(NULL,file_stat.st_mtime,name));
    else SchHD[i_sh].tm = file_stat.st_mtime;

    return i_sh;
}

void TModSchedul::libUnreg( const string &iname )
{
    ResAlloc res(nodeRes(),true);
    for(unsigned i_sh = 0; i_sh < SchHD.size(); i_sh++)
	if(SchHD[i_sh].name == iname)
	{
	    if(SchHD[i_sh].hd) libDet(iname);
	    SchHD.erase(SchHD.begin()+i_sh);
	    return;
	}
    throw TError(nodePath().c_str(),_("SO '%s' is not present!"),iname.c_str());
}

void TModSchedul::libAtt( const string &iname, bool full )
{
    char *dlErr = NULL;
    ResAlloc res(nodeRes(),true);
    for(unsigned i_sh = 0; i_sh < SchHD.size(); i_sh++)
	if(SchHD[i_sh].name == iname)
	{
	    if(SchHD[i_sh].hd) throw TError(nodePath().c_str(),_("SO '%s' is already attached!"),iname.c_str());

	    void *h_lib = dlopen((iname[0]!='*')?iname.c_str():NULL, RTLD_LAZY|RTLD_LOCAL);
	    if(!h_lib)
	    {
		SchHD[i_sh].err = dlerror();
		throw TError(nodePath().c_str(),_("SO '%s' error: %s !"),iname.c_str(),SchHD[i_sh].err.c_str());
	    }

	    //> Connect to module function
	    TModule::SAt (*module)( int );
	    module = (TModule::SAt (*)(int)) dlsym(h_lib,(iname[0]!='*')?"module":(iname.substr(1)+"_module").c_str());
	    if((dlErr=dlerror()) != NULL)
	    {
		SchHD[i_sh].err = dlErr;
		dlclose(h_lib);
		throw TError(nodePath().c_str(),_("SO '%s' error: %s !"),iname.c_str(),SchHD[i_sh].err.c_str());
	    }

	    //> Connect to attach function
	    TModule *(*attach)( const TModule::SAt &, const string & );
	    attach = (TModule * (*)(const TModule::SAt &, const string &)) dlsym(h_lib,(iname[0]!='*')?"attach":(iname.substr(1)+"_attach").c_str());
	    if((dlErr=dlerror()) != NULL)
	    {
		SchHD[i_sh].err = dlErr;
		dlclose(h_lib);
		throw TError(nodePath().c_str(),_("SO '%s' error: %s !"),iname.c_str(),SchHD[i_sh].err.c_str());
	    }

	    //> Get allow modules from library and start it
	    int n_mod=0, add_mod=0;
	    TModule::SAt AtMod;
	    while((AtMod = (module)(n_mod++)).id.size())
	    {
		vector<string> list;
		owner().list(list);
		for(unsigned i_sub = 0; i_sub < list.size(); i_sub++)
		{
		    if(owner().at(list[i_sub]).at().subModule() && AtMod.type == owner().at(list[i_sub]).at().subId())
		    {
			//>> Check type module version
			if( AtMod.t_ver != owner().at(list[i_sub]).at().subVer() )
			{
			    mess_warning(nodePath().c_str(),_("%s for type '%s' doesn't support module version: %d!"),
				AtMod.id.c_str(),AtMod.type.c_str(),AtMod.t_ver);
			    break;
			}
			//>> Check module present
			if( owner().at(list[i_sub]).at().modPresent(AtMod.id) )
			    mess_warning(nodePath().c_str(),_("Module '%s' is already present!"),AtMod.id.c_str());
			else
			{
			    //>> Attach new module
			    TModule *LdMod = (attach)( AtMod, iname );
			    if(LdMod == NULL)
			    {
				mess_warning(nodePath().c_str(),_("Attach module '%s' error!"),AtMod.id.c_str());
				break;
			    }
			    //>> Add atached module
			    owner().at(list[i_sub]).at().modAdd(LdMod);
			    SchHD[i_sh].use.push_back(list[i_sub]+"."+LdMod->modId());
			    if(full)
			    {
				owner().at(list[i_sub]).at().load(true);
				owner().at(list[i_sub]).at().subStart();
			    }
			    add_mod++;
			    break;
			}
		    }
		}
	    }
	    if(add_mod == 0) dlclose(h_lib);
	    else SchHD[i_sh].hd = h_lib;
	    return;
	}
    throw TError(nodePath().c_str(),_("SO '%s' is not present!"),iname.c_str());
}

void TModSchedul::libDet( const string &iname )
{
    ResAlloc res(nodeRes(),true);
    for(unsigned i_sh = 0; i_sh < SchHD.size(); i_sh++)
	if(SchHD[i_sh].name == iname)
	{
	    if(!SchHD[i_sh].hd) return;
	    try
	    {
		//> Stop all modules
		for(unsigned i_m = 0; i_m < SchHD[i_sh].use.size(); i_m++)
		    owner().at(TSYS::strSepParse(SchHD[i_sh].use[i_m],0,'.')).at().
			modAt(TSYS::strSepParse(SchHD[i_sh].use[i_m],1,'.')).at().modStop();
		//> Delete all modules
		while(SchHD[i_sh].use.size())
		{
		    owner().at(TSYS::strSepParse(SchHD[i_sh].use[0],0,'.')).at().
			modDel(TSYS::strSepParse(SchHD[i_sh].use[0],1,'.'));
		    SchHD[i_sh].use.erase(SchHD[i_sh].use.begin());
		}
	    }catch(TError err)
	    {
		//owner().at(SchHD[i_sh]->use[0].mod_sub).at().modAt(SchHD[i_sh]->use[0].n_mod).at().load();
		//> Start all modules
		for(unsigned i_m = 0; i_m < SchHD[i_sh].use.size(); i_m++)
		    owner().at(TSYS::strSepParse(SchHD[i_sh].use[i_m],0,'.')).at().
			modAt(TSYS::strSepParse(SchHD[i_sh].use[i_m],1,'.')).at().modStart();
		throw;
	    }
	    dlclose(SchHD[i_sh].hd);
	    SchHD[i_sh].hd = NULL;
	    return;
	}
    throw TError(nodePath().c_str(),_("SO '%s' is not present!"),iname.c_str());
}

bool TModSchedul::chkAllowMod( const string &name )
{
    string sel, nmFile;
    int path_els = 0;

    for(int off = 0; (sel=TSYS::pathLev(name,0,true,&off)).size(); path_els++) nmFile = sel;

    if(allowList().empty())	return false;
    if(TSYS::strNoSpace(allowList()) != "*")
    {
	for(int off = 0; (sel=TSYS::strSepParse(allowList(),0,';',&off)).size(); )
	    if(TSYS::strNoSpace(sel) == name || TSYS::strNoSpace(sel) == nmFile) break;
	if(sel.empty()) return false;
    }
    for(int off = 0; (sel=TSYS::strSepParse(denyList(),0,';',&off)).size(); )
	if(TSYS::strNoSpace(sel) == name || TSYS::strNoSpace(sel) == nmFile) return false;

    return true;
}

void TModSchedul::libList( vector<string> &list )
{
    ResAlloc res(nodeRes(),false);
    list.clear();
    for(unsigned i_sh = 0; i_sh < SchHD.size(); i_sh++)
	list.push_back(SchHD[i_sh].name);
}

TModSchedul::SHD TModSchedul::lib( const string &iname )
{
    ResAlloc res(nodeRes(),false);
    for(unsigned i_sh = 0; i_sh < SchHD.size(); i_sh++)
	if(SchHD[i_sh].name == iname)
	    return SchHD[i_sh];
    throw TError(nodePath().c_str(),_("SO '%s' is not present!"),iname.c_str());
}

int TModSchedul::libLoad( const string &iname, bool full )
{
    int ldCnt = 0;
    vector<string> files, llist;

    //> Check and register builtin modules
    string smod;
    for(int off = 0; (smod=TSYS::strParse(MODS_INCL,0," ",&off)).size(); )
    {
	unsigned i_sh;
	ResAlloc res(nodeRes(),false);
	for(i_sh = 0; i_sh < SchHD.size(); i_sh++)
	    if(SchHD[i_sh].name == ("*"+smod)) break;
	if(i_sh >= SchHD.size())
	{
	    res.release();
	    libReg("*"+smod);
	    try{ libAtt("*"+smod,full); ldCnt++; }
	    catch(TError err) { mess_warning(err.cat.c_str(),"%s",err.mess.c_str()); }
	}
    }

    //> Get new or changed modules list
    ScanDir(iname, files);

    //> Add and process allowed modules
    for(unsigned i_f = 0; i_f < files.size(); i_f++)
    {
	unsigned i_sh;
	bool st_auto = chkAllowMod(files[i_f]);
	ResAlloc res(nodeRes(),false);
	for(i_sh = 0; i_sh < SchHD.size(); i_sh++)
	    if(SchHD[i_sh].name == files[i_f]) break;
	if(i_sh < SchHD.size())
	{
	    try
	    {
		res.release();
		if(st_auto) libDet(files[i_f]);
	    }
	    catch(TError err)
	    {
		mess_warning(err.cat.c_str(),"%s",err.mess.c_str());
		mess_warning(nodePath().c_str(),_("Can't detach library '%s'."),files[i_f].c_str());
		continue;
	    }
	}
	res.release();
	libReg(files[i_f]);
	if(st_auto)
	{
	    try{ libAtt(files[i_f],full); ldCnt++; }
	    catch(TError err){ mess_warning(err.cat.c_str(),"%s",err.mess.c_str()); }
	}
    }

    return ldCnt;
}

void TModSchedul::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if(opt->name() == "info")
    {
	TSubSYS::cntrCmdProc(opt);
	if(ctrMkNode("area",opt,0,"/ms",_("Subsystem"),R_R_R_,"root",SMSH_ID))
	{
	    ctrMkNode("fld",opt,-1,"/ms/mod_path",_("Path to shared libs(modules)"),R_R_R_,"root",SMSH_ID,1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/ms/mod_deny",_("Denied modules"),RWRWR_,"root",SMSH_ID,2,"tp","str",
		"help",_("List of shared libs(modules) deny for auto connection.\n"
		         "Elements separated by symbol ';'."));
	    ctrMkNode("fld",opt,-1,"/ms/mod_allow",_("Allowed modules"),RWRWR_,"root",SMSH_ID,2,"tp","str",
		"help",_("List of shared libs(modules) allowed for auto connection.\n"
		         "Elements separated by symbol ';'.\n"
		         "Value '*' used for allow all modules."));
	    ctrMkNode("fld",opt,-1,"/ms/chk_per",_("Check modules period (sec)"),RWRWR_,"root",SMSH_ID,1,"tp","dec");
	    ctrMkNode("comm",opt,-1,"/ms/chk_now",_("Check modules now."),RWRW__,"root",SMSH_ID);
	    if(ctrMkNode("table",opt,-1,"/ms/libs",_("Shared libs(modules)"),RWRWR_,"root",SMSH_ID,1,"key","path"))
	    {
		ctrMkNode("list",opt,-1,"/ms/libs/path",_("Path"),R_R_R_,"root",SMSH_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/ms/libs/tm",_("Time"),R_R_R_,"root",SMSH_ID,1,"tp","time");
		ctrMkNode("list",opt,-1,"/ms/libs/mods",_("Modules"),R_R_R_,"root",SMSH_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/ms/libs/en",_("Enable"),RWRWR_,"root",SMSH_ID,1,"tp","bool");
	    }
	}
	return;
    }

    //> Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/ms/mod_path" && ctrChkNode(opt,"get"))	opt->setText(SYS->modDir());
    else if(a_path == "/ms/mod_deny")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SMSH_ID,SEC_RD))	opt->setText(denyList());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SMSH_ID,SEC_WR))	setDenyList(opt->text());
    }
    else if(a_path == "/ms/mod_allow")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SMSH_ID,SEC_RD))	opt->setText(allowList());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SMSH_ID,SEC_WR))	setAllowList(opt->text());
    }
    else if(a_path == "/ms/chk_per")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SMSH_ID,SEC_RD))	opt->setText(TSYS::int2str(chkPer()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SMSH_ID,SEC_WR))	setChkPer(atoi(opt->text().c_str()));
    }
    else if(a_path == "/ms/chk_now" && ctrChkNode(opt,"set",RWRW__,"root",SMSH_ID,SEC_WR))	libLoad(SYS->modDir(),true);
    else if(a_path == "/ms/libs")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SMSH_ID,SEC_RD))
	{
	    XMLNode *n_nm	= ctrMkNode("list",opt,-1,"/ms/libs/path","");
	    XMLNode *n_tm	= ctrMkNode("list",opt,-1,"/ms/libs/tm","");
	    XMLNode *n_mods	= ctrMkNode("list",opt,-1,"/ms/libs/mods","");
	    XMLNode *n_en	= ctrMkNode("list",opt,-1,"/ms/libs/en","");
	    ResAlloc res(nodeRes(),false);
	    for(unsigned i_sl = 0; i_sl < SchHD.size(); i_sl++)
	    {
		if(n_nm)	n_nm->childAdd("el")->setText(SchHD[i_sl].name);
		if(n_tm)	n_tm->childAdd("el")->setText(TSYS::TSYS::int2str(SchHD[i_sl].tm));
		if(n_mods)
		{
		    string useLs;
		    if( !SchHD[i_sl].hd && !SchHD[i_sl].err.empty() ) useLs = SchHD[i_sl].err;
		    else
			for(unsigned i_el = 0; i_el < SchHD[i_sl].use.size(); i_el++)
			    useLs += SchHD[i_sl].use[i_el]+"; ";
		    n_mods->childAdd("el")->setText(useLs);
		}
		if(n_en)	n_en->childAdd("el")->setText(TSYS::TSYS::int2str((bool)SchHD[i_sl].hd));
	    }
	}
	if(ctrChkNode(opt,"set",RWRWR_,"root",SMSH_ID,SEC_WR))
	{
	    if(opt->attr("col") == "en")
	    {
		if(atoi(opt->text().c_str())) libAtt(opt->attr("key_path"),true);
		else libDet(opt->attr("key_path"));
	    }
	}
    }
    else TSubSYS::cntrCmdProc(opt);
}
