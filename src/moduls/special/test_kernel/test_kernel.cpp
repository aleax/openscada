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

#include <sys/types.h>
#include <sys/stat.h>
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
#include <tarhives.h>
#include "test_kernel.h"

//============ Modul info! =====================================================
#define NAME_MODUL  "test_kernel"
#define NAME_TYPE   "Special"
#define VER_TYPE    VER_SPC
#define SUB_TYPE    "TEST"
#define VERSION     "0.0.4"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "OpenScada Kernel test module: Configs, Values ... ."
#define LICENSE     "GPL"
//==============================================================================

extern "C"
{
    SAtMod module( int n_mod )
    {
	SAtMod AtMod;

	if(n_mod==0)
	{
	    AtMod.name  = NAME_MODUL;
	    AtMod.type  = NAME_TYPE;
	    AtMod.t_ver = VER_TYPE;
	}
    	else
	    AtMod.name  = "";

	return( AtMod );
    }

    TModule *attach( SAtMod &AtMod, string source )
    {
	KernelTest::TTest *self_addr = NULL;

    	if( AtMod.name == NAME_MODUL && AtMod.type == NAME_TYPE && AtMod.t_ver == VER_TYPE )
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
    NameModul = NAME_MODUL;
    NameType  = NAME_TYPE;
    Vers      = VERSION;
    Autors    = AUTORS;
    DescrMod  = DESCRIPTION;
    License   = LICENSE;
    Source    = name;
}

TTest::~TTest()
{
    if( run_st ) stop();
}

string TTest::mod_info( const string &name )
{
    if( name == "SubType" ) return(SUB_TYPE);
    else return( TModule::mod_info( name) );
}

void TTest::mod_info( vector<string> &list )
{
    TModule::mod_info(list);
    list.push_back("SubType");
}

void TTest::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "============== Module %s command line options =======================\n"    
    "------------------ Fields <%s> sections of config file --------------\n"
    "XML=<1>         enable XML parsed and create test (1 - on; 0 - off);\n"
    "  file=<name>     atribute for seting <name> file for XML parsing;\n"
    "MESS_BUF=<1>    enable Message buffer test (1 - on; 0 - off);\n"
    "PARAM=<1>       enable Parameter test (1 - on; 0 - off);\n"
    "  name=<name>     atribute for seting <name> testing parameter;\n"
    "\n",NAME_MODUL,NAME_MODUL);
}

void TTest::mod_CheckCommandLine(  )
{
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{NULL        ,0,NULL,0  }
    };

    optind=opterr=0;
    do
    {
	next_opt=getopt_long(SYS->argc,(char * const *)SYS->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': pr_opt_descr(stdout); break;
	    case -1 : break;
	}
    } while(next_opt != -1);
}

void TTest::mod_UpdateOpt( )
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
    if( SYS->event_wait( run_st, true, string(NAME_MODUL)+": Is starting....",5) )
	throw TError("%s: No started!",NAME_MODUL);
}

void TTest::stop(  )
{
    if( !run_st ) return;

    endrun = true;
    if( SYS->event_wait( run_st, false, string(NAME_MODUL)+": Is stoping....",5) )
	throw TError("%s: No stoped!",NAME_MODUL);
    pthread_join( pthr_tsk, NULL );
}

void *TTest::Task( void *CfgM )
{
    int count = 0, i_cnt = 0;

    TTest *tst = (TTest *)CfgM;
    tst->run_st = true;
    tst->endrun = false;
    
#if OSC_DEBUG
    tst->Owner().m_put("DEBUG",MESS_DEBUG,"%s:Thread <%d>!",NAME_MODUL,getpid() );
#endif

    tst->Test(-1);    
    while( !tst->endrun )
    {
	if( ++i_cnt > 1000/STD_WAIT_DELAY )  // 1 sec
	{
	    i_cnt = 0;
	    if( ++count == 1000000 ) count = 0;	
	    tst->Test(count);
	}
	usleep(STD_WAIT_DELAY*1000);
    }
    tst->run_st = false;
}

void TTest::Test( int count )
{
    /*
    vector<string> list_ct,list_c,list_pt,list_pc;
    Owner().Controller->List(list_ct);
    Mess->put(1,"Controller types: %d",list_ct.size());
    for(int i=0; i < list_ct.size(); i++)
    {
	try
	{
	    Mess->put(1,"Controller type: <%s>",list_ct[i].c_str());

	    Owner().Controller->at_tp(list_ct[i])->ListTpPrm(list_pt);
	    Mess->put(1,"Types param's: %d",list_pt.size());
	    for(int ii=0; ii < list_pt.size(); ii++)
    		Mess->put(1,"Type: <%s>",list_pt[ii].c_str());

	    Owner().Controller->at_tp(list_ct[i])->List(list_c);
	    Mess->put(1,"Controllers: %d",list_c.size());
	    for(int ii=0; ii < list_c.size(); ii++)
	    {
		Mess->put(1,"Controller: <%s>",list_c[ii].c_str());
		for(int i_pt=0; i_pt < list_pt.size(); i_pt++)
		{
		    Owner().Controller->at(list_c[ii])->List(list_pt[i_pt],list_pc);
		    Mess->put(1,"%s Parameters: %d",list_pt[i_pt].c_str(),list_pc.size());
		    for(int iii=0; iii < list_pc.size(); iii++)
		    Mess->put(1,"Parameter: <%s>",list_pc[iii].c_str());
		}
	    }
	}
	catch(TError err){ }
    }
    //---------------- All parameter's list ----------------
    vector<string> list_pc;
    Owner().Owner().Param().list(list_pc);
    Mess->put("TEST",MESS_DEBUG,"TEST: Params: %d",list_pc.size());
    for(unsigned i=0; i < list_pc.size(); i++)
    	Mess->put("TEST",MESS_DEBUG,"TEST: Param: <%s>",list_pc[i].c_str());	    			
    */
    //---------------- Configs element's test ----------------
    try
    {
	TParamS &param = Owner().Owner().Param();
	XMLNode *t_n = mod_XMLCfgNode()->get_child("PARAM");
	if( atoi(t_n->get_attr("on").c_str()) == 1 )	
	{
	    if( count < 0 || ( atoi(t_n->get_attr("period").c_str()) && !( count % atoi(t_n->get_attr("period").c_str()) ) ) )
	    {
		AutoHD<TParam> prm = param.at( t_n->get_attr("name") );
		m_put("TEST",MESS_DEBUG,"-------- Start parameter <%s> test ----------",t_n->get_attr("name").c_str());
    
		vector<string> list_el;
		prm.at().at().cfListEl(list_el);
		m_put("TEST",MESS_DEBUG,"Config elements avoid: %d",list_el.size());
		for(unsigned i=0; i< list_el.size(); i++)
		    m_put("TEST",MESS_DEBUG,"Element: %s",list_el[i].c_str());		    
		
		m_put("TEST",MESS_DEBUG,"Value elements avoid: %d",list_el.size());
		//prm.vlSetR(0,30);
		list_el.clear();
		prm.at().at().vlList(list_el);
		for(unsigned i=0; i< list_el.size(); i++)
		{
		    AutoHD<TVal> val = prm.at().at().vlAt(list_el[i]);
		    if( val.at().fld().type()&T_SELECT )
			m_put("TEST",MESS_DEBUG,"Element (SELECT): %s: %s",list_el[i].c_str(), val.at().getSEL().c_str() );
		    else if( val.at().fld().type()&T_STRING )
			m_put("TEST",MESS_DEBUG,"Element (STRING): %s: %s",list_el[i].c_str(), val.at().getS().c_str() );
		    else if( val.at().fld().type()&T_REAL )
			m_put("TEST",MESS_DEBUG,"Element (REAL): %s: %f",list_el[i].c_str(), val.at().getR() );
		    else if( val.at().fld().type()&T_BOOL )
			m_put("TEST",MESS_DEBUG,"Element (BOOLEAN): %s: %d",list_el[i].c_str(), val.at().getB() );
		    else m_put("TEST",MESS_DEBUG,"Element (INTEGER): %s: %d",list_el[i].c_str(), val.at().getI() );
		}
		
		m_put("TEST",MESS_DEBUG,"Configs throw control: %d",list_el.size());
		    
		XMLNode node;
		prm.at().at().ctr_info(node);
		pr_XMLNode( &node, 0 );

		m_put("TEST",MESS_DEBUG,"-------- Stop parameter <%s> test ----------",t_n->get_attr("name").c_str());
    	    }
	}
    } catch( TError error )
    { m_put_s("TEST",MESS_DEBUG,error.what()); }
    //=============== Test XML =====================
    try
    {
	XMLNode *t_n = mod_XMLCfgNode()->get_child("XML");
	if( atoi(t_n->get_attr("on").c_str()) == 1 )
	{
	    if( count < 0 || ( atoi(t_n->get_attr("period").c_str()) && !( count % atoi(t_n->get_attr("period").c_str()) ) ) )
	    {
		int hd = open(t_n->get_attr("file").c_str(),O_RDONLY);
		if(hd > 0)
		{
		    m_put_s("TEST",MESS_DEBUG,"-------- Start TEST XML parsing ----------");
		    int cf_sz = lseek(hd,0,SEEK_END);
		    lseek(hd,0,SEEK_SET);
		    char *buf = (char *)malloc(cf_sz);
		    read(hd,buf,cf_sz);
		    close(hd);
		    string s_buf = buf;
		    free(buf);
		    XMLNode node;
		    node.load_xml(s_buf);
		    pr_XMLNode( &node, 0 );
		    m_put_s("TEST",MESS_DEBUG,"-------- End TEST XML parsing ----------");
		}
	    }
	}
    } catch( TError error )
    { m_put_s("TEST",MESS_DEBUG,error.what()); }
    //=============== Test MESS =====================
    try
    {
	XMLNode *t_n = mod_XMLCfgNode()->get_child("MESS");
	if( atoi(t_n->get_attr("on").c_str()) == 1 )
	{
	    if( count < 0 || ( atoi(t_n->get_attr("period").c_str()) && !( count % atoi(t_n->get_attr("period").c_str()) ) ) )
	    {
		TArhiveS &Arh_s = Owner().Owner().Arhive();
		
		string n_arh = t_n->get_attr("arh");
		string t_arh = t_n->get_attr("t_arh");
		m_put("TEST",MESS_DEBUG,"-------- Start Message buffer %s test ----------",n_arh.c_str());
		vector<SBufRec> buf_rec;
		if( n_arh == "sys" ) Mess->get(0,time(NULL),buf_rec,t_n->get_attr("categ"));
		else		    
		    ((TTipArhive &)Arh_s.gmd_at(t_arh).at()).mess_at(n_arh).at().get(0,time(NULL),buf_rec,t_n->get_attr("categ"));
		m_put("TEST",MESS_DEBUG,"Messages avoid %d.",buf_rec.size() );
		for(unsigned i_rec = 0; i_rec < buf_rec.size(); i_rec++)
		{
		    char *c_tm = ctime( &buf_rec[i_rec].time);
		    for( int i_ch = 0; i_ch < strlen(c_tm); i_ch++ )
			if( c_tm[i_ch] == '\n' ) c_tm[i_ch] = '\0';
		    m_put("TEST",MESS_DEBUG,"<%s> : <%s> : <%s>",c_tm, buf_rec[i_rec].categ.c_str(), buf_rec[i_rec].mess.c_str() );
		}
		m_put("TEST",MESS_DEBUG,"-------- Stop Message buffer %s test ----------",n_arh.c_str());
	    }
	}
    } catch( TError error )
    { m_put_s("TEST",MESS_DEBUG,error.what()); }
    try
    {
	XMLNode *t_n = mod_XMLCfgNode()->get_child("SOAttDet");
	if( atoi(t_n->get_attr("on").c_str()) == 1 )
	{	    
	    if( count < 0 || ( atoi(t_n->get_attr("period").c_str()) && !( count % atoi(t_n->get_attr("period").c_str()) ) ) )
	    {
		TModSchedul &sched = Owner().Owner().ModSchedul();
		string SO_name = t_n->get_attr("name");
	        SHD so_st = sched.SO(SO_name);
		so_st.name;
		m_put("TEST",MESS_DEBUG,"-------- Start SO <%s> test ----------",so_st.name.c_str());
		if( so_st.hd ) sched.DetSO( so_st.name );
		else           sched.AttSO( so_st.name,(bool)atoi( t_n->get_attr("full").c_str()) );		
		m_put("TEST",MESS_DEBUG,"-------- Stop SO <%s> test ----------",so_st.name.c_str());
	    }
	}
    } catch( TError error )
    { m_put_s("TEST",MESS_DEBUG,error.what()); }
    //Check values
    try
    {
	XMLNode *t_n = mod_XMLCfgNode()->get_child("Val");
	if( atoi(t_n->get_attr("on").c_str()) == 1 )
	{	    
	    if( count < 0 || ( atoi(t_n->get_attr("period").c_str()) && !( count % atoi(t_n->get_attr("period").c_str()) ) ) )
	    {
		//m_put("TEST",MESS_DEBUG,"-------- Start Value <%s> test ----------",t_n->get_attr("name").c_str());		
		TParamS &param = Owner().Owner().Param();

		int p_id, p_id1 = 0;
		do
		{
    		    p_id = t_n->get_attr("name").find(";",p_id1);
    		    string s_prm = t_n->get_attr("name").substr(p_id1,p_id-p_id1);
		    
		    int s_pos = s_prm.find("/",0);
		    if( s_pos == string::npos ) throw TError("Parameter value error!");		    
		    AutoHD<TParam> prm = param.at( s_prm.substr(0,s_pos) );
		    AutoHD<TVal> val = prm.at().at().vlAt( s_prm.substr(s_pos+1) );
		    if( val.at().fld().type()&T_SELECT )
			m_put("TEST",MESS_DEBUG,"%s: %s = %s",prm.at().at().Name().c_str(), val.at().fld().descr().c_str(), val.at().getSEL().c_str() );
		    else if( val.at().fld().type()&T_STRING )
			m_put("TEST",MESS_DEBUG,"%s: %s = %s",prm.at().at().Name().c_str(), val.at().fld().descr().c_str(), val.at().getS().c_str() );
		    else if( val.at().fld().type()&T_REAL )
			m_put("TEST",MESS_DEBUG,"%s: %s = %f",prm.at().at().Name().c_str(), val.at().fld().descr().c_str(), val.at().getR() );
		    else if( val.at().fld().type()&(T_DEC|T_OCT|T_HEX) )
			m_put("TEST",MESS_DEBUG,"%s: %s = %d",prm.at().at().Name().c_str(), val.at().fld().descr().c_str(), val.at().getI() );
		    else if( val.at().fld().type()&T_BOOL )
			m_put("TEST",MESS_DEBUG,"%s: %s = %d",prm.at().at().Name().c_str(), val.at().fld().descr().c_str(), val.at().getB() );
		    p_id1 = p_id + 1;
		}while( p_id != string::npos );		
		//m_put("TEST",MESS_DEBUG,"-------- Stop Value <%s> test ----------",t_n->get_attr("name").c_str());		
	    }
	}
    } catch( TError error )
    { m_put_s("TEST",MESS_DEBUG,error.what()); }
    //=============== Test Object controll =====================
    /*
    try
    {
	XMLNode *t_n = mod_XMLCfgNode()->get_child("Controll");
	if( atoi(t_n->get_attr("on").c_str()) == 1 )
	{
	    if( count < 0 || ( atoi(t_n->get_attr("period").c_str()) && !( count % atoi(t_n->get_attr("period").c_str()) ) ) )
	    {
		int hd = param.att( t_n->get_attr("name"), string("")+NAME_MODUL+": PARAM test!" );
		try
		{		
		    TParamContr &prm = param.at(hd).at();
    		    m_put_s("TEST",MESS_DEBUG,"-------- Begin object controll test ----------");
		
    		    XMLNode node;
    		    param->ctr_info(node);
		    pr_XMLNode( node, 0 );
		
		    m_put_s("TEST",MESS_DEBUG,"-------- End object controll test ----------");
		}
		catch( TError error )
		{
		    param.det( hd );
		    throw;
		}		    
	    }
	}
    } catch( TError error )
    { m_put_s("TEST",MESS_DEBUG,error.what()); }
    */
	
    //m_put_s("TEST",MESS_DEBUG,"***** End test block *****");
}

void TTest::pr_XMLNode( XMLNode *node, int level )
{
    char *buf = (char *)malloc(level+1);
    for(int i_c = 0; i_c < level; i_c++) buf[i_c] = ' ';
    buf[level] = 0;
	
    vector<string> list;
    m_put("TEST",MESS_DEBUG,"%s{%d <%s>, text <%s>, childs - %d!",
	buf, level, node->get_name().c_str(),node->get_text().c_str(),node->get_child_count());
    node->get_attr_list(list);
    for(unsigned i_att = 0; i_att < list.size(); i_att++)
	m_put("TEST",MESS_DEBUG,"        Attr <%s> = <%s>!",list[i_att].c_str(),node->get_attr(list[i_att]).c_str());	
    for(int i_ch = 0; i_ch < node->get_child_count(); i_ch++)
	pr_XMLNode( node->get_child(i_ch), level+1 ); 
    m_put("TEST",MESS_DEBUG,"%s}%d <%s>", buf, level, node->get_name().c_str());
    free(buf);
}

