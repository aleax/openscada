#include <getopt.h>

#include "tapplication.h"
#include "tmessage.h"
#include "tbd.h"
#include "ttipcontroller.h"

 TTipController::TTipController(  ) : TGRPModule("Controller"), gener_bd("generic") 
{
    
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

void TTipController::ContrTipList( string & List )
{
    int i;
    
    List.erase();
    for(i=0;i < Moduls.size(); i++)
	if(Moduls[i]->stat != GRM_ST_OFF) 
	    List=List+Moduls[i]->name+',';
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
	    case 'h': pr_opt_descr(stdout); break;
	    case 'm': DirPath=strdup(optarg); break;
	    case 'b': gener_bd.assign(optarg); break;
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
	if(App->BD->GetCell(b_hd,"NAME",i,cell) != 0) return;
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
	App->BD->GetCell(b_hd,"MODUL",i,cell);
	Contr[ii]->modul=cell;
	Contr[ii]->id_mod=name_to_id(cell);
	App->BD->GetCell(b_hd,"BDNAME",i,cell);
	Contr[ii]->bd=cell;
	App->BD->GetCell(b_hd,"STAT",i,cell);
	if(atoi(cell.c_str())==0) Contr[ii]->stat = TCNTR_DISABLE;
	else                      Contr[ii]->stat = TCNTR_ENABLE;
	PutCntrComm("INIT", ii );
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
	App->BD->GetCell(b_hd,"NAME",i,cell);
	
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
	    App->BD->GetCell(b_hd,"name",ii,cell);
    	    if(cell==Contr[i]->name) break;
	}
	if(ii == App->BD->NLines(b_hd)) App->BD->AddLine(b_hd,ii);
	App->BD->SetCell(b_hd,"NAME",ii,Contr[i]->name);
	App->BD->SetCell(b_hd,"MODUL",ii,Contr[i]->modul);
	App->BD->SetCell(b_hd,"BDNAME",ii,Contr[i]->bd);
	if(Contr[i]->stat==TCNTR_DISABLE) stat='0';
	else stat='1';
	App->BD->SetCell(b_hd,"STAT",ii,stat);
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
	if(Moduls[i]->stat != GRM_ST_OFF && Moduls[i]->name == tip) break;
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
    Contr[i]->bd    = bd;
    Contr[i]->stat  = TCNTR_DISABLE;
    //Add controller into modul
    PutCntrComm("ADD", i );	
   
    return(0);
}

int TTipController::DelContr( string name )
{
    int i;

    //Find Controller 
    for(i=0;i < Contr.size(); i++)
    	if(Contr[i]->stat == TCNTR_FREE && Contr[i]->name == name) break;
    if(i == Contr.size())   return(-1);
    //Delete controller into modul
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
    
    if(comm=="DISABLE")
    {
	PutCntrComm("STOP", id_ctr );
	Contr[id_ctr]->stat=TCNTR_DISABLE;
    } 
    else if(comm=="ENABLE")
    {
	Contr[id_ctr]->stat=TCNTR_ENABLE;
    }
    else
    {
	//controller command
    }
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
    App->BD->SetCell(hd_b,"NAME",id,"TEST1");
    App->BD->SetCell(hd_b,"MODUL",id,"virt");
    App->BD->SetCell(hd_b,"BDNAME",id,"bd_virt");
    App->BD->SetCell(hd_b,"STAT",id,"1");
    id=App->BD->AddLine(hd_b,1000);
    App->Mess->put(0, "Add line: %d !",id);
    App->BD->SetCell(hd_b,"NAME",id,"TEST2");
    App->BD->SetCell(hd_b,"MODUL",id,"virt");
    App->BD->SetCell(hd_b,"BDNAME",id,"bd_virt");
    App->BD->SetCell(hd_b,"STAT",id,"0");
    id=App->BD->SaveBD(hd_b);
    id=App->BD->CloseBD(hd_b);
}

