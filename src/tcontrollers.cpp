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
    {"NAME"    ,"Controller's name."               ,CFG_T_STRING              ,"","",""           ,"20",""          ,"%s"},
    {"DESCRIPT","Description controler."           ,CFG_T_STRING              ,"","",""           ,"50",""          ,"%s"},
    {"MODUL"   ,"Module(plugin) of type controler.",CFG_T_STRING              ,"","",""           ,"20",""          ,"%s"},
    {"BDTYPE"  ,"Type controller's BD."            ,CFG_T_STRING              ,"","","direct_dbf" ,"20",""          ,"%s"},
    {"BDNAME"  ,"Name controller's BD."            ,CFG_T_STRING              ,"","","./DATA"     ,"50",""          ,"%s"},
    {"TABLE"   ,"Name controller's Table."         ,CFG_T_STRING              ,"","","contr.dbf"  ,"20",""          ,"%s"},
    {"STAT"    ,"Controller's stat."               ,CFG_T_BOOLEAN|CFG_T_SELECT,"","","false"      ,"1" ,"false;true","%s","Disable;Enable"}
};

const char *TControllerS::o_name = "TControllerS";

TControllerS::TControllerS( TKernel *app ) : TGRPModule(app,"Controller"), TConfig(NULL), m_bd("direct_dbf", "./DATA", "generic.dbf")
{
    for(unsigned i = 0; i < sizeof(gen_elem)/sizeof(SCfgFld); i++) 
	cf_ConfElem()->cfe_Add(&gen_elem[i]);    
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

SHDCntr TControllerS::att( SCntrS cntr )
{
    SHDCntr HDCntr;
    HDCntr.h_tp  = gmd_att( cntr.tp );
    try{ HDCntr.h_obj = gmd_at(HDCntr.h_tp).att( cntr.obj ); }
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

void TControllerS::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "========================= TipController options ===========================\n"
    "    --TCModPath=<path>   Set moduls <path>;\n"
    "    --TCTypeGenBD=<name> Set a name of type generic BD (default \"direct_dbf\");\n"
    "    --TCNameGenBD=<name> Set a name of generic BD (default \"./DATA\");\n"
    "    --TCNameGenTB=<name> Set a name of generic table (default \"generic.dbf\");\n"
    "------------------ Section fields of config file --------------------\n"
    " mod_path = <path>   path to modules;\n"
    " GenBD = <fullname>  generic bd recorded: \"<TypeBD>:<NameBD>:<NameTable>\";\n"
    " CONTR id=<name> type=<module> bd=<type:bd:table> and description into text;\n"
    "    name          - name transport;\n"
    "    module        - module transport;\n"
    "    type:bd:table - full bd description: type bd, name bd and name table;\n"
    "\n");
}

void TControllerS::gmd_CheckCommandLine( )
{
    TGRPModule::gmd_CheckCommandLine( );
    
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{"TCModPath"  ,1,NULL,'m'},
	{"TCTypeGenBD",1,NULL,'t'},
	{"TCNameGenBD",1,NULL,'b'},
	{"TCNameGenTB",1,NULL,'l'},	
	{NULL         ,0,NULL,0  }
    };

    optind=opterr=0;	
    do
    {
	next_opt=getopt_long(SYS->argc,(char * const *)SYS->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': pr_opt_descr(stdout); break;
	    case 'm': DirPath  = optarg;    break;
	    case 't': m_bd.tp  = optarg;    break;
	    case 'b': m_bd.bd  = optarg;    break;
	    case 'l': m_bd.tbl = optarg;    break;
	    case -1 : break;
	}
    } while(next_opt != -1);
}

void TControllerS::gmd_UpdateOpt()
{
    TGRPModule::gmd_UpdateOpt();
    
    string opt;
  
    try{ DirPath = gmd_XMLCfgNode()->get_child("mod_path")->get_text(); }
    catch(...) {  }
    
    try
    { 
	string opt = gmd_XMLCfgNode()->get_child("GenBD")->get_text(); 
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
    	int i = 0, pos = 0;
    	while( true )
	{
	    XMLNode *t_n = gmd_XMLCfgNode()->get_child("CONTR",i++);
	    int rec = cf_AddRecord( cf_Size() );
	    cf_Set_S("NAME"    , t_n->get_attr("id")  , rec);
	    cf_Set_S("DESCRIPT", t_n->get_text( )     , rec);
	    cf_Set_S("MODUL"   , t_n->get_attr("type"), rec);
	    
	    string opt = t_n->get_attr("bd");
 	    cf_Set_S("BDTYPE", opt.substr(pos,opt.find(":",pos)-pos), rec); pos = opt.find(":",pos)+1;
 	    cf_Set_S("BDNAME", opt.substr(pos,opt.find(":",pos)-pos), rec); pos = opt.find(":",pos)+1;
 	    cf_Set_S("TABLE" , opt.substr(pos,opt.find(":",pos)-pos), rec); pos = opt.find(":",pos)+1;
	    cf_Set_SEL("STAT", "Enable", rec);
	}	
    }
    catch(...) {  }    
}

void TControllerS::LoadBD()
{
    string cell;
    //bool   reload = false;
    //---- NEW ----
    try
    {
	SHDBD b_hd = Owner().BD().open( m_bd );
	cf_LoadAllValBD( Owner().BD().at(b_hd) );
	cf_FreeDubl("NAME",false);
	Owner().BD().close(b_hd);
    }catch(TError err) { m_put_s("SYS",MESS_ERR,err.what()); }
    //Create controller 
    for(unsigned i_cfg = 0; i_cfg < cf_Size(); i_cfg++)
	if( cf_Get_SEL("STAT", i_cfg) == "Enable" )
	{
    	    try
	    {
		SCntrS CntrS(cf_Get_S("MODUL", i_cfg), cf_Get_S("NAME", i_cfg));
		add( CntrS, SBDS(cf_Get_S("BDTYPE", i_cfg), cf_Get_S("BDNAME", i_cfg), cf_Get_S("TABLE", i_cfg)) );

		SHDCntr hd = att(CntrS);
		try{ at(hd).Enable(); }
		catch(...){ det(hd); throw; }
		det(hd);
	    }
	    catch(TError err) { m_put_s("SYS",MESS_ERR,err.what()); }
	}									    
}

void TControllerS::UpdateBD(  )
{
    SHDBD b_hd;
    string cell, stat;

    try { b_hd = Owner().BD().open( m_bd ); }
    catch(...) { b_hd = Owner().BD().open( m_bd, true ); }
    cf_ConfElem()->cfe_UpdateBDAttr( Owner().BD().at(b_hd) );
    cf_SaveAllValBD( Owner().BD().at(b_hd) );
    Owner().BD().at(b_hd).Save();
    Owner().BD().close(b_hd);
    //Controllers update
    vector<SCntrS> m_list;
    list( m_list );
    for(unsigned i_m = 0; i_m < m_list.size(); i_m++)
    {
	SHDCntr hd = att(m_list[i_m]);
	try{ at(hd).Save( ); }
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


