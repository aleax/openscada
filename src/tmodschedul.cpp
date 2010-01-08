
//OpenSCADA system file: tmodschedul.cpp
/***************************************************************************
 *   Copyright (C) 2003-2009 by Roman Savochenko                           *
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

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include <dirent.h>
#include <stdlib.h>
#include <signal.h>
#include <getopt.h>
#include <string.h>

#include <string>

#include "tsys.h"
#include "tmodschedul.h"

//*************************************************
//* TModSchedul                                   *
//*************************************************
TModSchedul::TModSchedul( ) : TSubSYS("ModSched","Modules sheduler",false), mPer(10), mAllow("*")
{

}

void TModSchedul::preDisable(int flag)
{
    //> Detach all share libs
    ResAlloc res(nodeRes(),true);
    for( unsigned i_sh = 0; i_sh < SchHD.size(); i_sh++ )
	if( SchHD[i_sh].hd )
	{
	    while( SchHD[i_sh].use.size() )
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
	"=================== Subsystem \"Module sheduler\" options =================\n"
	"    --ModPath=<path>   Modules <path> (/var/os/modules/).\n"
	"------------ Parameters of section <%s> in config file -----------\n"
	"ModPath  <path>        Path to shared libraries(modules).\n"
	"ModAllow <list>        List of shared libraries allowed for automatic loading, attaching and starting (bd_DBF.so;daq_JavaLikeCalc.so).\n"
	"                       Use '*' value for allow all modules.\n"
	"ModDeny  <list>        List of shared libraries deny for automatic loading, attaching and starting (bd_DBF.so;daq_JavaLikeCalc.so).\n"
	"ChkPer   <sec>         Period of checking at new shared libraries(modules).\n\n"
	),nodePath().c_str());

    return(buf);
}

int TModSchedul::loadLibS(  )
{
    return libLoad(SYS->modDir(),false);
}

void TModSchedul::load_( )
{
    //> Load parameters from command line
    int next_opt;
    const char *short_opt="h";
    struct option long_opt[] =
    {
	{"help"     ,0,NULL,'h'},
	{"ModPath"  ,1,NULL,'m'},
	{NULL       ,0,NULL,0  }
    };

    optind=opterr=0;
    do
    {
	next_opt=getopt_long(SYS->argc,( char *const * ) SYS->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': fprintf(stdout,"%s",optDescr().c_str()); break;
	    case 'm': SYS->setModDir(optarg); break;
	    case -1 : break;
	}
    } while(next_opt != -1);

    //> Load parameters from command line
    setChkPer( atoi(TBDS::genDBGet(nodePath()+"ChkPer",TSYS::int2str(chkPer())).c_str()) );
    SYS->setModDir( TBDS::genDBGet(nodePath()+"ModPath",SYS->modDir()) );
    setAllowList(TBDS::genDBGet(nodePath()+"ModAllow",allowList(),"root",true));
    setDenyList(TBDS::genDBGet(nodePath()+"ModDeny",denyList(),"root",true));
}

void TModSchedul::save_( )
{
    TBDS::genDBSet(nodePath()+"ChkPer",TSYS::int2str(chkPer()));
}

void TModSchedul::ScanDir( const string &Paths, vector<string> &files )
{
    string NameMod, Path;

    files.clear();

    int ido, id=-1;
    do
    {
	ido=id+1; id = Paths.find(",",ido);

	dirent *scan_dirent;
	Path=Paths.substr(ido,id-ido);
	if(Path.size() <= 0) continue;

	DIR *IdDir = opendir(Path.c_str());
	if(IdDir == NULL) continue;

	while((scan_dirent = readdir(IdDir)) != NULL)
	{
	    if( string("..") == scan_dirent->d_name || string(".") == scan_dirent->d_name ) continue;
	    NameMod = Path+"/"+scan_dirent->d_name;
	    if( CheckFile(NameMod) ) files.push_back(NameMod);
	}
	closedir(IdDir);

    } while(id != (int)string::npos);
}

bool TModSchedul::CheckFile( const string &iname )
{
    struct stat file_stat;
    string NameMod;

    stat(iname.c_str(),&file_stat);

    if( /*iname.size() <= 3 || iname.substr(iname.size()-3,3) != ".so" ||*/ (file_stat.st_mode&S_IFMT) != S_IFREG ) return false;
    if( access(iname.c_str(),F_OK|R_OK) != 0 )  return false;
    NameMod=iname;

    void *h_lib = dlopen(iname.c_str(),RTLD_LAZY|RTLD_GLOBAL);
    if( h_lib == NULL )
    {
	//mess_warning(nodePath().c_str(),_("Module <%s> error: %s !"),iname.c_str(),dlerror());
	return false;
    }
    else dlclose(h_lib);

    ResAlloc res(nodeRes(),false);
    for( int i_sh=0; i_sh < SchHD.size(); i_sh++ )
	if( SchHD[i_sh].name == iname )
	    if(file_stat.st_mtime > SchHD[i_sh].tm) return true;
	    else return false;

    return true;
}

int TModSchedul::libReg( const string &name )
{
    struct stat file_stat;

    ResAlloc res(nodeRes(),true);
    stat(name.c_str(),&file_stat);
    unsigned i_sh;
    for( i_sh = 0; i_sh < SchHD.size(); i_sh++ )
	if( SchHD[i_sh].name == name ) break;
    if( i_sh >= SchHD.size() )	SchHD.push_back( SHD(NULL,file_stat.st_mtime,name) );
    else SchHD[i_sh].tm = file_stat.st_mtime;

    return i_sh;
}

void TModSchedul::libUnreg( const string &iname )
{
    ResAlloc res(nodeRes(),true);
    for(unsigned i_sh = 0; i_sh < SchHD.size(); i_sh++)
	if( SchHD[i_sh].name == iname ) 
	{
	    if( SchHD[i_sh].hd ) libDet( iname );
	    SchHD.erase(SchHD.begin()+i_sh);
	    return;
	}
    throw TError(nodePath().c_str(),_("SO <%s> is not present!"),iname.c_str());
}

void TModSchedul::libAtt( const string &iname, bool full )
{
    ResAlloc res(nodeRes(),true);
    for( int i_sh = 0; i_sh < SchHD.size(); i_sh++ )
	if( SchHD[i_sh].name == iname )
	{
	    if( SchHD[i_sh].hd )
		throw TError(nodePath().c_str(),_("SO <%s> is already attached!"),iname.c_str());

	    void *h_lib = dlopen(iname.c_str(),RTLD_LAZY|RTLD_GLOBAL);
	    if( !h_lib )
		throw TError(nodePath().c_str(),_("SO <%s> error: %s !"),iname.c_str(),dlerror());

	    //> Connect to module function
	    TModule::SAt (*module)( int );
	    module = (TModule::SAt (*)(int)) dlsym(h_lib,"module");
	    if( dlerror() != NULL )
	    {
		dlclose(h_lib);
		throw TError(nodePath().c_str(),_("SO <%s> error: %s !"),iname.c_str(),dlerror());
	    }

	    //> Connect to attach function
	    TModule *(*attach)( const TModule::SAt &, const string & );
	    attach = (TModule * (*)(const TModule::SAt &, const string &)) dlsym(h_lib,"attach");
	    if( dlerror() != NULL )
	    {
		dlclose(h_lib);
		throw TError(nodePath().c_str(),_("SO <%s> error: %s !"),iname.c_str(),dlerror());
	    }

	    struct stat file_stat;
	    stat(iname.c_str(),&file_stat);

	    int n_mod=0, add_mod=0;
	    TModule::SAt AtMod;
	    while( (AtMod = (module)( n_mod++ )).id.size() )
	    {
		vector<string> list;
		owner().list(list);
		for( unsigned i_sub = 0; i_sub < list.size(); i_sub++)
		{
		    if( owner().at(list[i_sub]).at().subModule() &&
			AtMod.type == owner().at(list[i_sub]).at().subId() )
		    {
			//-- Check type module version --
			if( AtMod.t_ver != owner().at(list[i_sub]).at().subVer() )
			{
			    mess_warning(nodePath().c_str(),_("%s for type <%s> doesn't support module version: %d!"),
				AtMod.id.c_str(),AtMod.type.c_str(),AtMod.t_ver);
			    break;
			}
			//-- Check module present --
			if( owner().at(list[i_sub]).at().modPresent(AtMod.id) )
			    mess_warning(nodePath().c_str(),_("Module <%s> is already present!"),AtMod.id.c_str());
			else
			{
			    //-- Attach new module --
			    TModule *LdMod = (attach)( AtMod, iname );
			    if( LdMod == NULL )
			    {
				mess_warning(nodePath().c_str(),_("Attach module <%s> error!"),AtMod.id.c_str());
				break;
			    }
			    //-- Add atached module --
			    owner().at(list[i_sub]).at().modAdd(LdMod);
			    SchHD[i_sh].use.push_back( list[i_sub]+"."+LdMod->modId() );
			    if(full)
			    {
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
	    else SchHD[i_sh].hd = h_lib;
	    return;
	}
    throw TError(nodePath().c_str(),_("SO <%s> is not present!"),iname.c_str());
}

void TModSchedul::libDet( const string &iname )
{
    ResAlloc res(nodeRes(),true);
    for( int i_sh = 0; i_sh < SchHD.size(); i_sh++ )
	if( SchHD[i_sh].name == iname && SchHD[i_sh].hd )
	{
	    while( SchHD[i_sh].use.size() )
	    {
		try
		{
		    owner().at(TSYS::strSepParse(SchHD[i_sh].use[0],0,'.')).at().
			    modAt(TSYS::strSepParse(SchHD[i_sh].use[0],1,'.')).at().modStop();
		    owner().at(TSYS::strSepParse(SchHD[i_sh].use[0],0,'.')).at().
			    modDel(TSYS::strSepParse(SchHD[i_sh].use[0],1,'.'));
		}catch(TError err)
		{
		    //owner().at(SchHD[i_sh]->use[0].mod_sub).at().modAt(SchHD[i_sh]->use[0].n_mod).at().load();
		    owner().at(TSYS::strSepParse(SchHD[i_sh].use[0],0,'.')).at().
			    modAt(TSYS::strSepParse(SchHD[i_sh].use[0],1,'.')).at().modStart();
		    throw;
		}
		SchHD[i_sh].use.erase(SchHD[i_sh].use.begin());
	    }
	    dlclose(SchHD[i_sh].hd);
	    SchHD[i_sh].hd = NULL;
	    return;
	}
    throw TError(nodePath().c_str(),_("SO <%s> is not present!"),iname.c_str());
}

bool TModSchedul::chkAllowMod( const string &name )
{
    string sel, nmFile;
    int path_els = 0;

    for( int off = 0; (sel=TSYS::pathLev(name,0,true,&off)).size(); path_els++ ) nmFile = sel;

    if( allowList().empty() )	return false;
    if( TSYS::strNoSpace(allowList()) != "*" )
    {
	for( int off = 0; (sel=TSYS::strSepParse(allowList(),0,';',&off)).size(); )
	    if( TSYS::strNoSpace(sel) == name || TSYS::strNoSpace(sel) == nmFile ) break;
	if( sel.empty() ) return false;
    }
    for( int off = 0; (sel=TSYS::strSepParse(denyList(),0,';',&off)).size(); )
	if( TSYS::strNoSpace(sel) == name || TSYS::strNoSpace(sel) == nmFile ) return false;

    return true;
}

void TModSchedul::libList( vector<string> &list )
{
    ResAlloc res(nodeRes(),false);
    list.clear();
    for(unsigned i_sh = 0; i_sh < SchHD.size(); i_sh++)
	list.push_back( SchHD[i_sh].name );
}

TModSchedul::SHD &TModSchedul::lib( const string &iname )
{
    ResAlloc res(nodeRes(),false);
    //string nm_t = SYS->fNameFix(name);
    for(unsigned i_sh = 0; i_sh < SchHD.size(); i_sh++)
	if( SchHD[i_sh].name == iname )
	    return SchHD[i_sh];
    throw TError(nodePath().c_str(),_("SO <%s> is not present!"),iname.c_str());
}

int TModSchedul::libLoad( const string &iname, bool full )
{
    int ldCnt = 0;
    vector<string> files;

    ScanDir( iname, files );
    for( int i_f = 0; i_f < files.size(); i_f++ )
    {
	int i_sh;
	bool st_auto = chkAllowMod(files[i_f]);
	ResAlloc res(nodeRes(),false);
	for( i_sh = 0; i_sh < SchHD.size(); i_sh++ )
	    if( SchHD[i_sh].name == files[i_f] ) break;
	if( i_sh < SchHD.size() )
	{
	    try { if(st_auto) libDet(files[i_f]); }
	    catch(TError err)
	    {
		mess_warning(err.cat.c_str(),"%s",err.mess.c_str());
		mess_warning(nodePath().c_str(),_("Can't detach library <%s>."),files[i_f].c_str());
		continue;
	    }
	}
	res.release();
	libReg(files[i_f]);
	if(st_auto)
	{
	    try{ libAtt(files[i_f],full); ldCnt++; }
	    catch( TError err ){ mess_warning(err.cat.c_str(),"%s",err.mess.c_str()); }
	}
    }

    return ldCnt;
}

void TModSchedul::cntrCmdProc( XMLNode *opt )
{
    //- Get page info -
    if( opt->name() == "info" )
    {
	TSubSYS::cntrCmdProc(opt);
	if(ctrMkNode("area",opt,0,"/ms",_("Subsystem"),0444,"root","root"))
	{
	    ctrMkNode("fld",opt,-1,"/ms/mod_path",_("Path to shared libs(modules)"),0444,"root","root",1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/ms/mod_allow",_("Allowed modules"),0444,"root","root",2,"tp","str",
		"help",_("List of shared libs(modules) allow for auto conection.\nElements separated by symbol ';'.\nValue '*' is used for allow all modules."));
	    ctrMkNode("fld",opt,-1,"/ms/mod_deny",_("Denied modules"),0444,"root","root",2,"tp","str",
		"help",_("List of shared libs(modules) deny for auto conection.\nElements separated by symbol ';'."));
	    ctrMkNode("fld",opt,-1,"/ms/chk_per",_("Check modules period (sec)"),0664,"root","root",1,"tp","dec");
	    ctrMkNode("comm",opt,-1,"/ms/chk_now",_("Check modules now."),0660,"root","root");
	    if(ctrMkNode("table",opt,-1,"/ms/libs",_("Shared libs(modules)"),0664,"root","root",1,"key","path"))
	    {
		ctrMkNode("list",opt,-1,"/ms/libs/path",_("Path"),0444,"root","root",1,"tp","str");
		ctrMkNode("list",opt,-1,"/ms/libs/tm",_("Time"),0444,"root","root",1,"tp","time");
		ctrMkNode("list",opt,-1,"/ms/libs/mods",_("Modules"),0444,"root","root",1,"tp","str");
		ctrMkNode("list",opt,-1,"/ms/libs/en",_("Enable"),0664,"root","root",1,"tp","bool");
	    }
	}
	ctrMkNode("fld",opt,-1,"/help/g_help",_("Options help"),0440,"root","root",3,"tp","str","cols","90","rows","10");
	return;
    }

    //- Process command to page -
    string a_path = opt->attr("path");
    if( a_path == "/ms/mod_path" && ctrChkNode(opt,"get") )		opt->setText( SYS->modDir() );
    else if( a_path == "/ms/mod_allow" && ctrChkNode(opt,"get") )	opt->setText(allowList());
    else if( a_path == "/ms/mod_deny" && ctrChkNode(opt,"get") )	opt->setText(denyList());
    else if( a_path == "/ms/chk_per" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText(TSYS::int2str(chkPer()));
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	setChkPer(atoi(opt->text().c_str()));
    }
    else if( a_path == "/help/g_help" && ctrChkNode(opt,"get",0440,"root","root",SEQ_RD) )	opt->setText(optDescr());
    else if( a_path == "/ms/chk_now" && ctrChkNode(opt,"set",0660,"root","root",SEQ_WR) )	libLoad(SYS->modDir(),true);
    else if( a_path == "/ms/libs" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )
	{
	    XMLNode *n_nm	= ctrMkNode("list",opt,-1,"/ms/libs/path","");
	    XMLNode *n_tm	= ctrMkNode("list",opt,-1,"/ms/libs/tm","");
	    XMLNode *n_mods	= ctrMkNode("list",opt,-1,"/ms/libs/mods","");
	    XMLNode *n_en	= ctrMkNode("list",opt,-1,"/ms/libs/en","");
	    ResAlloc res(nodeRes(),false);
	    for( int i_sl = 0; i_sl < SchHD.size(); i_sl++ )
	    {
		if( n_nm ) n_nm->childAdd("el")->setText(SchHD[i_sl].name);
		if( n_tm ) n_tm->childAdd("el")->setText(TSYS::TSYS::int2str(SchHD[i_sl].tm));
		if( n_mods )
		{
		    string useLs;
		    for( int i_el = 0; i_el < SchHD[i_sl].use.size(); i_el++ )
			useLs += SchHD[i_sl].use[i_el]+"; ";
		    n_mods->childAdd("el")->setText(useLs);
		}
		if( n_en ) n_en->childAdd("el")->setText(TSYS::TSYS::int2str((bool)SchHD[i_sl].hd));
	    }
	}
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )
	{
	    if( opt->attr("col") == "en" )
		if( atoi(opt->text().c_str()) ) libAtt(opt->attr("key_path"),true);
		else libDet(opt->attr("key_path"));
	}
    }
    else TSubSYS::cntrCmdProc(opt);
}
