
#include <getopt.h>

#include "tsys.h"
#include "tkernel.h"
#include "tmessage.h"
#include "tarhives.h"

//================================================================
//=============== TArhiveS =======================================
//================================================================
const char *TArhiveS::o_name = "TArhiveS";

TArhiveS::TArhiveS( TKernel *app ) : TGRPModule(app,"Arhiv") 
{

}

TArhiveS::~TArhiveS(  )
{
    for(unsigned i_arh = 0; i_arh < m_mess.size(); i_arh++)
	if( m_mess[i_arh].use ) MessClose( i_arh );
}

void TArhiveS::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "========================= %s options ================================\n"
    "    --ArhPath = <path>  set modules <path>;\n"
    "------------------ Section fields of config file --------------------\n"
    " mod_path = <path>      set modules <path>;\n"	    
    "\n",gmd_Name().c_str());
}

void TArhiveS::gmd_CheckCommandLine( )
{
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{"ArhPath" ,1,NULL,'m'},
	{NULL      ,0,NULL,0  }
    };

    optind=0,opterr=0;	
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

void TArhiveS::gmd_UpdateOpt()
{
    try{ DirPath = gmd_XMLCfgNode()->get_child("mod_path")->get_text(); }
    catch(...) {  }
}

int TArhiveS::MessOpen( string name, string t_name, string categor )
{    
    try{ MessNameToId( name ); }
    catch(...)
    {
    	SArhive n_obj;
	n_obj.use  = true;
	n_obj.type = gmd_NameToId(t_name);
	n_obj.obj  = at_tp(n_obj.type).OpenMess(name,categor);
	
	unsigned id;
	for( id = 0; id < m_mess.size(); id++ )
	    if( !m_mess[id].use ) break;
	if( id == m_mess.size() ) m_mess.push_back(n_obj);
	else                      m_mess[id] = n_obj;
	
	return(id);
    }
    throw TError("%s: Message arhive %s already open!",o_name,name.c_str());
}

void TArhiveS::MessClose( unsigned int id )
{
    if(id > m_mess.size() || !m_mess[id].use ) 
	throw TError("%s: Message arhive identificator error!",o_name);
    at_tp(m_mess[id].type).CloseMess(m_mess[id].obj);
    m_mess[id].use = false;
}

unsigned TArhiveS::MessNameToId( string name )
{
    for(unsigned i_arh=0; i_arh < m_mess.size(); i_arh++)
	if( m_mess[i_arh].use && Mess_at(i_arh)->Name() == name ) return(i_arh);
    throw TError("%s: Message arhive %s no avoid!",o_name,name.c_str());
}

TArhiveMess *TArhiveS::Mess_at( unsigned int id )
{
    if(id > m_mess.size() || !m_mess[id].use ) 
	throw TError("%s: Message arhive identificator error!",o_name);
    return(at_tp(m_mess[id].type).atMess(m_mess[id].obj));
}

void TArhiveS::MessList( vector<string> &list )
{
    list.clear();
    for(unsigned id=0;id < m_mess.size(); id++)
	if( m_mess[id].use ) list.push_back( Mess_at(id)->Name() );
}

int TArhiveS::ValOpen( string name, string t_name, string bd )
{    
    try{ ValNameToId( name ); }
    catch(...)
    {
    	SArhive n_obj;
	n_obj.use  = true;
	n_obj.type = gmd_NameToId(t_name);
	n_obj.obj  = at_tp(n_obj.type).OpenVal(name,bd);
	
	unsigned id;
	for( id = 0; id < m_val.size(); id++ )
	    if( !m_val[id].use ) break;
	if( id == m_val.size() ) m_val.push_back(n_obj);
	else                     m_val[id] = n_obj;
	
	return(id);
    }
    throw TError("%s: Value arhive %s already open!",o_name,name.c_str());
}

void TArhiveS::ValClose( unsigned int id )
{
    if(id > m_val.size() || !m_val[id].use ) 
	throw TError("%s: Value arhive identificator error!",o_name);
    at_tp(m_val[id].type).CloseVal(m_val[id].obj);
    m_val[id].use = false;
}

unsigned TArhiveS::ValNameToId( string name )
{
    for(unsigned i_arh=0; i_arh < m_val.size(); i_arh++)
	if( m_val[i_arh].use && Val_at(i_arh)->Name() == name ) return(i_arh);
    throw TError("%s: Value arhive %s no avoid!",o_name,name.c_str());
}

TArhiveVal *TArhiveS::Val_at( unsigned int id )
{
    if(id > m_val.size() || !m_val[id].use ) 
	throw TError("%s: Value arhive identificator error!",o_name);
    return(at_tp(m_val[id].type).atVal(m_val[id].obj));
}

void TArhiveS::ValList( vector<string> &list )
{
    list.clear();
    for(unsigned id=0;id < m_val.size(); id++)
	if( m_val[id].use ) list.push_back( Val_at(id)->Name() );
}

//================================================================
//=========== TTipArhive =========================================
//================================================================
const char *TTipArhive::o_name = "TTipArhive";

TTipArhive::TTipArhive()
{
    hd_res = SYS->ResCreate();
}
    
TTipArhive::~TTipArhive()
{
    SYS->ResRequest(hd_res);

    SYS->ResDelete(hd_res);    
}

unsigned TTipArhive::OpenMess(string name, string categories )
{
    unsigned id;
    
    TArhiveMess *a_mess = Mess(name, categories);
    SYS->ResRequest(hd_res);
    for(id=0; id < m_mess.size(); id++) 
	if( m_mess[id] == TO_FREE ) break;
    if( id == m_mess.size() ) m_mess.push_back(a_mess);
    else                      m_mess[id] = a_mess;
    SYS->ResRelease(hd_res);

    return(id);
}

void TTipArhive::CloseMess( unsigned int id )
{
    SYS->ResRequest(hd_res);
    if(id > m_mess.size() || m_mess[id] == TO_FREE )
    {
	SYS->ResRelease(hd_res);
	throw TError("%s: Message arhive id=%d - error!",o_name,id);
    }
    delete m_mess[id];  
    m_mess[id] = TO_FREE;
    SYS->ResRelease(hd_res);
}

TArhiveMess *TTipArhive::atMess( unsigned int id )
{
    if(id > m_mess.size() || m_mess[id] == TO_FREE ) 
	throw TError("%s: Message arhive id=%d - error!",o_name,id);
    return(m_mess[id]);
}

unsigned TTipArhive::OpenVal(string name, string bd )
{
    unsigned id;
    
    TArhiveVal *a_val = Val(name,bd);
    SYS->ResRequest(hd_res);
    for(id=0; id < m_val.size(); id++) 
	if(m_val[id] == TO_FREE) break;
    if(id == m_val.size()) m_val.push_back(a_val);
    else                   m_val[id] = a_val;
    SYS->ResRelease(hd_res);
    
    return(id);
}

void TTipArhive::CloseVal( unsigned int id )
{
    SYS->ResRequest(hd_res);
    if(id > m_val.size() || m_val[id] == TO_FREE ) 
    {
	SYS->ResRelease(hd_res);	
	throw TError("%s: Value arhiv id=%d - error!",o_name,id);
    }
    delete m_val[id];    
    m_val[id] = TO_FREE;
    SYS->ResRelease(hd_res);	
}

TArhiveVal *TTipArhive::atVal( unsigned int id )
{
    if(id > m_val.size() || m_val[id] == TO_FREE ) 
	throw TError("%s: Value arhive id=%d - error!",o_name,id);
    return(m_val[id]);
}

//================================================================
//=========== TArhiveMess ========================================
//================================================================
const char *TArhiveMess::o_name = "TArhiveMess";

TArhiveMess::~TArhiveMess()
{
    
}

//================================================================
//=========== TArhiveVal =========================================
//================================================================
const char *TArhiveVal::o_name = "TArhiveVal";

TArhiveVal::~TArhiveVal()
{
    
}

