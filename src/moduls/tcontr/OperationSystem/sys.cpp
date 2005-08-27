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

#include <sys/times.h>
#include <sys/time.h>
#include <getopt.h>
#include <unistd.h>
#include <dirent.h>
#include <signal.h>
#include <fcntl.h>

#include <terror.h>
#include <tsys.h>
#include <resalloc.h>
#include <tmess.h>
#include <ttransports.h>
#include <ttiparam.h>
#include <tcontrollers.h>
#include "sys.h"

//============ Modul info! =====================================================
#define MOD_ID      "OperationSystem"
#define MOD_NAME    "Operation system DA"
#define MOD_TYPE    "Controller"
#define VER_TYPE    VER_CNTR
#define VERSION     "0.6.0"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "Allow operation system data acquisition. Support OS Linux data sources: HDDTemp, LMSensors, Uptime, Memory, CPU"
#define LICENSE     "GPL"
//==============================================================================

SystemCntr::TTpContr *SystemCntr::mod;  //Pointer for direct access to module

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
	SystemCntr::TTpContr *self_addr = NULL;

    	if( AtMod.id == MOD_ID && AtMod.type == MOD_TYPE && AtMod.t_ver == VER_TYPE )
	    self_addr = SystemCntr::mod = new SystemCntr::TTpContr( source );

	return ( self_addr );
    }
}

using namespace SystemCntr;

//======================================================================
//==== TTpContr ======================================================== 
//======================================================================
TTpContr::TTpContr( string name )  
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

TTpContr::~TTpContr()
{    

}

string TTpContr::optDescr( )
{
    char buf[STR_BUF_LEN];

    snprintf(buf,sizeof(buf),I18N(
	"======================= The module <%s:%s> options =======================\n"
	"---------- Parameters of the module section <%s> in config file ----------\n\n"),
	MOD_TYPE,MOD_ID,nodePath().c_str());
    
    return(buf);
}

void TTpContr::modLoad( )
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

void TTpContr::modConnect( )
{    
    TModule::modConnect( );
    //==== Controler's bd structure ====    
    fldAdd( new TFld("PRM_BD",I18N("System parameteres table"),TFld::String,0,"30","system") );
    fldAdd( new TFld("PERIOD",I18N("The request period (ms)"),TFld::Dec,0,"5","1000","0;10000") );
    //==== Parameter type bd structure ====
    int t_prm = tpParmAdd("All","PRM_BD",I18N("All parameters"));
    tpPrmAt(t_prm).fldAdd( new TFld("TYPE",I18N("System part"),TFld::Dec,FLD_SELECT|FLD_NOVAL|FLD_PREV,"2","0","3;4;2;0;1",
	(I18Ns("CPU")+";"+I18Ns("Memory")+";"+I18Ns("Up time")+";"+I18Ns("Hdd temperature")+";"+I18Ns("Sensors")).c_str() ) );
    tpPrmAt(t_prm).fldAdd( new TFld("SUBT" ,"",TFld::Dec,FLD_SELECT|FLD_NOVAL|FLD_PREV|FLD_SELF,"2") );
}

TController *TTpContr::ContrAttach( const string &name, const TBDS::SName &bd)
{
    return( new TMdContr(name,bd,this,this));    
}

//======================================================================
//==== TMdContr 
//======================================================================

TMdContr::TMdContr( string name_c, const TBDS::SName &bd, ::TTipController *tcntr, ::TElem *cfgelem) :
	::TController(name_c,bd,tcntr,cfgelem), endrun(false), period(cfg("PERIOD").getId())
{    
    en_res = ResAlloc::resCreate();
}

TMdContr::~TMdContr()
{
    if( run_st ) stop();
    ResAlloc::resDelete(en_res);
}

TParamContr *TMdContr::ParamAttach( const string &name, int type )
{
    return(new TMdPrm(name,&owner().tpPrmAt(type)));
}

void TMdContr::load_( )
{

}

void TMdContr::save_( )
{

}

void TMdContr::start_( )
{      
    pthread_attr_t      pthr_attr;
    struct sched_param  prior;
    vector<string> 	list_p;

    if( !run_st )
    {
	//---- Enable parameters ----
	list(list_p);	
	for(unsigned i_prm=0; i_prm < list_p.size(); i_prm++)
	    if( at(list_p[i_prm]).at().toEnable() )
		at(list_p[i_prm]).at().enable();
	//    p_hd.push_back( at(list_p[i_prm],name()+"_start") );
	//---- Start process task ----
	pthread_attr_init(&pthr_attr);
	pthread_attr_setschedpolicy(&pthr_attr,SCHED_OTHER);
	pthread_create(&pthr_tsk,&pthr_attr,Task,this);
	pthread_attr_destroy(&pthr_attr);
	if( TSYS::eventWait( run_st, true,nodePath()+"start",5) )
	    throw TError(nodePath().c_str(),"Controller no started!");
    }    
}

void TMdContr::stop_( )
{  
    vector<string> 	list_p;

    if( run_st )
    {
	//---- Stop process task ----
	endrun = true;
	pthread_kill(pthr_tsk, SIGALRM);
	if( TSYS::eventWait( run_st, false, nodePath()+"stop",5) )
	    throw TError(nodePath().c_str(),"Controller no stoped!");
	pthread_join(pthr_tsk, NULL);
	
	//---- Disable params ----
	list(list_p);
        for(unsigned i_prm=0; i_prm < list_p.size(); i_prm++)
            if( at(list_p[i_prm]).at().enableStat() )
                at(list_p[i_prm]).at().disable();
    }
} 

void TMdContr::prmEn( const string &id, bool val )
{
    int i_prm;

    ResAlloc res(en_res,true);
    for( i_prm = 0; i_prm < p_hd.size(); i_prm++)
        if( p_hd[i_prm].at().name() == id ) break;
    
    if( val && i_prm >= p_hd.size() )
        p_hd.push_back(at(id));
    if( !val && i_prm < p_hd.size() )
        p_hd.erase(p_hd.begin()+i_prm);
}

void *TMdContr::Task(void *contr)
{
    struct itimerval mytim;             //Interval timer
    TMdContr *cntr = (TMdContr *)contr;

#if OSC_DEBUG
    Mess->put(cntr->nodePath().c_str(),TMess::Debug,Mess->I18N("Thread <%d> started!"),getpid() );
#endif

    if(cntr->period == 0) return(NULL);
    mytim.it_interval.tv_sec = 0; mytim.it_interval.tv_usec = cntr->period*1000;
    mytim.it_value.tv_sec    = 0; mytim.it_value.tv_usec    = cntr->period*1000;

    signal(SIGALRM,wakeup);
    setitimer(ITIMER_REAL,&mytim,NULL);    
    
    cntr->run_st = true;  cntr->endrun = false;
    while( !cntr->endrun )
    {
	ResAlloc::resRequestR(cntr->en_res);
	for(unsigned i_p=0; i_p < cntr->p_hd.size(); i_p++)
	    cntr->p_hd[i_p].at().getVal();
	ResAlloc::resReleaseR(cntr->en_res);
	    
	pause();
    }
    cntr->run_st = false;

    return(NULL);    
}

//======================================================================
//==== TMdPrm 
//======================================================================

TMdPrm::TMdPrm( string name, TTipParam *tp_prm ) : 
    TParamContr(name,tp_prm), m_type(PRM_NONE)
{
    cfg("TYPE").setSEL(mod->I18N("CPU"));
}

TMdPrm::~TMdPrm( )
{    

}

void TMdPrm::enable()
{
    if( enableStat() )	return;    
    cfgChange(cfg("TYPE"));
    TParamContr::enable();
    ((TMdContr&)owner()).prmEn( name(), true );	//Put to process
}

void TMdPrm::disable()
{
    if( !enableStat() )  return;

    ((TMdContr&)owner()).prmEn( name(), false );      //Remove from process 
    setType( PRM_NONE );
    TParamContr::disable();
}

void TMdPrm::preDisable( int flag )
{
    setType( PRM_NONE );
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

void TMdPrm::setType( char tp )
{
    if( m_type == tp ) return;
        
    //Free previous type
    if( m_type == PRM_CPU )         delete prm.p_cpu;
    else if( m_type == PRM_MEM )    delete prm.p_mem;
    else if( m_type == PRM_UPTIME ) delete prm.p_upt;
    else if( m_type == PRM_HDDT )   delete prm.p_hdd;
    else if( m_type == PRM_SENSOR ) delete prm.p_sens;
    m_type = PRM_NONE;			    

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

bool TMdPrm::cfgChange( TCfg &i_cfg )
{        
    //Change TYPE parameter
    if( i_cfg.name() == "TYPE" )
    {
	if( i_cfg.getSEL() == mod->I18N("CPU") ) 		setType( PRM_CPU );
	else if( i_cfg.getSEL() == mod->I18N("Memory") ) 	setType( PRM_MEM );
	else if( i_cfg.getSEL() == mod->I18N("Up time") )	setType( PRM_UPTIME );
	else if( i_cfg.getSEL() == mod->I18N("Hdd temperature") ) 	setType( PRM_HDDT );
	else if( i_cfg.getSEL() == mod->I18N("Sensors") )    	setType( PRM_SENSOR );
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
    prm(mprm), t_tr("socket"),n_tr("tr_"+mprm.name()), c_subt(prm.cfg("SUBT")), err_st(false)
{
    tr = mprm.owner().owner().owner().owner().transport();
    ((TTipTransport &)tr.at().modAt(t_tr).at()).outAdd(n_tr);
    otr = new AutoHD<TTransportOut>;
    *otr = (((TTipTransport &)tr.at().modAt(t_tr).at()).outAt(n_tr));
    
    otr->at().lName() = mod->I18N("Parametr Hddtemp");
    otr->at().address() = "TCP:127.0.0.1:7634";
    otr->at().start();
    
    //HDD value structure
    fldAdd( new TFld("disk",mod->I18N("Name"),TFld::String,FLD_NWR) );
    fldAdd( new TFld("ed",mod->I18N("Measure unit"),TFld::String,FLD_NWR) );
    fldAdd( new TFld("value",mod->I18N("Temperature"),TFld::Dec,FLD_NWR,"3","0") );    
    
    prm.vlAttElem( this );
    // Make direct access
    atrb.push_back(prm.vlAt("disk"));
    atrb.push_back(prm.vlAt("ed"));
    atrb.push_back(prm.vlAt("value"));
}

Hddtemp::~Hddtemp()
{
    delete otr;    
    ((TTipTransport &)tr.at().modAt(t_tr).at()).outDel(n_tr);
    atrb.clear();
    prm.vlDetElem( this );
}

void Hddtemp::init()
{
    //Create Config
    c_subt.fld().descr() = mod->I18N("Disk");
    c_subt.fld().selValI().clear();
    c_subt.fld().selNm().clear();    

    vector<string> list;
    dList(list);
    for( int i_l = 0; i_l < list.size(); i_l++ )
    {
	c_subt.fld().selValI().push_back(i_l);
	c_subt.fld().selNm().push_back(list[i_l]);
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
	len = otr->at().messIO("1",1,buf,sizeof(buf),1); buf[len] = '\0';	
	err_st = false;	
	val.append(buf,len);
	while( len == sizeof(buf) )
	{
	    len = otr->at().messIO(NULL,0,buf,sizeof(buf),1); buf[len] = '\0';
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
	if( !err_st ) Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); 
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
	
	len = otr->at().messIO("1",1,buf,sizeof(buf),1);
	err_st = false;
	val.append(buf,len);
	while( len == sizeof(buf) )
	{
	    len = otr->at().messIO(NULL,0,buf,sizeof(buf),1);
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
	if( !err_st ) Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str());
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
    //Sensors value structure
    fldAdd( new TFld("value","",TFld::Real,FLD_NWR,"8.2","0") );
    
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
    c_subt.fld().descr() = mod->I18N("Sensor");
    c_subt.fld().selValI().clear();
    c_subt.fld().selNm().clear();
    
    vector<string> list;
    dList(list);
    for( int i_l = 0; i_l < list.size(); i_l++ )
    {
	c_subt.fld().selValI().push_back(i_l);
	c_subt.fld().selNm().push_back(list[i_l]);
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
    if( sens.find("temp",0) == 0 )	prm.vlAt("value").at().fld().descr() = mod->I18N("Temperature");
    else if( sens.find("fan",0) == 0 ) 	prm.vlAt("value").at().fld().descr() = mod->I18N("Fan turns");
    else if( sens.find("in",0) == 0 )  	prm.vlAt("value").at().fld().descr() = mod->I18N("Voltage");
}

//======================================================================
//==== UpTime
//======================================================================
UpTime::UpTime( TMdPrm &mprm ) : TElem("uptime"), prm(mprm)
{
    st_tm = time(NULL);
    
    //Uptime value structure
    fldAdd( new TFld("value",mod->I18N("Full seconds"),TFld::Dec,FLD_NWR,"","0") );
    fldAdd( new TFld("sec",mod->I18N("Seconds"),TFld::Dec,FLD_NWR,"2","0") );
    fldAdd( new TFld("min",mod->I18N("Minutes"),TFld::Dec,FLD_NWR,"2","0") );
    fldAdd( new TFld("hour",mod->I18N("Hours"),TFld::Dec,FLD_NWR,"2","0") );
    fldAdd( new TFld("day",mod->I18N("Days"),TFld::Dec,FLD_NWR,"","0") );   
    
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
    c_subt.fld().selValI().clear();
    c_subt.fld().selNm().clear();
    
    c_subt.fld().selValI().push_back(0); c_subt.fld().selNm().push_back(mod->I18N("System"));
    c_subt.fld().selValI().push_back(1); c_subt.fld().selNm().push_back(mod->I18N("Station"));
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
CPU::CPU( TMdPrm &mprm ) : TElem("cpu"), prm(mprm)
{   
    //CPU value structure
    fldAdd( new TFld("value",mod->I18N("Load (%)"),TFld::Real,FLD_NWR,"4.1","0") );
    fldAdd( new TFld("sys",mod->I18N("System (%)"),TFld::Real,FLD_NWR,"4.1","0") );
    fldAdd( new TFld("user",mod->I18N("User (%)"),TFld::Real,FLD_NWR,"4.1","0") );
    fldAdd( new TFld("idle",mod->I18N("Idle (%)"),TFld::Real,FLD_NWR,"4.1","0") );
    
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
    t_fl.selValI().clear();
    t_fl.selNm().clear();
    
    t_fl.selValI().push_back(-1); t_fl.selNm().push_back(mod->I18N("OpenSCADA"));
    t_fl.selValI().push_back(0); t_fl.selNm().push_back(mod->I18N("General"));
    
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
	    t_fl.selValI().push_back(n_cpu+1);   
	    t_fl.selNm().push_back(TSYS::int2str(n_cpu));
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
    
    if( trg == -1 )
    {
	struct tms p_tm;
	clock_t cur_tm = times(&p_tm);
	sum  = cur_tm - gen.nice;
	user = p_tm.tms_utime + p_tm.tms_cutime - gen.user;
	sys  = p_tm.tms_stime + p_tm.tms_cstime - gen.sys;
	
       	prm.vlAt("value").at().setR( 100.0*(float(user + sys))/sum,NULL,true);       
       	prm.vlAt("sys").at().setR( 100.0*(float(sys))/sum,NULL,true);       
       	prm.vlAt("user").at().setR( 100.0*(float(user))/sum,NULL,true);       
       	prm.vlAt("idle").at().setR( 100.0*(float(sum - user - sys))/sum,NULL,true);       
	
	gen.nice = cur_tm;
	gen.user = p_tm.tms_utime + p_tm.tms_cutime;
	gen.sys  = p_tm.tms_stime + p_tm.tms_cstime;
	
	return;
    }

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
Mem::Mem( TMdPrm &mprm ) : TElem("mem"), prm(mprm)
{
    //Memory value structure
    fldAdd( new TFld("value",mod->I18N("Free (kB)"),TFld::Dec,FLD_NWR,"","0") );
    fldAdd( new TFld("total",mod->I18N("Total (kB)"),TFld::Dec,FLD_NWR,"","0") );
    fldAdd( new TFld("used",mod->I18N("Used (kB)"),TFld::Dec,FLD_NWR,"","0") );
    fldAdd( new TFld("buff",mod->I18N("Buffers (kB)"),TFld::Dec,FLD_NWR,"","0") );
    fldAdd( new TFld("cache",mod->I18N("Cached (kB)"),TFld::Dec,FLD_NWR,"","0") );
    
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
    t_fl.selValI().clear();
    t_fl.selNm().clear();
    
    t_fl.selValI().push_back(0); t_fl.selNm().push_back(mod->I18N("All"));
    t_fl.selValI().push_back(1); t_fl.selNm().push_back(mod->I18N("Phisical"));
    t_fl.selValI().push_back(2); t_fl.selNm().push_back(mod->I18N("Swap"));
    t_cf.setI(0);        
}

void Mem::getVal(  )
{    
    int n = 0;
    long m_total, m_free, m_buff, m_cach;
    long sw_total, sw_free;
    char buf[256];
    
    int trg = prm.cfg("SUBT").getI();

    FILE *f = fopen("/proc/meminfo","r");
    if( f == NULL ) return;
    
    while( fgets(buf,sizeof(buf),f) != NULL )
    {
	if( sscanf(buf,"MemTotal: %d kB\n",&m_total) )	n++;
	if( sscanf(buf,"MemFree: %d kB\n",&m_free) ) 	n++;
	if( sscanf(buf,"Buffers: %d kB\n",&m_buff) ) 	n++;
	if( sscanf(buf,"Cached: %d kB\n",&m_cach) ) 	n++;
	if( sscanf(buf,"SwapTotal: %d kB\n",&sw_total) ) n++;
	if( sscanf(buf,"SwapFree: %d kB\n",&sw_free) ) 	n++;
	if( n >= 6 ) break;
    
	//if( sscanf(buf,"Mem: %d %d %d %d %d %d\n",&m_total,&m_used,&m_free,&m_shar,&m_buff,&m_cach) ) n++;
	//if( sscanf(buf,"Swap: %d %d %d\n",&sw_total,&sw_used,&sw_free) ) n++;
	//if( n >= 2 ) break;
    }
    fclose(f);
    
    if( trg == 0 )
    {
	prm.vlAt("value").at().setI(m_free+m_buff+m_cach+sw_free,NULL,true);
	prm.vlAt("total").at().setI(m_total+sw_total,NULL,true);
	prm.vlAt("used").at().setI(m_total-m_free-m_buff-m_cach+sw_total-sw_free,NULL,true);
	prm.vlAt("buff").at().setI(m_buff,NULL,true);
	prm.vlAt("cache").at().setI(m_cach,NULL,true);
    }
    else if( trg == 1 )
    {
	prm.vlAt("value").at().setI(m_free+m_buff+m_cach,NULL,true);
	prm.vlAt("total").at().setI(m_total,NULL,true);
	prm.vlAt("used").at().setI(m_total-m_free-m_buff-m_cach,NULL,true);
	prm.vlAt("buff").at().setI(m_buff,NULL,true);
	prm.vlAt("cache").at().setI(m_cach,NULL,true);
    }
    else if( trg == 2 ) 	
    {
	prm.vlAt("value").at().setI(sw_free,NULL,true);
	prm.vlAt("total").at().setI(sw_total,NULL,true);
	prm.vlAt("used").at().setI(sw_total-sw_free,NULL,true);
    }
}

void Mem::chSub()
{
    int id;
    if( prm.cfg("SUBT").getI() == 2 )
    {
	try{ id = fldId("buff"); } catch(...) { return; }
	fldDel(fldId("buff"));
	fldDel(fldId("cache"));
    }
    else
    {
	try{ id = fldId("buff"); } 
	catch(...) 
	{ 
	    fldAdd( new TFld("buff",mod->I18N("Buffers (kB)"),TFld::Dec,FLD_NWR,"","0") );
	    fldAdd( new TFld("cache",mod->I18N("Cached (kB)"),TFld::Dec,FLD_NWR,"","0") );
	}
    }    
}

