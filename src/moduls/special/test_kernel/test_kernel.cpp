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

#include <getopt.h>
#include <signal.h>
#include <string>

#include <sys/times.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>

#include <tsys.h>
#include <tkernel.h>
#include <tmodschedul.h>
#include <tmessage.h>
#include <tparams.h>
#include <tparam.h>
#include <tparamcontr.h>
#include <tcontrollers.h>
#include <ttransports.h>
#include <tarchives.h>
#include "test_kernel.h"

//============ Modul info! =====================================================
#define MOD_ID      "test_kernel"
#define MOD_NAME    "Test kernel"
#define MOD_TYPE    "Special"
#define VER_TYPE    VER_SPC
#define SUB_TYPE    "TEST"
#define VERSION     "1.0.0"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "OpenScada Kernel test module: Configs, Values ... ."
#define LICENSE     "GPL"
//==============================================================================

extern "C"
{
    TModule::SAt module( int n_mod )
    {
	TModule::SAt AtMod;

	if(n_mod==0)
	{
	    AtMod.id	= MOD_ID;
	    AtMod.type  = MOD_TYPE;
	    AtMod.t_ver = VER_TYPE;
	}
    	else
	    AtMod.id	= "";

	return( AtMod );
    }

    TModule *attach( const TModule::SAt &AtMod, const string &source )
    {
	KernelTest::TTest *self_addr = NULL;

    	if( AtMod.id == MOD_ID && AtMod.type == MOD_TYPE && AtMod.t_ver == VER_TYPE )
	    self_addr = new KernelTest::TTest( source );       

	return ( self_addr );
    }
}

using namespace KernelTest;

//==============================================================================
//================= BDTest::TTest ==============================================
//==============================================================================
TTest::TTest( string name )
{
    mId 	= MOD_ID;
    mName       = MOD_NAME;
    mType  	= MOD_TYPE;
    Vers      	= VERSION;
    Autors    	= AUTORS;
    DescrMod  	= DESCRIPTION;
    License   	= LICENSE;
    Source    	= name;
}

TTest::~TTest()
{
    if( run_st ) stop();
}

string TTest::modInfo( const string &name )
{
    if( name == "SubType" ) return(SUB_TYPE);
    else return( TModule::modInfo( name) );
}

void TTest::modInfo( vector<string> &list )
{
    TModule::modInfo(list);
    list.push_back("SubType");
}

string TTest::optDescr( )
{
    char buf[STR_BUF_LEN];

    snprintf(buf,sizeof(buf),I18N(
    	"======================= The module <%s:%s> options =======================\n"
	"---------- Parameters of the module section <%s> in config file ----------\n"
	"XML          XML parsed and create test:\n"
	"  file         file for XML parsing;\n"
	"MESS         Message test:\n"
	"  arh          archive name;\n"
	"  t_arh        type archive name;\n"
	"  categ        message category;\n"
	"PARAM        Parameter test:\n"
	"  name         parameter name;\n"
	"SOAttDet     Attach/Detach module test:\n"
	"  name         name modul;\n"
	"  full         full attach(starting);\n"
	"Controll     Controll test.\n"
	"Val          Parameter atributes value test:\n"
	"  name         Parameter and his atribut;\n"
	"BD           Full database test:\n"
	"  type         type BD;\n"
	"  bd           BD name;\n"
	"  table        table;\n"
	"  size         test size;\n"
	"TrOut        Output transport test:\n"
	"  addr         address a input transport;\n"
	"  type         type transport;\n"
	"  req          request to a input transport;\n\n"),
	MOD_TYPE,MOD_ID,MOD_ID);
    
    return(buf);
}			

void TTest::modCheckCommandLine(  )
{
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{"help"    ,0,NULL,'h'},
	{NULL      ,0,NULL,0  }
    };

    optind=opterr=0;
    do
    {
	next_opt=getopt_long(SYS->argc,(char * const *)SYS->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': fprintf(stdout,optDescr().c_str()); break;
	    case -1 : break;
	}
    } while(next_opt != -1);
}

void TTest::modUpdateOpt( )
{

}

void TTest::start(  )
{
    if( run_st ) return;
    pthread_attr_t pthr_attr;

    pthread_attr_init(&pthr_attr);
    pthread_attr_setschedpolicy(&pthr_attr,SCHED_OTHER);
    pthread_create(&pthr_tsk,&pthr_attr,Task,this);
    pthread_attr_destroy(&pthr_attr);
    if( SYS->event_wait( run_st, true, string(MOD_ID)+": Is starting....",5) )
	throw TError("%s: No started!",MOD_ID);
}

void TTest::stop(  )
{
    if( !run_st ) return;

    endrun = true;
    if( SYS->event_wait( run_st, false, string(MOD_ID)+": Is stoping....",5) )
	throw TError("%s: No stoped!",MOD_ID);
    pthread_join( pthr_tsk, NULL );
}

void *TTest::Task( void *CfgM )
{
    int count = 0, i_cnt = 0;

    TTest *tst = (TTest *)CfgM;
    tst->run_st = true;
    tst->endrun = false;
    
#if OSC_DEBUG
    tst->owner().mPut("DEBUG",MESS_DEBUG,"%s:Thread <%d>!",MOD_ID,getpid() );
#endif

    //Task counter
    while( !tst->endrun )
    {		
	//1 sec
	if( ++i_cnt > 1000/STD_WAIT_DELAY )  // 1 sec
	{
	    i_cnt = 0;
            if( ++count == 1000000 ) count = 0;
	    
	    //Get All fields
	    try
	    {
		int nd_cnt = 0;
    		while(true)
    		{
		    XMLNode *t_n = tst->modCfgNode()->childGet("prm",nd_cnt++);
		    
		    if( t_n->attr("on") == "1" && atoi(t_n->attr("per").c_str()) && 
			    !( count % atoi(t_n->attr("per").c_str()) ) )
		    {
			string id = t_n->attr("id");		    
		    	try{ tst->Test( id, t_n ); }
			catch( TError error )
			{ Mess->put_s(string("TEST:")+id+":ERROR",MESS_ERR,error.what()); }
		    }
		}
	    }catch(...){ }	    
	}
	usleep(STD_WAIT_DELAY*1000);
    }
    tst->run_st = false;
}

void TTest::Test( const string &id, XMLNode *t_n )
{
    string test_cat = "TEST:"+id;
    
    //Parameter config test
    if(id == "PARAM" )
    {
	TParamS &param = owner().owner().Param();
	
	AutoHD<TParam> prm = param.at( t_n->attr("name") );
	Mess->put(test_cat,MESS_INFO,"-------- Start parameter <%s> test ----------",t_n->attr("name").c_str());
    
	vector<string> list_el;
	prm.at().at().cfgList(list_el);
	Mess->put(test_cat,MESS_INFO,"Config elements avoid: %d",list_el.size());
	for(unsigned i=0; i< list_el.size(); i++)
	    Mess->put(test_cat,MESS_INFO,"Element: %s",list_el[i].c_str());		    
	    
	Mess->put(test_cat,MESS_INFO,"Value elements avoid: %d",list_el.size());
	//prm.vlSetR(0,30);
	list_el.clear();
	prm.at().at().vlList(list_el);
	for(unsigned i=0; i< list_el.size(); i++)
	{
	    AutoHD<TVal> val = prm.at().at().vlAt(list_el[i]);
	    if( val.at().fld().type()&T_SELECT )
		Mess->put(test_cat,MESS_INFO,"Element (SELECT): %s: %s",list_el[i].c_str(), val.at().getSEL().c_str() );
	    else if( val.at().fld().type()&T_STRING )
		Mess->put(test_cat,MESS_INFO,"Element (STRING): %s: %s",list_el[i].c_str(), val.at().getS().c_str() );
	    else if( val.at().fld().type()&T_REAL )
		Mess->put(test_cat,MESS_INFO,"Element (REAL): %s: %f",list_el[i].c_str(), val.at().getR() );
	    else if( val.at().fld().type()&T_BOOL )
		Mess->put(test_cat,MESS_INFO,"Element (BOOLEAN): %s: %d",list_el[i].c_str(), val.at().getB() );
	    else Mess->put(test_cat,MESS_INFO,"Element (INTEGER): %s: %d",list_el[i].c_str(), val.at().getI() );
	}
		
	Mess->put(test_cat,MESS_INFO,"Configs throw control: %d",list_el.size());
	    
	XMLNode node;
	prm.at().at().ctrStat(node);
	pr_XMLNode( &node, 0 );

	Mess->put(test_cat,MESS_INFO,"-------- Stop parameter <%s> test ----------",t_n->attr("name").c_str());
    }
    
    //XML parsing test
    else if(id == "XML" )
    {
	int hd = open(t_n->attr("file").c_str(),O_RDONLY);
	if(hd > 0)
	{
	    Mess->put_s(test_cat,MESS_INFO,"-------- Start TEST XML parsing ----------");
	    int cf_sz = lseek(hd,0,SEEK_END);
	    lseek(hd,0,SEEK_SET);
	    char *buf = (char *)malloc(cf_sz);
	    read(hd,buf,cf_sz);
	    close(hd);
	    string s_buf = buf;
	    free(buf);
	    XMLNode node;
	    node.load(s_buf);
	    pr_XMLNode( &node, 0 );
	    Mess->put_s(test_cat,MESS_INFO,"-------- End TEST XML parsing ----------");
	}
    }
    
    //Message arhive test
    else if(id == "MESS" )
    {
	TArchiveS &Arh_s = owner().owner().Archive();
		
	string n_arh = t_n->attr("arh");
	string t_arh = t_n->attr("t_arh");
	Mess->put(test_cat,MESS_INFO,"-------- Start Message buffer %s test ----------",n_arh.c_str());
	vector<TMessage::SRec> buf_rec;
	if( n_arh == "sys" ) Mess->get(0,time(NULL),buf_rec,t_n->attr("categ"));
	else		    
	    ((TTipArchive &)Arh_s.gmdAt(t_arh).at()).messAt(n_arh).at().get(0,time(NULL),buf_rec,t_n->attr("categ"));
	Mess->put(test_cat,MESS_INFO,"Messages avoid %d.",buf_rec.size() );
	for(unsigned i_rec = 0; i_rec < buf_rec.size(); i_rec++)
	{
	    char *c_tm = ctime( &buf_rec[i_rec].time);
	    for( int i_ch = 0; i_ch < strlen(c_tm); i_ch++ )
		if( c_tm[i_ch] == '\n' ) c_tm[i_ch] = '\0';
	    Mess->put(test_cat,MESS_INFO,"<%s> : <%s> : <%s>",c_tm, buf_rec[i_rec].categ.c_str(), buf_rec[i_rec].mess.c_str() );
	}
	Mess->put(test_cat,MESS_INFO,"-------- Stop Message buffer %s test ----------",n_arh.c_str());
    }
    
    //Librarry attach/detach test
    else if(id == "SOAttDet" )
    {
	TModSchedul &sched = owner().owner().ModSchedul();
	string SO_name = t_n->attr("name");
	TModSchedul::SHD so_st = sched.lib(SO_name);
	so_st.name;
	Mess->put(test_cat,MESS_INFO,"-------- Start SO <%s> test ----------",so_st.name.c_str());
	if( so_st.hd ) sched.libDet( so_st.name );
	else           sched.libAtt( so_st.name,(bool)atoi( t_n->attr("full").c_str()) );		
	Mess->put(test_cat,MESS_INFO,"-------- Stop SO <%s> test ----------",so_st.name.c_str());
    }
    
    //Parameter value test
    else if(id == "Val")
    {
	TParamS &param = owner().owner().Param();

	string s_prm = t_n->attr("name");
		
	int s_pos = s_prm.find("/",0);
	if( s_pos == string::npos ) throw TError("Parameter value error!");		    
	AutoHD<TParam> prm = param.at( s_prm.substr(0,s_pos) );
	AutoHD<TVal> val = prm.at().at().vlAt( s_prm.substr(s_pos+1) );
	if( val.at().fld().type()&T_SELECT )
	    Mess->put(test_cat,MESS_INFO,"%s: %s = %s",prm.at().at().name().c_str(), val.at().fld().descr().c_str(), val.at().getSEL().c_str() );
	else if( val.at().fld().type()&T_STRING )
	    Mess->put(test_cat,MESS_INFO,"%s: %s = %s",prm.at().at().name().c_str(), val.at().fld().descr().c_str(), val.at().getS().c_str() );
	else if( val.at().fld().type()&T_REAL )
	    Mess->put(test_cat,MESS_INFO,"%s: %s = %f",prm.at().at().name().c_str(), val.at().fld().descr().c_str(), val.at().getR() );
	else if( val.at().fld().type()&(T_DEC|T_OCT|T_HEX) )
	    Mess->put(test_cat,MESS_INFO,"%s: %s = %d",prm.at().at().name().c_str(), val.at().fld().descr().c_str(), val.at().getI() );
	else if( val.at().fld().type()&T_BOOL )
	    Mess->put(test_cat,MESS_INFO,"%s: %s = %d",prm.at().at().name().c_str(), val.at().fld().descr().c_str(), val.at().getB() );
    }
    
    //BD full test
    else if(id == "BD")
    {
	//Get test param
	string t_bd = t_n->attr("type");
	string n_bd = t_n->attr("bd");		
	string n_tbl = t_n->attr("table");
	int experem = atoi(t_n->attr("size").c_str());		
	
	AutoHD<TTipBD> bd = owner().owner().BD().gmdAt(t_bd);
		
	Mess->put_s(test_cat,MESS_INFO,"***** Begin BD test block *****");
		    
	Mess->put_s(test_cat,MESS_INFO,"Open BD: <"+n_bd+">");
	bd.at().open(n_bd,true);
			    
	Mess->put_s(test_cat,MESS_INFO,"Open Table: <"+n_tbl+">");
	bd.at().at(n_bd).at().open(n_tbl,true);
	Mess->put_s(test_cat,MESS_INFO,"Connect to table: <"+n_tbl+">");
	AutoHD<TTable> tbl = bd.at().at(n_bd).at().at(n_tbl);		

	Mess->put_s(test_cat,MESS_INFO,"Create bd config");
	TConfig bd_cfg;
	bd_cfg.elem().fldAdd( new TFld("name","Name fields",T_STRING|F_KEY,"20") );
	bd_cfg.elem().fldAdd( new TFld("descr","Description fields",T_STRING,"50") );
	bd_cfg.elem().fldAdd( new TFld("val","Field value",T_REAL,"10.2","5") );
	bd_cfg.elem().fldAdd( new TFld("id","Field id",T_DEC,"7","34") );
	bd_cfg.elem().fldAdd( new TFld("stat","Field stat",T_BOOL,"","1") );
		
	//Test of The create fields
	int st_time = times(NULL);
	for(int i_fld = 0; i_fld < experem; i_fld++)
	{
	    bd_cfg.cfg("name").setS("Sh"+SYS->int2str(i_fld));
	    bd_cfg.cfg("descr").setS("ûÉÆÒ "+SYS->int2str(i_fld));
	    bd_cfg.cfg("val").setR(sqrt(i_fld));
	    bd_cfg.cfg("id").setI(i_fld);
	    bd_cfg.cfg("stat").setB((i_fld%2)==0?true:false);
	    tbl.at().fieldSet(bd_cfg);
	}
	Mess->put(test_cat,MESS_INFO,"Create %d fields time <%f>sek!",experem,(float)(times(NULL)-st_time)/100.);
    
	//Check update fields
	st_time = times(NULL);
	for(int i_fld = 0; i_fld < experem; i_fld++)
	{
	    bd_cfg.cfg("name").setS("Sh"+SYS->int2str(i_fld));
	    bd_cfg.cfg("descr").setS("îÏ×ÙÊ ÛÉÆÒ "+SYS->int2str(i_fld));
	    bd_cfg.cfg("val").setR(2.*sqrt(i_fld));
	    bd_cfg.cfg("id").setI(2*i_fld);
	    bd_cfg.cfg("stat").setB((i_fld%2)==0?false:true);
	    tbl.at().fieldSet(bd_cfg);
	}
	Mess->put(test_cat,MESS_INFO,"Update %d fields time <%f>sek!",experem,(float)(times(NULL)-st_time)/100.);

	//Check get of fields
	st_time = times(NULL);
	for(int i_fld = 0; i_fld < experem; i_fld++)
	{
	    bd_cfg.cfg("name").setS("Sh"+SYS->int2str(i_fld));
	    tbl.at().fieldGet(bd_cfg);
    
	    if( i_fld == 155 )
		Mess->put(test_cat,MESS_INFO,"Field #155=<%s>; Descr=<%s>; Value=<%f>; Id=<%d>; Stat=<%d>!",				
		    bd_cfg.cfg("name").getS().c_str(), bd_cfg.cfg("descr").getS().c_str(),
		    bd_cfg.cfg("val").getR(), bd_cfg.cfg("id").getI(), bd_cfg.cfg("stat").getB() );

	    if( bd_cfg.cfg("name").getS() != (string("Sh")+SYS->int2str(i_fld)) ||
		    bd_cfg.cfg("descr").getS() != (string("îÏ×ÙÊ ÛÉÆÒ ")+SYS->int2str(i_fld)) ||
		    //ceil(100.*bd_cfg.cfg("val").getR()) != ceil(2.*sqrt(i_fld)) ||
		    bd_cfg.cfg("id").getI() != (2*i_fld) ||
		    bd_cfg.cfg("stat").getB() != ((i_fld%2)==0?false:true) )
		Mess->put(test_cat,MESS_INFO,"Read field <%s> error!",bd_cfg.cfg("name").getS().c_str());
	}
	Mess->put(test_cat,MESS_INFO,"Get %d fields time <%f>sek!",experem,(float)(times(NULL)-st_time)/100.);

	//Check List
	vector<string> ls_elem;
	tbl.at().fieldList("name",ls_elem);
	if( ls_elem.size() != experem ) Mess->put(test_cat,MESS_INFO,"List size error!");
		
	//Delete fields
	st_time = times(NULL);
	for(int i_fld = 0; i_fld < experem; i_fld++)
	{
	    bd_cfg.cfg("name").setS("Sh"+SYS->int2str(i_fld));
	    tbl.at().fieldDel(bd_cfg);
	}
	Mess->put(test_cat,MESS_INFO,"Del %d fields time <%f>sek!",experem,(float)(times(NULL)-st_time)/100.);

	tbl.free();

	Mess->put_s(test_cat,MESS_INFO,"Close Table: <"+n_tbl+">");
	bd.at().at(n_bd).at().close(n_tbl);
	Mess->put_s(test_cat,MESS_INFO,"Delete Table: <"+n_tbl+">");
	bd.at().at(n_bd).at().del(n_tbl);

	Mess->put_s(test_cat,MESS_INFO,"Close BD: <"+n_bd+">");
	bd.at().close(n_bd);
	Mess->put_s(test_cat,MESS_INFO,"Delete BD: <"+n_bd+">");
	bd.at().del(n_bd);
	
	bd.free();
    
	Mess->put_s(test_cat,MESS_INFO,"***** End BD test block *****");	
    }
    
    //Transport test full test
    else if(id == "TrOut")
    {
	//Get transport param
	string addr = t_n->attr("addr");
	string type = t_n->attr("type");
	string req = t_n->attr("req");

	char buf[200];

	AutoHD<TTipTransport> tr = owner().owner().Transport().gmdAt(type);
	if( !tr.at().outAvoid(addr) )
	{
	    tr.at().outAdd(addr);
	    tr.at().outAt(addr).at().address(addr);	    	    
	}
	if( !tr.at().outAt(addr).at().startStat() ) tr.at().outAt(addr).at().start();	
	
	int len = tr.at().outAt(addr).at().messIO((char *)req.c_str(),req.size(),buf,sizeof(buf)-1,1);
        buf[len] = 0;
        Mess->put(test_cat,MESS_INFO,"%s: Put <%s>. Get: <%s>",addr.c_str(),req.c_str(),buf);
    }
}

void TTest::pr_XMLNode( XMLNode *node, int level )
{
    char *buf = (char *)malloc(level+1);
    for(int i_c = 0; i_c < level; i_c++) buf[i_c] = ' ';
    buf[level] = 0;
	
    vector<string> list;
    Mess->put("TEST",MESS_INFO,"%s{%d <%s>, text <%s>, childs - %d!",
	buf, level, node->name().c_str(),node->text().c_str(),node->childSize());
    node->attrList(list);
    for(unsigned i_att = 0; i_att < list.size(); i_att++)
	Mess->put("TEST",MESS_INFO,"        Attr <%s> = <%s>!",list[i_att].c_str(),node->attr(list[i_att]).c_str());	
    for(int i_ch = 0; i_ch < node->childSize(); i_ch++)
	pr_XMLNode( node->childGet(i_ch), level+1 ); 
    Mess->put("TEST",MESS_INFO,"%s}%d <%s>", buf, level, node->name().c_str());
    free(buf);
}

