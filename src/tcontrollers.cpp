#include <getopt.h>

#include "tsys.h"
#include "tkernel.h"
#include "tmessage.h"
#include "tcontroller.h"
#include "ttipcontroller.h"
#include "tmodule.h"
#include "tvalue.h"
#include "tcontrollers.h"


SCfgFld TControllerS::gen_elem[] =
{
    {"NAME"    ,"Controller's name."               ,CFG_T_STRING              ,"","",""           ,"20"},
    {"MODUL"   ,"Module(plugin) of type controler.",CFG_T_STRING              ,"","",""           ,"20"},
    {"BDTYPE"  ,"Type controller's BD."            ,CFG_T_STRING              ,"","","direct_dbf" ,"20"},
    {"BDNAME"  ,"Name controller's BD."            ,CFG_T_STRING              ,"","","./DATA"     ,"50"},
    {"TABLE"   ,"Name controller's Table."         ,CFG_T_STRING              ,"","","contr.dbf"  ,"20"}
};

const char *TControllerS::o_name = "TControllerS";
const char *TControllerS::i_cntr = 
    "<area id='a_bd' acs='0440'>"
    " <fld id='t_bd' acs='0660' tp='str' dest='select' select='a_bd/b_mod'/>"
    " <fld id='bd' acs='0660' tp='str'/>"
    " <fld id='tbl' acs='0660' tp='str'/>"
    " <fld id='g_help' acs='0440' tp='str' cols='90' rows='5'/>"
    " <comm id='load_bd'/>"
    " <comm id='upd_bd'/>"
    " <list id='b_mod' tp='str' hide='1'/>"
    "</area>";

TControllerS::TControllerS( TKernel *app ) 
	: TGRPModule(app,"Controller"), m_bd("direct_dbf", "./DATA", "generic.dbf") 
{
    s_name = "Controllers";
    for(unsigned i = 0; i < sizeof(gen_elem)/sizeof(SCfgFld); i++) cfe_Add(&gen_elem[i]);    
}

TControllerS::~TControllerS(  )
{
    gmd_Stop();
    
    vector<SCntrS> m_list;
    list( m_list );
    for(unsigned i_m = 0; i_m < m_list.size(); i_m++)
    	try{ del( m_list[i_m] ); }
	catch(TError err) { m_put_s("SYS",MESS_ERR,err.what()); }
}

void TControllerS::gmd_Init( )
{
    LoadBD();
}

void TControllerS::gmd_Start(  )         
{
    vector<SCntrS> m_list;
    list( m_list );
    for(unsigned i_m = 0; i_m < m_list.size(); i_m++)
    {
	SHDCntr hd = att(m_list[i_m]);
	if( at(hd).auto_start() )
    	    try{ at(hd).Start( ); }
    	    catch(TError err) { m_put_s("SYS",MESS_ERR,err.what()); }
	det(hd);
    }
}

void TControllerS::gmd_Stop(  )
{
    vector<SCntrS> m_list;
    list( m_list );
    for(unsigned i_m = 0; i_m < m_list.size(); i_m++)
    {
	SHDCntr hd = att(m_list[i_m]);
	if( at(hd).st_run() )
	    try{ at(hd).Stop( ); }
	    catch(TError err) { m_put_s("SYS",MESS_ERR,err.what()); }
	det(hd);
    }
}

void TControllerS::list( vector<SCntrS> &list )
{
    list.clear();
    vector<string> m_list;
    gmd_list(m_list);
    for( unsigned i_m = 0; i_m < m_list.size(); i_m++ )
    {
	unsigned m_hd = gmd_att( m_list[i_m] );
	vector<string> cntr_list;
	gmd_at(m_hd).list(cntr_list);
	for( unsigned i_cntr = 0; i_cntr < cntr_list.size(); i_cntr++ )
	list.push_back( SCntrS(m_list[i_m],cntr_list[i_cntr]) );
	gmd_det( m_hd );
    }
}

void TControllerS::add( SCntrS cntr, SBDS bd )
{
#if OSC_DEBUG
    m_put("DEBUG",MESS_INFO,"Add controller <%s:%s>!",cntr.obj.c_str(),cntr.tp.c_str());
#endif
    unsigned m_hd = gmd_att( cntr.tp );
    try 
    { 
	if( !bd.tp.size() ) bd.tp = m_bd.tp;
	if( !bd.bd.size() ) bd.bd = m_bd.bd;	
	gmd_at(m_hd).add( cntr.obj, bd ); 
    }
    catch( TError err )
    {
	gmd_det( m_hd );
	throw;
    }
    gmd_det( m_hd );
#if OSC_DEBUG
    m_put("DEBUG",MESS_DEBUG,"Add controller <%s:%s> ok!",cntr.obj.c_str(),cntr.tp.c_str());
#endif
}

void TControllerS::del( SCntrS cntr )
{
#if OSC_DEBUG
    m_put("DEBUG",MESS_INFO,"Delete controller <%s:%s>!",cntr.obj.c_str(),cntr.tp.c_str());
#endif
    unsigned m_hd = gmd_att( cntr.tp );
    try{ gmd_at(m_hd).del( cntr.obj ); }
    catch(...)
    {
	gmd_det( m_hd );
	throw;
    }
    gmd_det( m_hd );
#if OSC_DEBUG
    m_put("DEBUG",MESS_DEBUG,"Delete controller <%s:%s> ok!",cntr.obj.c_str(),cntr.tp.c_str());
#endif
}

SHDCntr TControllerS::att( SCntrS cntr, string how )
{
    SHDCntr HDCntr;
    HDCntr.h_tp  = gmd_att( cntr.tp, how );
    try{ HDCntr.h_obj = gmd_at(HDCntr.h_tp).att( cntr.obj, how ); }
    catch(...)
    {
	gmd_det( HDCntr.h_tp );
	throw;
    }

    return(HDCntr);
}

void TControllerS::det( SHDCntr &hd )
{
    gmd_at( hd.h_tp ).det( hd.h_obj );
    gmd_det( hd.h_tp );
}

string TControllerS::opt_descr( )
{
    char buf[STR_BUF_LEN];
    snprintf(buf,sizeof(buf),Mess->I18N(
	"======================== The controller subsystem options =================\n"
	"    --TCModPath = <path>   set moduls <path>;\n"
	"------------ Parameters of section <%s> in config file -----------\n"
    	"mod_path  <path>           set modules <path>;\n"
    	"GenBD     <fullname>       generic bd recorded: \"<TypeBD>:<NameBD>:<NameTable>\";\n"
	),gmd_Name().c_str());

    return(buf);
}

void TControllerS::gmd_CheckCommandLine( )
{
    TGRPModule::gmd_CheckCommandLine( );
    
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{"TCModPath"  ,1,NULL,'m'},
	{NULL         ,0,NULL,0  }
    };

    optind=opterr=0;	
    do
    {
	next_opt=getopt_long(SYS->argc,(char * const *)SYS->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': fprintf(stdout,opt_descr().c_str()); break;
	    case 'm': DirPath  = optarg;    break;
	    case -1 : break;
	}
    } while(next_opt != -1);
}

void TControllerS::gmd_UpdateOpt()
{
    TGRPModule::gmd_UpdateOpt();
    
    string opt;
  
    try{ DirPath = gmd_XMLCfgNode()->get_child("id","mod_path")->get_text(); }
    catch(...) {  }
    
    try
    { 
	string opt = gmd_XMLCfgNode()->get_child("id","GenBD")->get_text(); 
    	int pos = 0;
        m_bd.tp  = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
	m_bd.bd  = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
	m_bd.tbl = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
    }
    catch(...) {  }
    if( !m_bd.tp.size() ) m_bd.tp = Owner().DefBDType;
    if( !m_bd.bd.size() ) m_bd.bd = Owner().DefBDName;	    
}

void TControllerS::LoadBD()
{
    TConfig *g_cfg = new TConfig(this);
    try
    {
	SHDBD b_hd = Owner().BD().open( m_bd );
	g_cfg->cf_LoadAllValBD( Owner().BD().at(b_hd) );
	Owner().BD().close(b_hd);
    //Create controller 
	for(unsigned i_cfg = 0; i_cfg < g_cfg->cf_Size(); i_cfg++)
	    try
	    {
		SCntrS CntrS(g_cfg->cf_Get_S("MODUL", i_cfg), g_cfg->cf_Get_S("NAME", i_cfg));
		add( CntrS, SBDS(g_cfg->cf_Get_S("BDTYPE", i_cfg), g_cfg->cf_Get_S("BDNAME", i_cfg), g_cfg->cf_Get_S("TABLE", i_cfg)) );

		SHDCntr hd = att(CntrS);
		try
		{ 
		    if( at(hd).auto_enable() ) at(hd).Enable(); 
		}
		catch(...){ det(hd); throw; }
		det(hd);
	    }
	    catch(TError err) { m_put_s("SYS",MESS_ERR,err.what()); }
    }catch(TError err) { m_put_s("SYS",MESS_ERR,err.what()); }
    delete g_cfg;
}

void TControllerS::UpdateBD(  )
{
    SHDBD b_hd = Owner().BD().open( m_bd, true );
    TConfig *g_cfg = new TConfig(this);    
    g_cfg->cf_LoadAllValBD( Owner().BD().at(b_hd) );  //Load temp config
    //Clean all BD
    Owner().BD().at(b_hd).Clean();                    //Clean BD
    cfe_UpdateBDAttr( Owner().BD().at(b_hd) );        //Update BD struct
    Owner().BD().at(b_hd).Save();                     //Save BD
    Owner().BD().close(b_hd);
    //Clean controller type BD
    for(unsigned i_cfg = 0; i_cfg < g_cfg->cf_Size(); i_cfg++)
	try
	{
	    SHDBD b_hd = Owner().BD().open( SBDS(g_cfg->cf_Get_S("BDTYPE", i_cfg), g_cfg->cf_Get_S("BDNAME", i_cfg), g_cfg->cf_Get_S("TABLE", i_cfg)) );
	    Owner().BD().at(b_hd).Clean();                    //Clean BD
	    Owner().BD().at(b_hd).Save();                     //Save BD
	    Owner().BD().close(b_hd);
	}
	catch(TError err) { m_put_s("SYS",MESS_ERR,err.what()); }
    delete g_cfg;
    //Save all controllers    
    vector<SCntrS> m_list;
    list( m_list );
    for(unsigned i_m = 0; i_m < m_list.size(); i_m++)
    {
	SHDCntr hd = att(m_list[i_m]);
	try{ at(hd).Save( true ); }
	catch(TError err) { m_put_s("SYS",MESS_ERR,err.what()); }
	det(hd);
    }
}

void TControllerS::gmd_del( string name )
{
    vector<SCntrS> m_list;
    list( m_list );
    for(unsigned i_m = 0; i_m < m_list.size(); i_m++)
	if( m_list[i_m].tp == name ) del( m_list[i_m] );

    TGRPModule::gmd_del( name );
}

//================== Controll functions ========================
void TControllerS::ctr_fill_info( XMLNode *inf )
{
    char *dscr="dscr";
    TGRPModule::ctr_fill_info( inf );
    
    XMLNode *n_add = inf->add_child();
    n_add->load_xml(i_cntr);
    n_add->set_attr(dscr,Mess->I18N("Subsystem control"));
    n_add->get_child(0)->set_attr(dscr,Mess->I18N("BD (module:bd:table)"));
    n_add->get_child(3)->set_attr(dscr,Mess->I18N("Options help"));
    n_add->get_child(4)->set_attr(dscr,Mess->I18N("Load BD"));
    n_add->get_child(5)->set_attr(dscr,Mess->I18N("Update BD"));
}

void TControllerS::ctr_din_get_( string a_path, XMLNode *opt )
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

void TControllerS::ctr_din_set_( string a_path, XMLNode *opt )
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

void TControllerS::ctr_cmd_go_( string a_path, XMLNode *fld, XMLNode *rez )
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

