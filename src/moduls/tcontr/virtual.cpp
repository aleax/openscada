/* Test Modul
** ==============================================================
*/

#include <getopt.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/times.h>
#include <signal.h>

#include "../../tapplication.h"
#include "../../tmessage.h"
#include "../../tconfig.h"
#include "../../tvalue.h"
#include "../../tcontroller.h"
#include "../../tparamcontr.h"
#include "virtual.h"

//============ Modul info! =====================================================
#define NAME_MODUL  "virtual_v1"
#define NAME_TYPE   "Controller"
#define VERSION     "0.1"
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

extern "C" TModule *attach( char *FName, int n_mod );

SExpFunc TVirtual::ExpFuncLc[] = 
{
    {"ContrAttach" ,( void ( TModule::* )(  ) ) &TVirtual::ContrAttach ,"TController *ContrAttach(string name, string bd);",
     "Attach new controller",10,0}
};
			      

//==== Extended field's elements  ====

SRecStr TVirtual::RStr[] =
{
    {""}, {"Virtual controller"}, {"VRT_AN"}, {"VRT_DG"}, {"VRT_BL"}, {"Analog parameter"}
};

SRecNumb TVirtual::RNumb[] =
{
    {0., 10000., 1000., 0, 0}, {0.,    99.,    1., 0, 0}, {0.,     0.,    0., 2, 3},
    {0.,     0.,  100., 2, 3}, {0.,    50.,   0.5, 2, 3}
};

SRecSel TVirtual::RSel[] =
{
    {"0","Linear;Square","0;1"},
    {"0","Average;Integrate;Counter","0;1;2"},
    {"A_IN","Input;Output;Regulator","A_IN;A_OUT;PID"},
    {"D_IN","Input;Output","D_IN;D_OUT"}
};

//==== Desribe controler's bd fields ====

SElem TVirtual::elem[] =
{
    {"NAME"    ,"Short name of controller."        ,CFGTP_STRING,20,"","",&RStr[0],NULL     ,NULL},
    {"LNAME"   ,"Description of controller."       ,CFGTP_STRING,50,"","",&RStr[1],NULL     ,NULL},
    {PRM_B_AN  ,"Name BD for ANALOG parameteres."  ,CFGTP_STRING,30,"","",&RStr[2],NULL     ,NULL},
    {PRM_B_DG  ,"Name BD for DIGIT parameteres."   ,CFGTP_STRING,30,"","",&RStr[3],NULL     ,NULL},
    {PRM_B_BLCK,"Name BD for BLOCK parameteres."   ,CFGTP_STRING,30,"","",&RStr[4],NULL     ,NULL},
    {"PERIOD"  ,"Pooled period (ms)."              ,CFGTP_NUMBER,5 ,"","",NULL    ,&RNumb[0],NULL},
    {"ITER"    ,"Number of a iterations at period.",CFGTP_NUMBER,2 ,"","",NULL    ,&RNumb[1],NULL}
};

//==== Desribe ANALOG parameter's bd fields ====
SElem TVirtual::ElemAN[] =
{
    {"SHIFR"  ,"Short name of parameter (TAGG)."  ,CFGTP_STRING,20,"","",&RStr[0],NULL     ,NULL},
    {"NAME"   ,"Description of parameter"         ,CFGTP_STRING,50,"","",&RStr[5],NULL     ,NULL},
    {"ED"     ,"Value of measurement"             ,CFGTP_STRING,10,"","",&RStr[0],NULL     ,NULL},
    {"SCALE"  ,"Scale"                            ,CFGTP_SELECT,1 ,"","",NULL    ,NULL     ,&RSel[0]},
    {"TIPO"   ,"Type of processing"               ,CFGTP_SELECT,1 ,"","",NULL    ,NULL     ,&RSel[1]},
    {"MIN"    ,"Lower scale border"               ,CFGTP_NUMBER,10,"","",NULL    ,&RNumb[2],NULL},
    {"MAX"    ,"Upper scale border"               ,CFGTP_NUMBER,10,"","",NULL    ,&RNumb[3],NULL},
    {"NTG"    ,"Lower technically scale border"   ,CFGTP_NUMBER,10,"","",NULL    ,&RNumb[2],NULL},
    {"VTG"    ,"Upper technically scale border"   ,CFGTP_NUMBER,10,"","",NULL    ,&RNumb[2],NULL},
    {"NAG"    ,"Lower alarm scale border"         ,CFGTP_NUMBER,10,"","",NULL    ,&RNumb[2],NULL},
    {"VAG"    ,"Upper alarm scale border"         ,CFGTP_NUMBER,10,"","",NULL    ,&RNumb[2],NULL},
    {"Z_GR"   ,"Non-sensitive zone"               ,CFGTP_NUMBER,4 ,"","",NULL    ,&RNumb[4],NULL},
    {"TYPE"   ,"Parameter type"                   ,CFGTP_SELECT,6 ,"","",NULL    ,NULL     ,&RSel[2]}
};

//==== Desribe DIGIT parameter's bd fields ====
SElem TVirtual::ElemDG[] =
{
    {"SHIFR"  ,"Short name of parameter (TAGG)."  ,CFGTP_STRING,20,"","",&RStr[0],NULL     ,NULL},
    {"NAME"   ,"Description of parameter"         ,CFGTP_STRING,50,"","",&RStr[5],NULL     ,NULL},
    {"TYPE"   ,"Parameter type"                   ,CFGTP_SELECT,6 ,"","",NULL    ,NULL     ,&RSel[3]}
};

//==== Desribe BLOCK parameter's bd fields ====
SElem TVirtual::ElemBL[] =
{
    {"SHIFR"  ,"Short name of parameter (TAGG)."  ,CFGTP_STRING,20,"","",&RStr[0],NULL     ,NULL},
    {"NAME"   ,"Description of parameter"         ,CFGTP_STRING,50,"","",&RStr[5],NULL     ,NULL}
};

//=============================================
//==== Describe ANALOG param struct ===========
SVAL TVirtual::ValAN[] =
{
    {"VAL" ,"Value"      ,"Value analog parameter" ,VAL_T_REAL,VAL_S_GENER,VAL_M_OFTN,VAL_D_BD          ,0000,0.0, 0.0}, 
    //{"MIN" ,"Min value"  ,"Value low border"       ,VAL_T_REAL,VAL_S_UTIL ,VAL_M_SELD,VAL_D_BD|VAL_D_VBD,0644,-1.0E10, 1.0E10}, 
    //{"MAX" ,"Max value"  ,"Value up border"        ,VAL_T_REAL,VAL_S_UTIL ,VAL_M_SELD,VAL_D_BD|VAL_D_VBD,0644,-1.0E10, 1.0E10}, 
    {"NTG" ,"Low tech"   ,"Value low tech border"  ,VAL_T_REAL,VAL_S_UTIL ,VAL_M_SELD,VAL_D_BD|VAL_D_VBD,0644,0.0, 0.0}, 
    {"VTG" ,"Up tech"    ,"Value up tech border"   ,VAL_T_REAL,VAL_S_UTIL ,VAL_M_SELD,VAL_D_BD|VAL_D_VBD,0644,0.0, 0.0}, 
    {"NAG" ,"Low alarm"  ,"Value low alarm border" ,VAL_T_REAL,VAL_S_UTIL ,VAL_M_SELD,VAL_D_BD|VAL_D_VBD,0644,0.0, 0.0}, 
    {"VAG" ,"Up alarm"   ,"Value up alarm border"  ,VAL_T_REAL,VAL_S_UTIL ,VAL_M_SELD,VAL_D_BD|VAL_D_VBD,0644,0.0, 0.0}, 
    {"Z_GR","Non-sensit" ,"Non-sensitive zone"     ,VAL_T_REAL,VAL_S_UTIL ,VAL_M_SELD,VAL_D_VBD         ,0644,0.0,50.0} 
};

//==== Describe DIGIT param struct ===========
SVAL TVirtual::ValDG[] =
{
    {"VAL" ,"Value"     ,"Value digital parameter",VAL_T_BOOL,VAL_S_GENER,VAL_M_SELD,VAL_D_BD,0000}
};
//==== PID regulator ===========
SVAL TVirtual::ValPID[] =
{
    {"IN"   ,"Input"    ,"Input of regulator"                 ,VAL_T_REAL,VAL_S_GENER,VAL_M_OFTN,VAL_D_BD   ,0000,  0.0,   0.0},
    {"OUT"  ,"Output"   ,"Output of regulator"                ,VAL_T_REAL,VAL_S_GENER,VAL_M_OFTN,VAL_D_BD   ,0000,  0.0, 100.0},
    {"SP"   ,"SetPoint" ,"Setpoint of regulator"              ,VAL_T_REAL,VAL_S_GENER,VAL_M_OFTN,VAL_D_BD   ,0000,  0.0,   0.0},
    {"Kp"   ,"Gain"     ,"Koefficient of proportion"          ,VAL_T_REAL,VAL_S_UTIL ,VAL_M_SELD,VAL_D_FIX  ,0644,-20.0,  20.0},
    {"Ti"   ,"Integr"   ,"Time of integrated (cek)"           ,VAL_T_REAL,VAL_S_UTIL ,VAL_M_SELD,VAL_D_FIX  ,0644,  0.0,1000.0},
    {"Td"   ,"Diferent" ,"Time of diff (cek)"                 ,VAL_T_REAL,VAL_S_UTIL ,VAL_M_SELD,VAL_D_FIX  ,0644,  0.0,1000.0},
    {"Tf"   ,"Filter"   ,"Time of lag (cek)"                  ,VAL_T_REAL,VAL_S_UTIL ,VAL_M_SELD,VAL_D_FIX  ,0644,  0.0,1000.0},
    {"K1"   ,"K input 1","Koefficient scale of addon input 1" ,VAL_T_REAL,VAL_S_UTIL ,VAL_M_SELD,VAL_D_FIX  ,0644,-20.0,  20.0},
    {"K2"   ,"K input 2","Koefficient scale of addon input 2" ,VAL_T_REAL,VAL_S_UTIL ,VAL_M_SELD,VAL_D_FIX  ,0644,-20.0,  20.0},
    {"K3"   ,"K input 3","Koefficient scale of addon input 3" ,VAL_T_REAL,VAL_S_UTIL ,VAL_M_SELD,VAL_D_FIX  ,0644,-20.0,  20.0},
    {"K4"   ,"K input 4","Koefficient scale of addon input 4" ,VAL_T_REAL,VAL_S_UTIL ,VAL_M_SELD,VAL_D_FIX  ,0644,-20.0,  20.0},
    {"STAT" ,"Stat mode","Stat of regulator (Manual,Auto)"    ,VAL_T_BOOL,VAL_S_GENER,VAL_M_OFTN,VAL_D_BD   ,0000},
    {"CASC" ,"Cascade mode","Cascade mode of regulator"       ,VAL_T_BOOL,VAL_S_GENER,VAL_M_OFTN,VAL_D_BD   ,0000}
};


TVirtual::TVirtual(char *name) : TModule()
{
    NameModul = NAME_MODUL;
    NameType  = NAME_TYPE;
    Vers      = VERSION;
    Autors    = AUTORS;
    DescrMod  = DESCRIPTION;
    License   = LICENSE;
    FileName  = strdup(name);
    
    ExpFunc   = (SExpFunc *)ExpFuncLc;
    NExpFunc  = sizeof(ExpFuncLc)/sizeof(SExpFunc);
}

TVirtual::~TVirtual()
{
    free(FileName);	
}

TModule *attach( char *FName, int n_mod )
{
    TVirtual *self_addr;
    if(n_mod==0) self_addr = new TVirtual( FName );
    else         self_addr = NULL;
    return ( self_addr );
}

int TVirtual::info( const string & name, string & info )
{
    info.erase();
    TModule::info(name,info);
    
    return(0);
}

void TVirtual::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "==================== %s options =================================\n"
    "\n",NAME_MODUL);
}

void TVirtual::CheckCommandLine(  )
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
	next_opt=getopt_long(App->argc,(char * const *)App->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': pr_opt_descr(stdout); break;
	    case -1 : break;
	}
    } while(next_opt != -1);
}

int TVirtual::init( void *param )
{
    TContr  = (TTipController *)param;
    TContr->LoadElCtr(elem,sizeof(elem)/sizeof(SElem));
    //Add parameter types
    TContr->AddTpParm(PRM_ANALOG,PRM_B_AN  ,"Analog parameters");
    TContr->AddTpParm(PRM_DIGIT ,PRM_B_DG  ,"Digit parameters");
    TContr->AddTpParm(PRM_BLOCK ,PRM_B_BLCK,"Block parameter (algoblock)");
    //Load views for parameter's types
    TContr->LoadElParm(PRM_ANALOG,ElemAN,sizeof(ElemAN)/sizeof(SElem));
    TContr->LoadElParm(PRM_DIGIT ,ElemDG,sizeof(ElemDG)/sizeof(SElem));
    TContr->LoadElParm(PRM_BLOCK ,ElemBL,sizeof(ElemBL)/sizeof(SElem));
    //Add types of value
    TContr->AddValType("A_IN",ValAN ,sizeof(ValAN)/sizeof(SVAL));
    TContr->AddValType("D_IN",ValDG ,sizeof(ValDG)/sizeof(SVAL));
    TContr->AddValType("PID" ,ValPID,sizeof(ValPID)/sizeof(SVAL));

    CheckCommandLine();
    TModule::init( param );

    return(0);
}

TController *TVirtual::ContrAttach(string name, string bd)
{
    return( new TContrVirt(TContr,name,bd,TContr->ConfigElem()));    
}

//======================================================================
//==== TContrVirt 
//======================================================================
TContrVirt::TContrVirt(TTipController *tcntr, string name_c, string bd_c, TConfigElem *cfgelem) : 
	TController(tcntr,name_c,bd_c,cfgelem), run_st(true), endrun(true)
{

}

TContrVirt::~TContrVirt()
{
    Stop();
    Free();
}

int TContrVirt::Load( )
{
    TController::Load();
    
    return(0);    
}

int TContrVirt::Save( )
{
    TController::Save();
    
    return(0);
}

int TContrVirt::Free( )
{
    TController::Free();
    
    return(0);
}

int TContrVirt::Start( )
{    
    pthread_attr_t      pthr_attr;
    struct sched_param  prior;

    pthread_attr_init(&pthr_attr);
    if(App->UserName() == "root")
    {
	prior.__sched_priority=10;
	pthread_attr_setschedpolicy(&pthr_attr,SCHED_FIFO);
	pthread_attr_setschedparam(&pthr_attr,&prior);
#ifdef debug
	App->Mess->put(1,"Start into realtime mode!");
#endif
    }
    else 
	pthread_attr_setschedpolicy(&pthr_attr,SCHED_OTHER);
    pthread_create(&pthr_tsk,&pthr_attr,Task,this);
    pthread_attr_destroy(&pthr_attr);
    sleep(1);
    if(run_st == false) return(-1);

    TController::Start();
    
    return(0);
}

int TContrVirt::Stop( )
{
    if(run_st == true)
    {
	endrun = true;
	sleep(1);
	if(run_st == true) return(-1);
    }

    TController::Stop();    

    return(0);
} 

void *TContrVirt::Task(void *contr)
{
    double val;
    struct itimerval mytim;             //Interval timer
    int    count=0;
    long   time_t1,time_t2,cnt_lost=0;
    int    frq = sysconf(_SC_CLK_TCK);  //Count of system timer n/sek
    TContrVirt *cntr = (TContrVirt *)contr;


    if(cntr->GetVal("PERIOD",val) < 0) return(NULL);
    cntr->period = (int)val;
    if(cntr->period == 0)              return(NULL); 
    if(cntr->GetVal("ITER",val) < 0)   return(NULL);
    if(val <= 0.) { val = 1.; cntr->SetVal("ITER",val); } 
    cntr->iterate = (int)val;


    mytim.it_interval.tv_sec = 0; mytim.it_interval.tv_usec = cntr->period*1000;
    mytim.it_value.tv_sec    = 0; mytim.it_value.tv_usec    = cntr->period*1000;
    
    signal(SIGALRM,wakeup);
    setitimer(ITIMER_REAL,&mytim,NULL);
    
    cntr->run_st = true;  cntr->endrun = false;
    time_t1=times(NULL);
    while(cntr->endrun == false)
    {
	pause();
#ifdef debug
	//check hard cycle
	time_t2=times(0);
	if( time_t2 != (time_t1+cntr->period*frq/1000) )
	{
	    cnt_lost+=time_t2-(time_t1+cntr->period*frq/1000);
	    App->Mess->put(3,"Lost ticks %s = %d - %d (%d)\n",cntr->Name().c_str(),time_t2,time_t1+cntr->period*frq/1000,cnt_lost);
    	}
	time_t1=time_t2;	
	//----------------
#endif
    }
    cntr->run_st = false;
}

