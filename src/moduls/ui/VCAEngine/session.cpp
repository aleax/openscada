
//OpenSCADA system module UI.VCAEngine file: session.cpp
/***************************************************************************
 *   Copyright (C) 2007 by Roman Savochenko
 *   rom_as@diyaorg.dp.ua
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 ***************************************************************************/

#include <pthread.h>
#include <signal.h>

#include <tsys.h>

#include "vcaengine.h"
#include "session.h"


using namespace VCA;

//************************************************
//* Project's session                       	 *
//************************************************
Session::Session( const string &iid, const string &iproj ) :
    m_enable(false), m_start(false), endrun_req(false), tm_calc(0.0),
    m_id(iid), m_user("root"), m_prjnm(iproj), m_per(100), m_calcClk(1),
    m_backgrnd(false), m_connects(0)
{
    m_page = grpAdd("pg_");
}

Session::~Session( )
{

}

void Session::postEnable( int flag )
{
    if( flag&TCntrNode::NodeRestore )	setEnable(true);
}

void Session::preDisable( int flag )
{
    if( enable() )  setEnable(false);
}

void Session::setEnable( bool val )
{
    vector<string> pg_ls;
    
    if( val )
    {
	//- Connect to project -
	m_parent = mod->prjAt(m_prjnm);
	
	//- Create root pages -	
	parent().at().list(pg_ls);
	for( int i_p = 0; i_p < pg_ls.size(); i_p++ )
	    if( !present(pg_ls[i_p]) )
		add(pg_ls[i_p],parent().at().at(pg_ls[i_p]).at().path());
    
	//- Pages enable -
	list(pg_ls);
	for( int i_ls = 0; i_ls < pg_ls.size(); i_ls++ )
    	    at(pg_ls[i_ls]).at().setEnable(true);
    }
    else
    {
	if( start() )	setStart(false);
	//- Pages disable -
	list(pg_ls);
	for( int i_ls = 0; i_ls < pg_ls.size(); i_ls++ )
    	    at(pg_ls[i_ls]).at().setEnable(false);
	
	//- Delete pages -
	for( int i_ls = 0; i_ls < pg_ls.size(); i_ls++ )
    	    del(pg_ls[i_ls]);
	    
	//- Disconnect from project -
	m_parent.free();
    }

    m_enable = val;
}

void Session::setStart( bool val )
{
    vector<string> pg_ls;
    
    if( val )
    {
	//- Enable session if it disabled -
	if( !enable() )	setEnable(true);
	
	//- Process all pages is on -
	list(pg_ls);
	for( int i_ls = 0; i_ls < pg_ls.size(); i_ls++ )
    	    at(pg_ls[i_ls]).at().setProcess(true);
	
	//- Start process task -
	if( !m_start )
	{
	    pthread_attr_t pthr_attr;
	    pthread_attr_init(&pthr_attr);
	    struct sched_param prior;
	    pthread_attr_setschedpolicy(&pthr_attr,SCHED_OTHER);
	    prior.__sched_priority=2;
    	    pthread_attr_setschedparam(&pthr_attr,&prior);
	
	    pthread_create(&calcPthr,&pthr_attr,Session::Task,this);
    	    pthread_attr_destroy(&pthr_attr);
    	    if( TSYS::eventWait(m_start, true, nodePath()+"start",5) )
        	throw TError(nodePath().c_str(),_("Session process task no started!"));
        }
    }
    else
    {
	//- Stop process task -
	if( m_start )
	{
            endrun_req = true;
            pthread_kill( calcPthr, SIGALRM );
            if( TSYS::eventWait(m_start,false,nodePath()+"stop",5) )
                throw TError(nodePath().c_str(),_("Sesion process task no stoped!"));
    	    pthread_join( calcPthr, NULL );
	}
	
	//- Process all pages is off -
	list(pg_ls);
	for( int i_ls = 0; i_ls < pg_ls.size(); i_ls++ )
    	    at(pg_ls[i_ls]).at().setProcess(false);
    }
}

string Session::ico( )
{
    if( !parent().freeStat() )	return parent().at().ico();
    return "";
}

AutoHD<Project> Session::parent( )
{
    return m_parent;
}

void Session::add( const string &iid, const string &iparent )
{
    if( present(iid) )	return;
    chldAdd(m_page,new SessPage(iid,iparent));
}

void Session::openReg( const string &iid )
{
    int i_op;
    for( i_op = 0; i_op < m_open.size(); i_op++ )
	if( iid == m_open[i_op] ) break;
    if( i_op >= m_open.size() )	m_open.push_back(iid);	
}

void Session::openUnreg( const string &iid )
{
    for( int i_op = 0; i_op < m_open.size(); i_op++ )
	if( iid == m_open[i_op] ) m_open.erase(m_open.begin()+i_op);
}

AutoHD<SessPage> Session::at( const string &id )
{
    return chldAt(m_page,id);
}

void Session::uiComm( const string &com, const string &prm, const string &src )
{
    //- Find of pattern adequancy for opened page -
    string oppg;		//Opened page according of pattern

    vector<string> &op_ls = openList();
    for( int i_op = 0; i_op < op_ls.size(); i_op++ )
    {
	string cur_pt_el, cur_el;
        int i_el = 0;
        while((cur_pt_el=TSYS::pathLev(prm,i_el++)).size())
        {
    	    cur_el = TSYS::pathLev(op_ls[i_op],i_el);
	    if( cur_el.empty() || (cur_pt_el.substr(0,3) == "pg_" && cur_pt_el != cur_el) ) break;
	}
	if( cur_pt_el.empty() ) { oppg = op_ls[i_op]; break; }
    }
    //printf("TEST 20: UI Prm %s; Open page %s\n",prm.c_str(),oppg.c_str());    
    //- Individual commands process -
    try
    {	    
	//-- Go to destination page --
	string cur_pt_el;
	int i_el = 0;
	AutoHD<SessPage> cpg;
	while((cur_pt_el=TSYS::pathLev(prm,i_el++)).size())	
	{
	    string op_pg;
	    if( cur_pt_el.substr(0,3) == "pg_" ) op_pg = cur_pt_el.substr(3);
	    else if( cur_pt_el == "*" || (cur_pt_el == "$" && ( com == "next" || com == "prev")) )
	    {
		vector<string> pls;
		if( cpg.freeStat() ) list(pls); else cpg.at().pageList(pls);
		if( pls.empty() )	return;
		string cur_el = TSYS::pathLev(oppg,i_el);
		if( cur_el.empty() ) 
		{
		    if( cur_pt_el == "$" )	return;
		    op_pg = pls[0];
		}
		else
		{
		    cur_el = cur_el.substr(3);
		    int i_l;
		    for( i_l = 0; i_l < pls.size(); i_l++ )
			if( cur_el == pls[i_l] ) break;
		    if( i_l < pls.size() ) 	
		    {
			if( cur_pt_el == "$" )
			{
			    if( com == "next" ) i_l++;
			    if( com == "prev" ) i_l--;
			    i_l = (i_l < 0) ? pls.size()-1 : (i_l >= pls.size()) ? 0 : i_l;
			    op_pg = pls[i_l];
			    if( op_pg == cur_el ) return;
			}
			else op_pg = cur_el;
		    }
		    else
		    {
			if( cur_pt_el == "$" )  return;
			op_pg = pls[0];
		    }
		}
	    }
	    else op_pg = cur_pt_el;
	    //-- Go to next page --
	    cpg = cpg.freeStat() ? at(op_pg) : cpg.at().pageAt(op_pg);
	    //printf("TEST 21: Open page %s\n",cpg.at().path().c_str());
	}
	//- Open found page -
	if( !cpg.freeStat() ) 
	{
	    //-- Close previous page --
	    if( !oppg.empty() ) 
		((AutoHD<SessPage>)mod->nodeAt(oppg)).at().attrAt("pgOpen").at().setB(false);
	    //-- Open new page --
	    cpg.at().attrAt("pgOpen").at().setB(true);
	}
    }catch(...){ }
}

void *Session::Task( void *icontr )
{
    vector<string> pls;
    long long work_tm, last_tm = 0;
    struct timespec get_tm;
    Session &ses = *(Session *)icontr;
	    
    ses.endrun_req = false;
    ses.m_start    = true;
		    
    bool is_start = true;
    bool is_stop  = false;
    
    ses.list(pls);
    while(true)
    {
        //Check calk time
        unsigned long long t_cnt = SYS->shrtCnt();	

	//Calc session pages and all other items at recursion
	for( int i_l = 0; i_l < pls.size(); i_l++ )
	    ses.at(pls[i_l]).at().calc(is_start,is_stop,ses.calcClk());

	if( (ses.m_calcClk++) == 0 ) ses.m_calcClk = 1;

	ses.tm_calc = 1.0e3*((float)(SYS->shrtCnt()-t_cnt))/((float)SYS->sysClk());
	/*ses.rez_calc+=ses.tm_calc;
	if( !(ses.calcClk()%10) )
	{
	    printf("Session calc time: %d = %f\n",ses.calcClk(),ses.rez_calc);
	    ses.rez_calc=0;
	}*/
	
	if(is_stop) break;
	
        //Calc next work time and sleep
	clock_gettime(CLOCK_REALTIME,&get_tm);
	work_tm = (((long long)get_tm.tv_sec*1000000000+get_tm.tv_nsec)/((long long)ses.period()*1000000) + 1)*(long long)ses.period()*1000000;
        if(work_tm == last_tm)  work_tm+=(long long)ses.period()*1000000; //Fix early call!
        last_tm = work_tm;
        get_tm.tv_sec = work_tm/1000000000; get_tm.tv_nsec = work_tm%1000000000;
        clock_nanosleep(CLOCK_REALTIME,TIMER_ABSTIME,&get_tm,NULL);

        if(ses.endrun_req) is_stop = true;
        is_start = false;
    }

    ses.m_start = false;

    return NULL;
}

void Session::cntrCmdProc( XMLNode *opt )
{
    string a_path = opt->attr("path");
    //- Service commands process -
    if( a_path == "/serv/0" )	//Pages operations
    {
	if( ctrChkNode(opt,"openlist",RWRWRW,"root","root",SEQ_RD) )	//Open pages list
	{
	    vector<string> &lst = openList();
	    for( int i_f = 0; i_f < lst.size(); i_f++ )
		opt->childAdd("el")->setText(lst[i_f]);
	}
	return;
    }

    //Get page info
    if( opt->name() == "info" )
    {
        ctrMkNode("oscada_cntr",opt,-1,"/",_("Session: ")+id());
	if(ico().size()) ctrMkNode("img",opt,-1,"/ico","",R_R_R_);
        if(ctrMkNode("branches",opt,-1,"/br","",R_R_R_))
	    ctrMkNode("grp",opt,-1,"/br/pg_",_("Page"),R_R_R_,"root","UI",1,"list","/page/page");
        if(ctrMkNode("area",opt,-1,"/obj",_("Session")))
	{
    	    if(ctrMkNode("area",opt,-1,"/obj/st",_("State")))
	    {
		ctrMkNode("fld",opt,-1,"/obj/st/en",_("Enable"),RWRWR_,user().c_str(),"UI",1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/obj/st/start",_("Start"),RWRWR_,user().c_str(),"UI",1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/obj/st/user",_("User"),R_R_R_,"root","root",1,"tp","str");
		ctrMkNode("fld",opt,-1,"/obj/st/prj",_("Project"),RWR_R_,user().c_str(),"UI",3,"tp","str","dest","sel_ed","select","/obj/prj_ls");
		ctrMkNode("fld",opt,-1,"/obj/st/backgrnd",_("Background execution mode"),R_R_R_,"root","root",1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/obj/st/connect",_("Connections counter"),R_R_R_,"root","root",1,"tp","int");
		ctrMkNode("fld",opt,-1,"/obj/st/calc_tm",_("Calc session time (ms)"),R_R_R_,"root","root",1,"tp","real");
	    }
	    if(ctrMkNode("area",opt,-1,"/obj/cfg",_("Config")))
            {
		ctrMkNode("fld",opt,-1,"/obj/cfg/per",_("Period (ms)"),RWRWR_,user().c_str(),"UI",1,"tp","dec");
		ctrMkNode("list",opt,-1,"/obj/cfg/openPg",_("Opened pages"),R_R_R_,user().c_str(),"UI",1,"tp","str");
	    }
	}
	if(ctrMkNode("area",opt,-1,"/page",_("Pages")))
    	    ctrMkNode("list",opt,-1,"/page/page",_("Pages"),R_R_R_,"root","root",3,"tp","br","idm","1","br_pref","pg_");
        return;
    }
    //Process command to page
    if( a_path == "/ico" && ctrChkNode(opt) )   opt->setText(ico());
    else if( a_path == "/obj/st/en" )
    {
        if( ctrChkNode(opt,"get",RWRWR_,user().c_str(),"UI",SEQ_RD) ) opt->setText(TSYS::int2str(enable()));
        if( ctrChkNode(opt,"set",RWRWR_,user().c_str(),"UI",SEQ_WR) ) setEnable(atoi(opt->text().c_str()));
    }
    else if( a_path == "/obj/st/start" )
    {
	if( ctrChkNode(opt,"get",RWRWR_,user().c_str(),"UI",SEQ_RD) )	opt->setText(TSYS::int2str(start()));
        if( ctrChkNode(opt,"set",RWRWR_,user().c_str(),"UI",SEQ_WR) )	setStart(atoi(opt->text().c_str()));
    }
    else if( a_path == "/obj/st/user" && ctrChkNode(opt) )	opt->setText(user());
    else if( a_path == "/obj/st/prj" )
    {
	if( ctrChkNode(opt,"get",RWR_R_,user().c_str(),"UI",SEQ_RD) )	opt->setText(projNm());
	if( ctrChkNode(opt,"set",RWR_R_,user().c_str(),"UI",SEQ_WR) )	setProjNm(opt->text());
    }
    else if( a_path == "/obj/st/backgrnd" && ctrChkNode(opt) )	opt->setText(TSYS::int2str(backgrnd()));
    else if( a_path == "/obj/st/connect" && ctrChkNode(opt) )	opt->setText(TSYS::int2str(connects()));
    else if( a_path == "/obj/st/calc_tm" && ctrChkNode(opt) )	opt->setText(TSYS::real2str(calcTm(),6));
    else if( a_path == "/obj/prj_ls" && ctrChkNode(opt) )
    {
	vector<string> lst;
	mod->prjList(lst);
	for( unsigned i_f=0; i_f < lst.size(); i_f++ )
	    opt->childAdd("el")->setAttr("id",lst[i_f])->setText(mod->prjAt(lst[i_f]).at().name());
    }
    else if( a_path == "/obj/cfg/per" )
    {
	if( ctrChkNode(opt,"get",RWR_R_,user().c_str(),"UI",SEQ_RD) )	opt->setText(TSYS::int2str(period()));
	if( ctrChkNode(opt,"set",RWR_R_,user().c_str(),"UI",SEQ_WR) )	setPeriod(atoi(opt->text().c_str()));
    }
    else if( a_path == "/obj/cfg/openPg" && ctrChkNode(opt) )
    {
	vector<string> &lst = openList();
	for( unsigned i_f=0; i_f < lst.size(); i_f++ )
	    opt->childAdd("el")->setText(lst[i_f]);
    }    
    else if( a_path == "/page/page" && ctrChkNode(opt) )
    {
	vector<string> lst;
	list(lst);
	for( unsigned i_f=0; i_f < lst.size(); i_f++ )
	    opt->childAdd("el")->setAttr("id",lst[i_f])->setText(at(lst[i_f]).at().name());
    }
}

//************************************************
//* Page of Project's session                    *
//************************************************
SessPage::SessPage( const string &iid, const string &ipage ) : SessWdg(iid,ipage)
{
    m_page = grpAdd("pg_");    
}

SessPage::~SessPage( )
{
    
}

string SessPage::path( )
{
    return ownerFullId(true)+"/pg_"+id();
}

void SessPage::setEnable( bool val )
{
    if( val == enable() ) return;

    if( !val )
    {
	vector<string> pg_ls;        
	//- Unregister opened page -
	if( !(parent().at().prjFlags( )&Page::Empty) && attrAt("pgOpen").at().getB() ) 
	    ownerSess()->openUnreg(path());
	//- Disable include pages -
	pageList(pg_ls);
	for(int i_l = 0; i_l < pg_ls.size(); i_l++ )
	    pageAt(pg_ls[i_l]).at().setEnable(false);
	//- Delete included widgets -
	for(int i_l = 0; i_l < pg_ls.size(); i_l++ )
	    pageDel(pg_ls[i_l]);
    }
    
    //- Call parrent enable method -
    SessWdg::setEnable(val);

    if( val )
    {
	vector<string> pg_ls;        
	//- Register opened page -
	if( !(parent().at().prjFlags( )&Page::Empty) && attrAt("pgOpen").at().getB() ) 
	    ownerSess()->openReg(path());	
	//- Create included pages -
	parent().at().pageList(pg_ls);
	for( int i_p = 0; i_p < pg_ls.size(); i_p++ )
	    if( !pagePresent(pg_ls[i_p]) )
		pageAdd(pg_ls[i_p],parent().at().pageAt(pg_ls[i_p]).at().path());		
	//- Enable included pages -
	pageList(pg_ls);
	for(int i_l = 0; i_l < pg_ls.size(); i_l++ )
	    pageAt(pg_ls[i_l]).at().setEnable(true);
    }
}

void SessPage::setProcess( bool val )
{
    if( !enable() ) return;

    //- Change process state for included pages -
    vector<string> ls;
    pageList(ls);
    for(int i_l = 0; i_l < ls.size(); i_l++ )
        pageAt(ls[i_l]).at().setProcess(val);

    //- Change self process state -
    if( val && !parent().at().parent().freeStat() && ( attrAt("pgOpen").at().getB() || 
	    attrAt("pgNoOpenProc").at().getB() ) )
	SessWdg::setProcess(true);
    else if( !val ) SessWdg::setProcess(false);
}

AutoHD<Page> SessPage::parent( )
{
    return Widget::parent();
}

void SessPage::pageAdd( const string &iid, const string &iparent )
{
    if( pagePresent(iid) )return;
    chldAdd(m_page,new SessPage(iid,iparent));
}

AutoHD<SessPage> SessPage::pageAt( const string &iid )
{
    return chldAt(m_page,iid);
}

void SessPage::calc( bool first, bool last, unsigned clcClk )
{
    //- Process self data -
    if( process() ) SessWdg::calc(first,last,clcClk);
    
    //- Put calculate to include pages -
    vector<string> ls;	
    pageList(ls);
    for(int i_l = 0; i_l < ls.size(); i_l++ )
        pageAt(ls[i_l]).at().calc(first,last,clcClk);
}

bool SessPage::attrChange( Attr &cfg, void *prev )
{
    if( cfg.id() == "pgOpen" && enable() )
    {
	if( cfg.getB() && !process() ) 
	{
	    setProcess(true);
	    ownerSess()->openReg(path());
	}
	if( !cfg.getB() && process() )
	{ 
	    ownerSess()->openUnreg(path());
	    if( !attrAt("pgNoOpenProc").at().getB() )	setProcess(false);
	}
    }
    
    return SessWdg::attrChange( cfg, prev );
}

bool SessPage::cntrCmdGeneric( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
        SessWdg::cntrCmdGeneric(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Session page: ")+ownerFullId()+"/"+id());
	if( enable() && !(parent().at().prjFlags( )&Page::Empty) )
	    ctrMkNode("fld",opt,1,"/wdg/st/open",_("Open"),RWRWR_,user().c_str(),grp().c_str(),1,"tp","bool");
	if( enable() && parent().at().prjFlags()&(Page::Template|Page::Container) )
	{
	    if(ctrMkNode("area",opt,1,"/page",_("Pages")))
	        ctrMkNode("list",opt,-1,"/page/page",_("Pages"),R_R_R_,"root","UI",3,"tp","br","idm","1","br_pref","pg_");
	    if(ctrMkNode("branches",opt,-1,"/br","",R_R_R_))
	        ctrMkNode("grp",opt,-1,"/br/pg_",_("Page"),R_R_R_,"root","UI",1,"list","/page/page");
	}
        return true;
    }
    //Process command to page
    string a_path = opt->attr("path");    
    if( a_path == "/wdg/st/open" && enable() && !(parent().at().prjFlags( )&Page::Empty) )
    {
	if( ctrChkNode(opt,"get",RWRWR_,user().c_str(),grp().c_str(),SEQ_RD) ) 
	    opt->setText(TSYS::int2str(attrAt("pgOpen").at().getB()));
	if( ctrChkNode(opt,"set",RWRWR_,user().c_str(),grp().c_str(),SEQ_WR) ) 
	    attrAt("pgOpen").at().setB(atoi(opt->text().c_str()));
    }        
    else if( a_path == "/page/page" && ctrChkNode(opt) )
    {
	vector<string> lst;
	pageList(lst);
	for( unsigned i_f=0; i_f < lst.size(); i_f++ )
	    opt->childAdd("el")->setAttr("id",lst[i_f])->setText(pageAt(lst[i_f]).at().name());
    }
    else if( SessWdg::cntrCmdGeneric(opt) ) return true;
    else return false;
}


//************************************************
//* Session page's widget                        *
//************************************************
SessWdg::SessWdg( const string &iid, const string &iparent ) : 
    Widget(iid,iparent), m_proc(false), TValFunc(iid+"_wdg",NULL), m_clc(0), m_mdfClc(0)
{
    
}

SessWdg::~SessWdg( )
{
    
}

SessWdg *SessWdg::ownerSessWdg( bool base )
{
    if( nodePrev(true) ) 
    {
	if( !base && dynamic_cast<SessPage*>(nodePrev()) )   return NULL;
	return dynamic_cast<SessWdg*>(nodePrev());
    }
    return NULL;
}

SessPage *SessWdg::ownerPage()
{
    if( nodePrev(true) && dynamic_cast<SessPage*>(nodePrev()) ) return (SessPage*)nodePrev();
    SessWdg *own = ownerSessWdg( );
    if( own )	return own->ownerPage( );
    return NULL;
}

Session *SessWdg::ownerSess()
{
    SessPage *own = ownerPage( );
    if( own )   return own->ownerSess();
    if( nodePrev(true) ) return dynamic_cast<Session*>(nodePrev());
    return NULL;
}				

string SessWdg::path( )
{
    return ownerFullId(true)+"/wdg_"+id();
}

string SessWdg::ownerFullId( bool contr )
{
    SessWdg *ownW = ownerSessWdg( );
    if( ownW )	return ownW->ownerFullId(contr)+(contr?"/wdg_":"/")+ownW->id();
    SessPage *ownP = ownerPage( );
    if( ownP )	return ownP->ownerFullId(contr)+(contr?"/pg_":"/")+ownP->id();
    return string(contr?"/ses_":"/")+ownerSess()->id();
}

void SessWdg::setEnable( bool val )
{
    Widget::setEnable(val);

    if( !val )
    {
	//- Delete included widgets -
	vector<string> ls;	
	wdgList(ls);
	for(int i_l = 0; i_l < ls.size(); i_l++ )
	    wdgDel(ls[i_l]);
    }
}

void SessWdg::setProcess( bool val )
{
    if( val && !enable() ) setEnable(true);
    //- Prepare process function value level -
    if( val && calcProg().size() )
    {
	//-- Prepare function io structure --
	TFunction fio(parent().at().calcId());
	//--- Add generic io ---
	fio.ioIns( new IO("f_frq",_("Function calculate frequency (Hz)"),IO::Real,IO::Default,"1000",false),0);
	fio.ioIns( new IO("f_start",_("Function start flag"),IO::Boolean,IO::Default,"0",false),1);
	fio.ioIns( new IO("f_stop",_("Function stop flag"),IO::Boolean,IO::Default,"0",false),2);
	//--- Add calc widget's attributes ---
	AutoHD<Widget> fulw = parentNoLink();
	vector<string> iwls, als;
	fulw.at().wdgList(iwls);
	for( int i_w = -1; i_w < (int)iwls.size(); i_w++ )
	{	
	    AutoHD<Widget> curw = fulw;
	    if( i_w >= 0 ) curw = fulw.at().wdgAt(iwls[i_w]);
	
	    curw.at().attrList(als);
	    for( int i_a = 0; i_a < als.size(); i_a++ )
	    {
		AutoHD<Attr> cattr = curw.at().attrAt(als[i_a]);
		if( cattr.at().flgSelf()&Attr::ProcAttr )
		{
		    IO::Type tp = IO::String;
		    switch( cattr.at().type() )
		    {
			case TFld::Boolean: tp = IO::Boolean;	break;
			case TFld::Integer: tp = IO::Integer;	break;
			case TFld::Real:    tp = IO::Real;	break;
			case TFld::String:  tp = IO::String;	break;
		    }
		    fio.ioAdd( new IO((((i_w<0)?"":iwls[i_w]+"_")+als[i_a]).c_str(),
				      (((i_w<0)?"":curw.at().name()+".")+cattr.at().name()).c_str(),
				      tp,IO::Output,"",false,
				      (((i_w<0)?"./":iwls[i_w]+"/")+als[i_a]).c_str()) );
		}
	    }
	}
	if( attrPresent("event") ) fio.ioAdd( new IO("event",_("Event"),IO::String,IO::Output) );
	
	//-- Compile function --	
	work_prog = SYS->daq().at().at(TSYS::strSepParse(calcLang(),0,'.')).at().
                    compileFunc(TSYS::strSepParse(calcLang(),1,'.'),fio,calcProg());
	//-- Connect to compiled function --
	TValFunc::func(&((AutoHD<TFunction>)SYS->nodeAt(work_prog,1)).at());

    }
    if( !val )	TValFunc::func(NULL);

    //- Change process for included widgets -
    vector<string> ls;
    wdgList(ls);
    for(int i_l = 0; i_l < ls.size(); i_l++ )
        wdgAt(ls[i_l]).at().setProcess(val);

    //-- Make process element's lists --
    if( val ) prcElListUpdate( );

    m_proc = val;
}

string SessWdg::ico( )
{
    if( !parent().freeStat() )  return parent().at().ico();
    return "";
}

string SessWdg::user( )
{
    if( !parent().freeStat() )  return parent().at().user();
    return Widget::user();
}

string SessWdg::grp( )
{
    if( !parent().freeStat() )  return parent().at().grp();
    return Widget::grp();
}

short SessWdg::permit( )
{
    //return R_R_R_;
    if( !parent().freeStat() )	return parent().at().permit();
    return Widget::permit();
}

string SessWdg::calcLang( )
{
    if( !parent().freeStat() )    return parent().at().calcLang();
    return "";
}

string SessWdg::calcProg( )
{
    if( !parent().freeStat() )    return parent().at().calcProg();
    return "";
}

string SessWdg::resourceGet( const string &id, string *mime )
{
    string mimeType, mimeData;
    
    mimeData = parent().at().resourceGet( id, &mimeType );    
    if( mime )	*mime = mimeType;
    
    return mimeData;
}

void SessWdg::wdgAdd( const string &iid, const string &name, const string &iparent )
{
    if( !isContainer() )  throw TError(nodePath().c_str(),_("No container widget!"));
    if( wdgPresent(iid) ) return;

    chldAdd(inclWdg,new SessWdg(iid,iparent));
}

AutoHD<SessWdg> SessWdg::wdgAt( const string &wdg )
{
    return Widget::wdgAt(wdg);
}

void SessWdg::eventAdd( const string &ev )
{
    if( !attrPresent("event") )	return;
    Res &res = ownerSess()->eventRes();
    res.resRequestW( );
    attrAt("event").at().setS(attrAt("event").at().getS()+ev);
    res.resReleaseW( );
}

string SessWdg::eventGet( bool clear )
{
    if( !attrPresent("event") )	return "";
    Res &res = ownerSess()->eventRes();
    
    res.resRequestW( );    
    string rez = attrAt("event").at().getS();
    if( clear )	attrAt("event").at().setS("");
    res.resReleaseW( );

    return rez;
}

void SessWdg::prcElListUpdate( )
{
    vector<string> ls;
    wdgList(ls);
    m_wdgChldAct.clear();
    for(int i_l = 0; i_l < ls.size(); i_l++ )
	if( wdgAt(ls[i_l]).at().process() )
	    m_wdgChldAct.push_back(ls[i_l]);
    attrList(ls);
    m_attrLnkLs.clear();
    m_attrUILs.clear();
    for( int i_a = 0; i_a < ls.size(); i_a++ )
    {
	AutoHD<Attr> attr = attrAt(ls[i_a]);
	if( attr.at().flgSelf()&(Attr::CfgConst|Attr::CfgLnkIn|Attr::CfgLnkOut) && !attr.at().cfgVal().empty() )
	    m_attrLnkLs.push_back(ls[i_a]);
	if( !(attr.at().flgGlob()&Attr::IsUser) ) m_attrUILs.push_back(ls[i_a]);
    }
}

unsigned int SessWdg::modifVal( Attr &cfg )
{ 
    if( !(cfg.flgGlob()&Attr::IsUser) )	m_mdfClc = m_clc;
    return m_clc; 
}

void SessWdg::calc( bool first, bool last, unsigned clc )
{
    string sw_attr, s_attr, obj_tp;

    if( !process() )	return;
    m_clc = clc;

    if( !(clc%100) ) prcElListUpdate( );

    //- Calculate include widgets -
    //vector<string> ls;	
    //wdgList(ls);
    for(int i_l = 0; i_l < m_wdgChldAct.size(); i_l++ )
        wdgAt(m_wdgChldAct[i_l]).at().calc(first,last,clc);
	
    try
    {    
	//- Load events to process -
	string wevent = eventGet(true);
	
	//- Process input links and constants -    
	AutoHD<Attr> attr;    
	//attrList(ls);
	for( int i_a = 0; i_a < m_attrLnkLs.size(); i_a++ )
	{
	    attr = attrAt(m_attrLnkLs[i_a]);
	    if( attr.at().flgSelf()&Attr::CfgConst )	attr.at().setS(attr.at().cfgVal());
	    else if( attr.at().flgSelf()&Attr::CfgLnkIn && !attr.at().cfgVal().empty() )
	    {
		obj_tp = TSYS::strSepParse(attr.at().cfgVal(),0,':')+":";
		if( obj_tp == "val:" )	attr.at().setS(attr.at().cfgVal().substr(obj_tp.size()));
		else if( obj_tp == "prm:" )
		{
		    
		    if( attr.at().flgGlob()&Attr::Address )	
			attr.at().setS("/DAQ"+attr.at().cfgVal().substr(obj_tp.size()));
		    else switch( attr.at().type() )
		    {
			case TFld::Boolean:
			    attr.at().setB(((AutoHD<TVal>)SYS->daq().at().nodeAt(
				attr.at().cfgVal(),0,0,obj_tp.size())).at().getB());
			    break;
			case TFld::Integer:
			    attr.at().setI(((AutoHD<TVal>)SYS->daq().at().nodeAt(
				attr.at().cfgVal(),0,0,obj_tp.size())).at().getI());
			    break;
			case TFld::Real:
			    attr.at().setR(((AutoHD<TVal>)SYS->daq().at().nodeAt(
				attr.at().cfgVal(),0,0,obj_tp.size())).at().getR());
			    break;
			case TFld::String:
			    attr.at().setS(((AutoHD<TVal>)SYS->daq().at().nodeAt(
				attr.at().cfgVal(),0,0,obj_tp.size())).at().getS());
			    break;
		    }
		}
		else if( obj_tp == "addr:" )
		    attr.at().setS(attr.at().cfgVal().substr(obj_tp.size()));
    	    }
	}

	if( TValFunc::func() )
	{    
	    //- Load events to calc procedure -
	    int evId = ioId("event");
	    if( evId >= 0 )	setS(evId,wevent);
	
	    //-- Load data to calc area --
	    setR(0,1000./ownerSess()->period());
	    setB(1,first);
	    setB(2,last);
    	    for( int i_io = 3; i_io < ioSize( ); i_io++ )
	    {
	    	sw_attr = TSYS::pathLev(func()->io(i_io)->rez(),0);	
		s_attr  = TSYS::pathLev(func()->io(i_io)->rez(),1);
		attr = (sw_attr==".")?attrAt(s_attr):wdgAt(sw_attr).at().attrAt(s_attr);
		switch(ioType(i_io))
		{
		    case IO::String:	setS(i_io,attr.at().getS());	break;
		    case IO::Integer: 	setI(i_io,attr.at().getI());	break;
		    case IO::Real:	setR(i_io,attr.at().getR());	break;
		    case IO::Boolean:	setB(i_io,attr.at().getB());	break;
    		}	
    	    }
    	    //-- Calc --
	    TValFunc::calc();
    	    //-- Load data from calc area --
    	    for( int i_io = 3; i_io < ioSize( ); i_io++ )
    	    {
    		sw_attr = TSYS::pathLev(func()->io(i_io)->rez(),0);
    		s_attr  = TSYS::pathLev(func()->io(i_io)->rez(),1);
    		attr = (sw_attr==".")?attrAt(s_attr):wdgAt(sw_attr).at().attrAt(s_attr);
    		switch(ioType(i_io))
    		{
    		    case IO::String:	attr.at().setS(getS(i_io));	break;
    		    case IO::Integer:	attr.at().setI(getI(i_io));	break;
    		    case IO::Real:     	attr.at().setR(getR(i_io));	break;
    		    case IO::Boolean:   attr.at().setB(getB(i_io));	break;
    		}
    	    }
	    //-- Save events from calc procedure --
	    if( evId >= 0 ) wevent = getS(evId);
	}
    	
	//-- Process output links --
    	for( int i_a = 0; i_a < m_attrLnkLs.size(); i_a++ )
    	{
    	    attr = attrAt(m_attrLnkLs[i_a]);
	    obj_tp = TSYS::strSepParse(attr.at().cfgVal(),0,':')+":";
    	    if( attr.at().flgSelf()&Attr::CfgLnkOut && !attr.at().cfgVal().empty() && obj_tp == "prm:" )
		switch( attr.at().type() )
		{
		    case TFld::Boolean:		
		        ((AutoHD<TVal>)SYS->daq().at().nodeAt(
			    attr.at().cfgVal(),0,0,obj_tp.size())).at().setB(attr.at().getB());
			break;
		    case TFld::Integer:
			((AutoHD<TVal>)SYS->daq().at().nodeAt(
			    attr.at().cfgVal(),0,0,obj_tp.size())).at().setI(attr.at().getI());
			break;
		    case TFld::Real:
			((AutoHD<TVal>)SYS->daq().at().nodeAt(
			    attr.at().cfgVal(),0,0,obj_tp.size())).at().setR(attr.at().getR());
			break;
		    case TFld::String:
		        ((AutoHD<TVal>)SYS->daq().at().nodeAt(
			    attr.at().cfgVal(),0,0,obj_tp.size())).at().setS(attr.at().getS());
			break;
		}
    	}
    
	//-- Process widget's events --
	if( !wevent.empty() )
	{
	    string sevup, sev;
	    for( int el_off = 0; (sev=TSYS::strSepParse(wevent,0,';',&el_off)).size(); )
	    {
		//-- Check for process events --
		string sprc_lst = attrAt("evProc").at().getS(), 
		       sprc;	
		bool evProc = false;
		for( int elp_off = 0; (sprc=TSYS::strSepParse(sprc_lst,0,'\n',&elp_off)).size(); )
		    if( TSYS::strSepParse(sprc,0,':') == sev )
		    {
	    		ownerSess()->uiComm(TSYS::strSepParse(sprc,1,':'),TSYS::strSepParse(sprc,2,':'),path());
			evProc = true;
		    }
		if( !evProc ) sevup+=sev+";";
	    }
	    //-- Put left events to parent widget --
	    SessWdg *owner = ownerSessWdg(true);
	    if( owner && !sevup.empty() ) owner->eventAdd(sevup);
	}	
    }
    catch(TError err)
    {
        mess_err(err.cat.c_str(),err.mess.c_str());
        mess_err(nodePath().c_str(),_("Widget '%s' calc is error. Process disabled."),path().c_str());
        setProcess(false);
    }
}

bool SessWdg::attrChange( Attr &cfg, void *prev )
{
    Widget::attrChange( cfg, prev );
    if( cfg.id() == "active" )
    {
        if( cfg.getB() && !attrPresent("event") )
	    attrAdd( new TFld("event",_("Events"),TFld::String,Attr::Mutable,"200") );
	if( !cfg.getB() && attrPresent("event") ) attrDel("event");
    }
}

bool SessWdg::cntrCmdServ( XMLNode *opt )
{
    string a_path = opt->attr("path");
    if( a_path == "/serv/0" )		//Attribute's value operations
    {
	if( ctrChkNode(opt,"get",RWRWRW,"root","root",SEQ_RD) )		//Get values
	{
	    unsigned  tm = strtoul(opt->attr("tm").c_str(),0,10);

	    //vector<string> ls;
	    //attrList(ls);
	    AutoHD<Attr> attr;
	    if( m_mdfClc >= tm )
		for( int i_l = 0; i_l < m_attrUILs.size(); i_l++ )
		{
		    attr = attrAt(m_attrUILs[i_l]);
		    if( !(attr.at().flgGlob()&Attr::IsUser) && attr.at().modif() >= tm )
			opt->childAdd("el")->setAttr("id",m_attrUILs[i_l].c_str())->
				    	     setAttr("pos",TSYS::int2str(attr.at().fld().reserve()))->
					     setText(attr.at().getS());
		}
	    opt->setAttr("tm",TSYS::uint2str(m_clc));
	}
	else if( ctrChkNode(opt,"set",RWRWRW,"root","root",SEQ_WR) )	//Set values
	    for( int i_ch = 0; i_ch < opt->childSize(); i_ch++ )
	    {
	        string aid = opt->childGet(i_ch)->attr("id");
	        if( aid == "event" ) eventAdd(opt->childGet(i_ch)->text()+";");
	        else attrAt(aid).at().setS(opt->childGet(i_ch)->text());
	    }
	return true;
    }else return Widget::cntrCmdServ(opt);
    
    return false;
}

bool SessWdg::cntrCmdGeneric( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
        Widget::cntrCmdGeneric(opt);
	ctrMkNode("fld",opt,1,"/wdg/st/proc",_("Process"),RWRWR_,user().c_str(),grp().c_str(),1,"tp","bool");
        return true;
    }
    //Process command to page
    string a_path = opt->attr("path");    
    if( a_path.substr(0,5) == "/attr" &&
            TSYS::pathLev(a_path,1).size() > 4 &&
            TSYS::pathLev(a_path,1).substr(0,4) == "sel_" && TCntrNode::ctrChkNode(opt) )
    {
        AutoHD<Attr> attr = attrAt(TSYS::pathLev(a_path,1).substr(4));
	for( int i_a=0; i_a < attr.at().fld().selNm().size(); i_a++ )
            opt->childAdd("el")->setText(attr.at().fld().selNm()[i_a]);
    }
    else if( a_path.substr(0,6) == "/attr/" )
    {
	//unsigned tm = ownerSess()->calcClk( );//time(NULL);    
        AutoHD<Attr> attr = attrAt(TSYS::pathLev(a_path,1));
	if( ctrChkNode(opt,"get",(attr.at().fld().flg()&TFld::NoWrite)?(permit()&~0222):permit(),user().c_str(),grp().c_str(),SEQ_RD) )
        {
            if( attr.at().fld().flg()&TFld::Selected )  opt->setText(attr.at().getSEL());
            else                                        opt->setText(attr.at().getS());
	}
        if( ctrChkNode(opt,"set",(attr.at().fld().flg()&TFld::NoWrite)?(permit()&~0222):permit(),user().c_str(),grp().c_str(),SEQ_WR) )
        {
	    if( attr.at().id() == "event" )	eventAdd(opt->text()+";");
            else if( attr.at().fld().flg()&TFld::Selected )  
						attr.at().setSEL(opt->text());
            else				attr.at().setS(opt->text());
        }
    }
    else if( a_path == "/wdg/st/proc" )
    {
	if( ctrChkNode(opt,"get",RWRWR_,user().c_str(),grp().c_str(),SEQ_RD) ) opt->setText(TSYS::int2str(process()));
	if( ctrChkNode(opt,"set",RWRWR_,user().c_str(),grp().c_str(),SEQ_WR) ) setProcess(atoi(opt->text().c_str()));
    }
    else if( Widget::cntrCmdGeneric(opt) ) return true;
    else return false;
}

void SessWdg::cntrCmdProc( XMLNode *opt )
{
    if( cntrCmdServ(opt) ) return;

    //Get page info
    if( opt->name() == "info" )
    {
        cntrCmdGeneric(opt);
        cntrCmdAttributes(opt);
        return;
    }
    cntrCmdGeneric(opt) || cntrCmdAttributes(opt);
}
