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
 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <signal.h>

#include <getopt.h>
#include <string>

#include <tsys.h>
#include <tkernel.h>
#include <tmessage.h>
#include "base.h"

//============ Modul info! =====================================================
#define MOD_ID      "base_arh"
#define MOD_NAME    "Base archivator"
#define MOD_TYPE    "Archive"
#define VER_TYPE    VER_ARH
#define VERSION     "0.1.1"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "The Archive module support base function of message arhiving."
#define LICENSE     "GPL"
//==============================================================================

extern "C"
{ 
    SAtMod module( int n_mod )
    {
	SAtMod AtMod;
	
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
    
    TModule *attach( const SAtMod &AtMod, const string &source )
    {
	BaseArch::TMArchive *self_addr = NULL;
	
	if( AtMod.id == MOD_ID && AtMod.type == MOD_TYPE && AtMod.t_ver == VER_TYPE ) 
	    self_addr = new BaseArch::TMArchive( source );
	    
	return ( self_addr );
    }    
}

using namespace BaseArch;

//==============================================================================
//================= BaseArch::TMArchive ========================================
//==============================================================================
TMArchive::TMArchive( const string &name) : m_mess_max_size(0), m_mess_numb_file(5), m_mess_time_size(7), m_mess_charset("UTF8")
{
    mId 	= MOD_ID;
    mName	= MOD_NAME;
    mType  	= MOD_TYPE;
    Vers      	= VERSION;
    Autors    	= AUTORS;
    DescrMod  	= DESCRIPTION;
    License   	= LICENSE;
    Source    	= name;
}

TMArchive::~TMArchive()
{

}

string TMArchive::opt_descr( )
{
    char buf[STR_BUF_LEN];

    snprintf(buf,sizeof(buf),I18N(
	"======================= The module <%s:%s> options =======================\n"
	"---------- Parameters of the module section <%s> in config file ----------\n"
    	"mess_charset      <name>    charset <name> of the Archive (default UTF8);\n"
    	"mess_max_size     <size>    maximum <size> kb of message Archive file (0 - unlimited default);\n"
    	"mess_numb_file    <number>  number of message Archive files (5 - default);\n"
    	"mess_time_size    <days>    number days to one message file (7 days - default);\n"
    	"mess_timeout_free <min>     timeout of free message file buffer. Timeout no access (10 min default);\n\n"),
	MOD_TYPE,MOD_ID,MOD_ID);

    return(buf);
}

void TMArchive::modCheckCommandLine( ) 
{
    TModule::modCheckCommandLine();
    
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
	    case 'h': fprintf(stdout,opt_descr().c_str()); break;
	    case -1 : break;
	}
    } while(next_opt != -1);
}

void TMArchive::modUpdateOpt()
{
    TModule::modUpdateOpt();
    
    int val;
    try
    { 
	val = atoi( modXMLCfgNode()->get_child("id","mess_max_size")->get_text().c_str() ); 
    	if(val >= 0) m_mess_max_size = val;
    }
    catch(...){ }
    try
    { 
	val = atoi( modXMLCfgNode()->get_child("id","mess_numb_file")->get_text().c_str() ); 
    	if(val > 0) m_mess_numb_file = val;
    }
    catch(...){ }
    try
    {
	val = atoi( modXMLCfgNode()->get_child("id","mess_time_size")->get_text().c_str() ); 
    	if(val > 0) m_mess_time_size = val;
    }
    catch(...){ }
    try{ m_mess_charset = modXMLCfgNode()->get_child("id","mess_charset")->get_text(); }
    catch(...){ }
    try
    {
	val = atoi( modXMLCfgNode()->get_child("id","mess_timeout_free")->get_text().c_str() ); 
    	if(val > 0) m_mess_timeout_free = val;
    }
    catch(...){ }
}

void TMArchive::mod_connect(  )
{
    TModule::mod_connect(  );
}

TArchiveMess *TMArchive::AMess(const string &name)
{
    return( new	TMessArch(name,this) );
}

//================== Controll functions ========================
void TMArchive::ctr_fill_info( XMLNode *inf )
{
    char *dscr = "dscr";
    TTipArchive::ctr_fill_info( inf );
    
    char *i_cntr = 
    	"<area id='bs'>"
       	" <area id='opt' acs='0440'>"
	"  <fld id='a_ch' acs='0660' tp='str'/>"
	"  <fld id='a_sz' acs='0660' tp='dec'/>"
	"  <fld id='a_fl' acs='0660' tp='dec'/>"
	"  <fld id='a_len' acs='0660' tp='dec'/>"
	"  <fld id='a_tm' acs='0660' tp='dec'/>"
	" </area>"
	"</area>";
    
    XMLNode *n_add = inf->ins_child(1);
    n_add->load_xml(i_cntr);
    n_add->set_attr(dscr,I18N(MOD_NAME));
    n_add = n_add->get_child(0);
    n_add->set_attr(dscr,I18N("The message Archive options"));
    n_add->get_child(0)->set_attr(dscr,I18N("Internal Archive charset"));
    n_add->get_child(1)->set_attr(dscr,I18N("Maximum Archive size(byte)"));
    n_add->get_child(2)->set_attr(dscr,I18N("Maximum Archive files"));
    n_add->get_child(3)->set_attr(dscr,I18N("Maximum Archive length time(days)"));
    n_add->get_child(4)->set_attr(dscr,I18N("Timeout freeing buffer of no used Archives(min)"));
    
    //Insert to Help
    char *i_help = "<fld id='g_help' acs='0440' tp='str' cols='90' rows='5'/>";
    
    n_add = inf->get_child("id","help")->add_child();    
    n_add->load_xml(i_help);     
    n_add->set_attr(dscr,Mess->I18N("Options help"));
}

void TMArchive::ctr_din_get_( const string &a_path, XMLNode *opt )
{
    if( a_path == "/bs/opt/a_ch" )		ctr_opt_setS( opt, m_mess_charset );
    else if( a_path == "/bs/opt/a_sz" ) 	ctr_opt_setI( opt, m_mess_max_size );
    else if( a_path == "/bs/opt/a_fl" ) 	ctr_opt_setI( opt, m_mess_numb_file );
    else if( a_path == "/bs/opt/a_len" )	ctr_opt_setI( opt, m_mess_time_size );
    else if( a_path == "/bs/opt/a_tm" ) 	ctr_opt_setI( opt, m_mess_timeout_free );
    else if( a_path == "/help/g_help" ) 	ctr_opt_setS( opt, opt_descr() );       
    else TTipArchive::ctr_din_get_( a_path, opt );
}

void TMArchive::ctr_din_set_( const string &a_path, XMLNode *opt )
{
    if( a_path == "/bs/opt/a_ch" )	m_mess_charset = ctr_opt_getS( opt );
    else if( a_path == "/bs/opt/a_sz" ) m_mess_max_size = ctr_opt_getI( opt );
    else if( a_path == "/bs/opt/a_fl" ) m_mess_numb_file = ctr_opt_getI( opt );
    else if( a_path == "/bs/opt/a_len" )m_mess_time_size = ctr_opt_getI( opt );
    else if( a_path == "/bs/opt/a_tm" )	m_mess_timeout_free = ctr_opt_getI( opt );
    else TTipArchive::ctr_din_set_( a_path, opt );
}

//==============================================================================
//================= BaseArch::TMessArch ========================================
//==============================================================================
TMessArch::TMessArch( const string &name, TTipArchive *owner ) : 
    TArchiveMess( name, owner ), m_endrun(false)
{
    m_res = ResAlloc::ResCreate( );
}

TMessArch::~TMessArch( )
{
    try{ stop(); }catch(...){}
    
    ResAlloc::ResDelete( m_res );
}

void TMessArch::start()
{
    if(run_st) throw TError("(%s) Archive %s already started!",MOD_ID,name().c_str());
    //start thread
    pthread_attr_t pthr_attr;
    pthread_attr_init(&pthr_attr);
    pthread_attr_setschedpolicy(&pthr_attr,SCHED_OTHER);
    pthread_create(&m_pthr,&pthr_attr,TMessArch::Task,this);
    pthread_attr_destroy(&pthr_attr);
    if( SYS->event_wait( run_st, true, string(MOD_ID)+": Task of message arhiv "+name()+" is starting....",5) )
	throw TError("(%s) Archive <%s> no started!",MOD_ID, name().c_str());
}

void TMessArch::stop()
{
    if(!run_st) throw TError("(%s) Archive %s already stoped!",MOD_ID,name().c_str());
    m_endrun = true;
    SYS->event_wait( run_st, false, string(MOD_ID)+": Thread is stoping....");
    pthread_join( m_pthr, NULL );
}
	    
void *TMessArch::Task(void *param)
{
    int i_cnt = 0;
    TMessArch *arh = (TMessArch *)param;

    arh->m_endrun = false;

#if OSC_DEBUG
    arh->Owner().m_put("DEBUG",MESS_DEBUG,"%s:Thread <%d>!",arh->name().c_str(),getpid() );
#endif	
    
    try
    {
	arh->ScanDir();    
    
	arh->run_st   = true;
	while( !arh->m_endrun )
	{    
	    usleep(STD_WAIT_DELAY);
	    if( ++i_cnt > 5*1000/STD_WAIT_DELAY )
	    {
		i_cnt = 0;
		try{ arh->ScanDir(); }
		catch(TError err) { arh->Owner().m_put("SYS",MESS_WARNING,"%s:%s",arh->name().c_str(),err.what().c_str() ); } 
	    }
	}
	arh->run_st = false;
    }
    catch(TError err)
    { 
	arh->run_st   = false;
	arh->Owner().m_put("SYS",MESS_CRIT,"%s:%s",arh->name().c_str(),err.what().c_str()); 
    }

    return(NULL);
}

void TMessArch::put( vector<SBufRec> &mess )
{
    ResAlloc res(m_res,false);
    
    if(!run_st) throw TError("(%s) No started!",name().c_str());
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
	    res.release();
	    
	    res.request(m_res,true);
	    //Create new Archive
	    time_t tm = time(NULL);
	    char *c_tm = ctime( &tm );
	    for( int i_ch = 0; i_ch < strlen(c_tm); i_ch++ )
    		if( c_tm[i_ch] == '\n' ) c_tm[i_ch] = '\0';
	    string AName = m_addr+'/'+c_tm+".msg";
	    try
	    {		
		arh_s.push_back( new TFileArch(  SYS->FixFName(AName), 
						mess[i_m].time, 
						mess[i_m].time+((TMArchive &)Owner()).m_mess_time_size*24*60*60, 
						this ) );
    	    }
	    catch(TError err) 
	    { 
		Owner().m_put("SYS",MESS_CRIT,"%s:Error create new Archive file <%s>!",name().c_str(),AName.c_str() ); 
		return;
	    }
	    res.release();
	    
	    res.request(m_res,false);
	    arh_s[arh_s.size()-1]->put(mess[i_m]);	    
	}
    }
}

void TMessArch::get( time_t b_tm, time_t e_tm, vector<SBufRec> &mess, const string &category, char level )
{
    if( e_tm < b_tm ) return;
    if(!run_st) throw TError("(%s) No started!",name().c_str());
    
    printf("TEST 00\n");
    
    ResAlloc res(m_res,false);
    int p_cnt = 0;
    for( unsigned i_arh = 0; i_arh < arh_s.size(); i_arh++) 
	if( !arh_s[i_arh]->Err() && 
    		!( (b_tm < arh_s[i_arh]->Begin() && e_tm < arh_s[i_arh]->Begin() ) ||
   		  (b_tm > arh_s[i_arh]->End() && e_tm > arh_s[i_arh]->End()  ) ) )
	    arh_s[i_arh]->get(b_tm, e_tm, mess, category, level);
}

void TMessArch::ScanDir()
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
	    throw TError("Can not open or create dir %s!", Path.c_str() );
    	IdDir = opendir(Path.c_str());
    }
    //---- Free scan flag ----	
    ResAlloc res(m_res,false);
    for( unsigned i_arh = 0; i_arh < arh_s.size(); i_arh++) 
	arh_s[i_arh]->scan = false;
    res.release();
    
    while((scan_dirent = readdir(IdDir)) != NULL)
    {
	if( string("..") == scan_dirent->d_name || string(".") == scan_dirent->d_name ) continue;
	string NameArh = Path+"/"+scan_dirent->d_name;	
	stat(NameArh.c_str(),&file_stat);        
	if( (file_stat.st_mode&S_IFMT) != S_IFREG || access(NameArh.c_str(),F_OK|R_OK) != 0) continue;
	//===== Check all files ====
	int i_arh;
	res.request(m_res,false);
	for( i_arh = 0; i_arh < arh_s.size(); i_arh++) 
	    if( arh_s[i_arh]->Name() == NameArh ) break;
	if( i_arh < arh_s.size() )
	{
	    //========== Archive already registred=============
	    arh_s[i_arh]->scan = true;
	    arh_s[i_arh]->Sync();
	    res.release();
	}
	else
	{	
	    res.release();
	    TFileArch *f_arh = new TFileArch(this);
	    f_arh->Attach( NameArh );			    
	    f_arh->scan = true;
	    //Free used memory for old Archives
	    if( time(NULL) < f_arh->Begin() || time(NULL) > f_arh->End() ) f_arh->Sync(true);
	    res.request(m_res,true);
	    arh_s.push_back( f_arh );		
	    res.release();
	}
    }
    //==== Check deleting Archives ====
    res.request(m_res,true);
    for( unsigned i_arh = 0; i_arh < arh_s.size(); i_arh++) 
	if( !arh_s[i_arh]->scan )
	{
	    delete arh_s[i_arh];
	    arh_s.erase( arh_s.begin() + i_arh );
	    i_arh--;
	}	
    res.release();
    
    closedir(IdDir);    
}

//==============================================================================
//================= BaseArch::TFileArch ========================================
//==============================================================================
TFileArch::TFileArch( TMessArch *owner ) : m_owner(owner), scan(false), m_err(false), m_write(false), m_load(false) 
{
    m_res = ResAlloc::ResCreate( );
}

TFileArch::TFileArch( const string &name, time_t beg, time_t end, TMessArch *owner ) ://  string charset, int time_size) :
    m_owner(owner), scan(false), m_err(false), m_write(false), m_load(false)
{
    char buf[20];
    m_res = ResAlloc::ResCreate( );    
    
    //int hd = open( name.c_str(),O_RDWR|O_CREAT|O_TRUNC, S_IWRITE|S_IREAD );
    int hd = open( name.c_str(),O_RDWR|O_CREAT|O_TRUNC, SYS->cr_file_perm() );
    if(hd <= 0) throw TError("%s: Can not create file: %s!"MOD_ID,name.c_str());						 

    m_node.new_xml();
    m_node.set_name(MOD_ID);
    m_node.set_attr("Version",VERSION,true);
    m_node.set_attr("Charset",((TMArchive &)Owner().Owner()).m_mess_charset,true);
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

TFileArch::~TFileArch()
{
    Sync(); 
    
    ResAlloc::ResDelete( m_res );       
}

void TFileArch::Attach( const string &name )
{
    ResAlloc res(m_res,true);
    
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
	    if( m_node.get_name() != MOD_ID ) 
    	    { 
		Owner().Owner().m_put("SYS",MESS_ERR,"%s:No my Archive file: %s",Owner().name().c_str(),name.c_str()); 
		m_err = true; 
	    }
	}
	catch( TError err )
	{ 
	    Owner().Owner().m_put("SYS",MESS_ERR,"%s:%s",Owner().name().c_str(),err.what().c_str()); 
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
}

void TFileArch::put( SBufRec mess )
{    
    if( m_err ) throw TError("%s: Put message to error Archive file!",MOD_ID);
    if( !m_load )
    {
	Attach( m_name ); 
	if( m_err || !m_load )
	    throw TError("%s: Archive file isn't attaching!",MOD_ID);
    }
    
    ResAlloc res(m_res,true);

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
    cl_node->set_text(Mess->SconvOut(m_chars, mess.mess));	    
    m_write = true;
    m_acces = time(NULL);
}

void TFileArch::get( time_t b_tm, time_t e_tm, vector<SBufRec> &mess, const string &category, char level )
{
    if( m_err ) throw TError("%s: Put message to error Archive file!",MOD_ID);
    if( !m_load )
    {
	Attach( m_name ); 
	if( m_err || !m_load )
	    throw TError("%s: Archive file isn't attaching!",MOD_ID);
    }
    
    ResAlloc res(m_res,false);
    for( unsigned i_ch = 0; i_ch < m_node.get_child_count(); i_ch++)
    {
	//find messages
	SBufRec b_rec;
        b_rec.time  = strtol( m_node.get_child(i_ch)->get_attr("tm").c_str(),(char **)NULL,16);
        b_rec.categ = m_node.get_child(i_ch)->get_attr("cat");
        b_rec.level = atoi( m_node.get_child(i_ch)->get_attr("lv").c_str() );
	b_rec.mess  = Mess->SconvIn(m_chars, m_node.get_child(i_ch)->get_text() );
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
}

void TFileArch::Sync( bool free )
{
    ResAlloc res(m_res,true);
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
	if( time(NULL) > m_acces + ((TMArchive &)Owner().Owner()).m_mess_timeout_free*60 || free )
	{
	    m_node.new_xml();
	    m_load = false;
	}
    }    
}

