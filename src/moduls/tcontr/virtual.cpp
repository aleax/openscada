/* Test Modul
** ==============================================================
*/

#include <getopt.h>

#include "../../tapplication.h"
#include "../../tmessage.h"
#include "../../tconfig.h"
#include "../../tcontroller.h"
#include "../gener/tmodule.h"
#include "virtual.h"

//============ Modul info! =====================================================
#define NAME_MODUL  "virtual"
#define NAME_TYPE   "Controller"
#define VERSION     "0.1"
#define AUTORS      "Roman_Savochenko"
#define DESCRIPTION "Virtual controller my be used how internal controller or instrument for GUI"
#define LICENSE     "LGPL"
//==============================================================================

extern "C" TModule *attach( char *FName, int n_mod );

//==== Extended field's elements  ====

SRecStr TVirtual::RStr[] =
{
    {""},
    {"Virtual controller"},
    {"VRT_AN"},
    {"VRT_DG"},
    {"VRT_BL"},
    {"Analog parameter"}
};

SRecNumb TVirtual::RNumb[] =
{
    {0., 10000., 1000., 0, 0},
    {0.,    99.,    1., 0, 0},
    {0.,     0.,    0., 2, 3},
    {0.,     0.,  100., 2, 3},
    {0.,    50.,   0.5, 1, 3}
};

SRecSel TVirtual::RSel[] =
{
    {"0","Linear;Square","0;1"},
    {"0","Average;Integrate;Counter","0;1;2"},
    {"0","Input;Output","0;1"}
};

//==== Desribe controler's bd fields ====

SElem TVirtual::elem[] =
{
    {"NAME"   ,"Short name of controller."        ,CFGTP_STRING,20,"","",&RStr[0],NULL     ,NULL},
    {"LNAME"  ,"Description of controller."       ,CFGTP_STRING,50,"","",&RStr[1],NULL     ,NULL},
    {"PRM_BD1","Name BD for ANALOG parameteres."  ,CFGTP_STRING,20,"","",&RStr[2],NULL     ,NULL},
    {"PRM_BD2","Name BD for DIGIT parameteres."   ,CFGTP_STRING,20,"","",&RStr[3],NULL     ,NULL},
    {"PRM_BD3","Name BD for BLOCK parameteres."   ,CFGTP_STRING,20,"","",&RStr[4],NULL     ,NULL},
    {"PERIOD" ,"Pooled period (ms)."              ,CFGTP_NUMBER,5 ,"","",NULL    ,&RNumb[0],NULL},
    {"ITER"   ,"Number of a iterations at period.",CFGTP_NUMBER,2 ,"","",NULL    ,&RNumb[1],NULL}
};

//==== Desribe ANALOG parameter's bd fields ====

SElem TVirtual::ElemAN[] =
{
    {"SHIFR"  ,"Short name of parameter (TAGG)."  ,CFGTP_STRING,20,"","",&RStr[0],NULL     ,NULL},
    {"NAME"   ,"Description of parameter"         ,CFGTP_STRING,50,"","",&RStr[5],NULL     ,NULL},
    {"ED"     ,"Value of measurement"             ,CFGTP_STRING,10,"","",&RStr[0],NULL     ,NULL},
    {"SCALE"  ,"Scale"                            ,CFGTP_SELECT,1 ,"","",NULL    ,NULL     ,&RSel[0]},
    {"TIPO"   ,"Type of processing"               ,CFGTP_SELECT,1 ,"","",NULL    ,NULL     ,&RSel[1]},
    {"NG"     ,"Lower scale border"               ,CFGTP_NUMBER,10,"","",NULL    ,&RNumb[2],NULL},
    {"VG"     ,"Upper scale border"               ,CFGTP_NUMBER,10,"","",NULL    ,&RNumb[3],NULL},
    {"NTG"    ,"Lower technically scale border"   ,CFGTP_NUMBER,10,"","",NULL    ,&RNumb[2],NULL},
    {"VTG"    ,"Upper technically scale border"   ,CFGTP_NUMBER,10,"","",NULL    ,&RNumb[2],NULL},
    {"NAG"    ,"Lower alarm scale border"         ,CFGTP_NUMBER,10,"","",NULL    ,&RNumb[2],NULL},
    {"VAG"    ,"Upper alarm scale border"         ,CFGTP_NUMBER,10,"","",NULL    ,&RNumb[2],NULL},
    {"Z_GR"   ,"Non-sensitive zone"               ,CFGTP_NUMBER,4 ,"","",NULL    ,&RNumb[4],NULL},
    {"TIP"    ,"Parameter type"                   ,CFGTP_SELECT,1 ,"","",NULL    ,NULL     ,&RSel[2]}
};

SElem TVirtual::ElemDG[] =
{
    {"SHIFR"  ,"Short name of parameter (TAGG)."  ,CFGTP_STRING,20,"","",&RStr[0],NULL     ,NULL},
    {"NAME"   ,"Description of parameter"         ,CFGTP_STRING,50,"","",&RStr[5],NULL     ,NULL},
    {"TIP"    ,"Parameter type"                   ,CFGTP_SELECT,1 ,"","",NULL    ,NULL     ,&RSel[2]}
};

SElem TVirtual::ElemBL[] =
{
    {"SHIFR"  ,"Short name of parameter (TAGG)."  ,CFGTP_STRING,20,"","",&RStr[0],NULL     ,NULL},
    {"NAME"   ,"Description of parameter"         ,CFGTP_STRING,50,"","",&RStr[5],NULL     ,NULL}
};

//===========================

TVirtual::TVirtual(char *name) : TModule()
{
    NameModul = NAME_MODUL;
    NameType  = NAME_TYPE;
    Vers      = VERSION;
    Autors    = AUTORS;
    DescrMod  = DESCRIPTION;
    License   = LICENSE;
    FileName  = strdup(name);

//    ExpFunc   = NULL; // (SExpFunc *)ExpFuncLc;
//    NExpFunc  = 0; // sizeof(ExpFuncLc)/sizeof(SExpFunc);
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

int TVirtual::PutCommand( string command, int id_cntr )
{
#if debug
    App->Mess->put(1, "Command: <%s> to controller <%d>!",command.c_str(),id_cntr);
#endif
    if(command == "INIT")        return(InitContr(id_cntr));
    else if(command == "DEINIT")
    {
//	    SaveBDContr( atoi( param.c_str() ) );
//	    SaveBDParams( atoi( param.c_str() ) );
    }
    else return(-1);

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
    int parm_id;
    
    TContr  = (TTipController *)param;
    TContr->LoadElCtr(elem,sizeof(elem)/sizeof(SElem));
    TContr->LoadElParm(0,ElemAN,sizeof(ElemAN)/sizeof(SElem));
    TContr->LoadElParm(1,ElemDG,sizeof(ElemDG)/sizeof(SElem));
    TContr->LoadElParm(2,ElemBL,sizeof(ElemBL)/sizeof(SElem));

    CheckCommandLine();
    TModule::init( param );

    return(0);
}

int TVirtual::InitContr(int id)
{
    string val;	
    
//    test(id);
    TContr->LoadCtrCfg(id);
    TContr->LoadParmCfg(id,0);
    TContr->LoadParmCfg(id,1);
    TContr->LoadParmCfg(id,2);
    
    test1(id);
    
#if debug
    App->Mess->put(1, "Init controller: <%d>, bd <%s>!",id,TContr->contr[id]->bd.c_str());
#endif
    return(0);    
}

void TVirtual::test(int id)
{
    char str[40];

    sprintf(str,"Test virtual controller %d",id+1);
    TContr->contr[id]->config->SetVal(0,"LNAME",str);
    sprintf(str,"virt_test%d_an",id+1);
    TContr->contr[id]->config->SetVal(0,"PRM_BD1",str);    
    sprintf(str,"virt_test%d_dig",id+1);
    TContr->contr[id]->config->SetVal(0,"PRM_BD2",str);    
    sprintf(str,"virt_test%d_bl",id+1);
    TContr->contr[id]->config->SetVal(0,"PRM_BD3",str);    
    TContr->contr[id]->config->SetVal(0,"PERIOD",1000.);    
    TContr->contr[id]->config->SetVal(0,"ITER",1.);    
    TContr->SaveCtrCfg(id);
}

void TVirtual::test1(int id)
{
    string val,val1,val2;
    
    for(int ii=0; ii < 2; ii++)
    	for(int i=0; i < TContr->contr[id]->prm_cfg[ii]->Size(); i++)
	{
	    TContr->contr[id]->prm_cfg[ii]->GetVal(i,"SHIFR",val);
	    App->Mess->SconvOut("KOI8-U",val);
	    TContr->contr[id]->prm_cfg[ii]->GetVal(i,"NAME",val1);
	    App->Mess->SconvOut("KOI8-U",val1);
	    TContr->contr[id]->prm_cfg[ii]->GetVal(i,"TIP",val2);
	    App->Mess->put(1, "Param <%s>; Name <%s>; Type <%s>",val.c_str(), val1.c_str(),val2.c_str() );
    	}  
}


