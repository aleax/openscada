#include <getopt.h>

#include "tsys.h"
#include "tkernel.h"
#include "tmessage.h"
#include "tbds.h"
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

TControllerS::TControllerS( TKernel *app ) : TGRPModule(app,"Controller"), TConfig(NULL), 
	t_bd("direct_dbf"), n_bd("./DATA"), n_tb("generic.dbf")
{
    for(unsigned i = 0; i < sizeof(gen_elem)/sizeof(SCfgFld); i++) 
	cf_ConfElem()->cfe_Add(&gen_elem[i]);    
}

TControllerS::~TControllerS(  )
{
    gmd_Stop();
    for(unsigned i_ctr = 0; i_ctr < Contr.size(); i_ctr++)
        if( Contr[i_ctr].use ) DelContr( i_ctr );	
}

void TControllerS::gmd_Init( )
{
    LoadBD();
}

void TControllerS::gmd_Start(  )         
{
    for(unsigned i=0; i< Contr.size(); i++)
	if( Contr[i].use ) 
	{
	    try{ at_tp(Contr[i].id_mod).at(Contr[i].id_contr).Start( ); }
	    catch(TError err) {  Mess->put("SYS",MESS_ERR,"%s:%s",o_name,err.what().c_str()); }
	}
}

void TControllerS::gmd_Stop(  )
{
//    LoadBD();
    for(unsigned i=0; i< Contr.size(); i++)
	if( Contr[i].use )
	{
	    try{ at_tp(Contr[i].id_mod).at(Contr[i].id_contr).Stop( ); }
	    catch(TError err) {  Mess->put("SYS",MESS_ERR,"%s:%s",o_name,err.what().c_str()); }
	}
}

void TControllerS::ContrList( vector<string> & List )
{
    List.clear();
    for(unsigned i=0;i < Contr.size(); i++)
	if( Contr[i].use ) 
	    List.push_back(at_tp(Contr[i].id_mod).at(Contr[i].id_contr).Name());
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
	    case 't': t_bd     = optarg;    break;
	    case 'b': n_bd     = optarg;    break;
	    case 'l': n_tb     = optarg;    break;
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
	t_bd = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
	n_bd = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
	n_tb = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
	if( !t_bd.size() ) t_bd = Owner().DefBDType;
	if( !n_bd.size() ) n_bd = Owner().DefBDName;	
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
    	int b_hd = Owner().BD().OpenTable(t_bd,n_bd,n_tb);
	cf_LoadAllValBD( Owner().BD().at_tbl(b_hd) );
	cf_FreeDubl("NAME",false);
	Owner().BD().CloseTable(b_hd);
    }catch(TError err) { Mess->put("SYS",MESS_ERR,"%s:%s",o_name,err.what().c_str()); }
    //Create controller 
    for(unsigned i_cfg = 0; i_cfg < cf_Size(); i_cfg++)
	if( cf_Get_SEL("STAT", i_cfg) == "Enable" )
	{
	    //Check avoid controller
	    try{ NameCntrToId( cf_Get_S("NAME", i_cfg) ); }
	    catch(...)
    	    {
		try
		{
		    int hd = AddContr(  cf_Get_S("NAME", i_cfg), 
					cf_Get_S("MODUL", i_cfg), 
					cf_Get_S("BDTYPE", i_cfg), 
					cf_Get_S("BDNAME", i_cfg), 
					cf_Get_S("TABLE", i_cfg) );
		    at(hd).Enable();
		}
		catch(TError err) { Mess->put("SYS",MESS_ERR,"%s:%s",o_name,err.what().c_str()); }
	    }
	}									    
}


void TControllerS::UpdateBD(  )
{
    int b_hd;
    string cell, stat;
    try { b_hd = Owner().BD().OpenTable(t_bd,n_bd,n_tb); }
    catch(...) { b_hd = Owner().BD().OpenTable(t_bd,n_bd,n_tb,true); }
    cf_ConfElem()->cfe_UpdateBDAttr( Owner().BD().at_tbl(b_hd) );
    cf_SaveAllValBD( Owner().BD().at_tbl(b_hd) );
    Owner().BD().at_tbl(b_hd).Save();
    Owner().BD().CloseTable(b_hd);
    for(unsigned i=0;i < Contr.size(); i++) if( Contr[i].use ) at(i).Save();    
}

unsigned TControllerS::AddContr( string name, string tip, string t_bd, string n_bd, string n_tb )
{
    unsigned i;
    //!!! Want request resource 
    try{ NameCntrToId( name ); }
    catch(...)
    {
#if OSC_DEBUG
	Mess->put("DEBUG",MESS_INFO,"%s: Add controller <%s>!",o_name,name.c_str());
#endif
	SContr n_cntr;
	n_cntr.use      = true;
	n_cntr.id_mod   = gmd_NameToId(tip);
	
	if( !t_bd.size() ) t_bd = TControllerS::t_bd;
	if( !n_bd.size() ) n_bd = TControllerS::n_bd;	
	n_cntr.id_contr = at_tp(n_cntr.id_mod).Add(name,t_bd,n_bd,n_tb);
	
    	for(i=0;i < Contr.size(); i++)
	    if( !Contr[i].use ) break;
	if(i == Contr.size() ) Contr.push_back(n_cntr);
	else                   Contr[i] = n_cntr;
	
#if OSC_DEBUG
	Mess->put("DEBUG",MESS_DEBUG,"%s: Add controller <%s> ok!",o_name,name.c_str());
#endif

    	return(i);
    }
    throw TError("%s: Controller %s already avoid!",o_name,name.c_str());
}

void TControllerS::DelContr( unsigned id )
{
    //!!! Want request resource 
    if(id >= Contr.size() || !Contr[id].use ) 
	throw TError("%s: Controller %d error!",o_name,id);
	
#if OSC_DEBUG
    Mess->put("DEBUG",MESS_INFO,"%s: Delete controller <%s>!",o_name,at(id).Name().c_str());
#endif

    at_tp(Contr[id].id_mod).Del(Contr[id].id_contr);
    Contr[id].use = false;
    
#if OSC_DEBUG
    Mess->put("DEBUG",MESS_DEBUG,"%s: Delete controller ok!",o_name);
#endif

}

TController &TControllerS::at( unsigned id)
{ 
    if(id >= Contr.size() || !Contr[id].use ) 
	throw TError("%s: Controller %d error!",o_name,id);
    return(at_tp(Contr[id].id_mod).at(Contr[id].id_contr));
}

unsigned TControllerS::NameCntrToId( string Name )
{
    for(unsigned i_id = 0; i_id < Contr.size(); i_id++)
        if( Contr[i_id].use && at_tp(Contr[i_id].id_mod).at(Contr[i_id].id_contr).Name() == Name ) 
	    return(i_id);
    throw TError("%s: %s controller no avoid!",o_name,Name.c_str());
}

void TControllerS::gmd_DelM( unsigned hd )
{    
    for(unsigned i_c = 0; i_c < Contr.size(); i_c++)
    	if( Contr[i_c].use && Contr[i_c].id_mod == hd )
	    DelContr(i_c);

    TGRPModule::gmd_DelM( hd );
}


