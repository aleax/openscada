#include <getopt.h>

#include "tsys.h"
#include "tkernel.h"
#include "tmessage.h"
#include "tmodule.h"
#include "tprotocols.h"
#include "ttransports.h"

//================================================================
//=========== TTransportS ========================================
//================================================================

SFld TTransportS::gen_elem[] =
{
    {"NAME"    ,"Transport name."               ,T_STRING       ,""     ,"20"},
    {"DESCRIPT","Transport description."        ,T_STRING       ,""     ,"50"},    
    {"MODULE"  ,"Type transport (module name)." ,T_STRING       ,""     ,"20"},
    {"ADDR"    ,"Transport address."            ,T_STRING       ,""     ,"50"},
    {"PROT"    ,"Assign transport protocol."    ,T_STRING       ,""     ,"20"},
    {"TYPE"    ,"Transport type (Input;Output).",T_BOOL|T_SELECT,"false","1" ,"false;true","Input;Output"}
};

const char *TTransportS::o_name = "TTransportS";
const char *TTransportS::i_cntr = 
    "<area id='a_bd' acs='0440'>"
    " <fld id='t_bd' acs='0660' tp='str' dest='select' select='a_bd/b_mod'/>"
    " <fld id='bd' acs='0660' tp='str'/>"
    " <fld id='tbl' acs='0660' tp='str'/>"
    " <fld id='g_help' acs='0440' tp='str' cols='90' rows='5'/>"
    " <comm id='load_bd'/>"
    " <comm id='upd_bd'/>"
    " <list id='b_mod' tp='str' hide='1'/>"
    "</area>";

TTransportS::TTransportS( TKernel *app ) 
    : TGRPModule(app,"Transport"), TElem(""), m_bd("","","transport.dbf") 
{
    s_name = "Transports"; 
    for(unsigned i = 0; i < sizeof(gen_elem)/sizeof(SFld); i++) elAdd(&gen_elem[i]);
}

TTransportS::~TTransportS(  )
{
    vector<STrS> list;
    in_list( list );
    for(unsigned i_m = 0; i_m < list.size(); i_m++)
	try{ in_del( list[i_m] ); }
	catch(TError err) { m_put_s("SYS",MESS_ERR,err.what()); }

    out_list( list );
    for(unsigned i_m = 0; i_m < list.size(); i_m++)
	try{ out_del( list[i_m] ); }
	catch(TError err) { m_put_s("SYS",MESS_ERR,err.what()); }    
}

void TTransportS::gmd_Init( )
{
    LoadBD();
}

void TTransportS::gmd_Start( )
{    
    vector<STrS> list;
    in_list(list);
    for( int i_l = 0; i_l < list.size(); i_l++ )
    {
	SHDTr hd = in_att(list[i_l]);
	try{ in_at(hd).start(); }catch(...){}
	in_det( hd );
    }    
    out_list(list);
    for( int i_l = 0; i_l < list.size(); i_l++ )
    {
	SHDTr hd = out_att(list[i_l]);
	try{ out_at(hd).start(); }catch(...){}
	out_det( hd );
    }    
}

void TTransportS::gmd_Stop( )
{    
    vector<STrS> list;
    in_list(list);
    for( int i_l = 0; i_l < list.size(); i_l++ )
    {
	SHDTr hd = in_att(list[i_l]);
	try{ in_at(hd).stop(); }catch(...){} 
	in_det( hd );
    }    
    out_list(list);
    for( int i_l = 0; i_l < list.size(); i_l++ )
    {
	SHDTr hd = out_att(list[i_l]);
	try{ out_at(hd).stop(); }catch(...){} 
	out_det( hd );
    }    
}

string TTransportS::opt_descr( )
{
    char buf[STR_BUF_LEN];
    snprintf(buf,sizeof(buf),Mess->I18N(
    	"======================= The transport subsystem options ===================\n"
	"    --TRMPath=<path>  Set moduls <path>;\n"
	"------------ Parameters of section <%s> in config file -----------\n"
	"mod_path  <path>      set path to modules;\n"
	"GenBD     <fullname>  generic bd recorded: \"<TypeBD>:<NameBD>:<NameTable>\";\n"
	),gmd_Name().c_str());

    return(buf);
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
	    case 'h': fprintf(stdout,opt_descr().c_str()); break;
	    case 'm': DirPath = optarg;     break;
	    case -1 : break;
	}
    } while(next_opt != -1);
}

void TTransportS::gmd_UpdateOpt()
{
    TGRPModule::gmd_UpdateOpt();
    
    string opt;

    try{ DirPath = gmd_XMLCfgNode()->get_child("id","mod_path")->get_text(); }
    catch(...) {  }
    
    try
    {
    	opt = gmd_XMLCfgNode()->get_child("id","GenBD")->get_text(); 
	int pos = 0;
        m_bd.tp  = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
        m_bd.bd  = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
	m_bd.tbl = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
    }
    catch(...) {  }
    if( !m_bd.tp.size() ) m_bd.tp = Owner().DefBDType;
    if( !m_bd.bd.size() ) m_bd.bd = Owner().DefBDName;
}

void TTransportS::LoadBD( )
{ 
    TConfig *c_el;
    string name,type,module;
    
    try
    {
	SHDBD b_hd = Owner().BD().open( m_bd );    
	for( int i_ln = 0; i_ln < Owner().BD().at(b_hd).NLines(); i_ln++ )
	{
	    c_el = new TConfig(this);
	    c_el->cfLoadValBD(i_ln,Owner().BD().at(b_hd));
	    name   = c_el->cfg("NAME").getS();
	    module = c_el->cfg("MODULE").getS();
	    type   = c_el->cfg("TYPE").getSEL();
	    delete c_el;	
	
	    try
	    {
		if( type == "Input" )
		{
		    try{in_add(STrS(module,name));}catch(...){}
		    SHDTr hd = in_att(STrS(module,name));
		    in_at(hd).cfLoadValBD(i_ln,Owner().BD().at(b_hd));
		    in_det(hd);
		}
		else if( type == "Output" )
		{
		    try{out_add(STrS(module,name));}catch(...){}
		    SHDTr hd = out_att(STrS(module,name));
		    out_at(hd).cfLoadValBD(i_ln,Owner().BD().at(b_hd));
		    out_det(hd);
		}
	    }catch(TError err){ m_put_s("SYS",MESS_ERR,err.what()); }	    
	}
	Owner().BD().close(b_hd);
    }catch(...){}
}

void TTransportS::UpdateBD( )
{    
    vector<STrS> list;
    SHDBD b_hd;
    
    try{ b_hd = Owner().BD().open( m_bd ); }
    catch(...) { b_hd = Owner().BD().open( m_bd,true ); }
    Owner().BD().at(b_hd).Clean();
    elUpdateBDAttr( Owner().BD().at(b_hd) );
    in_list(list);
    for( int i_l = 0; i_l < list.size(); i_l++ )
    {
	SHDTr hd = in_att(list[i_l]);
	in_at(hd).cfSaveValBD(-1,Owner().BD().at(b_hd));
	in_det( hd );
    }
    out_list(list);
    for( int i_l = 0; i_l < list.size(); i_l++ )
    {
	SHDTr hd = out_att(list[i_l]);
	out_at(hd).cfSaveValBD(-1,Owner().BD().at(b_hd));
	out_det( hd );
    }
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

void TTransportS::in_add( STrS tr )
{    
    unsigned m_hd = gmd_att( tr.tp );
    try { gmd_at(m_hd).in_add( tr.obj ); }
    catch( TError err ) { gmd_det( m_hd ); throw; }
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

void TTransportS::out_add( STrS tr )
{
    unsigned m_hd = gmd_att( tr.tp );
    try{ gmd_at(m_hd).out_add( tr.obj ); }
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

//==============================================================
//================== Controll functions ========================
//==============================================================
void TTransportS::ctr_fill_info( XMLNode *inf )
{
    char *dscr = "dscr";
    TGRPModule::ctr_fill_info( inf );
    
    XMLNode *n_add = inf->add_child();
    n_add->load_xml(i_cntr);
    n_add->set_attr(dscr,Mess->I18N("Subsystem control"));
    n_add->get_child(0)->set_attr(dscr,Mess->I18N("BD (module:bd:table)"));
    n_add->get_child(3)->set_attr(dscr,Mess->I18N("Options help"));
    n_add->get_child(4)->set_attr(dscr,Mess->I18N("Load BD"));
    n_add->get_child(5)->set_attr(dscr,Mess->I18N("Update BD"));
}

void TTransportS::ctr_din_get_( string a_path, XMLNode *opt )
{
    vector<string> list;
    
    TGRPModule::ctr_din_get_( a_path, opt );
    
    string t_id = ctr_path_l(a_path,0);
    if( t_id == "a_bd" )
    {
	t_id = ctr_path_l(a_path,1);
	if( t_id == "t_bd" )     ctr_opt_setS( opt, m_bd.tp );
	else if( t_id == "bd" )  ctr_opt_setS( opt, m_bd.bd );
	else if( t_id == "tbl" ) ctr_opt_setS( opt, m_bd.tbl );
	else if( t_id == "b_mod" )
	{
	    Owner().BD().gmd_list(list);
	    for( unsigned i_a=0; i_a < list.size(); i_a++ )
		ctr_opt_setS( opt, list[i_a], i_a );
	}
	else if( t_id == "g_help" ) ctr_opt_setS( opt, opt_descr() );       
    }
}

void TTransportS::ctr_din_set_( string a_path, XMLNode *opt )
{
    TGRPModule::ctr_din_set_( a_path, opt );
    
    string t_id = ctr_path_l(a_path,0);
    if( t_id == "a_bd" )
    {
	t_id = ctr_path_l(a_path,1);
	if( t_id == "t_bd" )       m_bd.tp    = ctr_opt_getS( opt );
	else if( t_id == "bd" )    m_bd.bd    = ctr_opt_getS( opt );
	else if( t_id == "tbl" )   m_bd.tbl   = ctr_opt_getS( opt );
    }   
}

void TTransportS::ctr_cmd_go_( string a_path, XMLNode *fld, XMLNode *rez )
{
    TGRPModule::ctr_cmd_go_( a_path, fld, rez );
    
    string t_id = ctr_path_l(a_path,0);
    if( t_id == "a_bd" )
    {
	t_id = ctr_path_l(a_path,1);
	if( t_id == "load_bd" )     LoadBD();
	else if( t_id == "upd_bd" ) UpdateBD();
    }
}


//================================================================
//=========== TTipTransport ======================================
//================================================================
const char *TTipTransport::o_name = "TTipTransport";
const char *TTipTransport::i_cntr = 
    "<area id='a_tr'>"
    " <list id='in' s_com='add,del' tp='br' mode='att'/>"
    " <list id='out' s_com='add,del' tp='br' mode='att'/>"
    "</area>";

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

void TTipTransport::in_add( string name )
{
    TTransportIn *tr_in = In( name );
    try{ m_hd_in.obj_add( tr_in, &tr_in->Name() ); }
    catch(TError err) { delete tr_in; }
}

void TTipTransport::out_add( string name )
{
    TTransportOut *tr_out = Out(name);
    try{ m_hd_out.obj_add( tr_out, &tr_out->Name() ); }
    catch(TError err) { delete tr_out; }
}

//================== Controll functions ========================
void TTipTransport::ctr_fill_info( XMLNode *inf )
{
    char *dscr="dscr";
    
    TModule::ctr_fill_info( inf );
    
    XMLNode *n_add = inf->add_child();
    n_add->load_xml(i_cntr);
    n_add->set_attr(dscr,Mess->I18N("Transports of the transport type"));
    n_add->get_child(0)->set_attr(dscr,Mess->I18N("Input transports"));
    n_add->get_child(1)->set_attr(dscr,Mess->I18N("Output transports"));
}

void TTipTransport::ctr_din_get_( string a_path, XMLNode *opt )
{
    vector<string> list;
    
    TModule::ctr_din_get_( a_path, opt );

    string t_id = ctr_path_l(a_path,0);
    if( t_id == "a_tr" )
    {
	t_id = ctr_path_l(a_path,1);
	if( t_id == "in" )
	{
	    in_list(list);
	    for( unsigned i_a=0; i_a < list.size(); i_a++ )
		ctr_opt_setS( opt, list[i_a], i_a ); 	
	}
	else if( t_id == "out" )
	{
	    out_list(list);
	    for( unsigned i_a=0; i_a < list.size(); i_a++ )
		ctr_opt_setS( opt, list[i_a], i_a ); 	
	}   
    }
}

void TTipTransport::ctr_din_set_( string a_path, XMLNode *opt )
{
    TModule::ctr_din_set_( a_path, opt );
    
    string t_id = ctr_path_l(a_path,0);
    if( t_id == "a_tr" )
    {
	t_id = ctr_path_l(a_path,1);
	if( t_id == "in" )
	    for( int i_el=0; i_el < opt->get_child_count(); i_el++)	    
	    {
		XMLNode *t_c = opt->get_child(i_el);
		if( t_c->get_name() == "el")
		{
		    if(t_c->get_attr("do") == "add")      in_add(t_c->get_text());
		    else if(t_c->get_attr("do") == "del") in_del(t_c->get_text());
		}
	    }
	else if( t_id == "out" )
	    for( int i_el=0; i_el < opt->get_child_count(); i_el++)	    
	    {
		XMLNode *t_c = opt->get_child(i_el);
		if( t_c->get_name() == "el")
		{
		    if(t_c->get_attr("do") == "add")      out_add(t_c->get_text());
		    else if(t_c->get_attr("do") == "del") out_del(t_c->get_text());
		}
	    }
    }
}

unsigned TTipTransport::ctr_att( string a_path )
{
    if( ctr_path_l(a_path,0) == "a_tr" )
    {
	string t_id = ctr_path_l(a_path,1);
	if( t_id == "in" )       return(in_att(ctr_path_l(a_path,2)));
	else if( t_id == "out" ) return(out_att(ctr_path_l(a_path,2)));
    }
    throw TError("(%s) Branch %s error",o_name,a_path.c_str());
}

void TTipTransport::ctr_det( string a_path, unsigned hd )
{
    if( ctr_path_l(a_path,0) == "a_tr" )
    {
	string t_id = ctr_path_l(a_path,1);
	if( t_id == "in" )       { in_det(hd); return; }
	else if( t_id == "out" ) { out_det(hd);  return; }
    }
    throw TError("(%s) Branch %s error",o_name,a_path.c_str());
}

TContr &TTipTransport::ctr_at( string a_path, unsigned hd )
{
    if( ctr_path_l(a_path,0) == "a_tr" )
    {
	string t_id = ctr_path_l(a_path,1);
	if( t_id == "in" )       return(in_at(hd));
	else if( t_id == "out" ) return(out_at(hd));
    }
    throw TError("(%s) Branch %s error",o_name,a_path.c_str());
}
//================================================================
//=========== TTransportIn =======================================
//================================================================
const char *TTransportIn::o_name = "TTransportIn";
const char *TTransportIn::i_cntr = 
    "<oscada_cntr>"
    " <area id='a_prm'>"
    "  <fld id='name' acs='0664' tp='str'/>"
    "  <fld id='dscr' acs='0664' tp='str'/>"
    "  <fld id='addr' acs='0664' tp='str'/>"
    "  <fld id='prot' acs='0664' tp='str' dest='select' select='a_prm/p_mod'/>"
    "  <fld id='r_st' acs='0664' tp='bool'/>"
    "  <list id='p_mod' tp='str' hide='1'/>"
    " </area>"
    "</oscada_cntr>";

TTransportIn::TTransportIn( string name, TTipTransport *owner ) : 
    m_owner(owner), TConfig((TTransportS *)&(owner->Owner())), run_st(false),
    m_name(cfg("NAME").getS()), m_lname(cfg("DESCRIPT").getS()), m_addr(cfg("ADDR").getS()), m_prot(cfg("PROT").getS())
{
    m_name = name;
    cfg("TYPE").setSEL("Input");
    cfg("MODULE").setS(Owner().mod_Name());
}
    
TTransportIn::~TTransportIn()
{
    
}

//================== Controll functions ========================
void TTransportIn::ctr_fill_info( XMLNode *inf )
{
    char *dscr = "dscr";

    inf->load_xml( i_cntr );
    inf->set_text(Mess->I18N("Input transport: ")+Name());
    XMLNode *n_add = inf->get_child(0);
    n_add->set_attr(dscr,Mess->I18N("Parameters"));
    n_add->get_child(0)->set_attr(dscr,Mess->I18N("Name"));
    n_add->get_child(1)->set_attr(dscr,Mess->I18N("Full name"));
    n_add->get_child(2)->set_attr(dscr,Mess->I18N("Address"));
    n_add->get_child(3)->set_attr(dscr,Mess->I18N("Protocol"));
    n_add->get_child(4)->set_attr(dscr,Mess->I18N("Runing"));
}

void TTransportIn::ctr_din_get_( string a_path, XMLNode *opt )
{
    string t_id = ctr_path_l(a_path,0);    
    if( t_id == "a_prm" )
    {
    	t_id = ctr_path_l(a_path,1);
    	if( t_id == "name" )       ctr_opt_setS( opt, m_name );
    	else if( t_id == "dscr" )  ctr_opt_setS( opt, m_lname );
    	else if( t_id == "addr" )  ctr_opt_setS( opt, m_addr );
    	else if( t_id == "prot" )  ctr_opt_setS( opt, m_prot );
    	else if( t_id == "r_st" )  ctr_opt_setB( opt, run_st );
    	else if( t_id == "p_mod" )
	{
	    vector<string> list;
	    Owner().Owner().Owner().Protocol().gmd_list(list);
	    for( unsigned i_a=0; i_a < list.size(); i_a++ )
		ctr_opt_setS( opt, list[i_a], i_a );
	}
    }    
}

void TTransportIn::ctr_din_set_( string a_path, XMLNode *opt )
{
    string t_id = ctr_path_l(a_path,0);    
    if( t_id == "a_prm" )
    {
    	t_id = ctr_path_l(a_path,1);
    	if( t_id == "name" )       m_name  = ctr_opt_getS( opt );
    	else if( t_id == "dscr" )  m_lname = ctr_opt_getS( opt );
    	else if( t_id == "addr" )  m_addr  = ctr_opt_getS( opt );
    	else if( t_id == "prot" )  m_prot  = ctr_opt_getS( opt );
    	else if( t_id == "r_st" ) { if( ctr_opt_getB( opt ) ) start(); else stop(); }
    }
    
}

//================================================================
//=========== TTransportOut ======================================
//================================================================
const char *TTransportOut::o_name = "TTransportOut";
const char *TTransportOut::i_cntr = 
    "<oscada_cntr>"
    " <area id='a_prm'>"
    "  <fld id='name' acs='0664' tp='str'/>"
    "  <fld id='dscr' acs='0664' tp='str'/>"
    "  <fld id='addr' acs='0664' tp='str'/>"
    "  <fld id='r_st' acs='0664' tp='bool'/>"
    " </area>"
    "</oscada_cntr>";

TTransportOut::TTransportOut(string name, TTipTransport *owner ) : 
    m_owner(owner), TConfig((TTransportS *)&(owner->Owner())), run_st(false),
    m_name(cfg("NAME").getS()), m_lname(cfg("DESCRIPT").getS()), m_addr(cfg("ADDR").getS()) 
{ 
    m_name = name;
    cfg("TYPE").setSEL("Output");
    cfg("MODULE").setS(Owner().mod_Name());
}

TTransportOut::~TTransportOut()
{
    
}

//================== Controll functions ========================
void TTransportOut::ctr_fill_info( XMLNode *inf )
{
    char *dscr = "dscr";

    inf->load_xml( i_cntr );
    inf->set_text(Mess->I18N("Output transport: ")+Name());
    XMLNode *n_add = inf->get_child(0);
    n_add->set_attr(dscr,Mess->I18N("Parameters"));
    n_add->get_child(0)->set_attr(dscr,Mess->I18N("Name"));
    n_add->get_child(1)->set_attr(dscr,Mess->I18N("Full name"));
    n_add->get_child(2)->set_attr(dscr,Mess->I18N("Address"));
    n_add->get_child(3)->set_attr(dscr,Mess->I18N("Runing"));
}

void TTransportOut::ctr_din_get_( string a_path, XMLNode *opt )
{
    string t_id = ctr_path_l(a_path,0);    
    if( t_id == "a_prm" )
    {
    	t_id = ctr_path_l(a_path,1);
    	if( t_id == "name" )       ctr_opt_setS( opt, m_name );
    	else if( t_id == "dscr" )  ctr_opt_setS( opt, m_lname );
    	else if( t_id == "addr" )  ctr_opt_setS( opt, m_addr );
    	else if( t_id == "r_st" )  ctr_opt_setB( opt, run_st );
    }    
}

void TTransportOut::ctr_din_set_( string a_path, XMLNode *opt )
{
    string t_id = ctr_path_l(a_path,0);    
    if( t_id == "a_prm" )
    {
    	t_id = ctr_path_l(a_path,1);
    	if( t_id == "name" )       m_name  = ctr_opt_getS( opt );
    	else if( t_id == "dscr" )  m_lname = ctr_opt_getS( opt );
    	else if( t_id == "addr" )  m_addr  = ctr_opt_getS( opt );
    	else if( t_id == "r_st" ) { if( ctr_opt_getB( opt ) ) start(); else stop(); }
    }    
}

