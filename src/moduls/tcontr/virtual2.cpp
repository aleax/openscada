/* Test Modul
** ==============================================================
*/

#include <getopt.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <math.h>

#include "../../terror.h"
#include "../../tsys.h"
#include "../../tkernel.h"
#include "../../tmessage.h"
#include "../../tconfig.h"
#include "../../tvalue.h"
#include "../../tcontroller.h"
#include "../../ttipcontroller.h"
#include "../../tcontrollers.h"
#include "../../tparamcontr.h"
#include "../../tparam.h"
#include "../../tparams.h"
#include "virtual2.h"

//============ Modul info! =====================================================
#define NAME_MODUL  "virtual_v2"
#define NAME_TYPE   "Controller"
#define VERSION     "0.0.5"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "Virtual controller V2.x new generation"
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
    TModule *attach( char *FName, int n_mod )
    {
	Virtual2::TVirtual *self_addr;
	if(n_mod==0) self_addr = new Virtual2::TVirtual( FName );
	else         self_addr = NULL;
	return static_cast< TModule *>( self_addr );
    }
}

using namespace Virtual2;

//======================================================================
//==== TVirtual ======================================================== 
//======================================================================

//==== Desribe controler's bd fields ====
SCfgFld TVirtual::elem[] =         
{    
    {PRM_B_AN  ,"Name BD for ANALOG parameteres."    ,CFG_T_STRING,"","","VRT_AN","30",""       ,"%s"},
    {PRM_B_DG  ,"Name BD for DIGIT parameteres."     ,CFG_T_STRING,"","","VRT_DG","30",""       ,"%s"},
    {PRM_B_BLCK,"Name BD for BLOCK parameteres."     ,CFG_T_STRING,"","","VRT_BL","30",""       ,"%s"},
    {"PERIOD"  ,"Pooled period of calc (ms)."        ,CFG_T_INT   ,"","","1000"  ,"5" ,"0;10000","%d"},
    {"ITER"    ,"Number a iterations at calc period.",CFG_T_INT   ,"","","1"     ,"2" ,"0;99"   ,"%d"},
    {"PER_S"   ,"Pooled period of sync (ms)."        ,CFG_T_INT   ,"","","1000"  ,"5" ,"0;10000","%d"}
};
//==== Desribe ANALOG parameter's bd fields ====
SCfgFld TVirtual::ElemAN[] =         
{
    {"ED"     ,"Value of measurement"          ,CFG_T_STRING             ,"","",""    ,"10"  ,""              ,"%s"   },
    {"SCALE"  ,"Scale"                         ,CFG_T_INT|CFG_T_SELECT   ,"","","0"   ,"1"   ,"0;1"           ,"%s"   ,"Linear;Square"},
    {"TIPO"   ,"Type of processing"            ,CFG_T_INT|CFG_T_SELECT   ,"","","0"   ,"1"   ,"0;1;2"         ,"%s"   ,"Average;Integrate;Counter"},
    {"MIN"    ,"Lower scale border"            ,CFG_T_REAL               ,"","","0"   ,"10.2",""              ,"%.2f" },
    {"MAX"    ,"Upper scale border"            ,CFG_T_REAL               ,"","","100" ,"10.2",""              ,"%.2f" },
    {"NTG"    ,"Lower technically scale border",CFG_T_REAL               ,"","","0"   ,"10.2","$MIN;$MAX"     ,"%.2f" },
    {"VTG"    ,"Upper technically scale border",CFG_T_REAL               ,"","","0"   ,"10.2","$MIN;$MAX"     ,"%.2f" },
    {"NAG"    ,"Lower alarm scale border"      ,CFG_T_REAL               ,"","","0"   ,"10.2","$MIN;$MAX"     ,"%.2f" },
    {"VAG"    ,"Upper alarm scale border"      ,CFG_T_REAL               ,"","","0"   ,"10.2","$MIN;$MAX"     ,"%.2f" },
    {"Z_GR"   ,"Non-sensitive zone (%)"        ,CFG_T_REAL               ,"","","0.5" ,"4.1" ,"0;50"          ,"%2.1f"},
    {"TYPE"   ,"Parameter type"                ,CFG_T_STRING|CFG_T_SELECT,"","","A_IN","6"   ,"A_IN;A_OUT;PID","%s"   ,"Input;Output;PID"}
};
//==== Desribe DIGIT parameter's bd fields ====
SCfgFld TVirtual::ElemDG[] =
{
    {"TYPE"   ,"Parameter type",CFG_T_STRING|CFG_T_SELECT,"","","D_IN","6","D_IN;D_OUT","%s","Input;Output"}
};
//==== Desribe BLOCK parameter's bd fields ====
SCfgFld TVirtual::ElemBL[] =
{
};
//=============================================
//==== Describe ANALOG param struct ===========
SVAL TVirtual::ValAN[] =
{
    {"VAL" ,"Value"      ,"Value analog parameter" ,VAL_T_REAL,VAL_S_LOCAL,VAL_IO_DEF,"1.10","$MIN;$MAX","0666"},
    {"NTG" ,"Low tech"   ,"Value low tech border"  ,VAL_T_REAL,VAL_S_BD   ,VAL_IO_DEF,"2.5" ,"NTG"      ,"0644"}, 
    {"VTG" ,"Up tech"    ,"Value up tech border"   ,VAL_T_REAL,VAL_S_BD   ,VAL_IO_DEF,"2.5" ,"VTG"      ,"0644"}, 
    {"NAG" ,"Low alarm"  ,"Value low alarm border" ,VAL_T_REAL,VAL_S_BD   ,VAL_IO_DEF,"2.4" ,"NAG"      ,"0644"}, 
    {"VAG" ,"Up alarm"   ,"Value up alarm border"  ,VAL_T_REAL,VAL_S_BD   ,VAL_IO_DEF,"2.4" ,"VAG"      ,"0644"},
    {"Z_GR","Nonsensit"  ,"Nonsensitive zone"      ,VAL_T_REAL,VAL_S_BD   ,VAL_IO_DEF,"3.10","Z_GR"     ,"0644"} 
};

//==== Describe DIGIT param struct ===========
SVAL TVirtual::ValDG[] =
{
    {"VAL" ,"Value"     ,"Value digital parameter",VAL_T_BOOL,VAL_S_LOCAL,VAL_IO_DEF,"1.10","","0666"}
};

//==== PID regulator ===========

SVAL TVirtual::ValPID[] =
{
    {"OUT"   ,"Output"     ,"Output of regulator"                ,VAL_T_REAL            ,VAL_S_LOCAL,VAL_IO_DEF               ,"1.9" ,"0;100"                     ,"0666"},
    {"SP"    ,"SetPoint"   ,"Setpoint of regulator"              ,VAL_T_REAL            ,VAL_S_LOCAL,VAL_IO_DEF               ,"1.8" ,"$MIN;$MAX"                 ,"0666"},
    {"STAT"  ,"Stat mode"  ,"Stat regulator (Manual,Auto,Casc)"  ,VAL_T_INT|VAL_T_SELECT,VAL_S_LOCAL,VAL_IO_DEF               ,"1.7" ,"0;1;2","Manual;Auto;Cascad","0666"},
    {"Kp"    ,"Gain"       ,"Koefficient of proportion"          ,VAL_T_REAL            ,VAL_S_LOCAL,VAL_IO_W_DIR|VAL_IO_R_DIR,"3.9" ,"-20;20"                    ,"0644"},
    {"Ti"    ,"Integr"     ,"Time of integrated (sek)"           ,VAL_T_REAL            ,VAL_S_LOCAL,VAL_IO_W_DIR|VAL_IO_R_DIR,"3.8" ,"0;1000"                    ,"0644"},
    {"Td"    ,"Diferent"   ,"Time of diff (sek)"                 ,VAL_T_REAL            ,VAL_S_LOCAL,VAL_IO_W_DIR|VAL_IO_R_DIR,"3.8" ,"0;1000"                    ,"0644"},
    {"Tf"    ,"Filter"     ,"Time of lag (sek)"                  ,VAL_T_REAL            ,VAL_S_LOCAL,VAL_IO_W_DIR|VAL_IO_R_DIR,"3.8" ,"0;1000"                    ,"0644"},
    {"H_UP"  ,"Up border"  ,"Up out border (%)"                  ,VAL_T_REAL            ,VAL_S_LOCAL,VAL_IO_W_DIR|VAL_IO_R_DIR,"3.7" ,"0;100"                     ,"0644"},
    {"H_DOWN","Down border","Down out border (%)"                ,VAL_T_REAL            ,VAL_S_LOCAL,VAL_IO_W_DIR|VAL_IO_R_DIR,"3.7" ,"0;100"                     ,"0644"},
    {"ZN"    ,"PID nonsensit","Non-sensitive pid error (%)"      ,VAL_T_REAL            ,VAL_S_LOCAL,VAL_IO_W_DIR|VAL_IO_R_DIR,"3.7" ,"0;20"                      ,"0644"},
    {"K1"    ,"K input 1"  ,"Koefficient scale of addon input 1" ,VAL_T_REAL            ,VAL_S_LOCAL,VAL_IO_W_DIR|VAL_IO_R_DIR,"3.7" ,"-20;20"                    ,"0644"},
    {"K2"    ,"K input 2"  ,"Koefficient scale of addon input 2" ,VAL_T_REAL            ,VAL_S_LOCAL,VAL_IO_W_DIR|VAL_IO_R_DIR,"3.7" ,"-20;20"                    ,"0644"},
    {"K3"    ,"K input 3"  ,"Koefficient scale of addon input 3" ,VAL_T_REAL            ,VAL_S_LOCAL,VAL_IO_W_DIR|VAL_IO_R_DIR,"3.7" ,"-20;20"                    ,"0644"},
    {"K4"    ,"K input 4"  ,"Koefficient scale of addon input 4" ,VAL_T_REAL            ,VAL_S_LOCAL,VAL_IO_W_DIR|VAL_IO_R_DIR,"3.7" ,"-20;20"                    ,"0644"}
};

TVirtual::TVirtual(char *name) : NameCfgF("./alg.cfg"), algbs(NULL)
{
    NameModul = NAME_MODUL;
    NameType  = NAME_TYPE;
    Vers      = VERSION;
    Autors    = AUTORS;
    DescrMod  = DESCRIPTION;
    License   = LICENSE;
    FileName  = strdup(name);    
}

TVirtual::~TVirtual()
{    
    if(algbs) delete algbs;
    free(FileName);	
}


void TVirtual::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "==================== Module %s options =======================\n"
    "    --Vrt1CFG=<path>   Set config file name (default ./alg.cfg)\n"
    "------------------ Fields <%s> sections of config file --------------\n"
    "config=<path>          path to config file;\n"
    "\n",NAME_MODUL,NAME_MODUL);
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
    SYS->GetOpt(NAME_MODUL,"config",NameCfgF);
}

void TVirtual::mod_connect( )
{
    TModule::mod_connect( );

    LoadCfg(elem,sizeof(elem)/sizeof(SCfgFld));
    //Add parameter types
    AddTpParm(PRM_ANALOG,PRM_B_AN  ,"Analog parameters");
    AddTpParm(PRM_DIGIT ,PRM_B_DG  ,"Digit parameters");
    AddTpParm(PRM_BLOCK ,PRM_B_BLCK,"Block parameter (algoblock)");
    //Load views for parameter's types    
    LoadTpParmCfg(PRM_ANALOG,ElemAN,sizeof(ElemAN)/sizeof(SCfgFld));
    LoadTpParmCfg(PRM_DIGIT ,ElemDG,sizeof(ElemDG)/sizeof(SCfgFld));
    LoadTpParmCfg(PRM_BLOCK ,ElemBL,sizeof(ElemBL)/sizeof(SCfgFld));
    //Add types of value
    AddTpVal("A_IN",ValAN ,sizeof(ValAN)/sizeof(SVAL));
    AddTpVal("D_IN",ValDG ,sizeof(ValDG)/sizeof(SVAL));
    AddTpVal("PID" ,ValAN ,sizeof(ValAN)/sizeof(SVAL));    
    AddTpVal("PID" ,ValPID,sizeof(ValPID)/sizeof(SVAL));
    //Load algobloks
    algbs = new TVirtAlgb(NameCfgF);
}



TController *TVirtual::ContrAttach(string name, string t_bd, string n_bd, string n_tb)
{
    return( new TVContr(this,name,t_bd, n_bd, n_tb,this));    
}

//======================================================================
//==== TVContr 
//======================================================================

TVContr::TVContr( ::TTipController *tcntr, string name_c,string _t_bd, string _n_bd, string _n_tb, ::TConfigElem *cfgelem) :
	::TController(tcntr,name_c,_t_bd,_n_bd,_n_tb,cfgelem), run_st(true), endrun(true)
{

}

TVContr::~TVContr()
{
    //Stop();
    //Free();
}

void TVContr::Load( )
{
    TController::Load( );
}

void TVContr::Save( )
{
    TController::Save( );
}

void TVContr::Free( )
{
    TController::Free();
}

void TVContr::Start( )
{   
    pthread_attr_t      pthr_attr;
    struct sched_param  prior;
    //---- Attach parameter algoblock ----
    vector<string> list_p;
  
    List(list_p);
    for(unsigned i_prm=0; i_prm < list_p.size(); i_prm++)
	( (TVPrm &)at(NameToHd(list_p[i_prm])) ).Load(  );
    //------------------------------------    
    pthread_attr_init(&pthr_attr);
    if(SYS->UserName() == "root")
    {
	prior.__sched_priority=10;
	pthread_attr_setschedpolicy(&pthr_attr,SCHED_FIFO);
	pthread_attr_setschedparam(&pthr_attr,&prior);
#ifdef OSC_DEBUG
	Mess->put(1,"Start into realtime mode!");
#endif
    }
    else pthread_attr_setschedpolicy(&pthr_attr,SCHED_OTHER);
    pthread_create(&pthr_tsk,&pthr_attr,Task,this);
    pthread_attr_destroy(&pthr_attr);
    sleep(1);
    if(run_st == false) throw TError("%s: Controller %s no starting!",NAME_MODUL,Name().c_str());
    
    TController::Start();
}

void TVContr::Stop( )
{  
    if(run_st == true)
    {
	endrun = true;
	sleep(1);
	if(run_st == true) throw TError("%s: Controller %s no stoping!",NAME_MODUL,Name().c_str());
    }
    TController::Stop();    
} 

void *TVContr::Task(void *contr)
{
    int    i_sync=0;
    
    struct itimerval mytim;             //Interval timer
    long   time_t1,time_t2,cnt_lost=0;
    int    frq = sysconf(_SC_CLK_TCK);  //Count of system timer n/sek
    TVContr *cntr = (TVContr *)contr;

    try
    {
	cntr->period  = cntr->cf_Get_I("PERIOD");
	if(cntr->period == 0) return(NULL);
	cntr->d_sync = cntr->cf_Get_I("PER_S")/cntr->period;
	if(cntr->d_sync == 0) cntr->d_sync = 1;
	cntr->cf_Set_I("PER_S",cntr->d_sync*cntr->period);
	cntr->iterate = cntr->cf_Get_I("ITER");    
	if(cntr->iterate <= 0) { cntr->iterate = 1; cntr->cf_Set_R("ITER",(double)cntr->iterate); } 

	mytim.it_interval.tv_sec = 0; mytim.it_interval.tv_usec = cntr->period*1000;
	mytim.it_value.tv_sec    = 0; mytim.it_value.tv_usec    = cntr->period*1000;
    
	signal(SIGALRM,wakeup);
	setitimer(ITIMER_REAL,&mytim,NULL);
    
	cntr->run_st = true;  cntr->endrun = false;
	time_t1=times(NULL);
    
    
	while(cntr->endrun == false)
	{
	    pause();
#ifdef OSC_DEBUG
	    //check hard cycle
	    time_t2=times(0);
	    if( time_t2 != (time_t1+cntr->period*frq/1000) )
	    {
		cnt_lost+=time_t2-(time_t1+cntr->period*frq/1000);
		Mess->put(3,"Lost ticks %s = %d - %d (%d)\n",cntr->Name().c_str(),time_t2,time_t1+cntr->period*frq/1000,cnt_lost);
	    }
	    time_t1=time_t2;	
	    //----------------
#endif
	    if((++i_sync) >= cntr->d_sync) { i_sync=0; cntr->Sync(); }
	    for(int i_c=0; i_c < cntr->iterate; i_c++)
		for(unsigned i_p=0; i_p < cntr->cntr_prm.size(); i_p++)
		    ((TVPrm *)cntr->cntr_prm[i_p])->Calc();
	}
    } catch(...) { }
    cntr->run_st = false;

    return(NULL);
}

void TVContr::Sync()
{
    for(unsigned i_p=0; i_p < cntr_prm.size(); i_p++)
	((TVPrm *)cntr_prm[i_p])->Sync();
}

TParamContr *TVContr::ParamAttach(int type)
{
    return(new TVPrm(this,&Owner().at_TpPrm(type)));
}

//======================================================================
//==== TVPrm 
//====================================================================== 

TVPrm::TVPrm(TController *contr, TTipParam *tp_prm ) : TParamContr(contr,tp_prm), pid(NULL)
{

}

void TVPrm::UpdateVAL( )
{
    TParamContr::UpdateVAL();
    
    hd_y  = vl_Elem().vle_NameToId("VAL");
    if( vl_Elem().vle_Type(hd_y)&VAL_T_REAL )
    {
	STime tm = {0,0}; 
    	y_min = _vl_GetR(hd_y,tm,V_MIN);
	y_max = _vl_GetR(hd_y,tm,V_MAX);
    }
    else y_max = y_min = 0.0;

    if(vl_Elem().vle_Name() == "PID")
    {	
    	pid = new SPID;
    	pid->hd_out  = vl_Elem().vle_NameToId("OUT");
    	pid->hd_sp   = vl_Elem().vle_NameToId("SP");
    	pid->hd_stat = vl_Elem().vle_NameToId("STAT");	
    }
}    

TVPrm::~TVPrm( )
{    
    if(pid) delete pid;
}

void TVPrm::vl_Set( int id_elem )
{
    Mess->put(1,"Comand to direct set value of element!");
}
void TVPrm::vl_Get( int id_elem )
{
    Mess->put(1,"Comand to direct get value of element!");
}

void TVPrm::Load( )
{
    SAlgb *algb = NULL;
    try
    {
	algb = ((TVirtual &)((TVContr &)Owner()).Owner()).AlgbS()->GetAlg(Name());
    }
    catch(...) 
    {
    	form = -1;
	return;
    }
    form = algb->tp_alg;


    for(unsigned i_x=0; i_x < algb->io.size(); i_x++)
    {
	if( i_x >= x_id.size() ) x_id.insert(x_id.begin()+i_x);
	if( i_x >= x.size() )    x.insert(x.begin()+i_x);

	try
	{
	    x_id[i_x].hd_prm   = Owner().NameToHd(algb->io[i_x]);
	    x_id[i_x].internal = true;
	}
	catch(TError)
	{   
	    try
	    {
    		x_id[i_x].hd_prm   = Owner().Owner().Owner().Owner().Param().NameToHd(algb->io[i_x]);
		x_id[i_x].internal = false;
	    }
	    catch(TError) { x_id[i_x].hd_prm = -1; x[i_x] = 1E+10; }
	}
    }
    for(unsigned i_k = 0;i_k < algb->kf.size(); i_k++)
    {
	if( i_k >= k.size() ) k.insert(k.begin()+i_k);
	k[i_k] = algb->kf[i_k];
    }
}


inline void TVPrm::Y(float val)
{
    STime tm = {0,0};
    _vl_SetR(hd_y,val,tm);
}

inline float TVPrm::Y()
{ 
    STime tm = {0,0};
    return(_vl_GetR(hd_y,tm));
}

inline void TVPrm::X(unsigned id ,float val)
{ 
    float val_t;

    if(x_id[id].hd_prm < 0) return;
    if(x_id[id].internal)  ((TVPrm &)Owner().at(x_id[id].hd_prm)).Y(val);
    else
    {
	if(x_id[id].max == x_id[id].min) val_t = val;
	else val_t = (val > x_id[id].max)?x_id[id].max:(val < x_id[id].min)?x_id[id].min:val;
	if(x[id] != val_t) { x_id[id].sync = true; x[id] = val_t; }
    }
}

inline float TVPrm::X(unsigned id)
{
    if(x_id[id].hd_prm < 0) return(0.0);
    if(x_id[id].internal)   return( ((TVPrm &)Owner().at(x_id[id].hd_prm)).Y() );
    return(x[id]);
}

void TVPrm::Sync()
{    
    STime tm = {0,0};
    TKernel &Kern = Owner().Owner().Owner().Owner();
    //Syncing no internal io to TValue
    for(unsigned i_x = 0; i_x < x_id.size(); i_x++)
	if(!x_id[i_x].internal && x_id[i_x].hd_prm >= 0 )
	{
	    try
	    {
		int hd_v = Kern.Param()[x_id[i_x].hd_prm].at().vl_Elem().vle_NameToId("VAL");
		if( !Kern.Param()[x_id[i_x].hd_prm].at().vl_Valid(hd_v) ) continue;
		if(	x_id[i_x].sync )
		{
		    Kern.Param()[x_id[i_x].hd_prm].at().vl_SetR(hd_v,x[i_x],tm);
		    x_id[i_x].sync = false;
		}
		else x[i_x] = Kern.Param()[x_id[i_x].hd_prm].at().vl_GetR(hd_v,tm);
	    }catch(TError) { x_id[i_x].hd_prm = -1; }
	}    
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
    Mess->put(1,"%d: Furmule id= %d no avoid!",form, ((TVirtual &)((TVContr &)Owner()).Owner()).AlgbS()->GetFrm(form)->tip);

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
//    	Mess->put(1,"TEST %d!!!",*off);
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
    STime  tm;

    if(!pid) return(1E+10);
    
    int    period      = ((TVContr &)Owner()).Period();
    int    HZ          = 1000*sysconf(_SC_CLK_TCK);
    int    cnt_in_cycl = ((TVContr &)Owner()).Iterate();
    float  sp          = _vl_GetR(pid->hd_sp,tm);
    float  out         = _vl_GetR(pid->hd_out,tm);
    char   stat        = (char)_vl_GetI(pid->hd_stat,tm);
    
    k1 = 100 * X(1)/( (labs((long)y_max) > labs((long)y_min))?labs((long)y_max):labs((long)y_min) );
    k2 = 100 * X(2)/( (labs((long)y_max) > labs((long)y_min))?labs((long)y_max):labs((long)y_min) );
    k3 = 100 * X(3)/( (labs((long)y_max) > labs((long)y_min))?labs((long)y_max):labs((long)y_min) );
    k4 = 100 * X(4)/( (labs((long)y_max) > labs((long)y_min))?labs((long)y_max):labs((long)y_min) );
    
    if(x_id[5].hd_prm >= 0)  sp = 100.*(X(5)-y_min)/(y_max-y_min);

    if(stat != R_MAN && stat != R_AUTO && stat != R_CAS) stat = R_MAN;
    if(stat == R_CAS && x_id[6].hd_prm < 0) stat = R_AUTO;
    //if(stat == R_CAS) sp =    c_ptr[ALGB->inp[6].nc].a_ptr[ALGB->inp[6].nz].out_cod;
    in = 100. * (X(0)-y_min)/(y_max-y_min);

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

    if( x_id[7].hd_prm >= 0 ) X(7,out);

    _vl_SetR(pid->hd_sp,sp,tm);
    _vl_SetR(pid->hd_out,out,tm);
    _vl_SetI(pid->hd_stat,stat,tm);

    if( x_id[0].hd_prm < 0 )  return 0.;
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
	if(i_frm == frm_s.size()) frm_s.push_back();
       	read(fh,&len_1,1);
	if(len_1)
	{
	    read(fh,buf,len_1); buf[len_1]=0;
	    frm_s[i_frm].name = buf;
	    Mess->SconvIn("CP866",frm_s[i_frm].name);
	}
       
	read(fh,&frm_s[i_frm].tip,1);
	read(fh,&frm_s[i_frm].n_inp,1); 
	read(fh,&frm_s[i_frm].n_koef,1);
	if(frm_s[i_frm].n_inp)
   	    for(unsigned i_inp=0;i_inp < frm_s[i_frm].n_inp;i_inp++)
   	    {
		if( i_inp == frm_s[i_frm].name_inp.size() ) 
		    frm_s[i_frm].name_inp.push_back();
       		read(fh,&len_1,1);
	       	read(fh,buf,len_1); buf[len_1] = 0;
	       	frm_s[i_frm].name_inp[i_inp] = buf;
	    }
	if(frm_s[i_frm].n_koef)
	    for(unsigned i_kf=0;i_kf < frm_s[i_frm].n_koef;i_kf++)
	    {
    		if( i_kf == frm_s[i_frm].name_kf.size() ) 
		    frm_s[i_frm].name_kf.push_back();
		read(fh,&len_1,1);
		read(fh,buf,len_1); buf[len_1] = 0;
		frm_s[i_frm].name_kf[i_kf] = buf;
	    }
	read(fh,&len_2,2);
    	read(fh,buf,len_2); buf[len_2] = 0;
      	frm_s[i_frm].formul = buf;
	if(frm_s[i_frm].tip==5)
    	{
	    if(!frm_s[i_frm].formul.size()) continue;
       	    char *str1 = (char *)frm_s[i_frm].formul.c_str();
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
            frm_s[i_frm].form_e  = strdup(buf);
	    frm_s[i_frm].l_frm_e = i_n;
	}
    }
    lseek(fh,ofs_alg,SEEK_SET);
    read(fh,&k_alg,2);
    for(unsigned i_alg=0; i_alg < k_alg; i_alg++)
    {
	if(i_alg == algb_s.size()) algb_s.push_back();
	
    	read(fh,buf,9); 
	buf[9] = 0;
	for(int i=8; i >= 0; i--) 
	    if(buf[i]==' ' || buf[i]== 0) buf[i]=0; 
	    else break; 
	algb_s[i_alg].name = buf;
	Mess->SconvIn("CP866",algb_s[i_alg].name);

	read(fh,&len_1,1); 
	read(fh,buf,len_1); buf[len_1]=0;
	algb_s[i_alg].descr = buf;
	Mess->SconvIn("CP866",algb_s[i_alg].descr);
	
	read(fh,&tp_alg,2);
	algb_s[i_alg].tp_alg = tp_alg;
	
	unsigned i_n = frm_s[tp_alg].n_inp;
	if(i_n)
	    for(unsigned i_x=0;i_x < i_n;i_x++)
	    { 
		if( i_x == algb_s[i_alg].io.size() ) algb_s[i_alg].io.push_back();
		
		read(fh,buf,9); buf[9] = 0;
		for(int i=8; i >= 0; i--) 
		    if(buf[i]==' ' || buf[i]== 0) buf[i]=0; 
		    else break;
		algb_s[i_alg].io[i_x] = buf;
		Mess->SconvIn("CP866",algb_s[i_alg].io[i_x]);
	    }
	i_n = frm_s[tp_alg].n_koef;
	if(i_n)
	    for(unsigned i_k = 0;i_k < i_n; i_k++)
	    {
		if( i_k == algb_s[i_alg].kf.size() ) algb_s[i_alg].kf.push_back();
		read(fh,&(algb_s[i_alg].kf[i_k]),4);
	    }
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
    while( algb_s.size() ) algb_s.erase(algb_s.begin());
    while( frm_s.size() )
    {
	if(frm_s[0].tip==5) free(frm_s[0].form_e);
	frm_s.erase(frm_s.begin());
    }
}


SAlgb *TVirtAlgb::GetAlg(string name)
{
    for(unsigned i_alg = 0; i_alg < algb_s.size(); i_alg++)
	if(algb_s[i_alg].name == name) return(&algb_s[i_alg]);
    throw TError("%s: Algoblok %s no avoid!",NAME_MODUL,name.c_str());
}

SFrm *TVirtAlgb::GetFrm(unsigned id)
{
    if(id < frm_s.size()) return(&frm_s[id]);
    throw TError("%s: Formule %d no avoid!",NAME_MODUL,id);
}

