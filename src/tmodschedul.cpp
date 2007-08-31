
//OpenSCADA system file: tmodschedul.cpp
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

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include <dirent.h>
#include <stdlib.h>
#include <signal.h>
#include <getopt.h>

#include <string>

#include "tsys.h"
#include "tmodschedul.h"

TModSchedul::TModSchedul( ) : 
    TSubSYS("ModSched","Modules sheduler",false), prc_st(false), m_mod_path("./"), m_per(10)
{
    //Create calc timer
    struct sigevent sigev;
    sigev.sigev_notify = SIGEV_THREAD;
    sigev.sigev_value.sival_ptr = this;
    sigev.sigev_notify_function = SchedTask;
    sigev.sigev_notify_attributes = NULL;
    timer_create(CLOCK_REALTIME,&sigev,&tmId);
}

TModSchedul::~TModSchedul(  )
{
    timer_delete(tmId);
}

void TModSchedul::preDisable(int flag)
{
    //Detach all share libs
    ResAlloc res(hd_res,true);
    for( unsigned i_sh = 0; i_sh < SchHD.size(); i_sh++ )
        if( SchHD[i_sh]->hd )
        {
            while( SchHD[i_sh]->use.size() )
            {
                owner().at(SchHD[i_sh]->use[0].mod_sub).at().modDel( SchHD[i_sh]->use[0].n_mod );
                SchHD[i_sh]->use.erase(SchHD[i_sh]->use.begin());
    	    }
            dlclose(SchHD[i_sh]->hd);
	    delete SchHD[i_sh];
	}
    SchHD.clear();	
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
    	"ModAuto  <list>        List of automatic loaded, attached and started shared libraries (direct_dbf.so;virt.so).\n"
	"ChkPer   <sec>         Period of checking at new shared libraries(modules).\n\n"
	),nodePath().c_str());
    
    return(buf);
}

void TModSchedul::subStart(  )
{ 
#if OSC_DEBUG
    mess_debug(nodePath().c_str(),_("Start subsystem."));
#endif

    //Start interval timer for periodic thread creating
    struct itimerspec itval;
    itval.it_interval.tv_sec = itval.it_value.tv_sec = m_per;
    itval.it_interval.tv_nsec = itval.it_value.tv_nsec = 0;
    timer_settime(tmId, 0, &itval, NULL);
}

void TModSchedul::subStop(  )
{
#if OSC_DEBUG
    mess_debug(nodePath().c_str(),_("Stop subsystem."));
#endif

    //Stop interval timer for periodic thread creating
    struct itimerspec itval;
    itval.it_interval.tv_sec = itval.it_interval.tv_nsec =
    	itval.it_value.tv_sec = itval.it_value.tv_nsec = 0;
    timer_settime(tmId, 0, &itval, NULL);
    if( TSYS::eventWait( prc_st, false, nodePath()+"stop",20) )
	throw TError(nodePath().c_str(),_("Module scheduler thread no stoped!"));

#if OSC_DEBUG
    mess_debug(nodePath().c_str(),_("Stop subsystem. OK"));
#endif    
}

void TModSchedul::chkPer( int per )
{
    m_per = per;
    struct itimerspec itval;
    itval.it_interval.tv_sec = itval.it_value.tv_sec = m_per;
    itval.it_interval.tv_nsec = itval.it_value.tv_nsec = 0;
    timer_settime(tmId, 0, &itval, NULL);
}

void TModSchedul::SchedTask(union sigval obj)
{
    TModSchedul  *shed = (TModSchedul *)obj.sival_ptr;
    if( shed->prc_st )  return;
    shed->prc_st = true;
    
    try
    {
	shed->libLoad(shed->m_mod_path,true);
    } catch(TError err){ mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }

    shed->prc_st = false;
}

void TModSchedul::loadLibS(  )
{
    libLoad(m_mod_path,false);
}

void TModSchedul::subLoad( )
{
    //===================== Load parameters from command line ================================
    int next_opt;
    char *short_opt="h";
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
	    case 'h': fprintf(stdout,optDescr().c_str()); break;
	    case 'm': m_mod_path = optarg; break;
	    case -1 : break;
	}
    } while(next_opt != -1);
    
    //===================== Load parameters from command line ================================
    m_per = atoi(TBDS::genDBGet(nodePath()+"ChkPer",TSYS::int2str(m_per)).c_str());
    m_mod_path = TBDS::genDBGet(nodePath()+"ModPath",m_mod_path);
    

    string opt = TBDS::genDBGet(nodePath()+"ModAuto");
    string ovl;    
    m_am_list.clear();    
    for( int el_off = 0; (ovl=TSYS::strSepParse(opt,0,';',&el_off)).size(); )
        m_am_list.push_back(ovl);
}

void TModSchedul::subSave( )
{
    TBDS::genDBSet(nodePath()+"ChkPer",TSYS::int2str(m_per));
    TBDS::genDBSet(nodePath()+"ModPath",m_mod_path);
    string m_auto;
    for(int i_a = 0; i_a < m_am_list.size(); i_a++ )
	m_auto+=m_am_list[i_a]+";";
    TBDS::genDBSet(nodePath()+"ModAuto",m_auto);
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
            NameMod=Path+"/"+scan_dirent->d_name;
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

    if( (file_stat.st_mode&S_IFMT) != S_IFREG ) return false;
    if( access(iname.c_str(),F_OK|R_OK) != 0 )  return false;
    NameMod=iname;
    
    void *h_lib = dlopen(iname.c_str(),RTLD_LAZY|RTLD_GLOBAL);
    if(h_lib == NULL)
    {
        mess_warning(nodePath().c_str(),_("SO <%s> error: %s !"),iname.c_str(),dlerror());
        return(false);
    }
    else dlclose(h_lib);        

    for(unsigned i_sh=0; i_sh < SchHD.size(); i_sh++)
        if(SchHD[i_sh]->name == iname )
	    if(file_stat.st_mtime > SchHD[i_sh]->m_tm) return true;
	    else return false;

    return true;
}

int TModSchedul::libReg( const string &name )
{
    struct stat file_stat;

    ResAlloc res(hd_res,true);
    stat(name.c_str(),&file_stat);
    unsigned i_sh;
    for( i_sh = 0; i_sh < SchHD.size(); i_sh++ )
       	if( SchHD[i_sh]->name == name ) break;
    if( i_sh == SchHD.size() )
    {
	SHD *so_t = new SHD();
	so_t->hd = NULL;
	so_t->m_tm = file_stat.st_mtime;
	so_t->name = name;
	SchHD.push_back( so_t );
    }
    else SchHD[i_sh]->m_tm = file_stat.st_mtime;
    
    return i_sh;    
}

void TModSchedul::libUnreg( const string &iname )
{
    ResAlloc res(hd_res,true);
    for(unsigned i_sh = 0; i_sh < SchHD.size(); i_sh++)
       	if( SchHD[i_sh]->name == iname ) 
	{
	    if( SchHD[i_sh]->hd ) libDet( iname );
	    delete SchHD[i_sh];
	    SchHD.erase(SchHD.begin()+i_sh);
	    return;
	}
    throw TError(nodePath().c_str(),_("SO <%s> no present!"),iname.c_str());
}
    
void TModSchedul::libAtt( const string &iname, bool full )
{
    ResAlloc res(hd_res,true);
    for(unsigned i_sh = 0; i_sh < SchHD.size(); i_sh++)
       	if( SchHD[i_sh]->name == iname ) 
	{
	    if( SchHD[i_sh]->hd ) 
		throw TError(nodePath().c_str(),_("SO <%s> already attached!"),iname.c_str());	    
	    
	    void *h_lib = dlopen(iname.c_str(),RTLD_LAZY|RTLD_GLOBAL);
	    if( !h_lib )
		throw TError(nodePath().c_str(),_("SO <%s> error: %s !"),iname.c_str(),dlerror());	    
	    
	    //Connect to module function
	    TModule::SAt (*module)( int );
	    module = (TModule::SAt (*)(int)) dlsym(h_lib,"module");
	    if( dlerror() != NULL )
	    {
		dlclose(h_lib);
		throw TError(nodePath().c_str(),_("SO <%s> error: %s !"),iname.c_str(),dlerror());
	    }    
	    
	    //Connect to attach function	    
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
			//Check type module version
			if( AtMod.t_ver != owner().at(list[i_sub]).at().subVer() )
			{
			    mess_warning(nodePath().c_str(),_("%s for type <%s> no support module version: %d!"),
				AtMod.id.c_str(),AtMod.type.c_str(),AtMod.t_ver);
			    break;
			}
			//Check module present
			if( owner().at(list[i_sub]).at().modPresent(AtMod.id) )
			    mess_warning(nodePath().c_str(),_("Module <%s> already present!"),AtMod.id.c_str());
			else
			{
			    //Attach new module
			    TModule *LdMod = (attach)( AtMod, iname );
			    if( LdMod == NULL )
			    {
				mess_warning(nodePath().c_str(),_("Attach module <%s> error!"),AtMod.id.c_str());
				break;
			    }
			    //Add atached module
			    owner().at(list[i_sub]).at().modAdd(LdMod);
			    SUse t_suse = { list[i_sub], LdMod->modId() };
			    SchHD[i_sh]->use.push_back( t_suse );
			    if(full)
			    {
				owner().at(list[i_sub]).at().subLoad();
				owner().at(list[i_sub]).at().subStart();
			    }
			    add_mod++;
			    break;
			}
		    }
		}
	    }
	    if(add_mod == 0) dlclose(h_lib);	    
	    else SchHD[i_sh]->hd = h_lib;
	    return;
	}
    throw TError(nodePath().c_str(),_("SO <%s> no present!"),iname.c_str());
}

void TModSchedul::libDet( const string &iname )
{
    ResAlloc res(hd_res,true);
    for(unsigned i_sh = 0; i_sh < SchHD.size(); i_sh++)
    {
       	if( SchHD[i_sh]->name == iname && SchHD[i_sh]->hd )
	{
    	    while( SchHD[i_sh]->use.size() )
	    {
		try
		{
		    owner().at(SchHD[i_sh]->use[0].mod_sub).at().modAt(SchHD[i_sh]->use[0].n_mod).at().modStop();
		    owner().at(SchHD[i_sh]->use[0].mod_sub).at().modDel( SchHD[i_sh]->use[0].n_mod );
		}catch(TError err)
		{   
		    //owner().at(SchHD[i_sh]->use[0].mod_sub).at().modAt(SchHD[i_sh]->use[0].n_mod).at().modLoad();
		    owner().at(SchHD[i_sh]->use[0].mod_sub).at().modAt(SchHD[i_sh]->use[0].n_mod).at().modStart();
		    throw;
		}		
		SchHD[i_sh]->use.erase(SchHD[i_sh]->use.begin());
	    }	    
	    dlclose(SchHD[i_sh]->hd);
	    SchHD[i_sh]->hd = NULL;
	    return;
	}
    }
    throw TError(nodePath().c_str(),_("SO <%s> no present!"),iname.c_str());
}

bool TModSchedul::CheckAuto( const string &name ) const
{
    if( m_am_list.size() == 1 && m_am_list[0] == "*") return(true);
    else 
	for( unsigned i_au = 0; i_au < m_am_list.size(); i_au++)
	    if( name == m_am_list[i_au] ) return(true);
	    //if( name == SYS->fNameFix( m_am_list[i_au] ) ) return(true);
    return(false);
}

void TModSchedul::libList( vector<string> &list )
{  
    ResAlloc res(hd_res,false);
    list.clear();
    for(unsigned i_sh = 0; i_sh < SchHD.size(); i_sh++)
       	list.push_back( SchHD[i_sh]->name );
}

TModSchedul::SHD &TModSchedul::lib( const string &iname )
{
    ResAlloc res(hd_res,false);
    //string nm_t = SYS->fNameFix(name);
    for(unsigned i_sh = 0; i_sh < SchHD.size(); i_sh++)
       	if( SchHD[i_sh]->name == iname ) 
	    return *SchHD[i_sh];
    throw TError(nodePath().c_str(),_("SO <%s> no present!"),iname.c_str());
}

void TModSchedul::libLoad( const string &iname, bool full)
{
    vector<string> files;

    ScanDir( iname, files );
    for(unsigned i_f = 0; i_f < files.size(); i_f++)
    {
	unsigned i_sh;
	bool st_auto = CheckAuto(files[i_f]);
    	for( i_sh = 0; i_sh < SchHD.size(); i_sh++ )
	    if( SchHD[i_sh]->name == files[i_f] ) break;
	if(i_sh < SchHD.size())
	{
	    try { if(st_auto) libDet(files[i_f]); }
	    catch(TError err) 
	    { 
		mess_warning(err.cat.c_str(),"%s",err.mess.c_str());
		mess_warning(nodePath().c_str(),_("Can't detach library <%s>."),files[i_f].c_str());
		continue;
	    }
	}
	libReg(files[i_f]);	
	if(st_auto) 
	{
	    try{ libAtt(files[i_f],full); }
	    catch( TError err ){ mess_warning(err.cat.c_str(),"%s",err.mess.c_str()); }
	}
    }
}

void TModSchedul::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
	TSubSYS::cntrCmdProc(opt);
        if(ctrMkNode("area",opt,0,"/ms",_("Subsystem"),0444,"root","root"))
	{
	    ctrMkNode("fld",opt,-1,"/ms/chk_per",_("Check modules period (sec)"),0664,"root","root",1,"tp","dec");
	    ctrMkNode("comm",opt,-1,"/ms/chk_now",_("Check modules now."),0660,"root","root");
	    ctrMkNode("fld",opt,-1,"/ms/mod_path",_("Path to shared libs(modules)"),0664,"root","root",1,"tp","str");
	    ctrMkNode("list",opt,-1,"/ms/mod_auto",_("List of auto conected shared libs(modules)"),0664,"root","root",2,"tp","str","s_com","add,ins,edit,del");
	    ctrMkNode("comm",opt,-1,"/ms/load",_("Load"),0660,"root","root");
    	    ctrMkNode("comm",opt,-1,"/ms/save",_("Save"),0660,"root","root");
	}
	ctrMkNode("fld",opt,-1,"/help/g_help",_("Options help"),0440,"root","root",3,"tp","str","cols","90","rows","10");
        return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/ms/chk_per" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText(TSYS::int2str(m_per));
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	chkPer(atoi(opt->text().c_str()));
    }
    else if( a_path == "/ms/mod_path" )	
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText(m_mod_path);
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	m_mod_path = opt->text();
    }
    else if( a_path == "/ms/mod_auto" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )
	    for( unsigned i_a=0; i_a < m_am_list.size(); i_a++ )
    		opt->childAdd("el")->setText(m_am_list[i_a]);
	if( ctrChkNode(opt,"add",0664,"root","root",SEQ_WR) )	m_am_list.push_back(opt->text());
	if( ctrChkNode(opt,"ins",0664,"root","root",SEQ_WR) )	m_am_list.insert(m_am_list.begin()+atoi(opt->attr("pos").c_str()),opt->text());
	if( ctrChkNode(opt,"edit",0664,"root","root",SEQ_WR) )	m_am_list[atoi(opt->attr("pos").c_str())] = opt->text();
	if( ctrChkNode(opt,"del",0664,"root","root",SEQ_WR) )	m_am_list.erase(m_am_list.begin()+atoi(opt->attr("pos").c_str()));
    }		
    else if( a_path == "/help/g_help" && ctrChkNode(opt,"get",0440,"root","root",SEQ_RD) )	opt->setText(optDescr());
    else if( a_path == "/ms/chk_now" && ctrChkNode(opt,"set",0660,"root","root",SEQ_WR) )	libLoad(m_mod_path,true);
    else if( a_path == "/ms/load" && ctrChkNode(opt,"set",0660,"root","root",SEQ_WR) )		subLoad( );
    else if( a_path == "/ms/save" && ctrChkNode(opt,"set",0660,"root","root",SEQ_WR) )		subSave( );           
    else TSubSYS::cntrCmdProc(opt);
}
