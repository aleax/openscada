#include <getopt.h>



#include "tapplication.h"
#include "tmessage.h"
#include "tbd.h"
#include "tcontroller.h"
#include "ttipcontroller.h"
#include "./moduls/gener/tmodule.h"
#include "tvalue.h"
#include "tcontrollers.h"

SRecStr RStr[] =
{
    {""}
};

SRecNumb RNumb[] =
{
    {0.,   3.,   0., 0, 0},
    {0., 511., 292., 0, 1},
    {0.,   0.,   0., 0, 3}
};
//==== Desribe Val element fields ====
SElem ValEl[] =
{
    {"NAME"   ,"Short name a element of value"             ,CFGTP_STRING,20,"","",&RStr[0],NULL     ,NULL},
    {"LNAME"  ,"Long name a element of value"              ,CFGTP_STRING,20,"","",&RStr[0],NULL     ,NULL},
    {"DESCR"  ,"Description a element of value"            ,CFGTP_STRING,20,"","",&RStr[0],NULL     ,NULL},
    {"TYPE"   ,"Type (VAL_T_REAL, VAL_T_BOOL, VAL_T_INT )" ,CFGTP_NUMBER,1 ,"","",NULL    ,&RNumb[0],NULL},
    {"STAT"   ,"Stat (VAL_S_GENER, VAL_S_UTIL, VAL_S_SYS)" ,CFGTP_NUMBER,1 ,"","",NULL    ,&RNumb[0],NULL},
    {"MODE"   ,"Mode (VAL_M_OFTN, VAL_M_SELD, VAL_M_CNST)" ,CFGTP_NUMBER,1 ,"","",NULL    ,&RNumb[0],NULL},
    {"DATA"   ,"Data from: (VAL_D_FIX, VAL_D_BD, VAL_D_VBD)",CFGTP_NUMBER,1 ,"","",NULL    ,&RNumb[0],NULL},
    {"ACCESS" ,"Access to element (0444)"                  ,CFGTP_NUMBER,1 ,"","",NULL    ,&RNumb[1],NULL},
    {"MIN"    ,"Minimum"                                   ,CFGTP_NUMBER,10,"","",NULL    ,&RNumb[2],NULL},
    {"MAX"    ,"Maximum"                                   ,CFGTP_NUMBER,10,"","",NULL    ,&RNumb[2],NULL}
};


TControllerS::TControllerS(  ) : TGRPModule("Controller"), gener_bd("generic") 
{
    ValElem.Load(ValEl,sizeof(ValEl)/sizeof(SElem));
}

TControllerS::~TControllerS(  )
{
    while(Contr.size())
	Contr.erase(Contr.begin());
    while(TContr.size())
    {
	delete TContr[0];
	TContr.erase(TContr.begin());
    }
}

int TControllerS::InitAll( )
{
    for(int i=0;i<Moduls.size();i++) 
	if(Moduls[i].stat == GRM_ST_OCCUP) Moduls[i].modul->init(TContr[i]);
    LoadBD();
    
    return(0);
}

void TControllerS::DeInit(  )
{


}

int TControllerS::StartAll(  )         
{
    for(int i=0; i< Contr.size(); i++)
	TContr[Contr[i].id_mod]->Start(Contr[i].name);
//==== Test ====
//    test();
//==============        
}

int TControllerS::StopAll(  )
{
//    LoadBD();
    for(int i=0; i< Contr.size(); i++)
	TContr[Contr[i].id_mod]->Stop(Contr[i].name);
}

void TControllerS::ContrList( vector<string> & List )
{
    int i,id_ctr;
    
    List.clear();
    for(i=0;i < Size(); i++)
	if(Contr[i].id_mod >= 0 ) 
	    List.push_back(Contr[i].name);
}


void TControllerS::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "========================= TipController options ===========================\n"
    "    --TCModPath=<path>  Set moduls <path>;\n"
    "    --TCGenerBD=<BD>    Set a name of generic BD (default \"generic\");\n"
    "\n");
}

void TControllerS::CheckCommandLine(  )
{
    int i,next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{"TCModPath",1,NULL,'m'},
	{"TCGenerBD",1,NULL,'b'},	
	{NULL       ,0,NULL,0  }
    };

    optind=opterr=0;	
    do
    {
	next_opt=getopt_long(App->argc,(char * const *)App->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': pr_opt_descr(stdout);   break;
	    case 'm': DirPath=strdup(optarg); break;
	    case 'b': gener_bd=optarg;        break;
	    case -1 : break;
	}
    } while(next_opt != -1);
//    if(optind < App->argc) pr_opt_descr(stdout);
}


void TControllerS::LoadBD()
{
    string cell;
    bool   reload = false;

    int b_hd = App->BD->OpenBD(gener_bd);
    if(b_hd < 0) return;

    for(int i=0; i < App->BD->NLines(b_hd); i++)
    {
	//Get Controller's name
	if(App->BD->GetCellS(b_hd,"NAME",i,cell) != 0) return;
	//Find duplicate of controllers
	int ii;
	for(ii=0;ii < Size(); ii++)
	    if(cell == Contr[ii].name) break;
	if(ii < Size())
	{
	    reload = true;
#if debug
    	    App->Mess->put(0, "Reload controller %s: %d !",cell.c_str(),ii);
#endif
	}
	else
	{
	    reload = false;
    	    Contr.push_back();
	    HdIns(Size()-1);
#if debug
    	    App->Mess->put(0, "Add Contr %s: %d !",cell.c_str(),ii);
#endif
    	    Contr[ii].name=cell;
	}
	// Add/modify controller
	App->BD->GetCellS(b_hd,"MODUL",i,cell);
	Contr[ii].modul=cell;
	Contr[ii].id_mod=name_to_id(cell);
	App->BD->GetCellS(b_hd,"BDNAME",i,cell);
	Contr[ii].bd=cell;                                          
	if(Contr[ii].id_mod >= 0)
	{
	    TContr[Contr[ii].id_mod]->idmod = Contr[ii].id_mod;            //????
	    if(reload == false)
	    {
		//find free record
		unsigned iii;
		for(iii=0; iii < TContr[Contr[ii].id_mod]->Size(); iii++)
		    if(TContr[Contr[ii].id_mod]->at(iii)==TO_FREE) break;
		//add controller record                           
		Contr[ii].id_contr = TContr[Contr[ii].id_mod]->Add(Contr[ii].name, Contr[ii].bd);
	    }
	    double val;
	    App->BD->GetCellN(b_hd,"STAT",i,val);
	    if(val == 0.) TContr[Contr[ii].id_mod]->at(Contr[ii].id_contr)->stat=TCNTR_DISABLE;
	    else          TContr[Contr[ii].id_mod]->at(Contr[ii].id_contr)->stat=TCNTR_ENABLE;
	    TContr[Contr[ii].id_mod]->LoadContr(Contr[ii].name);
	}	    
    }
    App->BD->CloseBD(b_hd);
}


int TControllerS::UpdateBD(  )
{
    int i,ii,b_hd;
    string cell, stat;
    
    //Update general BD
    if( (b_hd = App->BD->OpenBD(gener_bd)) < 0) return(-1);
    //Find deleted controllers
    for(i=0; i < App->BD->NLines(b_hd); i++)
    {
	App->BD->GetCellS(b_hd,"NAME",i,cell);
	
	for(ii=0;ii < Size(); ii++)
	    if( cell==Contr[ii].name) break;
	if(ii == Contr.size())
	{ 
	    App->BD->DelLine(b_hd,i);
	    i--;
	}
    }
    //Modify present and add new controllers
    for(i=0;i < Size(); i++)
    {
	for(ii=0; ii < App->BD->NLines(b_hd); ii++)
	{
	    App->BD->GetCellS(b_hd,"NAME",ii,cell);
    	    if(cell==Contr[i].name) break;
	}
	if(ii == App->BD->NLines(b_hd)) App->BD->AddLine(b_hd,ii);
	App->BD->SetCellS(b_hd,"NAME",ii,Contr[i].name);
	App->BD->SetCellS(b_hd,"MODUL",ii,Contr[i].modul);
	App->BD->SetCellS(b_hd,"BDNAME",ii,Contr[i].bd);
	double stat;
	if(Contr[i].id_mod < 0 || Contr[i].id_contr < 0) stat=0;
	else
	{ 
	    if( TContr[Contr[i].id_mod]->at(Contr[i].id_contr)->stat == TCNTR_DISABLE) stat = 0 ;
	    else stat = 1;
	}
	App->BD->SetCellN(b_hd,"STAT",ii,stat);

	TContr[Contr[i].id_mod]->SaveContr(Contr[ii].name);	
    }
    App->BD->SaveBD(b_hd);
    App->BD->CloseBD(b_hd);

    return(0);
}


int TControllerS::CreateGenerBD( string type_bd )
{
    SRowAttr attr;
    int b_hd;

    if( (b_hd = App->BD->NewBD(type_bd, gener_bd)) < 0) return(-1);
    attr.name = "NAME";   attr.type = 'C'; attr.len = 20; App->BD->AddRow(type_bd,b_hd,&attr);
    attr.name = "MODUL";  attr.type = 'C'; attr.len = 20; App->BD->AddRow(type_bd,b_hd,&attr);
    attr.name = "BDNAME"; attr.type = 'C'; attr.len = 20; App->BD->AddRow(type_bd,b_hd,&attr);
    attr.name = "STAT";   attr.type = 'N'; attr.len = 1;  App->BD->AddRow(type_bd,b_hd,&attr);

    App->BD->SaveBD(type_bd,b_hd); 
    App->BD->CloseBD(type_bd,b_hd);
    
    return(0);
}


int TControllerS::AddContr( string name, string tip, string bd )
{
    int i;
    //!!! Want request resource 
    if(name_to_id(tip) < 0) return(-1);
    //Find Controller dublicate
    for(i=0;i < Size(); i++)
    	if(Contr[i].name == name) break;
    if(i < Size())   return(-2);   
    Contr.push_back();
    HdIns(Size()-1);
    //Fill record
    Contr[i].name  = name;
    Contr[i].modul = tip;
    Contr[i].id_mod= name_to_id(tip);
    Contr[i].bd    = bd;
    Contr[i].id_contr = TContr[Contr[i].id_mod]->Add(name,bd);
   
    return(0);
}

int TControllerS::DelContr( string name )
{
    int i_cnt;

    //!!! Want request resource 
    //Find Controller 
    for(i_cnt = 0; i_cnt < Size(); i_cnt++)
    	if(Contr[i_cnt].name == name) break;
    if(i_cnt == Size())   return(-1);
    //Delete controller at modul
    TContr[Contr[i_cnt].id_mod]->Del(name);
    Contr.erase(Contr.begin()+i_cnt);
    HdFree(i_cnt);
    
    return(0);
}

int TControllerS::AddM( TModule *modul )
{

    int kz=TGRPModule::AddM(modul);
    if(kz < 0) return(kz);
    if(kz == TContr.size())      TContr.push_back( new TTipController(modul) );
    else if(TContr[kz]==TO_FREE) TContr[kz] = new TTipController(modul);

    return(kz);
}

int TControllerS::DelM( int hd )
{
    int kz;
    kz=TGRPModule::DelM(hd);
    if(kz != 0) return(kz);
    delete TContr[kz]; 
    TContr[kz]=TO_FREE;

    return(0);
}

int TControllerS::HdIns(int id)
{
    unsigned i_hd;
    for( i_hd=0; i_hd < hd.size(); i_hd++)
        if(hd[i_hd] >= id ) hd[i_hd]++;
    for( i_hd=0; i_hd < hd.size(); i_hd++)
        if(hd[i_hd] < 0 ) break;
    if( i_hd == hd.size() ) hd.push_back();
    hd[i_hd] = id;

    return(i_hd);
}

int TControllerS::HdFree(int id)
{
    for(unsigned i_hd=0; i_hd < hd.size(); i_hd++)
        if( hd[i_hd] == id ) { hd[i_hd] = -1; break; }
    for(unsigned i_hd=0; i_hd < hd.size(); i_hd++)
        if( hd[i_hd] > id ) hd[i_hd]--;

    return(0);
}

TController *TControllerS::at( int id_hd)
{ 
    if(id_hd >= hd.size() || id_hd < 0 || hd[id_hd] < 0 ) return(NULL); 
    if(Contr[hd[id_hd]].id_mod < 0 || Contr[hd[id_hd]].id_contr < 0 ) return(NULL);
    return(TContr[Contr[hd[id_hd]].id_mod]->at(Contr[hd[id_hd]].id_contr));
}

int TControllerS::HdChange( int id1, int id2 )
{
    for(unsigned i_hd = 0; i_hd < hd.size(); i_hd++)
        if( hd[i_hd] == id1 )      { hd[i_hd] = id2; continue; }
        else if( hd[i_hd] == id2 ) { hd[i_hd] = id1; continue; }

    return(0);
}

int TControllerS::NameToHd( string Name )
{
    for(unsigned i_hd = 0; i_hd < hd.size(); i_hd++)
        if(hd[i_hd] >= 0 && Contr[hd[i_hd]].name == Name ) return(i_hd);

    return(-1);
}

TTipController *TControllerS::at_tp( string name )
{
    for(unsigned i_cntrt = 0; i_cntrt < TContr.size(); i_cntrt++)
	if(TContr[i_cntrt]!=TO_FREE && TContr[i_cntrt]->Name() == name )
	    return(TContr[i_cntrt]);

    return(NULL);
}

int TControllerS::test( )
{
    int hd_b, id;

    int kz=CreateGenerBD("direct_bd");
    App->Mess->put(0, "Create BD: %d !",kz);
    hd_b=App->BD->OpenBD(gener_bd);
    App->Mess->put(0, "Open BD %s: %d !",gener_bd.c_str(),hd_b);
    id=App->BD->AddLine(hd_b,1000);
    App->Mess->put(0, "Add line: %d !",id);
    App->BD->SetCellS(hd_b,"NAME",id,"virt_test1");
    App->BD->SetCellS(hd_b,"MODUL",id,"virtual");
    App->BD->SetCellS(hd_b,"BDNAME",id,"virt_c");
    App->BD->SetCellN(hd_b,"STAT",id,1.0);
    id=App->BD->AddLine(hd_b,1000);
    App->Mess->put(0, "Add line: %d !",id);
    App->BD->SetCellS(hd_b,"NAME",id,"virt_test2");
    App->BD->SetCellS(hd_b,"MODUL",id,"virtual");
    App->BD->SetCellS(hd_b,"BDNAME",id,"virt_c");
    App->BD->SetCellN(hd_b,"STAT",id,0.0);
    id=App->BD->SaveBD(hd_b);
    id=App->BD->CloseBD(hd_b);
}

