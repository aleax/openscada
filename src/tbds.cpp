#include <unistd.h>
#include <getopt.h>

#include "tsys.h"
#include "tkernel.h"
#include "tmessage.h"
#include "tmodule.h"
#include "tbds.h"

//================================================================
//=========== TBDS ===============================================
//================================================================

const char *TBDS::o_name = "TBDS";

TBDS::TBDS( TKernel *app ) : TGRPModule(app,"BaseDate")
{

}

TBDS::~TBDS(  )
{

}

void TBDS::list( vector<SBDS> &list )
{
    list.clear();
    vector<string> m_list;
    gmd_list(m_list);
    for( unsigned i_m = 0; i_m < m_list.size(); i_m++ )
    {
	unsigned m_hd = gmd_att( m_list[i_m] );
	vector<string> b_list;
	gmd_at(m_hd).list(b_list);
	for( unsigned i_b = 0; i_b < b_list.size(); i_b++ )
	{
    	    unsigned b_hd = gmd_at(m_hd).open( b_list[i_b], false );
    	    vector<string> t_list;
    	    gmd_at(m_hd).at(b_hd).list(t_list);
    	    for( unsigned i_t = 0; i_t < t_list.size(); i_t++ )
    		list.push_back( SBDS( m_list[i_m], b_list[i_b], t_list[i_t]) );
    	    gmd_at(m_hd).close( b_hd );
	}
    	gmd_det( m_hd );
    }
}

SHDBD TBDS::open( SBDS bd_t, bool create )
{
    SHDBD HDBD;
    HDBD.h_tp = gmd_att( bd_t.tp );
    try{ HDBD.h_bd = gmd_at(HDBD.h_tp).open( bd_t.bd, create ); }
    catch(...) { gmd_det( HDBD.h_tp ); throw; }
    try{ HDBD.h_tbl = gmd_at(HDBD.h_tp).at(HDBD.h_bd).open( bd_t.tbl, create ); }
    catch(...) { gmd_at(HDBD.h_tp).close(HDBD.h_bd); gmd_det( HDBD.h_tp ); throw; }
    
    return( HDBD );
}

void TBDS::close( SHDBD &hd )
{
    gmd_at(hd.h_tp).at(hd.h_bd).close(hd.h_tbl);
    gmd_at(hd.h_tp).close(hd.h_bd);
    gmd_det(hd.h_tp);
}

void TBDS::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "========================= %s options ======================================\n"
    "    --BDMPath=<path>    Set moduls <path>;\n"
    "------------------ Section fields of config file --------------------\n"
    "mod_path=<path>    set path to modules;\n"
    "\n",gmd_Name().c_str());
}


void TBDS::gmd_CheckCommandLine( )
{
    TGRPModule::gmd_CheckCommandLine( );
    
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{"BDMPath" ,1,NULL,'m'},
	{NULL      ,0,NULL,0  }
    };

    optind=opterr=0;	
    do
    {
	next_opt=getopt_long(SYS->argc,(char * const *)SYS->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': pr_opt_descr(stdout); break;
	    case 'm': DirPath  = optarg; break;
	    case -1 : break;
	}
    } while(next_opt != -1);
//    if(optind < App->argc) pr_opt_descr(stdout);
}

void TBDS::gmd_UpdateOpt()
{
    TGRPModule::gmd_UpdateOpt();
    
    try{ DirPath = gmd_XMLCfgNode()->get_child("mod_path")->get_text(); }
    catch(...) {  }
}

//================================================================
//=========== TTipBD =============================================
//================================================================

const char *TTipBD::o_name = "TTipBD";
TTipBD::TTipBD(  )
{ 

};

TTipBD::~TTipBD( )
{
    m_hd_bd.lock();
    SYS->event_wait( m_hd_bd.hd_obj_free(), true, string(o_name)+": BDs is closing....");
    /*
    while( m_hd_bd.hd_obj_cnt() )
    {
	Mess->put("SYS",MESS_WARNING,"%s: No all BD closed!",o_name);
	sleep(1);
    }
    */
}

unsigned TTipBD::open( string name, bool create )
{
    TBD *t_bd = BDOpen(name,create);
    try { m_hd_bd.hd_obj_add( t_bd, &t_bd->Name() ); }
    catch(TError err) {	delete t_bd; }
    return( m_hd_bd.hd_att( t_bd->Name() ) );
}

void TTipBD::close( unsigned hd )
{
    string name = at(hd).Name();
    m_hd_bd.hd_det( hd );
    if( !m_hd_bd.obj_use( name ) )
    	delete (TBD *)m_hd_bd.hd_obj_del( name );
}

//================================================================
//=========== TBD ================================================
//================================================================

const char *TBD::o_name = "TBD";


TBD::TBD( string &name ) : m_name(name) 
{    

}

TBD::~TBD()
{
    m_hd_tb.lock();
    SYS->event_wait( m_hd_tb.hd_obj_free(), true, string(o_name)+": Tables is closing....");
    /*
    while( m_hd_tb.hd_obj_cnt() )
    {
	Mess->put("SYS",MESS_WARNING,"%s: No all tables closed!",o_name);
	sleep(1);
    }
    */
}

unsigned TBD::open( string table, bool create )
{
    TTable *tbl = TableOpen(table, create);    
    try { m_hd_tb.hd_obj_add( tbl, &tbl->Name() ); }
    catch(TError err) {	delete tbl; }
    return( m_hd_tb.hd_att( tbl->Name() ) );
}

void TBD::close( unsigned hd )
{
    string name = at(hd).Name();
    m_hd_tb.hd_det( hd );
    if( !m_hd_tb.obj_use( name ) )
    	delete (TTable *)m_hd_tb.hd_obj_del( name );
}

//================================================================
//=========== TTable =============================================
//================================================================
const char *TTable::o_name = "TTable";
char *TTable::_err   = "%s: function %s no support!";

TTable::TTable( string &name ) :  m_name(name)
{

};

TTable::~TTable()
{ 

};  
    

