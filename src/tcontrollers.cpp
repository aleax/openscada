#include <getopt.h>

#include "tapplication.h"
#include "tmessage.h"
#include "tbd.h"
#include "tcontroller.h"
#include "ttipcontroller.h"
#include "tmodule.h"
#include "tvalue.h"
#include "tcontrollers.h"


SCfgFld TControllerS::gen_elem[] =
{
    {"NAME"  ,"Controller's name."                     ,CFG_T_STRING              ,"","",""     ,"20",""          ,"%s"},
    {"MODUL" ,"Module(plugin) of type controler."      ,CFG_T_STRING              ,"","",""     ,"20",""          ,"%s"},
    {"BDNAME","Name controller's BD."                  ,CFG_T_STRING              ,"","",""     ,"20",""          ,"%s"},
    {"STAT"  ,"Controller's stat (0-disable;1-enable).",CFG_T_BOOLEAN|CFG_T_SELECT,"","","false","1" ,"false;true","%s","Disable,Enable"}
};

const char *TControllerS::o_name = "TControllerS";

TControllerS::TControllerS(  ) : TGRPModule("Controller"), gener_bd("generic")
{
    for(int i = 0; i < sizeof(gen_elem)/sizeof(SCfgFld); i++) gener_ecfg.Add(&gen_elem[i]);    
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
	TContr[Contr[i].id_mod]->at(Contr[i].name)->Start( );

    return(0);
}

int TControllerS::StopAll(  )
{
//    LoadBD();
    for(int i=0; i< Contr.size(); i++)
	TContr[Contr[i].id_mod]->at(Contr[i].name)->Stop( );

    return(0);
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
	    case 'h': pr_opt_descr(stdout); break;
	    case 'm': DirPath  = optarg;    break;
	    case 'b': gener_bd = optarg;    break;
	    case -1 : break;
	}
    } while(next_opt != -1);
}

void TControllerS::UpdateOpt()
{

}

void TControllerS::LoadBD()
{
    string cell;
    bool   reload = false;

    int b_hd = App->BD->OpenBD(gener_bd);
    for(int i=0; i < App->BD->NLines(b_hd); i++)
    {
	//Get Controller's name
	cell = App->BD->GetCellS(b_hd,App->BD->ColumNameToId(b_hd,"NAME"),i);
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
	    Contr[ii].config = new TConfig(&gener_ecfg);
	    HdIns(Size()-1);
#if debug
    	    App->Mess->put(0, "Add Contr %s: %d !",cell.c_str(),ii);
#endif
    	    Contr[ii].name=cell;
	}
	// Add/modify controller
	Contr[ii].config->Set_S("NAME",Contr[ii].name);
	Contr[ii].config->LoadRecValBD("NAME",b_hd);
        
	try{ Contr[ii].id_mod = name_to_id(Contr[ii].config->Get_S("MODUL")); }
	catch(...) { Contr[ii].id_mod = -1; continue; }
	TContr[Contr[ii].id_mod]->idmod = Contr[ii].id_mod;            //????
	if(reload == false)
	    Contr[ii].id_contr = TContr[Contr[ii].id_mod]->Add(Contr[ii].name, Contr[ii].config->Get_S("BDNAME"));
	if( Contr[ii].config->Get_B("STAT") == false ) 
	    TContr[Contr[ii].id_mod]->at(Contr[ii].id_contr)->Disable();
	else
	    TContr[Contr[ii].id_mod]->at(Contr[ii].id_contr)->Enable();
    }
    App->BD->CloseBD(b_hd);
}


int TControllerS::UpdateBD(  )
{
    int i;
    string cell, stat;
    
    //Update general BD
    int b_hd = App->BD->OpenBD(gener_bd);
    //Find deleted controllers
    while(App->BD->NLines(b_hd)) App->BD->DelLine(b_hd,0);

    //Modify present and add new controllers
    for(i=0;i < Size(); i++)
    {
	gener_ecfg.UpdateBDAtr( b_hd );
        Contr[i].config->SaveRecValBD("NAME",b_hd);
	TContr[Contr[i].id_mod]->at(Contr[i].name)->Save();	
    }
    App->BD->SaveBD(b_hd);
    App->BD->CloseBD(b_hd);

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
    Contr[i].config = new TConfig(&gener_ecfg);
    HdIns(Size()-1);
    //Fill record
    Contr[i].name  = name;
    Contr[i].config->Set_S("NAME",name);
    Contr[i].config->Set_S("MODUL",tip);
    Contr[i].id_mod= name_to_id(tip);
    Contr[i].config->Set_S("BDNAME",bd);
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
    delete Contr[i_cnt].config;
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
    if(id_hd >= hd.size() || id_hd < 0 || hd[id_hd] < 0 ) 
	throw TError("%s: header error!",o_name); 
    if(Contr[hd[id_hd]].id_mod < 0 || Contr[hd[id_hd]].id_contr < 0 ) 
	throw TError("%s: module or object no avoid!",o_name);
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
    throw TError("%s: %s controller no avoid!",o_name,Name.c_str());
}

TTipController *TControllerS::at_tp( string name )
{
    for(unsigned i_cntrt = 0; i_cntrt < TContr.size(); i_cntrt++)
	if(TContr[i_cntrt]!=TO_FREE && TContr[i_cntrt]->Name() == name )
	    return(TContr[i_cntrt]);
    throw TError("%s: %s type controller no avoid!",o_name,name.c_str()); 	
}


