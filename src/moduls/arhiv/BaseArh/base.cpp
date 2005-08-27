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
#include <resalloc.h>
#include <tmess.h>
#include "base.h"

//============ Modul info! =====================================================
#define MOD_ID      "BaseArh"
#define MOD_NAME    "Base archivator"
#define MOD_TYPE    "Archive"
#define VER_TYPE    VER_ARH
#define VERSION     "0.3.0"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "The Archive module. Allow base functions of message and value arhiving."
#define LICENSE     "GPL"
//==============================================================================

BaseArch::TMArchive *BaseArch::mod;

extern "C"
{ 
    TModule::SAt module( int n_mod )
    {
	TModule::SAt AtMod;
	
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
    
    TModule *attach( const TModule::SAt &AtMod, const string &source )
    {
	BaseArch::TMArchive *self_addr = NULL;
	
	if( AtMod.id == MOD_ID && AtMod.type == MOD_TYPE && AtMod.t_ver == VER_TYPE ) 
	    self_addr = BaseArch::mod = new BaseArch::TMArchive( source );
	    
	return ( self_addr );
    }    
}

using namespace BaseArch;

//==============================================================================
//================= BaseArch::TMArchive ========================================
//==============================================================================
TMArchive::TMArchive( const string &name)
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

string TMArchive::optDescr( )
{
    char buf[STR_BUF_LEN];

    snprintf(buf,sizeof(buf),I18N(
	"======================= The module <%s:%s> options =======================\n"
	"---------- Parameters of the module section <%s> in config file ----------\n\n"),
	MOD_TYPE,MOD_ID,nodePath().c_str());

    return(buf);
}

void TMArchive::modLoad()
{
    //========== Load parameters from command line ============
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
	    case 'h': fprintf(stdout,optDescr().c_str()); break;
	    case -1 : break;
	}
    } while(next_opt != -1);
}

void TMArchive::modConnect(  )
{
    TModule::modConnect(  );
    
    //Add self DB-fields
    if( !((TArchiveS &)owner()).messE().fldAvoid("PRM_1") )
	((TArchiveS &)owner()).messE().fldAdd( new TFld("PRM_1",Mess->I18N("Parameter 1"),TFld::String,0,"20","300") );
    if( !((TArchiveS &)owner()).messE().fldAvoid("PRM_2") )
	((TArchiveS &)owner()).messE().fldAdd( new TFld("PRM_2",Mess->I18N("Parameter 2"),TFld::String,0,"20","10") );
    if( !((TArchiveS &)owner()).messE().fldAvoid("PRM_3") )
	((TArchiveS &)owner()).messE().fldAdd( new TFld("PRM_3",Mess->I18N("Parameter 3"),TFld::String,0,"20","30") );
    if( !((TArchiveS &)owner()).messE().fldAvoid("PRM_4") )
	((TArchiveS &)owner()).messE().fldAdd( new TFld("PRM_4",Mess->I18N("Parameter 4"),TFld::String,0,"20","5") );
}

TArchiveMess *TMArchive::AMess(const string &name)
{
    return( new	TMessArch(name,this) );
}

//================== Controll functions ========================
void TMArchive::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    if( cmd==TCntrNode::Info )
    {
	TTipArchive::cntrCmd_( a_path, opt, cmd );       //Call parent

	ctrMkNode("fld",opt,a_path.c_str(),"/help/g_help",Mess->I18N("Options help"),0440,0,0,"str")->
	    attr_("cols","90")->attr_("rows","5");
    }
    else if( cmd==TCntrNode::Get )
    {
	if( a_path == "/help/g_help" )	ctrSetS( opt, optDescr() );       
	else TTipArchive::cntrCmd_( a_path, opt, cmd );
    }
    else if( cmd==TCntrNode::Set )
	TTipArchive::cntrCmd_( a_path, opt, cmd );
}

//==============================================================================
//================= BaseArch::TMessArch ========================================
//==============================================================================
TMessArch::TMessArch( const string &name, TTipArchive *owner ) : 
    TArchiveMess( name, owner ), m_endrun(false)
{
    m_res = ResAlloc::resCreate( );
}

TMessArch::~TMessArch( )
{
    try{ stop(); }catch(...){}
    
    ResAlloc::resDelete( m_res );
}

void TMessArch::start()
{
    m_max_size = atoi(cfg("PRM_1").getS().c_str());
    m_numb_files = atoi(cfg("PRM_2").getS().c_str());
    m_time_size = atoi(cfg("PRM_3").getSd().c_str());
    m_timeout_free = atoi(cfg("PRM_4").getSd().c_str());

    if(run_st)	return;
    //start thread
    pthread_attr_t pthr_attr;
    pthread_attr_init(&pthr_attr);
    pthread_attr_setschedpolicy(&pthr_attr,SCHED_OTHER);
    pthread_create(&m_pthr,&pthr_attr,TMessArch::Task,this);
    pthread_attr_destroy(&pthr_attr);
    if( TSYS::eventWait( run_st, true, nodePath()+"start",5) )
	throw TError(nodePath().c_str(),"Archive no started!");
}

void TMessArch::stop()
{
    if(!run_st) return;
    m_endrun = true;
    TSYS::eventWait( run_st, false, nodePath()+"stop");
    pthread_join( m_pthr, NULL );
}
	    
void *TMessArch::Task(void *param)
{
    int i_cnt = 0;
    TMessArch *arh = (TMessArch *)param;

    arh->m_endrun = false;

#if OSC_DEBUG
    Mess->put(arh->nodePath().c_str(),TMess::Debug,Mess->I18N("Thread <%d> started!"),getpid() );
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
		catch(TError err) { Mess->put(err.cat.c_str(),TMess::Warning,err.mess.c_str() ); } 
	    }
	}
	arh->run_st = false;
    }
    catch(TError err)
    { 
	arh->run_st   = false;
	Mess->put(err.cat.c_str(),TMess::Crit,err.mess.c_str()); 
    }

    return(NULL);
}

void TMessArch::put( vector<TMess::SRec> &mess )
{
    ResAlloc res(m_res,false);
    
    if(!run_st) throw TError(nodePath().c_str(),"Archive no started!");
    for( unsigned i_m = 0; i_m < mess.size(); i_m++)
    {
	int i_arh;
	for( i_arh = 0; i_arh < arh_s.size(); i_arh++) 
	    if( !arh_s[i_arh]->err() && mess[i_m].time >= arh_s[i_arh]->begin() ) 
	    {	
		if( i_arh == 0 && 
		    ((m_max_size && arh_s[i_arh]->size() > m_max_size*1024) ||
		     (mess[i_m].time >= arh_s[i_arh]->begin()+m_time_size*24*60*60)) ) break;
	      	arh_s[i_arh]->put(mess[i_m]);
		i_arh = -1;
		break;
	    }
	if( i_arh == 0 )
	//if( mess[i_m].time > time(NULL)-60 )
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
		printf("TEST 00\n");
		arh_s.push_front( new TFileArch( AName, mess[i_m].time, this ) );
		//---- Check file count and delete odd files ----
		while( true )
		{
		    int f_cnt = 0;
		    for( int i_arh = 0; i_arh < arh_s.size(); i_arh++) 
			if( !arh_s[i_arh]->err() )	f_cnt++;		    
		    if( !m_numb_files || f_cnt <= m_numb_files ) break;
		    //Find oldest file
		    int old_id = -1;
		    time_t old_tm = time(NULL);
		    for( int i_arh = 0; i_arh < arh_s.size(); i_arh++)
			if( !arh_s[i_arh]->err() && arh_s[i_arh]->begin() < old_tm )
			{	
			    old_tm = arh_s[i_arh]->begin();
			    old_id = i_arh;
			}
		    if( old_id >= 0 )
		    {
			string f_nm = arh_s[old_id]->name();
			delete arh_s[old_id];
			arh_s.erase( arh_s.begin() + old_id );
			printf("TEST 01\n");
			remove(f_nm.c_str());
		    }			    
		} 
    	    }
	    catch(TError err) 
	    { 
		Mess->put(nodePath().c_str(),TMess::Crit,"Error create new Archive file <%s>!",AName.c_str() ); 
		return;
	    }
	    res.release();
	    
	    res.request(m_res,false);
	    arh_s[arh_s.size()-1]->put(mess[i_m]);	    
	}
    }
}

void TMessArch::get( time_t b_tm, time_t e_tm, vector<TMess::SRec> &mess, const string &category, char level )
{
    if( e_tm <= b_tm ) return;
    if(!run_st) throw TError(nodePath().c_str(),"Archive no started!");
    
    ResAlloc res(m_res,false);
    int p_cnt = 0;
    for( unsigned i_arh = 0; i_arh < arh_s.size(); i_arh++) 
	if( !arh_s[i_arh]->err() &&
		!( (b_tm < arh_s[i_arh]->begin() && e_tm < arh_s[i_arh]->begin() ) ||
   		   (b_tm > arh_s[i_arh]->end() && e_tm > arh_s[i_arh]->end() ) ) )
	    arh_s[i_arh]->get(b_tm, e_tm, mess, category, level);
}

void TMessArch::ScanDir()
{    
    struct stat file_stat;
    dirent *scan_dirent;
    
    string Path = m_addr;
    DIR *IdDir = opendir(Path.c_str());
    if(IdDir == NULL) 
    {
    	if( mkdir(Path.c_str(),0777) )
	    throw TError(nodePath().c_str(),"Can not create dir <%s>.",Path.c_str());
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
	    if( arh_s[i_arh]->name() == NameArh ) break;
	if( i_arh < arh_s.size() )
	{
	    //========== Archive already registred=============
	    arh_s[i_arh]->scan = true;
	    arh_s[i_arh]->sync();
	    res.release();
	}
	else
	{	
	    res.release();
	    
	    TFileArch *f_arh = new TFileArch(this);
	    f_arh->Attach( NameArh, false );
	    f_arh->scan = true;
	    
	    res.request(m_res,true);
	    //Oldest and broken arhives to down
	    if( f_arh->err() )	arh_s.push_back( f_arh );
	    else
	    {
		int i_arh, i_brk = -1;
		for( i_arh = 0; i_arh < arh_s.size(); i_arh++)
		{
		    if( arh_s[i_arh]->err() )	{ i_brk = i_arh; break; }
		    if( f_arh->begin() >= arh_s[i_arh]->begin() ) 
		    {			
			arh_s.insert(arh_s.begin()+i_arh,f_arh);
			break;	    
		    }
		}		
		if( i_brk >= 0 ) arh_s.insert(arh_s.begin()+i_brk,f_arh);		
		if( i_arh >= arh_s.size() ) arh_s.push_back( f_arh );
	    }	    
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

//================== Controll functions ========================
void TMessArch::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    if( cmd==TCntrNode::Info )
    {
	TArchiveMess::cntrCmd_( a_path, opt, cmd );       //Call parent

	ctrInsNode("area",1,opt,a_path.c_str(),"/bs",mod->I18N(MOD_NAME));
	ctrMkNode("fld",opt,a_path.c_str(),"/bs/a_sz",mod->I18N("Maximum archive file size (kB)"),0660,0,0,"dec");
	ctrMkNode("fld",opt,a_path.c_str(),"/bs/a_fl",mod->I18N("Archive files number"),0660,0,0,"dec");
	ctrMkNode("fld",opt,a_path.c_str(),"/bs/a_len",mod->I18N("Maximum archive length time(days)"),0660,0,0,"dec");
	ctrMkNode("fld",opt,a_path.c_str(),"/bs/a_tm",mod->I18N("Timeout of freeing buffer (min)"),0660,0,0,"dec");
    }
    else if( cmd==TCntrNode::Get )
    {
	if( a_path == "/bs/a_sz" ) 	ctrSetI( opt, atoi(cfg("PRM_1").getS().c_str()) );
	else if( a_path == "/bs/a_fl" )	ctrSetI( opt, atoi(cfg("PRM_2").getS().c_str()) );
	else if( a_path == "/bs/a_len" )ctrSetI( opt, atoi(cfg("PRM_3").getS().c_str()) );
	else if( a_path == "/bs/a_tm" )	ctrSetI( opt, atoi(cfg("PRM_4").getS().c_str()) );
	else TArchiveMess::cntrCmd_( a_path, opt, cmd );
    }
    else if( cmd==TCntrNode::Set )
    {
	if( a_path == "/bs/a_sz" ) 	
	{ m_max_size = ctrGetI( opt ); 	cfg("PRM_1").setS(TSYS::int2str(m_max_size) ); }
	else if( a_path == "/bs/a_fl" ) 
	{ m_numb_files = ctrGetI( opt ); cfg("PRM_2").setS(TSYS::int2str(m_numb_files) ); }
	else if( a_path == "/bs/a_len" )
	{ m_time_size = ctrGetI( opt );	cfg("PRM_3").setS(TSYS::int2str(m_time_size) ); }
	else if( a_path == "/bs/a_tm" )	
	{ m_timeout_free = ctrGetI( opt ); cfg("PRM_4").setS(TSYS::int2str(m_timeout_free) ); }
	else TArchiveMess::cntrCmd_( a_path, opt, cmd );
    }
}

//==============================================================================
//================= BaseArch::TFileArch ========================================
//==============================================================================
TFileArch::TFileArch( TMessArch *owner ) : m_owner(owner), scan(false), m_err(false), m_write(false), m_load(false) 
{
    m_res = ResAlloc::resCreate( );
}

TFileArch::TFileArch( const string &name, time_t beg, TMessArch *n_owner ) ://  string charset, int time_size) :
    m_owner(n_owner), scan(false), m_err(false), m_write(false), m_load(false), m_size(0), 
    m_name(name), m_chars("UTF-8"), m_beg(beg), m_end(beg)
{
    m_res = ResAlloc::resCreate( );    
    
    int hd = open( name.c_str(),O_RDWR|O_CREAT|O_TRUNC, 0666 );
    if(hd <= 0) throw TError(owner().nodePath().c_str(),"Can not create file: <%s>!",name.c_str());						 

    m_node.clear();
    m_node.name(MOD_ID);
    m_node.attr("Version",VERSION);
    //m_node.attr("Charset",Mess->charset( ));
    m_node.attr("Begin",TSYS::int2str(m_beg,C_INT_HEX));
    m_node.attr("End",TSYS::int2str(m_end,C_INT_HEX));
    string x_cf = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" + m_node.save(XML_BR_OPEN_PREV);
    write(hd,x_cf.c_str(),x_cf.size());	
    close(hd);    
    m_load  = true;
}

TFileArch::~TFileArch()
{
    sync(); 
    
    ResAlloc::resDelete( m_res );       
}

void TFileArch::Attach( const string &name, bool full )
{
    ResAlloc res(m_res,true);
    
    m_err = false;         // checking file stat
    m_load = false;
    m_name = name;
    
    FILE *f = fopen(name.c_str(),"r");
    if( f == NULL ) { m_err = true; return; }
    
    try
    {
        if( full )
        {
	    int r_cnt;
	    string s_buf;
	    char buf[STR_BUF_LEN];
	    
    	    fseek(f,0,SEEK_SET);	    
	    while( r_cnt = fread(buf,1,sizeof(buf),f) )
		s_buf.append(buf,r_cnt);
	    fclose(f);
	
    	    m_node.load(s_buf);
	    if( m_node.name() != MOD_ID ) 
    	    { 
	        Mess->put(owner().nodePath().c_str(),TMess::Error,"No my Archive file: <%s>",name.c_str()); 
	        m_node.clear();
	        m_err = true; 
	        return;
	    }
	    m_size = s_buf.size();
	    m_write = false;
	    m_chars = "UTF-8"; //m_node.attr("Charset");
	    m_beg = strtol( m_node.attr("Begin").c_str(),(char **)NULL,16);
	    m_end = strtol( m_node.attr("End").c_str(),(char **)NULL,16);
	    m_load = true;
	    m_acces = time(NULL);	    
	    return;
	}
	else
	{	
	    //Get archive header
	    char c;
	    string prm, val;	    
	    
	    do
	    {
		while( (c = fgetc(f)) != '<' && c != EOF );
		if( c == EOF )
		{	
		    Mess->put(owner().nodePath().c_str(),TMess::Error,"Archive <%s> file error.",name.c_str());
		    m_err = true;
	    	    fclose(f);
		    return;
		}		
		prm.clear();
		while( (c = fgetc(f)) != ' ' && c != '\t' && c != '>' && c != EOF ) prm+=c;	    
		if( c == EOF )
		{ 
	    	    Mess->put(owner().nodePath().c_str(),TMess::Error,"Archive <%s> file error.",name.c_str());
		    m_err = true;
	    	    fclose(f);
		    return;
		}
	    } while( prm != MOD_ID );
	    //Go to 
	    while( true )
	    {	    
		prm.clear();
		val.clear();
		while( (c = fgetc(f)) == ' ' || c == '\t' );
		if( c == '>' || c == EOF ) break;
		while( c != '=' && c != '>' && c != EOF )	{ prm+=c; c = fgetc(f); }
		while( (c = fgetc(f)) != '"' && c != '>' && c != EOF );		
		while( (c = fgetc(f)) != '"' && c != '>' && c != EOF )  val+=c;
				
		//if( prm == "Charset" )		m_chars = val;
		if( prm == "Begin" )	m_beg = strtol(val.c_str(),NULL,16);
		else if( prm == "End" )	m_end = strtol(val.c_str(),NULL,16);		
	    }
	    fseek(f,0,SEEK_END);
	    m_size = ftell(f);
	    fclose(f);
	    m_write = false;
	    m_load = false;
	    return;	    	
	}
    }
    catch( TError err )
    { 
	Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); 
	m_node.clear();
	m_err = true;
	return;
    }
}

void TFileArch::put( TMess::SRec mess )
{    
    if( m_err ) throw TError(owner().nodePath().c_str(),"Put message to error Archive file!");
    if( !m_load )
    {
	Attach( m_name ); 
	if( m_err || !m_load )
	    throw TError(owner().nodePath().c_str(),"Archive file isn't attaching!");
    }
    
    ResAlloc res(m_res,true);

    // Want of make checking time into file         //!!!!
    unsigned i_ch;	    
    for( unsigned i_ch = 0; i_ch < m_node.childSize(); i_ch++)
	if( strtol( m_node.childGet(i_ch)->attr("tm").c_str(),(char **)NULL,16) > mess.time )
	    break;  
	
    XMLNode *cl_node = m_node.childIns(i_ch,"m");
    cl_node->attr("tm",TSYS::int2str(mess.time,C_INT_HEX));
    cl_node->attr("lv",TSYS::int2str(mess.level));
    cl_node->attr("cat",mess.categ);
    cl_node->text(Mess->SconvOut(m_chars, mess.mess));
    if( mess.time > m_end ) 
    { 
	m_end = mess.time;
	m_node.attr("End",TSYS::int2str(m_end,C_INT_HEX));
    }
    m_write = true;
    m_acces = time(NULL);
}

void TFileArch::get( time_t b_tm, time_t e_tm, vector<TMess::SRec> &mess, const string &category, char level )
{
    if( m_err ) throw TError(owner().nodePath().c_str(),"Get messages from error Archive file!");
    if( !m_load )
    {
	Attach( m_name ); 
	if( m_err || !m_load )
	    throw TError(owner().nodePath().c_str(),"Archive file isn't attaching!");
    }
    
    ResAlloc res(m_res,false);
    for( unsigned i_ch = 0; i_ch < m_node.childSize(); i_ch++)
    {
	//find messages
	TMess::SRec b_rec;
        b_rec.time  = strtol( m_node.childGet(i_ch)->attr("tm").c_str(),(char **)NULL,16);
        b_rec.categ = m_node.childGet(i_ch)->attr("cat");
        b_rec.level = (TMess::Type)atoi( m_node.childGet(i_ch)->attr("lv").c_str() );
	b_rec.mess  = Mess->SconvIn(m_chars, m_node.childGet(i_ch)->text() );
	if( b_rec.time >= b_tm && b_rec.time < e_tm && b_rec.level >= level && TMess::chkPattern(b_rec.categ,category) )
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

void TFileArch::sync( bool free )
{
    ResAlloc res(m_res,true);
    if( !m_err && m_load )
    {
	if( m_write )
	{
	    int hd = open( m_name.c_str(),O_RDWR|O_TRUNC );
	    if(hd > 0 ) 
	    {		
		string x_cf = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" + m_node.save(XML_BR_OPEN_PREV);
		m_size = x_cf.size();
		write(hd,x_cf.c_str(),m_size);
		close(hd);
		m_write = false;
	    }
	}
	// Free memory after 10 minets
	if( time(NULL) > m_acces + owner().timeoutFree()*60 || free )
	{
	    m_node.clear();
	    m_load = false;
	}
    }    
}

