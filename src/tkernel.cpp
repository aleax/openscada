
#include <stdarg.h>
#include <stdlib.h>
#include <langinfo.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <string>
#include <iostream>
#include <new>

#include "../config.h"
#include "terror.h"
#include "tmessage.h"
#include "tbds.h"
#include "ttransports.h"
#include "tprotocols.h"
#include "tarhives.h"
#include "tcontrollers.h"
#include "tspecials.h"
#include "tparams.h"
#include "tuis.h"
#include "tmodschedul.h"
#include "tsequrity.h"
#include "tsys.h"
#include "tkernel.h"

TMessage   *Mess;
TSYS       *SYS;

const char *TKernel::n_opt = "generic";
const char *TKernel::o_name = "TKernel";
const char *TKernel::i_cntr = 
    "<oscada_cntr>"
    " <area id='a_gen' dscr='Generic control'>"
    "  <fld id='mod_path' dscr='Path to shared libs(modules)' com='1' cfg='1' dest='dir' tp='str'/>"
    "  <list id='mod_auto' dscr='List of auto conected shared libs(modules)' tp='str' dest='file'/>"
    "  <fld id='def_tp_bd' dscr='Default type bd(bd module)' cfg='1' tp='str'/>"
    "  <fld id='def_bd' dscr='Default bd' cfg='1' tp='str'/>"
    "  <comm id='run' dscr='Run'/>"
    "  <comm id='upd_opt' dscr='Update options(from config)'/>"
    " </area>"
    " <area id='a_subs' dscr='Subsystems'>"
    "  <list id='subs_br' dscr='Subsystems' tp='br' mode='at' acs='0555'/>"
    " </area>"
    " <area id='a_help' dscr='Help'>"
    "  <fld id='s_help' dscr='Config help' acs='0444' tp='str' cols='90' rows='10'/>"
    " </area>"
    "</oscada_cntr>";

TKernel::TKernel( string name ) 
	: ModPath("./"), DefBDType(""), DefBDName(""), m_name(name), TContr( i_cntr ), s_run(false)
{
    m_put_s("INFO",MESS_INFO,"Create!");
    
    //auto_ptr<TMessage> Mess (new TMessage());
    param    = new TParamS(this);
    bd 	     = new TBDS(this);
    sequrity = new TSequrity(this);
    transport = new TTransportS(this);
    protocol = new TProtocolS(this);
    arhive   = new TArhiveS(this);
    controller  = new TControllerS(this);
    special  = new TSpecialS(this);
    ui       = new TUIS(this);

    modschedul  = new TModSchedul(this);
    ModSchedul().RegGroupM(bd);
    ModSchedul().RegGroupM(transport);
    ModSchedul().RegGroupM(protocol);
    ModSchedul().RegGroupM(controller);    
    ModSchedul().RegGroupM(arhive);
    ModSchedul().RegGroupM(special);    
    ModSchedul().RegGroupM(ui);    
}

TKernel::~TKernel()
{
    m_put_s("INFO",MESS_INFO,"Destroy!");

    //Stop all controllers   //????
    vector<SCntrS> c_list;
    Controller().list(c_list);
    for( unsigned i_ls = 0; i_ls < c_list.size(); i_ls++)
    {
	SHDCntr hd = Controller().att(c_list[i_ls]);
	Controller().at(hd).Stop();
	Controller().det(hd);
    }							    
    //Disable all controllers   //????
    for( unsigned i_ls = 0; i_ls < c_list.size(); i_ls++)
    {
	SHDCntr hd = Controller().att(c_list[i_ls]);
	Controller().at(hd).Disable();
	Controller().det(hd);
    }							    
    
    delete modschedul;
    delete ui;
    delete param;
    delete arhive;
    delete controller;
    delete protocol;
    delete transport;
    delete special;
    delete sequrity;
    delete bd;
}

int TKernel::run()
{
    if(s_run) throw TError("(%s) Kernel already started!",o_name);
    
    m_put("INFO",MESS_INFO,"Start!",m_name.c_str());

    try
    {
	CheckCommandLine();
	UpdateOpt();
	
	ModSchedul().LoadAll();
	CheckCommandLine(true);   //check help, error and exit
	
	ModSchedul().InitAll();	
	ModSchedul().StartAll();	
	ModSchedul().StartSched();	
    } 
    catch(TError error) 
    { 
	m_put("SYS",MESS_CRIT,"Run exception: %s",error.what().c_str()); 
	return(-1); 
    }
    catch(...)
    { return(-2); }

    s_run = true;
    //Start signal listen
    return(0);
}


string TKernel::opt_descr( )
{
    string kz;

    kz = kz +
	"============================ Kernel options ==============================\n"+
    	"    --ModPath=<path>   Set modules <path>: \"/var/os/modules/,./mod/\"\n"+
    	"--------------- Fields <"+n_opt+"> sections of config file -------------------\n"+
    	"mod_path=<path>         set path to shared libs;\n"+
    	"mod_auto=<list>         name automatic loaded,  attached and started shared libs <direct_dbf.so;virt.so>\n"+
    	"DefaultBD = <type:name> set default bd type and bd name (next, may use only table name);\n\n";

    return(kz);
}


void TKernel::CheckCommandLine( bool mode )
{
#if OSC_DEBUG
    m_put_s("DEBUG",MESS_INFO,"Read commandline options!");
#endif
	
    int next_opt;
    char *short_opt="hd:";
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
	if(mode==false)
	{
    	    switch(next_opt)
    	    {
    		case 'h': fprintf(stdout,opt_descr().c_str()); break;
    		case 'm': ModPath = optarg; break;
    		case -1 : break;
    	    }
	}
	else if(next_opt == 'h') throw TError("Comandline help print!");
    } while(next_opt != -1);
/*  
    if(optind < argc) 
    {
	if(mode==false)
	{
	    fprintf(stdout,"Error Option\n");	
	    pr_opt_descr(stdout);
	}
	else exit(0);
    }
*/    

    if( mode == false )
    {
	ModSchedul().CheckCommandLine(); 
	ModSchedul().CheckCommandLineMod(); 
    }
    
#if OSC_DEBUG
    m_put_s("DEBUG",MESS_DEBUG,"Read commandline options ok!");
#endif
}

void TKernel::UpdateOpt()
{
#if OSC_DEBUG
    m_put_s("DEBUG",MESS_INFO,"Read config options!");
#endif

    string opt;
    
    try{ ModPath = XMLCfgNode()->get_child("mod_path")->get_text(); }
    catch(...) {  }

    try
    {
	opt = XMLCfgNode()->get_child("mod_auto")->get_text();
	if( opt.size() )
	{
	    int i_beg = -1;
    	    auto_m_list.clear();
	    do
	    {
		auto_m_list.push_back(opt.substr(i_beg+1,opt.find(";",i_beg+1)-i_beg-1));
		i_beg = opt.find(";",i_beg+1);
	    } while(i_beg != (int)string::npos);
	}
    }
    catch(...) {  }
    
    try
    {
	opt = XMLCfgNode()->get_child("DefaultBD")->get_text();
	if( opt.size() )
    	{
    	    int pos = 0;
    	    DefBDType = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
    	    DefBDName = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
	}
    }
    catch(...) {  }

    ModSchedul().UpdateOpt();
    ModSchedul().UpdateOptMod();    
    
#if OSC_DEBUG
    m_put_s("DEBUG",MESS_DEBUG,"Read config options ok!");
#endif
}

XMLNode *TKernel::XMLCfgNode()
{    
    int i_k = 0;
    while(true)
    {
	XMLNode *t_n = SYS->XMLCfgNode()->get_child("kernel",i_k++); 
	if( t_n->get_attr("id") == m_name) return( t_n );
    }
}

//==============================================================
//================== Controll functions ========================
//==============================================================
void TKernel::ctr_fill_info( XMLNode *inf )
{
    inf->set_text(string("Kernel: ")+Name());
}

void TKernel::ctr_din_get_( string a_path, XMLNode *opt )
{
    string t_id = ctr_path_l(a_path,0);
    if( t_id == "a_gen" )
    {
    	t_id = ctr_path_l(a_path,1);
	if( t_id == "mod_path" )       ctr_opt_setS( opt, ModPath );
	else if( t_id == "def_tp_bd" ) ctr_opt_setS( opt, DefBDType );
	else if( t_id == "def_bd" )    ctr_opt_setS( opt, DefBDName ); 
	else if( t_id == "mod_auto" )
	    for( unsigned i_a=0; i_a < auto_m_list.size(); i_a++ )
		ctr_opt_setS( opt, auto_m_list[i_a], i_a );
    }
    else if( t_id == "a_subs" && ctr_path_l(a_path,1) == "subs_br" )
    {
	ctr_opt_setS( opt, ModSchedul().Name(),0 );
	ctr_opt_setS( opt, Sequrity().Name()  ,1 );
	ctr_opt_setS( opt, Arhive().Name()    ,2 );
	ctr_opt_setS( opt, BD().Name()        ,3 );
	ctr_opt_setS( opt, Controller().Name(),4 );
	ctr_opt_setS( opt, Protocol().Name()  ,5 );
	ctr_opt_setS( opt, Transport().Name() ,6 );
	ctr_opt_setS( opt, Special().Name()   ,7 );
	ctr_opt_setS( opt, Param().Name()     ,8 );
	ctr_opt_setS( opt, UI().Name()        ,9 );
    }
    else if( t_id == "a_help" )
       	ctr_opt_setS( opt, opt_descr() );       
} 

void TKernel::ctr_din_set_( string a_path, XMLNode *opt )
{
    string t_id = ctr_path_l(a_path,0);
    if( t_id == "a_gen" )
    {
    	t_id = ctr_path_l(a_path,1);
	if( t_id == "mod_path" )       ModPath   = ctr_opt_getS( opt );
	else if( t_id == "def_tp_bd" ) DefBDType = ctr_opt_getS( opt );
	else if( t_id == "def_bd" )    DefBDName = ctr_opt_getS( opt ); 
	else if( t_id == "mod_auto" )
	    for( int i_el=0; i_el < opt->get_child_count(); i_el++)
    	    {
    		XMLNode *t_c = opt->get_child(i_el);
    		if( t_c->get_name() == "el")
    		{
    		    if(t_c->get_attr("do") == "add")      
			auto_m_list.push_back(t_c->get_text());
		    else if(t_c->get_attr("do") == "ins") 
			auto_m_list.insert(auto_m_list.begin()+atoi(t_c->get_attr("id").c_str()),t_c->get_text()); 
		    else if(t_c->get_attr("do") == "edit") 
			auto_m_list[atoi(t_c->get_attr("id").c_str())] = t_c->get_text();  
    		    else if(t_c->get_attr("do") == "del") 
			auto_m_list.erase(auto_m_list.begin()+atoi(t_c->get_attr("id").c_str()));
    		}
    	    }
    }
}

void TKernel::ctr_cmd_go( string a_path, XMLNode *fld, XMLNode *rez )
{
    string t_id = ctr_path_l(a_path,0);    
    if( t_id == "a_gen" )
    {
    	t_id = ctr_path_l(a_path,1);    
    	if( t_id == "run" )          run();
	else if( t_id == "upd_opt" ) UpdateOpt();
    }
}  

TContr &TKernel::ctr_at( string br )
{
    string t_id = ctr_path_l(br,2);
    if( t_id == ModSchedul().Name() )    return( ModSchedul() );
    else if( t_id == Sequrity().Name() ) return( Sequrity() );
    else if( t_id == Arhive().Name() )   return( Arhive() );
    else if( t_id == BD().Name() )       return( BD() ) ;
    else if( t_id == Controller().Name() ) return( Controller() );
    else if( t_id == Protocol().Name() ) return( Protocol() );
    else if( t_id == Transport().Name() )  return( Transport() );
    else if( t_id == Special().Name() )  return( Special() );
    else if( t_id == Param().Name() )    return( Param() );
    else if( t_id == UI().Name() )       return( UI() );
    throw TError("(%s) Subsystem <%s> no avoid!",o_name, t_id.c_str());
}

//==============================================================
//================== Message functions ========================
//==============================================================
void TKernel::m_put( string categ, int level, char *fmt,  ... )
{
    char str[STR_BUF_LEN];
    va_list argptr;

    va_start (argptr,fmt);
    vsnprintf(str,sizeof(str),fmt,argptr);
    va_end(argptr);
    m_put_s( categ, level, str );
}

void TKernel::m_put_s( string categ, int level, string mess )
{
    Mess->put_s( categ, level, Name()+":"+mess );
}
 

