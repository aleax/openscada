/***************************************************************************
 *   Copyright (C) 2004 by Roman Savochenko                                *
 *   rom_as@fromru.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <unistd.h>
#include <getopt.h>
#include <signal.h>

#include "tsys.h"
#include "tkernel.h"
#include "tarchives.h"
            
//================================================================
//=============== TArchiveS =======================================
//================================================================
const char *TArchiveS::o_name = "TArchiveS";

TArchiveS::TArchiveS( TKernel *app ) : 
    TGRPModule(app,"Archive"), m_mess_r_stat(false), m_mess_per(2), 
    m_bd_mess("","","arh_mess.dbf"), m_bd_val("","","arh_val.dbf"),
    el_mess(""), el_val("")
{
    s_name = "Archives"; 
    
    SFld gen_el[] =
    {
	{"NAME" ,Mess->I18N("Name")               ,T_STRING,"","20"},
	{"DESCR",Mess->I18N("Description")        ,T_STRING,"","50"},
	{"MODUL",Mess->I18N("Module(plugin) name"),T_STRING,"","20"},
	{"START",Mess->I18N("Start archive")      ,T_BOOL  ,"","1" }
    };
    SFld mess_el[] =
    {
	{"CATEG"   ,Mess->I18N("Message categories"),T_STRING,"" ,"20"},
	{"LEVEL"   ,Mess->I18N("Message level")     ,T_DEC   ,"" ,"1" ,"0;7"},
	{"ADDR"    ,Mess->I18N("Address")           ,T_STRING,"" ,"50"}
    };
    SFld val_el[] =
    {
	{"ADDR"    ,Mess->I18N("Address")     ,T_STRING,"" ,"50"}
    };
    // Fill messages elements
    for(unsigned i = 0; i < sizeof(gen_el)/sizeof(SFld); i++)  el_mess.elAdd(&gen_el[i]);
    for(unsigned i = 0; i < sizeof(mess_el)/sizeof(SFld); i++) el_mess.elAdd(&mess_el[i]);
    // Fill values elements
    for(unsigned i = 0; i < sizeof(gen_el)/sizeof(SFld); i++)  el_val.elAdd(&gen_el[i]);
    for(unsigned i = 0; i < sizeof(val_el)/sizeof(SFld); i++)  el_val.elAdd(&mess_el[i]);
}

TArchiveS::~TArchiveS(  )
{
    if( m_mess_r_stat )
    {
	m_mess_r_endrun = true;
	SYS->event_wait( m_mess_r_stat, false, string(o_name)+": The archive thread is stoping....");	
	pthread_join( m_mess_pthr, NULL );
    }    
    
    /*
    vector<SArhS> list;
    messList( list );
    for(unsigned i_m = 0; i_m < list.size(); i_m++)
        messDel( list[i_m] );
	
    valList( list );
    for(unsigned i_m = 0; i_m < list.size(); i_m++)
	valDel( list[i_m] );	
    */
}

void TArchiveS::gmdInit( )
{
    loadBD();
}

string TArchiveS::opt_descr(  )
{
    char buf[STR_BUF_LEN];
    snprintf(buf,sizeof(buf),Mess->I18N(
    	"========================== The Archive subsystem options ===================\n"
	"    --ArhPath = <path>  set modules <path>;\n"
	"------------ Parameters of section <%s> in config file -----------\n"
    	"mod_path    <path>      set modules <path>;\n"
    	"MessBD      <fullname>  Messages bd: \"<TypeBD>:<NameBD>:<NameTable>\";\n"
    	"ValBD       <fullname>  Value bd: \"<TypeBD>:<NameBD>:<NameTable>\";\n"
    	"mess_period <per>       set message arhiving period;\n\n"
	),gmdName().c_str());

    return(buf);
}

void TArchiveS::gmdCheckCommandLine( )
{
    TGRPModule::gmdCheckCommandLine( );

    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{"help"    ,0,NULL,'h'},
	{"ArhPath" ,1,NULL,'m'},
	{NULL      ,0,NULL,0  }
    };

    optind=0,opterr=0;	
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

void TArchiveS::gmdUpdateOpt()
{
    TGRPModule::gmdUpdateOpt();
    
    try{ DirPath = gmdXMLCfgNode()->get_child("id","mod_path")->get_text(); }
    catch(...) {  }
    try{ m_mess_per = atoi( gmdXMLCfgNode()->get_child("id","mess_period")->get_text().c_str() ); }
    catch(...) {  }
    try
    {
    	string opt = gmdXMLCfgNode()->get_child("id","MessBD")->get_text(); 
	int pos = 0;
        m_bd_mess.tp  = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
	m_bd_mess.bd  = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
        m_bd_mess.tbl = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
    }
    catch(...) {  }
    if( !m_bd_mess.tp.size() ) m_bd_mess.tp = Owner().DefBDType;
    if( !m_bd_mess.bd.size() ) m_bd_mess.bd = Owner().DefBDName;
    try
    {
    	string opt = gmdXMLCfgNode()->get_child("id","ValBD")->get_text(); 
	int pos = 0;
        m_bd_val.tp  = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
	m_bd_val.bd  = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
        m_bd_val.tbl = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
    }
    catch(...) {  }
    if( !m_bd_val.tp.size() ) m_bd_val.tp = Owner().DefBDType;
    if( !m_bd_val.bd.size() ) m_bd_val.bd = Owner().DefBDName;
}

void TArchiveS::loadBD( )
{    
    TConfig *c_el;
    
    // Load messages BD
    try
    {
	AutoHD<TTable> tbl = Owner().BD().open(m_bd_mess);	
	for( int i_ln = 0; i_ln < tbl.at().nLines(); i_ln++ )
	{	    
	    TConfig c_el(&el_mess);
	    c_el.cfLoadValBD(i_ln,tbl.at());
	    	    
	    AutoHD<TTipArchive> mod = gmdAt(c_el.cfg("MODUL").getS());
	    try{mod.at().messAdd(c_el.cfg("NAME").getS());}catch(...){}
	    mod.at().messAt(c_el.cfg("NAME").getS()).at().cfLoadValBD("NAME",tbl.at());
	}
	tbl.free();
	Owner().BD().close(m_bd_mess);
    }catch(...){}    
    // Load values BD    
    try
    {
	AutoHD<TTable> tbl = Owner().BD().open(m_bd_val);				
	for( int i_ln = 0; i_ln < tbl.at().nLines(); i_ln++ )
	{	    
	    TConfig c_el(&el_val);
	    c_el.cfLoadValBD(i_ln,tbl.at());
	    
	    AutoHD<TModule> mod = gmdAt(c_el.cfg("MODUL").getS());
	    try{((TTipArchive &)mod.at()).valAdd(c_el.cfg("NAME").getS());}catch(...){}
	    ((TTipArchive &)mod.at()).valAt(c_el.cfg("NAME").getS()).at().cfLoadValBD("NAME",tbl.at());
	}
	tbl.free();
	Owner().BD().close(m_bd_val);
    }catch(...){}     
}

void TArchiveS::saveBD( )
{
    vector<string> t_lst, o_lst;
    // Save messages bd  
    AutoHD<TTable> tbl = Owner().BD().open(m_bd_mess,true);	
    tbl.at().clean();
    el_mess.elUpdateBDAttr( tbl.at() );
    gmdList(t_lst);
    for( int i_t = 0; i_t < t_lst.size(); i_t++ )
    {
	AutoHD<TTipArchive> mod = gmdAt(t_lst[i_t]);
	mod.at().messList(o_lst);
	for( int i_o = 0; i_o < o_lst.size(); i_o++ )
	    mod.at().messAt(o_lst[i_o]).at().cfSaveValBD("NAME",tbl.at());
    }    
    tbl.at().save();
    tbl.free();
    Owner().BD().close(m_bd_mess);
    
    // Save values bd
    tbl = Owner().BD().open(m_bd_val,true);
    tbl.at().clean();
    el_val.elUpdateBDAttr( tbl.at() );
    gmdList(t_lst);
    for( int i_t = 0; i_t < t_lst.size(); i_t++ )
    {
	AutoHD<TTipArchive> mod = gmdAt(t_lst[i_t]);
	mod.at().valList(o_lst);
	for( int i_o = 0; i_o < o_lst.size(); i_o++ )
	    mod.at().valAt(o_lst[i_o]).at().cfSaveValBD("NAME",tbl.at());
    }    
    tbl.at().save();
    tbl.free();
    Owner().BD().close(m_bd_val);
}

void TArchiveS::gmdStart( )
{    
    vector<string> t_lst, o_lst;
    pthread_attr_t      pthr_attr;
    if( m_mess_r_stat ) return; 
    
    // Archives start    
    gmdList(t_lst);
    for( int i_t = 0; i_t < t_lst.size(); i_t++ )
    {
	AutoHD<TModule> mod = gmdAt(t_lst[i_t]);
	((TTipArchive &)mod.at()).messList(o_lst);
	for( int i_o = 0; i_o < o_lst.size(); i_o++ )
	{
	    AutoHD<TArchiveMess> mess = ((TTipArchive &)mod.at()).messAt(o_lst[i_o]);
	    if( mess.at().toStart() ) mess.at().start();
	}
    }    
    // Self task start
    pthread_attr_init(&pthr_attr);
    pthread_attr_setschedpolicy(&pthr_attr,SCHED_OTHER);
    pthread_create(&m_mess_pthr,&pthr_attr,TArchiveS::MessArhTask,this);
    pthread_attr_destroy(&pthr_attr);
    if( SYS->event_wait(m_mess_r_stat, true, string(o_name)+": Task of The message arhivator is starting....",5) )	
	throw TError("(%s) Task of The message arhivator no started!",o_name);
}

void TArchiveS::gmdStop( )
{    
    vector<string> t_lst, o_lst;
    // Archives stop        
    gmdList(t_lst);
    for( int i_t = 0; i_t < t_lst.size(); i_t++ )
    {
	AutoHD<TModule> mod = gmdAt(t_lst[i_t]);
	((TTipArchive &)mod.at()).messList(o_lst);
	for( int i_o = 0; i_o < o_lst.size(); i_o++ )
	{
	    AutoHD<TArchiveMess> mess = ((TTipArchive &)mod.at()).messAt(o_lst[i_o]);
	    if( mess.at().startStat() ) mess.at().stop();
	}
    }    
    
    if( m_mess_r_stat )
    {
    	m_mess_r_endrun = true;
    	if( SYS->event_wait(m_mess_r_stat, false, string(o_name)+": Task of The message arhivator is stoping....",5) )
	    throw TError("(%s) Task of The message arhivator no stoped!",o_name);       
	pthread_join( m_mess_pthr, NULL );
    }
}

void *TArchiveS::MessArhTask(void *param)
{
    bool quit = false;
    int i_cnt = 0;
    TArchiveS &arh = *(TArchiveS *)param;
    vector<SBufRec> i_mess, o_mess;    
    time_t t_last = 0, t_cur;

#if OSC_DEBUG
    arh.m_put("DEBUG",MESS_DEBUG,"Thread <%d>!",getpid() );
#endif	

    arh.m_mess_r_stat = true;
    arh.m_mess_r_endrun = false;
    
    while( !quit )
    {	 
        if( arh.m_mess_r_endrun ) quit = true;
	if( ++i_cnt > arh.m_mess_per*1000/STD_WAIT_DELAY || quit )
	{
	    i_cnt = 0;
    	    try
    	    {
    		t_cur = time(NULL);
    		Mess->get( t_last, t_cur, i_mess );
		if( i_mess.size() )
		{
		    t_last = i_mess[i_mess.size()-1].time+1;    		
		    
		    vector<string> t_lst, o_lst;
		    arh.gmdList(t_lst);
		    for( int i_t = 0; i_t < t_lst.size(); i_t++ )
		    {
			AutoHD<TModule> mod = arh.gmdAt(t_lst[i_t]);
			((TTipArchive &)mod.at()).messList(o_lst);
			for( int i_o = 0; i_o < o_lst.size(); i_o++ )
			{
			    AutoHD<TArchiveMess> mess = ((TTipArchive &)mod.at()).messAt(o_lst[i_o]);
			    if( !mess.at().startStat() ) continue;
			    o_mess.clear();
			    for(unsigned i_m = 0; i_m < i_mess.size(); i_m++)
			    {
				unsigned i_cat;
				vector<string> categ;
				mess.at().categ(categ);
				for( i_cat = 0; i_cat < categ.size(); i_cat++ )
				    if( categ[i_cat] == i_mess[i_m].categ ) break;
				if( (i_cat < categ.size() || !categ.size()) 
				    && i_mess[i_m].level >= mess.at().level() ) 
    				{
    				    i_mess[i_m].mess.insert(0,SYS->Station()+":");
    				    o_mess.push_back(i_mess[i_m]);
    				}
    			    }
			    if( o_mess.size() ) mess.at().put(o_mess);
			}
		    }    
		}
    	    }
    	    catch(TError err){ arh.m_put_s("SYS",MESS_ERR,err.what()); }
	}	
	usleep(STD_WAIT_DELAY*1000);
    }
    
    arh.m_mess_r_stat = false;
    
    return(NULL);
}

void TArchiveS::gmdDel( const string &name )
{    
    /*
    vector<SArhS> list;
    messList( list );
    for(unsigned i_m = 0; i_m < list.size(); i_m++)
	if( list[i_m].tp == name ) messDel( list[i_m] );
	
    valList( list );
    for(unsigned i_m = 0; i_m < list.size(); i_m++)
	if( list[i_m].tp == name ) valDel( list[i_m] );
    */
	    
    TGRPModule::gmdDel( name );
}

//================== Controll functions ========================
void TArchiveS::ctr_fill_info( XMLNode *inf )
{
    char *dscr = "dscr";
    TGRPModule::ctr_fill_info( inf );

    char *i_cntr = 
    	"<area id='a_bd' acs='0440'>"
	" <fld id='t_bdm' acs='0660' tp='str' dest='select' select='/a_bd/b_mod'/>"
	" <fld id='bdm' acs='0660' tp='str'/>"
	" <fld id='tblm' acs='0660' tp='str'/>"
	" <fld id='t_bdv' acs='0660' tp='str' dest='select' select='/a_bd/b_mod'/>"
	" <fld id='bdv' acs='0660' tp='str'/>"
	" <fld id='tblv' acs='0660' tp='str'/>"
	" <fld id='m_per' acs='0660' tp='dec'/>"    
	" <comm id='load_bd'/>"
	" <comm id='upd_bd'/>"
	" <list id='b_mod' tp='str' hide='1'/>"
	"</area>";
    
    XMLNode *n_add = inf->ins_child(0);
    n_add->load_xml(i_cntr);
    n_add->set_attr(dscr,Mess->I18N("Subsystem"));
    n_add->get_child(0)->set_attr(dscr,Mess->I18N("Message BD (module:bd:table)"));
    n_add->get_child(3)->set_attr(dscr,Mess->I18N("Value BD (module:bd:table)"));
    n_add->get_child(6)->set_attr(dscr,Mess->I18N("Period reading new messages"));
    n_add->get_child(7)->set_attr(dscr,Mess->I18N("Load"));
    n_add->get_child(8)->set_attr(dscr,Mess->I18N("Save"));
    
    //Insert to Help
    char *i_help = "<fld id='g_help' acs='0440' tp='str' cols='90' rows='5'/>";
    
    n_add = inf->get_child("id","help")->add_child();    
    n_add->load_xml(i_help);
    n_add->set_attr(dscr,Mess->I18N("Options help"));
}

void TArchiveS::ctr_din_get_( const string &a_path, XMLNode *opt )
{
    vector<string> list;
    
    if( a_path == "/a_bd/t_bdm" )	ctr_opt_setS( opt, m_bd_mess.tp );
    else if( a_path == "/a_bd/bdm" )	ctr_opt_setS( opt, m_bd_mess.bd );
    else if( a_path == "/a_bd/tblm" )	ctr_opt_setS( opt, m_bd_mess.tbl );
    else if( a_path == "/a_bd/t_bdv" )	ctr_opt_setS( opt, m_bd_val.tp );
    else if( a_path == "/a_bd/bdv" )	ctr_opt_setS( opt, m_bd_val.bd );
    else if( a_path == "/a_bd/tblv" )	ctr_opt_setS( opt, m_bd_val.tbl );
    else if( a_path == "/a_bd/b_mod" )
    {
	opt->clean_childs();
	Owner().BD().gmdList(list);
	for( unsigned i_a=0; i_a < list.size(); i_a++ )
	    ctr_opt_setS( opt, list[i_a], i_a );
    }
    else if( a_path == "/a_bd/m_per" )	ctr_opt_setI( opt, m_mess_per );
    else if( a_path == "/help/g_help" )	ctr_opt_setS( opt, opt_descr() );       
    else TGRPModule::ctr_din_get_( a_path, opt );
}

void TArchiveS::ctr_din_set_( const string &a_path, XMLNode *opt )
{
    if( a_path == "/a_bd/t_bdm" )    	m_bd_mess.tp   = ctr_opt_getS( opt );
    else if( a_path == "/a_bd/bdm" )	m_bd_mess.bd   = ctr_opt_getS( opt );
    else if( a_path == "/a_bd/tblm" )  	m_bd_mess.tbl  = ctr_opt_getS( opt );
    else if( a_path == "/a_bd/t_bdv" ) 	m_bd_val.tp    = ctr_opt_getS( opt );
    else if( a_path == "/a_bd/bdv" )   	m_bd_val.bd    = ctr_opt_getS( opt );
    else if( a_path == "/a_bd/tblv" )  	m_bd_val.tbl   = ctr_opt_getS( opt );
    else if( a_path == "/a_bd/m_per" ) 	m_mess_per = ctr_opt_getI( opt );
    else TGRPModule::ctr_din_set_( a_path, opt );
}

void TArchiveS::ctr_cmd_go_( const string &a_path, XMLNode *fld, XMLNode *rez )
{
    if( a_path == "/a_bd/load_bd" )	loadBD();
    else if( a_path == "/a_bd/upd_bd" )	saveBD();    
    else TGRPModule::ctr_cmd_go_( a_path, fld, rez );
}


//================================================================
//=========== TTipArchive =========================================
//================================================================
const char *TTipArchive::o_name = "TTipArchive";

TTipArchive::TTipArchive() : m_hd_mess(o_name), m_hd_val(o_name)
{

}
    
TTipArchive::~TTipArchive()
{
    vector<string> list;

    m_hd_mess.lock();
    messList(list);
    for( unsigned i_ls = 0; i_ls < list.size(); i_ls++)
        messDel(list[i_ls]);    
	
    m_hd_val.lock();
    valList(list);
    for( unsigned i_ls = 0; i_ls < list.size(); i_ls++)
	valDel(list[i_ls]);    
}

void TTipArchive::messAdd(const string &name )
{
    TArchiveMess *mess = AMess(name);
    try{ m_hd_mess.obj_add( mess, &mess->name() ); }
    catch(TError err) {	delete mess; }
}

void TTipArchive::valAdd( const string &name )
{
    TArchiveVal *val = AVal(name);
    try{ m_hd_val.obj_add( val, &val->name() ); }
    catch(TError err) {	delete val; }    
}

//================== Controll functions ========================
void TTipArchive::ctr_fill_info( XMLNode *inf )
{
    char *dscr="dscr";
    
    TModule::ctr_fill_info( inf );    
    
    char *i_cntr = 
	"<area id='arch'>"
	" <list id='mess' s_com='add,del' tp='br' mode='att'/>"
	" <list id='val' s_com='add,del' tp='br' mode='att'/>"
	"</area>";
    
    XMLNode *n_add = inf->ins_child(0);
    n_add->load_xml(i_cntr);
    n_add->set_attr(dscr,Mess->I18N("Archives"));
    n_add->get_child(0)->set_attr(dscr,Mess->I18N("Message archives"));
    n_add->get_child(1)->set_attr(dscr,Mess->I18N("Value archives"));
}

void TTipArchive::ctr_din_get_( const string &a_path, XMLNode *opt )
{
    vector<string> list;    

    if( a_path == "/arch/mess" )
    {
	opt->clean_childs();
	messList(list);
	for( unsigned i_a=0; i_a < list.size(); i_a++ )
	    ctr_opt_setS( opt, list[i_a], i_a ); 	
    }
    else if( a_path == "/arch/val" )
    {
	opt->clean_childs();
	valList(list);
	for( unsigned i_a=0; i_a < list.size(); i_a++ )
	    ctr_opt_setS( opt, list[i_a], i_a ); 	
    }   
    else TModule::ctr_din_get_( a_path, opt );
}

void TTipArchive::ctr_din_set_( const string &a_path, XMLNode *opt )
{
    if( a_path.substr(0,10) == "/arch/mess" )
	for( int i_el=0; i_el < opt->get_child_count(); i_el++)	    
	{
	    XMLNode *t_c = opt->get_child(i_el);
	    if( t_c->get_name() == "el")
	    {
		if(t_c->get_attr("do") == "add")      messAdd(t_c->get_text());
		else if(t_c->get_attr("do") == "del") messDel(t_c->get_text());
	    }
	}
    else if( a_path.substr(0,9) == "/arch/val" )
	for( int i_el=0; i_el < opt->get_child_count(); i_el++)	    
	{
	    XMLNode *t_c = opt->get_child(i_el);
	    if( t_c->get_name() == "el")
	    {
		if(t_c->get_attr("do") == "add")      valAdd(t_c->get_text());
		else if(t_c->get_attr("do") == "del") valDel(t_c->get_text());
	    }
	}
    else TModule::ctr_din_set_( a_path, opt );
}

AutoHD<TContr> TTipArchive::ctr_at1( const string &a_path )
{
    if( a_path.substr(0,10) == "/arch/mess" )     return messAt(ctr_path_l(a_path,2));
    else if( a_path.substr(0,9) == "/arch/val" ) return valAt(ctr_path_l(a_path,2));
    else return TModule::ctr_at1(a_path);
}


//================================================================
//=========== TArchiveMess ========================================
//================================================================
const char *TArchiveMess::o_name = "TArchiveMess";

TArchiveMess::TArchiveMess(const string &name, TTipArchive *owner) : 
    m_owner(owner), TConfig( &((TArchiveS &)owner->Owner()).messE() ), run_st(false),
    m_name(cfg("NAME").getS()), m_lname(cfg("DESCR").getS()), m_addr(cfg("ADDR").getS()), 
    m_cat_o(cfg("CATEG").getS()), m_level(cfg("LEVEL").getI()) ,m_start(cfg("START").getB())
{     
    m_name = name;
    cfg("MODUL").setS(Owner().modName());
};

TArchiveMess::~TArchiveMess()
{

}

void TArchiveMess::load( )
{
    AutoHD<TTable> tbl = Owner().Owner().Owner().BD().open(((TArchiveS &)Owner().Owner()).messB());	
    cfLoadValBD("NAME",tbl.at());
    tbl.free(); 
    Owner().Owner().Owner().BD().close(((TArchiveS &)Owner().Owner()).messB());
}

void TArchiveMess::save( )
{
    AutoHD<TTable> tbl = Owner().Owner().Owner().BD().open(((TArchiveS &)Owner().Owner()).messB());	
    cfSaveValBD("NAME",tbl.at());
    tbl.at().save(); 
    tbl.free(); 
    Owner().Owner().Owner().BD().close(((TArchiveS &)Owner().Owner()).messB());
}

void TArchiveMess::categ( vector<string> &list )
{
    int pos = 0,c_pos;    
    do
    {    
	c_pos = m_cat_o.find(";",pos);
	string val = m_cat_o.substr(pos,c_pos-pos);
    	if( val.size() ) list.push_back( val ); 
	pos = c_pos+1;	
    }while( c_pos != string::npos );
}

//================== Controll functions ========================
void TArchiveMess::ctr_fill_info( XMLNode *inf )
{
    char *i_cntr = 
    	"<oscada_cntr>"
	" <area id='a_prm'>"
	"  <fld id='dscr' acs='0664' tp='str'/>"
	"  <fld id='addr' acs='0664' tp='str' dest='dir'/>"
	"  <fld id='lvl'  acs='0664' tp='dec'/>"
	"  <list id='cats' acs='0664' tp='str' s_com='add,del'/>"
	"  <fld id='start' acs='0664' tp='bool'/>"
	"  <fld id='r_st'  acs='0664' tp='bool'/>"
	"  <comm id='load' acs='0550'/>"
	"  <comm id='save' acs='0550'/>"    
	" </area>"
	" <area id='a_mess'>"
	"  <table id='mess' tp='flow' acs='0440'>"
	"   <comm id='view'>"
	"    <fld id='beg' tp='time'/>"
	"    <fld id='end' tp='time'/>"
	"    <fld id='cat' tp='str'/>"
	"    <fld id='lvl' tp='dec' min='0' max='7'/>"
	"   </comm>"
	"   <list id='0' tp='time'/>"
	"   <list id='1' tp='str'/>"
	"   <list id='2' tp='dec'/>"
	"   <list id='3' tp='str'/>"
	"  </table>"
	"  <comm id='add'>"
	"   <fld id='tm' tp='time'/>"
	"   <fld id='cat' tp='str'/>"
	"   <fld id='lvl' tp='dec' min='0' max='7'/>"
	"   <fld id='mess' tp='str'/>"
	"  </comm>"
	" </area>"
	"</oscada_cntr>";
    char *dscr = "dscr";
    
    inf->load_xml( i_cntr );
    inf->set_text(Mess->I18N("Message archive: ")+name());
    XMLNode *c_nd = inf->get_child(0);
    c_nd->set_attr(dscr,Mess->I18N("Archive"));
    c_nd->get_child(0)->set_attr(dscr,cfg("DESCR").fld().descr());
    c_nd->get_child(1)->set_attr(dscr,cfg("ADDR").fld().descr());
    c_nd->get_child(2)->set_attr(dscr,cfg("LEVEL").fld().descr());
    c_nd->get_child(3)->set_attr(dscr,cfg("CATEG").fld().descr());
    c_nd->get_child(4)->set_attr(dscr,Mess->I18N("To start"));
    c_nd->get_child(5)->set_attr(dscr,Mess->I18N("Runing"));
    c_nd->get_child(6)->set_attr(dscr,Mess->I18N("Load"));
    c_nd->get_child(7)->set_attr(dscr,Mess->I18N("Save"));
    c_nd = inf->get_child(1);
    c_nd->set_attr(dscr,Mess->I18N("Messages"));
    XMLNode *c_nd1 = c_nd->get_child(0);
	c_nd1->set_attr(dscr,Mess->I18N("Messages"));
	XMLNode *c_nd2 = c_nd1->get_child(0);    
	    c_nd2->set_attr(dscr,Mess->I18N("View"));
	    c_nd2->get_child(0)->set_attr(dscr,Mess->I18N("Begin"));
	    c_nd2->get_child(1)->set_attr(dscr,Mess->I18N("End"));
	    c_nd2->get_child(2)->set_attr(dscr,Mess->I18N("Category"));
	    c_nd2->get_child(3)->set_attr(dscr,Mess->I18N("Level"));
	c_nd1->get_child(1)->set_attr(dscr,Mess->I18N("Time"));
	c_nd1->get_child(2)->set_attr(dscr,Mess->I18N("Category"));
	c_nd1->get_child(3)->set_attr(dscr,Mess->I18N("Level"));
	c_nd1->get_child(4)->set_attr(dscr,Mess->I18N("Message"));	
    c_nd1 = c_nd->get_child(1);
	c_nd1->set_attr(dscr,Mess->I18N("Add"));
	c_nd1->get_child(0)->set_attr(dscr,Mess->I18N("Time"));
	c_nd1->get_child(1)->set_attr(dscr,Mess->I18N("Category"));
	c_nd1->get_child(2)->set_attr(dscr,Mess->I18N("Level"));
	c_nd1->get_child(3)->set_attr(dscr,Mess->I18N("Message"));	
}

void TArchiveMess::ctr_din_get_( const string &a_path, XMLNode *opt )
{
    if( a_path == "/a_prm/name" )       ctr_opt_setS( opt, m_name );
    else if( a_path == "/a_prm/dscr" )  ctr_opt_setS( opt, m_lname );
    else if( a_path == "/a_prm/addr" )  ctr_opt_setS( opt, m_addr );
    else if( a_path == "/a_prm/lvl" )   ctr_opt_setI( opt, m_level );
    else if( a_path == "/a_prm/start" ) ctr_opt_setB( opt, m_start );
    else if( a_path == "/a_prm/r_st" )  ctr_opt_setB( opt, run_st );
    else if( a_path == "/a_prm/cats" )
    {
	opt->clean_childs();
	vector<string> list;
	categ(list);
	for( int i_l=0; i_l < list.size(); i_l++)
	    ctr_opt_setS( opt, list[i_l], i_l );       
    }
    else throw TError("(%s) Branch %s error!",o_name,a_path.c_str());
}

void TArchiveMess::ctr_din_set_( const string &a_path, XMLNode *opt )
{
    if( a_path == "/a_prm/name" )       m_name  = ctr_opt_getS( opt );
    else if( a_path == "/a_prm/dscr" )  m_lname = ctr_opt_getS( opt );
    else if( a_path == "/a_prm/addr" )  m_addr  = ctr_opt_getS( opt );
    else if( a_path == "/a_prm/lvl" )   m_level = ctr_opt_getI( opt );
    else if( a_path == "/a_prm/start" ) m_start = ctr_opt_getB( opt );
    else if( a_path == "/a_prm/r_st" ) { if( ctr_opt_getB( opt ) ) start(); else stop(); }
    else if( a_path == "/a_prm/cats" )
	for( int i_el=0; i_el < opt->get_child_count(); i_el++)	    
	{
	    XMLNode *t_c = opt->get_child(i_el);
	    if( t_c->get_name() == "el")
	    {
		if(t_c->get_attr("do") == "add")
		{
		    if( m_cat_o.size() ) m_cat_o = m_cat_o+";";
		    m_cat_o = m_cat_o+t_c->get_text();
		}
		else if(t_c->get_attr("do") == "del") 
		{
		    int pos = m_cat_o.find(string(";")+t_c->get_text(),0);
		    if(pos != string::npos) 
			m_cat_o.erase(pos,t_c->get_text().size()+1);
		    else                    
			m_cat_o.erase(m_cat_o.find(t_c->get_text(),0),t_c->get_text().size()+1);
		}
	    }
	}
    else throw TError("(%s) Branch %s error!",o_name,a_path.c_str());
}

void TArchiveMess::ctr_cmd_go_( const string &a_path, XMLNode *fld, XMLNode *rez )
{
    if( a_path == "/a_prm/load" )      load();
    else if( a_path == "/a_prm/save" ) save();
    else if( a_path == "/a_mess/mess/view" )     
    {
	vector<SBufRec> rec;
	get( ctr_opt_getI(ctr_id(fld,"beg")),
	    ctr_opt_getI(ctr_id(fld,"end")),
	    rec,
	    ctr_opt_getS(ctr_id(fld,"cat")),
	    ctr_opt_getI(ctr_id(fld,"lvl")) );
	
	XMLNode *n_tm   = ctr_id(rez,"0");
	XMLNode *n_cat  = ctr_id(rez,"1");
	XMLNode *n_lvl  = ctr_id(rez,"2");
	XMLNode *n_mess = ctr_id(rez,"3");
	for( int i_rec = 0; i_rec < rec.size(); i_rec++)
	{
	    ctr_opt_setI(n_tm,rec[i_rec].time,i_rec);
	    ctr_opt_setS(n_cat,rec[i_rec].categ,i_rec);
	    ctr_opt_setI(n_lvl,rec[i_rec].level,i_rec);
	    ctr_opt_setS(n_mess,rec[i_rec].mess,i_rec);
	}
    }
    else if( a_path == "/a_mess/add" )
    {
	vector<SBufRec> brec;
	SBufRec rec;
	rec.time  = ctr_opt_getI(ctr_id(fld,"tm"));
	rec.categ = ctr_opt_getS(ctr_id(fld,"cat"));
	rec.level = ctr_opt_getI(ctr_id(fld,"lvl"));
	rec.mess  = ctr_opt_getS(ctr_id(fld,"mess"));
	brec.push_back(rec);
	put(brec);	
    }
    else throw TError("(%s) Branch %s error!",o_name,a_path.c_str());
}
//================================================================
//=========== TArchiveVal =========================================
//================================================================
const char *TArchiveVal::o_name = "TArchiveVal";
 
TArchiveVal::TArchiveVal( const string &name, TTipArchive *owner ) : 
    m_owner(owner), TConfig(&((TArchiveS &)owner->Owner()).valE()),    
    m_name(cfg("NAME").getS()), m_bd(cfg("ADDR").getS())   
{    
    m_name = name;
    cfg("MODUL").setS(Owner().modName());
}

TArchiveVal::~TArchiveVal()
{
    
}

//================== Controll functions ========================
void TArchiveVal::ctr_fill_info( XMLNode *inf )
{
    char *i_cntr = 
    	"<oscada_cntr>"
	"</oscada_cntr>";
    
    inf->load_xml( i_cntr );
    inf->set_text(string("Value archive: ")+name());
}

