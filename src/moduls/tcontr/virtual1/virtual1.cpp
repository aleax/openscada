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
#include <unistd.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <math.h>

#include <terror.h>
#include <tsys.h>
#include <tkernel.h>
#include <tmessage.h>
#include <tconfig.h>
#include <tvalue.h>
#include <tparam.h>
#include <tparams.h>
#include "virtual1.h"

//============ Modul info! =====================================================
#define NAME_MODUL  "virtual_v1"
#define NAME_TYPE   "Controller"
#define VER_TYPE    VER_CNTR
#define VERSION     "0.0.9"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "Virtual controller V1.x (from Complex2) - may be used how internal controller or instrument for GUI"
#define LICENSE     "GPL"
//==============================================================================

//Present controller parameter
#define PRM_ANALOG "ANALOG"
#define PRM_B_AN   "PRM_BD1"

#define PRM_DIGIT  "DIGIT"
#define PRM_B_DG   "PRM_BD2"

#define PRM_BLOCK  "BLOCK"
#define PRM_B_BLCK "PRM_BD3"

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
	Virtual1::TVirtual *self_addr = NULL;

	if( AtMod.name == NAME_MODUL && AtMod.type == NAME_TYPE && AtMod.t_ver == VER_TYPE )
	    self_addr = new Virtual1::TVirtual( source );

	return ( self_addr );
    }
}

using namespace Virtual1;

//======================================================================
//==== TVirtual ======================================================== 
//======================================================================


//=============================================
//==== Describe ANALOG param struct ===========
SFld TVirtual::ValAN[] =
{
    {"VAL","Value analog parameter" ,T_REAL,"","10.2"}
};

//==== Describe DIGIT param struct ===========
SFld TVirtual::ValDG[] =
{
    {"VAL","Value digital parameter",T_BOOL,"","10.2"}
};

//==== PID regulator ===========

SFld TVirtual::ValPID[] =
{
    {"OUT"   ,"Output of regulator"                ,T_REAL        ,"","10.2","0;100"},
    {"SP"    ,"Setpoint of regulator"              ,T_REAL        ,"","10.2"},
    {"STAT"  ,"Stat regulator (Manual,Auto,Casc)"  ,T_DEC|T_SELECT,"","1"   ,"0;1;2","Manual;Auto;Cascad"},
    {"Kp"    ,"Koefficient of proportion"          ,T_REAL        ,"","6.2" ,"-20;20"},
    {"Ti"    ,"Time of integrated (sek)"           ,T_REAL        ,"","7.2" ,"0;1000"},
    {"Td"    ,"Time of diff (sek)"                 ,T_REAL        ,"","7.2" ,"0;1000"},
    {"Tf"    ,"Time of lag (sek)"                  ,T_REAL        ,"","7.2" ,"0;1000"},
    {"H_UP"  ,"Up out border (%)"                  ,T_REAL        ,"","6.2" ,"0;100"},
    {"H_DOWN","Down out border (%)"                ,T_REAL        ,"","6.2" ,"0;100"},
    {"ZN"    ,"Non-sensitive pid error (%)"        ,T_REAL        ,"","5.2" ,"0;20"},
    {"K1"    ,"Koefficient scale of addon input 1" ,T_REAL        ,"","6.2" ,"-20;20"},
    {"K2"    ,"Koefficient scale of addon input 2" ,T_REAL        ,"","6.2" ,"-20;20"},
    {"K3"    ,"Koefficient scale of addon input 3" ,T_REAL        ,"","6.2" ,"-20;20"},
    {"K4"    ,"Koefficient scale of addon input 4" ,T_REAL        ,"","6.2" ,"-20;20"}
};

TVirtual::TVirtual( string name ) : NameCfgF("./alg.cfg"), algbs(NULL)
{
    NameModul = NAME_MODUL;
    NameType  = NAME_TYPE;
    Vers      = VERSION;
    Autors    = AUTORS;
    DescrMod  = DESCRIPTION;
    License   = LICENSE;
    Source    = name;    
}

TVirtual::~TVirtual()
{    
    if(algbs) delete algbs;
}

void TVirtual::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    	"======================= %s module options ========================\n"
	"    --Vrt1CFG=<path>   Set config file name (default ./alg.cfg)\n"
	"--------------------- Module parameters of config file -------------------\n"
	"config <path>          path to config file;\n"
	"\n",NAME_MODUL);
}

void TVirtual::mod_CheckCommandLine( )
{
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{"Vrt1CFG"   ,1,NULL,'c'},
	{NULL        ,0,NULL,0  }
    };

    optind=opterr=0;
    do
    {
	next_opt=getopt_long(SYS->argc,(char * const *)SYS->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': pr_opt_descr(stdout); break;
	    case 'c': NameCfgF = optarg;    break;
	    case -1 : break;
	}
    } while(next_opt != -1);
}

void TVirtual::mod_UpdateOpt( )
{
    try{ NameCfgF = mod_XMLCfgNode()->get_child("id","config")->get_text(); }
    catch(...) {  }
}

void TVirtual::mod_connect( )
{    
    TModule::mod_connect( );
    
    //==== Desribe controler's bd fields ====
    SFld elem[] =         
    {    
	{PRM_B_AN  ,I18N("Name of ANALOG parameteres table")     ,T_STRING,"VRT_AN","30"},
	{PRM_B_DG  ,I18N("Name of DIGIT parameteres table")      ,T_STRING,"VRT_DG","30"},
	{PRM_B_BLCK,I18N("Name of BLOCK parameteres table")      ,T_STRING,"VRT_BL","30"},
	{"PERIOD"  ,I18N("The calc period (ms)")                 ,T_DEC   ,"1000"  ,"5" ,"0;10000"},
	{"ITER"    ,I18N("The iteration number into calc period"),T_DEC   ,"1"     ,"2" ,"0;99"   },
	{"PER_S"   ,I18N("The sync period (ms)")                 ,T_DEC   ,"1000"  ,"5" ,"0;10000"}
    };
    //==== Desribe ANALOG parameter's bd fields ====
    SFld ElemAN[] =         
    {
	{"ED"     ,I18N("Value of measurement")          ,T_STRING         ,""    ,"10"  },
	{"SCALE"  ,I18N("Scale")                         ,T_DEC|T_SELECT   ,"0"   ,"1"   ,"0;1"           ,I18N("Linear;Square")},
	{"TIPO"   ,I18N("Type of processing")            ,T_DEC|T_SELECT   ,"0"   ,"1"   ,"0;1;2"         ,I18N("Average;Integrate;Counter")},
	{"MIN"    ,I18N("Lower scale border")            ,T_REAL           ,"0"   ,"10.2"},
	{"MAX"    ,I18N("Upper scale border")            ,T_REAL           ,"100" ,"10.2"},
	{"NTG"    ,I18N("Lower technically scale border"),T_REAL           ,"0"   ,"10.2"},
	{"VTG"    ,I18N("Upper technically scale border"),T_REAL           ,"0"   ,"10.2"},
	{"NAG"    ,I18N("Lower alarm scale border")      ,T_REAL           ,"0"   ,"10.2"},
	{"VAG"    ,I18N("Upper alarm scale border")      ,T_REAL           ,"0"   ,"10.2"},
	{"Z_GR"   ,I18N("Non-sensitive zone (%)")        ,T_REAL           ,"0.5" ,"4.1" ,"0;50"          },
	{"TYPE"   ,I18N("The parameter value type")      ,T_STRING|T_SELECT,"A_IN","5"   ,"A_IN;A_OUT;PID",I18N("Input;Output;PID")}
    };
    //==== Desribe DIGIT parameter's bd fields ====
    SFld ElemDG[] =
    {
	{"TYPE"   ,I18N("Parameter type"),T_STRING|T_SELECT,"D_IN","5","D_IN;D_OUT",I18N("Input;Output")}
    };
    //==== Desribe BLOCK parameter's bd fields ====
    SFld ElemBL[] =
    {
    };

    LoadCfg(elem,sizeof(elem)/sizeof(SFld));
    //Add parameter types
    LoadTpParmCfg(AddTpParm(PRM_ANALOG,PRM_B_AN  ,I18N("Analog parameter"))           ,ElemAN,sizeof(ElemAN)/sizeof(SFld));
    LoadTpParmCfg(AddTpParm(PRM_DIGIT ,PRM_B_DG  ,I18N("Digital parameter"))          ,ElemDG,sizeof(ElemDG)/sizeof(SFld));
    LoadTpParmCfg(AddTpParm(PRM_BLOCK ,PRM_B_BLCK,I18N("Block parameter (algoblock)")),ElemBL,sizeof(ElemBL)/sizeof(SFld));
    //Add types of value
    AddTpVal("A_IN",ValAN ,sizeof(ValAN)/sizeof(SFld));
    AddTpVal("D_IN",ValDG ,sizeof(ValDG)/sizeof(SFld));
    AddTpVal("PID" ,ValAN ,sizeof(ValAN)/sizeof(SFld));    
    AddTpVal("PID" ,ValPID,sizeof(ValPID)/sizeof(SFld));
    //Load algobloks
    algbs = new TVirtAlgb(NameCfgF);
}

TController *TVirtual::ContrAttach(string name, SBDS bd)
{
    return( new TVContr(name,bd,this,this));    
}

//======================================================================
//==== TVContr 
//======================================================================

TVContr::TVContr(  string name_c, SBDS bd, ::TTipController *tcntr, ::TElem *cfgelem) :
    ::TController(name_c, bd, tcntr, cfgelem), endrun(false),
    period(cfg("PERIOD").getI()), per_sync(cfg("PER_S").getI()), iterate(cfg("ITER").getI())    
{

}

TVContr::~TVContr()
{
    if( run_st ) Stop();
}

void TVContr::Load_( )
{

}

void TVContr::Save_( )
{

}

void TVContr::Start_( )
{   
    pthread_attr_t      pthr_attr;
    struct sched_param  prior;
    //---- Attach parameter algoblock ----
    vector<string> list_p;
    
    if( !run_st ) 
    {    
    	list(list_p);
	for(unsigned i_prm=0; i_prm < list_p.size(); i_prm++)
	{
	    int hd = att(list_p[i_prm],Name()+"_start");
	    ( (TVPrm &)at( hd ) ).Load();
	    ( (TVPrm &)at( hd ) ).Enable();
	    p_hd.push_back(hd);
	}
	//------------------------------------    
	pthread_attr_init(&pthr_attr);
	if(SYS->UserName() == "root")
	{
	    prior.__sched_priority=10;
	    pthread_attr_setschedpolicy(&pthr_attr,SCHED_FIFO);
	    pthread_attr_setschedparam(&pthr_attr,&prior);
	    
	    Owner().m_put("SYS",MESS_DEBUG,"%s:Start into realtime mode!",Name().c_str());
	}
	else pthread_attr_setschedpolicy(&pthr_attr,SCHED_OTHER);
	pthread_create(&pthr_tsk,&pthr_attr,Task,this);
	pthread_attr_destroy(&pthr_attr);
	if( SYS->event_wait( run_st, true, string(NAME_MODUL)+": Controller "+Name()+" is starting....",5) )
	    throw TError("%s: Controller %s no started!",NAME_MODUL,Name().c_str());    	    
    }	
}

void TVContr::Stop_( )
{  
    if( run_st )
    {
	endrun = true;
	pthread_kill(pthr_tsk, SIGALRM);
    	if( SYS->event_wait( run_st, false, string(NAME_MODUL)+": Controller "+Name()+" is stoping....",5) )
    	    throw TError("%s: Controller %s no stoped!",NAME_MODUL,Name().c_str());
	pthread_join(pthr_tsk, NULL);
	
	for(unsigned i_prm=0; i_prm < p_hd.size(); i_prm++)
	{
	    ( (TVPrm &)at( p_hd[i_prm] ) ).Disable();
	    det( p_hd[i_prm] );
	}
	p_hd.clear();
	for(unsigned i_prm=0; i_prm < p_io_hd.size(); i_prm++)
	{
	    if( p_io_hd[i_prm]->internal ) det( p_io_hd[i_prm]->hd_prm );
	    else Owner().Owner().Owner().Param().det( p_io_hd[i_prm]->hd_prm );
	    delete p_io_hd[i_prm];
	}
	p_io_hd.clear();    
    }
} 

void *TVContr::Task(void *contr)
{
    int    i_sync=0;
    
    struct itimerval mytim;             //Interval timer
    long   time_t1,time_t2,cnt_lost=0;
    int    frq = sysconf(_SC_CLK_TCK);  //Count of system timer n/sek
    TVContr *cntr = (TVContr *)contr;

#if OSC_DEBUG
    cntr->Owner().m_put("DEBUG",MESS_DEBUG,"%s: Thread <%d>!",cntr->Name().c_str(),getpid() );
#endif	

    try
    {
	if(cntr->period == 0) return(NULL);
	cntr->d_sync = cntr->per_sync/cntr->period;
	if(cntr->d_sync == 0)
	{ 
	    cntr->d_sync = 1;
	    cntr->per_sync = cntr->d_sync*cntr->period;
	}
	if(cntr->iterate <= 0) cntr->iterate = 1; 

	mytim.it_interval.tv_sec = 0; mytim.it_interval.tv_usec = cntr->period*1000;
	mytim.it_value.tv_sec    = 0; mytim.it_value.tv_usec    = cntr->period*1000;
    
	signal(SIGALRM,wakeup);
	setitimer(ITIMER_REAL,&mytim,NULL);
    
	cntr->run_st = true;  cntr->endrun = false;
	time_t1=times(NULL);    
    
	while( !cntr->endrun )
	{
	    pause();
	    /*
#ifdef OSC_DEBUG
	    //check hard cycle
	    time_t2 = times(0);
	    if( time_t2 != (time_t1+cntr->period*frq/1000) )
	    {
		cnt_lost+=time_t2-(time_t1+cntr->period*frq/1000);
		cntr->Owner().m_put("DEBUG",MESS_DEBUG,"%s:Lost ticks %d - %d (%d)",cntr->Name().c_str(),time_t2,time_t1+cntr->period*frq/1000,cnt_lost);
	    }
	    time_t1 = time_t2;	
	    //----------------
#endif
            */
	    if((++i_sync) >= cntr->d_sync) { i_sync=0; cntr->Sync(); }
	    for(int i_c=0; i_c < cntr->iterate; i_c++)
		for(unsigned i_p=0; i_p < cntr->p_hd.size(); i_p++)
		    ((TVPrm &)cntr->at(cntr->p_hd[i_p])).Calc();
	}
	cntr->run_st = false;
    } catch(...) { }

    return(NULL);
}

void TVContr::Sync()
{
    TKernel &Kern = Owner().Owner().Owner();
    for(unsigned i_x = 0; i_x < p_io_hd.size(); i_x++)
	if( !p_io_hd[i_x]->internal )
	{
	    try
	    {
		string vl_nm = "VAL";
		TVal &val = Kern.Param()[p_io_hd[i_x]->hd_prm].at().val().vlVal(vl_nm);
		if( !val.valid() ) continue;
		if( p_io_hd[i_x]->sync )
		{
		    val.setR(p_io_hd[i_x]->x,NULL,true);
		    p_io_hd[i_x]->sync = false;
		}
		else p_io_hd[i_x]->x = val.getR( );
	    }catch(TError) {  }
	}    
    //Sync individual parameters
    for(unsigned i_p=0; i_p < p_hd.size(); i_p++)
	((TVPrm &)at(p_hd[i_p])).Sync();
}

TParamContr *TVContr::ParamAttach( string name, int type )
{
    return(new TVPrm(name,&Owner().at_TpPrm(type),this));
}

int TVContr::prm_connect( string name )
{
    STime tm = {0,0}; 
    //Find already connected
    for(unsigned i_hd = 0; i_hd < p_io_hd.size(); i_hd++)
    {
	if( p_io_hd[i_hd]->internal && at(p_io_hd[i_hd]->hd_prm).Name() == name )	
	    return(i_hd);
	if( !p_io_hd[i_hd]->internal && Owner().Owner().Owner().Param().at(p_io_hd[i_hd]->hd_prm).Name() == name )	
	    return(i_hd);
    }
    //Create new
    SIO io;
    try
    {
	io.hd_prm   = att(name,Name());
	//io.min = at(io.hd_prm)._vl_GetR(hd_y,tm,V_MIN);
	//io.max = at(io.hd_prm)._vl_GetR(hd_y,tm,V_MAX);
	io.internal = true;
    }
    catch(...)
    {   
	try
	{
	    io.hd_prm   = Owner().Owner().Owner().Param().att(name,Name());
	    //io.min = Owner().Owner().Owner().Param().at(io.hd_prm)._vl_GetR(hd_y,tm,V_MIN);
	    //io.max = Owner().Owner().Owner().Param().at(io.hd_prm)._vl_GetR(hd_y,tm,V_MAX);
    	    io.internal = false;
	}
	catch(...) { return(-1); }
    }
    io.x = 0.0;
    p_io_hd.push_back( new SIO(io) );
    
    return( p_io_hd.size()-1 );
}

SIO &TVContr::prm( unsigned hd )
{
    if(hd >= p_io_hd.size()) throw TError("%s: hd %d no avoid!",NAME_MODUL,hd);
    return( *p_io_hd[hd] );
}

//======================================================================
//==== TVPrm 
//====================================================================== 

TVPrm::TVPrm( string name, TTipParam *tp_prm, TController *contr) : 
    TParamContr(name,tp_prm,contr), pid(NULL)
{

}

/*
void TVPrm::UpdateVAL( )
{
    TParamContr::UpdateVAL();
    
    
    hd_y  = vl_Elem().elNameId("VAL");
    if( vl_Elem().vle_Type(hd_y)&VAL_T_REAL )
    {
	STime tm = {0,0}; 
    	y_min = _vl_GetR(hd_y,tm,V_MIN);
	y_max = _vl_GetR(hd_y,tm,V_MAX);
    }
    else y_max = y_min = 0.0;
    
    if(vlElem().elName() == "PID")
    {	
    	pid = new SPID;
    	pid->hd_out  = vlElem().elNameId("OUT");
    	pid->hd_sp   = vlElem().elNameId("SP");
    	pid->hd_stat = vlElem().elNameId("STAT");	
    }
} 
*/

TVPrm::~TVPrm( )
{    
    if(pid) delete pid;
}

void TVPrm::vlSet( int id_elem )
{
    Owner().Owner().m_put("DEBUG",MESS_WARNING,"%s:%s:Comand to direct set value of element!",Owner().Name().c_str(),Name().c_str());
}

void TVPrm::vlGet( int id_elem )
{
    Owner().Owner().m_put("DEBUG",MESS_WARNING,"%s: Comand to direct get value of element!",Owner().Name().c_str(),Name().c_str());
}

void TVPrm::Load( )
{
    SAlgb *algb = NULL;
    try
    {
	algb = ( (TVirtual &)( (TVContr &)Owner() ).Owner() ).AlgbS()->GetAlg(Name());
    }
    catch(TError err) 
    {
    	form = -1;
	return;
    }
    form = algb->tp_alg;
    
    y_id = ( (TVContr &)Owner() ).prm_connect(Name());
    for(unsigned i_x=0; i_x < algb->io.size(); i_x++)
    {
	if( i_x >= x_id.size() ) x_id.insert(x_id.begin()+i_x,-1);
	x_id[i_x] =  ( (TVContr &)Owner() ).prm_connect(algb->io[i_x]);
    }
    for(unsigned i_k = 0;i_k < algb->kf.size(); i_k++)
    {
	if( i_k >= k.size() ) k.insert(k.begin()+i_k,0.0);
	k[i_k] = algb->kf[i_k];
    }
}

inline void TVPrm::Y(float val)
{
    float val_t;
    
    SIO &io = ( (TVContr &)Owner() ).prm(y_id);   
    if(io.max == io.min) val_t = val;
    else val_t = (val > io.max)?io.max:(val < io.min)?io.min:val;
    if(io.x != val_t) { io.sync = true; io.x = val_t; }
    /*
    STime tm = {0,0};
    _vl_SetR(hd_y,val,tm);
    */
}

inline float TVPrm::Y()
{ 
    return( ( (TVContr &)Owner() ).prm(y_id).x );
    /*
    STime tm = {0,0};
    return(_vl_GetR(hd_y,tm));
    */
}

inline void TVPrm::X(unsigned id ,float val)
{ 
    float val_t;

    if(x_id[id] < 0) return;
    SIO &io = ( (TVContr &)Owner() ).prm(x_id[id]);   
    if(io.max == io.min) val_t = val;
    else val_t = (val > io.max)?io.max:(val < io.min)?io.min:val;
    if(io.x != val_t) { io.sync = true; io.x = val_t; }
}

inline float TVPrm::X(unsigned id)
{
    if(x_id[id] < 0) return(0.0);
    return( ( (TVContr &)Owner() ).prm(x_id[id]).x );
}

void TVPrm::Sync()
{    

}

float TVPrm::Calc()
{
    if(form < 0) return(1E+10);

    
    switch( ((TVirtual &)((TVContr &)Owner()).Owner()).AlgbS()->GetFrm(form)->tip)
    {	
	case  0:return(0.0);
	case  1:return blok_dig();
        //case  2:return komyt(GB);
    	case  3:return ymn();
     	case  4:return sym();
	case  5:return free_formul();
//      case  8:return ymndl(GB);
//      case  9:return esx(GB);
//      case 10:return xsy(GB);
//      case 11:
//      case 12:return es1(GB);
//      case 13:return es3(GB);
//      case 14:return es4(GB);
//      case 15:return es5(GB);
//      case 16:return del(GB);
	case 17:return pid_n(); //pid(GB);
//      case 18:return nar(GB);
//      case 19:return zad_n(GB); //zad(GB);
//      case 20:return imzad(GB);
//      case 21:return petl(GB);
//      case 22:return im1(GB);
//      case 23:return symn(GB);
//      case 24:return sdel(GB);
//      case 25:return zap1(GB);
//      case 26:return zap2(GB);
//      case 27:return vard(GB);
//      case 28:return systep(GB);
//      case 29:return exp1(GB);
//      case 30:return perekl(GB);
//      case 31:return volna(GB);
//      case 32:return baj(GB);
//      case 33:return alarmk(GB);
//      case 34:return srob(GB);
    }
    Owner().Owner().m_put("CONTR",MESS_WARNING,"%s:%s:%d Furmule id= %d no avoid!",
		Owner().Name().c_str(),Name().c_str(),form, 
		((TVirtual &)((TVContr &)Owner()).Owner()).AlgbS()->GetFrm(form)->tip);

    return(1E+10);
}

//************************************************************
//* float blok_dig( );                                       *
//*   Сборка дискретных параметров.                          *
//************************************************************
float TVPrm::blok_dig( )
{
    bool set = false;

    if(X(0) && k[2] != 1.) { k[2]=1.; set = true; }
    if(X(2) && k[2] != 2.) { k[2]=2.; set = true; }
    if(X(4) && k[2] != 3.) { k[2]=3.; set = true; }
    if( set && k[0] > 0. ) { k[1]=k[0]; set = false; }
    if(k[1] > 0.) k[1] -= ((TVContr &)Owner()).Period()/(1000*sysconf(_SC_CLK_TCK)*((TVContr &)Owner()).Iterate());
    else
    {
    	k[1] = 0.;
	if(k[0] > 0.)
	{
	    if(k[2] == 1.) {k[2]=0.; X(0,0.); }
	    if(k[2] == 2.) {k[2]=0.; X(2,0.); }
	    if(k[2] == 3.) {k[2]=0.; X(4,0.); }
	}
    }
    return 0.;
}
       
//************************************************************
//* float ymn();                                             *
//*   Function simple multiply and divi.                     *
//************************************************************
float TVPrm::ymn()
{
    if(X(2)==0. || X(3)==0. || X(6)==0. || X(7)==0.) return(1E+10);
    if( !k[0] ) k[0] = 1.;
    return(k[0]*k[5]*X(0)*X(1)*X(4)*X(5))/(X(2)*X(3)*X(6)*X(7));
}
         
//************************************************************
//* float sym(a_fli *GB);                                    *
//*   Function addition.                                     *
//************************************************************
float TVPrm::sym()
{
    return(k[0]*X(0)+k[1]*X(1)+k[2]*X(2)+k[5]*X(3)+k[6]*X(4)+k[7]*X(5)+k[10]*X(6)+k[11]*X(7));
}

//************************************************************
//* float free_formul(a_fli *GB)                             *
//*   Flow formuls that free edit.                           *
//************************************************************
float TVPrm::free_formul( )
{
    int offset = 0;
    SFrm *formul = ((TVirtual &)((TVContr &)Owner()).Owner()).AlgbS()->GetFrm(form);
    return(calk_form(formul->form_e,formul->l_frm_e,&offset,0,0));
}
               
float TVPrm::calk_form(char *form, int len, int *off, float rez,byte h_prior)
{
    int     i,ii;
    dword   oper;
    float   parm=0,temp[10];
    byte    b_form;
    byte    symb=0;
    byte    flow_prior=0;
    byte    tmb_b;


    hom_f:
    if(*off > len-1) goto exit_f;
    b_form = form[*off];
    if(b_form=='(')
    {
	(*off)++;
	if(symb) parm = calk_form(form,len,off,0,0);  
	else     rez  = calk_form(form,len,off,0,0);
	goto hom_f;
    }    
    if(b_form==')' || b_form==',' || b_form==';')
    {
	exit_f:
	calk_oper(symb,&rez,parm);
	if( !h_prior && b_form!=';') (*off)++;
	return rez;
    }
    if(b_form=='Y')                              //Self param
    {
	(*off)++;
	if(form[*off]=='=')
	{ (*off)++; Y(calk_form(form,len,off,0,0)); (*off)++; goto hom_f; }
    	if(symb) parm = Y(); else rez = Y();
	goto hom_f;
    }
    if(b_form=='X')                              //External param
    {
	oper = form[++(*off)]; ++(*off);
	if(form[*off]=='='){(*off)++; X(oper,calk_form(form,len,off,0,0)); (*off)++; goto hom_f; }
	if(form[*off]=='('){(*off)++; X(oper,calk_form(form,len,off,0,0)); }
	if(symb) parm = X(oper); else rez = X(oper);
	goto hom_f;
    }
    if(b_form=='K')                              //Koefficients
    {
	oper = form[++(*off)]; ++(*off);
	if(form[*off]=='='){ (*off)++; k[oper]=calk_form(form,len,off,0,0); (*off)++; goto hom_f; }
	if(form[*off]=='('){ (*off)++; k[oper]=calk_form(form,len,off,0,0); }
	if(symb) parm = k[oper]; else rez = k[oper];
	goto hom_f;
    }
    if(b_form=='C')                              //Konstans
    {
	if(symb) parm = *(float *)(form+(++(*off)));
	else     rez  = *(float *)(form+(++(*off)));
	(*off)+=4; goto hom_f;
    }
    //-----------------------
    if(b_form=='&' || b_form=='|')                 {i=5;  goto m1;}
    if(b_form=='>' || b_form=='<' || b_form=='~')  {i=10; goto m1;}
    if(b_form=='+' || b_form=='-')                 {i=15; goto m1;}
    if(b_form=='*' || b_form=='/')                 {i=20; goto m1;}
    if(b_form=='^')
    {
	i=25;
	m1:
	if(!symb) { symb = b_form; flow_prior=i; ++(*off); goto hom_f; }
	if(flow_prior == i){ calk_oper(symb,&rez,parm); symb = b_form; ++(*off); goto hom_f; }
	if(flow_prior > i)
    	{
	    calk_oper(symb,&rez,parm);
    	    if(h_prior) return rez;
	    flow_prior=i;
	    symb=b_form;
	    ++(*off);
	    goto hom_f;
	}
	if(flow_prior < i) { parm = calk_form(form,len,off,parm,1); goto hom_f; }
    }
    //            ---- Function ----
    oper = *(dword *)(form+(*off));
    if((word)oper == *(word *)"pi")               //pi
    { (*off)+=2; if(symb) parm=3.14159256; else rez=3.14159256; goto hom_f; }
    if((oper&0x00FFFFFF) == 'krb'){ (*off) = len; goto exit_f;}   //brk
    if((oper&0x00FFFFFF) == 'qrf')                 //frq
    {
	(*off)+=3;
	if(symb) parm = 1000*((TVContr &)Owner()).Iterate()*sysconf(_SC_CLK_TCK)/((TVContr &)Owner()).Period();
	else     rez  = 1000*((TVContr &)Owner()).Iterate()*sysconf(_SC_CLK_TCK)/((TVContr &)Owner()).Period();
	goto hom_f;
    }
    if(oper == *(dword *)"exp(")                             //exp()
    {
	(*off)+=4;
    	if(symb) parm = exp(calk_form(form,len,off,0,0));
	else     rez  = exp(calk_form(form,len,off,0,0));
	goto hom_f;
    }
    if(oper == *(dword *)"abs(")                             //abs()
    {
    	(*off)+=4;
	if(symb) parm = fabs(calk_form(form,len,off,0,0));
	else     rez  = fabs(calk_form(form,len,off,0,0));
	goto hom_f;
    }
    if(oper == *(dword *)"sig(")                             //sig()
    {
	(*off)+=4;
	temp[0] = calk_form(form,len,off,0,0);
	if(temp[0] >= 0.) temp[0]=1.0; else temp[0]=-1.0;
	if(symb) parm = temp[0]; else rez = temp[0];
	goto hom_f;
    }
    if(oper == *(dword *)"cos(")                             //cos()
    {
	(*off)+=4;
    	if(symb) parm = cos(calk_form(form,len,off,0,0));
	else     rez  = cos(calk_form(form,len,off,0,0));
	goto hom_f;
    }
    if((oper&0x00FFFFFF) =='(nl')                       //ln()
    {
	(*off)+=3;
	if(symb) parm = log(calk_form(form,len,off,0,0));
	else     rez  = log(calk_form(form,len,off,0,0));
    	goto hom_f;
    }
    if(oper == *(dword *)"sin(")                             //sin()
    {
	(*off)+=4;
	if(symb) parm = sin(calk_form(form,len,off,0,0));
	else     rez  = sin(calk_form(form,len,off,0,0));
	goto hom_f;
    }
    if(oper == *(dword *)"rnd(")                             //rnd()
    {
	(*off)+=4;
	if(symb) parm = ((float)rand()/(float)RAND_MAX) * calk_form(form,len,off,0,0);
	else     rez  = ((float)rand()/(float)RAND_MAX) * calk_form(form,len,off,0,0);
	goto hom_f;
    }
    if(oper == *(dword *)"max(")                             //max()
    {
	(*off)+=4;
	temp[1] = calk_form(form,len,off,0,0); 
	temp[2] = calk_form(form,len,off,0,0);
	if(symb) { if(temp[1] > temp[2]) parm = temp[1]; else parm = temp[2]; }
	else     { if(temp[1] > temp[2]) rez  = temp[1]; else rez  = temp[2]; }
	goto hom_f;
    }
    if(oper == *(dword *)"min(")                             //min()
    {
	(*off)+=4;
    	temp[1] = calk_form(form,len,off,0,0); 
	temp[2] = calk_form(form,len,off,0,0);
	if(symb) { if(temp[1] < temp[2]) parm = temp[1]; else parm = temp[2]; }
	else     { if(temp[1] < temp[2]) rez  = temp[1]; else rez  = temp[2]; }
	goto hom_f;
    }
    if(oper == *(dword *)"msg(")                             //msg()
    {
	(*off)+=4;
	temp[9] = calk_form(form,len,off,0,0);
	for(;form[*off] != '"'; (*off)++);
	for(i=0 , (*off)++; (*off)+i <= len-1;i++)
	if(form[(*off)+i]=='"')
	{
	    /*strncpy(buf_glb,PTR_STR+FORM_OFFS,i);*/ 
	    (*off)+=i; 
	    /*buf_glb[i]=0;*/ 
	    i=0;
	    break; 
	}
	for((*off)++;(*off)<=len-1;(*off)++)
	{
	    if(form[*off]==',' || form[*off]==')')
	    {
    		(*off)++; ii=0;
		while(form[(*off)-1]==',') { temp[ii] = calk_form(form,len,off,0,0); ii++;}
		//       sprintf(srv_mes.mess,buf_glb,temp[0],temp[1],temp[2],temp[3],temp[4],temp[5]);
		//       srv_mes.tip_mes=temp[9]; s_mes();
                if(symb) parm= 0.; else rez= 0.;
                goto hom_f;
	    }
	}
    }
    if((oper&0x00FFFFFF) =='(fi')                             //if()  :
    {
    	(*off)+=3;
	if(calk_form(form,len,off,0,0) > 0)
	{
	    if(symb) parm = calk_form(form,len,off,0,0);
	    else     rez  = calk_form(form,len,off,0,0);
	    if(form[(*off)-1]==')') { (*off)--; goto hom_f; }
	    if( (*off) > len-1) goto hom_f;
	    i=0;
	    while((*off) <= len-1)
	    {
		tmb_b = form[*off];
		if(tmb_b=='K' || tmb_b=='X') (*off)+=1;
		if(tmb_b=='C')               (*off)+=4;
    		if(tmb_b=='(') i++;
		if(tmb_b==')') { i--; if(i== -1) { (*off)++; goto hom_f;} }
		if(tmb_b=='"') for((*off)++; ((*off) <= len-1) && form[*off]!='"'; (*off)++);
		(*off)++;
	    }
	    goto hom_f;
	}
	i=0;
    	while( (*off) <= len-1)
	{
	    tmb_b = form[*off];
	    if(tmb_b=='K' || tmb_b=='X') (*off)+=1;
    	    if(tmb_b=='C')               (*off)+=4;
	    if(tmb_b=='(') i++;
	    if(tmb_b==')') {i--;if(i== -1)goto hom_f;}
	    if(tmb_b==',' && !i)
	    {
		(*off)++;
		if(symb) parm = calk_form(form,len,off,0,0);
		else     rez  = calk_form(form,len,off,0,0);
		goto hom_f;
	    }
    	    if(tmb_b=='"')
	    for( (*off)++;( (*off) <= len-1) && form[*off]!='"';(*off)++);
	    (*off)++;
	}
	goto hom_f;
    }
    return 0.;
}
//--------------------------------------------------------------
void TVPrm::calk_oper(byte symb,float *rez,float parm)
{
    if(symb=='+'){ *rez +=parm; return;}
    if(symb=='-'){ *rez -=parm; return;}
    if(symb=='*'){ *rez *=parm; return;}
    if(symb=='/'){ *rez /=parm; return;}
    if(symb=='^'){ *rez=pow(*rez,parm); return;}
    if(symb=='>'){ if(*rez >parm)*rez=1;else *rez=0; return;}
    if(symb=='<'){ if(*rez <parm)*rez=1;else *rez=0; return;}
    if(symb=='~'){ if(*rez==parm)*rez=1;else *rez=0; return;}
    if(symb=='&'){ *rez=*rez &&parm; return;}
    if(symb=='|'){ *rez=*rez ||parm; return;}
}

//************************************************************
//* float pid_n(a_fli *GB);                                  *
//*  Function of a PID regulator                             *
//************************************************************
float TVPrm::pid_n( )
{
    float  err=0.,vhod=0.,KInt,Kzdif,Dif,Kf,k1,k2,k3,k4,in;

    if(!pid) return(1E+10);
    
    int    period      = ((TVContr &)Owner()).Period();
    int    HZ          = 1000*sysconf(_SC_CLK_TCK);
    int    cnt_in_cycl = ((TVContr &)Owner()).Iterate();
    float  sp          = pid->sp->getR();
    float  out         = pid->out->getR();
    char   stat        = pid->stat->getI();
    
    k1 = 100 * X(1)/( (labs((long)Y_MAX()) > labs((long)Y_MIN()))?labs((long)Y_MAX()):labs((long)Y_MIN()) );
    k2 = 100 * X(2)/( (labs((long)Y_MAX()) > labs((long)Y_MIN()))?labs((long)Y_MAX()):labs((long)Y_MIN()) );
    k3 = 100 * X(3)/( (labs((long)Y_MAX()) > labs((long)Y_MIN()))?labs((long)Y_MAX()):labs((long)Y_MIN()) );
    k4 = 100 * X(4)/( (labs((long)Y_MAX()) > labs((long)Y_MIN()))?labs((long)Y_MAX()):labs((long)Y_MIN()) );
    
    if(x_id[5] >= 0)  sp = 100.*(X(5)-Y_MIN())/(Y_MAX()-Y_MIN());

    if(stat != R_MAN && stat != R_AUTO && stat != R_CAS) stat = R_MAN;
    if(stat == R_CAS && x_id[6] < 0) stat = R_AUTO;
    //if(stat == R_CAS) sp =    c_ptr[ALGB->inp[6].nc].a_ptr[ALGB->inp[6].nz].out_cod;
    in = 100. * (X(0)-Y_MIN())/(Y_MAX()-Y_MIN());

    vhod = in+k[0]*k1+k[1]*k2;
    if(vhod >  100.) vhod =  100.;
    if(vhod < -100.) vhod = -100.;
	    
    err = sp - vhod;
    if((err > 0. && err < k[6]) || (err < 0. && err > -k[6])) err = 0.;
    if( err > 0. ) err -= k[6];
    if( err < 0. ) err += k[6];

    err *= k[9];
    if(err >  100.) err =  100.;
    if(err < -100.) err = -100.;
	    
    if( k[12] > period/(HZ*cnt_in_cycl) ) Kf = period/(k[12]*HZ*cnt_in_cycl);
    else                                  Kf = 1.;
    k[7] += Kf*(err - k[7]);

    if(stat != R_MAN)
    {
	if(k[10] > period/(HZ*cnt_in_cycl)) KInt = period/(k[10]*HZ*cnt_in_cycl);
	else                                KInt = 1.;
	if(k[11] > period/(HZ*cnt_in_cycl)) Kzdif = period/(k[11]*HZ*cnt_in_cycl);
	else                                Kzdif = 1.;

	k[14] -= Kzdif * (k[14]-k[7]);
	k[14] -= Kzdif * (k[14]-k[7]);
	Dif = k[7]-k[14];
	k[13] += KInt * k[7];
	out = (k[7] + k[13] + Dif);

	out += (k[2] * k3 + k[3] * k4);

	if(out > k[5]){ out = k[5]; k[13] = out-k[7]-Dif-(k[2] * k3 + k[3] * k4);}
	if(out < k[4]){ out = k[4]; k[13] = out-k[7]-Dif-(k[2] * k3 + k[3] * k4);}
        /*
	if(x_id[6].hd_prm >= 0)
	{
	    if( pid->stat == R_CAS) pid->sp = ((c_ptr+ALGB->inp[6].nc)->a_ptr+ALGB->inp[6].nz)->out_cod;
	    else
	    {
		((c_ptr+ALGB->inp[6].nc)->a_ptr+ALGB->inp[6].nz)->out_cod=ptr_a->set_cod;
		((c_ptr+ALGB->inp[6].nc)->a_ptr+ALGB->inp[6].nz)->mod_cod=R_MAN;
	    }
	}
	*/
    }
    else
    {
    	k[13] = out - k[2] * k3 - k[3] * k4 - k[7];
	k[14] = k[7];
    }

    if( x_id[7] >= 0 ) X(7,out);

    pid->sp->setR(sp);
    pid->out->setR(out);
    pid->stat->setI(stat);

    if( x_id[0] < 0 )  return 0.;
    return X(0);
}

//======================================================================
//==== TVirtAlgb =======================================================
//====================================================================== 

TVirtAlgb::TVirtAlgb(string cfg_file) : file(cfg_file)
{
    Load();
}

TVirtAlgb::~TVirtAlgb( )
{
    Free();
}

void TVirtAlgb::Load(string f_alg)
{
    int        buf_len = 100000;
    dword      ofs_alg;
    word       form_am, k_alg, tp_alg, len_2;
    byte       len_1;
    char       *buf, *file_alg;
    
    if(f_alg.size())     file_alg = (char *)f_alg.c_str();
    else if(file.size()) file_alg = (char *)file.c_str();
    else throw TError("%s: File algoblocs no avoid!",NAME_MODUL);
   
    int fh = open(file_alg,O_RDONLY);
    if(fh == -1) throw TError("%s: Open file %s for read, error!",NAME_MODUL,file_alg);    

    Free();

    buf = (char *)malloc(buf_len);
    
    read(fh,&ofs_alg,4); 
    read(fh,&form_am,2);
    for(unsigned i_frm = 0; i_frm < form_am; i_frm++)
    {
	SFrm *frm = new SFrm;
       	read(fh,&len_1,1);
	if(len_1)
	{
	    read(fh,buf,len_1); buf[len_1]=0;
	    frm->name = buf;
	    Mess->SconvIn("CP866",frm->name);
	}
       
	read(fh,&frm->tip,1);
	read(fh,&frm->n_inp,1); 
	read(fh,&frm->n_koef,1);
	if(frm->n_inp)
   	    for(unsigned i_inp=0;i_inp < frm->n_inp;i_inp++)
   	    {
		if( i_inp == frm->name_inp.size() ) 
		    frm->name_inp.push_back("");
       		read(fh,&len_1,1);
	       	read(fh,buf,len_1); buf[len_1] = 0;
	       	frm->name_inp[i_inp] = buf;
	    }
	if(frm->n_koef)
	    for(unsigned i_kf=0;i_kf < frm->n_koef;i_kf++)
	    {
    		if( i_kf == frm->name_kf.size() ) 
		    frm->name_kf.push_back("");
		read(fh,&len_1,1);
		read(fh,buf,len_1); buf[len_1] = 0;
		frm->name_kf[i_kf] = buf;
	    }
	read(fh,&len_2,2);
    	read(fh,buf,len_2); buf[len_2] = 0;
      	frm->formul = buf;
	if(frm->tip==5)
    	{
	    if(!frm->formul.size()) continue;
       	    char *str1 = (char *)frm->formul.c_str();
   	    unsigned i_c, i_n;
	    for( i_c = 0, i_n=0;i_c < strlen(str1);i_c++)
   	    {
       		if(*(word *)(str1+i_c) == *(word *)"//")
	   	{ 
		    for( ; (i_c < strlen(str1) && str1[i_c]!=0x0D);i_c++) ; 
		    continue;
		}
		if(str1[i_c] == '"' )
		{
		    buf[i_n++] = str1[i_c++];
		    for( ; i_c < strlen(str1);i_c++,i_n++)
		    {
	      		buf[i_n] = str1[i_c];
		    	if(str1[i_c] == '"') 
			{
			    buf[i_n++] = str1[i_c]; 
			    break;
			}
		    }
		    continue;
		}
		if(str1[i_c] == ' ' || str1[i_c] == '\x0D' || str1[i_c] == '\x0A') continue;
		if(str1[i_c] == 'K' || str1[i_c] == 'X')
    		{
	 	    buf[i_n] = str1[i_c];
     		    i_c++; i_n++;
     		    buf[i_n] = str1[i_c];
     		    i_n++;
     		    continue;
	 	}
		if(str1[i_c] == 'C')
		{
		    buf[i_n] = str1[i_c];
		    i_c++; i_n++;
		    *(float*)(buf+i_n) = *(float*)(str1+i_c);
		    i_c+=3; i_n+=4;
		    continue;
		}
		buf[i_n]=str1[i_c]; 
		i_n++;
	    }
	    buf[i_n] = 0;
            frm->form_e  = strdup(buf);
	    frm->l_frm_e = i_n;
	}
	frm_s.push_back(frm);
	//if(i_frm == frm_s.size()) frm_s.push_back(frm);
	//else                      frm_s[i_frm] = frm;
    }
    lseek(fh,ofs_alg,SEEK_SET);
    read(fh,&k_alg,2);
    for(unsigned i_alg=0; i_alg < k_alg; i_alg++)
    {
	SAlgb *algb = new SAlgb;
	
    	read(fh,buf,9); 
	buf[9] = 0;
	for(int i=8; i >= 0; i--) 
	    if(buf[i]==' ' || buf[i]== 0) buf[i]=0; 
	    else break; 
	algb->name = buf;
	Mess->SconvIn("CP866",algb->name);

	read(fh,&len_1,1); 
	read(fh,buf,len_1); buf[len_1]=0;
	algb->descr = buf;
	Mess->SconvIn("CP866",algb->descr);
	
	read(fh,&tp_alg,2);
	algb->tp_alg = tp_alg;
	
	unsigned i_n = frm_s[tp_alg]->n_inp;
	if(i_n)
	    for(unsigned i_x=0;i_x < i_n;i_x++)
	    { 
		if( i_x == algb->io.size() ) algb->io.push_back("");
		
		read(fh,buf,9); buf[9] = 0;
		for(int i=8; i >= 0; i--) 
		    if(buf[i]==' ' || buf[i]== 0) buf[i]=0; 
		    else break;
		algb->io[i_x] = buf;
		Mess->SconvIn("CP866",algb->io[i_x]);
	    }
	i_n = frm_s[tp_alg]->n_koef;
	if(i_n)
	    for(unsigned i_k = 0;i_k < i_n; i_k++)
	    {
		if( i_k == algb->kf.size() ) algb->kf.push_back(0.0);
		read(fh,&(algb->kf[i_k]),4);
	    }
	algb_s.push_back(algb);
    }
    free(buf);
    close(fh); 
}

void TVirtAlgb::Save(string f_alg)
{
    char   *file_alg;
    
    if(f_alg.size())     file_alg = (char *)f_alg.c_str();
    else if(file.size()) file_alg = (char *)file.c_str();
    else throw TError("%s: File algobloks no avoid!",NAME_MODUL);
   
    int fh = open(file_alg,O_WRONLY);
    if(fh == -1) throw TError("%s: Open file %s for write, error!",NAME_MODUL,file_alg);    
    //----------------
    //make in future
    //----------------    
    close(fh); 
}

void TVirtAlgb::Free( )
{
    while( algb_s.size() )
    {
        delete algb_s[0];	
	algb_s.erase(algb_s.begin());
    }
    while( frm_s.size() )
    {
	if( frm_s[0]->tip==5 ) free( frm_s[0]->form_e );
        delete frm_s[0];
	frm_s.erase(frm_s.begin());
    }
}


SAlgb *TVirtAlgb::GetAlg(string name)
{
    for(unsigned i_alg = 0; i_alg < algb_s.size(); i_alg++)
	if(algb_s[i_alg]->name == name) return(algb_s[i_alg]);
    throw TError("%s: Algoblok %s no avoid!",NAME_MODUL,name.c_str());
}

SFrm *TVirtAlgb::GetFrm(unsigned id)
{
    if( id < frm_s.size() ) return( frm_s[id] );
    throw TError("%s: Formule %d no avoid!",NAME_MODUL,id);
}

