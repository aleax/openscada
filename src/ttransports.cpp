#include <getopt.h>

#include "tsys.h"
#include "tkernel.h"
#include "tmessage.h"
#include "tmodule.h"
#include "tbds.h"
#include "ttransports.h"

//================================================================
//=========== TTransportS ========================================
//================================================================

SCfgFld TTransportS::gen_elem[] =
{
    {"NAME"  ,"Transport name."               ,CFG_T_STRING              ,"","",""           ,"20",""          ,"%s"},
    {"MODULE","Type transport (module name)." ,CFG_T_STRING              ,"","",""           ,"20",""          ,"%s"},
    {"ADDR"  ,"Transport address."            ,CFG_T_STRING              ,"","",""           ,"50",""          ,"%s"},
    {"PROT"  ,"Assign transport protocol."    ,CFG_T_STRING              ,"","",""           ,"20",""          ,"%s"},
    {"TYPE"  ,"Transport type (Input;Output).",CFG_T_BOOLEAN|CFG_T_SELECT,"","","false"      ,"1" ,"false;true","%s","Input;Output"},
    {"STAT"  ,"Transport's stat."             ,CFG_T_BOOLEAN|CFG_T_SELECT,"","","false"      ,"1" ,"false;true","%s","Disable;Enable"}
};

const char *TTransportS::o_name = "TTransportS";

TTransportS::TTransportS( TKernel *app ) : TGRPModule(app,"Transport"), 
	TConfig(NULL), t_bd("direct_dbf"), n_bd("./DATA"), n_tb("transport.dbf")
{
    TConfigElem *gen_ecfg = new TConfigElem;
    for(unsigned i = 0; i < sizeof(gen_elem)/sizeof(SCfgFld); i++) gen_ecfg->cfe_Add(&gen_elem[i]);
    cf_ConfElem(gen_ecfg);
}

TTransportS::~TTransportS(  )
{
    for(unsigned i_tr = 0; i_tr < TranspIn.size(); i_tr++)
	if( TranspIn[i_tr].use ) CloseIn( i_tr );
    for(unsigned o_tr = 0; o_tr < TranspOut.size(); o_tr++)
	if( TranspOut[o_tr].use ) CloseOut( o_tr );
    TConfigElem *gen_ecfg = cf_ConfElem();
    cf_ConfElem(NULL);
    delete gen_ecfg;
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
    "                        add input transport <name> <module> <addr> <prmod>;\n"
    " OUT id=<name> type=<module> addr=<addr> prot=<prmod>\n"
    "                        add output transport <name> <module> <addr> <prmod>;\n"
    "    name   - name transport;\n"
    "    module - module transport;\n"
    "    addr   - addres transport;\n"
    "    prmod  - name assign to transport protocol;\n"
    "\n",gmd_Name().c_str());
}

void TTransportS::gmd_CheckCommandLine( )
{
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
    string opt;

    try{ DirPath = gmd_XMLCfgNode()->get_child("mod_path")->get_text(); }
    catch(...) {  }
    
    try
    {
    	opt = gmd_XMLCfgNode()->get_child("GenBD")->get_text(); 
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
    	int i = 0;
    	while( true )
	{
	    XMLNode *t_n = gmd_XMLCfgNode()->get_child("IN",i++);
	    int rec = cf_AddRecord( cf_Size() );
	    cf_Set_S("NAME"  , t_n->get_attr("id")  , rec);
	    cf_Set_S("MODULE", t_n->get_attr("type"), rec);
	    cf_Set_S("ADDR"  , t_n->get_attr("addr"), rec);
	    cf_Set_SEL("TYPE", "Input", rec);
	    cf_Set_SEL("STAT", "Enable", rec);
    	    cf_Set_S("PROT"  , t_n->get_attr("prot"), rec);	   
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
	int b_hd = Owner().BD().OpenTable(t_bd,n_bd,n_tb);
	cf_LoadAllValBD( Owner().BD().at_tbl(b_hd) );
	cf_FreeDubl("NAME",false);   //Del new (from bd)
	Owner().BD().CloseTable(b_hd);
    }catch(TError err) { Mess->put("SYS",MESS_ERR,"%s:%s",o_name,err.what().c_str()); }
    
    //Open transports (open new transports)
    for(unsigned i_cfg = 0; i_cfg < cf_Size(); i_cfg++)
    {
	if( cf_Get_SEL("TYPE", i_cfg) == "Input" && cf_Get_SEL("STAT", i_cfg) == "Enable" )
	{
	    //Check avoid transport
	    try{ NameInToId( cf_Get_S("NAME", i_cfg) ); }
	    catch(...)
	    {
		try 
		{ 
		    OpenIn( cf_Get_S("NAME", i_cfg), cf_Get_S("MODULE", i_cfg), cf_Get_S("ADDR", i_cfg), cf_Get_S("PROT", i_cfg) ); 
		}
		catch(TError err) { Mess->put("SYS",MESS_ERR,"%s:%s",o_name,err.what().c_str()); }
	    }
	}
	else if( cf_Get_SEL("TYPE", i_cfg) == "Output" && cf_Get_SEL("STAT", i_cfg) == "Enable" )
	{
	    //Check avoid transport
	    try{ NameOutToId( cf_Get_S("NAME", i_cfg) ); }
	    catch(...)
	    {
    		try 
    		{ 
		    OpenOut( cf_Get_S("NAME", i_cfg), cf_Get_S("MODULE", i_cfg), cf_Get_S("ADDR", i_cfg) ); 
    		}
    		catch(TError err) { Mess->put("SYS",MESS_ERR,"%s:%s",o_name,err.what().c_str()); }
	    }
	}
    }
    //Close no avoid in bd transports    
}

void TTransportS::UpdateBD( )
{
    int b_hd;
    try{ b_hd = Owner().BD().OpenTable(t_bd,n_bd,n_tb); }
    catch(...) { b_hd = Owner().BD().OpenTable(t_bd,n_bd,n_tb,true); }
    cf_ConfElem()->cfe_UpdateBDAttr( Owner().BD().at_tbl(b_hd) );
    cf_SaveAllValBD( Owner().BD().at_tbl(b_hd) );
    Owner().BD().at_tbl(b_hd).Save();
    Owner().BD().CloseTable(b_hd);
}

int TTransportS::OpenIn( string name, string t_name, string address, string proto )
{    
    try{ NameInToId( name ); }
    catch(...)
    {
    	STransp n_tr;
	n_tr.use     = true;
	n_tr.type_tr = gmd_NameToId(t_name);
	n_tr.tr      = at_tp(n_tr.type_tr).OpenIn(name,address,proto);
	
	unsigned id;
	for( id = 0; id < TranspIn.size(); id++ )
	    if( !TranspIn[id].use ) break;
	if( id == TranspIn.size() ) TranspIn.push_back(n_tr);
	else                        TranspIn[id] = n_tr;
	
	return(id);
    }
    throw TError("%s: Input transport %s already open!",o_name,name.c_str());
}

void TTransportS::CloseIn( unsigned int id )
{
    if(id > TranspIn.size() || !TranspIn[id].use ) 
	throw TError("%s: transport identificator error!",o_name);
    at_tp(TranspIn[id].type_tr).CloseIn(TranspIn[id].tr);
    TranspIn[id].use = false;
}

unsigned TTransportS::NameInToId( string name )
{
    for(unsigned i_in=0; i_in < TranspIn.size(); i_in++)
	if( TranspIn[i_in].use && at_in(i_in)->Name() == name ) return(i_in);
    throw TError("%s: transport %s no avoid!",o_name,name.c_str());
}

TTransportIn *TTransportS::at_in( unsigned int id )
{
    if(id > TranspIn.size() || !TranspIn[id].use ) 
	throw TError("%s: Input transport identificator error!",o_name);
    return(at_tp(TranspIn[id].type_tr).atIn(TranspIn[id].tr));
}

void TTransportS::ListIn( vector<string> &list )
{
    list.clear();
    for(unsigned id=0;id < TranspIn.size(); id++)
	if( TranspIn[id].use ) list.push_back( at_in(id)->Name() );
}

int TTransportS::OpenOut( string name, string t_name, string address )
{
    try{ NameOutToId( name ); }
    catch(...)
    {
    	STransp n_tr;
	n_tr.use     = true;
	n_tr.type_tr = gmd_NameToId(t_name);
	n_tr.tr      = at_tp(n_tr.type_tr).OpenOut(name,address);
	
    	unsigned id;
	for( id = 0; id < TranspOut.size(); id++ )
	    if( !TranspOut[id].use ) break;
	if( id == TranspOut.size() ) TranspOut.push_back(n_tr);
	else                         TranspOut[id] = n_tr;
	
	return(id);
    }
    throw TError("%s: Output transport %s already open!",o_name,name.c_str());
}

void TTransportS::CloseOut( unsigned int id )
{
    if(id > TranspOut.size() || !TranspOut[id].use ) 
	throw TError("%s: Output transport identificator error!",o_name);
    at_tp(TranspOut[id].type_tr).CloseOut(TranspOut[id].tr);
    TranspOut[id].use = false;
}

unsigned TTransportS::NameOutToId( string name )
{
    for(unsigned i_out = 0; i_out < TranspOut.size(); i_out++)
	if(TranspOut[i_out].use && at_out(i_out)->Name() == name )
	    return(i_out);
    throw TError("%s: Output transport %s no avoid!",o_name,name.c_str());
}

TTransportOut *TTransportS::at_out( unsigned int id )
{
    if( id > TranspOut.size() || !TranspOut[id].use ) 
	throw TError("%s: Output transport identificator error!",o_name);
    return(at_tp(TranspOut[id].type_tr).atOut(TranspOut[id].tr));
}

void TTransportS::ListOut( vector<string> &list )
{
    list.clear();
    for(unsigned id=0;id < TranspOut.size(); id++)
	if( TranspOut[id].use ) list.push_back( at_out(id)->Name() );
}
//================================================================
//=========== TTipTransport ======================================
//================================================================
const char *TTipTransport::o_name = "TTipTransport";

TTipTransport::TTipTransport()
{
    hd_res = SYS->ResCreate();
}
    
TTipTransport::~TTipTransport()
{
    SYS->ResRequest(hd_res);
    for(unsigned id=0; id < i_tr.size(); id++)
	if( i_tr[id] ) delete i_tr[id];
    for(unsigned id=0; id < o_tr.size(); id++)
	if( o_tr[id] ) delete o_tr[id];
    SYS->ResDelete(hd_res);    
}

TTransportIn *TTipTransport::atIn( unsigned int id )
{
    if(id > i_tr.size() || i_tr[id] == TO_FREE ) 
	throw TError("%s: input transport id = %d - error!",o_name,id);
    return(i_tr[id]);
}


TTransportOut *TTipTransport::atOut( unsigned int id )
{
    if(id > o_tr.size() || o_tr[id] == TO_FREE ) 
	throw TError("%s: output transport id = %d - error!",o_name,id);
    return(o_tr[id]);
}

unsigned TTipTransport::OpenIn(string name, string address, string prot )
{
    unsigned id;
    
    TTransportIn *tr_in = In(name,address,prot);
    SYS->ResRequest(hd_res);
    for(id=0; id < i_tr.size(); id++) 
	if( i_tr[id] == TO_FREE ) break;
    if( id == i_tr.size() ) i_tr.push_back(tr_in);
    else                    i_tr[id] = tr_in;
    SYS->ResRelease(hd_res);

    return(id);
}

void TTipTransport::CloseIn( unsigned int id )
{
    SYS->ResRequest(hd_res);
    if(id > i_tr.size() || i_tr[id] == TO_FREE )
    {
	SYS->ResRelease(hd_res);
	throw TError("%s: input transport id = %d - error!",o_name,id);
    }
    delete i_tr[id];  
    i_tr[id] = TO_FREE;
    SYS->ResRelease(hd_res);
}

unsigned TTipTransport::OpenOut(string name, string address )
{
    unsigned id;
    
    TTransportOut *tr_out = Out(name,address);
    SYS->ResRequest(hd_res);
    for(id=0; id < o_tr.size(); id++) 
	if(o_tr[id] == TO_FREE) break;
    if(id == o_tr.size()) o_tr.push_back(tr_out);
    else                  o_tr[id] = tr_out;
    SYS->ResRelease(hd_res);
    
    return(id);
}

void TTipTransport::CloseOut( unsigned int id )
{
    SYS->ResRequest(hd_res);
    if(id > o_tr.size() || o_tr[id] == TO_FREE ) 
    {
	SYS->ResRelease(hd_res);	
	throw TError("%s: output transport id = %d - error!",o_name,id);
    }
    delete o_tr[id];    
    o_tr[id] = TO_FREE;
    SYS->ResRelease(hd_res);	
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

