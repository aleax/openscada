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
#include "../../tkernel.h"
#include "../../tmessage.h"
#include "../../tconfig.h"
#include "../../tvalue.h"
#include "../../tcontroller.h"
#include "../../ttipcontroller.h"
#include "../../tcontrollers.h"
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

TVirtual::TVirtual(char *name) : NameCfgF("./alg.cfg")
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
    return static_cast< TModule *>( self_addr );
}

void TVirtual::info( const string & name, string & info )
{
    info.erase();
    TModule::info(name,info);
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


void TVirtual::CheckCommandLine( char **argv, int argc )
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
	next_opt=getopt_long(argc,argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': pr_opt_descr(stdout); break;
	    case 'c': NameCfgF = optarg;    break;
	    case -1 : break;
	}
    } while(next_opt != -1);
}

void TVirtual::UpdateOpt( )
{
    try{ NameCfgF = owner->owner->GetOpt(NAME_MODUL,"config"); } catch(...){  }
}

void TVirtual::init( void *param )
{    
    LoadElCtr(elem,sizeof(elem)/sizeof(SCfgFld));
    //Add parameter types
    AddTpParm(PRM_ANALOG,PRM_B_AN  ,"Analog parameters");
    AddTpParm(PRM_DIGIT ,PRM_B_DG  ,"Digit parameters");
    AddTpParm(PRM_BLOCK ,PRM_B_BLCK,"Block parameter (algoblock)");
    //Load views for parameter's types
    LoadElParm(PRM_ANALOG,ElemAN,sizeof(ElemAN)/sizeof(SCfgFld));
    LoadElParm(PRM_DIGIT ,ElemDG,sizeof(ElemDG)/sizeof(SCfgFld));
    LoadElParm(PRM_BLOCK ,ElemBL,sizeof(ElemBL)/sizeof(SCfgFld));
    //Add types of value
    AddValType("A_IN",ValAN ,sizeof(ValAN)/sizeof(SVAL));
    AddValType("D_IN",ValDG ,sizeof(ValDG)/sizeof(SVAL));
    AddValType("PID" ,ValAN ,sizeof(ValAN)/sizeof(SVAL));    
    AddValType("PID" ,ValPID,sizeof(ValPID)/sizeof(SVAL));
    //Load algobloks
    LoadAlg(NameCfgF);

    TModule::init( param );
}



TController *TVirtual::ContrAttach(string name, string t_bd, string n_bd, string n_tb)
{
    return( new TContrVirt(this,name,t_bd, n_bd, n_tb,ConfigElem()));    
}

void TVirtual::LoadAlg( string NameCfgF )
{
    int        fh;
    dword      ofs_alg;
    word       form_am;
    byte       len_1;
    char       buf[256];
    
    if((fh=open(NameCfgF.c_str(),O_RDONLY)) == -1)
    { 
	Mess->put(3,"%s: Open file \"%s\" error!",NAME_MODUL,NameCfgF.c_str());
	return;
    }

    read(fh,&ofs_alg,4); read(fh,&form_am,2);
    for(int i_frm = 0; i_frm < form_am; i_frm++)
    {
	formuls.insert(formuls.begin()+i_frm);
       	read(fh,&len_1,1);
	if(len_1)
	{
	    read(fh,buf,len_1); buf[len_1]=0;
	    formuls[i_frm].name = buf;
	    Mess->SconvIn("CP866",formuls[i_frm].name);
	}
       
	read(fh,&formuls[i_frm].tip,1);
	read(fh,&formuls[i_frm].n_inp,1); read(fh,&formuls[i_frm].n_koef,1);
	if(formuls[i_frm].n_inp)
	{
	    formuls[i_frm].name_inp=(char **)calloc(formuls[i_frm].n_inp, sizeof(char **));
   	    for(int i_inp=0;i_inp < formuls[i_frm].n_inp;i_inp++)
   	    {
       		read(fh,&len_1,1);
	   	formuls[i_frm].name_inp[i_inp]=(char *)calloc(len_1+1,1);
	       	read(fh,formuls[i_frm].name_inp[i_inp],len_1); 
		formuls[i_frm].name_inp[i_inp][len_1]=0;
	    }
	}
	if(formuls[i_frm].n_koef)
    	{
       	    formuls[i_frm].name_kf=(char **)calloc(formuls[i_frm].n_koef,sizeof(char **));
	    for(int i_kf=0;i_kf < formuls[i_frm].n_koef;i_kf++)
	    {
		read(fh,&len_1,1);
		formuls[i_frm].name_kf[i_kf]=(char *)calloc(len_1+1,1);
		read(fh,formuls[i_frm].name_kf[i_kf],len_1); 
		formuls[i_frm].name_kf[i_kf][len_1]=0;
	    }
	}

	read(fh,&formuls[i_frm].l_frm,2);
  	formuls[i_frm].formul = (char *)calloc(formuls[i_frm].l_frm+1,1);
    	read(fh,formuls[i_frm].formul,formuls[i_frm].l_frm);
      	formuls[i_frm].formul[formuls[i_frm].l_frm]=0;
	if(formuls[i_frm].tip==5)
    	{
       	    char *str1 = formuls[i_frm].formul;
	    if(!formuls[i_frm].l_frm) continue;
	    char *str2 = formuls[i_frm].form_e = (char *)calloc(formuls[i_frm].l_frm,1);
   	    int i_c, i_n;
	    for( i_c = 0, i_n=0;i_c < formuls[i_frm].l_frm;i_c++)
   	    {
       		if(*(word *)(str1+i_c) == *(word *)"//")
	   	{ 
		    for( ; (i_c < formuls[i_frm].l_frm && str1[i_c]!=0x0D);i_c++) ; 
		    continue;
		}
		if(str1[i_c] == '"' )
		{
		    str2[i_n++] = str1[i_c++];
		    for( ; i_c < formuls[i_frm].l_frm;i_c++,i_n++)
		    {
	      		str2[i_n] = str1[i_c];
		    	if(str1[i_c] == '"') {str2[i_n++] = str1[i_c]; break;}
		    }
		    continue;
		}
		if(str1[i_c] == ' ' || str1[i_c] == '\x0D' || str1[i_c] == '\x0A') continue;
		if(str1[i_c] == 'K' || str1[i_c] == 'X')
    		{
	 	    str2[i_n] = str1[i_c];
     		    i_c++; i_n++;
     		    str2[i_n] = str1[i_c];
     		    i_n++;
     		    continue;
	 	}
		if(str1[i_c] == 'C')
		{
		    str2[i_n] = str1[i_c];
		    i_c++; i_n++;
		    *(float*)(str2+i_n) = *(float*)(str1+i_c);
		    i_c+=3; i_n+=4;
		    continue;
		}
		str2[i_n]=str1[i_c]; 
		i_n++;
	    }
	    if(i_n != formuls[i_frm].l_frm)
            formuls[i_frm].form_e = (char *)realloc(formuls[i_frm].form_e,i_n);
   	    formuls[i_frm].l_frm1 = i_n;
	}
    }
    
    close(fh); 
}

//======================================================================
//==== TContrVirt 
//======================================================================

TContrVirt::TContrVirt( TTipController *tcntr, string name_c,string _t_bd, string _n_bd, string _n_tb, TConfigElem *cfgelem) :
	TController(tcntr,name_c,_t_bd,_n_bd,_n_tb,cfgelem), run_st(true), endrun(true)
{

}

TContrVirt::~TContrVirt()
{
    Stop();
    Free();
}

int TContrVirt::Load( )
{
    TController::Load( );
    
    return(0);    
}

int TContrVirt::Save( )
{
    TController::Save( );
    
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
    //---- Attach parameter algoblock ----
    vector<string> list_t, list_p;
    
    owner->ListTpPrm(list_t);
    for(unsigned i_tprm=0; i_tprm < list_t.size(); i_tprm++)
    {
	List(list_t[i_tprm],list_p);
	for(unsigned i_prm=0; i_prm < list_p.size(); i_prm++)
	    ((TPrmVirt *)at(list_p[i_prm]))->Load(((TVirtual *)owner)->NameCfg() );
    } 
    //------------------------------------
    pthread_attr_init(&pthr_attr);
    if(owner->owner->owner->UserName() == "root")
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
    int    i_sync=0;
    
    struct itimerval mytim;             //Interval timer
    long   time_t1,time_t2,cnt_lost=0;
    int    frq = sysconf(_SC_CLK_TCK);  //Count of system timer n/sek
    TContrVirt *cntr = (TContrVirt *)contr;

    cntr->period  = cntr->Get_I("PERIOD");
    if(cntr->period == 0) return(NULL);
    cntr->d_sync = cntr->Get_I("PER_S")/cntr->period;
    if(cntr->d_sync == 0) cntr->d_sync = 1;
    cntr->Set_I("PER_S",cntr->d_sync*cntr->period);
    cntr->iterate = cntr->Get_I("ITER");    
    if(cntr->iterate <= 0) { cntr->iterate = 1; cntr->Set_R("ITER",(double)cntr->iterate); } 

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
	    for(unsigned i_tp=0; i_tp < cntr->prm_cfg.size(); i_tp++)
		for(unsigned i_p=0; i_p < cntr->prm_cfg[i_tp].size(); i_p++)
		    ((TPrmVirt *)cntr->prm_cfg[i_tp][i_p])->Calc();    
    }
    cntr->run_st = false;

    return(NULL);
}

void TContrVirt::Sync()
{
    for(unsigned i_tp=0; i_tp < prm_cfg.size(); i_tp++)
	for(unsigned i_p=0; i_p < prm_cfg[i_tp].size(); i_p++)
	    ((TPrmVirt *)prm_cfg[i_tp][i_p])->Sync();
}

TParamContr *TContrVirt::ParamAttach(int type)
{
    return(new TPrmVirt(this,owner->at_TpPrmCfg(type)));
}

//======================================================================
//==== TPrmVirt 
//====================================================================== 

TPrmVirt::TPrmVirt(TController *contr, TConfigElem *cfgelem) : TParamContr(contr,cfgelem), pid(NULL)
{

}


void TPrmVirt::UpdateVAL( )
{
    TParamContr::UpdateVAL();
    
    hd_y  = Elem()->NameToId("VAL");
    if( Elem()->Type(hd_y)&VAL_T_REAL )
    {
	STime tm = {0,0}; 
    	y_min = _GetR(hd_y,tm,V_MIN);
	y_max = _GetR(hd_y,tm,V_MAX);
    }
    else y_max = y_min = 0.0;

    if(Elem()->Name() == "PID")
    {	
    	pid = new SPID;
    	pid->hd_out  = Elem()->NameToId("OUT");
    	pid->hd_sp   = Elem()->NameToId("SP");
    	pid->hd_stat = Elem()->NameToId("STAT");	
    }
}    

TPrmVirt::~TPrmVirt( )
{    
    if(pid) delete pid;
}

void TPrmVirt::SetVal( int id_elem )
{
    Mess->put(1,"Comand to direct set value of element!");
}
void TPrmVirt::GetVal( int id_elem )
{
    Mess->put(1,"Comand to direct get value of element!");
}

void TPrmVirt::Load( string FCfg )
{
    int        fh;
    dword      ofs_alg;
    word       k_alg, t_form;    
    byte       len_1;
    char       buf[256];
    string     str;
    bool       i_ok=false;
    
    if(Name().size() == 0) return;
    
    if((fh=open(FCfg.c_str(),O_RDONLY)) == -1)
    { 
	Mess->put(3,"%s: Open file \"%s\" error!",NAME_MODUL,FCfg.c_str());
	return;
    }

    form = -1;
    
    read(fh,&ofs_alg,4);
    lseek(fh,0,SEEK_SET);
    lseek(fh,ofs_alg,SEEK_SET);
    read(fh,&k_alg,2);
    for(int i_alg=0;i_alg < k_alg; i_alg++)
    {
    	read(fh,buf,9); buf[9]=0;
	for(int i=8; i >= 0; i--) 
	    if(buf[i]==' ' || buf[i]== 0) buf[i]=0; else break; 
	str=buf;
	Mess->SconvIn("CP866",str);
	if(Name() == str) i_ok = true;

	read(fh,&len_1,1); 
	read(fh,buf,len_1); buf[len_1]=0;
	read(fh,&t_form,2);
	if(i_ok)
	{
	    descript = buf;
	    Mess->SconvIn("CP866",descript);
	    form     = t_form;
	}
	int i_n = ((TVirtual *)((TContrVirt *)owner)->owner)->formuls[t_form].n_inp;
	if(i_n)
	{
	    if(i_ok)
		for(int i_x=0;i_x < i_n;i_x++)
	    	{
	    	    if( i_x >= (int)x_id.size() ) x_id.insert(x_id.begin()+i_x);
	    	    if( i_x >= (int)x.size() )    x.insert(x.begin()+i_x);
	    	    read(fh,buf,9); buf[9] = 0;
		    for(int i=8; i >= 0; i--) 
			if(buf[i]==' ' || buf[i]== 0) buf[i]=0; else break;
		    if(buf[0]==0) { x_id[i_x].hd_prm = -1; x[i_x] = 1E+10; }
		    else
		    {
			str = buf;
			Mess->SconvIn("CP866",str);
			try
			{
			    try
			    {
				x_id[i_x].hd_prm   = owner->NameToHd(str);
				x_id[i_x].internal = true;
			    }
			    catch(TError)
			    { 
				x_id[i_x].hd_prm   = owner->owner->owner->owner->Param->NameToHd(str);
				x_id[i_x].internal = false;
			    }				
			}
			catch(TError) { x_id[i_x].hd_prm = -1; x[i_x] = 1E+10; }
		    }
		}
	    else lseek(fh,i_n*9,SEEK_CUR);
	}
	i_n = ((TVirtual *)((TContrVirt *)owner)->owner)->formuls[t_form].n_koef;
	if(i_n)
	{
	    if(i_ok)
    		for(int i_k = 0;i_k < i_n; i_k++)
    		{
    		    if( i_k >= (int)k.size() ) k.insert(k.begin()+i_k);
    		    read(fh,&k[i_k],4);
    		}
	    else lseek(fh,i_n*4,SEEK_CUR);
	}
	if(i_ok) break;
    }
    
    close(fh); 
}


inline void TPrmVirt::Y(float val)
{
    STime tm = {0,0};
    _SetR(hd_y,val,tm);
}

inline float TPrmVirt::Y()
{ 
    STime tm = {0,0};
    return(_GetR(hd_y,tm));
}

inline void TPrmVirt::X(unsigned id ,float val)
{ 
    float val_t;

    if(x_id[id].hd_prm < 0) return;
    if(x_id[id].internal)  ((TPrmVirt *)owner->at(x_id[id].hd_prm))->Y(val);
    else
    {
	if(x_id[id].max == x_id[id].min) val_t = val;
	else val_t = (val > x_id[id].max)?x_id[id].max:(val < x_id[id].min)?x_id[id].min:val;
	if(x[id] != val_t) { x_id[id].sync = true; x[id] = val_t; }
    }
}

inline float TPrmVirt::X(unsigned id)
{
    if(x_id[id].hd_prm < 0) return(0.0);
    if(x_id[id].internal)   return( ((TPrmVirt *)owner->at(x_id[id].hd_prm))->Y() );
    return(x[id]);
}

void TPrmVirt::Sync()
{
    STime tm = {0,0};
    //Syncing no internal io to TValue
    for(unsigned i_x = 0; i_x < x_id.size(); i_x++)
	if(!x_id[i_x].internal && x_id[i_x].hd_prm >= 0 )
	{
	    int hd_v = owner->at(x_id[i_x].hd_prm)->Elem()->NameToId("VAL");
	    if(	x_id[i_x].sync )
	    {
		owner->at(x_id[i_x].hd_prm)->SetR(hd_v,x[i_x],tm);
		x_id[i_x].sync = false;
	    }
	    else x[i_x] = owner->at(x_id[i_x].hd_prm)->GetR(hd_v,tm);
	}
}

float TPrmVirt::Calc()
{
    if(form < 0) return(1E+10);

    
    switch( ((TVirtual *)((TContrVirt *)owner)->owner)->formuls[form].tip)
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
    Mess->put(1,"%d: Furmule id= %d no avoid!",form, ((TVirtual *)((TContrVirt *)owner)->owner)->formuls[form].tip);

    return(1E+10);
}

//************************************************************
//* float blok_dig( );                                       *
//*   Сборка дискретных параметров.                          *
//************************************************************
float TPrmVirt::blok_dig( )
{
    bool set = false;

    if(X(0) && k[2] != 1.) { k[2]=1.; set = true; }
    if(X(2) && k[2] != 2.) { k[2]=2.; set = true; }
    if(X(4) && k[2] != 3.) { k[2]=3.; set = true; }
    if( set && k[0] > 0. ) { k[1]=k[0]; set = false; }
    if(k[1] > 0.) k[1] -= ((TContrVirt *)owner)->Period()/(1000*sysconf(_SC_CLK_TCK)*((TContrVirt *)owner)->Iterate());
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
float TPrmVirt::ymn()
{
    if(X(2)==0. || X(3)==0. || X(6)==0. || X(7)==0.) return(1E+10);
    if( !k[0] ) k[0] = 1.;
    return(k[0]*k[5]*X(0)*X(1)*X(4)*X(5))/(X(2)*X(3)*X(6)*X(7));
}
         
//************************************************************
//* float sym(a_fli *GB);                                    *
//*   Function addition.                                     *
//************************************************************
float TPrmVirt::sym()
{
    return(k[0]*X(0)+k[1]*X(1)+k[2]*X(2)+k[5]*X(3)+k[6]*X(4)+k[7]*X(5)+k[10]*X(6)+k[11]*X(7));
}

//************************************************************
//* float free_formul(a_fli *GB)                             *
//*   Flow formuls that free edit.                           *
//************************************************************
float TPrmVirt::free_formul( )
{
    int offset = 0;
    SFrm *formul = &((TVirtual *)((TContrVirt *)owner)->owner)->formuls[form];
    return(calk_form(formul->form_e,formul->l_frm1,&offset,0,0));
}
               
float TPrmVirt::calk_form(char *form, int len, int *off, float rez,byte h_prior)
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
	if(symb) parm = 1000*((TContrVirt *)owner)->Iterate()*sysconf(_SC_CLK_TCK)/((TContrVirt *)owner)->Period();
	else     rez  = 1000*((TContrVirt *)owner)->Iterate()*sysconf(_SC_CLK_TCK)/((TContrVirt *)owner)->Period();
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
void TPrmVirt::calk_oper(byte symb,float *rez,float parm)
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
float TPrmVirt::pid_n( )
{
    float  err=0.,vhod=0.,KInt,Kzdif,Dif,Kf,k1,k2,k3,k4,in;
    STime  tm;

    if(!pid) return(1E+10);
    
    int    period      = ((TContrVirt *)owner)->Period();
    int    HZ          = 1000*sysconf(_SC_CLK_TCK);
    int    cnt_in_cycl = ((TContrVirt *)owner)->Iterate();
    float  sp          = _GetR(pid->hd_sp,tm);
    float  out         = _GetR(pid->hd_out,tm);
    char   stat        = (char)_GetI(pid->hd_stat,tm);
    
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

    _SetR(pid->hd_sp,sp,tm);
    _SetR(pid->hd_out,out,tm);
    _SetI(pid->hd_stat,stat,tm);

    if( x_id[0].hd_prm < 0 )  return 0.;
    return X(0);
}

