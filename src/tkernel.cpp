
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

const char *TKernel::o_name = "TKernel";
const char *TKernel::i_cntr = 
    "<oscada_cntr>"
    " <area id='a_subs'>"
    "  <list id='subs_br' tp='br' mode='at' acs='0555'/>"
    " </area>"
    " <area id='a_gen' acs='0440'>"
    "  <fld id='def_tp_bd' cfg='1' tp='str' dest='select' select='a_gen/b_mod'/>"
    "  <fld id='def_bd' cfg='1' tp='str'/>"
    "  <fld id='g_help' acs='0440' tp='str' cols='90' rows='5'/>"
    "  <comm id='run'/>"
    "  <comm id='upd_opt'/>"
    "  <list id='b_mod' tp='str' hide='1'/>"
    " </area>"
    "</oscada_cntr>";

TKernel::TKernel( string name ) 
	: DefBDType(""), DefBDName(""), m_name(name), s_run(false)
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
	if( Controller().at(hd).st_run() ) 
	    Controller().at(hd).Stop();
	Controller().det(hd);
    }							    
    //Disable all controllers   //????
    for( unsigned i_ls = 0; i_ls < c_list.size(); i_ls++)
    {
	SHDCntr hd = Controller().att(c_list[i_ls]);
	if( Controller().at(hd).st_enable() ) 
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
	
	Sequrity().Init();
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
    char buf[STR_BUF_LEN];
    snprintf(buf,sizeof(buf),Mess->I18N(
	"========================= The kernel options ==============================\n"
    	"------------- The config file kernel <%s> parameters ------------\n"
    	"DefaultBD <type:name>  set default bd type and bd name (next, may use only table name);\n\n"
	),Name().c_str());

    return(buf);
}


void TKernel::CheckCommandLine( bool mode )
{
#if OSC_DEBUG
    m_put_s("DEBUG",MESS_INFO,"Read commandline options!");
#endif
	
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{"help"     ,0,NULL,'h'},
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
	Sequrity().CheckCommandLine();
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
    try
    {
	opt = XMLCfgNode()->get_child("id","DefaultBD")->get_text();
	if( opt.size() )
    	{
    	    int pos = 0;
    	    DefBDType = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
    	    DefBDName = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
	}
    }
    catch(...) {  }

    Sequrity().UpdateOpt();
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
    char *dscr = "dscr";
    
    inf->load_xml( i_cntr );
    inf->set_text(Mess->I18Ns("Kernel: ")+Name());
    //a_gen
    XMLNode *c_nd = inf->get_child(1);
    c_nd->set_attr(dscr,Mess->I18N("Generic control"));
    c_nd->get_child(0)->set_attr(dscr,Mess->I18N("Default bd(module:bd)"));
    c_nd->get_child(2)->set_attr(dscr,Mess->I18N("Options help"));
    c_nd->get_child(3)->set_attr(dscr,Mess->I18N("Run"));
    c_nd->get_child(4)->set_attr(dscr,Mess->I18N("Update options(from config)"));    
    c_nd = inf->get_child(0);
    c_nd->set_attr(dscr,Mess->I18N("Subsystems"));
    c_nd->get_child(0)->set_attr(dscr,Mess->I18N("Subsystems"));
}

void TKernel::ctr_din_get_( string a_path, XMLNode *opt )
{
    vector<string> list;
    
    string t_id = ctr_path_l(a_path,0);
    if( t_id == "a_gen" )
    {
    	t_id = ctr_path_l(a_path,1);
	if( t_id == "def_tp_bd" ) ctr_opt_setS( opt, DefBDType );
	else if( t_id == "def_bd" )    ctr_opt_setS( opt, DefBDName ); 
	else if( t_id == "b_mod" )
	{
	    BD().gmd_list(list);
	    for( unsigned i_a=0; i_a < list.size(); i_a++ )
		ctr_opt_setS( opt, list[i_a], i_a );
	}
	else if( t_id == "g_help" )    ctr_opt_setS( opt, opt_descr() );       
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
} 

void TKernel::ctr_din_set_( string a_path, XMLNode *opt )
{
    string t_id = ctr_path_l(a_path,0);
    if( t_id == "a_gen" )
    {
    	t_id = ctr_path_l(a_path,1);
	if( t_id == "def_tp_bd" ) DefBDType = ctr_opt_getS( opt );
	else if( t_id == "def_bd" )    DefBDName = ctr_opt_getS( opt ); 
    }
}

void TKernel::ctr_cmd_go_( string a_path, XMLNode *fld, XMLNode *rez )
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
    int t_id = atoi(ctr_path_l(br,2).c_str());
    switch( t_id )
    {
	case 0: return( ModSchedul() );
	case 1: return( Sequrity() );
	case 2: return( Arhive() );
	case 3: return( BD() ) ;
	case 4: return( Controller() );
	case 5: return( Protocol() );
	case 6: return( Transport() );
	case 7: return( Special() );
	case 8: return( Param() );
	case 9: return( UI() );
	default: throw TError("(%s) Subsystem <%d> no avoid!",o_name, t_id);
    }
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
 

