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
#include "tarhives.h"

//================================================================
//=============== TArhiveS =======================================
//================================================================

const char *TArhiveS::o_name = "TArhiveS";
const char *TArhiveS::i_cntr = 
    "<area id='a_bd' acs='0440'>"
    " <fld id='t_bdm' acs='0660' tp='str' dest='select' select='a_bd/b_mod'/>"
    " <fld id='bdm' acs='0660' tp='str'/>"
    " <fld id='tblm' acs='0660' tp='str'/>"
    " <fld id='t_bdv' acs='0660' tp='str' dest='select' select='a_bd/b_mod'/>"
    " <fld id='bdv' acs='0660' tp='str'/>"
    " <fld id='tblv' acs='0660' tp='str'/>"
    " <fld id='m_per' acs='0660' tp='dec'/>"    
    " <fld id='g_help' acs='0440' tp='str' cols='90' rows='5'/>"
    " <comm id='load_bd'/>"
    " <comm id='upd_bd'/>"
    " <list id='b_mod' tp='str' hide='1'/>"
    "</area>";

TArhiveS::TArhiveS( TKernel *app ) : 
    TGRPModule(app,"Arhiv"), m_mess_r_stat(false), m_mess_per(2), 
    m_bd_mess("","","arh_mess.dbf"), m_bd_val("","","arh_val.dbf"),
    el_mess(""), el_val("")
{
    s_name = "Arhives"; 
    
    SFld gen_el[] =
    {
	{"NAME" ,Mess->I18N("Arhive name")        ,T_STRING,"","20"},
	{"DESCR",Mess->I18N("Arhive description") ,T_STRING,"","50"},
	{"MODUL",Mess->I18N("Module(plugin) name"),T_STRING,"","20"},
	{"START",Mess->I18N("Start arhive")       ,T_BOOL  ,"","1" }
    };
    SFld mess_el[] =
    {
	{"CATEG"   ,Mess->I18N("Message categories") ,T_STRING,"" ,"20"},
	{"LEVEL"   ,Mess->I18N("Message level")      ,T_DEC   ,"" ,"1" ,"0;7"},
	{"ADDR"    ,Mess->I18N("Arhive address")     ,T_STRING,"" ,"50"}
    };
    SFld val_el[] =
    {
	{"ADDR"    ,Mess->I18N("Arhive address")     ,T_STRING,"" ,"50"}
    };
    // Fill messages elements
    for(unsigned i = 0; i < sizeof(gen_el)/sizeof(SFld); i++)  el_mess.elAdd(&gen_el[i]);
    for(unsigned i = 0; i < sizeof(mess_el)/sizeof(SFld); i++) el_mess.elAdd(&mess_el[i]);
    // Fill values elements
    for(unsigned i = 0; i < sizeof(gen_el)/sizeof(SFld); i++)  el_val.elAdd(&gen_el[i]);
    for(unsigned i = 0; i < sizeof(val_el)/sizeof(SFld); i++)  el_val.elAdd(&mess_el[i]);
}

TArhiveS::~TArhiveS(  )
{
    if( m_mess_r_stat )
    {
	m_mess_r_endrun = true;
	SYS->event_wait( m_mess_r_stat, false, string(o_name)+": The arhive thread is stoping....");	
	pthread_join( m_mess_pthr, NULL );
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

string TArhiveS::opt_descr(  )
{
    char buf[STR_BUF_LEN];
    snprintf(buf,sizeof(buf),Mess->I18N(
    	"========================== The Arhive subsystem options ===================\n"
	"    --ArhPath = <path>  set modules <path>;\n"
	"------------ Parameters of section <%s> in config file -----------\n"
    	"mod_path    <path>      set modules <path>;\n"
    	"MessBD      <fullname>  Messages bd recorded: \"<TypeBD>:<NameBD>:<NameTable>\";\n"
    	"ValBD       <fullname>  Value bd recorded: \"<TypeBD>:<NameBD>:<NameTable>\";\n"
    	"mess_period <per>       set message arhiving period;\n"
	),gmd_Name().c_str());

    return(buf);
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
	    case 'h': fprintf(stdout,opt_descr().c_str()); break;
	    case 'm': DirPath = optarg;     break;
	    case -1 : break;
	}
    } while(next_opt != -1);
}

void TArhiveS::gmd_UpdateOpt()
{
    TGRPModule::gmd_UpdateOpt();
    
    try{ DirPath = gmd_XMLCfgNode()->get_child("id","mod_path")->get_text(); }
    catch(...) {  }
    try{ m_mess_per = atoi( gmd_XMLCfgNode()->get_child("id","mess_period")->get_text().c_str() ); }
    catch(...) {  }
    try
    {
    	string opt = gmd_XMLCfgNode()->get_child("id","MessBD")->get_text(); 
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
    	string opt = gmd_XMLCfgNode()->get_child("id","ValBD")->get_text(); 
	int pos = 0;
        m_bd_val.tp  = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
	m_bd_val.bd  = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
        m_bd_val.tbl = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
    }
    catch(...) {  }
    if( !m_bd_val.tp.size() ) m_bd_val.tp = Owner().DefBDType;
    if( !m_bd_val.bd.size() ) m_bd_val.bd = Owner().DefBDName;
}

void TArhiveS::LoadBD( )
{    
    TConfig *c_el;
    SHDBD b_hd;
    
    // Load messages BD
    try
    {
	b_hd = Owner().BD().open( m_bd_mess );    
	for( int i_ln = 0; i_ln < Owner().BD().at(b_hd).NLines(); i_ln++ )
	{	    
	    c_el = new TConfig(&el_mess);
	    c_el->cfLoadValBD(i_ln,Owner().BD().at(b_hd));
	    SArhS nm = SArhS(c_el->cfg("MODUL").getS(),c_el->cfg("NAME").getS());
	    delete c_el;	
	    	    
	    try{mess_add(nm);}catch(...){}
	    SHDArh hd = mess_att(nm);	    
	    mess_at(hd).cfLoadValBD("NAME",Owner().BD().at(b_hd));
	    mess_det(hd);
	}
	Owner().BD().close(b_hd);
    }catch(...){}    
    // Load values BD    
    try
    {
	b_hd = Owner().BD().open( m_bd_val );    
	for( int i_ln = 0; i_ln < Owner().BD().at(b_hd).NLines(); i_ln++ )
	{	    
	    c_el = new TConfig(&el_val);
	    c_el->cfLoadValBD(i_ln,Owner().BD().at(b_hd));
	    SArhS nm = SArhS(c_el->cfg("MODUL").getS(),c_el->cfg("NAME").getS());
	    delete c_el;	
	    	    
	    try{val_add(nm);}catch(...){}
	    SHDArh hd = val_att(nm);	    
	    val_at(hd).cfLoadValBD("NAME",Owner().BD().at(b_hd));
	    val_det(hd);
	}
	Owner().BD().close(b_hd);
    }catch(...){}     
}

void TArhiveS::UpdateBD( )
{
    vector<SArhS> list;
    SHDBD b_hd;
    // Save messages bd
    b_hd = Owner().BD().open( m_bd_mess, true );
    Owner().BD().at(b_hd).Clean();
    el_mess.elUpdateBDAttr( Owner().BD().at(b_hd) );
    mess_list(list);
    for( int i_l = 0; i_l < list.size(); i_l++ )
    {
	SHDArh hd = mess_att(list[i_l]);
	mess_at(hd).cfSaveValBD("NAME",Owner().BD().at(b_hd));
	mess_det( hd );
    }
    Owner().BD().at(b_hd).Save();
    Owner().BD().close(b_hd);
    // Save values bd
    b_hd = Owner().BD().open( m_bd_val, true );
    Owner().BD().at(b_hd).Clean();
    el_val.elUpdateBDAttr( Owner().BD().at(b_hd) );
    val_list(list);
    for( int i_l = 0; i_l < list.size(); i_l++ )
    {
	SHDArh hd = val_att(list[i_l]);
	val_at(hd).cfSaveValBD("NAME",Owner().BD().at(b_hd));
	val_det( hd );
    }
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

void TArhiveS::mess_add( SArhS arh )
{
    unsigned m_hd = gmd_att( arh.tp );
    try{ gmd_at(m_hd).mess_add( arh.obj ); }
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

void TArhiveS::val_add( SArhS arh )
{
    unsigned m_hd = gmd_att( arh.tp );
    try{ gmd_at(m_hd).val_add( arh.obj ); }
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
    
    // Arhives start    
    vector<SArhS> list;
    mess_list(list);
    for( int i_l = 0; i_l < list.size(); i_l++ )
    {
	SHDArh hd = mess_att(list[i_l]);
	try
	{ 
	    if( mess_at(hd).toStart() ) 
		mess_at(hd).start(); 
	}
	catch(TError err) { m_put_s("SYS",MESS_ERR,err.what()); }
	mess_det( hd );
    }    
    // Self task start
    pthread_attr_init(&pthr_attr);
    pthread_attr_setschedpolicy(&pthr_attr,SCHED_OTHER);
    pthread_create(&m_mess_pthr,&pthr_attr,TArhiveS::MessArhTask,this);
    pthread_attr_destroy(&pthr_attr);
    if( SYS->event_wait(m_mess_r_stat, true, string(o_name)+": Task of The message arhivator is starting....",5) )	
	throw TError("(%s) Task of The message arhivator no started!",o_name);
}

void TArhiveS::gmd_Stop( )
{    
    // Arhives stop    
    vector<SArhS> list;
    mess_list(list);
    for( int i_l = 0; i_l < list.size(); i_l++ )
    {
	SHDArh hd = mess_att(list[i_l]);
	try{ mess_at(hd).stop(); }catch(...){} 
	mess_det( hd );
    }    
    
    if( m_mess_r_stat )
    {
    	m_mess_r_endrun = true;
    	if( SYS->event_wait(m_mess_r_stat, false, string(o_name)+": Task of The message arhivator is stoping....",5) )
	    throw TError("(%s) Task of The message arhivator no stoped!",o_name);       
	pthread_join( m_mess_pthr, NULL );
    }
}

void *TArhiveS::MessArhTask(void *param)
{
    bool quit = false;
    int i_cnt = 0;
    TArhiveS *arh = (TArhiveS *)param;
    vector<SBufRec> i_mess, o_mess;    
    time_t t_last = 0, t_cur;

#if OSC_DEBUG
    arh->m_put("DEBUG",MESS_DEBUG,"Thread <%d>!",getpid() );
#endif	

    arh->m_mess_r_stat = true;
    arh->m_mess_r_endrun = false;
    
    while( !quit )
    {	 
        if( arh->m_mess_r_endrun ) quit = true;
	if( ++i_cnt > arh->m_mess_per*1000/STD_WAIT_DELAY || quit )
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
			    vector<string> categ;
			    arh->mess_at(m_hd).Categ(categ);
			    for( i_cat = 0; i_cat < categ.size(); i_cat++ )
				if( categ[i_cat] == i_mess[i_m].categ ) break;
			    if( (i_cat < categ.size() || !categ.size()) 
				&& i_mess[i_m].level >= arh->mess_at(m_hd).Level() ) 
			    {
				i_mess[i_m].mess.insert(0,SYS->Station()+":");
				o_mess.push_back(i_mess[i_m]);
			    }
			}
			if( o_mess.size() ) 
			    try{ arh->mess_at(m_hd).put(o_mess); } catch(...){ }
			arh->mess_det(m_hd);
		    }
		}
    	    }
    	    catch(TError err){ arh->m_put_s("SYS",MESS_ERR,err.what()); }
	}	
	usleep(STD_WAIT_DELAY*1000);
    }
    
    arh->m_mess_r_stat = false;
    
    return(NULL);
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

//================== Controll functions ========================
void TArhiveS::ctr_fill_info( XMLNode *inf )
{
    char *dscr = "dscr";
    
    TGRPModule::ctr_fill_info( inf );    
    XMLNode *n_add = inf->add_child();
    n_add->load_xml(i_cntr);
    n_add->set_attr(dscr,Mess->I18N("Subsystem control"));
    n_add->get_child(0)->set_attr(dscr,Mess->I18N("Message BD (module:bd:table)"));
    n_add->get_child(3)->set_attr(dscr,Mess->I18N("Value BD (module:bd:table)"));
    n_add->get_child(6)->set_attr(dscr,Mess->I18N("Period reading new messages"));
    n_add->get_child(7)->set_attr(dscr,Mess->I18N("Options help"));
    n_add->get_child(8)->set_attr(dscr,Mess->I18N("Load BD"));
    n_add->get_child(9)->set_attr(dscr,Mess->I18N("Update BD"));
}

void TArhiveS::ctr_din_get_( string a_path, XMLNode *opt )
{
    vector<string> list;
    
    TGRPModule::ctr_din_get_( a_path, opt );
    
    string t_id = ctr_path_l(a_path,0);
    if( t_id == "a_bd" )
    {
	t_id = ctr_path_l(a_path,1);
	if( t_id == "t_bdm" )      ctr_opt_setS( opt, m_bd_mess.tp );
	else if( t_id == "bdm" )   ctr_opt_setS( opt, m_bd_mess.bd );
	else if( t_id == "tblm" )  ctr_opt_setS( opt, m_bd_mess.tbl );
	else if( t_id == "t_bdv" ) ctr_opt_setS( opt, m_bd_val.tp );
	else if( t_id == "bdv" )   ctr_opt_setS( opt, m_bd_val.bd );
	else if( t_id == "tblv" )  ctr_opt_setS( opt, m_bd_val.tbl );
	else if( t_id == "b_mod" )
	{
	    Owner().BD().gmd_list(list);
	    for( unsigned i_a=0; i_a < list.size(); i_a++ )
		ctr_opt_setS( opt, list[i_a], i_a );
	}
	else if( t_id == "m_per" )  ctr_opt_setI( opt, m_mess_per );
	else if( t_id == "g_help" ) ctr_opt_setS( opt, opt_descr() );       
    }
}

void TArhiveS::ctr_din_set_( string a_path, XMLNode *opt )
{
    TGRPModule::ctr_din_set_( a_path, opt );
    
    string t_id = ctr_path_l(a_path,0);
    if( t_id == "a_bd" )
    {
	t_id = ctr_path_l(a_path,1);
	if( t_id == "t_bdm" )      m_bd_mess.tp   = ctr_opt_getS( opt );
	else if( t_id == "bdm" )   m_bd_mess.bd   = ctr_opt_getS( opt );
	else if( t_id == "tblm" )  m_bd_mess.tbl  = ctr_opt_getS( opt );
	else if( t_id == "t_bdv" ) m_bd_val.tp    = ctr_opt_getS( opt );
	else if( t_id == "bdv" )   m_bd_val.bd    = ctr_opt_getS( opt );
	else if( t_id == "tblv" )  m_bd_val.tbl   = ctr_opt_getS( opt );
	else if( t_id == "m_per" ) m_mess_per = ctr_opt_getI( opt );
    }   
}

void TArhiveS::ctr_cmd_go_( string a_path, XMLNode *fld, XMLNode *rez )
{
    TGRPModule::ctr_cmd_go_( a_path, fld, rez );
    
    string t_id = ctr_path_l(a_path,0);
    if( t_id == "a_bd" )
    {
	t_id = ctr_path_l(a_path,1);
	if( t_id == "load_bd" )      LoadBD();
	else if( t_id == "upd_bd" ) UpdateBD();
    }
}


//================================================================
//=========== TTipArhive =========================================
//================================================================
const char *TTipArhive::o_name = "TTipArhive";
const char *TTipArhive::i_cntr = 
    "<area id='a_arh'>"
    " <list id='mess' s_com='add,del' tp='br' mode='att'/>"
    " <list id='val' s_com='add,del' tp='br' mode='att'/>"
    "</area>";

TTipArhive::TTipArhive() : m_hd_mess(o_name), m_hd_val(o_name)
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

void TTipArhive::mess_add(string name )
{
    TArhiveMess *mess = AMess(name);
    try{ m_hd_mess.obj_add( mess, &mess->Name() ); }
    catch(TError err) {	delete mess; }
}

void TTipArhive::val_add(string name )
{
    TArhiveVal *val = AVal(name);
    try{ m_hd_val.obj_add( val, &val->Name() ); }
    catch(TError err) {	delete val; }    
}

//================== Controll functions ========================
void TTipArhive::ctr_fill_info( XMLNode *inf )
{
    char *dscr="dscr";
    
    TModule::ctr_fill_info( inf );    
    XMLNode *n_add = inf->add_child();
    n_add->load_xml(i_cntr);
    n_add->set_attr(dscr,Mess->I18Ns("Arhiv's type: ")+mod_Name());
    n_add->get_child(0)->set_attr(dscr,Mess->I18N("Message arhives"));
    n_add->get_child(1)->set_attr(dscr,Mess->I18N("Value arhives"));
}

void TTipArhive::ctr_din_get_( string a_path, XMLNode *opt )
{
    vector<string> list;
    
    TModule::ctr_din_get_( a_path, opt );

    if( ctr_path_l(a_path,0) == "a_arh" )
    {
	string t_id = ctr_path_l(a_path,1);
	if( t_id == "mess" )
	{
	    mess_list(list);
	    for( unsigned i_a=0; i_a < list.size(); i_a++ )
		ctr_opt_setS( opt, list[i_a], i_a ); 	
	}
	else if( t_id == "val" )
	{
	    val_list(list);
	    for( unsigned i_a=0; i_a < list.size(); i_a++ )
		ctr_opt_setS( opt, list[i_a], i_a ); 	
	}   
    }
}

void TTipArhive::ctr_din_set_( string a_path, XMLNode *opt )
{
    TModule::ctr_din_set_( a_path, opt );
    
    if( ctr_path_l(a_path,0) == "a_arh" )
    {
	string t_id = ctr_path_l(a_path,1);
	if( t_id == "mess" )
	    for( int i_el=0; i_el < opt->get_child_count(); i_el++)	    
	    {
		XMLNode *t_c = opt->get_child(i_el);
		if( t_c->get_name() == "el")
		{
		    if(t_c->get_attr("do") == "add")      mess_add(t_c->get_text());
		    else if(t_c->get_attr("do") == "del") mess_del(t_c->get_text());
		}
	    }
	else if( t_id == "val" )
	    for( int i_el=0; i_el < opt->get_child_count(); i_el++)	    
	    {
		XMLNode *t_c = opt->get_child(i_el);
		if( t_c->get_name() == "el")
		{
		    if(t_c->get_attr("do") == "add")      val_add(t_c->get_text());
		    else if(t_c->get_attr("do") == "del") val_del(t_c->get_text());
		}
	    }
    }
}

unsigned TTipArhive::ctr_att( string a_path )
{
    if( ctr_path_l(a_path,0) == "a_arh" )
    {
	string t_id = ctr_path_l(a_path,1);
	if( t_id == "mess" )     return(mess_att(ctr_path_l(a_path,2)));
	else if( t_id == "val" ) return(val_att(ctr_path_l(a_path,2)));
    }
    throw TError("(%s) Branch %s error",o_name,a_path.c_str());
}

void TTipArhive::ctr_det( string a_path, unsigned hd )
{
    if( ctr_path_l(a_path,0) == "a_arh" )	    
    {
	string t_id = ctr_path_l(a_path,1);
	if( t_id == "mess" )     { mess_det(hd); return; }
	else if( t_id == "val" ) { val_det(hd);  return; }
    }
    throw TError("(%s) Branch %s error",o_name,a_path.c_str());
}

TContr &TTipArhive::ctr_at( string a_path, unsigned hd )
{
    if( ctr_path_l(a_path,0) == "a_arh" )
    {
	string t_id = ctr_path_l(a_path,1);
	if( t_id == "mess" )     return(mess_at(hd));
	else if( t_id == "val" ) return(val_at(hd));
    }
    throw TError("(%s) Branch %s error",o_name,a_path.c_str());
}

//================================================================
//=========== TArhiveMess ========================================
//================================================================
const char *TArhiveMess::o_name = "TArhiveMess";
const char *TArhiveMess::i_cntr = 
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

TArhiveMess::TArhiveMess(string name, TTipArhive *owner) : 
    m_owner(owner), TConfig( &((TArhiveS &)owner->Owner()).messE() ), run_st(false),
    m_name(cfg("NAME").getS()), m_lname(cfg("DESCR").getS()), m_addr(cfg("ADDR").getS()), 
    m_cat_o(cfg("CATEG").getS()), m_level(cfg("LEVEL").getI()) ,m_start(cfg("START").getB())
{     
    m_name = name;
    cfg("MODUL").setS(Owner().mod_Name());
};

TArhiveMess::~TArhiveMess()
{

}

void TArhiveMess::Load( )
{
    TBDS &bds  = Owner().Owner().Owner().BD();
    SHDBD t_hd = bds.open( ((TArhiveS &)Owner().Owner()).messB() );	
    cfLoadValBD("NAME",bds.at(t_hd));
    bds.close(t_hd);
}

void TArhiveMess::Save( )
{
    TBDS &bds  = Owner().Owner().Owner().BD();
    SHDBD t_hd = bds.open( ((TArhiveS &)Owner().Owner()).messB() );	
    cfSaveValBD("NAME",bds.at(t_hd));
    bds.at(t_hd).Save(); 
    bds.close(t_hd);
}

void TArhiveMess::Categ( vector<string> &list )
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
void TArhiveMess::ctr_fill_info( XMLNode *inf )
{
    char *dscr = "dscr";
    
    inf->load_xml( i_cntr );
    inf->set_text(Mess->I18N("Message arhive: ")+Name());
    XMLNode *c_nd = inf->get_child(0);
    c_nd->set_attr(dscr,Mess->I18N("Parameters"));
    c_nd->get_child(0)->set_attr(dscr,cfg("DESCR").fld().descr());
    c_nd->get_child(1)->set_attr(dscr,cfg("ADDR").fld().descr());
    c_nd->get_child(2)->set_attr(dscr,cfg("LEVEL").fld().descr());
    c_nd->get_child(3)->set_attr(dscr,cfg("CATEG").fld().descr());
    c_nd->get_child(4)->set_attr(dscr,Mess->I18N("To start"));
    c_nd->get_child(5)->set_attr(dscr,Mess->I18N("Runing"));
    c_nd->get_child(6)->set_attr(dscr,Mess->I18N("Load arhive"));
    c_nd->get_child(7)->set_attr(dscr,Mess->I18N("Save arhive"));
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

void TArhiveMess::ctr_din_get_( string a_path, XMLNode *opt )
{
    string t_id = ctr_path_l(a_path,0);    
    if( t_id == "a_prm" )
    {
    	t_id = ctr_path_l(a_path,1);
    	if( t_id == "name" )       ctr_opt_setS( opt, m_name );
    	else if( t_id == "dscr" )  ctr_opt_setS( opt, m_lname );
    	else if( t_id == "addr" )  ctr_opt_setS( opt, m_addr );
    	else if( t_id == "lvl" )   ctr_opt_setI( opt, m_level );
    	else if( t_id == "start" ) ctr_opt_setB( opt, m_start );
    	else if( t_id == "r_st" )  ctr_opt_setB( opt, run_st );
    	else if( t_id == "cats" )
	{
	    vector<string> list;
	    Categ(list);
	    for( int i_l=0; i_l < list.size(); i_l++)
       		ctr_opt_setS( opt, list[i_l], i_l );       
	}
    }
}

void TArhiveMess::ctr_din_set_( string a_path, XMLNode *opt )
{
    string t_id = ctr_path_l(a_path,0);    
    if( t_id == "a_prm" )
    {
    	t_id = ctr_path_l(a_path,1);
    	if( t_id == "name" )       m_name  = ctr_opt_getS( opt );
    	else if( t_id == "dscr" )  m_lname = ctr_opt_getS( opt );
    	else if( t_id == "addr" )  m_addr  = ctr_opt_getS( opt );
    	else if( t_id == "lvl" )   m_level = ctr_opt_getI( opt );
    	else if( t_id == "start" ) m_start = ctr_opt_getB( opt );
    	else if( t_id == "r_st" ) { if( ctr_opt_getB( opt ) ) start(); else stop(); }
    	else if( t_id == "cats" )
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
    }
}

void TArhiveMess::ctr_cmd_go_( string a_path, XMLNode *fld, XMLNode *rez )
{
    string t_id = ctr_path_l(a_path,0);
    if( t_id == "a_prm" )
    {
	t_id = ctr_path_l(a_path,1);
	if( t_id == "load" )      Load();
	else if( t_id == "save" ) Save();
    }
    else if( t_id == "a_mess")
    {
	t_id = ctr_path_l(a_path,1);
	if( t_id == "mess" )
	{
    	    t_id = ctr_path_l(a_path,2);
    	    if(t_id == "view" )     
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
	}
	if( t_id == "add" )
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
    }
}
//================================================================
//=========== TArhiveVal =========================================
//================================================================
const char *TArhiveVal::o_name = "TArhiveVal";
const char *TArhiveVal::i_cntr = 
    "<oscada_cntr>"
    "</oscada_cntr>";
 
TArhiveVal::TArhiveVal( string name, TTipArhive *owner ) : 
    m_owner(owner), TConfig(&((TArhiveS &)owner->Owner()).valE()),    
    m_name(cfg("NAME").getS()), m_bd(cfg("ADDR").getS())   
{    
    m_name = name;
    cfg("MODUL").setS(Owner().mod_Name());
}

TArhiveVal::~TArhiveVal()
{
    
}

//================== Controll functions ========================
void TArhiveVal::ctr_fill_info( XMLNode *inf )
{
    inf->load_xml( i_cntr );
    inf->set_text(string("Value arhive: ")+Name());
}

