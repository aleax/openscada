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
#include "tkernel.h"
#include "tmessage.h"
#include "tbds.h"
#include "tarchives.h"
#include "tparams.h"
#include "tparam.h"
#include "tcontroller.h"
#include "tparamcontr.h"
#include "tcontrollers.h"
#include "ttipcontroller.h"
#include "tprotocols.h"
#include "ttransports.h"
#include "tspecials.h"
#include "tvalue.h"
#include "tmodschedul.h"

const char *TModSchedul::o_name = "TModSchedul";
const char *TModSchedul::s_name = "Modules sheduler";

TModSchedul::TModSchedul( TKernel *app ) : m_stat(false), m_owner(app), m_mod_path("./")
{
    hd_res = ResAlloc::resCreate();
}

TModSchedul::~TModSchedul(  )
{
    if( m_stat ) 
    {
    	m_endrun = true;
	SYS->event_wait( m_stat, false, string(o_name)+": The modules scheduler thread is stoping....");
	pthread_join( pthr_tsk, NULL );
    }
    
    //Detach all share libs 
    ResAlloc res(hd_res,true);
    for( unsigned i_sh = 0; i_sh < SchHD.size(); i_sh++ )
	if( SchHD[i_sh]->hd )
	{        
	    while( SchHD[i_sh]->use.size() )
	    {	
		string n_mod = SchHD[i_sh]->use[0].mod.at().modName();
		SchHD[i_sh]->use[0].mod.free();
	       	grpmod[SchHD[i_sh]->use[0].id_tmod]->gmdDel( n_mod );
		SchHD[i_sh]->use.erase(SchHD[i_sh]->use.begin());
	    }	    
	    dlclose(SchHD[i_sh]->hd);
	    SchHD[i_sh]->hd = NULL;
	}	
    res.release();
    
    ResAlloc::resDelete(hd_res);
}

string TModSchedul::name()
{ 
    return(Mess->I18N((char *)s_name)); 
}

string TModSchedul::optDescr( )
{
    char buf[STR_BUF_LEN];
    snprintf(buf,sizeof(buf),Mess->I18N(
	"=================== The module sheduler subsystem options =================\n"
    	"    --ModPath=<path>   set modules <path>: \"/var/os/modules/,./mod/\"\n"
	"------------ Parameters of section <%s> in config file -----------\n"
    	"mod_path  <path>       set path to shared libs;\n"
    	"mod_auto  <list>       names of automatic loaded, attached and started shared libs <direct_dbf.so;virt.so>\n\n"
	),s_name);
    
    return(buf);
}

void TModSchedul::schedStart( )
{ 
    if( m_stat ) return;
    pthread_attr_t      pthr_attr;

    pthread_attr_init(&pthr_attr);
    pthread_attr_setschedpolicy(&pthr_attr,SCHED_OTHER);
    pthread_create(&pthr_tsk,&pthr_attr,TModSchedul::SchedTask,this);
    pthread_attr_destroy(&pthr_attr);
    if( SYS->event_wait( m_stat, true, string(o_name)+": The modules scheduler thread is starting....",5) )
    	throw TError("%s: The modules scheduler thread no started!",o_name);
}

void *TModSchedul::SchedTask(void *param)
{
    int cntr = 0;
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED,NULL);

    TModSchedul  *shed = (TModSchedul *)param;
    
    shed->m_stat   = true;
    shed->m_endrun = false;
    
#if OSC_DEBUG
    shed->owner().mPut("DEBUG",MESS_DEBUG,"%s:Thread <%d>!",s_name,getpid() );
#endif
    do 
    {	
	try
	{
	    if( ++cntr >= 10*1000/STD_WAIT_DELAY ) //10 second
	    {
		cntr = 0;
		
		shed->libLoad(shed->m_mod_path,-1,true);
		for(unsigned i_gm=0; i_gm < shed->grpmod.size(); i_gm++)
		    shed->libLoad(shed->grpmod[i_gm]->gmdModPath(),i_gm,true);
	    }
	} catch(TError err){ shed->owner().mPut("SYS",MESS_ERR,"%s:%s",s_name,err.what().c_str()); }
	usleep(STD_WAIT_DELAY*1000);
    } while( !shed->m_endrun );
    shed->m_stat   = false;

    return(NULL);
}

int TModSchedul::gmdReg(TGRPModule *gmod)
{
    if(gmod == NULL) return(-1);
    for(unsigned i_grmd = 0; i_grmd < grpmod.size(); i_grmd++)
	if(grpmod[i_grmd] == gmod) return(i_grmd);
    grpmod.push_back(gmod);

    return(grpmod.size()-1);
}

int TModSchedul::gmdUnReg(TGRPModule *gmod)
{
    if(gmod == NULL) return(-1);
    for(unsigned i_grmd = 0; i_grmd < grpmod.size(); i_grmd++)
	if(grpmod[i_grmd] == gmod)
	{
	    grpmod.erase(grpmod.begin()+i_grmd);
	    return(0);
	}

    return(-2);
}

XMLNode *TModSchedul::cfgNode()
{
    int i_k = 0;
    while(true)
    {
	XMLNode *t_n = owner().cfgNode()->childGet("section",i_k++);
	if( t_n->attr("id") == s_name ) return( t_n );
    }
}

void TModSchedul::checkCommandLine(  )
{
#if OSC_DEBUG
    owner().mPut("DEBUG",MESS_INFO,"%s:Read commandline options!",s_name);
#endif
	
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
    
#if OSC_DEBUG
    owner().mPut("DEBUG",MESS_DEBUG,"%s:Read commandline options ok!",s_name);
#endif

    // Check all subsystems
    for(unsigned i_gm=0; i_gm < grpmod.size(); i_gm++)
	grpmod[i_gm]->gmdCheckCommandLine( );
}

void TModSchedul::checkCommandLineMod(  )
{
    for(unsigned i_gm=0; i_gm < grpmod.size(); i_gm++)
	grpmod[i_gm]->gmdCheckCommandLineMods();
}

void TModSchedul::updateOpt()
{
    try{ m_mod_path = cfgNode()->childGet("id","mod_path")->text(); }
    catch(...) {  }
    
    try
    {
	string opt = cfgNode()->childGet("id","mod_auto")->text();
	if( opt.size() )
	{
	    int i_beg = -1;
    	    m_am_list.clear();
	    do
	    {
		m_am_list.push_back(opt.substr(i_beg+1,opt.find(";",i_beg+1)-i_beg-1));
		i_beg = opt.find(";",i_beg+1);
	    } while(i_beg != (int)string::npos);
	}
    }
    catch(...) {  }
    
    for(unsigned i_gm=0; i_gm < grpmod.size(); i_gm++)
	grpmod[i_gm]->gmdUpdateOpt();
}

void TModSchedul::updateOptMod()
{
    for(unsigned i_gm=0; i_gm < grpmod.size(); i_gm++)
	grpmod[i_gm]->gmdUpdateOptMods();
}

void TModSchedul::gmdLoadAll(  )
{
    libLoad(m_mod_path,-1,false);
    for(unsigned i_gm=0; i_gm < grpmod.size(); i_gm++)
	libLoad(grpmod[i_gm]->gmdModPath(),i_gm,false);
}

void TModSchedul::gmdInitAll(  )
{
    for(unsigned i_gm=0; i_gm < grpmod.size(); i_gm++)
	grpmod[i_gm]->gmdInit( );
}

void TModSchedul::gmdStartAll(  )
{
    for(unsigned i_gm=0; i_gm < grpmod.size(); i_gm++)
    {
	try{ grpmod[i_gm]->gmdStart( ); }
	catch(...){ }
    }
}

void TModSchedul::ScanDir( const string &Paths, vector<string> &files, bool new_f ) const
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
    	owner().mPut("DEBUG",MESS_INFO,"%s:Scan dir <%s> !",s_name,Path.c_str());
#endif  

	// Convert to absolutly path
        //Path = SYS->FixFName(Path);

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
    	owner().mPut("DEBUG",MESS_DEBUG,"%s:Scan dir <%s> ok !",s_name,Path.c_str());
#endif    
	
    } while(id != (int)string::npos);
}

bool TModSchedul::CheckFile( const string &name, bool new_f ) const
{
    struct stat file_stat;
    string NameMod;

    stat(name.c_str(),&file_stat);

    if( (file_stat.st_mode&S_IFMT) != S_IFREG ) return(false);
    if( access(name.c_str(),F_OK|R_OK) != 0 )      return(false);
    NameMod=name;
    
    void *h_lib = dlopen(name.c_str(),RTLD_GLOBAL|RTLD_LAZY);
    if(h_lib == NULL)
    {
        owner().mPut("SYS",MESS_WARNING,"%s:SO %s error: %s !",s_name,name.c_str(),dlerror());
        return(false);
    }
    else dlclose(h_lib);        
    
    if(new_f)
	for(unsigned i_sh=0; i_sh < SchHD.size(); i_sh++)
	    if(SchHD[i_sh]->name == name && SchHD[i_sh]->m_tm == file_stat.st_mtime) 
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

void TModSchedul::libUnreg( const string &name )
{
    ResAlloc res(hd_res,true);
    for(unsigned i_sh = 0; i_sh < SchHD.size(); i_sh++)
       	if( SchHD[i_sh]->name == name ) 
	{
	    if( SchHD[i_sh]->hd ) libDet( name );
	    delete SchHD[i_sh];
	    SchHD.erase(SchHD.begin()+i_sh);
	    return;
	}
    throw TError("%s: SO <%s> no avoid!",o_name,name.c_str());
}
    
void TModSchedul::libAtt( const string &name, bool full, int dest )
{
    ResAlloc res(hd_res,true);
    for(unsigned i_sh = 0; i_sh < SchHD.size(); i_sh++)
       	if( SchHD[i_sh]->name == name ) 
	{
	    if( SchHD[i_sh]->hd ) 
		throw TError("%s: SO <%s> already attached!",o_name,name.c_str());	    
	    
	    void *h_lib = dlopen(name.c_str(),RTLD_GLOBAL|RTLD_LAZY);	    
	    if( !h_lib )
		throw TError("%s: SO <%s> error: %s !",o_name,name.c_str(),dlerror());	    
	    
	    //Connect to module function
	    TModule::SAt (*module)( int );
	    (void *)module = dlsym(h_lib,"module");
	    if( dlerror() != NULL )
	    {
		dlclose(h_lib);
		throw TError("%s: SO <%s> error: %s !",o_name,name.c_str(),dlerror());
	    }    
	    
	    //Connect to attach function	    
	    TModule *(*attach)( const TModule::SAt &, const string & );
	    (void *)attach = dlsym(h_lib,"attach");
	    if( dlerror() != NULL )
	    {
		dlclose(h_lib);
		throw TError("%s: SO <%s> error: %s !",o_name,name.c_str(),dlerror());
	    }    
	    
	    struct stat file_stat;
	    stat(name.c_str(),&file_stat);
    
	    int n_mod=0, add_mod=0;
	    TModule::SAt AtMod;
	    while( (AtMod = (module)( n_mod++ )).id.size() )
	    {
		for( unsigned i_grm = 0; i_grm < grpmod.size(); i_grm++)
		{
		    if(dest >= 0) i_grm = dest;
		    if( AtMod.type == grpmod[i_grm]->gmdName() )
		    { 
			//Check type module version
			if( AtMod.t_ver != grpmod[i_grm]->gmdVer() )
			{
			    owner().mPut("SYS",MESS_WARNING,"%s:%s for type <%s> no support module version: %d!",
				s_name,AtMod.id.c_str(),AtMod.type.c_str(),AtMod.t_ver);
			    break;
			}
			//Check avoid module
			try{ grpmod[i_grm]->gmdAt( AtMod.id ); }
			catch(TError)
			{
			    //Attach new module
			    TModule *LdMod = (attach)( AtMod, name );
			    if( LdMod == NULL )
			    {
				owner().mPut("SYS",MESS_WARNING,"%s:Attach module <%s> error!",s_name,AtMod.id.c_str());
				break;
			    }
			    //Add atached module
			    grpmod[i_grm]->gmdAdd(LdMod);
			    SUse t_suse = { i_grm, grpmod[i_grm]->gmdAt(LdMod->modName()) };
			    SchHD[i_sh]->use.push_back( t_suse );
			    if(full)
			    {
				grpmod[i_grm]->gmdInit();
				grpmod[i_grm]->gmdStart();
			    }
			    add_mod++;
			    break;
			}
			owner().mPut("SYS",MESS_WARNING,"%s:Module %s already avoid!",s_name,AtMod.id.c_str());		    
		    }
		    if(dest >= 0) break;
		}
	    }
	    if(add_mod == 0) dlclose(h_lib);	    
	    else SchHD[i_sh]->hd = h_lib;
	    return;
	}
    throw TError("%s: SO <%s> no avoid!",o_name,name.c_str());
}

void TModSchedul::libDet( const string &name )
{
    ResAlloc res(hd_res,true);
    for(unsigned i_sh = 0; i_sh < SchHD.size(); i_sh++)
    {
       	if( SchHD[i_sh]->name == name && SchHD[i_sh]->hd )
	{
	    
    	    while( SchHD[i_sh]->use.size() )
	    {
		string n_mod = SchHD[i_sh]->use[0].mod.at().modName();
		SchHD[i_sh]->use[0].mod.free();
	       	grpmod[SchHD[i_sh]->use[0].id_tmod]->gmdDel( n_mod );
		SchHD[i_sh]->use.erase(SchHD[i_sh]->use.begin());
	    }	    
	    dlclose(SchHD[i_sh]->hd);
	    SchHD[i_sh]->hd = NULL;
	    return;
	}
    }
    throw TError("%s: SO %s no avoid!",o_name,name.c_str());
}

bool TModSchedul::CheckAuto( const string &name) const
{
    if( m_am_list.size() == 1 && m_am_list[0] == "*") return(true);
    else 
	for( unsigned i_au = 0; i_au < m_am_list.size(); i_au++)
	    if( name == m_am_list[i_au] ) return(true);
	    //if( name == SYS->FixFName( m_am_list[i_au] ) ) return(true);
    return(false);
}

void TModSchedul::libList( vector<string> &list )
{  
    ResAlloc res(hd_res,false);
    list.clear();
    for(unsigned i_sh = 0; i_sh < SchHD.size(); i_sh++)
       	list.push_back( SchHD[i_sh]->name );
}

TModSchedul::SHD TModSchedul::lib( const string &name )
{
    ResAlloc res(hd_res,false);
    //string nm_t = SYS->FixFName(name);
    for(unsigned i_sh = 0; i_sh < SchHD.size(); i_sh++)
       	if( SchHD[i_sh]->name == name ) 
	    return *SchHD[i_sh];
    throw TError("%s: SO <%s> no avoid!",o_name,name.c_str());
}

void TModSchedul::libLoad( const string &name, int dest, bool full)
{
    vector<string> files;

    ScanDir( name, files, true );
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
	    try{ libAtt(files[i_f],full,dest); }
	    catch( TError err ){ owner().mPut("SYS",MESS_WARNING,"%s:%s",s_name,err.what().c_str()); }
	}
    }
}

//==============================================================
//================== Controll functions ========================
//==============================================================
void TModSchedul::ctrStat_( XMLNode *inf )
{
    char *i_cntr = 
	"<oscada_cntr>"
	" <area id='a_ms' dscr='Subsystem control.' acs='0440'>"
	"  <fld id='mod_path' com='1' cfg='1' dest='dir' tp='str'/>"
	"  <list id='mod_auto' tp='str' dest='file' s_com='add,ins,edit,del'/>"
	" </area>"
	" <area id='help'>"
	"  <fld id='g_help' acs='0440' tp='str' cols='90' rows='5'/>"
	" </area>"	
	"</oscada_cntr>";
    char *dscr = "dscr";
    
    XMLNode *c_nd;
    inf->load( i_cntr );
    inf->text(Mess->I18N("Module sheduler subsystem"));
    //a_ms
    c_nd = inf->childGet(0);
    c_nd->attr(dscr,Mess->I18N("Subsystem"));
    c_nd->childGet(0)->attr(dscr,Mess->I18N("Path to shared libs(modules)"));
    c_nd->childGet(1)->attr(dscr,Mess->I18N("List of auto conected shared libs(modules)"));
    c_nd = inf->childGet(1);
    c_nd->attr(dscr,Mess->I18N("Help"));
    c_nd->childGet(0)->attr(dscr,Mess->I18N("Options help"));    
}

void TModSchedul::ctrDinGet_( const string &a_path, XMLNode *opt )
{
    if( a_path == "/a_ms/mod_path" )	ctrSetS( opt, m_mod_path );
    else if( a_path == "/a_ms/mod_auto" )
    {
	opt->childClean();
	for( unsigned i_a=0; i_a < m_am_list.size(); i_a++ )
	    ctrSetS( opt, m_am_list[i_a], i_a );
    }
    else if( a_path == "/help/g_help" )	ctrSetS( opt, optDescr() );       
    else throw TError("(%s) Branch %s error!",o_name,a_path.c_str());
} 

void TModSchedul::ctrDinSet_( const string &a_path, XMLNode *opt )
{
    if( a_path == "/a_ms/mod_path" ) m_mod_path = ctrGetS( opt );
    else if( a_path.substr(0,14) == "/a_ms/mod_auto" )
	for( int i_el=0; i_el < opt->childSize(); i_el++)
	{
	    XMLNode *t_c = opt->childGet(i_el);
	    if( t_c->name() == "el")
	    {
		if(t_c->attr("do") == "add")      
		    m_am_list.push_back(t_c->text());
		else if(t_c->attr("do") == "ins") 
		    m_am_list.insert(m_am_list.begin()+atoi(t_c->attr("id").c_str()),t_c->text()); 
		else if(t_c->attr("do") == "edit") 
		    m_am_list[atoi(t_c->attr("id").c_str())] = t_c->text();  
		else if(t_c->attr("do") == "del") 
		    m_am_list.erase(m_am_list.begin()+atoi(t_c->attr("id").c_str()));
	    }
	}
    else throw TError("(%s) Branch %s error!",o_name,a_path.c_str());
}
