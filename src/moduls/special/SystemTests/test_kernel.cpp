
//OpenSCADA system module Special.SystemTests file: test_kernel.cpp
/***************************************************************************
 *   Copyright (C) 2003-2008 by Roman Savochenko                           *
 *   rom_as@fromru.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
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
#include <string.h>

#include "test_kernel.h"

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"SystemTests"
#define MOD_NAME	"OpenSCADA system's tests"
#define MOD_TYPE	"Special"
#define VER_TYPE	VER_SPC
#define SUB_TYPE	"TEST"
#define VERSION		"1.3.6"
#define AUTORS		"Roman Savochenko"
#define DESCRIPTION	"Allow the group tests for OpenSCADA system."
#define LICENSE		"GPL"
//*************************************************

KernelTest::TTest *KernelTest::mod;

extern "C"
{
    TModule::SAt module( int n_mod )
    {
	if( n_mod==0 )	return TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE);
	return TModule::SAt("");
    }

    TModule *attach( const TModule::SAt &AtMod, const string &source )
    {
	if( AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE) )
	    return new KernelTest::TTest( source );
	return NULL;
    }
}

using namespace KernelTest;

//*************************************************
//* BDTest::TTest                                 *
//*************************************************
TTest::TTest( string name )
{
    mId		= MOD_ID;
    mName	= MOD_NAME;
    mType	= MOD_TYPE;
    mVers	= VERSION;
    mAutor	= AUTORS;
    mDescr	= DESCRIPTION;
    mLicense	= LICENSE;
    mSource	= name;

    mod		= this;
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

    snprintf(buf,sizeof(buf),_(
	"======================= The module <%s:%s> options =======================\n"
	"---------- Parameters of the module section <%s> in config file ----------\n"
	"All tests main options:\n"
	"  id           test's id;\n"
	"  on           on test's flag;\n"
	"  per          repeat period (sek).\n"
	"       *** Test's options ***\n"
	"PARAM        Parameter test:\n"
	"  name         paremeter full name (OperationSystem.AutoDA.CPULoad).\n"
	"XML          XML parsing test:\n"
	"  file         file for XML parsing.\n"
	"MESS         Messages archive test:\n"
	"  arhtor       archivator name (StatErrors.BaseArh);\n"
	"  categ        messages category pattern.\n"
	"SOAttDet     Attach/Detach module test:\n"
	"  name         name modul (path to module);\n"
	"  full         full attach(to start).\n"
	"Val          Parameter atributes value test:\n"
	"  name         parameter attribute (OperationSystem.AutoDA.CPULoad.load);\n"
	"  arch_len     archive value getting depth;\n"
	"  arch_per     archive value getting period.\n"
	"BD           Full database test:\n"
	"  type         type BD;\n"
	"  bd           name BD;\n"
	"  table        table;\n"
	"  size         fields number.\n"
	"TrOut        Output transport test:\n"
	"  addr         input transport address;\n"
	"  type         transport type;\n"
	"  req          request to a input transport.\n"
	"Func	      Function API test;\n"
	"SysContrLang System control language test:\n"
	"  path         path to language element (/Archive/BaseArh/mess_StatErrors/%%2fprm%%2fst).\n"
	"ValBuf       Value buffer tests;\n"
	"Archive      Value archive allocation tests:\n"
	"  arch         value archive;\n"
	"  period       values period (us).\n"
	"Base64Code   Mime Base64 algorithm tests.\n"
	"\n"),
	MOD_TYPE,MOD_ID,nodePath().c_str());

    return buf;
}

void TTest::load_( )
{
    //- Load parameters from command line -
    int next_opt;
    const char *short_opt="h";
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
	    case 'h': fprintf(stdout,"%s",optDescr().c_str()); break;
	    case -1 : break;
	}
    } while(next_opt != -1);

    //- Load parameters from config file -
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
	throw TError(nodePath().c_str(),_("Not started!"));
}

void TTest::modStop(  )
{
    if( !run_st ) return;

    endrun = true;
    if( TSYS::eventWait( run_st, false, string(MOD_ID)+": Is stopping....",5) )
	throw TError(nodePath().c_str(),_("Not stopped!"));
    pthread_join( pthr_tsk, NULL );
}

void *TTest::Task( void *CfgM )
{
    int count = 0, i_cnt = 0;

    TTest *tst = (TTest *)CfgM;
    tst->run_st = true;
    tst->endrun = false;

#if OSC_DEBUG >= 2
    mess_debug(tst->nodePath().c_str(),_("Thread <%u> is started. TID: %ld"),pthread_self(),(long int)syscall(224));
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
	    ResAlloc res(SYS->nodeRes(),false);
	    XMLNode *mn, *t_n;
	    mn = TCntrNode::ctrId(&SYS->cfgRoot(),tst->nodePath(),true);
	    if( mn )
		for( int nd_cnt = 0; t_n=mn->childGet("prm",nd_cnt++,true); )
		    if( t_n->attr("on") == "1" && atoi(t_n->attr("per").c_str()) && !( count % atoi(t_n->attr("per").c_str()) ) )
		    {
			string id = t_n->attr("id");
			try{ tst->Test( id, t_n ); }
			catch( TError err ) { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
		    }
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
	string paddr = t_n->attr("name");
	AutoHD<TParamContr> prm = SYS->daq().at().at(TSYS::strSepParse(paddr,0,'.')).at().
					          at(TSYS::strSepParse(paddr,1,'.')).at().
					          at(TSYS::strSepParse(paddr,2,'.'));
	mess_info(test_cat,"-------- Begin parameter <%s> test ----------",t_n->attr("name").c_str());

	vector<string> list_el;
	list_el.clear();
	prm.at().vlList(list_el);
	mess_info(test_cat,"Value attrbutes present: %d",list_el.size());
	for(int i=0; i< list_el.size(); i++)
	{
	    AutoHD<TVal> val = prm.at().vlAt(list_el[i]);
	    if( val.at().fld().flg()&TFld::Selected )
		mess_info(test_cat,"%s(SELECT): %s",list_el[i].c_str(), val.at().getSEL().c_str() );
	    switch(val.at().fld().type())
	    {
		case TFld::String:
		    mess_info(test_cat,"%s(STRING): %s",list_el[i].c_str(), val.at().getS().c_str() );
		    break;
		case TFld::Integer:
		    mess_info(test_cat,"%s(INTEGER): %d",list_el[i].c_str(), val.at().getI() );
		    break;
		case TFld::Real:
		    mess_info(test_cat,"%s(REAL): %f",list_el[i].c_str(), val.at().getR() );
		    break;
		case TFld::Boolean:
		    mess_info(test_cat,"%s(BOOLEAN): %d",list_el[i].c_str(), val.at().getB() );
		    break;
	    }
	}

	prm.at().cfgList(list_el);
	mess_info(test_cat,"Configs throw control: %d",list_el.size());
	for(int i=0; i< list_el.size(); i++)
	{
	    if(prm.at().cfg(list_el[i]).fld().flg()&TFld::Selected)
		mess_info(test_cat,"%s(SELECT): %s",list_el[i].c_str(), prm.at().cfg(list_el[i]).getSEL().c_str() );
	    switch(prm.at().cfg(list_el[i]).fld().type())
	    {
		case TFld::String:
		    mess_info(test_cat,"%s(STRING): %s",list_el[i].c_str(), prm.at().cfg(list_el[i]).getS().c_str() );
		    break;
		case TFld::Integer:
		    mess_info(test_cat,"%s(INTEGER): %d",list_el[i].c_str(), prm.at().cfg(list_el[i]).getI() );
		    break;
		case TFld::Real:
		    mess_info(test_cat,"%s(REAL): %f",list_el[i].c_str(), prm.at().cfg(list_el[i]).getR() );
		    break;
		case TFld::Boolean:
		    mess_info(test_cat,"%s(BOOLEAN): %d",list_el[i].c_str(), prm.at().cfg(list_el[i]).getB() );
		    break;
	    }
	}
	/*XMLNode node("info");
	node.attr("path","")->attr("user","root");
	prm.at().cntrCmd(&node);
	pr_XMLNode( test_cat, &node, 0 );*/

	mess_info(test_cat,"-------- End parameter <%s> test ----------",t_n->attr("name").c_str());
    }

    //XML parsing test
    else if(id == "XML" )
    {
	int hd = open(t_n->attr("file").c_str(),O_RDONLY);
	if(hd > 0)
	{
	    mess_info(test_cat,"-------- Begin XML parsing test ----------");
	    int cf_sz = lseek(hd,0,SEEK_END);
	    lseek(hd,0,SEEK_SET);
	    char *buf = (char *)malloc(cf_sz);
	    read(hd,buf,cf_sz);
	    close(hd);
	    string s_buf(buf,cf_sz);
	    free(buf);
	    XMLNode node;
	    node.load(s_buf);
	    pr_XMLNode( test_cat, &node, 0 );
	    mess_info(test_cat,"-------- End XML parsing test ----------");
	}
    }

    //Message arhive test
    else if(id == "MESS" )
    {
	AutoHD<TArchiveS> Arh_s = owner().owner().archive();

	string n_arhtor = t_n->attr("arhtor");
	mess_info(test_cat,"-------- Begin message archive test for archivator %s ----------",n_arhtor.c_str());
	vector<TMess::SRec> buf_rec;
	SYS->archive().at().messGet(time(NULL)-2*atoi(t_n->attr("per").c_str()),time(NULL),buf_rec,t_n->attr("categ"),TMess::Debug,n_arhtor);
	mess_info(test_cat,"New messages present %d.",buf_rec.size() );
	for(unsigned i_rec = 0; i_rec < buf_rec.size(); i_rec++)
	{
	    char *c_tm = ctime( &buf_rec[i_rec].time);
	    for( int i_ch = 0; i_ch < strlen(c_tm); i_ch++ )
		if( c_tm[i_ch] == '\n' ) c_tm[i_ch] = '\0';
	    mess_info(test_cat,"<%s> : <%s> : <%s>",c_tm, buf_rec[i_rec].categ.c_str(), buf_rec[i_rec].mess.c_str() );
	}
	mess_info(test_cat,"-------- End message archive test for archivator %s ----------",n_arhtor.c_str());
    }

    //Librarry attach/detach test
    else if(id == "SOAttDet" )
    {
	//AutoHD<TTransportIn> tr = ((TTipTransport &)owner().owner().Transport().modAt("socket").at()).inAt("www");
	AutoHD<TModSchedul> sched = owner().owner().modSchedul();
	string SO_name = t_n->attr("name");
	TModSchedul::SHD &so_st = sched.at().lib(SO_name);
	mess_info(test_cat,"-------- Begin SO <%s> test ----------",so_st.name.c_str());
	if( so_st.hd ) sched.at().libDet( so_st.name );
	else           sched.at().libAtt( so_st.name,(bool)atoi( t_n->attr("full").c_str()) );
	mess_info(test_cat,"-------- End SO <%s> test ----------",so_st.name.c_str());
    }

    //Parameter value test
    else if(id == "Val")
    {
	string s_prm = t_n->attr("name");
	int a_len = atoi(t_n->attr("arch_len").c_str());
	int a_per = atoi(t_n->attr("arch_per").c_str());

	mess_info(test_cat,"Value of: %s.",s_prm.c_str());

	AutoHD<TVal> val = SYS->daq().at().at(TSYS::strSepParse(s_prm,0,'.')).at().
					    at(TSYS::strSepParse(s_prm,1,'.')).at().
					    at(TSYS::strSepParse(s_prm,2,'.')).at().
					    vlAt(TSYS::strSepParse(s_prm,3,'.'));
	//SYS->nodeAt(s_prm,0,'.');
	mess_info(test_cat,"Last value = %s", val.at().getS(NULL).c_str() );
	if( a_len && a_per )
	{
	    long long cur = TSYS::curTime()-a_per;
	    for( int i_v = 1; i_v <= a_len; i_v++, cur-=a_per)
	    {
		long long rvtm = cur;
		mess_info(test_cat,"Value %d = %s.",i_v, val.at().getS(&rvtm).c_str() );  
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

	mess_info(test_cat,"***** Begin DB tests block *****");

	mess_info(test_cat,"Open DB: <%s>",n_bd.c_str());

	bd.at().open(n_bd);
	bd.at().at(n_bd).at().setAddr(bd_addr);
	bd.at().at(n_bd).at().enable();

	mess_info(test_cat,"Open Table: <%s>",n_tbl.c_str());
	bd.at().at(n_bd).at().open(n_tbl,true);
	mess_info(test_cat,"Connect to table: <%s>",n_tbl.c_str());
	AutoHD<TTable> tbl = bd.at().at(n_bd).at().at(n_tbl);

	mess_info(test_cat,"Create DB config");
	TConfig bd_cfg;
	bd_cfg.elem().fldAdd( new TFld("name","Name fields",TFld::String,TCfg::Key,"20") );
	bd_cfg.elem().fldAdd( new TFld("descr","Description fields",TFld::String,0,"50") );
	bd_cfg.elem().fldAdd( new TFld("val","Field value",TFld::Real,0,"10.2","5") );
	bd_cfg.elem().fldAdd( new TFld("id","Field id",TFld::Integer,0,"7","34") );
	bd_cfg.elem().fldAdd( new TFld("stat","Field stat",TFld::Boolean,0,"1","1") );

	//Test of The create fields
	mess_info(test_cat,"Create fields!");
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
	mess_info(test_cat,"Create %d fields time <%f>sek!",experem,(float)(times(NULL)-st_time)/TSYS::HZ());

	//Check update fields
	mess_info(test_cat,"Update fields!");
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
	mess_info(test_cat,"Update %d fields time <%f>sek!",experem,(float)(times(NULL)-st_time)/TSYS::HZ());

	//Check get of fields
	mess_info(test_cat,"Check fields!");
	st_time = times(NULL);
	for(int i_fld = 0; i_fld < experem; i_fld++)
	{
	    bd_cfg.cfg("name").setS("Sh"+SYS->int2str(i_fld));
	    tbl.at().fieldGet(bd_cfg);

	    if( i_fld == 155 )
		mess_info(test_cat,"Field #155=<%s>; Descr=<%s>; Value=<%f>; Id=<%d>; Stat=<%d>!",
		    bd_cfg.cfg("name").getS().c_str(), bd_cfg.cfg("descr").getS().c_str(),
		    bd_cfg.cfg("val").getR(), bd_cfg.cfg("id").getI(), bd_cfg.cfg("stat").getB() );

	    if( bd_cfg.cfg("name").getS() != (string("Sh")+SYS->int2str(i_fld)) )
		mess_info(test_cat,"Field <Sh> <%s>!=<%s> error!",
		    bd_cfg.cfg("name").getS().c_str(),(string("Sh")+SYS->int2str(i_fld)).c_str());
	    if( bd_cfg.cfg("descr").getS() != (string("New shifr ")+SYS->int2str(i_fld)) )
		mess_info(test_cat,"Field <descr> <%s>!=<%s> error!",
		    bd_cfg.cfg("descr").getS().c_str(),(string("New shifr ")+SYS->int2str(i_fld)).c_str() );
		    //ceil(100.*bd_cfg.cfg("val").getR()) != ceil(2.*sqrt(i_fld)) ||
	    if( bd_cfg.cfg("id").getI() != (2*i_fld) )
		mess_info(test_cat,"Field <id> <%d>!=<%d> error!",
		    bd_cfg.cfg("id").getI(), (2*i_fld) );
	    if( bd_cfg.cfg("stat").getB() != ((i_fld%2)==0?false:true) )
		mess_info(test_cat,"Field <stat> <%d>!=<%d> error!",
		    bd_cfg.cfg("stat").getB(), ((i_fld%2)==0?false:true) );
	}
	mess_info(test_cat,"Get %d fields time <%f>sek!",experem,(float)(times(NULL)-st_time)/TSYS::HZ());

	//Check Fix structure
	mess_info(test_cat,"Change DB structure!");
	//Add column
	bd_cfg.elem().fldAdd( new TFld("fix","BD fix test",TFld::String,0,"20") );
	bd_cfg.cfg("name").setS("Sh1");
	tbl.at().fieldSet(bd_cfg);
	bd_cfg.cfg("name").setS("Sh2");
	tbl.at().fieldGet(bd_cfg);
	if( bd_cfg.cfg("name").getS() != "Sh2" || bd_cfg.cfg("descr").getS() != "New shifr 2" ||
	    bd_cfg.cfg("id").getI() != 4 || bd_cfg.cfg("stat").getB() != false )
	{
	    mess_info(test_cat,"Add column error!");
	    mess_info(test_cat,"Field #2=<%s>; Descr=<%s>; Value=<%f>; Id=<%d>; Stat=<%d>!",
		bd_cfg.cfg("name").getS().c_str(), bd_cfg.cfg("descr").getS().c_str(),
		bd_cfg.cfg("val").getR(), bd_cfg.cfg("id").getI(), bd_cfg.cfg("stat").getB() );
	}
	else
	    mess_info(test_cat,"Add column ok!");

	//Del column
	bd_cfg.elem().fldDel(bd_cfg.elem().fldId("fix"));
	bd_cfg.cfg("name").setS("Sh1");
	tbl.at().fieldSet(bd_cfg);
	bd_cfg.cfg("name").setS("Sh2");
	tbl.at().fieldGet(bd_cfg);
	if( bd_cfg.cfg("name").getS() != "Sh2" || bd_cfg.cfg("descr").getS() != "New shifr 2" ||
	    bd_cfg.cfg("id").getI() != 4 || bd_cfg.cfg("stat").getB() != false )
	{
	    mess_info(test_cat,"Del column error!");
	    mess_info(test_cat,"Field #2=<%s>; Descr=<%s>; Value=<%f>; Id=<%d>; Stat=<%d>!",
		bd_cfg.cfg("name").getS().c_str(), bd_cfg.cfg("descr").getS().c_str(),
		bd_cfg.cfg("val").getR(), bd_cfg.cfg("id").getI(), bd_cfg.cfg("stat").getB() );
	}
	else
	    mess_info(test_cat,"Del column ok!");

	//Check List
	//vector<string> ls_elem;
	//tbl.at().fieldList("name",ls_elem);
	//if( ls_elem.size() != experem ) mess_info(test_cat,"List size error!");

	//Delete fields
	st_time = times(NULL);
	for( int i_fld = 0; i_fld < experem; i_fld++ )
	{
	    bd_cfg.cfg("name").setS("Sh"+SYS->int2str(i_fld),true);
	    tbl.at().fieldDel(bd_cfg);
	}
	mess_info(test_cat,"Del %d fields time <%f>sek!",experem,(float)(times(NULL)-st_time)/TSYS::HZ());

	tbl.free();

	mess_info(test_cat,"Close and delete table: <%s>",n_tbl.c_str());
	bd.at().at(n_bd).at().close(n_tbl,true);

	mess_info(test_cat,"Close and delete DB: <%s>",n_bd.c_str());
	bd.at().close(n_bd,true);

	bd.free();

	mess_info(test_cat,"***** End BD tests block *****");
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
	    tr.at().outAt(addr).at().setAddr(addr);
	}
	if( !tr.at().outAt(addr).at().startStat() ) tr.at().outAt(addr).at().start();
	int len = tr.at().outAt(addr).at().messIO(req.c_str(),req.size(),buf,sizeof(buf)-1,1000);
	tr.at().outAt(addr).at().stop();
	buf[len] = 0;
	mess_info(test_cat,"%s: Put <%s>. Get: <%s>",addr.c_str(),req.c_str(),buf);
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
	mess_info(test_cat,"TLibrarieS tests.");
	//--------------------------- Test 1 ----------------------------------
	mess_info(test_cat,"Test1.");
	mess_info(test_cat,"Create the true library.");
	TestLib *tlib = new TestLib("testLib");
	
	mess_info(test_cat,"Register the true library.");
	owner().owner().func().at().reg( tlib );

	mess_info(test_cat,"Present check of library.");
	if( !owner().owner().func().at().present("testLib") )
	{
	    owner().owner().func().at().unreg("testLib");
	    throw TError("","Test1 failed! Present check error!" );
	}

	mess_info(test_cat,"Libraries list check.");
	owner().owner().func().at().list(lst);
	for( i_ls = 0; i_ls < lst.size(); i_ls++ )
	    if( lst[i_ls] == "testLib" ) break;
	if( i_ls >= lst.size() ) throw TError("","Test1 failed! Libraries list error!" );
	mess_info(test_cat,"Test1 passed!");

	//--------------------------- Test 2 ----------------------------------
	mess_info(test_cat,"Test2.");
	err_ok = false;
	mess_info(test_cat,"Register the double library.");
	try{ owner().owner().func().at().reg( tlib ); }
	catch( TError err )
	{
	    mess_info(test_cat,"Register the double library exception: %s",err.mess.c_str());
	    err_ok = true;
	}
	if( !err_ok )
	{
	    owner().owner().func().at().unreg("testLib");
	    throw TError("","Test2 failed! Register double library error!" );
	}
	mess_info(test_cat,"Test2 passed!");

	//--------------------------- Test 3 ----------------------------------
	mess_info(test_cat,"Test3.");
	mess_info(test_cat,"Library access check.");
	AutoHD<TLibFunc> hd_lb = owner().owner().func().at().at("testLib");

	err_ok = false;
	mess_info(test_cat,"Library blocking check.");
	try{ owner().owner().func().at().unreg("testLib"); }
	catch( TError err )
	{
	    mess_info(test_cat,"Library blocking exception: %s",err.mess.c_str());
	    err_ok = true;
	}
	if( !err_ok ) { throw TError("","Test3 failed! Library blocking error!" ); }
	hd_lb.free();
	mess_info(test_cat,"Test3 passed!");

	//--------------------------- Test 4 ----------------------------------
	mess_info(test_cat,"Test4.");
	mess_info(test_cat,"Library bad access check.");
	err_ok = false;
	try{ AutoHD<TLibFunc> hd_lb = owner().owner().func().at().at("testLib1"); }
	catch( TError err )
	{
	    mess_info(test_cat,"Library access exception: %s",err.mess.c_str());
	    err_ok = true;
	}
	if( !err_ok ) { throw TError("","Test4 failed! Library access error!" ); }
	mess_info(test_cat,"Test4 passed!");

	//--------------------------- Test 5 ----------------------------------
	mess_info(test_cat,"Test5.");
	mess_info(test_cat,"Remove the true library.");
	owner().owner().func().at().unreg("testLib");

	mess_info(test_cat,"Not present check of library.");
	if( owner().owner().func().at().present("testLib") )
	{
	    owner().owner().func().at().unreg("testLib");
	    throw TError("","Test5 false! Not present check error!" );
	}
	mess_info(test_cat,"Libraries list check.");
	owner().owner().func().at().list(lst);
	for( i_ls = 0; i_ls < lst.size(); i_ls++ )
	    if( lst[i_ls] == "testLib" ) break;
	if( i_ls < lst.size() ) throw TError("","Test5 failed! Libraries list error!" );
	mess_info(test_cat,"Test5 passed!");

	//--------------------------- Test 6 ----------------------------------
	mess_info(test_cat,"Test6.");
	mess_info(test_cat,"Remove no present library.");
	err_ok = false;
	try{ owner().owner().func().at().unreg("testLib"); }
	catch( TError err )
	{
	    mess_info(test_cat,"Library remove exception: %s",err.mess.c_str());
	    err_ok = true;
	}
	if( !err_ok ) { throw TError("","Test6 failed! Library remove error!" ); }
	mess_info(test_cat,"Test6 passed!");

	//--------------------------- Test 7 ----------------------------------
	mess_info(test_cat,"Test7.");
	err_ok = false;
	mess_info(test_cat,"Create the empty library.");
	tlib = new TestLib("");

	mess_info(test_cat,"Register the empty library.");
	try{ owner().owner().func().at().reg( tlib ); }
	catch( TError err )
	{
	    mess_info(test_cat,"Register the empty library exception: %s",err.mess.c_str());
	    err_ok = true;
	}
	if( !err_ok ) { throw TError("","Test7 failed! Register the empty library error!" ); }
	mess_info(test_cat,"Test7 passed!");

	//=========================== Test TLibFunc =========================
	mess_info(test_cat,"TLibFunc tests.");
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
		    mess_info(test_cat.c_str(),"Test8.");
		    mess_info(test_cat.c_str(),"Create the true function.");
		    TestFunc *tfnc = new TestFunc("testFnc");

		    mess_info(test_cat.c_str(),"Register the true function.");
		    reg( tfnc );

		    mess_info(test_cat.c_str(),"Present check of function.");
		    if( !present("testFnc") )
		    {
			unreg("testFnc");
			throw TError("","Test8 failed! Present check error!" );
		    }

		    mess_info(test_cat.c_str(),"Functions list check.");
		    list(lst);
		    for( i_ls = 0; i_ls < lst.size(); i_ls++ )
			if( lst[i_ls] == "testFnc" ) break;
		    if( i_ls >= lst.size() ) throw TError("","Test8 failed! Functions list error!" );
		    mess_info(test_cat.c_str(),"Test8 passed!");

		    //--------------------------- Test 9 ----------------------------------
		    mess_info(test_cat.c_str(),"Test9.");
		    err_ok = false;
		    mess_info(test_cat.c_str(),"Register the double function.");
		    try{ reg( tfnc ); }
		    catch( TError err )
		    {
			mess_info(test_cat.c_str(),"Register the double function exception: %s",err.mess.c_str());
			err_ok = true;
		    }
		    if( !err_ok )
		    {
			unreg("testFnc");
			throw TError("","Test9 failed. Register double function error!" );
		    }
		    mess_info(test_cat.c_str(),"Test9 passed!");

		    //--------------------------- Test 10 ----------------------------------
		    mess_info(test_cat.c_str(),"Test10.");
		    mess_info(test_cat.c_str(),"Function access check.");
		    AutoHD<TFunction> hd_fnc = at("testFnc");

		    err_ok = false;
		    mess_info(test_cat.c_str(),"Function blocking check.");
		    try{ unreg("testFnc"); }
		    catch( TError err )
		    {
			mess_info(test_cat.c_str(),"Function blocking exception: %s",err.mess.c_str());
			err_ok = true;
		    }
		    if( !err_ok ) { throw TError("","Test10 failed! Function blocking error!" ); }
		    hd_fnc.free();
		    mess_info(test_cat.c_str(),"Test10 passed!");

		    //--------------------------- Test 11 ----------------------------------
		    mess_info(test_cat.c_str(),"Test11.");
		    mess_info(test_cat.c_str(),"Function bad access check.");
		    err_ok = false;
		    try{ AutoHD<TFunction> hd_fnc = at("testFnc1"); }
		    catch( TError err )
		    {
			mess_info(test_cat.c_str(),"Function access exception: %s",err.mess.c_str()); 
			err_ok = true;
		    }
		    if( !err_ok ) { throw TError("","Test11 failed! Function access error!" ); }
		    mess_info(test_cat.c_str(),"Test11 passed!");

		    //--------------------------- Test 12 ----------------------------------
		    mess_info(test_cat.c_str(),"Test12.");
		    mess_info(test_cat.c_str(),"Remove the true function.");
		    unreg("testFnc");

		    mess_info(test_cat.c_str(),"Not present check of function.");
		    if( present("testFnc") )
		    {
			unreg("testFnc");
			throw TError("","Test12 failed! Not present check error!" );
		    }
		    mess_info(test_cat.c_str(),"Functions list check.");
		    list(lst);
		    for( i_ls = 0; i_ls < lst.size(); i_ls++ )
			if( lst[i_ls] == "testFnc" ) break;
		    if( i_ls < lst.size() ) throw TError("","Test12 failed! Functions list error!" );
		    mess_info(test_cat.c_str(),"Test12 passed!");

		    //--------------------------- Test 13 ----------------------------------
		    mess_info(test_cat.c_str(),"Test13.");
		    mess_info(test_cat.c_str(),"Remove no present function.");
		    err_ok = false;
		    try{ unreg("testFnc"); }
		    catch( TError err )
		    {
			mess_info(test_cat.c_str(),"Function remove exception: %s",err.mess.c_str()); 
			err_ok = true;
		    }
		    if( !err_ok ) { throw TError("","Test13 failed! Function remove error!" ); }
		    mess_info(test_cat.c_str(),"Test13 passed!");

		    //--------------------------- Test 14 ----------------------------------
		    mess_info(test_cat.c_str(),"Test14.");
		    mess_info(test_cat.c_str(),"Create the empty function.");
		    tfnc = new TestFunc("");

		    mess_info(test_cat.c_str(),"Register the empty function.");
		    err_ok = false;
		    try{ reg( tfnc ); }
		    catch( TError err )
		    {
			mess_info(test_cat.c_str(),"Register the empty function exception: %s",err.mess.c_str());
			err_ok = true;
		    }
		    if( !err_ok ) { throw TError("","Test14 failed. Register the empty function error!" ); }
		    mess_info(test_cat.c_str(),"Test14 passed!");
		}

		string name()	{ return "TestLib"; }
		string descr()	{ return "Test library"; }
	};
	owner().owner().func().at().reg( new TestLib1(test_cat) );
	owner().owner().func().at().unreg( "TestLib1" );

	//=========================== Test TValFunc =========================
	mess_info(test_cat,"TValFunc tests.");
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
	mess_info(test_cat,"Test15.");
	mess_info(test_cat,"Create the function.");
	TestFunc *w_fnc = new TestFunc();

	mess_info(test_cat,"Create the value function.");
	TValFunc *vl_fnc = new TValFunc("test",NULL);

	mess_info(test_cat,"Connect function to value.");
	vl_fnc->func(w_fnc);

	mess_info(test_cat,"Write values.");
	vl_fnc->setS(vl_fnc->ioId("str"),"TEST value");
	vl_fnc->setI(vl_fnc->ioId("int"),12345);
	vl_fnc->setR(vl_fnc->ioId("real"),12345.12345);
	vl_fnc->setB(vl_fnc->ioId("bool"),true);

	mess_info(test_cat,"Read values");
	mess_info(test_cat,"Values: <%s>,<%d>,<%f>,<%d>",
		vl_fnc->getS(vl_fnc->ioId("str")).c_str(),vl_fnc->getI(vl_fnc->ioId("int")),
		vl_fnc->getR(vl_fnc->ioId("real")),vl_fnc->getB(vl_fnc->ioId("bool")) );
	mess_info(test_cat,"Test15 passed!");
	//--------------------------- Test 16 ----------------------------------
	mess_info(test_cat,"Test16.");
	mess_info(test_cat,"Disconnect function from value.");
	vl_fnc->func(NULL);
	mess_info(test_cat,"Read values");
	err_ok = false;
	try{ mess_info(test_cat,"Values: <%s>,<%d>,<%f>,<%d>",
		vl_fnc->getS(vl_fnc->ioId("str")).c_str(),vl_fnc->getI(vl_fnc->ioId("int")),
		vl_fnc->getR(vl_fnc->ioId("real")),vl_fnc->getB(vl_fnc->ioId("bool")) ); }
	catch( TError err )
	{
	    mess_info(test_cat,"Read values exception: %s",err.mess.c_str());
	    err_ok = true;
	}
	if( !err_ok ) { throw TError("","Test16 failed. Read values error!" ); }
	delete vl_fnc;
	delete w_fnc;
	mess_info(test_cat,"Test16 passed!");*/
    }
    //System controll test
    else if(id == "SysContrLang")
    {
	string path = t_n->attr("path");
	mess_info(test_cat,"SysContr tests for path <%s>.",path.c_str());

	XMLNode node("info");
	node.setAttr("path",path)->setAttr("user","root");
	SYS->cntrCmd(&node);
	//printf("Source: <%s>\n",node.save().c_str());
	pr_XMLNode( test_cat, &node, 0 );
    }
    else if(id == "ValBuf")
    {
	long long rtm, wtm;
	unsigned long long st_cnt;

	mess_info(test_cat,"*** Begin value buffer tests. ***");
	//--------------------------- Test 1 ----------------------------------
	mess_info(test_cat,"Test1. Create buffer: Data = string, Size = 10, Period = 1s, HardGrid = yes, HighRes = no.");
	TValBuf *buf = new TValBuf( TFld::String, 10, 1000000, true, false );
	if( buf->valType() == TFld::String && buf->size() == 10 && buf->period() == 1000000 && buf->hardGrid() && !buf->highResTm() )
	    mess_info(test_cat,"Test1 passed.");
	else throw TError("","Test1 failed! Create buffer error!" );

	//--------------------------- Test 2 ----------------------------------
	mess_info(test_cat,"Test2. Change buffer mode.");
	buf->setHardGrid( false );
	if( buf->hardGrid() == false )	mess_info(test_cat,"  Disable hard griding ok.");
	else throw TError("","Test2 failed! Disable hard griding failed!" );
	buf->setHighResTm(true);
 	if( buf->highResTm() == true )	mess_info(test_cat,"  Set high resolution time ok.");
	else throw TError("","Test2 failed! Set high resolution time failed!" );
	buf->setSize( 500 );
	buf->setSize( 2000 );
	if( buf->size() == 2000 )	mess_info(test_cat,"  Change buffer size ok.");
	else throw TError("","Test2 failed! Change buffer size failed!" );
	buf->setPeriod(0);
	if( buf->period() == 0 )	mess_info(test_cat,"  Change period ok.");
	else throw TError("","Test2 failed! Change period failed!" );
	mess_info(test_cat,"Test2 passed.");

	//--------------------------- Test 3 ----------------------------------
	mess_info(test_cat,"Test3. Destroy buffer.");
	delete buf;
	mess_info(test_cat,"Test3 passed.");

	//--------------------------- Test 4 ----------------------------------
	mess_info(test_cat,"Test4. Fill and check hard time griding string buffer.");
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
	mess_info(test_cat,"  Write a half buffer ok.");
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
	if(!(buf->end()/buf->period() == wtm/buf->period()+buf->size()+4 && buf->begin()/buf->period() == wtm/buf->period()+5))
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
	mess_info(test_cat,"  Write a roll buffer ok.");
	rtm = buf->end();
	buf->setS("Test up.",rtm);
	if(buf->getS(&rtm) != "Test up.")
	    throw TError("","Test4 failed! Update buffer end error!" );
	mess_info(test_cat,"  Update buffer end ok.");

	wtm += buf->period()*(buf->size()+5);
	st_cnt = TSYS::curTime();
	buf->setSize(1000);
	for(int i=0; i<buf->size(); i++)
	    buf->setS("Test: "+TSYS::int2str(i),wtm+i*buf->period());
	mess_info(test_cat,"  Write 1000 values time %f ms!",1e-3*(TSYS::curTime()-st_cnt));

	st_cnt = TSYS::curTime();
	for(long long i = buf->begin(); i <= buf->end(); i+=buf->period())
	{ rtm = i; buf->getS(&rtm); }
	mess_info(test_cat,"  Read 1000 values time %f ms!",1e-3*(TSYS::curTime()-st_cnt));

	mess_info(test_cat,"Test4 passed.");
	delete buf;

	//--------------------------- Test 5 ----------------------------------
	mess_info(test_cat,"Test5. Fill and check hard time griding integer buffer.");
	buf = new TValBuf( TFld::Integer, 10, 100000, true, true );
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
	mess_info(test_cat,"  Write a half buffer ok.");
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
	if(!(buf->end()/buf->period() == wtm/buf->period()+buf->size()+4 && buf->begin()/buf->period() == wtm/buf->period()+5))
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
	mess_info(test_cat,"  Write a roll buffer ok.");
	rtm = buf->end();
	buf->setI(100,rtm);
	if(buf->getI(&rtm) != 100)
	    throw TError("","Test5 failed! Update buffer end error!" );
	mess_info(test_cat,"  Update buffer end ok.");

	wtm += buf->period()*(buf->size()+5);
	st_cnt = TSYS::curTime();
	buf->setSize(1000);
	for(int i=0; i<buf->size(); i++)
	    buf->setI(i,wtm+i*buf->period());
	mess_info(test_cat,"  Write 1000 values time %f ms!",1e-3*(TSYS::curTime()-st_cnt));

	st_cnt = TSYS::curTime();
	for(long long i = buf->begin(); i <= buf->end(); i+=buf->period())
	{ rtm = i; buf->getI(&rtm); }
	mess_info(test_cat,"  Read 1000 values time %f ms!",1e-3*(TSYS::curTime()-st_cnt));

	mess_info(test_cat,"Test5 passed.");
	delete buf;

	//--------------------------- Test 6 ----------------------------------
	mess_info(test_cat,"Test6. Fill and check soft time griding string buffer (high time).");
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
	mess_info(test_cat,"  Write a half buffer ok.");
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
	if(!(buf->end()/buf->period() == wtm/buf->period()+buf->size()+4 && buf->begin()/buf->period() == wtm/buf->period()+4))
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
	mess_info(test_cat,"  Write a roll buffer ok.");
	rtm = buf->end();
	buf->setS("Test up.",rtm);
	if(buf->getS(&rtm) != "Test up.")
	    throw TError("","Test6 failed! Update buffer end error!" );
	mess_info(test_cat,"  Update buffer end ok.");

	wtm += buf->period()*(buf->size()+5);
	st_cnt = TSYS::curTime();
	buf->setSize(1000);
	for(int i=0; i<buf->size(); i++)
	    buf->setS("Test: "+TSYS::int2str(i),wtm+i*buf->period());
	mess_info(test_cat,"  Write 1000 values time %f ms!",1e-3*(TSYS::curTime()-st_cnt));

	st_cnt = TSYS::curTime();
	for(long long i = buf->begin(); i <= buf->end(); i+=buf->period())
	{ rtm = i; buf->getS(&rtm); }
	mess_info(test_cat,"  Read 1000 values time %f ms!",1e-3*(TSYS::curTime()-st_cnt));

	mess_info(test_cat,"Test6 passed.");
	delete buf;

	//--------------------------- Test 7 ----------------------------------
	mess_info(test_cat,"Test7. Fill and check soft time griding integer buffer (high time).");
	buf = new TValBuf( TFld::Integer, 10, 100000, false, true );
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
	mess_info(test_cat,"  Write a half buffer ok.");
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
	if(!(buf->end()/buf->period() == wtm/buf->period()+buf->size()+4 && buf->begin()/buf->period() == wtm/buf->period()+4))
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
	mess_info(test_cat,"  Write a roll buffer ok.");
	rtm = buf->end();
	buf->setI(1000,rtm);
	if(buf->getI(&rtm) != 1000)
	    throw TError("","Test7 failed! Update buffer end error!" );
	mess_info(test_cat,"  Update buffer end ok.");

	wtm += buf->period()*(buf->size()+5);
	st_cnt = TSYS::curTime();
	buf->setSize(1000);
	for(int i=0; i<buf->size(); i++)
	    buf->setI(i,wtm+i*buf->period());
	mess_info(test_cat,"  Write 1000 values time %f ms!",1e-3*(TSYS::curTime()-st_cnt));

	st_cnt = TSYS::curTime();
	for(long long i = buf->begin(); i <= buf->end(); i+=buf->period())
	{ rtm = i; buf->getI(&rtm); }
	mess_info(test_cat,"  Read 1000 values time %f ms!",1e-3*(TSYS::curTime()-st_cnt));

	mess_info(test_cat,"Test7 passed.");
	delete buf;

	//--------------------------- Test 8 ----------------------------------
	mess_info(test_cat,"Test8. Fill and check soft time griding string buffer (low time).");
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
	mess_info(test_cat,"  Write a half buffer ok.");
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
	if(!(buf->end()/buf->period() == wtm/buf->period()+buf->size()+4 && buf->begin()/buf->period() == wtm/buf->period()+4))
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
	mess_info(test_cat,"  Write a roll buffer ok.");
	rtm = buf->end();
	buf->setS("Test up.",rtm);
	if(buf->getS(&rtm) != "Test up.")
	    throw TError("","Test8 failed! Update buffer end error!" );
	mess_info(test_cat,"  Update buffer end ok.");

	wtm += buf->period()*(buf->size()+5);
	st_cnt = TSYS::curTime();
	buf->setSize(1000);
	for(int i=0; i<buf->size(); i++)
	    buf->setS("Test: "+TSYS::int2str(i),wtm+i*buf->period());
	mess_info(test_cat,"  Write 1000 values time %f ms!",1e-3*(TSYS::curTime()-st_cnt));

	st_cnt = TSYS::curTime();
	for(long long i = buf->begin(); i <= buf->end(); i+=buf->period())
	{ rtm = i; buf->getS(&rtm); }
	mess_info(test_cat,"  Read 1000 values time %f ms!",1e-3*(TSYS::curTime()-st_cnt));

	mess_info(test_cat,"Test8 passed.");
	delete buf;

	//--------------------------- Test 9 ----------------------------------
	mess_info(test_cat,"Test9. Fill and check soft time griding integer buffer (low time).");
	buf = new TValBuf( TFld::Integer, 10, 1000000, false, false );
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
	mess_info(test_cat,"  Write a half buffer ok.");
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
	if(!(buf->end()/buf->period() == wtm/buf->period()+buf->size()+4 && buf->begin()/buf->period() == wtm/buf->period()+4))
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
	mess_info(test_cat,"  Write a roll buffer ok.");
	rtm = buf->end();
	buf->setI(1000,rtm);
	if(buf->getI(&rtm) != 1000)
	    throw TError("","Test9 failed! Update buffer end error!" );
	mess_info(test_cat,"  Update buffer end ok.");

	wtm += buf->period()*(buf->size()+5);
	st_cnt = TSYS::curTime();
	buf->setSize(1000);
	for(int i=0; i<buf->size(); i++)
	    buf->setI(i,wtm+i*buf->period());
	mess_info(test_cat,"  Write 1000 values time %f ms!",1e-3*(TSYS::curTime()-st_cnt));

	st_cnt = TSYS::curTime();
	for(long long i = buf->begin(); i <= buf->end(); i+=buf->period())
	{ rtm = i; buf->getI(&rtm); }
	mess_info(test_cat,"  Read 1000 values time %f ms!",1e-3*(TSYS::curTime()-st_cnt));

	mess_info(test_cat,"Test9 passed.");
	delete buf;

	//--------------------------- Test 10 ----------------------------------
	mess_info(test_cat,"Test10. Fill and check free time string buffer (high time).");
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
	mess_info(test_cat,"  Write a half buffer ok.");
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
	mess_info(test_cat,"  Write a roll buffer ok.");
	rtm = buf->end();
	buf->setS("Test up.",rtm);
	if(buf->getS(&rtm) != "Test up.")
	    throw TError("","Test10 failed! Update buffer end error!" );
	mess_info(test_cat,"  Update buffer end ok.");

	wtm += wper*(buf->size()+5);
	st_cnt = TSYS::curTime();
	buf->setSize(1000);
	for(int i=0; i<buf->size(); i++)
	    buf->setS("Test: "+TSYS::int2str(i),wtm+i*wper);
	mess_info(test_cat,"  Write 1000 values time %f ms!",1e-3*(TSYS::curTime()-st_cnt));

	st_cnt = TSYS::curTime();
	for(long long i = buf->end(); i > buf->begin(); i=rtm-1)
	{ rtm = i; buf->getS(&rtm); }
	mess_info(test_cat,"  Read 1000 values time %f ms!",1e-3*(TSYS::curTime()-st_cnt));

	mess_info(test_cat,"Test10 passed.");
	delete buf;

	//--------------------------- Test 11 ----------------------------------
	mess_info(test_cat,"Test11. Fill and check free time integer buffer (high time).");
	buf = new TValBuf( TFld::Integer, 10, 0, false, true );
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
	mess_info(test_cat,"  Write a half buffer ok.");
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
	mess_info(test_cat,"  Write a roll buffer ok.");
	rtm = buf->end();
	buf->setI(1000,rtm);
	if(buf->getI(&rtm) != 1000)
	    throw TError("","Test11 failed! Update buffer end error!" );
	mess_info(test_cat,"  Update buffer end ok.");

	wtm += wper*(buf->size()+5);
	st_cnt = TSYS::curTime();
	buf->setSize(1000);
	for(int i=0; i<buf->size(); i++)
	    buf->setI(i,wtm+i*wper);
	mess_info(test_cat,"  Write 1000 values time %f ms!",1e-3*(TSYS::curTime()-st_cnt));

	st_cnt = TSYS::curTime();
	for(long long i = buf->end(); i > buf->begin(); i=rtm-1)
	{ rtm = i; buf->getI(&rtm); }
	mess_info(test_cat,"  Read 1000 values time %f ms!",1e-3*(TSYS::curTime()-st_cnt));

	mess_info(test_cat,"Test11 passed.");
	delete buf;

	//--------------------------- Test 12 ----------------------------------
	mess_info(test_cat,"Test12. Fill and check free time string buffer (low time).");
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
	mess_info(test_cat,"  Write a half buffer ok.");
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
	mess_info(test_cat,"  Write a roll buffer ok.");
	rtm = buf->end();
	buf->setS("Test up.",rtm);
	if(buf->getS(&rtm) != "Test up.")
	    throw TError("","Test12 failed! Update buffer end error!" );
	mess_info(test_cat,"  Update buffer end ok.");

	wtm += wper*(buf->size()+5);
	st_cnt = TSYS::curTime();
	buf->setSize(1000);
	for(int i=0; i<buf->size(); i++)
	    buf->setS("Test: "+TSYS::int2str(i),wtm+i*wper);
	mess_info(test_cat,"  Write 1000 values time %f ms!",1e-3*(TSYS::curTime()-st_cnt));

	st_cnt = TSYS::curTime();
	for(long long i = buf->end(); i > buf->begin(); i=rtm-1)
	{ rtm = i; buf->getS(&rtm); }
	mess_info(test_cat,"  Read 1000 values time %f ms!",1e-3*(TSYS::curTime()-st_cnt));

	mess_info(test_cat,"Test12 passed.");
	delete buf;

	//--------------------------- Test 13 ----------------------------------
	mess_info(test_cat,"Test13. Fill and check free time integer buffer (low time).");
	buf = new TValBuf( TFld::Integer, 10, 0, false, false );
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
	mess_info(test_cat,"  Write a half buffer ok.");
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
	mess_info(test_cat,"  Write a roll buffer ok.");
	rtm = buf->end();
	buf->setI(100,rtm);
	if(buf->getI(&rtm) != 100)
	    throw TError("","Test13 failed! Update buffer end error!" );
	mess_info(test_cat,"  Update buffer end ok.");

	wtm += wper*(buf->size()+5);
	st_cnt = TSYS::curTime();
	buf->setSize(1000);
	for(int i=0; i<buf->size(); i++)
	    buf->setI(i,wtm+i*wper);
	mess_info(test_cat,"  Write 1000 values time %f ms!",1e-3*(TSYS::curTime()-st_cnt));

	st_cnt = TSYS::curTime();
	for(long long i = buf->end(); i > buf->begin(); i=rtm-1)
	{ rtm = i; buf->getI(&rtm); }
	mess_info(test_cat,"  Read 1000 values time %f ms!",1e-3*(TSYS::curTime()-st_cnt));

	mess_info(test_cat,"Test13 passed.");
	delete buf;
    }
    else if(id == "Archive")
    {
	string arch   = t_n->attr("arch");
	long long per = atoll(t_n->attr("period").c_str());

	mess_info(test_cat,"*** Begin archive <%s> tests. ***",arch.c_str());
	AutoHD<TVArchive> o_arch = SYS->archive().at().valAt(arch);

	int buf_sz = 5;
	long long wtm = per*(TSYS::curTime()/per);
	long long ttm;

	TValBuf buf(TFld::Integer, buf_sz, per, true, false );
	//--------------------------- Test 1 ----------------------------------
	mess_info(test_cat,"Test1. Simple fill and check archive.");
	for( int i_el = 0; i_el < buf_sz; i_el++)
	    buf.setI((int)pow(10,i_el),wtm+i_el*per);
	o_arch.at().setVals(buf,buf.begin(),buf.end(),"");
	for( int i_el = 0; i_el < buf_sz+2; i_el++)
	{
	    ttm = wtm+i_el*per;
	    int val = o_arch.at().getI(&ttm);
	    if(	(i_el < buf_sz && val != pow(10,i_el)) ||
		    (i_el >= buf_sz && val != EVAL_INT) )
		throw TError("","Test1 failed!" );
	}
	mess_info(test_cat,"Test1 passed.");
	//--------------------------- Test 2 ----------------------------------
	mess_info(test_cat,"Test2. Internal insert double value (down).");
	buf.clear();
	buf.setI((int)pow(10,2),wtm+3*per);
	o_arch.at().setVals(buf,wtm+3*per,wtm+3*per,"");
	for( int i_el = 0; i_el < buf_sz+2; i_el++)
	{
	    ttm = wtm+i_el*per;
	    if( (i_el < buf_sz && i_el != 3 && o_arch.at().getI(&ttm) != pow(10,i_el)) || 
		    (i_el < buf_sz && i_el == 3 && o_arch.at().getI(&ttm) != pow(10,2)) ||
		    (i_el >= buf_sz && o_arch.at().getI(&ttm) != EVAL_INT) )
		throw TError("","Test2 failed!" );
	}
	mess_info(test_cat,"Test2 passed.");
	//--------------------------- Test 3 ----------------------------------
	mess_info(test_cat,"Test3. Internal insert double value (up).");
	buf.clear();
	buf.setI((int)pow(10,4),wtm+3*per);
	o_arch.at().setVals(buf,wtm+3*per,wtm+3*per,"");
	for( int i_el = 0; i_el < buf_sz+2; i_el++)
	{
	    ttm = wtm+i_el*per;
	    if( (i_el < buf_sz && i_el != 3 && o_arch.at().getI(&ttm) != pow(10,i_el)) ||
		    (i_el < buf_sz && i_el == 3 && o_arch.at().getI(&ttm) != pow(10,4)) ||
		    (i_el >= buf_sz && o_arch.at().getI(&ttm) != EVAL_INT) )
		throw TError("","Test3 failed!" );
	}
	mess_info(test_cat,"Test3 passed.");
	//--------------------------- Test 4 ----------------------------------
	mess_info(test_cat,"Test4. Internal insert double value (down).");
	buf.clear();
	buf.setI((int)pow(10,2),wtm+3*per);
	o_arch.at().setVals(buf,wtm+3*per,wtm+3*per,"");
	for( int i_el = 0; i_el < buf_sz+2; i_el++)
	{
	    ttm = wtm+i_el*per;
	    if( (i_el < buf_sz && i_el != 3 && o_arch.at().getI(&ttm) != pow(10,i_el)) ||
		    (i_el < buf_sz && i_el == 3 && o_arch.at().getI(&ttm) != pow(10,2)) ||
		    (i_el >= buf_sz && o_arch.at().getI(&ttm) != EVAL_INT) )
		throw TError("","Test4 failed!" );
	}
	mess_info(test_cat,"Test4 passed.");
	//--------------------------- Test 5 ----------------------------------
	mess_info(test_cat,"Test5. Internal insert no double value.");
	buf.clear();
	buf.setI((int)pow(10,9),wtm+per);
	o_arch.at().setVals(buf,wtm+per,wtm+per,"");
	for( int i_el = 0; i_el < buf_sz+2; i_el++)
	{
	    ttm = wtm+i_el*per;
	    if( (i_el < buf_sz && i_el != 3 && i_el != 1 && o_arch.at().getI(&ttm) != pow(10,i_el)) ||
		    (i_el < buf_sz && i_el == 3 && o_arch.at().getI(&ttm) != pow(10,2)) ||
		    (i_el < buf_sz && i_el == 1 && o_arch.at().getI(&ttm) != pow(10,9)) ||
		    (i_el >= buf_sz && o_arch.at().getI(&ttm) != EVAL_INT) )
		throw TError("","Test5 failed!" );
	}
	mess_info(test_cat,"Test5 passed.");
	//--------------------------- Test 6 ----------------------------------
	mess_info(test_cat,"Test6. Internal insert double (up) value.");
	buf.clear();
	buf.setI((int)pow(10,2),wtm+per);
	o_arch.at().setVals(buf,wtm+per,wtm+per,"");
	for( int i_el = 0; i_el < buf_sz+2; i_el++)
	{
	    ttm = wtm+i_el*per;
	    if( (i_el < buf_sz && i_el != 3 && i_el != 1 && o_arch.at().getI(&ttm) != pow(10,i_el)) ||
		    (i_el < buf_sz && i_el == 3 && o_arch.at().getI(&ttm) != pow(10,2)) ||
		    (i_el < buf_sz && i_el == 1 && o_arch.at().getI(&ttm) != pow(10,2)) ||
		    (i_el >= buf_sz && o_arch.at().getI(&ttm) != EVAL_INT) )
		throw TError("","Test6 failed!" );
	}
	mess_info(test_cat,"Test6 passed.");
	//--------------------------- Test 7 ----------------------------------
	mess_info(test_cat,"Test7. Internal insert value instaead double value.");
	buf.clear();
	buf.setI((int)pow(10,3),wtm+3*per);
	o_arch.at().setVals(buf,wtm+3*per,wtm+3*per,"");
	for( int i_el = 0; i_el < buf_sz+2; i_el++)
	{
	    ttm = wtm+i_el*per;
	    if( (i_el < buf_sz && i_el != 1 && o_arch.at().getI(&ttm) != pow(10,i_el)) ||
		    (i_el < buf_sz && i_el == 1 && o_arch.at().getI(&ttm) != pow(10,2)) ||
		    (i_el >= buf_sz && o_arch.at().getI(&ttm) != EVAL_INT) )
		throw TError("","Test7 failed!" );
	}
	mess_info(test_cat,"Test7 passed.");
	//--------------------------- Test 8 ----------------------------------
	/*sleep(2);
	mess_info(test_cat,"Test8. Set three values to end.");
	wtm = o_arch.at().end("");
	buf.clear();
	for( int i_el = -1; i_el <= 1; i_el++ )
	    buf.setI(i_el,wtm+i_el*per);
	o_arch.at().setVals(buf,buf.begin(),buf.end(),"");
	for( int i_el = -1; i_el <= 1; i_el++)
	{
	    ttm = wtm+i_el*per;
	    if( o_arch.at().getI(&ttm) != i_el )
		throw TError("","Test8 failed!" );
	}
	mess_info(test_cat,"Test8 passed.");*/
	mess_info(test_cat,"*** End archive <%s> tests. ***",arch.c_str());
    }
    else if(id == "Base64Code")
    {
	mess_info(test_cat,"*** Begin Base64 coding and encoding test. ***");

	mess_info(test_cat,"Test1. Coding test.");
	string inbuf, outbuf;
	for(int i_s = 0; i_s < 256; i_s++) inbuf.push_back((unsigned char)i_s);

	long long st_cnt = TSYS::curTime();
	outbuf = TSYS::strEncode(inbuf,TSYS::base64);
	mess_info(test_cat,"  Code %d size text time %f ms!",inbuf.size(),1e-3*(TSYS::curTime()-st_cnt));
	mess_info(test_cat,(string("  Coded text: ")+outbuf).c_str());

	st_cnt = TSYS::curTime();
	inbuf = TSYS::strDecode(outbuf,TSYS::base64);
	mess_info(test_cat,"  Encode %d size text time %f ms!",outbuf.size(),1e-3*(TSYS::curTime()-st_cnt));
	for(int i_s = 0; i_s < 256; i_s++) 
	    if((unsigned char)inbuf[i_s] != i_s) 
		throw TError("","Test1 failed! Coding error!" );
	mess_info(test_cat,"Test1 passed.");
    }
}

void TTest::pr_XMLNode( const char *cat, XMLNode *node, int level )
{
    char *buf = (char *)malloc(level+1);
    for(int i_c = 0; i_c < level; i_c++) buf[i_c] = ' ';
    buf[level] = 0;

    vector<string> list;
    mess_info(cat,"%s{%d <%s>, text <%s>, childs - %d!",
	buf, level, node->name().c_str(),node->text().c_str(),node->childSize());
    node->attrList(list);
    for(unsigned i_att = 0; i_att < list.size(); i_att++)
	mess_info(cat,"        Attr <%s> = <%s>!",list[i_att].c_str(),node->attr(list[i_att]).c_str());
    for(int i_ch = 0; i_ch < node->childSize(); i_ch++)
	pr_XMLNode( cat, node->childGet(i_ch), level+1 ); 
    mess_info(cat,"%s}%d <%s>", buf, level, node->name().c_str());
    free(buf);
}
