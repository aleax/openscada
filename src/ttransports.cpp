#include <getopt.h>

#include "tsys.h"
#include "tkernel.h"
#include "tmessage.h"
#include "tmodule.h"
#include "ttransports.h"

//================================================================
//=========== TTransportS ========================================
//================================================================

SCfgFld TTransportS::gen_elem[] =
{
    {"NAME"    ,"Transport name."               ,CFG_T_STRING              ,"","",""           ,"20",""          ,"%s"},
    {"DESCRIPT","Transport description."        ,CFG_T_STRING              ,"","",""           ,"50",""          ,"%s"},    
    {"MODULE"  ,"Type transport (module name)." ,CFG_T_STRING              ,"","",""           ,"20",""          ,"%s"},
    {"ADDR"    ,"Transport address."            ,CFG_T_STRING              ,"","",""           ,"50",""          ,"%s"},
    {"PROT"    ,"Assign transport protocol."    ,CFG_T_STRING              ,"","",""           ,"20",""          ,"%s"},
    {"TYPE"    ,"Transport type (Input;Output).",CFG_T_BOOLEAN|CFG_T_SELECT,"","","false"      ,"1" ,"false;true","%s","Input;Output"},
    {"STAT"    ,"Transport's stat."             ,CFG_T_BOOLEAN|CFG_T_SELECT,"","","false"      ,"1" ,"false;true","%s","Disable;Enable"}
};

const char *TTransportS::o_name = "TTransportS";

TTransportS::TTransportS( TKernel *app ) : TGRPModule(app,"Transport"),	TConfig(NULL), m_bd("direct_dbf", "./DATA", "transport.dbf")
{
    for(unsigned i = 0; i < sizeof(gen_elem)/sizeof(SCfgFld); i++) 
	cf_ConfElem()->cfe_Add(&gen_elem[i]);
}

TTransportS::~TTransportS(  )
{
    vector<STrS> list;
    in_list( list );
    for(unsigned i_m = 0; i_m < list.size(); i_m++)
	try{ in_del( list[i_m] ); }
	catch(TError err) { Mess->put("SYS",MESS_ERR,"%s:%s",o_name, err.what().c_str()); }

    out_list( list );
    for(unsigned i_m = 0; i_m < list.size(); i_m++)
	try{ out_del( list[i_m] ); }
	catch(TError err) { Mess->put("SYS",MESS_ERR,"%s:%s",o_name, err.what().c_str()); }    
}

void TTransportS::gmd_Init( )
{
    LoadBD();
}

void TTransportS::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "========================= %s options ================================\n"
    "    --TRMPath=<path>  Set moduls <path>;\n"
    "------------------ Section fields of config file --------------------\n"
    " mod_path = <path>      set path to modules;\n"	    
    " GenBD    = <fullname>  generic bd recorded: \"<TypeBD>:<NameBD>:<NameTable>\";\n"
    " IN id=<name> type=<module> addr=<addr> prot=<prmod>\n"
    "                        add input transport <name> <module> <addr> <prmod> and description into text;\n"
    " OUT id=<name> type=<module> addr=<addr> prot=<prmod>\n"
    "                        add output transport <name> <module> <addr> <prmod> and description into text;\n"
    "    name   - name transport;\n"
    "    module - module transport;\n"
    "    addr   - addres transport;\n"
    "    prmod  - name assign to transport protocol;\n"
    "\n",gmd_Name().c_str());
}

void TTransportS::gmd_CheckCommandLine( )
{
    TGRPModule::gmd_CheckCommandLine( );
    
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{"TRMPath" ,1,NULL,'m'},
	{NULL      ,0,NULL,0  }
    };

    optind=opterr=0;	
    do
    {
	next_opt=getopt_long(SYS->argc,(char * const *)SYS->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': pr_opt_descr(stdout); break;
	    case 'm': DirPath = optarg;     break;
	    case -1 : break;
	}
    } while(next_opt != -1);
}

void TTransportS::gmd_UpdateOpt()
{
    TGRPModule::gmd_UpdateOpt();
    
    string opt;

    try{ DirPath = gmd_XMLCfgNode()->get_child("mod_path")->get_text(); }
    catch(...) {  }
    
    try
    {
    	opt = gmd_XMLCfgNode()->get_child("GenBD")->get_text(); 
	int pos = 0;
        m_bd.tp  = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
        m_bd.bd  = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
	m_bd.tbl = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
	if( !m_bd.tp.size() ) m_bd.tp = Owner().DefBDType;
	if( !m_bd.bd.size() ) m_bd.bd = Owner().DefBDName;
    }
    catch(...) {  }
    
    while(cf_Size()) cf_FreeRecord(0);
    try
    {
    	int i = 0;
    	while( true )
	{
	    XMLNode *t_n = gmd_XMLCfgNode()->get_child("IN",i++);
	    int rec = cf_AddRecord( cf_Size() );
	    cf_Set_S("NAME"    , t_n->get_attr("id")  , rec);
	    cf_Set_S("DESCRIPT", t_n->get_text()      , rec);
	    cf_Set_S("MODULE"  , t_n->get_attr("type"), rec);
	    cf_Set_S("ADDR"    , t_n->get_attr("addr"), rec);
	    cf_Set_SEL("TYPE"  , "Input", rec);
	    cf_Set_SEL("STAT"  , "Enable", rec);
    	    cf_Set_S("PROT"    , t_n->get_attr("prot"), rec);	   
	}	
    }
    catch(...) {  }
    try
    {
    	int i = 0;
    	while( true )
	{
	    XMLNode *t_n = gmd_XMLCfgNode()->get_child("OUT",i++);
	    int rec = cf_AddRecord( cf_Size() );
	    cf_Set_S("NAME"  , t_n->get_attr("id")  , rec);
	    cf_Set_S("DESCRIPT", t_n->get_text()    , rec);
	    cf_Set_S("MODULE", t_n->get_attr("type"), rec);
	    cf_Set_S("ADDR"  , t_n->get_attr("addr"), rec);
	    cf_Set_SEL("TYPE", "Output", rec);
	    cf_Set_SEL("STAT", "Enable", rec);
    	    cf_Set_S("PROT"  , t_n->get_attr("prot"), rec);	   
	}	
    }
    catch(...) {  }    
}

void TTransportS::LoadBD( )
{    
    try
    {
	SHDBD b_hd = Owner().BD().open( m_bd );
	cf_LoadAllValBD( Owner().BD().at(b_hd) );
	cf_FreeDubl("NAME",false);   //Del new (from bd)
	Owner().BD().close(b_hd);
    }catch(TError err) { Mess->put("SYS",MESS_ERR,"%s:%s",o_name,err.what().c_str()); }    
    
    
    //Open transports (open new transports)
    for(unsigned i_cfg = 0; i_cfg < cf_Size(); i_cfg++)
    {
	if( cf_Get_SEL("TYPE", i_cfg) == "Input" && cf_Get_SEL("STAT", i_cfg) == "Enable" )
	{
	    try 
	    { 
    		in_add( STrS(cf_Get_S("MODULE", i_cfg), cf_Get_S("NAME", i_cfg)), cf_Get_S("ADDR", i_cfg), cf_Get_S("PROT", i_cfg) );
	    }
	    catch(TError err) { Mess->put("SYS",MESS_ERR,"%s:%s",o_name,err.what().c_str()); }			
	}
	else if( cf_Get_SEL("TYPE", i_cfg) == "Output" && cf_Get_SEL("STAT", i_cfg) == "Enable" )
	{
	    try 
	    { 
    		out_add( STrS(cf_Get_S("MODULE", i_cfg), cf_Get_S("NAME", i_cfg)), cf_Get_S("ADDR", i_cfg) );
	    }
	    catch(TError err) { Mess->put("SYS",MESS_ERR,"%s:%s",o_name,err.what().c_str()); }			
	}
    }
    //Close no avoid in bd transports    
}

void TTransportS::UpdateBD( )
{
    SHDBD b_hd;
    
    try{ b_hd = Owner().BD().open( m_bd ); }
    catch(...) { b_hd = Owner().BD().open( m_bd,true ); }
    cf_ConfElem()->cfe_UpdateBDAttr( Owner().BD().at(b_hd) );
    cf_SaveAllValBD( Owner().BD().at(b_hd) );
    Owner().BD().at(b_hd).Save();
    Owner().BD().close(b_hd);
}

void TTransportS::in_list( vector<STrS> &list )
{
    list.clear();
    vector<string> m_list;
    gmd_list(m_list);
    for( unsigned i_m = 0; i_m < m_list.size(); i_m++ )
    {
	unsigned m_hd = gmd_att( m_list[i_m] );
	vector<string> tr_list;
	gmd_at(m_hd).in_list(tr_list);
	for( unsigned i_tr = 0; i_tr < tr_list.size(); i_tr++ )
	    list.push_back( STrS(m_list[i_m],tr_list[i_tr]) );
	gmd_det( m_hd );
    }
}

void TTransportS::in_add( STrS tr, string address, string prot )
{
    unsigned m_hd = gmd_att( tr.tp );
    try { gmd_at(m_hd).in_add( tr.obj, address, prot ); }
    catch( TError err )
    {
	gmd_det( m_hd );
	throw;
    }
    gmd_det( m_hd );
}

void TTransportS::in_del( STrS tr )
{
    unsigned m_hd = gmd_att( tr.tp );
    try{ gmd_at(m_hd).in_del( tr.obj ); }
    catch(...)
    {
	gmd_det( m_hd );
	throw;
    }
    gmd_det( m_hd );
}

SHDTr TTransportS::in_att( STrS tr )
{
    SHDTr HDTr;
    HDTr.h_tp  = gmd_att( tr.tp );
    try{ HDTr.h_obj = gmd_at(HDTr.h_tp).in_att( tr.obj ); }
    catch(...)
    {
	gmd_det( HDTr.h_tp );
	throw;
    }

    return(HDTr);
}

void TTransportS::in_det( SHDTr &hd )
{
    gmd_at( hd.h_tp ).in_det( hd.h_obj );
    gmd_det( hd.h_tp );
}

void TTransportS::out_list( vector<STrS> &list )
{
    list.clear();
    vector<string> m_list;
    gmd_list(m_list);
    for( unsigned i_m = 0; i_m < m_list.size(); i_m++ )
    {
	unsigned m_hd = gmd_att( m_list[i_m] );
	vector<string> tr_list;
	gmd_at(m_hd).out_list(tr_list);
	for( unsigned i_tr = 0; i_tr < tr_list.size(); i_tr++ )
	    list.push_back( STrS(m_list[i_m],tr_list[i_tr]) );
	gmd_det( m_hd );
    }
}

void TTransportS::out_add( STrS tr, string address )
{
    unsigned m_hd = gmd_att( tr.tp );
    try{ gmd_at(m_hd).out_add( tr.obj, address ); }
    catch(...)
    {
	gmd_det( m_hd );
	throw;
    }
    gmd_det( m_hd );
}

void TTransportS::out_del( STrS tr )
{    
    unsigned m_hd = gmd_att( tr.tp );
    try{ gmd_at(m_hd).out_del( tr.obj ); }
    catch(...)
    {
	gmd_det( m_hd );
	throw;
    }
    gmd_det( m_hd );
}

SHDTr TTransportS::out_att( STrS tr )
{
    SHDTr HDTr;
    HDTr.h_tp  = gmd_att( tr.tp );
    try{ HDTr.h_obj = gmd_at(HDTr.h_tp).out_att( tr.obj ); }
    catch(...)
    {
	gmd_det( HDTr.h_tp );
	throw;
    }

    return(HDTr);
}

void TTransportS::out_det( SHDTr &hd )
{
    gmd_at( hd.h_tp ).out_det( hd.h_obj );
    gmd_det( hd.h_tp );
}

void TTransportS::gmd_del( string name )
{
    vector<STrS> list;
    in_list( list );
    for(unsigned i_m = 0; i_m < list.size(); i_m++)
	if( list[i_m].tp == name ) in_del( list[i_m] );

    out_list( list );
    for(unsigned i_m = 0; i_m < list.size(); i_m++)
	if( list[i_m].tp == name ) out_del( list[i_m] );

    TGRPModule::gmd_del( name );
}


//================================================================
//=========== TTipTransport ======================================
//================================================================
const char *TTipTransport::o_name = "TTipTransport";

TTipTransport::TTipTransport() : m_hd_in(o_name), m_hd_out(o_name)
{

}
    
TTipTransport::~TTipTransport()
{
    vector<string> list;

    m_hd_in.lock();
    in_list(list);
    for( unsigned i_ls = 0; i_ls < list.size(); i_ls++)
	in_del(list[i_ls]);

    m_hd_out.lock();
    out_list(list);
    for( unsigned i_ls = 0; i_ls < list.size(); i_ls++)
	out_del(list[i_ls]);
}

void TTipTransport::in_add( string name, string address, string prot )
{
    TTransportIn *tr_in = In(name,address,prot);
    try{ m_hd_in.obj_add( tr_in, &tr_in->Name() ); }
    catch(TError err) { delete tr_in; }
}

void TTipTransport::out_add( string name, string address )
{
    TTransportOut *tr_out = Out(name,address);
    try{ m_hd_out.obj_add( tr_out, &tr_out->Name() ); }
    catch(TError err) { delete tr_out; }
}

//================================================================
//=========== TTransportIn =======================================
//================================================================
const char *TTransportIn::o_name = "TTransportIn";

TTransportIn::~TTransportIn()
{
    
}

//================================================================
//=========== TTransportOut ======================================
//================================================================
const char *TTransportOut::o_name = "TTransportOut";

TTransportOut::~TTransportOut()
{
    
}

