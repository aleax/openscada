
#include <unistd.h>
#include <getopt.h>
#include <signal.h>

#include "tsys.h"
#include "tbds.h"
#include "tkernel.h"
#include "tarhives.h"

//================================================================
//=============== TArhiveS =======================================
//================================================================
SCfgFld TArhiveS::gen_elem[] =
{
    {"NAME"    ,"Arhive name."         ,CFG_T_STRING              ,"","",""           ,"20",""          ,"%s"},
    {"DESCRIPT","Arhive description."  ,CFG_T_STRING              ,"","",""           ,"50",""          ,"%s"},
    {"MODUL"   ,"Module(plugin) name." ,CFG_T_STRING              ,"","",""           ,"20",""          ,"%s"},
    {"CATEG"   ,"Category for MESS."   ,CFG_T_STRING              ,"","",""           ,"20",""          ,"%s"},
    {"ADDR"    ,"Arhive address."      ,CFG_T_STRING              ,"","",""           ,"20",""          ,"%s"},
    {"TYPE"    ,"Type arhive."         ,CFG_T_INT|CFG_T_SELECT    ,"","","0"          ,"1" ,"0;1"       ,"%s","Message;Value"},
    {"STAT"    ,"Arhive stat."         ,CFG_T_BOOLEAN|CFG_T_SELECT,"","","false"      ,"1" ,"false;true","%s","Disable;Enable"}
};

const char *TArhiveS::o_name = "TArhiveS";

TArhiveS::TArhiveS( TKernel *app ) : TGRPModule(app,"Arhiv"), TConfig(NULL), m_mess_r_stat(false), m_mess_per(2)
{
    for(unsigned i = 0; i < sizeof(gen_elem)/sizeof(SCfgFld); i++) 
	cf_ConfElem()->cfe_Add(&gen_elem[i]);
}

TArhiveS::~TArhiveS(  )
{
    if( m_mess_r_stat )
    {
	m_mess_r_endrun = true;
	sleep(1);
	while( m_mess_r_stat )
	{
	    Mess->put("SYS",MESS_CRIT,"%s: Thread still no stoped!",o_name);
	    sleep(1);
    	}
    }
    
    vector<SArhS> list;
    mess_list( list );
    for(unsigned i_m = 0; i_m < list.size(); i_m++)
	mess_del( list[i_m] );
	
    val_list( list );
    for(unsigned i_m = 0; i_m < list.size(); i_m++)
	val_del( list[i_m] );	
}

void TArhiveS::gmd_Init( )
{
    LoadBD();
}

void TArhiveS::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "========================= %s options ================================\n"
    "    --ArhPath = <path>  set modules <path>;\n"
    "------------------ Section fields of config file --------------------\n"
    " mod_path = <path>      set modules <path>;\n"	   
    " GenBD    = <fullname>  generic bd recorded: \"<TypeBD>:<NameBD>:<NameTable>\";\n"
    " mess_period = <per>    set message arhiving period;\n"
    " MESS id=<name> type=<module> addr=<addr> cat=<categ>\n"
    "                        add message arhivator <name> <module> <addr> <categ> and description into text;\n"
    "    name   - name transport;\n"
    "    module - module transport;\n"
    "    addr   - addres transport;\n"
    "    categ  - message categories (TEST;DEBUG);\n"
    "\n",gmd_Name().c_str());
}

void TArhiveS::gmd_CheckCommandLine( )
{
    TGRPModule::gmd_CheckCommandLine( );

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
    TGRPModule::gmd_UpdateOpt();
    
    try{ DirPath = gmd_XMLCfgNode()->get_child("mod_path")->get_text(); }
    catch(...) {  }
    try{ m_mess_per = atoi( gmd_XMLCfgNode()->get_child("mess_period")->get_text().c_str() ); }
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
    	int i = 0;
    	while( true )
	{
	    XMLNode *t_n = gmd_XMLCfgNode()->get_child("MESS",i++);
	    int rec = cf_AddRecord( cf_Size() );
	    cf_Set_S("NAME"    , t_n->get_attr("id")  , rec);
	    cf_Set_S("DESCRIPT", t_n->get_text()      , rec);
	    cf_Set_S("MODUL"   , t_n->get_attr("type"), rec);
	    cf_Set_S("ADDR"    , t_n->get_attr("addr"), rec);
	    cf_Set_S("CATEG"   , t_n->get_attr("cat") , rec);
	    cf_Set_SEL("TYPE"  , "Message", rec);
	    cf_Set_SEL("STAT"  , "Enable", rec);
	}	
    }
    catch(...) {  }    
}

void TArhiveS::LoadBD( )
{    
    try
    {
	SHDBD b_hd = Owner().BD().open( SBDS(t_bd,n_bd,n_tb) );
	cf_LoadAllValBD( Owner().BD().at(b_hd) );
	cf_FreeDubl("NAME",false);   //Del new (from bd)
	Owner().BD().close(b_hd);
    }catch(TError err) { Mess->put("SYS",MESS_ERR,"%s:%s",o_name,err.what().c_str()); }    
    
    //Open transports (open new transports)
    for(unsigned i_cfg = 0; i_cfg < cf_Size(); i_cfg++)
    {
	if( cf_Get_SEL("TYPE", i_cfg) == "Message" && cf_Get_SEL("STAT", i_cfg) == "Enable" )
	{
	    try
	    { 
		mess_add( SArhS( cf_Get_S("MODUL", i_cfg), cf_Get_S("NAME", i_cfg) ), 
		    cf_Get_S("ADDR", i_cfg), cf_Get_S("CATEG", i_cfg) ); 
	    }
	    catch(TError err) { Mess->put("SYS",MESS_ERR,"%s:%s",o_name,err.what().c_str()); }
	}
	else if( cf_Get_SEL("TYPE", i_cfg) == "Value" && cf_Get_SEL("STAT", i_cfg) == "Enable" )
	{
	    try
	    { 
		val_add( SArhS(cf_Get_S("MODUL", i_cfg), cf_Get_S("NAME", i_cfg)), cf_Get_S("ADDR", i_cfg) ); 
	    }
	    catch(TError err) { Mess->put("SYS",MESS_ERR,"%s:%s",o_name,err.what().c_str()); }
	}
    }
    //Close no avoid in bd transports    
}

void TArhiveS::UpdateBD( )
{
    SHDBD b_hd;
    
    try{ b_hd = Owner().BD().open( SBDS(t_bd,n_bd,n_tb) ); }
    catch(...) { b_hd = Owner().BD().open( SBDS(t_bd,n_bd,n_tb), true ); }
    cf_ConfElem()->cfe_UpdateBDAttr( Owner().BD().at(b_hd) );
    cf_SaveAllValBD( Owner().BD().at(b_hd) );
    Owner().BD().at(b_hd).Save();
    Owner().BD().close(b_hd);
}

void TArhiveS::mess_list( vector<SArhS> &list )
{    
    list.clear();
    vector<string> m_list;
    gmd_list(m_list);
    for( unsigned i_l = 0; i_l < m_list.size(); i_l++ )
    {
	unsigned m_hd = gmd_att( m_list[i_l] );
    	vector<string> a_list;
	gmd_at(m_hd).mess_list(a_list);
	for( unsigned i_a = 0; i_a < a_list.size(); i_a++ )
	    list.push_back( SArhS(m_list[i_l], a_list[i_a]) );
	gmd_det( m_hd );
    }	
}

void TArhiveS::mess_add( SArhS arh, string addr ,string categor )
{
    unsigned m_hd = gmd_att( arh.tp );
    try{ gmd_at(m_hd).mess_add( arh.obj, addr, categor ); }
    catch(...)
    { 
	gmd_det( m_hd );
	throw;
    }
    gmd_det( m_hd );
}

void TArhiveS::mess_del( SArhS arh )
{
    unsigned m_hd = gmd_att( arh.tp );
    try{ gmd_at(m_hd).mess_del( arh.obj ); }
    catch(...)
    { 
	gmd_det( m_hd );
	throw;
    }
    gmd_det( m_hd );
}

SHDArh TArhiveS::mess_att( SArhS arh )
{   
    SHDArh HDArh;
    HDArh.h_tp  = gmd_att( arh.tp );
    try{ HDArh.h_obj = gmd_at(HDArh.h_tp).mess_att( arh.obj ); }
    catch(...)
    {
	gmd_det( HDArh.h_tp );
	throw;
    }
    
    return(HDArh);
}

void TArhiveS::mess_det( SHDArh &hd )
{
    gmd_at( hd.h_tp ).mess_det( hd.h_obj );
    gmd_det( hd.h_tp );
}

void TArhiveS::val_list( vector<SArhS> &list )
{
    list.clear();
    vector<string> m_list;
    gmd_list(m_list);
    for( unsigned i_l = 0; i_l < m_list.size(); i_l++ )
    {
	unsigned m_hd = gmd_att( m_list[i_l] );
    	vector<string> a_list;
	gmd_at(m_hd).val_list(a_list);
	for( unsigned i_a = 0; i_a < a_list.size(); i_a++ )
	    list.push_back( SArhS( m_list[i_l], a_list[i_a] ) );
	gmd_det( m_hd );
    }	
}

void TArhiveS::val_add( SArhS arh, string bd )
{
    unsigned m_hd = gmd_att( arh.tp );
    try{ gmd_at(m_hd).val_add( arh.obj, bd ); }
    catch(...)
    { 
	gmd_det( m_hd );
	throw;
    }
    gmd_det( m_hd );
}

void TArhiveS::val_del( SArhS arh )
{
    unsigned m_hd = gmd_att( arh.tp );
    try{ gmd_at(m_hd).val_del( arh.obj ); }
    catch(...)
    { 
	gmd_det( m_hd );
	throw;
    }
    gmd_det( m_hd );
}

SHDArh TArhiveS::val_att( SArhS arh )
{   
    SHDArh HDArh;
    HDArh.h_tp  = gmd_att( arh.tp );
    try{ HDArh.h_obj = gmd_at(HDArh.h_tp).val_att( arh.obj ); }
    catch(...)
    {
	gmd_det( HDArh.h_tp );
	throw;
    }
    
    return(HDArh);
}

void TArhiveS::val_det( SHDArh &hd )
{
    gmd_at( hd.h_tp ).val_det( hd.h_obj );
    gmd_det( hd.h_tp );
}

void TArhiveS::gmd_Start( )
{    
    pthread_attr_t      pthr_attr;
    if( m_mess_r_stat ) return; 

    pthread_attr_init(&pthr_attr);
    pthread_attr_setschedpolicy(&pthr_attr,SCHED_OTHER);
    pthread_create(&m_mess_pthr,&pthr_attr,TArhiveS::MessArhTask,this);
    pthread_attr_destroy(&pthr_attr);
    sleep(1);
    if( !m_mess_r_stat ) throw TError("%s: Task of Messages arhivator no starting!",o_name);    
}

void TArhiveS::gmd_Stop( )
{    
    if( m_mess_r_stat )
    {
    	m_mess_r_endrun = true;
	sleep(1);
	if( m_mess_r_stat ) throw TError("%s: Task of Messages arhivator no stoping!",o_name);
    }
}

void *TArhiveS::MessArhTask(void *param)
{
    int i_cnt = 0;
    TArhiveS *arh = (TArhiveS *)param;
    vector<SBufRec> i_mess, o_mess;    
    time_t t_last = 0, t_cur;

#if OSC_DEBUG
    Mess->put("DEBUG",MESS_DEBUG,"%s: Thread <%d>!",arh->o_name,getpid() );
#endif	

    arh->m_mess_r_stat = true;
    arh->m_mess_r_endrun = false;
    
    while( !arh->m_mess_r_endrun )
    {	
	if( ++i_cnt > arh->m_mess_per)
	{
	    i_cnt = 0;
    	    try
    	    {
    		t_cur = time(NULL);
    		Mess->get( t_last, t_cur, i_mess );
		if( i_mess.size() )
		{
		    t_last = i_mess[i_mess.size()-1].time+1;    		
		    
                    vector<SArhS> m_list;
		    arh->mess_list(m_list);		    
		    for( unsigned i_am = 0; i_am < m_list.size(); i_am++ )
		    {
		        SHDArh m_hd = arh->mess_att(m_list[i_am]);
			o_mess.clear();
			for(unsigned i_m = 0; i_m < i_mess.size(); i_m++)
			{
			    unsigned i_cat;
			    vector<string> &categ = arh->mess_at(m_hd).Categ();
			    for( i_cat = 0; i_cat < categ.size(); i_cat++ )
				if( categ[i_cat] == i_mess[i_m].categ ) break;
			    if( i_cat < categ.size() || !categ.size() ) 
				o_mess.push_back(i_mess[i_m]);
			}
			if( o_mess.size() ) arh->mess_at(m_hd).put(o_mess);
			arh->mess_det(m_hd);
		    }
		}
    	    }
    	    catch(TError err){ Mess->put("SYS",MESS_ERR,"%s:%s",o_name,err.what().c_str()); }
	}	
	sleep(1);
    }
    
    arh->m_mess_r_stat = false;
}

void TArhiveS::gmd_del( string name )
{
    vector<SArhS> list;
    mess_list( list );
    for(unsigned i_m = 0; i_m < list.size(); i_m++)
	if( list[i_m].tp == name ) mess_del( list[i_m] );
	
    val_list( list );
    for(unsigned i_m = 0; i_m < list.size(); i_m++)
	if( list[i_m].tp == name ) val_del( list[i_m] );
	    
    TGRPModule::gmd_del( name );
}

//================================================================
//=========== TTipArhive =========================================
//================================================================
const char *TTipArhive::o_name = "TTipArhive";

TTipArhive::TTipArhive()
{

}
    
TTipArhive::~TTipArhive()
{
    vector<string> list;

    m_hd_mess.lock();
    mess_list(list);
    for( unsigned i_ls = 0; i_ls < list.size(); i_ls++)
        mess_del(list[i_ls]);    
	
    m_hd_val.lock();
    val_list(list);
    for( unsigned i_ls = 0; i_ls < list.size(); i_ls++)
	val_del(list[i_ls]);    
}

void TTipArhive::mess_add(string name, string addr, string categories )
{
    TArhiveMess *mess = AMess(name, addr, categories);
    try{ m_hd_mess.hd_obj_add( mess, &mess->Name() ); }
    catch(TError err) {	delete mess; }
}

void TTipArhive::val_add(string name, string bd )
{
    TArhiveVal *val = AVal(name, bd);
    try{ m_hd_val.hd_obj_add( val, &val->Name() ); }
    catch(TError err) {	delete val; }    
}

//================================================================
//=========== TArhiveMess ========================================
//================================================================
const char *TArhiveMess::o_name = "TArhiveMess";

TArhiveMess::TArhiveMess(string name, string addr, string categoris, TTipArhive *owner) : 
	m_name(name), m_addr(addr), m_cat_o(categoris), m_owner(owner) 
{     
    int pos = 0;
    
    while( true )
    {    
    	m_categ.push_back( categoris.substr(pos,categoris.find(";",pos)-pos) ); 
	pos = categoris.find(";",pos);
       	if( pos++ == string::npos ) break;
    }    
};

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

