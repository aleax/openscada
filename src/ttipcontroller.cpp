#include <getopt.h>

#include "tapplication.h"
#include "tmessage.h"
#include "tbd.h"
#include "tcontroller.h"
#include "./moduls/gener/tmodule.h"
#include "ttipcontroller.h"

 TTipController::TTipController(  ) : TGRPModule("Controller"), gener_bd("generic") 
{
    
}

int TTipController::InitAll( )
{
    for(int i=0;i<Moduls.size();i++) 
	if(Moduls[i]->stat == GRM_ST_OCCUP) Moduls[i]->modul->init(TContr[i]);
}


void TTipController::Init(  )
{
    string StrPath;

    CheckCommandLine();
    LoadAll(StrPath+App->ModPath+","+DirPath);
    InitAll();
}

void DeInit(  )
{


}

void TTipController::Start(  )         
{
    LoadBD();
    for(int i=0; i< Contr.size(); i++)
	if(Contr[i]->stat==TCNTR_ENABLE)
	    PutCntrComm("START",i);
//==== Test ====
//    test();
//==============        
}

void TTipController::Stop(  )
{
    LoadBD();
    for(int i=0; i< Contr.size(); i++)
	if(Contr[i]->stat==TCNTR_ENABLE)
	    PutCntrComm("STOP",i);
}

void TTipController::ContrList( const string NameContrTip, string & List )
{
    int i;
    
    List.erase();
    for(i=0;i < Contr.size(); i++)
	if(Contr[i]->modul == NameContrTip) 
	    List=List+Contr[i]->name+',';
}


void TTipController::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "========================= TipController options ===========================\n"
    "    --TCModPath=<path>  Set moduls <path>;\n"
    "    --TCGenerBD=<BD>    Set a name of generic BD (default \"generic\");\n"
    "\n");
}

void TTipController::CheckCommandLine(  )
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


void TTipController::LoadBD()
{
    string cell;

    int b_hd = App->BD->OpenBD(gener_bd);
    if(b_hd < 0) return;

    for(int i=0; i < App->BD->NLines(b_hd); i++)
    {
	//Get Controller's name
	if(App->BD->GetCellS(b_hd,"NAME",i,cell) != 0) return;
	int ii;
	//Find duplicate of controllers
	for(ii=0;ii < Contr.size(); ii++)
	    if(cell == Contr[ii]->name) break;
	if(ii < Contr.size())
	{
#if debug
    	    App->Mess->put(0, "Reload controller %s: %d !",cell.c_str(),ii);
#endif
	}
	else
	{
    	    // Find free elements
    	    for(ii=0;ii < Contr.size(); ii++)
    		if(Contr[ii]->stat == TCNTR_FREE) break;
    	    if(ii == Contr.size()) Contr.push_back(new SContr);
#if debug
    	    App->Mess->put(0, "Add Contr %s: %d !",cell.c_str(),ii);
#endif
    	    Contr[ii]->name=cell;
	}
	// Add/modify controller
	App->BD->GetCellS(b_hd,"MODUL",i,cell);
	Contr[ii]->modul=cell;
	Contr[ii]->id_mod=name_to_id(cell);
	App->BD->GetCellS(b_hd,"BDNAME",i,cell);
	Contr[ii]->bd=cell;                                          
	double val;
	App->BD->GetCellN(b_hd,"STAT",i,val);
	if(Contr[ii]->id_mod < 0) Contr[ii]->stat = TCNTR_ERR;
	else if(val == 0.)        Contr[ii]->stat = TCNTR_DISABLE;
	else                      Contr[ii]->stat = TCNTR_ENABLE;
	if( Contr[ii]->stat == TCNTR_ENABLE )
	{
	    TContr[Contr[ii]->id_mod]->idmod = Contr[ii]->id_mod;
	    //add controller record                           
	    //find free record
	    int iii;
	    for(iii=0; iii < TContr[Contr[ii]->id_mod]->contr.size(); iii++)
		if(TContr[Contr[ii]->id_mod]->contr[iii]==NULL) break;
	    if(iii == TContr[Contr[ii]->id_mod]->contr.size()) 
		TContr[Contr[ii]->id_mod]->contr.push_back(new TController());
	    else TContr[Contr[ii]->id_mod]->contr[iii] = new TController();
	    TContr[Contr[ii]->id_mod]->contr[iii]->bd = Contr[ii]->bd;
	    TContr[Contr[ii]->id_mod]->contr[iii]->name = Contr[ii]->name;
	    Contr[ii]->id_contr=iii;
	    TContr[Contr[ii]->id_mod]->config.InitRecord(iii);
	    PutCntrComm("INIT", ii );
	}	    
    }
    App->BD->CloseBD(b_hd);
}


int TTipController::UpdateBD(  )
{
    int i,ii,b_hd;
    string cell, stat;
    
    //Update general BD
    if( (b_hd = App->BD->OpenBD(gener_bd)) < 0) return(-1);
    //Find deleted controllers
    for(i=0; i < App->BD->NLines(b_hd); i++)
    {
	App->BD->GetCellS(b_hd,"NAME",i,cell);
	
	for(ii=0;ii < Contr.size(); ii++)
	    if(Contr[ii]->stat != TCNTR_FREE && cell==Contr[ii]->name) break;
	if(ii == Contr.size())
	{ 
	    App->BD->DelLine(b_hd,i);
	    i--;
	}
    }
    //Modify present and add new controllers
    for(i=0;i < Contr.size(); i++)
    {
    	if(Contr[i]->stat == TCNTR_FREE) continue;
	for(ii=0; ii < App->BD->NLines(b_hd); ii++)
	{
	    App->BD->GetCellS(b_hd,"name",ii,cell);
    	    if(cell==Contr[i]->name) break;
	}
	if(ii == App->BD->NLines(b_hd)) App->BD->AddLine(b_hd,ii);
	App->BD->SetCellS(b_hd,"NAME",ii,Contr[i]->name);
	App->BD->SetCellS(b_hd,"MODUL",ii,Contr[i]->modul);
	App->BD->SetCellS(b_hd,"BDNAME",ii,Contr[i]->bd);
	App->BD->SetCellN(b_hd,"STAT",ii,(double)Contr[i]->stat);
	PutCntrComm("DEINIT", ii );	
    }
    App->BD->CloseBD(b_hd);

    return(0);
}


int TTipController::CreateGenerBD( string type_bd )
{
    int b_hd;
    if( (b_hd = App->BD->NewBD(type_bd, gener_bd)) < 0) return(-1);
    App->BD->AddRow(type_bd,b_hd,"NAME",'C',20);
    App->BD->AddRow(type_bd,b_hd,"MODUL",'C',20);
    App->BD->AddRow(type_bd,b_hd,"BDNAME",'C',20);
    App->BD->AddRow(type_bd,b_hd,"STAT",'N',1);

    App->BD->SaveBD(type_bd,b_hd); 
    App->BD->CloseBD(type_bd,b_hd);
    
    return(0);
}


int TTipController::AddContr( string name, string tip, string bd )
{
    int i;

    //Find modul <tip>
    for(i=0;i < Moduls.size(); i++)
	if(Moduls[i]->stat != GRM_ST_FREE && Moduls[i]->name == tip) break;
    if(i == Moduls.size()) return(-1);
    //Find Controller dublicate
    for(i=0;i < Contr.size(); i++)
    	if(Contr[i]->stat != TCNTR_FREE && Contr[i]->name == name) break;
    if(i < Contr.size())   return(-2);   
    //Find free elements and add into generic BD
    for(i=0;i < Contr.size(); i++)
	if(Contr[i]->stat == TCNTR_FREE) break;
    if(i == Contr.size()) Contr.push_back(new SContr);
    Contr[i]->name  = name;
    Contr[i]->modul = tip;
    Contr[i]->id_mod= name_to_id(tip);
    Contr[i]->bd    = bd;
    Contr[i]->stat  = TCNTR_DISABLE;
    //Add controller into modul
    int ii;
    for(ii=0; ii < TContr[Contr[i]->id_mod]->contr.size(); ii++)
	if(TContr[Contr[i]->id_mod]->contr[ii]==NULL) break;
    if(ii == TContr[Contr[i]->id_mod]->contr.size()) 
	TContr[Contr[i]->id_mod]->contr.push_back(new TController());
    else TContr[Contr[i]->id_mod]->contr[ii] = new TController();
    TContr[Contr[i]->id_mod]->contr[ii]->bd   = Contr[i]->bd;
    TContr[Contr[i]->id_mod]->contr[ii]->name = Contr[i]->name;
    Contr[i]->id_contr=ii;
    TContr[Contr[i]->id_mod]->config.InitRecord(ii);
    PutCntrComm("ADD",i);	
   
    return(0);
}

int TTipController::DelContr( string name )
{
    int i;

    //Find Controller 
    for(i=0;i < Contr.size(); i++)
    	if(Contr[i]->stat == TCNTR_FREE && Contr[i]->name == name) break;
    if(i == Contr.size())   return(-1);
    //Delete controller at modul
    delete TContr[Contr[i]->id_mod]->contr[Contr[i]->id_contr];
    TContr[Contr[i]->id_mod]->contr[Contr[i]->id_contr] == NULL;
    PutCntrComm("DELETE", i );
    //Delete from generic BD 
    Contr[i]->stat=TCNTR_FREE;
    Contr[i]->name.erase();
    Contr[i]->modul.erase();
    Contr[i]->bd.erase();
    
    return(0);
}

int TTipController::PutCntrComm( string comm, int id_ctr )
{
    string info;
    char str[20];
    
    if( id_ctr >= Contr.size() || Contr[id_ctr]->stat == TCNTR_ERR || Contr[id_ctr]->stat == TCNTR_FREE ) return(-1);
    if(comm=="DISABLE")
    {
	PutCntrComm("STOP", id_ctr );
	Contr[id_ctr]->stat=TCNTR_DISABLE;
    } 
    else if(comm=="ENABLE")
    {
	Contr[id_ctr]->stat=TCNTR_ENABLE;
    }
    else if(comm=="INIT")
    {
	return( Moduls[Contr[id_ctr]->id_mod]->modul->PutCommand( comm, Contr[id_ctr]->id_contr ) );
    }
    else 
	return( 0 );
}

int TTipController::AddM( TModule *modul )
{

    int kz=TGRPModule::AddM(modul);
    if(kz < 0) return(kz);
    if(kz == TContr.size())   TContr.push_back( new STContr );
    else if(TContr[kz]==NULL) TContr[kz] = new STContr;
    return(kz);
}

int TTipController::DelM( int hd )
{
    int kz;
    kz=TGRPModule::DelM(hd);
    if(kz != 0) return(kz);
    delete TContr[kz]; TContr[kz]=NULL;
    return(0);
}


int TTipController::test( )
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

