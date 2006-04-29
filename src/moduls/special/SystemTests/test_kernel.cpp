
//OpenSCADA system module Special.SystemTests file: test_kernel.cpp
/***************************************************************************
 *   Copyright (C) 2003-2006 by Roman Savochenko                           *
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
#include <string>

#include <sys/times.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>

#include "test_kernel.h"

//============ Modul info! =====================================================
#define MOD_ID      "SystemTests"
#define MOD_NAME    "OpenSCADA system's tests"
#define MOD_TYPE    "Special"
#define VER_TYPE    VER_SPC
#define SUB_TYPE    "TEST"
#define VERSION     "1.2.0"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "Allow the group tests for OpenSCADA system."
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
    mVers      	= VERSION;
    mAutor    	= AUTORS;
    mDescr  	= DESCRIPTION;
    mLicense   	= LICENSE;
    mSource    	= name;
}

TTest::~TTest()
{
    if( run_st ) modStop();
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
	"PARAM        Parameter test:\n"
        "  name         name.\n"
	"XML          XML parsing test:\n"
	"  file         file for XML parsing.\n"
	"MESS         Messages archive test:\n"
	"  arh          archive name;\n"
	"  t_arh        archive type;\n"
	"  categ        messages category pattern.\n"
	"SOAttDet     Attach/Detach module test:\n"
	"  name         name modul (path to module);\n"
	"  full         full attach(to start).\n"
	"Val          Parameter atributes value test:\n"
	"  name         parameter and his attribute ([param]/[attrib]).\n"
	"BD           Full database test:\n"
	"  type         type BD;\n"
	"  bd           name BD;\n"
	"  table        table;\n"
	"  size         fields number.\n"
	"TrOut        Output transport test:\n"
	"  addr         input transport address;\n"
	"  type         transport type;\n"
	"  req          request to a input transport.\n"
	"Func	      Function subsystem test;\n"
	"SysContrLang System control language test:\n"
	"  path         path to language element.\n"
	"ValBuf       Value buffer tests;\n"
	"\n"),
	MOD_TYPE,MOD_ID,nodePath().c_str());
    
    return(buf);
}			

void TTest::modLoad( )
{
    //========== Load parameters from command line ============
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

    //========== Load parameters from config file =============
}

void TTest::modStart(  )
{
    if( run_st ) return;
    pthread_attr_t pthr_attr;

    pthread_attr_init(&pthr_attr);
    pthread_attr_setschedpolicy(&pthr_attr,SCHED_OTHER);
    pthread_create(&pthr_tsk,&pthr_attr,Task,this);
    pthread_attr_destroy(&pthr_attr);
    if( TSYS::eventWait( run_st, true, string(MOD_ID)+": Is starting....",5) )
	throw TError("%s: No started!",MOD_ID);
}

void TTest::modStop(  )
{
    if( !run_st ) return;

    endrun = true;
    if( TSYS::eventWait( run_st, false, string(MOD_ID)+": Is stoping....",5) )
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
    Mess->put(tst->nodePath().c_str(),TMess::Debug,Mess->I18N("Thread <%d> started!"),getpid());
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
		    XMLNode *t_n = tst->ctrId(&SYS->cfgRoot(),tst->nodePath())->childGet("prm",nd_cnt++);
		    
		    if( t_n->attr("on") == "1" && atoi(t_n->attr("per").c_str()) && 
			    !( count % atoi(t_n->attr("per").c_str()) ) )
		    {
			string id = t_n->attr("id");		    
		    	try{ tst->Test( id, t_n ); }
			catch( TError err )
			{ Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); }
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
    string m_cat = nodePath()+id;
    const char *test_cat = m_cat.c_str();
    
    //Parameter config test
    if(id == "PARAM" )
    {
	AutoHD<TParamS> param = owner().owner().param();
	
	AutoHD<TParam> prm = param.at().at( t_n->attr("name") );
	Mess->put(test_cat,TMess::Info,"-------- Start parameter <%s> test ----------",t_n->attr("name").c_str());
    
	vector<string> list_el;
	/*prm.at().at().cfgList(list_el);
	Mess->put(test_cat,TMess::Info,"Config elements present: %d",list_el.size());
	for(unsigned i=0; i< list_el.size(); i++)
	    Mess->put(test_cat,TMess::Info,"Element: %s",list_el[i].c_str());*/
	    
	Mess->put(test_cat,TMess::Info,"Value elements present: %d",list_el.size());
	//prm.vlSetR(0,30);
	list_el.clear();
	prm.at().vlList(list_el);
	for(unsigned i=0; i< list_el.size(); i++)
	{
	    AutoHD<TVal> val = prm.at().vlAt(list_el[i]);
	    if( val.at().fld().flg()&FLD_SELECT )
		Mess->put(test_cat,TMess::Info,"Element (SELECT): %s: %s",list_el[i].c_str(), val.at().getSEL().c_str() );
	    switch(val.at().fld().type())
	    {
		case TFld::String:	
		    Mess->put(test_cat,TMess::Info,"Element (STRING): %s: %s",list_el[i].c_str(), val.at().getS().c_str() );
		    break;
		case TFld::Dec: case TFld::Oct: case TFld::Hex:
		    Mess->put(test_cat,TMess::Info,"Element (INTEGER): %s: %d",list_el[i].c_str(), val.at().getI() );
		    break;		    
		case TFld::Real:
		    Mess->put(test_cat,TMess::Info,"Element (REAL): %s: %f",list_el[i].c_str(), val.at().getR() );
		    break;
		case TFld::Bool:
		    Mess->put(test_cat,TMess::Info,"Element (BOOLEAN): %s: %d",list_el[i].c_str(), val.at().getB() );
		    break;
	    }
	}
		
	Mess->put(test_cat,TMess::Info,"Configs throw control: %d",list_el.size());
	    
	XMLNode node;
	prm.at().TCntrNode::cntrCmd("",&node,TCntrNode::Info);
	pr_XMLNode( test_cat, &node, 0 );

	Mess->put(test_cat,TMess::Info,"-------- Stop parameter <%s> test ----------",t_n->attr("name").c_str());
    }
    
    //XML parsing test
    else if(id == "XML" )
    {
	int hd = open(t_n->attr("file").c_str(),O_RDONLY);
	if(hd > 0)
	{
	    Mess->put(test_cat,TMess::Info,"-------- Start TEST XML parsing ----------");
	    int cf_sz = lseek(hd,0,SEEK_END);
	    lseek(hd,0,SEEK_SET);
	    char *buf = (char *)malloc(cf_sz);
	    read(hd,buf,cf_sz);
	    close(hd);
	    string s_buf = buf;
	    free(buf);
	    XMLNode node;
	    node.load(s_buf);
	    pr_XMLNode( test_cat, &node, 0 );
	    Mess->put(test_cat,TMess::Info,"-------- End TEST XML parsing ----------");
	}
    }
    
    //Message arhive test
    else if(id == "MESS" )
    {
	AutoHD<TArchiveS> Arh_s = owner().owner().archive();
		
	string n_arh = t_n->attr("arh");
	string t_arh = t_n->attr("t_arh");
	Mess->put(test_cat,TMess::Info,"-------- Start Message buffer %s test ----------",n_arh.c_str());
	vector<TMess::SRec> buf_rec;
	if( n_arh == "sys" ) Mess->get(0,time(NULL),buf_rec,t_n->attr("categ"));
	else		    
	    Arh_s.at().at(t_arh).at().messAt(n_arh).at().get(0,time(NULL),buf_rec,t_n->attr("categ"));
	Mess->put(test_cat,TMess::Info,"Messages present %d.",buf_rec.size() );
	for(unsigned i_rec = 0; i_rec < buf_rec.size(); i_rec++)
	{
	    char *c_tm = ctime( &buf_rec[i_rec].time);
	    for( int i_ch = 0; i_ch < strlen(c_tm); i_ch++ )
		if( c_tm[i_ch] == '\n' ) c_tm[i_ch] = '\0';
	    Mess->put(test_cat,TMess::Info,"<%s> : <%s> : <%s>",c_tm, buf_rec[i_rec].categ.c_str(), buf_rec[i_rec].mess.c_str() );
	}
	Mess->put(test_cat,TMess::Info,"-------- Stop Message buffer %s test ----------",n_arh.c_str());
    }
    
    //Librarry attach/detach test
    else if(id == "SOAttDet" )
    {
	//AutoHD<TTransportIn> tr = ((TTipTransport &)owner().owner().Transport().modAt("socket").at()).inAt("www");
	AutoHD<TModSchedul> sched = owner().owner().modSchedul();
	string SO_name = t_n->attr("name");
	TModSchedul::SHD &so_st = sched.at().lib(SO_name);
	Mess->put(test_cat,TMess::Info,"-------- Start SO <%s> test ----------",so_st.name.c_str());
	if( so_st.hd ) sched.at().libDet( so_st.name );
	else           sched.at().libAtt( so_st.name,(bool)atoi( t_n->attr("full").c_str()) );		
	Mess->put(test_cat,TMess::Info,"-------- Stop SO <%s> test ----------",so_st.name.c_str());
    }
    
    //Parameter value test
    else if(id == "Val")
    {
	AutoHD<TParamS> param = owner().owner().param();

	string s_prm = t_n->attr("name");
	int a_len = atoi(t_n->attr("arch_len").c_str());
	int a_per = atoi(t_n->attr("arch_per").c_str());
	
	Mess->put(test_cat,TMess::Info,"Value of: %s.",s_prm.c_str());	
	
	if( !dynamic_cast<TVal *>(&SYS->nodeAt(s_prm,0,'.').at()) )
	    throw TError("","Parameter path error!");	    
	AutoHD<TVal> val = SYS->nodeAt(s_prm,0,'.');
	Mess->put(test_cat,TMess::Info,"Last value = %s", val.at().getS(NULL).c_str() );
	if( a_len && a_per )
	{
	    long long cur = TSYS::curTime()-a_per;
	    for( int i_v = 1; i_v <= a_len; i_v++, cur-=a_per)
	    {
		long long rvtm = cur;
		Mess->put(test_cat,TMess::Info,"Value %d = %s.",i_v, val.at().getS(&rvtm).c_str() );  
	    }	
	}
    }
    
    //BD full test
    else if(id == "BD")
    {
	//Get test param
	string t_bd = t_n->attr("type");
	string n_bd = "test_bd";
	string bd_addr = t_n->attr("bd");
	string n_tbl = t_n->attr("table");
	int experem = atoi(t_n->attr("size").c_str());		
	
	AutoHD<TTipBD> bd = owner().owner().db().at().modAt(t_bd);
		
	Mess->put(test_cat,TMess::Info,"***** Begin BD test block *****");
		    
	Mess->put(test_cat,TMess::Info,"Open BD: <%s>",n_bd.c_str());	
	
	bd.at().open(n_bd);
	bd.at().at(n_bd).at().addr(bd_addr);
	bd.at().at(n_bd).at().create(true);
	bd.at().at(n_bd).at().enable();
			    
	Mess->put(test_cat,TMess::Info,"Open Table: <%s>",n_tbl.c_str());
	bd.at().at(n_bd).at().open(n_tbl,true);
	Mess->put(test_cat,TMess::Info,"Connect to table: <%s>",n_tbl.c_str());
	AutoHD<TTable> tbl = bd.at().at(n_bd).at().at(n_tbl);		

	Mess->put(test_cat,TMess::Info,"Create db config");
	TConfig bd_cfg;
	bd_cfg.elem().fldAdd( new TFld("name","Name fields",TFld::String,FLD_KEY,"20") );
	bd_cfg.elem().fldAdd( new TFld("descr","Description fields",TFld::String,0,"50") );
	bd_cfg.elem().fldAdd( new TFld("val","Field value",TFld::Real,0,"10.2","5") );
	bd_cfg.elem().fldAdd( new TFld("id","Field id",TFld::Dec,0,"7","34") );
	bd_cfg.elem().fldAdd( new TFld("stat","Field stat",TFld::Bool,0,"","1") );
	
	//Test of The create fields
	Mess->put(test_cat,TMess::Info,"Create fields!");
	int st_time = times(NULL);
	for(int i_fld = 0; i_fld < experem; i_fld++)
	{
	    bd_cfg.cfg("name").setS("Sh"+SYS->int2str(i_fld));
	    bd_cfg.cfg("descr").setS("Shifr "+SYS->int2str(i_fld));
	    bd_cfg.cfg("val").setR(sqrt(i_fld));
	    bd_cfg.cfg("id").setI(i_fld);
	    bd_cfg.cfg("stat").setB((i_fld%2)==0?true:false);
	    tbl.at().fieldSet(bd_cfg);
	}
	Mess->put(test_cat,TMess::Info,"Create %d fields time <%f>sek!",experem,(float)(times(NULL)-st_time)/TSYS::HZ());
    
	//Check update fields
	Mess->put(test_cat,TMess::Info,"Update fields!");
	st_time = times(NULL);
	for(int i_fld = 0; i_fld < experem; i_fld++)
	{
	    bd_cfg.cfg("name").setS("Sh"+SYS->int2str(i_fld));
	    bd_cfg.cfg("descr").setS("New shifr "+SYS->int2str(i_fld));
	    bd_cfg.cfg("val").setR(2.*sqrt(i_fld));
	    bd_cfg.cfg("id").setI(2*i_fld);
	    bd_cfg.cfg("stat").setB((i_fld%2)==0?false:true);
	    tbl.at().fieldSet(bd_cfg);
	}
	Mess->put(test_cat,TMess::Info,"Update %d fields time <%f>sek!",experem,(float)(times(NULL)-st_time)/TSYS::HZ());
	
	//Check get of fields
	Mess->put(test_cat,TMess::Info,"Check fields!");
	st_time = times(NULL);
	for(int i_fld = 0; i_fld < experem; i_fld++)
	{
	    bd_cfg.cfg("name").setS("Sh"+SYS->int2str(i_fld));
	    tbl.at().fieldGet(bd_cfg);
    
	    if( i_fld == 155 )
		Mess->put(test_cat,TMess::Info,"Field #155=<%s>; Descr=<%s>; Value=<%f>; Id=<%d>; Stat=<%d>!",				
		    bd_cfg.cfg("name").getS().c_str(), bd_cfg.cfg("descr").getS().c_str(),
		    bd_cfg.cfg("val").getR(), bd_cfg.cfg("id").getI(), bd_cfg.cfg("stat").getB() );

	    if( bd_cfg.cfg("name").getS() != (string("Sh")+SYS->int2str(i_fld)) )
		Mess->put(test_cat,TMess::Info,"Field <Sh> <%s>!=<%s> error!",
		    bd_cfg.cfg("name").getS().c_str(),(string("Sh")+SYS->int2str(i_fld)).c_str());
	    if( bd_cfg.cfg("descr").getS() != (string("New shifr ")+SYS->int2str(i_fld)) )
		Mess->put(test_cat,TMess::Info,"Field <descr> <%s>!=<%s> error!",		
		    bd_cfg.cfg("descr").getS().c_str(),(string("New shifr ")+SYS->int2str(i_fld)).c_str() );		    
		    //ceil(100.*bd_cfg.cfg("val").getR()) != ceil(2.*sqrt(i_fld)) ||
	    if( bd_cfg.cfg("id").getI() != (2*i_fld) )
		Mess->put(test_cat,TMess::Info,"Field <id> <%d>!=<%d> error!",
		    bd_cfg.cfg("id").getI(), (2*i_fld) );
	    if( bd_cfg.cfg("stat").getB() != ((i_fld%2)==0?false:true) )
		Mess->put(test_cat,TMess::Info,"Field <stat> <%d>!=<%d> error!",
		    bd_cfg.cfg("stat").getB(), ((i_fld%2)==0?false:true) );
	}
	Mess->put(test_cat,TMess::Info,"Get %d fields time <%f>sek!",experem,(float)(times(NULL)-st_time)/TSYS::HZ());

	//Check Fix structure
	Mess->put(test_cat,TMess::Info,"Change DB structure!");
	//Add column
	bd_cfg.elem().fldAdd( new TFld("fix","BD fix test",TFld::String,0,"20") );
	bd_cfg.cfg("name").setS("Sh1");
	tbl.at().fieldSet(bd_cfg);
	bd_cfg.cfg("name").setS("Sh2");
	tbl.at().fieldGet(bd_cfg);
	if( bd_cfg.cfg("name").getS() != "Sh2" || bd_cfg.cfg("descr").getS() != "New shifr 2" ||
	    bd_cfg.cfg("id").getI() != 4 || bd_cfg.cfg("stat").getB() != false )
	{
	    Mess->put(test_cat,TMess::Info,"Add column error!");
	    Mess->put(test_cat,TMess::Info,"Field #2=<%s>; Descr=<%s>; Value=<%f>; Id=<%d>; Stat=<%d>!",
                bd_cfg.cfg("name").getS().c_str(), bd_cfg.cfg("descr").getS().c_str(),
                bd_cfg.cfg("val").getR(), bd_cfg.cfg("id").getI(), bd_cfg.cfg("stat").getB() );
	}
	else 
	    Mess->put(test_cat,TMess::Info,"Add column ok!");    
	
	//Del column
	bd_cfg.elem().fldDel(bd_cfg.elem().fldId("fix"));
	bd_cfg.cfg("name").setS("Sh1");
        tbl.at().fieldSet(bd_cfg);
	bd_cfg.cfg("name").setS("Sh2");
        tbl.at().fieldGet(bd_cfg);
        if( bd_cfg.cfg("name").getS() != "Sh2" || bd_cfg.cfg("descr").getS() != "New shifr 2" ||
    	    bd_cfg.cfg("id").getI() != 4 || bd_cfg.cfg("stat").getB() != false )
	{
	    Mess->put(test_cat,TMess::Info,"Del column error!");	    
	    Mess->put(test_cat,TMess::Info,"Field #2=<%s>; Descr=<%s>; Value=<%f>; Id=<%d>; Stat=<%d>!",
                bd_cfg.cfg("name").getS().c_str(), bd_cfg.cfg("descr").getS().c_str(),
                bd_cfg.cfg("val").getR(), bd_cfg.cfg("id").getI(), bd_cfg.cfg("stat").getB() );
	}
	else
	    Mess->put(test_cat,TMess::Info,"Del column ok!");									    

	//Check List
	//vector<string> ls_elem;
	//tbl.at().fieldList("name",ls_elem);
	//if( ls_elem.size() != experem ) Mess->put(test_cat,TMess::Info,"List size error!");
		
	//Delete fields
	st_time = times(NULL);
	for(int i_fld = 0; i_fld < experem; i_fld++)
	{
	    bd_cfg.cfg("name").setS("Sh"+SYS->int2str(i_fld));
	    tbl.at().fieldDel(bd_cfg);
	}
	Mess->put(test_cat,TMess::Info,"Del %d fields time <%f>sek!",experem,(float)(times(NULL)-st_time)/TSYS::HZ());

	tbl.free();
	
	Mess->put(test_cat,TMess::Info,"Close and delete table: <%s>",n_tbl.c_str());
	bd.at().at(n_bd).at().close(n_tbl,true);

	Mess->put(test_cat,TMess::Info,"Close and delete BD: <%s>",n_bd.c_str());
	bd.at().close(n_bd,true);
	
	bd.free();
    
	Mess->put(test_cat,TMess::Info,"***** End BD test block *****");	
    }
    
    //Transport test full test
    else if(id == "TrOut")
    {
	//Get transport param
	string addr = t_n->attr("addr");
	string type = t_n->attr("type");
	string req = t_n->attr("req");

	char buf[200];

	AutoHD<TTipTransport> tr = owner().owner().transport().at().modAt(type);
	if( !tr.at().outPresent(addr) )
	{
	    tr.at().outAdd(addr);
	    tr.at().outAt(addr).at().addr(addr);	    	    
	}
	if( !tr.at().outAt(addr).at().startStat() ) tr.at().outAt(addr).at().start();	
	int len = tr.at().outAt(addr).at().messIO(req.c_str(),req.size(),buf,sizeof(buf)-1,1);
	tr.at().outAt(addr).at().stop();
        buf[len] = 0;
        Mess->put(test_cat,TMess::Info,"%s: Put <%s>. Get: <%s>",addr.c_str(),req.c_str(),buf);
    }
    //Function subsystem test
    else if(id == "Func")
    {
	//=========================== Test TFunctionS =========================
	/* bool err_ok;
	vector<string> lst;
	int i_ls;
	//Define library class
	class TestLib : public TLibFunc
	{
       	    public:
		TestLib( const string &id ) : TLibFunc(id){ }

		string name()	{ return "TestLib"; }
		string descr()	{ return "Test library"; }
	};
	Mess->put(test_cat,TMess::Info,"TLibrarieS tests.");
	//--------------------------- Test 1 ----------------------------------
	Mess->put(test_cat,TMess::Info,"Test1.");
	Mess->put(test_cat,TMess::Info,"Create the true library.");
	TestLib *tlib = new TestLib("testLib");
	
	Mess->put(test_cat,TMess::Info,"Register the true library.");
	owner().owner().func().at().reg( tlib );

	Mess->put(test_cat,TMess::Info,"Present check of library.");
	if( !owner().owner().func().at().present("testLib") )
	{
	    owner().owner().func().at().unreg("testLib");
	    throw TError("","Test1 failed! Present check error!" );
	}
	
	Mess->put(test_cat,TMess::Info,"Libraries list check.");
	owner().owner().func().at().list(lst);
	for( i_ls = 0; i_ls < lst.size(); i_ls++ )
	    if( lst[i_ls] == "testLib" ) break;
	if( i_ls >= lst.size() ) throw TError("","Test1 failed! Libraries list error!" );
	Mess->put(test_cat,TMess::Info,"Test1 passed!");
	
	//--------------------------- Test 2 ----------------------------------	
	Mess->put(test_cat,TMess::Info,"Test2.");
	err_ok = false;
	Mess->put(test_cat,TMess::Info,"Register the double library.");
	try{ owner().owner().func().at().reg( tlib ); }
	catch( TError err )
	{ 
	    Mess->put(test_cat,TMess::Info,"Register the double library exception: %s",err.mess.c_str()); 
	    err_ok = true;
	}
	if( !err_ok )
	{
	    owner().owner().func().at().unreg("testLib");
	    throw TError("","Test2 failed! Register double library error!" );
	}
	Mess->put(test_cat,TMess::Info,"Test2 passed!");
	
	//--------------------------- Test 3 ----------------------------------	
	Mess->put(test_cat,TMess::Info,"Test3.");
	Mess->put(test_cat,TMess::Info,"Library access check.");
	AutoHD<TLibFunc> hd_lb = owner().owner().func().at().at("testLib");
	
	err_ok = false;
	Mess->put(test_cat,TMess::Info,"Library blocking check.");
	try{ owner().owner().func().at().unreg("testLib"); }
	catch( TError err )
	{ 
	    Mess->put(test_cat,TMess::Info,"Library blocking exception: %s",err.mess.c_str()); 
	    err_ok = true;
	}
	if( !err_ok ) { throw TError("","Test3 failed! Library blocking error!" ); }
	hd_lb.free();
	Mess->put(test_cat,TMess::Info,"Test3 passed!");
	
	//--------------------------- Test 4 ----------------------------------	
	Mess->put(test_cat,TMess::Info,"Test4.");
	Mess->put(test_cat,TMess::Info,"Library bad access check.");
	err_ok = false;
	try{ AutoHD<TLibFunc> hd_lb = owner().owner().func().at().at("testLib1"); }
	catch( TError err )
	{ 
	    Mess->put(test_cat,TMess::Info,"Library access exception: %s",err.mess.c_str()); 
	    err_ok = true;
	}
	if( !err_ok ) { throw TError("","Test4 failed! Library access error!" ); }
	Mess->put(test_cat,TMess::Info,"Test4 passed!");
	
	//--------------------------- Test 5 ----------------------------------	
	Mess->put(test_cat,TMess::Info,"Test5.");	
	Mess->put(test_cat,TMess::Info,"Remove the true library.");
	owner().owner().func().at().unreg("testLib");
	
	Mess->put(test_cat,TMess::Info,"Not present check of library.");
	if( owner().owner().func().at().present("testLib") )
	{
	    owner().owner().func().at().unreg("testLib");
	    throw TError("","Test5 false! Not present check error!" );
	}
	Mess->put(test_cat,TMess::Info,"Libraries list check.");
	owner().owner().func().at().list(lst);
	for( i_ls = 0; i_ls < lst.size(); i_ls++ )
	    if( lst[i_ls] == "testLib" ) break;
	if( i_ls < lst.size() ) throw TError("","Test5 failed! Libraries list error!" );	
	Mess->put(test_cat,TMess::Info,"Test5 passed!");
	
	//--------------------------- Test 6 ----------------------------------	
	Mess->put(test_cat,TMess::Info,"Test6.");
	Mess->put(test_cat,TMess::Info,"Remove no present library.");
	err_ok = false;
	try{ owner().owner().func().at().unreg("testLib"); }
	catch( TError err )
	{ 
	    Mess->put(test_cat,TMess::Info,"Library remove exception: %s",err.mess.c_str()); 
	    err_ok = true;
	}
	if( !err_ok ) { throw TError("","Test6 failed! Library remove error!" ); }
	Mess->put(test_cat,TMess::Info,"Test6 passed!");
	
	//--------------------------- Test 7 ----------------------------------	
	Mess->put(test_cat,TMess::Info,"Test7.");	
	err_ok = false;
	Mess->put(test_cat,TMess::Info,"Create the empty library.");
	tlib = new TestLib("");
	
	Mess->put(test_cat,TMess::Info,"Register the empty library.");       
	try{ owner().owner().func().at().reg( tlib ); }
	catch( TError err )
	{ 	    
	    Mess->put(test_cat,TMess::Info,"Register the empty library exception: %s",err.mess.c_str());
	    err_ok = true;
	}
	if( !err_ok ) { throw TError("","Test7 failed! Register the empty library error!" ); }
	Mess->put(test_cat,TMess::Info,"Test7 passed!");	
	
	//=========================== Test TLibFunc =========================
	Mess->put(test_cat,TMess::Info,"TLibFunc tests.");
	class TestLib1 : public TLibFunc
	{
       	    public:
		TestLib1( const string &test_cat ) : TLibFunc("TestLib1")
		{
		    bool err_ok;
	    	    int i_ls;
		    vector<string> lst;
		    
		    class TestFunc : public TFunction
		    {
			public:
			    TestFunc( const string &id ) : TFunction(id)
			    {  }

			    string name()  { return "TestFunc"; }
			    string descr() { return "Test Function"; }

			    void calc( TValFunc *val ){ }
		    };
		    
		    //--------------------------- Test 8 ----------------------------------
		    Mess->put(test_cat.c_str(),TMess::Info,"Test8.");
		    Mess->put(test_cat.c_str(),TMess::Info,"Create the true function.");
		    TestFunc *tfnc = new TestFunc("testFnc");
	
		    Mess->put(test_cat.c_str(),TMess::Info,"Register the true function.");
		    reg( tfnc );
		    
	    	    Mess->put(test_cat.c_str(),TMess::Info,"Present check of function.");
	    	    if( !present("testFnc") )
	    	    {
			unreg("testFnc");
	    		throw TError("","Test8 failed! Present check error!" );
	    	    }	
		    
	    	    Mess->put(test_cat.c_str(),TMess::Info,"Functions list check.");
	    	    list(lst);
		    for( i_ls = 0; i_ls < lst.size(); i_ls++ )
			if( lst[i_ls] == "testFnc" ) break;
		    if( i_ls >= lst.size() ) throw TError("","Test8 failed! Functions list error!" );
		    Mess->put(test_cat.c_str(),TMess::Info,"Test8 passed!");
	    
		    //--------------------------- Test 9 ----------------------------------	
		    Mess->put(test_cat.c_str(),TMess::Info,"Test9.");	
		    err_ok = false;
		    Mess->put(test_cat.c_str(),TMess::Info,"Register the double function.");
		    try{ reg( tfnc ); }
		    catch( TError err )
		    { 
			Mess->put(test_cat.c_str(),TMess::Info,"Register the double function exception: %s",err.mess.c_str()); 
	    		err_ok = true;
		    }
		    if( !err_ok )
		    {
			unreg("testFnc");
			throw TError("","Test9 failed. Register double function error!" );
		    }
		    Mess->put(test_cat.c_str(),TMess::Info,"Test9 passed!");
	
		    //--------------------------- Test 10 ----------------------------------	
		    Mess->put(test_cat.c_str(),TMess::Info,"Test10.");		    
		    Mess->put(test_cat.c_str(),TMess::Info,"Function access check.");
		    AutoHD<TFunction> hd_fnc = at("testFnc");
	
	    	    err_ok = false;
		    Mess->put(test_cat.c_str(),TMess::Info,"Function blocking check.");
		    try{ unreg("testFnc"); }
	    	    catch( TError err )
		    { 
			Mess->put(test_cat.c_str(),TMess::Info,"Function blocking exception: %s",err.mess.c_str()); 
			err_ok = true;
	    	    }
		    if( !err_ok ) { throw TError("","Test10 failed! Function blocking error!" ); }
	    	    hd_fnc.free();	
		    Mess->put(test_cat.c_str(),TMess::Info,"Test10 passed!");
	
		    //--------------------------- Test 11 ----------------------------------	
		    Mess->put(test_cat.c_str(),TMess::Info,"Test11.");	
		    Mess->put(test_cat.c_str(),TMess::Info,"Function bad access check.");
		    err_ok = false;
		    try{ AutoHD<TFunction> hd_fnc = at("testFnc1"); }
		    catch( TError err )
		    { 
			Mess->put(test_cat.c_str(),TMess::Info,"Function access exception: %s",err.mess.c_str()); 
			err_ok = true;
		    }
		    if( !err_ok ) { throw TError("","Test11 failed! Function access error!" ); }
		    Mess->put(test_cat.c_str(),TMess::Info,"Test11 passed!");
	
	    	    //--------------------------- Test 12 ----------------------------------
		    Mess->put(test_cat.c_str(),TMess::Info,"Test12.");		
	    	    Mess->put(test_cat.c_str(),TMess::Info,"Remove the true function.");
	    	    unreg("testFnc");
	
	    	    Mess->put(test_cat.c_str(),TMess::Info,"Not present check of function.");
	    	    if( present("testFnc") )
	    	    {
			unreg("testFnc");
	    		throw TError("","Test12 failed! Not present check error!" );
	    	    }
	    	    Mess->put(test_cat.c_str(),TMess::Info,"Functions list check.");
	    	    list(lst);
		    for( i_ls = 0; i_ls < lst.size(); i_ls++ )
			if( lst[i_ls] == "testFnc" ) break;
		    if( i_ls < lst.size() ) throw TError("","Test12 failed! Functions list error!" );
		    Mess->put(test_cat.c_str(),TMess::Info,"Test12 passed!");
		    
	    	    //--------------------------- Test 13 ----------------------------------
		    Mess->put(test_cat.c_str(),TMess::Info,"Test13.");
	    	    Mess->put(test_cat.c_str(),TMess::Info,"Remove no present function.");
		    err_ok = false;
	    	    try{ unreg("testFnc"); }
		    catch( TError err )
		    { 
			Mess->put(test_cat.c_str(),TMess::Info,"Function remove exception: %s",err.mess.c_str()); 
			err_ok = true;
		    }
		    if( !err_ok ) { throw TError("","Test13 failed! Function remove error!" ); }
		    Mess->put(test_cat.c_str(),TMess::Info,"Test13 passed!");
		    
	    	    //--------------------------- Test 14 ----------------------------------
		    Mess->put(test_cat.c_str(),TMess::Info,"Test14.");
		    Mess->put(test_cat.c_str(),TMess::Info,"Create the empty function.");
		    tfnc = new TestFunc("");
	
	    	    Mess->put(test_cat.c_str(),TMess::Info,"Register the empty function.");       
	    	    err_ok = false;
	    	    try{ reg( tfnc ); }
	    	    catch( TError err )
		    { 	    
			Mess->put(test_cat.c_str(),TMess::Info,"Register the empty function exception: %s",err.mess.c_str());
	    		err_ok = true;
		    }
		    if( !err_ok ) { throw TError("","Test14 failed. Register the empty function error!" ); }
		    Mess->put(test_cat.c_str(),TMess::Info,"Test14 passed!");
		}

		string name()	{ return "TestLib"; }
		string descr()	{ return "Test library"; }
	};
	owner().owner().func().at().reg( new TestLib1(test_cat) );
	owner().owner().func().at().unreg( "TestLib1" );
	
	//=========================== Test TValFunc =========================
	Mess->put(test_cat,TMess::Info,"TValFunc tests.");
	class TestFunc : public TFunction
	{
	    public:
		TestFunc( ) : TFunction("TestFnc")
		{  
		    ioAdd( new IO("str","String",IO::String,IO::Input,"0") );
		    ioAdd( new IO("int","Integer",IO::Integer,IO::Input,"0") );
		    ioAdd( new IO("real","Real",IO::Real,IO::Input,"0") );
		    ioAdd( new IO("bool","Boolean",IO::Boolean,IO::Input,"0") );			    
		}
		string name()  { return "TestFunc"; }
		string descr() { return "Test Function"; }

		void calc( TValFunc *val ){ }
	};
	//--------------------------- Test 15 ----------------------------------
	Mess->put(test_cat,TMess::Info,"Test15.");
	Mess->put(test_cat,TMess::Info,"Create the function.");	
	TestFunc *w_fnc = new TestFunc();
	
	Mess->put(test_cat,TMess::Info,"Create the value function.");	
        TValFunc *vl_fnc = new TValFunc("test",NULL);

	Mess->put(test_cat,TMess::Info,"Connect function to value.");	
	vl_fnc->func(w_fnc);

	Mess->put(test_cat,TMess::Info,"Write values.");
	vl_fnc->setS(vl_fnc->ioId("str"),"TEST value");
	vl_fnc->setI(vl_fnc->ioId("int"),12345);
	vl_fnc->setR(vl_fnc->ioId("real"),12345.12345);
	vl_fnc->setB(vl_fnc->ioId("bool"),true);
	
	Mess->put(test_cat,TMess::Info,"Read values");
	Mess->put(test_cat,TMess::Info,"Values: <%s>,<%d>,<%f>,<%d>",
		vl_fnc->getS(vl_fnc->ioId("str")).c_str(),vl_fnc->getI(vl_fnc->ioId("int")),
		vl_fnc->getR(vl_fnc->ioId("real")),vl_fnc->getB(vl_fnc->ioId("bool")) );
	Mess->put(test_cat,TMess::Info,"Test15 passed!");
	//--------------------------- Test 16 ----------------------------------
	Mess->put(test_cat,TMess::Info,"Test16.");
	Mess->put(test_cat,TMess::Info,"Disconnect function from value.");
	vl_fnc->func(NULL);
	Mess->put(test_cat,TMess::Info,"Read values");
	err_ok = false;
	try{ Mess->put(test_cat,TMess::Info,"Values: <%s>,<%d>,<%f>,<%d>",
		vl_fnc->getS(vl_fnc->ioId("str")).c_str(),vl_fnc->getI(vl_fnc->ioId("int")),
		vl_fnc->getR(vl_fnc->ioId("real")),vl_fnc->getB(vl_fnc->ioId("bool")) ); }
	catch( TError err )
	{ 	    
	    Mess->put(test_cat,TMess::Info,"Read values exception: %s",err.mess.c_str());
	    err_ok = true;
	}
	if( !err_ok ) { throw TError("","Test16 failed. Read values error!" ); }
	delete vl_fnc;
	delete w_fnc;
	Mess->put(test_cat,TMess::Info,"Test16 passed!");*/
    }
    //System controll test
    else if(id == "SysContrLang")
    {
	XMLNode node;
	string path = t_n->attr("path");
	Mess->put(test_cat,TMess::Info,"SysContr tests for path <%s>.",path.c_str());
	
	SYS->cntrCmd( path, &node, TCntrNode::Info );
	//printf("Source: <%s>\n",node.save().c_str());
	pr_XMLNode( test_cat, &node, 0 );
    }
    else if(id == "ValBuf")
    {
	long long rtm, wtm;
	unsigned long long st_cnt;
	
	Mess->put(test_cat,TMess::Info,"*** Value buffer tests. ***");
	//--------------------------- Test 1 ----------------------------------
        Mess->put(test_cat,TMess::Info,"Test1. Create buffer: Data = string, Size = 10, Period = 1s, Tight = 10% HardGrid = yes, HighRes = no.");
	TValBuf *buf = new TValBuf( TFld::String, 10, 1000000, true, false );
	if( buf->valType() == TFld::String && buf->size() == 10 && buf->period() == 1000000 && buf->hardGrid() && !buf->highResTm() )
	    Mess->put(test_cat,TMess::Info,"Test1 passed.");
	else throw TError("","Test1 failed! Create buffer error!" );
	
        //--------------------------- Test 2 ----------------------------------
	Mess->put(test_cat,TMess::Info,"Test2. Change buffer mode.");
	buf->hardGrid( false );
 	if( buf->hardGrid() == false )	Mess->put(test_cat,TMess::Info,"  Disable hard griding ok.");
	else throw TError("","Test2 failed! Disable hard griding failed!" );
	buf->highResTm(true);
  	if( buf->highResTm() == true )	Mess->put(test_cat,TMess::Info,"  Set high resolution time ok.");
	else throw TError("","Test2 failed! Set high resolution time failed!" );
	buf->size( 500 );
	buf->size( 2000 );
   	if( buf->size() == 2000 ) 	Mess->put(test_cat,TMess::Info,"  Change buffer size ok.");
	else throw TError("","Test2 failed! Change buffer size failed!" );
	buf->period(0);
    	if( buf->period() == 0 ) 	Mess->put(test_cat,TMess::Info,"  Change period ok.");
	else throw TError("","Test2 failed! Change period failed!" );
	Mess->put(test_cat,TMess::Info,"Test2 passed.");
	
	//--------------------------- Test 3 ----------------------------------
	Mess->put(test_cat,TMess::Info,"Test3. Destroy buffer.");
	delete buf;
	Mess->put(test_cat,TMess::Info,"Test3 passed.");
	
	//--------------------------- Test 4 ----------------------------------
	Mess->put(test_cat,TMess::Info,"Test4. Fill and check hard time griding string buffer.");
	buf = new TValBuf( TFld::String, 10, 100000, true, true );
	wtm = buf->period()*(TSYS::curTime()/buf->period());
	//Not full fill
	for(int i=0; i<buf->size()/2; i++)
	    buf->setS("Test: "+TSYS::int2str(i),wtm+i*buf->period());
	if(!(buf->end()/buf->period() == wtm/buf->period()+(buf->size()/2)-1 && buf->begin()/buf->period() == wtm/buf->period()))
	    throw TError("","Test4 failed! Buffer begin or/and end error, at filling half buffer!" );
	for(long long i = buf->begin(); i <= buf->end(); i+=buf->period() )
	{
	    rtm = i;
	    if(buf->getS(&rtm) != "Test: "+TSYS::int2str((i-wtm)/buf->period()))
	    	throw TError("","Test4 failed! Write a half buffer values error!" );
	}
	Mess->put(test_cat,TMess::Info,"  Write a half buffer ok.");
	//Roll buff    	
	for(int i=buf->size()/2; i<buf->size()+5; i++)
	    switch(i)
	    {
		//Pass one value
		case 7: continue;
		//Drift values
		case 9: buf->setS("Test: "+TSYS::int2str(i),wtm+i*buf->period()+buf->period()/2); break;
		case 11:buf->setS("Test: "+TSYS::int2str(i),wtm+i*buf->period()-buf->period()/2); break;
		default:buf->setS("Test: "+TSYS::int2str(i),wtm+i*buf->period());
	    }
	if(!(buf->end()/buf->period() == wtm/buf->period()+buf->size()+4 && buf->begin()/buf->period() == wtm/buf->period()+4))
            throw TError("","Test4 failed! Buffer begin or/and end error, at roll filling buffer!");
	for(long long i = buf->begin(); i <= buf->end(); i+=buf->period())
	{
	    rtm = i;
	    if( !(((i-wtm)/buf->period() == 7 && buf->getS(&rtm) == EVAL_STR) || 
		    ((i-wtm)/buf->period() == 10 && buf->getS(&rtm) == "Test: 11" ) ||
		    ((i-wtm)/buf->period() == 11 && buf->getS(&rtm) == EVAL_STR) ||		
	    	    buf->getS(&rtm) == "Test: "+TSYS::int2str((i-wtm)/buf->period())) )
	    	throw TError("","Test4 failed! Write a roll buffer values error!" );
	}
	Mess->put(test_cat,TMess::Info,"  Write a roll buffer ok.");	
	rtm = buf->end();
	buf->setS("Test up.",rtm);
	if(buf->getS(&rtm) != "Test up.")
            throw TError("","Test4 failed! Update buffer end error!" );
	Mess->put(test_cat,TMess::Info,"  Update buffer end ok.");

	wtm += buf->period()*(buf->size()+5);
	st_cnt = SYS->shrtCnt();
	buf->size(1000);
	for(int i=0; i<buf->size(); i++)
	    buf->setS("Test: "+TSYS::int2str(i),wtm+i*buf->period());	
	Mess->put(test_cat,TMess::Info,"  Write 1000 values time %f ms!",1000.*(SYS->shrtCnt()-st_cnt)/SYS->sysClk());
	
	st_cnt = SYS->shrtCnt();
	for(long long i = buf->begin(); i <= buf->end(); i+=buf->period())
        { rtm = i; buf->getS(&rtm); }
	Mess->put(test_cat,TMess::Info,"  Read 1000 values time %f ms!",1000.*(SYS->shrtCnt()-st_cnt)/SYS->sysClk());
	
	Mess->put(test_cat,TMess::Info,"Test4 passed.");
        delete buf;

	//--------------------------- Test 5 ----------------------------------
	Mess->put(test_cat,TMess::Info,"Test5. Fill and check hard time griding integer buffer.");
	buf = new TValBuf( TFld::Dec, 10, 100000, true, true );
	wtm = buf->period()*(TSYS::curTime()/buf->period());
	//Not full fill
	for(int i=0; i<buf->size()/2; i++)
	    buf->setI(i,wtm+i*buf->period());
	if(!(buf->end()/buf->period() == wtm/buf->period()+(buf->size()/2)-1 && buf->begin()/buf->period() == wtm/buf->period()))
	    throw TError("","Test5 failed! Buffer begin or/and end error, at filling half buffer!" );
	for(long long i = buf->begin(); i <= buf->end(); i+=buf->period() )
	{
	    rtm = i;
	    if(buf->getI(&rtm) != (i-wtm)/buf->period())
	    	throw TError("","Test5 failed! Write a half buffer values error!" );
	}
	Mess->put(test_cat,TMess::Info,"  Write a half buffer ok.");
	//Roll buff    	
	for(int i=buf->size()/2; i<buf->size()+5; i++)
	    switch(i)
	    {
		//Pass one value
		case 7: continue;
		//Drift values
		case 9: buf->setI(i,wtm+i*buf->period()+buf->period()/2); break;
		case 11:buf->setI(i,wtm+i*buf->period()-buf->period()/2); break;
		default:buf->setI(i,wtm+i*buf->period());
	    }
	if(!(buf->end()/buf->period() == wtm/buf->period()+buf->size()+4 && buf->begin()/buf->period() == wtm/buf->period()+4))
            throw TError("","Test5 failed! Buffer begin or/and end error, at roll filling buffer!");
	for(long long i = buf->begin(); i <= buf->end(); i+=buf->period())
	{
	    rtm = i;
	    if( !(((i-wtm)/buf->period() == 7 && buf->getI(&rtm) == EVAL_INT) || 
		    ((i-wtm)/buf->period() == 10 && buf->getI(&rtm) == 11 ) ||
		    ((i-wtm)/buf->period() == 11 && buf->getI(&rtm) == EVAL_INT) ||		
	    	    buf->getI(&rtm) == (i-wtm)/buf->period()) )
	    	throw TError("","Test5 failed! Write a roll buffer values error!" );
	}
	Mess->put(test_cat,TMess::Info,"  Write a roll buffer ok.");	
	rtm = buf->end();
	buf->setI(100,rtm);
	if(buf->getI(&rtm) != 100)
            throw TError("","Test5 failed! Update buffer end error!" );
	Mess->put(test_cat,TMess::Info,"  Update buffer end ok.");
	
	wtm += buf->period()*(buf->size()+5);
        st_cnt = SYS->shrtCnt();
        buf->size(1000);
        for(int i=0; i<buf->size(); i++)
    	    buf->setI(i,wtm+i*buf->period());
        Mess->put(test_cat,TMess::Info,"  Write 1000 values time %f ms!",1000.*(SYS->shrtCnt()-st_cnt)/SYS->sysClk());
			    
        st_cnt = SYS->shrtCnt();
        for(long long i = buf->begin(); i <= buf->end(); i+=buf->period())
        { rtm = i; buf->getI(&rtm); }
        Mess->put(test_cat,TMess::Info,"  Read 1000 values time %f ms!",1000.*(SYS->shrtCnt()-st_cnt)/SYS->sysClk());
	
	Mess->put(test_cat,TMess::Info,"Test5 passed.");
        delete buf;
	
	//--------------------------- Test 6 ----------------------------------
	Mess->put(test_cat,TMess::Info,"Test6. Fill and check soft time griding string buffer (high time).");
	buf = new TValBuf( TFld::String, 10, 100000, false, true );
	wtm = buf->period()*(TSYS::curTime()/buf->period());
	//Not full fill
	for(int i=0; i<buf->size()/2; i++)
	    buf->setS("Test: "+TSYS::int2str(i),wtm+i*buf->period());
	if(!(buf->end()/buf->period() == wtm/buf->period()+(buf->size()/2)-1 && buf->begin()/buf->period() == wtm/buf->period()))
	    throw TError("","Test6 failed! Buffer begin or/and end error, at filling half buffer!" );
	for(long long i = buf->begin(); i <= buf->end(); i+=buf->period() )
	{
	    rtm = i;
	    if(buf->getS(&rtm) != "Test: "+TSYS::int2str((i-wtm)/buf->period()))
	    	throw TError("","Test6 failed! Write a half buffer values error!" );
	}
	Mess->put(test_cat,TMess::Info,"  Write a half buffer ok.");
	//Roll buff    	
	for(int i=buf->size()/2; i<buf->size()+5; i++)
	    switch(i)
	    {
		//Pass one value
		case 7: continue;
		//Drift values
		case 9: buf->setS("Test: "+TSYS::int2str(i),wtm+i*buf->period()+buf->period()/2); break;
		case 11:buf->setS("Test: "+TSYS::int2str(i),wtm+i*buf->period()-buf->period()/2); break;
		case 13:buf->setS("Test: "+TSYS::int2str(12),wtm+i*buf->period()); break;
		default:buf->setS("Test: "+TSYS::int2str(i),wtm+i*buf->period());
	    }
	if(!(buf->end()/buf->period() == wtm/buf->period()+buf->size()+4 && buf->begin()/buf->period() == wtm/buf->period()+3))
            throw TError("","Test6 failed! Buffer begin or/and end error, at roll filling buffer!" );
	for(long long i = buf->begin(); i <= buf->end(); i+=buf->period())
	{	    
	    rtm = i;
	    if( !(((i-wtm)/buf->period() == 7 && buf->getS(&rtm) == EVAL_STR) ||
		    ((i-wtm)/buf->period() == 10 && buf->getS(&rtm) == "Test: 11" ) ||
                    ((i-wtm)/buf->period() == 11 && buf->getS(&rtm) == EVAL_STR) ||
		    ((i-wtm)/buf->period() == 13 && buf->getS(&rtm) == "Test: 12") ||
	    	    buf->getS(&rtm) == "Test: "+TSYS::int2str((i-wtm)/buf->period())) )
	    	throw TError("","Test6 failed! Write a roll buffer values error!" );
	}
	Mess->put(test_cat,TMess::Info,"  Write a roll buffer ok.");
	rtm = buf->end();
        buf->setS("Test up.",rtm);
        if(buf->getS(&rtm) != "Test up.")
    	    throw TError("","Test6 failed! Update buffer end error!" );
	Mess->put(test_cat,TMess::Info,"  Update buffer end ok.");
	
	wtm += buf->period()*(buf->size()+5);
	st_cnt = SYS->shrtCnt();
	buf->size(1000);
	for(int i=0; i<buf->size(); i++)
	    buf->setS("Test: "+TSYS::int2str(i),wtm+i*buf->period());	
	Mess->put(test_cat,TMess::Info,"  Write 1000 values time %f ms!",1000.*(SYS->shrtCnt()-st_cnt)/SYS->sysClk());
	
	st_cnt = SYS->shrtCnt();
	for(long long i = buf->begin(); i <= buf->end(); i+=buf->period())
        { rtm = i; buf->getS(&rtm); }
	Mess->put(test_cat,TMess::Info,"  Read 1000 values time %f ms!",1000.*(SYS->shrtCnt()-st_cnt)/SYS->sysClk());	
	
	Mess->put(test_cat,TMess::Info,"Test6 passed.");
        delete buf;

	//--------------------------- Test 7 ----------------------------------
	Mess->put(test_cat,TMess::Info,"Test7. Fill and check soft time griding integer buffer (high time).");
	buf = new TValBuf( TFld::Dec, 10, 100000, false, true );
	wtm = buf->period()*(TSYS::curTime()/buf->period());
	//Not full fill
	for(int i=0; i<buf->size()/2; i++)
	    buf->setI(i,wtm+i*buf->period());
	if(!(buf->end()/buf->period() == wtm/buf->period()+(buf->size()/2)-1 && buf->begin()/buf->period() == wtm/buf->period()))
	    throw TError("","Test7 failed! Buffer begin or/and end error, at filling half buffer!" );
	for(long long i = buf->begin(); i <= buf->end(); i+=buf->period() )
	{
	    rtm = i;
	    if(buf->getI(&rtm) != (i-wtm)/buf->period())
	    	throw TError("","Test7 failed! Write a half buffer values error!" );
	}
	Mess->put(test_cat,TMess::Info,"  Write a half buffer ok.");
	//Roll buff    	
	for(int i=buf->size()/2; i<buf->size()+5; i++)
	    switch(i)
	    {
		//Pass one value
		case 7: continue;
		//Drift values
		case 9: buf->setI(i,wtm+i*buf->period()+buf->period()/2); break;
		case 11:buf->setI(i,wtm+i*buf->period()-buf->period()/2); break;
		case 13:buf->setI(12,wtm+i*buf->period()); break;
		default:buf->setI(i,wtm+i*buf->period());
	    }
	if(!(buf->end()/buf->period() == wtm/buf->period()+buf->size()+4 && buf->begin()/buf->period() == wtm/buf->period()+3))
            throw TError("","Test7 failed! Buffer begin or/and end error, at roll filling buffer!" );
	for(long long i = buf->begin(); i <= buf->end(); i+=buf->period())
	{	    
	    rtm = i;
	    if( !(((i-wtm)/buf->period() == 7 && buf->getI(&rtm) == EVAL_INT) ||
		    ((i-wtm)/buf->period() == 10 && buf->getI(&rtm) == 11 ) ||
                    ((i-wtm)/buf->period() == 11 && buf->getI(&rtm) == EVAL_INT) ||
		    ((i-wtm)/buf->period() == 13 && buf->getI(&rtm) == 12) ||
	    	    buf->getI(&rtm) == (i-wtm)/buf->period()) )
	    	throw TError("","Test7 failed! Write a roll buffer values error!" );
	}
	Mess->put(test_cat,TMess::Info,"  Write a roll buffer ok.");
	rtm = buf->end();
        buf->setI(1000,rtm);
        if(buf->getI(&rtm) != 1000)
    	    throw TError("","Test7 failed! Update buffer end error!" );
	Mess->put(test_cat,TMess::Info,"  Update buffer end ok.");
	
	wtm += buf->period()*(buf->size()+5);
        st_cnt = SYS->shrtCnt();
        buf->size(1000);
        for(int i=0; i<buf->size(); i++)
    	    buf->setI(i,wtm+i*buf->period());
        Mess->put(test_cat,TMess::Info,"  Write 1000 values time %f ms!",1000.*(SYS->shrtCnt()-st_cnt)/SYS->sysClk());
			    
        st_cnt = SYS->shrtCnt();
        for(long long i = buf->begin(); i <= buf->end(); i+=buf->period())
        { rtm = i; buf->getI(&rtm); }
        Mess->put(test_cat,TMess::Info,"  Read 1000 values time %f ms!",1000.*(SYS->shrtCnt()-st_cnt)/SYS->sysClk());	
	
	Mess->put(test_cat,TMess::Info,"Test7 passed.");
        delete buf;
	
	//--------------------------- Test 8 ----------------------------------
	Mess->put(test_cat,TMess::Info,"Test8. Fill and check soft time griding string buffer (low time).");
	buf = new TValBuf( TFld::String, 10, 1000000, false, false );
	wtm = buf->period()*(TSYS::curTime()/buf->period());
	//Not full fill
	for(int i=0; i<buf->size()/2; i++)
	    buf->setS("Test: "+TSYS::int2str(i),wtm+i*buf->period());
	if(!(buf->end()/buf->period() == wtm/buf->period()+(buf->size()/2)-1 && buf->begin()/buf->period() == wtm/buf->period()))
	    throw TError("","Test8 failed! Buffer begin or/and end error, at filling half buffer!" );
	for(long long i = buf->begin(); i <= buf->end(); i+=buf->period() )
	{
	    rtm = i;
	    if(buf->getS(&rtm) != "Test: "+TSYS::int2str((i-wtm)/buf->period()))
	    	throw TError("","Test8 failed! Write a half buffer values error!" );
	}
	Mess->put(test_cat,TMess::Info,"  Write a half buffer ok.");
	//Roll buff    	
	for(int i=buf->size()/2; i<buf->size()+5; i++)
	    switch(i)
	    {
		//Pass one value
		case 7: continue;
		//Drift values
		case 9: buf->setS("Test: "+TSYS::int2str(i),wtm+i*buf->period()+buf->period()/2); break;
		case 11:buf->setS("Test: "+TSYS::int2str(i),wtm+i*buf->period()-buf->period()/2); break;
		case 13:buf->setS("Test: "+TSYS::int2str(12),wtm+i*buf->period()); break;
		default:buf->setS("Test: "+TSYS::int2str(i),wtm+i*buf->period());
	    }
	if(!(buf->end()/buf->period() == wtm/buf->period()+buf->size()+4 && buf->begin()/buf->period() == wtm/buf->period()+3))
            throw TError("","Test8 failed! Buffer begin or/and end error, at roll filling buffer!" );
	for(long long i = buf->begin(); i <= buf->end(); i+=buf->period())
	{	    
	    rtm = i;
	    if( !(((i-wtm)/buf->period() == 7 && buf->getS(&rtm) == EVAL_STR) || 			
		    ((i-wtm)/buf->period() == 10 && buf->getS(&rtm) == "Test: 11") ||
                    ((i-wtm)/buf->period() == 11 && buf->getS(&rtm) == EVAL_STR) ||
	    	    ((i-wtm)/buf->period() == 13 && buf->getS(&rtm) == "Test: 12") ||
	    	    buf->getS(&rtm) == "Test: "+TSYS::int2str((i-wtm)/buf->period())) )
	    	throw TError("","Test6 failed! Write a roll buffer values error!" );
	}
	Mess->put(test_cat,TMess::Info,"  Write a roll buffer ok.");
	rtm = buf->end();
        buf->setS("Test up.",rtm);
        if(buf->getS(&rtm) != "Test up.")
    	    throw TError("","Test8 failed! Update buffer end error!" );
	Mess->put(test_cat,TMess::Info,"  Update buffer end ok.");
	
	wtm += buf->period()*(buf->size()+5);
	st_cnt = SYS->shrtCnt();
	buf->size(1000);
	for(int i=0; i<buf->size(); i++)
	    buf->setS("Test: "+TSYS::int2str(i),wtm+i*buf->period());	
	Mess->put(test_cat,TMess::Info,"  Write 1000 values time %f ms!",1000.*(SYS->shrtCnt()-st_cnt)/SYS->sysClk());
	
	st_cnt = SYS->shrtCnt();
	for(long long i = buf->begin(); i <= buf->end(); i+=buf->period())
        { rtm = i; buf->getS(&rtm); }
	Mess->put(test_cat,TMess::Info,"  Read 1000 values time %f ms!",1000.*(SYS->shrtCnt()-st_cnt)/SYS->sysClk());
	
	Mess->put(test_cat,TMess::Info,"Test8 passed.");
        delete buf;
	
	//--------------------------- Test 9 ----------------------------------
	Mess->put(test_cat,TMess::Info,"Test9. Fill and check soft time griding integer buffer (low time).");
	buf = new TValBuf( TFld::Dec, 10, 1000000, false, false );
	wtm = buf->period()*(TSYS::curTime()/buf->period());
	//Not full fill
	for(int i=0; i<buf->size()/2; i++)
	    buf->setI(i,wtm+i*buf->period());
	if(!(buf->end()/buf->period() == wtm/buf->period()+(buf->size()/2)-1 && buf->begin()/buf->period() == wtm/buf->period()))
	    throw TError("","Test9 failed! Buffer begin or/and end error, at filling half buffer!" );
	for(long long i = buf->begin(); i <= buf->end(); i+=buf->period() )
	{
	    rtm = i;
	    if(buf->getI(&rtm) != (i-wtm)/buf->period())
	    	throw TError("","Test9 failed! Write a half buffer values error!" );
	}
	Mess->put(test_cat,TMess::Info,"  Write a half buffer ok.");
	//Roll buff    	
	for(int i=buf->size()/2; i<buf->size()+5; i++)
	    switch(i)
	    {
		//Pass one value
		case 7: continue;
		//Drift values
		case 9: buf->setI(i,wtm+i*buf->period()+buf->period()/2); break;
		case 11:buf->setI(i,wtm+i*buf->period()-buf->period()/2); break;
		case 13:buf->setI(12,wtm+i*buf->period()); break;
		default:buf->setI(i,wtm+i*buf->period());
	    }
	if(!(buf->end()/buf->period() == wtm/buf->period()+buf->size()+4 && buf->begin()/buf->period() == wtm/buf->period()+3))
            throw TError("","Test9 failed! Buffer begin or/and end error, at roll filling buffer!" );
	for(long long i = buf->begin(); i <= buf->end(); i+=buf->period())
	{	    
	    rtm = i;
	    if( !(((i-wtm)/buf->period() == 7 && buf->getI(&rtm) == EVAL_INT) || 			
		    ((i-wtm)/buf->period() == 10 && buf->getI(&rtm) == 11) ||
                    ((i-wtm)/buf->period() == 11 && buf->getI(&rtm) == EVAL_INT) ||
	    	    ((i-wtm)/buf->period() == 13 && buf->getI(&rtm) == 12) ||
	    	    buf->getI(&rtm) == (i-wtm)/buf->period()) )
	    	throw TError("","Test9 failed! Write a roll buffer values error!" );
	}
	Mess->put(test_cat,TMess::Info,"  Write a roll buffer ok.");
	rtm = buf->end();
        buf->setI(1000,rtm);
        if(buf->getI(&rtm) != 1000)
    	    throw TError("","Test9 failed! Update buffer end error!" );
	Mess->put(test_cat,TMess::Info,"  Update buffer end ok.");	
	
	wtm += buf->period()*(buf->size()+5);
        st_cnt = SYS->shrtCnt();
        buf->size(1000);
        for(int i=0; i<buf->size(); i++)
    	    buf->setI(i,wtm+i*buf->period());
        Mess->put(test_cat,TMess::Info,"  Write 1000 values time %f ms!",1000.*(SYS->shrtCnt()-st_cnt)/SYS->sysClk());
			    
        st_cnt = SYS->shrtCnt();
        for(long long i = buf->begin(); i <= buf->end(); i+=buf->period())
        { rtm = i; buf->getI(&rtm); }
        Mess->put(test_cat,TMess::Info,"  Read 1000 values time %f ms!",1000.*(SYS->shrtCnt()-st_cnt)/SYS->sysClk());	
					    
	Mess->put(test_cat,TMess::Info,"Test9 passed.");
        delete buf;

	//--------------------------- Test 10 ----------------------------------
	Mess->put(test_cat,TMess::Info,"Test10. Fill and check free time string buffer (high time).");
	buf = new TValBuf( TFld::String, 10, 0, false, true );
	wtm = TSYS::curTime();
	int wper = 100000;
	//Not full fill
	for(int i=0; i<buf->size()/2; i++)
	    buf->setS("Test: "+TSYS::int2str(i),wtm+wper*i);
	if(!(buf->end() == wtm+wper*((buf->size()/2)-1) && buf->begin() == wtm) )
	    throw TError("","Test10 failed! Buffer begin or/and end error, at filling half buffer!" );
	int icnt = buf->size()/2 - 1;
	for(long long i = buf->end(); i >= buf->begin(); i--, icnt-- )
	    if(buf->getS(&i) != "Test: "+TSYS::int2str(icnt))
	    	throw TError("","Test10 failed! Write a half buffer values error!" );	    
	Mess->put(test_cat,TMess::Info,"  Write a half buffer ok.");
	//Roll buff    	
	for(int i=buf->size()/2; i<buf->size()+5; i++)
	    switch(i)
	    {
		//Pass one value
		case 7: continue;
		//Drift values
		case 9: buf->setS("Test: "+TSYS::int2str(i),wtm+wper*i+wper/2); break;
		case 11:buf->setS("Test: "+TSYS::int2str(i),wtm+wper*i-wper/2); break;
		case 13:buf->setS("Test: "+TSYS::int2str(i),wtm+wper*(i-1)); 	break;
		default:buf->setS("Test: "+TSYS::int2str(i),wtm+wper*i);
	    }
	if(!(buf->end() == wtm+wper*(buf->size()+4) && buf->begin() == wtm+3*wper) )
            throw TError("","Test10 failed! Buffer begin or/and end error, at roll filling buffer!" );
	icnt = buf->size()+4;
	for(long long i = buf->end(); i >= buf->begin(); i--, icnt--)
	{	    
	    //string val =  buf->getS(&rtm);
	    //printf("Value %lld: %s\n",rtm,val.c_str());
	    if(icnt == 12 || icnt == 7)	icnt--;
	    if( buf->getS(&i) != "Test: "+TSYS::int2str(icnt) )
	    	throw TError("","Test10 failed! Write a roll buffer values error!" );
	}
	Mess->put(test_cat,TMess::Info,"  Write a roll buffer ok.");	
	rtm = buf->end();
        buf->setS("Test up.",rtm);
        if(buf->getS(&rtm) != "Test up.")
            throw TError("","Test10 failed! Update buffer end error!" );
        Mess->put(test_cat,TMess::Info,"  Update buffer end ok.");
	
	wtm += wper*(buf->size()+5);
	st_cnt = SYS->shrtCnt();
	buf->size(1000);
	for(int i=0; i<buf->size(); i++)
	    buf->setS("Test: "+TSYS::int2str(i),wtm+i*wper);	
	Mess->put(test_cat,TMess::Info,"  Write 1000 values time %f ms!",1000.*(SYS->shrtCnt()-st_cnt)/SYS->sysClk());
	
	st_cnt = SYS->shrtCnt();
	for(long long i = buf->end(); i > buf->begin(); i=rtm-1)
        { rtm = i; buf->getS(&rtm); }
	Mess->put(test_cat,TMess::Info,"  Read 1000 values time %f ms!",1000.*(SYS->shrtCnt()-st_cnt)/SYS->sysClk());
		
	Mess->put(test_cat,TMess::Info,"Test10 passed.");
	delete buf;
	
	//--------------------------- Test 11 ----------------------------------
	Mess->put(test_cat,TMess::Info,"Test11. Fill and check free time integer buffer (high time).");
	buf = new TValBuf( TFld::Dec, 10, 0, false, true );
	wtm = TSYS::curTime();
	wper = 100000;
	//Not full fill
	for(int i=0; i<buf->size()/2; i++)
	    buf->setI(i,wtm+wper*i);
	if(!(buf->end() == wtm+wper*((buf->size()/2)-1) && buf->begin() == wtm) )
	    throw TError("","Test11 failed! Buffer begin or/and end error, at filling half buffer!" );
	icnt = buf->size()/2 - 1;
	for(long long i = buf->end(); i >= buf->begin(); i--, icnt-- )
	    if(buf->getI(&i) != icnt)
	    	throw TError("","Test11 failed! Write a half buffer values error!" );	    
	Mess->put(test_cat,TMess::Info,"  Write a half buffer ok.");
	//Roll buff    	
	for(int i=buf->size()/2; i<buf->size()+5; i++)
	    switch(i)
	    {
		//Pass one value
		case 7: continue;
		//Drift values
		case 9: buf->setI(i,wtm+wper*i+wper/2); break;
		case 11:buf->setI(i,wtm+wper*i-wper/2); break;
		case 13:buf->setI(i,wtm+wper*(i-1)); 	break;
		default:buf->setI(i,wtm+wper*i);
	    }
	if(!(buf->end() == wtm+wper*(buf->size()+4) && buf->begin() == wtm+3*wper) )
            throw TError("","Test11 failed! Buffer begin or/and end error, at roll filling buffer!" );
	icnt = buf->size()+4;
	for(long long i = buf->end(); i >= buf->begin(); i--, icnt--)
	{	    
	    if(icnt == 12 || icnt == 7)	icnt--;
	    if( buf->getI(&i) != icnt )
		throw TError("","Test11 failed! Write a roll buffer values error!" );
	}
	Mess->put(test_cat,TMess::Info,"  Write a roll buffer ok.");	
	rtm = buf->end();
        buf->setI(1000,rtm);
        if(buf->getI(&rtm) != 1000)
            throw TError("","Test11 failed! Update buffer end error!" );
        Mess->put(test_cat,TMess::Info,"  Update buffer end ok.");

	wtm += wper*(buf->size()+5);
	st_cnt = SYS->shrtCnt();
	buf->size(1000);
	for(int i=0; i<buf->size(); i++)
	    buf->setI(i,wtm+i*wper);	
	Mess->put(test_cat,TMess::Info,"  Write 1000 values time %f ms!",1000.*(SYS->shrtCnt()-st_cnt)/SYS->sysClk());
	
	st_cnt = SYS->shrtCnt();
	for(long long i = buf->end(); i > buf->begin(); i=rtm-1)
        { rtm = i; buf->getI(&rtm); }
	Mess->put(test_cat,TMess::Info,"  Read 1000 values time %f ms!",1000.*(SYS->shrtCnt()-st_cnt)/SYS->sysClk());	
	
	Mess->put(test_cat,TMess::Info,"Test11 passed.");
	delete buf;

	//--------------------------- Test 12 ----------------------------------
	Mess->put(test_cat,TMess::Info,"Test12. Fill and check free time string buffer (low time).");
	buf = new TValBuf( TFld::String, 10, 0, false, false );
	wper = 1000000;
	wtm = wper*(TSYS::curTime()/wper);
	//Not full fill
	for(int i=0; i<buf->size()/2; i++)
	    buf->setS("Test: "+TSYS::int2str(i),wtm+wper*i);
	if(!(buf->end() == wtm+wper*((buf->size()/2)-1) && buf->begin() == wtm) )
	    throw TError("","Test12 failed! Buffer begin or/and end error, at filling half buffer!" );
	icnt = buf->size()/2 - 1;
	for(long long i = buf->end(); i >= buf->begin(); i--, icnt-- )
	    if(buf->getS(&i) != "Test: "+TSYS::int2str(icnt))
	    	throw TError("","Test12 failed! Write a half buffer values error!" );	    
	Mess->put(test_cat,TMess::Info,"  Write a half buffer ok.");
	//Roll buff    	
	for(int i=buf->size()/2; i<buf->size()+5; i++)
	    switch(i)
	    {
		//Pass one value
		case 7: continue;
		//Drift values
		case 9: buf->setS("Test: "+TSYS::int2str(i),wtm+wper*i+wper/2); break;
		case 11:buf->setS("Test: "+TSYS::int2str(i),wtm+wper*i-wper/2); break;
		case 13:buf->setS("Test: "+TSYS::int2str(i),wtm+wper*(i-1)); 	break;
		default:buf->setS("Test: "+TSYS::int2str(i),wtm+wper*i);
	    }
	if(!(buf->end() == wtm+wper*(buf->size()+4) && buf->begin() == wtm+2*wper) )
            throw TError("","Test12 failed! Buffer begin or/and end error, at roll filling buffer!" );
	icnt = buf->size()+4;
	for(long long i = buf->end(); i >= buf->begin(); i--, icnt--)
	{
	    if(icnt == 12 || icnt == 10 || icnt == 7)	icnt--;
	    if( buf->getS(&i) != "Test: "+TSYS::int2str(icnt) )
	    	throw TError("","Test12 failed! Write a roll buffer values error!" );
	}
	Mess->put(test_cat,TMess::Info,"  Write a roll buffer ok.");	
	rtm = buf->end();
        buf->setS("Test up.",rtm);
        if(buf->getS(&rtm) != "Test up.")
            throw TError("","Test12 failed! Update buffer end error!" );
        Mess->put(test_cat,TMess::Info,"  Update buffer end ok.");
	
	wtm += wper*(buf->size()+5);
	st_cnt = SYS->shrtCnt();
	buf->size(1000);
	for(int i=0; i<buf->size(); i++)
	    buf->setS("Test: "+TSYS::int2str(i),wtm+i*wper);	
	Mess->put(test_cat,TMess::Info,"  Write 1000 values time %f ms!",1000.*(SYS->shrtCnt()-st_cnt)/SYS->sysClk());
	
	st_cnt = SYS->shrtCnt();
	for(long long i = buf->end(); i > buf->begin(); i=rtm-1)
        { rtm = i; buf->getS(&rtm); }
	Mess->put(test_cat,TMess::Info,"  Read 1000 values time %f ms!",1000.*(SYS->shrtCnt()-st_cnt)/SYS->sysClk());	
	
	Mess->put(test_cat,TMess::Info,"Test12 passed.");
	delete buf;

	//--------------------------- Test 13 ----------------------------------
	Mess->put(test_cat,TMess::Info,"Test13. Fill and check free time integer buffer (low time).");
	buf = new TValBuf( TFld::Dec, 10, 0, false, false );
	wper = 1000000;
	wtm = wper*(TSYS::curTime()/wper);
	//Not full fill
	for(int i=0; i<buf->size()/2; i++)
	    buf->setI(i,wtm+wper*i);
	if(!(buf->end() == wtm+wper*((buf->size()/2)-1) && buf->begin() == wtm) )
	    throw TError("","Test13 failed! Buffer begin or/and end error, at filling half buffer!" );
	icnt = buf->size()/2 - 1;
	for(long long i = buf->end(); i >= buf->begin(); i--, icnt-- )
	    if(buf->getI(&i) != icnt)
	    	throw TError("","Test13 failed! Write a half buffer values error!" );	    
	Mess->put(test_cat,TMess::Info,"  Write a half buffer ok.");
	//Roll buff    	
	for(int i=buf->size()/2; i<buf->size()+5; i++)
	    switch(i)
	    {
		//Pass one value
		case 7: continue;
		//Drift values
		case 9: buf->setI(i,wtm+wper*i+wper/2); break;
		case 11:buf->setI(i,wtm+wper*i-wper/2); break;
		case 13:buf->setI(i,wtm+wper*(i-1)); 	break;
		default:buf->setI(i,wtm+wper*i);
	    }
	if(!(buf->end() == wtm+wper*(buf->size()+4) && buf->begin() == wtm+2*wper) )
            throw TError("","Test13 failed! Buffer begin or/and end error, at roll filling buffer!" );
	icnt = buf->size()+4;
	for(long long i = buf->end(); i >= buf->begin(); i--, icnt--)
	{	    
	    if(icnt == 12 || icnt == 10 || icnt == 7)	icnt--;
	    if( buf->getI(&i) != icnt )
	    	throw TError("","Test13 failed! Write a roll buffer values error!" );
	}
	Mess->put(test_cat,TMess::Info,"  Write a roll buffer ok.");	
	rtm = buf->end();
        buf->setI(100,rtm);
        if(buf->getI(&rtm) != 100)
            throw TError("","Test13 failed! Update buffer end error!" );
        Mess->put(test_cat,TMess::Info,"  Update buffer end ok.");

	wtm += wper*(buf->size()+5);
	st_cnt = SYS->shrtCnt();
	buf->size(1000);
	for(int i=0; i<buf->size(); i++)
	    buf->setI(i,wtm+i*wper);	
	Mess->put(test_cat,TMess::Info,"  Write 1000 values time %f ms!",1000.*(SYS->shrtCnt()-st_cnt)/SYS->sysClk());
	
	st_cnt = SYS->shrtCnt();
	for(long long i = buf->end(); i > buf->begin(); i=rtm-1)
        { rtm = i; buf->getI(&rtm); }
	Mess->put(test_cat,TMess::Info,"  Read 1000 values time %f ms!",1000.*(SYS->shrtCnt()-st_cnt)/SYS->sysClk());	
	
	Mess->put(test_cat,TMess::Info,"Test13 passed.");
	delete buf;
    }
    else if(id == "Archive")
    {
	string arch   = t_n->attr("arch");
	long long per = atoll(t_n->attr("period").c_str());
	
	Mess->put(test_cat,TMess::Info,"*** Archive <%s> tests. ***",arch.c_str());
	AutoHD<TVArchive> o_arch = SYS->archive().at().valAt(arch);
	
	int buf_sz = 5;
	long long wtm = per*(TSYS::curTime()/per);
	long long ttm;
	
	TValBuf buf(TFld::Dec, buf_sz, per, true, false );
	//--------------------------- Test 1 ----------------------------------
        Mess->put(test_cat,TMess::Info,"Test1. Simple fill and check archive.");
	for( int i_el = 0; i_el < buf_sz; i_el++)
	    buf.setI((int)pow(10,i_el),wtm+i_el*per);	
	o_arch.at().setVal(buf,buf.begin(),buf.end(),"");
	for( int i_el = 0; i_el < buf_sz+2; i_el++)
	{
	    ttm = wtm+i_el*per;
	    int val = o_arch.at().getI(&ttm);
	    if(	(i_el < buf_sz && val != pow(10,i_el)) ||
		    (i_el >= buf_sz && val != EVAL_INT) )
		throw TError("","Test1 failed!" );
	}
	Mess->put(test_cat,TMess::Info,"Test1 passed.");
	//--------------------------- Test 2 ----------------------------------
        Mess->put(test_cat,TMess::Info,"Test2. Internal insert double value (down).");	
	buf.clear();
	buf.setI((int)pow(10,2),wtm+3*per);
	o_arch.at().setVal(buf,wtm+3*per,wtm+3*per,"");
	for( int i_el = 0; i_el < buf_sz+2; i_el++)
        {
            ttm = wtm+i_el*per;
            if( (i_el < buf_sz && i_el != 3 && o_arch.at().getI(&ttm) != pow(10,i_el)) || 
		    (i_el < buf_sz && i_el == 3 && o_arch.at().getI(&ttm) != pow(10,2)) ||
		    (i_el >= buf_sz && o_arch.at().getI(&ttm) != EVAL_INT) )
        	throw TError("","Test2 failed!" );
        }
	Mess->put(test_cat,TMess::Info,"Test2 passed.");
	//--------------------------- Test 3 ----------------------------------
	Mess->put(test_cat,TMess::Info,"Test3. Internal insert double value (up).");
        buf.clear();
	buf.setI((int)pow(10,4),wtm+3*per);
	o_arch.at().setVal(buf,wtm+3*per,wtm+3*per,"");
	for( int i_el = 0; i_el < buf_sz+2; i_el++)
        {
    	    ttm = wtm+i_el*per;
            if( (i_el < buf_sz && i_el != 3 && o_arch.at().getI(&ttm) != pow(10,i_el)) ||
            	    (i_el < buf_sz && i_el == 3 && o_arch.at().getI(&ttm) != pow(10,4)) ||
		    (i_el >= buf_sz && o_arch.at().getI(&ttm) != EVAL_INT) )
                throw TError("","Test3 failed!" );
        }
	Mess->put(test_cat,TMess::Info,"Test3 passed.");
	//--------------------------- Test 4 ----------------------------------
        Mess->put(test_cat,TMess::Info,"Test4. Internal insert double value (down).");	
	buf.clear();
	buf.setI((int)pow(10,2),wtm+3*per);
	o_arch.at().setVal(buf,wtm+3*per,wtm+3*per,"");
	for( int i_el = 0; i_el < buf_sz+2; i_el++)
        {
            ttm = wtm+i_el*per;
            if( (i_el < buf_sz && i_el != 3 && o_arch.at().getI(&ttm) != pow(10,i_el)) || 
		    (i_el < buf_sz && i_el == 3 && o_arch.at().getI(&ttm) != pow(10,2)) ||
		    (i_el >= buf_sz && o_arch.at().getI(&ttm) != EVAL_INT) )
        	throw TError("","Test4 failed!" );
        }
	Mess->put(test_cat,TMess::Info,"Test4 passed.");
	//--------------------------- Test 5 ----------------------------------
        Mess->put(test_cat,TMess::Info,"Test5. Internal insert no double value.");	
	buf.clear();
	buf.setI((int)pow(10,9),wtm+per);
	o_arch.at().setVal(buf,wtm+per,wtm+per,"");
	for( int i_el = 0; i_el < buf_sz+2; i_el++)
        {
            ttm = wtm+i_el*per;
            if( (i_el < buf_sz && i_el != 3 && i_el != 1 && o_arch.at().getI(&ttm) != pow(10,i_el)) || 
		    (i_el < buf_sz && i_el == 3 && o_arch.at().getI(&ttm) != pow(10,2)) ||
		    (i_el < buf_sz && i_el == 1 && o_arch.at().getI(&ttm) != pow(10,9)) ||
		    (i_el >= buf_sz && o_arch.at().getI(&ttm) != EVAL_INT) )
        	throw TError("","Test5 failed!" );
        }
	Mess->put(test_cat,TMess::Info,"Test5 passed.");
	//--------------------------- Test 6 ----------------------------------
        Mess->put(test_cat,TMess::Info,"Test6. Internal insert double (up) value.");	
	buf.clear();
	buf.setI((int)pow(10,2),wtm+per);
	o_arch.at().setVal(buf,wtm+per,wtm+per,"");
	for( int i_el = 0; i_el < buf_sz+2; i_el++)
        {
            ttm = wtm+i_el*per;
            if( (i_el < buf_sz && i_el != 3 && i_el != 1 && o_arch.at().getI(&ttm) != pow(10,i_el)) || 
		    (i_el < buf_sz && i_el == 3 && o_arch.at().getI(&ttm) != pow(10,2)) ||
		    (i_el < buf_sz && i_el == 1 && o_arch.at().getI(&ttm) != pow(10,2)) ||
		    (i_el >= buf_sz && o_arch.at().getI(&ttm) != EVAL_INT) )
        	throw TError("","Test6 failed!" );
        }
	Mess->put(test_cat,TMess::Info,"Test6 passed.");
	//--------------------------- Test 7 ----------------------------------
        Mess->put(test_cat,TMess::Info,"Test7. Internal insert value instaead double value.");	
	buf.clear();
	buf.setI((int)pow(10,3),wtm+3*per);
	o_arch.at().setVal(buf,wtm+3*per,wtm+3*per,"");
	for( int i_el = 0; i_el < buf_sz+2; i_el++)
        {
            ttm = wtm+i_el*per;
            if( (i_el < buf_sz && i_el != 1 && o_arch.at().getI(&ttm) != pow(10,i_el)) || 
		    (i_el < buf_sz && i_el == 1 && o_arch.at().getI(&ttm) != pow(10,2)) ||
		    (i_el >= buf_sz && o_arch.at().getI(&ttm) != EVAL_INT) )
        	throw TError("","Test7 failed!" );
        }
	Mess->put(test_cat,TMess::Info,"Test7 passed.");
	//--------------------------- Test 8 ----------------------------------
        /*sleep(2);
	Mess->put(test_cat,TMess::Info,"Test8. Set three values to end.");
        wtm = o_arch.at().end("");
	buf.clear();
	for( int i_el = -1; i_el <= 1; i_el++ )
	    buf.setI(i_el,wtm+i_el*per);
        o_arch.at().setVal(buf,buf.begin(),buf.end(),"");
	for( int i_el = -1; i_el <= 1; i_el++)
        {
	    ttm = wtm+i_el*per;
            if( o_arch.at().getI(&ttm) != i_el )
                throw TError("","Test8 failed!" );
        }
        Mess->put(test_cat,TMess::Info,"Test8 passed.");*/
    }
}

void TTest::pr_XMLNode( const char *cat, XMLNode *node, int level )
{
    char *buf = (char *)malloc(level+1);
    for(int i_c = 0; i_c < level; i_c++) buf[i_c] = ' ';
    buf[level] = 0;
	
    vector<string> list;
    Mess->put(cat,TMess::Info,"%s{%d <%s>, text <%s>, childs - %d!",
	buf, level, node->name().c_str(),node->text().c_str(),node->childSize());
    node->attrList(list);
    for(unsigned i_att = 0; i_att < list.size(); i_att++)
	Mess->put(cat,TMess::Info,"        Attr <%s> = <%s>!",list[i_att].c_str(),node->attr(list[i_att]).c_str());	
    for(int i_ch = 0; i_ch < node->childSize(); i_ch++)
	pr_XMLNode( cat, node->childGet(i_ch), level+1 ); 
    Mess->put(cat,TMess::Info,"%s}%d <%s>", buf, level, node->name().c_str());
    free(buf);
}

