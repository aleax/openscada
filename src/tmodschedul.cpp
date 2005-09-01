/***************************************************************************
 *   Copyright (C) 2004 by Roman Savochenko                                *
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
#include "resalloc.h"
#include "tmodschedul.h"

TModSchedul::TModSchedul( TSYS *app ) : 
    TSubSYS(app,"ModSched","Modules sheduler",false), m_stat(false), m_mod_path("./")
{
    hd_res = ResAlloc::resCreate();
}

TModSchedul::~TModSchedul(  )
{
    ResAlloc::resDelete(hd_res);
}

void TModSchedul::preDisable(int flag)
{
    subStop();
	
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
    	    SchHD[i_sh]->hd = NULL;
	}
    res.release();
}

string TModSchedul::optDescr( )
{
    char buf[STR_BUF_LEN];
    snprintf(buf,sizeof(buf),Mess->I18N(
	"=================== The module sheduler subsystem options =================\n"
    	"    --ModPath=<path>   set modules <path>: \"/var/os/modules/\"\n"
	"------------ Parameters of section <%s> in config file -----------\n"
    	"mod_path  <path>       set path to shared libs;\n"
    	"mod_auto  <list>       names of automatic loaded, attached and started shared libs <direct_dbf.so;virt.so>\n\n"
	),nodePath().c_str());
    
    return(buf);
}

void TModSchedul::subStart(  )
{ 
    if( m_stat ) return;
    pthread_attr_t      pthr_attr;

    pthread_attr_init(&pthr_attr);
    pthread_attr_setschedpolicy(&pthr_attr,SCHED_OTHER);
    pthread_create(&pthr_tsk,&pthr_attr,TModSchedul::SchedTask,this);
    pthread_attr_destroy(&pthr_attr);
    if( TSYS::eventWait( m_stat, true, nodePath()+"start",5) )
    	throw TError(nodePath().c_str(),"Thread no started!");
}

void TModSchedul::subStop(  )
{
    if( m_stat )
    {
        m_endrun = true;
        TSYS::eventWait( m_stat, false, nodePath()+"stop");
        pthread_join( pthr_tsk, NULL );
    }
}

void *TModSchedul::SchedTask(void *param)
{
    int cntr = 0;
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED,NULL);

    TModSchedul  *shed = (TModSchedul *)param;
    
    shed->m_stat   = true;
    shed->m_endrun = false;
    
#if OSC_DEBUG
    Mess->put(shed->nodePath().c_str(),TMess::Debug,Mess->I18N("Thread <%d> started!"),getpid() );
#endif
    do 
    {	
	try
	{
	    if( ++cntr >= 10*1000/STD_WAIT_DELAY ) //10 second
	    {
		cntr = 0;		
		shed->libLoad(shed->m_mod_path,true);
	    }
	} catch(TError err){ Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); }
	usleep(STD_WAIT_DELAY*1000);
    } while( !shed->m_endrun );
    shed->m_stat   = false;

    return(NULL);
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
    try{ m_mod_path = ctrId(&SYS->cfgRoot(),nodePath())->childGet("id","mod_path")->text(); }
    catch(...) {  }
    
    try
    {    
	string opt = ctrId(&SYS->cfgRoot(),nodePath())->childGet("id","mod_auto")->text();
	
	int el_cnt = 0;
	m_am_list.clear();
	while(TSYS::strSepParse(opt,el_cnt,':').size())
	{
	    m_am_list.push_back(TSYS::strSepParse(opt,el_cnt,':'));	    
	    el_cnt++;
	}
    }
    catch(...) {  }
}

void TModSchedul::ScanDir( const string &Paths, vector<string> &files, bool new_f )
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
	
#if OSC_DEBUG
    	Mess->put(nodePath().c_str(),TMess::Debug,Mess->I18N("Scan dir <%s>!"),Path.c_str());
#endif  

	// Convert to absolutly path
        //Path = SYS->fNameFix(Path);

        DIR *IdDir = opendir(Path.c_str());
        if(IdDir == NULL) continue;

        while((scan_dirent = readdir(IdDir)) != NULL)
        {
	    if( string("..") == scan_dirent->d_name || string(".") == scan_dirent->d_name ) continue;
            NameMod=Path+"/"+scan_dirent->d_name;
            if( CheckFile(NameMod, new_f) ) files.push_back(NameMod); 
        }
        closedir(IdDir);
	
#if OSC_DEBUG
    	Mess->put(nodePath().c_str(),TMess::Debug,Mess->I18N("Scan dir <%s> ok!"),Path.c_str());
#endif    
	
    } while(id != (int)string::npos);
}

bool TModSchedul::CheckFile( const string &iname, bool new_f )
{
    struct stat file_stat;
    string NameMod;

    stat(iname.c_str(),&file_stat);

    if( (file_stat.st_mode&S_IFMT) != S_IFREG ) return(false);
    if( access(iname.c_str(),F_OK|R_OK) != 0 )  return(false);
    NameMod=iname;
    
    void *h_lib = dlopen(iname.c_str(),RTLD_GLOBAL|RTLD_LAZY);
    if(h_lib == NULL)
    {
        Mess->put(nodePath().c_str(),TMess::Warning,"SO <%s> error: %s !",iname.c_str(),dlerror());
        return(false);
    }
    else dlclose(h_lib);        
    
    if(new_f)
	for(unsigned i_sh=0; i_sh < SchHD.size(); i_sh++)
	    if(SchHD[i_sh]->name == iname && SchHD[i_sh]->m_tm == file_stat.st_mtime) 
		return(false);

    return(true);
}

int  TModSchedul::libReg( const string &name )
{
    struct stat file_stat;

    ResAlloc res(hd_res,true);
    stat(name.c_str(),&file_stat);
    unsigned i_sh;
    for( i_sh = 0; i_sh < SchHD.size(); i_sh++ )
       	if( SchHD[i_sh]->name == name ) 
	{
	    SchHD[i_sh]->m_tm = file_stat.st_mtime;   
	    return(i_sh);    
	}
    SchHD.push_back( new SHD );	
    SchHD[i_sh]->hd   = NULL;   
    SchHD[i_sh]->m_tm = file_stat.st_mtime;   
    SchHD[i_sh]->name = name;       
    
    return(i_sh);    
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
    throw TError(nodePath().c_str(),"SO <%s> no present!",iname.c_str());
}
    
void TModSchedul::libAtt( const string &iname, bool full )
{
    ResAlloc res(hd_res,true);
    for(unsigned i_sh = 0; i_sh < SchHD.size(); i_sh++)
       	if( SchHD[i_sh]->name == iname ) 
	{
	    if( SchHD[i_sh]->hd ) 
		throw TError(nodePath().c_str(),"SO <%s> already attached!",iname.c_str());	    
	    
	    void *h_lib = dlopen(iname.c_str(),RTLD_GLOBAL|RTLD_LAZY);	    
	    if( !h_lib )
		throw TError(nodePath().c_str(),"SO <%s> error: %s !",iname.c_str(),dlerror());	    
	    
	    //Connect to module function
	    TModule::SAt (*module)( int );
	    (void *)module = dlsym(h_lib,"module");
	    if( dlerror() != NULL )
	    {
		dlclose(h_lib);
		throw TError(nodePath().c_str(),"SO <%s> error: %s !",iname.c_str(),dlerror());
	    }    
	    
	    //Connect to attach function	    
	    TModule *(*attach)( const TModule::SAt &, const string & );
	    (void *)attach = dlsym(h_lib,"attach");
	    if( dlerror() != NULL )
	    {
		dlclose(h_lib);
		throw TError(nodePath().c_str(),"SO <%s> error: %s !",iname.c_str(),dlerror());
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
			    Mess->put(nodePath().c_str(),TMess::Warning,"%s for type <%s> no support module version: %d!",
				AtMod.id.c_str(),AtMod.type.c_str(),AtMod.t_ver);
			    break;
			}
			//Check module present
			if( owner().at(list[i_sub]).at().modPresent(AtMod.id) )
			    Mess->put(nodePath().c_str(),TMess::Warning,"Module <%s> already present!",AtMod.id.c_str());
			else
			{
			    //Attach new module
			    TModule *LdMod = (attach)( AtMod, iname );
			    if( LdMod == NULL )
			    {
				Mess->put(nodePath().c_str(),TMess::Warning,"Attach module <%s> error!",AtMod.id.c_str());
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
    throw TError(nodePath().c_str(),"SO <%s> no present!",iname.c_str());
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
		owner().at(SchHD[i_sh]->use[0].mod_sub).at().modDel( SchHD[i_sh]->use[0].n_mod );
		SchHD[i_sh]->use.erase(SchHD[i_sh]->use.begin());
	    }	    
	    dlclose(SchHD[i_sh]->hd);
	    SchHD[i_sh]->hd = NULL;
	    return;
	}
    }
    throw TError(nodePath().c_str(),"SO <%s> no present!",iname.c_str());
}

bool TModSchedul::CheckAuto( const string &name) const
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
    throw TError(nodePath().c_str(),"SO <%s> no present!",iname.c_str());
}

void TModSchedul::libLoad( const string &iname, bool full)
{
    vector<string> files;

    ScanDir( iname, files, true );
    for(unsigned i_f = 0; i_f < files.size(); i_f++)
    {
	unsigned i_sh;
	bool st_auto = CheckAuto(files[i_f]);
    	for( i_sh = 0; i_sh < SchHD.size(); i_sh++ )
	    if( SchHD[i_sh]->name == files[i_f] ) break;
	if(i_sh < SchHD.size())
	{
	    if(st_auto) libDet(files[i_f]);
	}
	libReg(files[i_f]);	
	if(st_auto) 
	{
	    try{ libAtt(files[i_f],full); }
	    catch( TError err ){ Mess->put(err.cat.c_str(),TMess::Warning,err.mess.c_str()); }
	}
    }
}

//==============================================================
//================== Controll functions ========================
//==============================================================
void TModSchedul::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    switch(cmd)
    {
	case TCntrNode::Info:
	    TSubSYS::cntrCmd_( a_path, opt, cmd );       //Call parent

	    ctrInsNode("area",0,opt,a_path.c_str(),"/ms",Mess->I18N("Subsystem"),0440);
	    ctrMkNode("fld",opt,a_path.c_str(),"/ms/mod_path",Mess->I18N("Path to shared libs(modules)"),0664,0,0,"str");
	    ctrMkNode("list",opt,a_path.c_str(),"/ms/mod_auto",Mess->I18N("List of auto conected shared libs(modules)"),0664,0,0,"str")->
		attr_("s_com","add,ins,edit,del");
	    ctrMkNode("fld",opt,a_path.c_str(),"/help/g_help",Mess->I18N("Options help"),0440,0,0,"str")->
		attr_("cols","90")->attr_("rows","5");	
	    break;
	case TCntrNode::Get:
	    if( a_path == "/ms/mod_path" )	ctrSetS( opt, m_mod_path );
	    else if( a_path == "/ms/mod_auto" )
	    {
		opt->childClean();
		for( unsigned i_a=0; i_a < m_am_list.size(); i_a++ )
	    	    ctrSetS( opt, m_am_list[i_a] );
	    }
	    else if( a_path == "/help/g_help" )	ctrSetS( opt, optDescr() );       
	    else TSubSYS::cntrCmd_( a_path, opt, cmd );
	    break;
	case TCntrNode::Set:
	    if( a_path == "/ms/mod_path" ) m_mod_path = ctrGetS( opt );
	    else if( a_path == "/ms/mod_auto" )
	    {
		if( opt->name() == "add" )		
		    m_am_list.push_back(opt->text());
		else if( opt->name() == "ins" )	
		    m_am_list.insert(m_am_list.begin()+atoi(opt->attr("pos").c_str()),opt->text());
		else if( opt->name() == "edit" )
		    m_am_list[atoi(opt->attr("pos").c_str())] = opt->text();
		else if( opt->name() == "del" )
		    m_am_list.erase(m_am_list.begin()+atoi(opt->attr("pos").c_str()));
	    }
	    else TSubSYS::cntrCmd_( a_path, opt, cmd );
	    break;
    }
}

