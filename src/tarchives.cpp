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
TArchiveS::TArchiveS( TKernel *app ) : 
    TGRPModule(app,"Archive"), m_mess_r_stat(false), m_mess_per(2), 
    m_bd_mess("","","arh_mess"), m_bd_val("","","arh_val"),
    el_mess(""), el_val("")
{
    s_name = "Archives"; 
    
    //Message archive BD structure
    el_mess.fldAdd( new TFld("NAME",Mess->I18N("Name"),T_STRING|F_KEY,"20") );
    el_mess.fldAdd( new TFld("DESCR",Mess->I18N("Description"),T_STRING,"50") );
    el_mess.fldAdd( new TFld("MODUL",Mess->I18N("Module(plugin) name"),T_STRING,"20") );
    el_mess.fldAdd( new TFld("START",Mess->I18N("Start archive"),T_BOOL,"1") );
    el_mess.fldAdd( new TFld("CATEG",Mess->I18N("Message categories"),T_STRING,"20") );
    el_mess.fldAdd( new TFld("LEVEL",Mess->I18N("Message level"),T_DEC,"1","","0;7") );
    el_mess.fldAdd( new TFld("ADDR",Mess->I18N("Address"),T_STRING,"50") );
    
    //Value archive BD structure    
    el_val.fldAdd( new TFld("NAME",Mess->I18N("Name"),T_STRING|F_KEY,"20") );
    el_val.fldAdd( new TFld("DESCR",Mess->I18N("Description"),T_STRING,"50") );
    el_val.fldAdd( new TFld("MODUL",Mess->I18N("Module(plugin) name"),T_STRING,"20") );
    el_val.fldAdd( new TFld("START",Mess->I18N("Start archive"),T_BOOL,"1") );
    el_val.fldAdd( new TFld("ADDR",Mess->I18N("Address"),T_STRING,"50") );
}

TArchiveS::~TArchiveS(  )
{
    if( m_mess_r_stat )
    {
	m_mess_r_endrun = true;
	TSYS::eventWait( m_mess_r_stat, false, "Arhives' task is stoping....");	
	pthread_join( m_mess_pthr, NULL );
    }    
}

void TArchiveS::gmdInit( )
{
    loadBD();
}

string TArchiveS::optDescr(  )
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

TBDS::SName TArchiveS::messB() 
{ 
    return owner().nameDBPrep(m_bd_mess);
}

TBDS::SName TArchiveS::valB()  
{ 
    return owner().nameDBPrep(m_bd_val);
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
	    case 'h': fprintf(stdout,optDescr().c_str()); break;
	    case 'm': DirPath = optarg;     break;
	    case -1 : break;
	}
    } while(next_opt != -1);
}

void TArchiveS::gmdUpdateOpt()
{
    TGRPModule::gmdUpdateOpt();
    
    try{ DirPath = gmdCfgNode()->childGet("id","mod_path")->text(); }
    catch(...) {  }
    try{ m_mess_per = atoi( gmdCfgNode()->childGet("id","mess_period")->text().c_str() ); }
    catch(...) {  }
    try
    {
    	string opt = gmdCfgNode()->childGet("id","MessBD")->text(); 
	m_bd_mess.tp	= TSYS::strSepParse(opt,0,':');
	m_bd_mess.bd    = TSYS::strSepParse(opt,1,':');
	m_bd_mess.tbl   = TSYS::strSepParse(opt,2,':');	
    }
    catch(...) {  }
    try
    {
    	string opt = gmdCfgNode()->childGet("id","ValBD")->text(); 
	m_bd_val.tp	= TSYS::strSepParse(opt,0,':');
	m_bd_val.bd     = TSYS::strSepParse(opt,1,':');
	m_bd_val.tbl	= TSYS::strSepParse(opt,2,':');
    }
    catch(...) {  }
}

void TArchiveS::loadBD( )
{    
    string name,type;    
    
    //Load message archives
    try
    {    
	TConfig c_el(&el_mess);
	
	int fld_cnt = 0;
	AutoHD<TTable> tbl = owner().BD().open(messB());
	while( tbl.at().fieldSeek(fld_cnt++,c_el) )
	{
	    name = c_el.cfg("NAME").getS();
	    type = c_el.cfg("MODUL").getS();
	    
            AutoHD<TTipArchive> archs = gmdAt(type);
			
            if( !archs.at().messAvoid(name) )
	    {
		archs.at().messAdd(name);
	    	((TConfig &)archs.at().messAt(name).at()) = c_el;
	    }
	    else archs.at().messAt(name).at().load();
	}
	tbl.free();
	owner().BD().close(messB());	
    }catch( TError err ){ mPutS("SYS",MESS_ERR,err.what()); }            
}

void TArchiveS::saveBD( )
{
    vector<string> t_lst, o_lst;
    
    // Save messages bd  
    gmdList(t_lst);
    for( int i_t = 0; i_t < t_lst.size(); i_t++ )
    {
	AutoHD<TTipArchive> mod = gmdAt(t_lst[i_t]);
	mod.at().messList(o_lst);
	for( int i_o = 0; i_o < o_lst.size(); i_o++ )
	    mod.at().messAt(o_lst[i_o]).at().save();
    }    
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
	AutoHD<TTipArchive> mod = gmdAt(t_lst[i_t]);
	mod.at().messList(o_lst);
	for( int i_o = 0; i_o < o_lst.size(); i_o++ )
	{
	    AutoHD<TArchiveMess> mess = mod.at().messAt(o_lst[i_o]);
	    if( !mess.at().startStat() && mess.at().toStart() ) 
		mess.at().start();
	}
    }    
    // Self task start
    pthread_attr_init(&pthr_attr);
    pthread_attr_setschedpolicy(&pthr_attr,SCHED_OTHER);
    pthread_create(&m_mess_pthr,&pthr_attr,TArchiveS::MessArhTask,this);
    pthread_attr_destroy(&pthr_attr);
    if( TSYS::eventWait(m_mess_r_stat, true, "Arhives' task is starting....",5) )	
	throw TError("Arhives' task no started!");
}

void TArchiveS::gmdStop( )
{    
    vector<string> t_lst, o_lst;
    // Archives stop        
    gmdList(t_lst);
    for( int i_t = 0; i_t < t_lst.size(); i_t++ )
    {
	AutoHD<TTipArchive> mod = gmdAt(t_lst[i_t]);
	mod.at().messList(o_lst);
	for( int i_o = 0; i_o < o_lst.size(); i_o++ )
	{
	    AutoHD<TArchiveMess> mess = mod.at().messAt(o_lst[i_o]);
	    if( mess.at().startStat() ) mess.at().stop();
	}
    }    
    
    if( m_mess_r_stat )
    {
    	m_mess_r_endrun = true;
    	if( TSYS::eventWait(m_mess_r_stat, false, "Arhives' task is stoping....",5) )
	    throw TError("Arhives' task no stoped!");       
	pthread_join( m_mess_pthr, NULL );
    }
}

void *TArchiveS::MessArhTask(void *param)
{
    bool quit = false;
    int i_cnt = 0;
    TArchiveS &arh = *(TArchiveS *)param;
    vector<TMessage::SRec> i_mess, o_mess;    
    time_t t_last = 0, t_cur;

#if OSC_DEBUG
    arh.mPut("DEBUG",MESS_DEBUG,"Thread <%d>!",getpid() );
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
    				    i_mess[i_m].mess.insert(0,SYS->station()+":");
    				    o_mess.push_back(i_mess[i_m]);
    				}
    			    }
			    if( o_mess.size() ) mess.at().put(o_mess);
			}
		    }    
		}
    	    }
    	    catch(TError err){ arh.mPutS("SYS",MESS_ERR,err.what()); }
	}	
	usleep(STD_WAIT_DELAY*1000);
    }
    
    arh.m_mess_r_stat = false;
    
    return(NULL);
}

//================== Controll functions ========================
void TArchiveS::cntrCmd_( const string &a_path, XMLNode *opt, int cmd )
{
    XMLNode *el;
    vector<string> list;

    if( cmd==TCntrNode::Info )
    {	
	ctrInsNode("area",0,opt,a_path,"/bd",Mess->I18N("Subsystem"),0440);
	if( owner().genDB( ) )
        {		    
	    ctrMkNode("fld",opt,a_path,"/bd/tblm",Mess->I18N("Message table"),0660,0,0,"str");
	    ctrMkNode("fld",opt,a_path,"/bd/tblv",Mess->I18N("Value table"),0660,0,0,"str");
	}
	else
	{
	    ctrMkNode("fld",opt,a_path,"/bd/t_bdm",Mess->I18N("Message BD (module:bd:table)"),0660,0,0,"str")->
		attr("dest","select")->attr("select","/bd/b_mod");
	    ctrMkNode("fld",opt,a_path,"/bd/bdm","",0660,0,0,"str");
	    ctrMkNode("fld",opt,a_path,"/bd/tblm","",0660,0,0,"str");
	    ctrMkNode("fld",opt,a_path,"/bd/t_bdv",Mess->I18N("Value BD (module:bd:table)"),0660,0,0,"str")->
		attr("dest","select")->attr("select","/bd/b_mod");
	    ctrMkNode("fld",opt,a_path,"/bd/bdv","",0660,0,0,"str");
	    ctrMkNode("fld",opt,a_path,"/bd/tblv","",0660,0,0,"str");
	}
	ctrMkNode("fld",opt,a_path,"/bd/m_per",Mess->I18N("Period reading new messages"),0660,0,0,"dec");
	ctrMkNode("comm",opt,a_path,"/bd/load_bd",Mess->I18N("Load from BD"));
	ctrMkNode("comm",opt,a_path,"/bd/upd_bd",Mess->I18N("Save to BD"));
	ctrMkNode("fld",opt,a_path,"/help/g_help",Mess->I18N("Options help"),0440,0,0,"str")->
	    attr("cols","90")->attr("rows","5");	    
    }    
    else if( cmd==TCntrNode::Get )
    {
	if( a_path == "/bd/t_bdm" )	{ ctrSetS( opt, m_bd_mess.tp ); return; }
    	if( a_path == "/bd/bdm" )	{ ctrSetS( opt, m_bd_mess.bd ); return; }
	if( a_path == "/bd/tblm" )	{ ctrSetS( opt, m_bd_mess.tbl ); return; }
    	if( a_path == "/bd/t_bdv" )	{ ctrSetS( opt, m_bd_val.tp ); return; }
    	if( a_path == "/bd/bdv" )	{ ctrSetS( opt, m_bd_val.bd ); return; }
    	if( a_path == "/bd/tblv" )	{ ctrSetS( opt, m_bd_val.tbl ); return; }
    	if( a_path == "/bd/b_mod" )
	{
	    opt->childClean();
	    owner().BD().gmdList(list);
	    ctrSetS( opt, "" );	//Default DB
	    for( unsigned i_a=0; i_a < list.size(); i_a++ )
		ctrSetS( opt, list[i_a] );
	    return;
	}
	if( a_path == "/bd/m_per" )	{ ctrSetI( opt, m_mess_per ); return; }
    	if( a_path == "/help/g_help" )	{ ctrSetS( opt, optDescr() ); return; }
    }
    else if( cmd==TCntrNode::Set )
    {
	if( a_path == "/bd/t_bdm" )	{ m_bd_mess.tp = ctrGetS( opt ); return; }
   	if( a_path == "/bd/bdm" )	{ m_bd_mess.bd = ctrGetS( opt ); return; }
    	if( a_path == "/bd/tblm" )	{ m_bd_val.tbl = ctrGetS( opt ); return; }
    	if( a_path == "/bd/t_bdv" )	{ m_bd_val.tp = ctrGetS( opt ); return; }
    	if( a_path == "/bd/bdv" )	{ m_bd_val.bd = ctrGetS( opt ); return; }
    	if( a_path == "/bd/tblv" )	{ m_bd_val.tbl = ctrGetS( opt ); return; }
    	if( a_path == "/bd/m_per" )	{ m_mess_per = ctrGetI( opt ); return; }
    	if( a_path == "/bd/load_bd" )	{ loadBD(); return; }
    	if( a_path == "/bd/upd_bd" )   	{ saveBD(); return; }
    }
    TGRPModule::cntrCmd_( a_path, opt, cmd );	
}

//================================================================
//=========== TTipArchive =========================================
//================================================================
TTipArchive::TTipArchive()
{
    m_mess = grpAdd();
    m_val = grpAdd();
}
    
TTipArchive::~TTipArchive()
{
    delAll();
}

void TTipArchive::messAdd(const string &name )
{
    if( chldAvoid(m_mess,name) ) return;
    chldAdd(m_mess,AMess(name));
}

void TTipArchive::valAdd( const string &name )
{
    if( chldAvoid(m_val,name) ) return;
    chldAdd(m_val,AVal(name));
}
	
//================== Controll functions ========================
void TTipArchive::ctrStat_( XMLNode *inf )
{
    char *dscr="dscr";
    
    TModule::ctrStat_( inf );    
    
    char *i_cntr = 
	"<area id='arch'>"
	" <list id='mess' s_com='add,del' tp='br' mode='att' br_pref='_mess_'/>"
	" <list id='val' s_com='add,del' tp='br' mode='att' br_pref='_val_'/>"
	"</area>";
    
    XMLNode *n_add = inf->childIns(0);
    n_add->load(i_cntr);
    n_add->attr(dscr,Mess->I18N("Archives"));
    n_add->childGet(0)->attr(dscr,Mess->I18N("Message archives"));
    n_add->childGet(1)->attr(dscr,Mess->I18N("Value archives"));
}

void TTipArchive::ctrDinGet_( const string &a_path, XMLNode *opt )
{
    vector<string> list;    

    if( a_path == "/arch/mess" )
    {
	opt->childClean();
	messList(list);
	for( unsigned i_a=0; i_a < list.size(); i_a++ )
	    ctrSetS( opt, list[i_a] ); 	
    }
    else if( a_path == "/arch/val" )
    {
	opt->childClean();
	valList(list);
	for( unsigned i_a=0; i_a < list.size(); i_a++ )
	    ctrSetS( opt, list[i_a] ); 	
    }   
    else TModule::ctrDinGet_( a_path, opt );
}

void TTipArchive::ctrDinSet_( const string &a_path, XMLNode *opt )
{
    if( a_path.substr(0,10) == "/arch/mess" )
	for( int i_el=0; i_el < opt->childSize(); i_el++)	    
	{
	    XMLNode *t_c = opt->childGet(i_el);
	    if( t_c->name() == "el")
	    {
		if(t_c->attr("do") == "add")     	messAdd(t_c->text());
		else if(t_c->attr("do") == "del")	chldDel(m_mess,t_c->text(),-1,1);
	    }
	}
    else if( a_path.substr(0,9) == "/arch/val" )
	for( int i_el=0; i_el < opt->childSize(); i_el++)	    
	{
	    XMLNode *t_c = opt->childGet(i_el);
	    if( t_c->name() == "el")
	    {
		if(t_c->attr("do") == "add")      valAdd(t_c->text());
		else if(t_c->attr("do") == "del") valDel(t_c->text());
	    }
	}
    else TModule::ctrDinSet_( a_path, opt );
}

AutoHD<TCntrNode> TTipArchive::ctrAt1( const string &a_path )
{
    if( a_path.substr(0,6) == "_mess_" )	return messAt(pathEncode(a_path.substr(6),true));
    else if( a_path.substr(0,5) == "_val_" )	return valAt(pathEncode(a_path.substr(5),true));
    else return TModule::ctrAt1(a_path);
}


//================================================================
//=========== TArchiveMess ========================================
//================================================================
TArchiveMess::TArchiveMess(const string &name, TTipArchive *n_owner) : 
    m_owner(n_owner), TConfig( &((TArchiveS &)n_owner->owner()).messE() ), run_st(false), m_beg(time(NULL)), m_end(time(NULL)), m_lvl(0),
    m_name(cfg("NAME").getS()), m_lname(cfg("DESCR").getS()), m_addr(cfg("ADDR").getS()), 
    m_cat_o(cfg("CATEG").getS()), m_level(cfg("LEVEL").getI()) ,m_start(cfg("START").getB())
{     
    m_name = name;
    cfg("MODUL").setS(owner().modName());
};

TArchiveMess::~TArchiveMess()
{

}

void TArchiveMess::postDisable(int flag)
{
    try
    {
        if( flag )
        {
	    TBDS &bds = owner().owner().owner().BD();
    	    bds.open(((TArchiveS &)owner().owner()).messB()).at().fieldDel(*this);
    	    bds.close(((TArchiveS &)owner().owner()).messB());
	}
    }catch(TError err)
    { owner().mPut("SYS",MESS_ERR,"%s",err.what().c_str()); }
}																				

void TArchiveMess::load( )
{
    TBDS &bd = owner().owner().owner().BD();
    bd.open(((TArchiveS &)owner().owner()).messB()).at().fieldGet(*this);
    bd.close(((TArchiveS &)owner().owner()).messB());
}

void TArchiveMess::save( )
{
    TBDS &bd = owner().owner().owner().BD();
    bd.open(((TArchiveS &)owner().owner()).messB(),true).at().fieldSet(*this);
    bd.close(((TArchiveS &)owner().owner()).messB());
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
void TArchiveMess::ctrStat_( XMLNode *inf )
{
    char *i_cntr = 
    	"<oscada_cntr>"
	 "<area id='prm'>"
	  "<area id='st'>"
	   "<fld id='st' acs='0664' tp='bool'/>"
	  "</area>"
	  "<area id='cfg'>"	  
	   "<fld id='dscr' acs='0664' tp='str'/>"
	   "<fld id='addr' acs='0664' tp='str' dest='dir'/>"
	   "<fld id='lvl'  acs='0664' tp='dec'/>"
	   "<list id='cats' acs='0664' tp='str' s_com='add,del'/>"
	   "<fld id='start' acs='0664' tp='bool'/>"
	   "<comm id='load' acs='0550'/>"
	   "<comm id='save' acs='0550'/>"    
	  "</area>"
	 "</area>"
	 "<area id='mess'>"
	  "<comm id='add'>"
	   "<fld id='tm' tp='time'/>"
	   "<fld id='cat' tp='str'/>"
	   "<fld id='lvl' tp='dec' min='0' max='7'/>"
	   "<fld id='mess' tp='str'/>"
	  "</comm>"
	  "<fld id='v_beg' tp='time'/>"
	  "<fld id='v_end' tp='time'/>"
	  "<fld id='v_cat' tp='str'/>"
	  "<fld id='v_lvl' tp='dec' min='0' max='7'/>"
	  "<table id='mess' acs='0440'>"
	   "<list id='0' tp='time'/>"
	   "<list id='1' tp='str'/>"
	   "<list id='2' tp='dec'/>"
	   "<list id='3' tp='str'/>"
	  "</table>"
	 "</area>"
	"</oscada_cntr>";
    char *dscr = "dscr";
    
    inf->load( i_cntr );
    inf->text(Mess->I18N("Message archive: ")+name());
    XMLNode *c_nd = inf->childGet(0);
    c_nd->attr(dscr,Mess->I18N("Archive"));
    c_nd->childGet(0)->attr(dscr,Mess->I18N("State"));
    c_nd->childGet(0)->childGet(0)->attr(dscr,Mess->I18N("Runing"));    
    c_nd = c_nd->childGet(1);
    c_nd->attr(dscr,Mess->I18N("Config"));
    c_nd->childGet(0)->attr(dscr,cfg("DESCR").fld().descr());
    c_nd->childGet(1)->attr(dscr,cfg("ADDR").fld().descr());
    c_nd->childGet(2)->attr(dscr,cfg("LEVEL").fld().descr());
    c_nd->childGet(3)->attr(dscr,cfg("CATEG").fld().descr());
    c_nd->childGet(4)->attr(dscr,Mess->I18N("To start"));
    c_nd->childGet(5)->attr(dscr,Mess->I18N("Load from BD"));
    c_nd->childGet(6)->attr(dscr,Mess->I18N("Save to BD"));
    if( !run_st ) inf->childDel(1);
    else
    {
	c_nd = inf->childGet(1);
	c_nd->attr(dscr,Mess->I18N("Messages"));
	c_nd->childGet(1)->attr(dscr,Mess->I18N("Begin"));
	c_nd->childGet(2)->attr(dscr,Mess->I18N("End"));
	c_nd->childGet(3)->attr(dscr,Mess->I18N("Category"));
	c_nd->childGet(4)->attr(dscr,Mess->I18N("Level"));
	XMLNode *c_nd1 = c_nd->childGet(5);
	    c_nd1->attr(dscr,Mess->I18N("Messages"));
	    c_nd1->childGet(0)->attr(dscr,Mess->I18N("Time"));
	    c_nd1->childGet(1)->attr(dscr,Mess->I18N("Category"));
	    c_nd1->childGet(2)->attr(dscr,Mess->I18N("Level"));
	    c_nd1->childGet(3)->attr(dscr,Mess->I18N("Message"));	
	c_nd1 = c_nd->childGet(0);
	    c_nd1->attr(dscr,Mess->I18N("Add"));
	    c_nd1->childGet(0)->attr(dscr,Mess->I18N("Time"));
	    c_nd1->childGet(1)->attr(dscr,Mess->I18N("Category"));
	    c_nd1->childGet(2)->attr(dscr,Mess->I18N("Level"));
	    c_nd1->childGet(3)->attr(dscr,Mess->I18N("Message"));
    }
}

void TArchiveMess::ctrDinGet_( const string &a_path, XMLNode *opt )
{
    if( a_path == "/prm/st/st" )		ctrSetB( opt, run_st );
    else if( a_path == "/prm/cfg/name" )	ctrSetS( opt, m_name );
    else if( a_path == "/prm/cfg/dscr" )	ctrSetS( opt, m_lname );
    else if( a_path == "/prm/cfg/addr" )  	ctrSetS( opt, m_addr );
    else if( a_path == "/prm/cfg/lvl" )   	ctrSetI( opt, m_level );
    else if( a_path == "/prm/cfg/start" ) 	ctrSetB( opt, m_start );
    else if( a_path == "/prm/cfg/cats" )
    {
	opt->childClean();
	vector<string> list;
	categ(list);
	for( int i_l=0; i_l < list.size(); i_l++)
	    ctrSetS( opt, list[i_l] );       
    }
    else if( a_path == "/mess/v_beg" )	ctrSetI( opt, m_beg );
    else if( a_path == "/mess/v_end" )	ctrSetI( opt, m_end );
    else if( a_path == "/mess/v_cat" )	ctrSetS( opt, m_cat );
    else if( a_path == "/mess/v_lvl" )	ctrSetI( opt, m_lvl );
    else if( a_path == "/mess/mess" )
    {
	vector<TMessage::SRec> rec;
	get(  m_beg, m_end, rec, m_cat, m_lvl );
	
	XMLNode *n_tm   = ctrId(opt,"0");
	XMLNode *n_cat  = ctrId(opt,"1");
	XMLNode *n_lvl  = ctrId(opt,"2");
	XMLNode *n_mess = ctrId(opt,"3");
	for( int i_rec = 0; i_rec < rec.size(); i_rec++)
	{
	    ctrSetI(n_tm,rec[i_rec].time);
	    ctrSetS(n_cat,rec[i_rec].categ);
	    ctrSetI(n_lvl,rec[i_rec].level);
	    ctrSetS(n_mess,rec[i_rec].mess);
	}        
    }    
    else throw TError("(%s) Branch %s error!",__func__,a_path.c_str());
}

void TArchiveMess::ctrDinSet_( const string &a_path, XMLNode *opt )
{
    if( a_path == "/prm/st/st" )		{ if( ctrGetB( opt ) ) start(); else stop(); }
    else if( a_path == "/prm/cfg/name" )       	m_name  = ctrGetS( opt );
    else if( a_path == "/prm/cfg/dscr" )  	m_lname = ctrGetS( opt );
    else if( a_path == "/prm/cfg/addr" )  	m_addr  = ctrGetS( opt );
    else if( a_path == "/prm/cfg/lvl" )   	m_level = ctrGetI( opt );
    else if( a_path == "/prm/cfg/start" ) 	m_start = ctrGetB( opt );
    else if( a_path == "/prm/cfg/cats" )
	for( int i_el=0; i_el < opt->childSize(); i_el++)	    
	{
	    XMLNode *t_c = opt->childGet(i_el);
	    if( t_c->name() == "el")
	    {
		if(t_c->attr("do") == "add")
		{
		    if( m_cat_o.size() ) m_cat_o = m_cat_o+";";
		    m_cat_o = m_cat_o+t_c->text();
		}
		else if(t_c->attr("do") == "del") 
		{
		    int pos = m_cat_o.find(string(";")+t_c->text(),0);
		    if(pos != string::npos) 
			m_cat_o.erase(pos,t_c->text().size()+1);
		    else                    
			m_cat_o.erase(m_cat_o.find(t_c->text(),0),t_c->text().size()+1);
		}
	    }
	}
    else if( a_path == "/prm/cfg/load" )	load();
    else if( a_path == "/prm/cfg/save" )	save();
    else if( a_path == "/mess/v_beg" )		m_beg = ctrGetI(opt);
    else if( a_path == "/mess/v_end" )  	m_end = ctrGetI(opt);
    else if( a_path == "/mess/v_cat" )  	m_cat = ctrGetS(opt);
    else if( a_path == "/mess/v_lvl" )  	m_lvl = ctrGetI(opt);
    else if( a_path == "/mess/add" )
    {
	vector<TMessage::SRec> brec;
	TMessage::SRec rec;
	rec.time  = ctrGetI(ctrId(opt,"tm"));
	rec.categ = ctrGetS(ctrId(opt,"cat"));
	rec.level = ctrGetI(ctrId(opt,"lvl"));
	rec.mess  = ctrGetS(ctrId(opt,"mess"));
        brec.push_back(rec);
	put(brec);
    }												 
    else throw TError("(%s) Branch %s error!",__func__,a_path.c_str());
}

//================================================================
//=========== TArchiveVal =========================================
//================================================================
TArchiveVal::TArchiveVal( const string &name, TTipArchive *n_owner ) : 
    m_owner(n_owner), TConfig(&((TArchiveS &)n_owner->owner()).valE()),    
    m_name(cfg("NAME").getS()), m_bd(cfg("ADDR").getS())   
{    
    m_name = name;
    cfg("MODUL").setS(owner().modName());
}

TArchiveVal::~TArchiveVal()
{
    
}

//================== Controll functions ========================
void TArchiveVal::ctrStat_( XMLNode *inf )
{
    char *i_cntr = 
    	"<oscada_cntr>"
	"</oscada_cntr>";
    
    inf->load( i_cntr );
    inf->text(string("Value archive: ")+name());
}

