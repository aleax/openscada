/***************************************************************************
    base.cpp  -  description
    -------------------
    copyright            : (C) 2003 by Roman Savochenko
    email                : rom_as@fromru,com
***************************************************************************/

/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <signal.h>

#include <getopt.h>
#include <string>

#include "../../tsys.h"
#include "../../tkernel.h"
#include "../../tmessage.h"
#include "base.h"

//============ Modul info! =====================================================
#define NAME_MODUL  "base_arh"
#define NAME_TYPE   "Arhiv"
#define VER_TYPE    VER_ARH
#define VERSION     "0.0.3"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "The Arhive module support base function of message arhiving."
#define LICENSE     "GPL"
//==============================================================================

extern "C"
{ 
    SAtMod module( int n_mod )
    {
	SAtMod AtMod;
	
	if(n_mod==0) 
	{
	    AtMod.name  = NAME_MODUL;
	    AtMod.type  = NAME_TYPE;
	    AtMod.t_ver = VER_TYPE;
	}
	else
	    AtMod.name  = "";
	return( AtMod );    
    }
    
    TModule *attach( SAtMod &AtMod, string source )
    {
	BaseArh::TMArhive *self_addr = NULL;
	
	if( AtMod.name == NAME_MODUL && AtMod.type == NAME_TYPE && AtMod.t_ver == VER_TYPE ) 
	    self_addr = new BaseArh::TMArhive( source );
	    
	return ( self_addr );
    }    
}

using namespace BaseArh;

//==============================================================================
//================= BaseArh::TMArhive ==========================================
//==============================================================================
TMArhive::TMArhive(string name) : m_mess_max_size(0), m_mess_numb_file(5), m_mess_time_size(7), m_mess_charset("UTF8")
{
    NameModul = NAME_MODUL;
    NameType  = NAME_TYPE;
    Vers      = VERSION;
    Autors    = AUTORS;
    DescrMod  = DESCRIPTION;
    License   = LICENSE;
    Source    = name;
}

TMArhive::~TMArhive()
{

}

void TMArhive::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "============== Module %s command line options =======================\n"
    "------------------ Fields <%s> sections of config file --------------\n"
    " mess_charset      <name>    - set charset <name> of arhive (default UTF8);\n"
    " mess_max_size     <size>    - maximum <size> kb of message arhive file (0 - unlimited default);\n"
    " mess_numb_file    <number>  - number of message arhive files (5 - default);\n"
    " mess_time_size    <days>    - number days to one message file (7 days - default);\n"
    " mess_timeout_free <min>     - timeout of free message file buffer. Timeout no access (10 min default);\n"
    "\n",NAME_MODUL,NAME_MODUL);
}

void TMArhive::mod_CheckCommandLine( ) 
{
    TModule::mod_CheckCommandLine();
    
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{NULL        ,0,NULL,0  }
    };

    optind=opterr=0;
    do
    {
	next_opt=getopt_long(SYS->argc,(char * const *)SYS->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': pr_opt_descr(stdout); break;
	    case -1 : break;
	}
    } while(next_opt != -1);
}

void TMArhive::mod_UpdateOpt()
{
    TModule::mod_UpdateOpt();
    
    int val;
    try
    { 
	val = atoi( mod_XMLCfgNode()->get_child("mess_max_size")->get_text().c_str() ); 
    	if(val >= 0) m_mess_max_size = val;
    }
    catch(...){ }
    try
    { 
	val = atoi( mod_XMLCfgNode()->get_child("mess_numb_file")->get_text().c_str() ); 
    	if(val > 0) m_mess_numb_file = val;
    }
    catch(...){ }
    try
    {
	val = atoi( mod_XMLCfgNode()->get_child("mess_time_size")->get_text().c_str() ); 
    	if(val > 0) m_mess_time_size = val;
    }
    catch(...){ }
    try{ m_mess_charset = mod_XMLCfgNode()->get_child("mess_charset")->get_text(); }
    catch(...){ }
    try
    {
	val = atoi( mod_XMLCfgNode()->get_child("mess_timeout_free")->get_text().c_str() ); 
    	if(val > 0) m_mess_timeout_free = val;
    }
    catch(...){ }
}

void TMArhive::mod_connect(  )
{
    TModule::mod_connect(  );
}

TArhiveMess *TMArhive::AMess(string name, string addr, string categories )
{
    return( new	TMessArh(name, addr, categories,this) );
}

//==============================================================================
//================= BaseArh::TMessArh ==========================================
//==============================================================================
TMessArh::TMessArh( string name, string addr, string categoris, TTipArhive *owner ) : 
    TArhiveMess( name, addr, categoris, owner ), m_stat(false), m_endrun(false)
{
    m_res = SYS->ResCreate( );
    //start thread
    pthread_attr_t pthr_attr;
    pthread_attr_init(&pthr_attr);
    pthread_attr_setschedpolicy(&pthr_attr,SCHED_OTHER);
    pthread_create(&m_pthr,&pthr_attr,TMessArh::Task,this);
    pthread_attr_destroy(&pthr_attr);
    SYS->event_wait( m_stat, true, string(NAME_MODUL)+": Task of message arhiv "+name+" is starting....",5);
}

TMessArh::~TMessArh( )
{
    if( m_stat )
    {
	m_endrun = true;
	//pthread_kill( m_pthr,SIGALRM );
	SYS->event_wait( m_stat, false, string(NAME_MODUL)+": Thread is stoping....");
	pthread_join( m_pthr, NULL );
    }
    SYS->ResDelete( m_res );
}

void *TMessArh::Task(void *param)
{
    int i_cnt = 0;
    TMessArh *arh = (TMessArh *)param;

    arh->m_stat   = true;
    arh->m_endrun = false;

#if OSC_DEBUG
    arh->Owner().m_put("DEBUG",MESS_DEBUG,"%s:Thread <%d>!",arh->Name().c_str(),getpid() );
#endif	
    
    arh->ScanDir();    
    
    while( !arh->m_endrun )
    {    
	usleep(STD_WAIT_DELAY);
	if( ++i_cnt > 5*1000/STD_WAIT_DELAY )
	{
	    i_cnt = 0;
	    try{ arh->ScanDir(); }
	    catch(TError err) { arh->Owner().m_put("SYS",MESS_WARNING,"%s:%s",arh->Name().c_str(),err.what().c_str() ); } 
	}
    }
    arh->m_stat = false;

    return(NULL);
}

void TMessArh::put( vector<SBufRec> &mess )
{
    SYS->RResRequest(m_res);    
    
    for( unsigned i_m = 0; i_m < mess.size(); i_m++)
    {
	int p_cnt = 0;
	for( unsigned i_arh = 0; i_arh < arh_s.size(); i_arh++) 
	    if( !arh_s[i_arh]->Err() && mess[i_m].time >= arh_s[i_arh]->Begin() && mess[i_m].time < arh_s[i_arh]->End() ) 
	    {	    
	      	arh_s[i_arh]->put(mess[i_m]);
		p_cnt++;
	    }	    
	
	if( !p_cnt && mess[i_m].time > time(NULL)-60 )
	{
	    SYS->RResRelease(m_res);
	    
	    SYS->WResRequest(m_res);
	    //Create new arhive
	    time_t tm = time(NULL);
	    char *c_tm = ctime( &tm );
	    for( int i_ch = 0; i_ch < strlen(c_tm); i_ch++ )
    		if( c_tm[i_ch] == '\n' ) c_tm[i_ch] = '\0';
	    string AName = m_addr+'/'+c_tm+".msg";
	    try
	    {		
		arh_s.push_back( new TFileArh(  SYS->FixFName(AName), 
						mess[i_m].time, 
						mess[i_m].time+((TMArhive &)Owner()).m_mess_time_size*24*60*60, 
						this ) );
    	    }
	    catch(TError err) 
	    { 
		Owner().m_put("SYS",MESS_CRIT,"%s:Error create new arhive file <%s>!",Name().c_str(),AName.c_str() ); 
    		SYS->WResRelease(m_res);
		return;
	    }
	    SYS->WResRelease(m_res);
	    
	    SYS->RResRequest(m_res);
	    arh_s[arh_s.size()-1]->put(mess[i_m]);	    
	}
    }
    
    SYS->RResRelease(m_res);	
}

void TMessArh::get( time_t b_tm, time_t e_tm, vector<SBufRec> &mess, string category, char level )
{
    SYS->RResRequest(m_res);    
    int p_cnt = 0;
    for( unsigned i_arh = 0; i_arh < arh_s.size(); i_arh++) 
	if( !arh_s[i_arh]->Err() && 
    		( (arh_s[i_arh]->Begin() >= b_tm && arh_s[i_arh]->Begin() < e_tm) ||
   		  (arh_s[i_arh]->End() >= b_tm && arh_s[i_arh]->End() < e_tm ) ) )
	    arh_s[i_arh]->get(b_tm, e_tm, mess, category, level);
	
    SYS->RResRelease(m_res);	
}

void TMessArh::ScanDir()
{    
    struct stat file_stat;
    dirent *scan_dirent;
    // Convert to absolutly path
    string Path = SYS->FixFName(m_addr);
    DIR *IdDir = opendir(Path.c_str());
    if(IdDir == NULL) 
    {
    	//if( mkdir(Path.c_str(), S_IWRITE | S_IREAD | S_IEXEC ) )
    	if( mkdir(Path.c_str(), SYS->cr_dir_perm() ) )
	    throw TError("%s: Can not open or create dir %s!",NAME_MODUL, Path.c_str() );
    	IdDir = opendir(Path.c_str());
    }
    //---- Free scan flag ----	
    SYS->RResRequest(m_res);
    for( unsigned i_arh = 0; i_arh < arh_s.size(); i_arh++) 
	arh_s[i_arh]->scan = false;
    SYS->RResRelease(m_res);
    
    while((scan_dirent = readdir(IdDir)) != NULL)
    {
	if( string("..") == scan_dirent->d_name || string(".") == scan_dirent->d_name ) continue;
	string NameArh = Path+"/"+scan_dirent->d_name;	
	stat(NameArh.c_str(),&file_stat);        
	if( (file_stat.st_mode&S_IFMT) != S_IFREG || access(NameArh.c_str(),F_OK|R_OK) != 0) continue;
	//===== Check all files ====
	int i_arh;
    	SYS->RResRequest(m_res);
	for( i_arh = 0; i_arh < arh_s.size(); i_arh++) 
	    if( arh_s[i_arh]->Name() == NameArh ) break;
	if( i_arh < arh_s.size() )
	{
	    //========== Arhive already registred=============
	    arh_s[i_arh]->scan = true;
	    arh_s[i_arh]->Sync();
    	    SYS->RResRelease(m_res);
	}
	else
	{	
    	    SYS->RResRelease(m_res);
	    TFileArh *f_arh = new TFileArh(this);
	    f_arh->Attach( NameArh );			    
	    f_arh->scan = true;
	    //Free used memory for old arhives
	    if( time(NULL) < f_arh->Begin() || time(NULL) > f_arh->End() ) f_arh->Sync(true);
	    SYS->WResRequest(m_res);
	    arh_s.push_back( f_arh );		
	    SYS->WResRelease(m_res);
	}
    }
    //==== Check deleting arhives ====
    SYS->WResRequest(m_res);
    for( unsigned i_arh = 0; i_arh < arh_s.size(); i_arh++) 
	if( !arh_s[i_arh]->scan )
	{
	    delete arh_s[i_arh];
	    arh_s.erase( arh_s.begin() + i_arh );
	    i_arh--;
	}	
    SYS->WResRelease(m_res);
    
    closedir(IdDir);    
}

//==============================================================================
//================= BaseArh::TFileArh ==========================================
//==============================================================================
TFileArh::TFileArh( TMessArh *owner ) : m_owner(owner), scan(false), m_err(false), m_write(false), m_load(false) 
{
    m_res = SYS->ResCreate( );
}

TFileArh::TFileArh( string name, time_t beg, time_t end, TMessArh *owner ) ://  string charset, int time_size) :
    m_owner(owner), scan(false), m_err(false), m_write(false), m_load(false)
{
    char buf[20];
    m_res = SYS->ResCreate( );    
    
    //int hd = open( name.c_str(),O_RDWR|O_CREAT|O_TRUNC, S_IWRITE|S_IREAD );
    int hd = open( name.c_str(),O_RDWR|O_CREAT|O_TRUNC, SYS->cr_file_perm() );
    if(hd <= 0) throw TError("%s: Can not create file: %s!"NAME_MODUL,name.c_str());						 

    m_node.new_xml();
    m_node.set_name(NAME_MODUL);
    m_node.set_attr("Version",VERSION,true);
    m_node.set_attr("Charset",((TMArhive &)Owner().Owner()).m_mess_charset,true);
    snprintf(buf,sizeof(buf),"%X",beg);
    m_node.set_attr("Begin",buf,true);
    snprintf(buf,sizeof(buf),"%X",end);
    m_node.set_attr("End",buf,true);
    string x_cf = m_node.get_xml(true);
    write(hd,x_cf.c_str(),x_cf.size());	
    close(hd);    
    m_name  = name;
    m_chars = m_node.get_attr("Charset");
    m_err   = false;
    m_write = false;
    m_load  = true;
    m_beg   = beg;
    m_end   = end;    
}

TFileArh::~TFileArh()
{
    Sync(); 
    
    SYS->ResDelete( m_res );       
}

void TFileArh::Attach( string name )
{
    SYS->WResRequest(m_res);
    
    m_err = false;         // checking file stat
    m_load = false;
    int hd = open( name.c_str(),O_RDONLY );
    if(hd <= 0) m_err = true; 
    else
    {
	int cf_sz = lseek(hd,0,SEEK_END);
    	lseek(hd,0,SEEK_SET);
	char *buf = (char *)malloc(cf_sz+1);
	read(hd,buf,cf_sz);
	buf[cf_sz] = 0;
	string s_buf = buf;
	free(buf);
	try
	{
	    m_node.load_xml(s_buf);
	    if( m_node.get_name() != NAME_MODUL ) 
    	    { 
		Owner().Owner().m_put("SYS",MESS_ERR,"%s:No my arhive file: %s",Owner().Name().c_str(),name.c_str()); 
		m_err = true; 
	    }
	}
	catch( TError err )
	{ 
	    Owner().Owner().m_put("SYS",MESS_ERR,"%s:%s",Owner().Name().c_str(),err.what().c_str()); 
	    m_err = true; 
	}
    }
    m_name = name;
    if( !m_err )
    {
	m_write = false;
	m_chars = m_node.get_attr("Charset");
	m_beg = strtol( m_node.get_attr("Begin").c_str(),(char **)NULL,16);
	m_end = strtol( m_node.get_attr("End").c_str(),(char **)NULL,16); 
	m_load = true;
    	m_acces = time(NULL);
    }
    else m_node.new_xml();
    
    SYS->WResRelease(m_res);
}

void TFileArh::put( SBufRec mess )
{    
    if( m_err ) throw TError("%s: Put message to error arhive file!",NAME_MODUL);
    if( !m_load )
    {
	Attach( m_name ); 
	if( m_err || !m_load )
	    throw TError("%s: Arhive file isn't attaching!",NAME_MODUL);
    }
    SYS->WResRequest(m_res);

    // Want of make checking time into file         //!!!!
    unsigned i_ch;	    
    for( unsigned i_ch = 0; i_ch < m_node.get_child_count(); i_ch++)
	if( strtol( m_node.get_child(i_ch)->get_attr("tm").c_str(),(char **)NULL,16) > mess.time )
	    break;  
	
    char buf[20];    
    XMLNode *cl_node = m_node.ins_child(i_ch,"m");
    snprintf(buf,sizeof(buf),"%X",mess.time);
    cl_node->set_attr("tm",buf,true);
    snprintf(buf,sizeof(buf),"%d",mess.level);
    cl_node->set_attr("lv",buf,true);
    cl_node->set_attr("cat",mess.categ,true);
    string message = mess.mess;
    Mess->SconvOut(m_chars, message);
    cl_node->set_text(message);	    
    m_write = true;
    m_acces = time(NULL);
    
    SYS->WResRelease(m_res);	    
}

void TFileArh::get( time_t b_tm, time_t e_tm, vector<SBufRec> &mess, string category, char level )
{
    if( m_err ) throw TError("%s: Put message to error arhive file!",NAME_MODUL);
    if( !m_load )
    {
	Attach( m_name ); 
	if( m_err || !m_load )
	    throw TError("%s: Arhive file isn't attaching!",NAME_MODUL);
    }
    SYS->RResRequest(m_res);
    for( unsigned i_ch = 0; i_ch < m_node.get_child_count(); i_ch++)
    {
	//find messages
	SBufRec b_rec;
        b_rec.time  = strtol( m_node.get_child(i_ch)->get_attr("tm").c_str(),(char **)NULL,16);
        b_rec.categ = m_node.get_child(i_ch)->get_attr("cat");
        b_rec.level = atoi( m_node.get_child(i_ch)->get_attr("lv").c_str() );
	b_rec.mess  = m_node.get_child(i_ch)->get_text();
    	Mess->SconvIn(m_chars, b_rec.mess);
	if( b_rec.time >= b_tm && b_rec.time < e_tm && (b_rec.categ == category || category == "") && b_rec.level >= level )
	{
	    //Find message dublicates
	    unsigned i_m;
	    for( i_m = 0; i_m < mess.size(); i_m++ )
		if( b_rec.time == mess[i_m].time && b_rec.level == mess[i_m].level && b_rec.mess == mess[i_m].mess ) break;
	    if( i_m < mess.size() ) continue;
	    //Find insert position
	    for( i_m = 0; i_m < mess.size(); i_m++ )
		if( mess[i_m].time > b_rec.time ) break;		
	    mess.insert(mess.begin()+i_m,b_rec);
	}
    }   
    m_acces = time(NULL);
    
    SYS->RResRelease(m_res);
}

void TFileArh::Sync( bool free )
{
    SYS->WResRequest(m_res);
    if( !m_err && m_load )
    {
	if( m_write )
	{
	    int hd = open( m_name.c_str(),O_RDWR|O_TRUNC );
	    if(hd > 0 ) 
	    {
		string x_cf = m_node.get_xml(true);
		write(hd,x_cf.c_str(),x_cf.size());    
		close(hd);
		m_write = false;
	    }
	}
	// Free memory after 10 minets
	if( time(NULL) > m_acces + ((TMArhive &)Owner().Owner()).m_mess_timeout_free*60 || free )
	{
	    m_node.new_xml();
	    m_load = false;
	}
    }    
    SYS->WResRelease(m_res);
}

