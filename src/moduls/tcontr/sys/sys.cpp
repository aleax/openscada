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

#include <sys/time.h>
#include <getopt.h>
#include <unistd.h>
#include <dirent.h>
#include <signal.h>
#include <fcntl.h>

#include <terror.h>
#include <tsys.h>
#include <tkernel.h>
#include <tmessage.h>
#include <ttransports.h>
#include <tcontrollers.h>
#include "sys.h"

//============ Modul info! =====================================================
#define NAME_MODUL  "SysContr"
#define NAME_TYPE   "Controller"
#define VER_TYPE    VER_CNTR
#define VERSION     "0.5.0"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "System controller used for monitoring and control OS"
#define LICENSE     "GPL"
//==============================================================================

//Present controller parameter
#define PRM_All   "All"
#define PRM_B_All "PRM_BD"

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
	SystemCntr::TTpContr *self_addr = NULL;

    	if( AtMod.name == NAME_MODUL && AtMod.type == NAME_TYPE && AtMod.t_ver == VER_TYPE )
	    self_addr = new SystemCntr::TTpContr( source );

	return ( self_addr );
    }
}

using namespace SystemCntr;

//======================================================================
//==== TTpContr ======================================================== 
//======================================================================

TTpContr::TTpContr( string name )  
{
    NameModul = NAME_MODUL;
    NameType  = NAME_TYPE;
    Vers      = VERSION;
    Autors    = AUTORS;
    DescrMod  = DESCRIPTION;
    License   = LICENSE;
    Source    = name;    
}

TTpContr::~TTpContr()
{    

}

void TTpContr::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "==================== Module %s options =======================\n"
    "------------------ Fields <%s> sections of config file --------------\n"
    "\n",NAME_MODUL,NAME_MODUL);
}


void TTpContr::mod_CheckCommandLine( )
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

void TTpContr::mod_UpdateOpt( )
{

}

void TTpContr::mod_connect( )
{    
    TModule::mod_connect( );
    //==== Desribe controler's bd fields ====
    SFld elem[] =         
    {    
	{PRM_B_All ,I18N("System parameteres table"),T_STRING,"system","30"},
	{"PERIOD"  ,I18N("The request period (ms)") ,T_DEC   ,"1000"  ,"5" ,"0;10000"}
    };
    LoadCfg(elem,sizeof(elem)/sizeof(SFld));
    
    SFld elemPrm[] =         
    {
	{"TYPE" ,I18N("System part") ,T_DEC|T_SELECT|V_NOVAL|F_PREV       ,"0","2","3;4;2;0;1",
	    (I18Ns("CPU")+";"+I18Ns("Memory")+";"+I18Ns("Up time")+";"+I18Ns("Hdd temperature")+";"+I18Ns("Sensors")).c_str()},
	{"SUBT" ,""                  ,T_DEC|T_SELECT|V_NOVAL|F_PREV|F_SELF,"" ,"2"}
    };
    LoadTpParmCfg(AddTpParm(PRM_All,PRM_B_All,I18N("All parameters")),elemPrm,sizeof(elemPrm)/sizeof(SFld));
}

TController *TTpContr::ContrAttach( const string &name, const SBDS &bd)
{
    return( new TMdContr(name,bd,this,this));    
}

//======================================================================
//==== TMdContr 
//======================================================================

TMdContr::TMdContr( string name_c, const SBDS &bd, ::TTipController *tcntr, ::TElem *cfgelem) :
	::TController(name_c,bd,tcntr,cfgelem), endrun(false), period(cfg("PERIOD").getI())
{    

}

TMdContr::~TMdContr()
{
    if( run_st ) Stop();
}

TParamContr *TMdContr::ParamAttach( const string &name, int type )
{    
    return(new TMdPrm(name,&Owner().at_TpPrm(type),this));
}

void TMdContr::Load_( )
{

}

void TMdContr::Save_( )
{

}

void TMdContr::Start_( )
{      
    pthread_attr_t      pthr_attr;
    struct sched_param  prior;
    //---- Attach parameter algoblock ----
    vector<string> list_p;

    if( !run_st )
    {
	list(list_p);
	for(unsigned i_prm=0; i_prm < list_p.size(); i_prm++)
	    p_hd.push_back( att(list_p[i_prm],Name()+"_start") );
	//---------------------------------------
	pthread_attr_init(&pthr_attr);
	pthread_attr_setschedpolicy(&pthr_attr,SCHED_OTHER);
	pthread_create(&pthr_tsk,&pthr_attr,Task,this);
	pthread_attr_destroy(&pthr_attr);
	if( SYS->event_wait( run_st, true, string(NAME_MODUL)+": Controller "+Name()+" is starting....",5) )
	    throw TError("%s: Controller %s no started!",NAME_MODUL,Name().c_str());
    }    
}

void TMdContr::Stop_( )
{  
    if( run_st )
    {
	endrun = true;
	pthread_kill(pthr_tsk, SIGALRM);
	if( SYS->event_wait( run_st, false, string(NAME_MODUL)+": Controller "+Name()+" is stoping....",5) )
	    throw TError("%s: Controller %s no stoped!",NAME_MODUL,Name().c_str());
	pthread_join(pthr_tsk, NULL);

    	for(unsigned i_prm=0; i_prm < p_hd.size(); i_prm++)
	    det( p_hd[i_prm] );
    	p_hd.clear();
    }
} 

void *TMdContr::Task(void *contr)
{
    struct itimerval mytim;             //Interval timer
    TMdContr *cntr = (TMdContr *)contr;

#if OSC_DEBUG
    cntr->Owner().m_put("DEBUG",MESS_DEBUG,"%s: Thread <%d>!",cntr->Name().c_str(),getpid() );
#endif

    if(cntr->period == 0) return(NULL);
    mytim.it_interval.tv_sec = 0; mytim.it_interval.tv_usec = cntr->period*1000;
    mytim.it_value.tv_sec    = 0; mytim.it_value.tv_usec    = cntr->period*1000;

    signal(SIGALRM,wakeup);
    setitimer(ITIMER_REAL,&mytim,NULL);    
    
    cntr->run_st = true;  cntr->endrun = false;
    while( !cntr->endrun )
    {
	for(unsigned i_p=0; i_p < cntr->p_hd.size(); i_p++)
	    ((TMdPrm &)cntr->at(cntr->p_hd[i_p])).getVal();
	pause();
    }
    cntr->run_st = false;

    return(NULL);    
}

//======================================================================
//==== TMdPrm 
//======================================================================

TMdPrm::TMdPrm( string name, TTipParam *tp_prm, TController *contr) : 
    TParamContr(name,tp_prm,contr), m_type(PRM_NONE)
{        
    cfg("TYPE").setSEL(Owner().Owner().I18N("CPU"));
}

TMdPrm::~TMdPrm( )
{    
    free();
}

void TMdPrm::vlGet( TVal &val )
{
    
}

void TMdPrm::getVal()
{
    if( m_type == PRM_CPU ) 	    prm.p_cpu->getVal();
    else if( m_type == PRM_MEM )    prm.p_mem->getVal();
    else if( m_type == PRM_UPTIME ) prm.p_upt->getVal();
    else if( m_type == PRM_HDDT )   prm.p_hdd->getVal();
    else if( m_type == PRM_SENSOR ) prm.p_sens->getVal();
}

void TMdPrm::free( )
{
    if( m_type == PRM_CPU ) 	    delete prm.p_cpu;
    else if( m_type == PRM_MEM )    delete prm.p_mem;
    else if( m_type == PRM_UPTIME ) delete prm.p_upt;
    else if( m_type == PRM_HDDT )   delete prm.p_hdd;
    else if( m_type == PRM_SENSOR ) delete prm.p_sens;
    m_type = PRM_NONE;
}

void TMdPrm::setType( char tp )
{
    if( m_type == tp ) return;
    
    free();

    //Create new type
    try
    {
	if( tp == PRM_CPU ) 	  	{ prm.p_cpu = new CPU(*this); prm.p_cpu->init(); }
	else if( tp == PRM_MEM )   	{ prm.p_mem = new Mem(*this); prm.p_mem->init(); }
	else if( tp == PRM_UPTIME )  	{ prm.p_upt = new UpTime(*this); prm.p_upt->init(); }
	else if( tp == PRM_HDDT ) 	{ prm.p_hdd = new Hddtemp(*this); prm.p_hdd->init(); }
	else if( tp == PRM_SENSOR )	{ prm.p_sens = new Lmsensors(*this); prm.p_sens->init(); }
	m_type = tp;
    }catch(TError err) { m_type = PRM_NONE; }
}

bool TMdPrm::cfChange( TCfg &i_cfg )
{        
    //Change TYPE parameter
    if( i_cfg.name() == "TYPE" )
    {
	if( i_cfg.getSEL() == Owner().Owner().I18N("CPU") ) 			setType( PRM_CPU );
	else if( i_cfg.getSEL() == Owner().Owner().I18N("Memory") ) 		setType( PRM_MEM );
	else if( i_cfg.getSEL() == Owner().Owner().I18N("Up time") ) 		setType( PRM_UPTIME );
	else if( i_cfg.getSEL() == Owner().Owner().I18N("Hdd temperature") ) 	setType( PRM_HDDT );
	else if( i_cfg.getSEL() == Owner().Owner().I18N("Sensors") )    	setType( PRM_SENSOR );
	else return(false);
       	return(true);       
    }    
    //Change SUBTYPE parameter
    else 
    {
	if( m_type == PRM_CPU || m_type == PRM_UPTIME ) return true;
	else if( m_type == PRM_MEM ) 	prm.p_mem->chSub( );
	else if( m_type == PRM_HDDT )   prm.p_hdd->chSub( );
	else if( m_type == PRM_SENSOR ) prm.p_sens->chSub( );
	else return false;
       	return true;       
    }    
    return false;
}

//======================================================================
//==== HddTemp
//======================================================================
Hddtemp::Hddtemp( TMdPrm &mprm ) : TElem("hddtemp"),
    prm(mprm), tr( mprm.Owner().Owner().Owner().Owner().Transport() ),
    t_tr("socket"),n_tr("tr_"+mprm.Name()), c_subt(prm.cfg("SUBT")), err_st(false)
{
    ((TTipTransport &)tr.gmd_at(t_tr).at()).out_add(n_tr);
    otr = new AutoHD<TTransportOut>;
    *otr = (((TTipTransport &)tr.gmd_at(t_tr).at()).out_at(n_tr));
    
    otr->at().lName() = prm.Owner().Owner().I18N("Parametr Hddtemp");
    otr->at().addres() = "TCP:127.0.0.1:7634";
    otr->at().start();
    
    SFld valE[] =
    {
	{"disk" ,prm.Owner().Owner().I18N("Name")        ,T_STRING|F_NWR,"" ,"" },
	{"ed"   ,prm.Owner().Owner().I18N("Measure unit"),T_STRING|F_NWR,"" ,"" },
	{"value",prm.Owner().Owner().I18N("Temperature") ,T_DEC   |F_NWR,"0","3"}	
    };
    for( unsigned i_el = 0; i_el < sizeof(valE)/sizeof(SFld); i_el++ )
	elAdd(&valE[i_el]);    
    prm.vlAttElem( this );
    // Make direct access
    atrb.push_back(prm.vlAt("disk"));
    atrb.push_back(prm.vlAt("ed"));
    atrb.push_back(prm.vlAt("value"));
}

Hddtemp::~Hddtemp()
{
    delete otr;    
    ((TTipTransport &)tr.gmd_at(t_tr).at()).out_del(n_tr);
    atrb.clear();
    prm.vlDetElem( this );
}

void Hddtemp::init()
{
    //Create Config
    c_subt.fld().descr() = prm.Owner().Owner().I18N("Disk");
    c_subt.fld().val_i().clear();
    c_subt.fld().nSel().clear();    

    vector<string> list;
    dList(list);
    for( int i_l = 0; i_l < list.size(); i_l++ )
    {
	c_subt.fld().val_i().push_back(i_l);
	c_subt.fld().nSel().push_back(list[i_l]);
    }
    if( list.size() ) c_subt.setSEL(list[0]);    
}

void Hddtemp::dList( vector<string> &list )
{    
    int  len;
    char buf[20];
    string val;
    try 
    { 
	len = otr->at().IOMess("1",1,buf,sizeof(buf),1); buf[len] = '\0';	
	err_st = false;	
	val.append(buf,len);
	while( len == sizeof(buf) )
	{
	    len = otr->at().IOMess(NULL,0,buf,sizeof(buf),1); buf[len] = '\0';
	    val.append(buf,len);
	}
	
	len = -1;
	do
	{	    	    
	    int l_nm;
	    string val_t;
	    
	    //Get one disk
	    len += 1;	    
	    l_nm = val.find("||",len);
	    if( l_nm == string::npos )  val_t = val.substr(len);
	    else	    		val_t = val.substr(len,l_nm-len+1);
	    len = l_nm;
	    //Parse geted disk
	    l_nm = val_t.find("|",1);
	    if( l_nm != string::npos ) list.push_back( val_t.substr(1,l_nm-1) );
	}while( len != string::npos );
    }
    catch( TError err ) 
    { 
	if( !err_st ) prm.Owner().Owner().m_put("SYS",MESS_ERR,"Error %s\n",err.what().c_str()); 
	err_st = true;
    }
}

void Hddtemp::getVal(  )
{    
    int  len;
    char buf[20];
    string val;
    try 
    { 
       	string dev =  c_subt.getSEL();
	
	len = otr->at().IOMess("1",1,buf,sizeof(buf),1);
	err_st = false;
	val.append(buf,len);
	while( len == sizeof(buf) )
	{
	    len = otr->at().IOMess(NULL,0,buf,sizeof(buf),1);
	    val.append(buf,len);
	}

	len = -1;
	do
	{	    
	    int l_nm;
	    string val_t;
	    
	    len += 1;
	    l_nm = val.find("||",len);
	    if( l_nm == string::npos )  val_t = val.substr(len);
	    else	    		val_t = val.substr(len,l_nm-len+1);
	    len = l_nm;
	    
	    l_nm = val_t.find("|",1);
	    if( l_nm != string::npos && val_t.substr(1,l_nm-1) == dev ) 
	    {
		int l_nm1 = l_nm + 1;
		
		l_nm = val_t.find("|",l_nm1)+1;
		atrb[0].at().setS( val_t.substr(l_nm1,l_nm-l_nm1-1), NULL, true ); 
		l_nm1 = l_nm;
		
		l_nm = val_t.find("|",l_nm1)+1; 
		atrb[2].at().setI( atoi(val_t.substr(l_nm1,l_nm-l_nm1-1).c_str()), NULL, true );
		l_nm1 = l_nm;
		
		l_nm = val_t.find("|",l_nm1)+1; 
		atrb[1].at().setS( val_t.substr(l_nm1,l_nm-l_nm1-1), NULL, true ); 
	    }
	}while( len != string::npos );
    }    
    catch( TError err ) 
    {
	if( !err_st ) prm.Owner().Owner().m_put("SYS",MESS_ERR,"Error %s\n",err.what().c_str()); 
	err_st = true;
    }
}

void Hddtemp::chSub( )
{

}

//======================================================================
//==== Lmsensors
//======================================================================
Lmsensors::Lmsensors( TMdPrm &mprm ) : TElem("sensor"), prm(mprm), s_path("/proc/sys/dev/sensors/")
{
    SFld valE[] =
    {
	{"value","",T_REAL|F_NWR,"0","8.2"}
    };
    for( unsigned i_el = 0; i_el < sizeof(valE)/sizeof(SFld); i_el++ )
	elAdd(&valE[i_el]);
    prm.vlAttElem( this );
}

Lmsensors::~Lmsensors()
{
    prm.vlDetElem( this );
}

void Lmsensors::init()
{
    TCfg &c_subt = prm.cfg("SUBT");
    //Create config
    c_subt.fld().descr() = prm.Owner().Owner().I18N("Sensor");
    c_subt.fld().val_i().clear();
    c_subt.fld().nSel().clear();
    
    vector<string> list;
    dList(list);
    for( int i_l = 0; i_l < list.size(); i_l++ )
    {
	c_subt.fld().val_i().push_back(i_l);
	c_subt.fld().nSel().push_back(list[i_l]);
    }
    if( list.size() ) c_subt.setSEL(list[0]);        
}

void Lmsensors::dList( vector<string> &list )
{   
    dirent *s_dirent;
    
    DIR *IdDir = opendir(s_path.c_str());
    if(IdDir == NULL) return;    
    
    while((s_dirent = readdir(IdDir)) != NULL)
    {
	if( string("..") == s_dirent->d_name || string(".") == s_dirent->d_name ) continue;
	//Open temperatures
	for( int i_el = 0;i_el < 20;i_el++)
	{
	    string s_file = string(s_dirent->d_name)+"/temp"+TSYS::int2str(i_el);
	    if( access((s_path+s_file).c_str(),R_OK) ) continue;
	    list.push_back(s_file);
	}	
	//Open fans
	for( int i_el = 0;i_el < 20;i_el++)
	{
	    string s_file = string(s_dirent->d_name)+"/fan"+TSYS::int2str(i_el);
	    if( access((s_path+s_file).c_str(),R_OK) ) continue;
	    list.push_back(s_file);
	}	
	//Open powers
	for( int i_el = 0;i_el < 20;i_el++)
	{
	    string s_file = string(s_dirent->d_name)+"/in"+TSYS::int2str(i_el);
	    if( access((s_path+s_file).c_str(),R_OK) ) continue;
	    list.push_back(s_file);
	}	
    }
    closedir(IdDir);
}

void Lmsensors::getVal(  )
{    
    float max,min,val;
    
    TCfg &c_subt = prm.cfg("SUBT");
    string sens =  c_subt.getSEL();
    string tp   =  sens.substr(sens.find("/",0)+1);

    FILE *f = fopen((s_path+sens).c_str(),"r");
    if( f == NULL ) return;
    if( tp.find("temp",0) == 0 )
    {
	if( fscanf(f,"%f %f %f",&max,&min,&val) == 3 )
    	    prm.vlAt("value").at().setR(val,NULL,true);
    }
    else if( tp.find("fan",0) == 0 )
    {
	if( fscanf(f,"%f %f",&min,&val) == 2 )
	    prm.vlAt("value").at().setR(val,NULL,true);
    }
    else if( tp.find("in",0) == 0 )
    {
	if( fscanf(f,"%f %f %f",&min,&max,&val) == 3 )
    	    prm.vlAt("value").at().setR(val,NULL,true);
    }
    fclose(f);
}

void Lmsensors::chSub( )
{
    TCfg &c_subt = prm.cfg("SUBT");
    string sens =  c_subt.getSEL();
    sens = sens.substr(sens.find("/",0)+1);
    if( sens.find("temp",0) == 0 )
       	prm.vlAt("value").at().fld().descr() = prm.Owner().Owner().I18N("Temperature");
    else if( sens.find("fan",0) == 0 )
       	prm.vlAt("value").at().fld().descr() = prm.Owner().Owner().I18N("Fan turns");
    else if( sens.find("in",0) == 0 )
       	prm.vlAt("value").at().fld().descr() = prm.Owner().Owner().I18N("Voltage");
}

//======================================================================
//==== UpTime
//======================================================================
UpTime::UpTime( TMdPrm &mprm ) : TElem("uptime"), prm(mprm)
{
    st_tm = time(NULL);
    SFld valE[] =
    {
	{"value",prm.Owner().Owner().I18N("Full seconds"),T_DEC|F_NWR,"0","5"},
	{"sec"  ,prm.Owner().Owner().I18N("Seconds")     ,T_DEC|F_NWR,"0","2"},
	{"min"  ,prm.Owner().Owner().I18N("Minutes")     ,T_DEC|F_NWR,"0","2"},
	{"hour" ,prm.Owner().Owner().I18N("Hours")       ,T_DEC|F_NWR,"0","2"},
	{"day"  ,prm.Owner().Owner().I18N("Days")        ,T_DEC|F_NWR,"0"}
    };
    for( unsigned i_el = 0; i_el < sizeof(valE)/sizeof(SFld); i_el++ )
	elAdd(&valE[i_el]);
    prm.vlAttElem( this );
}

UpTime::~UpTime()
{
    prm.vlDetElem( this );
}

void UpTime::init()
{    
    //Create config
    TCfg &c_subt = prm.cfg("SUBT");
    c_subt.fld().descr() = "";
    c_subt.fld().val_i().clear();
    c_subt.fld().nSel().clear();
    
    c_subt.fld().val_i().push_back(0); c_subt.fld().nSel().push_back(prm.Owner().Owner().I18N("System"));
    c_subt.fld().val_i().push_back(1); c_subt.fld().nSel().push_back(prm.Owner().Owner().I18N("Station"));
    c_subt.setI(0);        
}

void UpTime::getVal(  )
{    
    long val;
    
    TCfg &c_subt = prm.cfg("SUBT");
    int trg = c_subt.getI();

    if( trg == 0 )
    {
	FILE *f = fopen("/proc/uptime","r");
	if( f == NULL ) return;
	fscanf(f,"%lu",&val);
	fclose(f);
    }
    else val = time(NULL) - st_tm;
    prm.vlAt("value").at().setI(val,NULL,true);
    prm.vlAt("day").at().setI(val/86400,NULL,true);
    prm.vlAt("hour").at().setI((val%86400)/3600,NULL,true);
    prm.vlAt("min").at().setI(((val%86400)%3600)/60,NULL,true);
    prm.vlAt("sec").at().setI(((val%86400)%3600)%60,NULL,true);    
}

//======================================================================
//==== CPU
//======================================================================
CPU::CPU( TMdPrm &mprm ) : TElem("cpu"), prm(mprm), mod(prm.Owner().Owner())
{    
    SFld valE[] =
    {
	{"value",mod.I18N("Load (%)")  ,T_REAL|F_NWR,"0","4.1"},
	{"sys"  ,mod.I18N("System (%)"),T_REAL|F_NWR,"0","4.1"},
	{"user" ,mod.I18N("User (%)")  ,T_REAL|F_NWR,"0","4.1"},
	{"idle" ,mod.I18N("Idle (%)")  ,T_REAL|F_NWR,"0","4.1"}
    };
    for( unsigned i_el = 0; i_el < sizeof(valE)/sizeof(SFld); i_el++ )
	elAdd(&valE[i_el]);
    prm.vlAttElem( this );
}

CPU::~CPU()
{    
    prm.vlDetElem( this );
}

void CPU::init()
{
    //Create config
    TCfg &t_cf = prm.cfg("SUBT");
    TFld &t_fl = t_cf.fld();
    t_fl.descr() = "";
    t_fl.val_i().clear();
    t_fl.nSel().clear();
    
    t_fl.val_i().push_back(0); t_fl.nSel().push_back(mod.I18N("General"));
    
    t_cf.setI(0);        
    //Init start value
    FILE *f = fopen("/proc/stat","r");
    if( f != NULL ) 
    {
	int  n_cpu;
	long iowait;
	int n = fscanf(f,"cpu %d %d %d %d %d\n",&gen.user,&gen.nice,&gen.sys,&gen.idle,&iowait);
	if( n == 5 ) gen.idle += iowait;
        while( fscanf(f,"cpu%d\n",&n_cpu) )
	{	    
	    cpu.insert(cpu.begin()+n_cpu,gen);
	    n = fscanf(f,"%d %d %d %d %d\n",&cpu[n_cpu].user,&cpu[n_cpu].nice,&cpu[n_cpu].sys,&cpu[n_cpu].idle,&iowait);
	    if( n == 6 ) cpu[n_cpu].idle += iowait;
	    t_fl.val_i().push_back(n_cpu+1);   
	    t_fl.nSel().push_back(TSYS::int2str(n_cpu));
	}	
	fclose(f);	
    }    
}

void CPU::getVal(  )
{    
    int n;
    long user,nice,sys,idle,iowait;
    float sum;
    
    int trg = prm.cfg("SUBT").getI();

    FILE *f = fopen("/proc/stat","r");
    if( f == NULL ) return;
    
    n = fscanf(f,"cpu %d %d %d %d %d\n",&user,&nice,&sys,&idle,&iowait);
    if( n == 5 ) idle += iowait;
    if( trg == 0 )
    {
	sum = (float)(user+nice+sys+idle-gen.user-gen.nice-gen.sys-gen.idle);
	prm.vlAt("value").at().setR( 100.0*(float(user+sys-gen.user-gen.sys))/sum,NULL,true);
	prm.vlAt("sys").at().setR( 100.0*(float(sys-gen.sys))/sum,NULL,true);
	prm.vlAt("user").at().setR( 100.0*(float(user-gen.user))/sum,NULL,true);
	prm.vlAt("idle").at().setR( 100.0*(float(idle-gen.idle))/sum,NULL,true);
	gen.user = user; 
	gen.nice = nice; 
	gen.sys  = sys; 
	gen.idle = idle;
    }
    else
    {
	trg--;
	string buf("cpu");
	buf=buf+TSYS::int2str(trg)+" %d %d %d %d %d\n";
	n = fscanf(f,buf.c_str(),&user,&nice,&sys,&idle,&iowait);
	if( n == 5 ) cpu[trg].idle += iowait;
	sum = (float)(user+nice+sys+idle-cpu[trg].user-cpu[trg].nice-cpu[trg].sys-cpu[trg].idle);
	prm.vlAt("value").at().setR( 100.0*(float(user+sys-cpu[trg].user-cpu[trg].sys))/sum,NULL,true);
	prm.vlAt("sys").at().setR( 100.0*(float(sys-cpu[trg].sys))/sum,NULL,true);
	prm.vlAt("user").at().setR( 100.0*(float(user-cpu[trg].user))/sum,NULL,true);
	prm.vlAt("idle").at().setR( 100.0*(float(idle-cpu[trg].idle))/sum,NULL,true);
	cpu[trg].user = user; 
	cpu[trg].nice = nice; 
	cpu[trg].sys  = sys; 
	cpu[trg].idle = idle;
    }
    fclose(f);    
}

//======================================================================
//==== Memory
//======================================================================
Mem::Mem( TMdPrm &mprm ) : TElem("mem"), prm(mprm), mod(prm.Owner().Owner())
{
    SFld valE[] =
    {
	{"value",mod.I18N("Free (kB)")   ,T_DEC|F_NWR,"0"},
	{"total",mod.I18N("Total (kB)")  ,T_DEC|F_NWR,"0"},
	{"used" ,mod.I18N("Used (kB)")   ,T_DEC|F_NWR,"0"},
	{"buff" ,mod.I18N("Buffers (kB)"),T_DEC|F_NWR,"0"},
	{"cache",mod.I18N("Cached (kB)") ,T_DEC|F_NWR,"0"}
    };
    for( unsigned i_el = 0; i_el < sizeof(valE)/sizeof(SFld); i_el++ )
	elAdd(&valE[i_el]);
    prm.vlAttElem( this );
}

Mem::~Mem()
{
    prm.vlDetElem( this );
}

void Mem::init()
{
    //Create config
    TCfg &t_cf = prm.cfg("SUBT");
    TFld &t_fl = t_cf.fld();
    t_fl.descr() = "";
    t_fl.val_i().clear();
    t_fl.nSel().clear();
    
    t_fl.val_i().push_back(0); t_fl.nSel().push_back(mod.I18N("All"));
    t_fl.val_i().push_back(1); t_fl.nSel().push_back(mod.I18N("Phisical"));
    t_fl.val_i().push_back(2); t_fl.nSel().push_back(mod.I18N("Swap"));
    t_cf.setI(0);        
}

void Mem::getVal(  )
{    
    int n = 0;
    long m_total, m_used, m_free, m_shar, m_buff, m_cach;
    long sw_total, sw_used, sw_free;
    char buf[256];
    
    int trg = prm.cfg("SUBT").getI();

    FILE *f = fopen("/proc/meminfo","r");
    if( f == NULL ) return;
    
    while( fgets(buf,sizeof(buf),f) != NULL )
    {
	if( sscanf(buf,"Mem: %d %d %d %d %d %d\n",&m_total,&m_used,&m_free,&m_shar,&m_buff,&m_cach) ) n++;
	if( sscanf(buf,"Swap: %d %d %d\n",&sw_total,&sw_used,&sw_free) ) n++;
	if( n >= 2 ) break;
    }
    fclose(f);
    
    if( trg == 0 )
    {
	prm.vlAt("value").at().setI((m_free+m_buff+m_cach+sw_free)/1024,NULL,true);
	prm.vlAt("total").at().setI((m_total+sw_total)/1024,NULL,true);
	prm.vlAt("used").at().setI((m_used-m_buff-m_cach+sw_used)/1024,NULL,true);
	prm.vlAt("buff").at().setI(m_buff/1024,NULL,true);
	prm.vlAt("cache").at().setI(m_cach/1024,NULL,true);
    }
    else if( trg == 1 )
    {
	prm.vlAt("value").at().setI((m_free+m_buff+m_cach)/1024,NULL,true);
	prm.vlAt("total").at().setI(m_total/1024,NULL,true);
	prm.vlAt("used").at().setI((m_used-m_buff-m_cach)/1024,NULL,true);
	prm.vlAt("buff").at().setI(m_buff/1024,NULL,true);
	prm.vlAt("cache").at().setI(m_cach/1024,NULL,true);
    }
    else if( trg == 2 ) 	
    {
	prm.vlAt("value").at().setI(sw_free/1024,NULL,true);
	prm.vlAt("total").at().setI(sw_total/1024,NULL,true);
	prm.vlAt("used").at().setI(sw_used/1024,NULL,true);
    }
}

void Mem::chSub()
{
    int id;
    if( prm.cfg("SUBT").getI() == 2 )
    {
	try{ id = elNameId("buff"); } catch(...) { return; }
	elDel(elNameId("buff"));
	elDel(elNameId("cache"));
    }
    else
    {
	try{ id = elNameId("buff"); } 
	catch(...) 
	{ 
	    SFld valE[] =
	    {
		{"buff" ,mod.I18N("Buffers (kB)"),T_DEC|F_NWR,"0"},
		{"cache",mod.I18N("Cached (kB)") ,T_DEC|F_NWR,"0"}
	    };
	    for( unsigned i_el = 0; i_el < sizeof(valE)/sizeof(SFld); i_el++ )
		elAdd(&valE[i_el]);
	}
    }    
}

