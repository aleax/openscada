#include <getopt.h>

#include "tkernel.h"
#include "tmessage.h"
#include "tbds.h"
#include "tcontroller.h"
#include "ttipcontroller.h"
#include "tmodule.h"
#include "tvalue.h"
#include "tcontrollers.h"


SCfgFld TControllerS::gen_elem[] =
{
    {"NAME"  ,"Controller's name."                     ,CFG_T_STRING              ,"","",""           ,"20",""          ,"%s"},
    {"MODUL" ,"Module(plugin) of type controler."      ,CFG_T_STRING              ,"","",""           ,"20",""          ,"%s"},
    {"BDTYPE","Type controller's BD."                  ,CFG_T_STRING              ,"","","direct_dbf" ,"20",""          ,"%s"},
    {"BDNAME","Name controller's BD."                  ,CFG_T_STRING              ,"","","./DATA"     ,"50",""          ,"%s"},
    {"TABLE" ,"Name controller's Table."               ,CFG_T_STRING              ,"","","contr.dbf"  ,"20",""          ,"%s"},
    {"STAT"  ,"Controller's stat (0-disable;1-enable).",CFG_T_BOOLEAN|CFG_T_SELECT,"","","false"      ,"1" ,"false;true","%s","Disable,Enable"}
};

const char *TControllerS::o_name = "TControllerS";
const char *TControllerS::n_opt  = "Controller";

TControllerS::TControllerS( TKernel *app ) : TGRPModule(app,"Controller"), t_bd("direct_dbf"), n_bd("./DATA"), 
	n_tb("generic.dbf")
{
    for(unsigned i = 0; i < sizeof(gen_elem)/sizeof(SCfgFld); i++) gener_ecfg.Add(&gen_elem[i]);    
}

TControllerS::~TControllerS(  )
{
    vector<string> List;
    ContrList(List);
    for(unsigned i_ctr = 0; i_ctr < List.size(); i_ctr++) 
	DelContr( List[i_ctr] );
    for(unsigned i_m = 0; i_m < TContr.size(); i_m++) DelM(i_m);
}

void TControllerS::InitAll( )
{
    TGRPModule::InitAll();
    LoadBD();
}

int TControllerS::StartAll(  )         
{
    for(unsigned i=0; i< Contr.size(); i++)
	if( Contr[i].id_mod >= 0 ) TContr[Contr[i].id_mod]->at(Contr[i].name)->Start( );

    return(0);
}

int TControllerS::StopAll(  )
{
//    LoadBD();
    for(unsigned i=0; i< Contr.size(); i++)
	if( Contr[i].id_mod >= 0 ) TContr[Contr[i].id_mod]->at(Contr[i].name)->Stop( );

    return(0);
}

void TControllerS::ContrList( vector<string> & List )
{
    List.clear();
    for(unsigned i=0;i < Size(); i++)
	if(Contr[i].id_mod >= 0 ) 
	    List.push_back(Contr[i].name);
}


void TControllerS::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "========================= TipController options ===========================\n"
    "    --TCModPath=<path>   Set moduls <path>;\n"
    "    --TCTypeGenBD=<name> Set a name of type generic BD (default \"direct_dbf\");\n"
    "    --TCNameGenBD=<name> Set a name of generic BD (default \"./DATA\");\n"
    "    --TCNameGenTB=<name> Set a name of generic table (default \"generic.dbf\");\n"
    "------------------ Fields <%s> sections of config file ----------------\n"
    " modules_path = <path>   path to modules;\n"
    " TypeGenBD    = <name>   type generic bd (modules name);\n"
    " NameGenBD    = <name>   name generic bd;\n"    
    " NameGenTB    = <name>   name generic table;\n"
    "\n",n_opt);
}

void TControllerS::CheckCommandLine( char **argv, int argc )
{
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{"TCModPath"  ,1,NULL,'m'},
	{"TCTypeGenBD",1,NULL,'t'},
	{"TCNameGenBD",1,NULL,'b'},
	{"TCNameGenTB",1,NULL,'l'},	
	{NULL         ,0,NULL,0  }
    };

    optind=opterr=0;	
    do
    {
	next_opt=getopt_long(argc,argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': pr_opt_descr(stdout); break;
	    case 'm': DirPath  = optarg;    break;
	    case 't': t_bd     = optarg;    break;
	    case 'b': n_bd     = optarg;    break;
	    case 'l': n_tb     = optarg;    break;
	    case -1 : break;
	}
    } while(next_opt != -1);
}

void TControllerS::UpdateOpt()
{
    try{ DirPath = owner->GetOpt(n_opt,"modules_path"); } catch(...){  }
    try{ t_bd    = owner->GetOpt(n_opt,"TypeGenBD"); }    catch(...){  }
    try{ n_bd    = owner->GetOpt(n_opt,"NameGenBD"); }    catch(...){  }
    try{ n_tb    = owner->GetOpt(n_opt,"NameGenTB"); }    catch(...){  }
}

void TControllerS::LoadBD()
{
    string cell;
    bool   reload = false;
    
    int b_hd = owner->BD->OpenTable(t_bd,n_bd,n_tb);
    for(int i=0; i < owner->BD->at_tbl(b_hd)->NLines( ); i++)
    {
	//Get Controller's name
	cell = owner->BD->at_tbl(b_hd)->GetCellS(owner->BD->at_tbl(b_hd)->ColumNameToId("NAME"),i);
	//Find duplicate of controllers
	unsigned ii;
	for(ii=0;ii < Size(); ii++)
	    if(cell == Contr[ii].name) break;
	if(ii < Size())
	{
	    reload = true;
#if OSC_DEBUG
    	    Mess->put(0, "Reload controller %s: %d !",cell.c_str(),ii);
#endif
	}
	else
	{
	    reload = false;
    	    Contr.push_back();
	    Contr[ii].config = new TConfig(&gener_ecfg);
	    HdIns(Size()-1);
#if OSC_DEBUG
    	    Mess->put(0, "Add Contr %s: %d !",cell.c_str(),ii);
#endif
    	    Contr[ii].name=cell;
	}
	// Add/modify controller
	Contr[ii].config->Set_S("NAME",Contr[ii].name);
	Contr[ii].config->LoadValBD("NAME",owner->BD->at_tbl(b_hd));
        
	try{ Contr[ii].id_mod = NameToId(Contr[ii].config->Get_S("MODUL")); }
	catch(...) { Contr[ii].id_mod = -1; continue; }
	TContr[Contr[ii].id_mod]->idmod = Contr[ii].id_mod;            //????
	if(reload == false)
	    Contr[ii].id_contr = TContr[Contr[ii].id_mod]->Add(Contr[ii].name,
		    Contr[ii].config->Get_S("BDTYPE"), 
		    Contr[ii].config->Get_S("BDNAME"), 
		    Contr[ii].config->Get_S("TABLE"));
	if( Contr[ii].config->Get_B("STAT") == false ) 
	    TContr[Contr[ii].id_mod]->at(Contr[ii].id_contr)->Disable();
	else
	    TContr[Contr[ii].id_mod]->at(Contr[ii].id_contr)->Enable();
    }
    
    owner->BD->CloseTable(b_hd);
}


int TControllerS::UpdateBD(  )
{
    string cell, stat;
    
    //Update general BD
    int b_hd = owner->BD->OpenTable(t_bd,n_bd,n_tb);
    //Find deleted controllers
    while(owner->BD->at_tbl(b_hd)->NLines()) owner->BD->at_tbl(b_hd)->DelLine(0);

    //Modify present and add new controllers
    for(unsigned i=0;i < Size(); i++)
    {
	gener_ecfg.UpdateBDAttr( owner->BD->at_tbl(b_hd) );
        Contr[i].config->SaveValBD("NAME",owner->BD->at_tbl(b_hd));
	TContr[Contr[i].id_mod]->at(Contr[i].name)->Save();	
    }
    owner->BD->at_tbl(b_hd)->Save();
    owner->BD->CloseTable(b_hd);

    return(0);
}

int TControllerS::AddContr( string name, string tip, string t_bd, string n_bd, string n_tb )
{
    unsigned i;
    //!!! Want request resource 
    if(NameToId(tip) < 0) return(-1);
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
    Contr[i].id_mod= NameToId(tip);
    Contr[i].config->Set_S("BDTYPE",t_bd);
    Contr[i].config->Set_S("BDNAME",n_bd);
    Contr[i].config->Set_S("TABLE",n_tb);
    Contr[i].id_contr = TContr[Contr[i].id_mod]->Add(name,t_bd,n_bd,n_tb);
   
    return(0);
}

int TControllerS::DelContr( string name )
{
    unsigned i_cnt;

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
    int hd=TGRPModule::AddM(modul);
    if(hd < 0) return(hd);
    if(hd == (int)TContr.size()) TContr.push_back( static_cast< TTipController *>(modul) );
    else if(TContr[hd]==TO_FREE) TContr[hd] = static_cast< TTipController *>(modul);
    TContr[hd]->owner = this;
    
    return(hd);
}

int TControllerS::DelM( int hd )
{
    if(hd >= TContr.size() || TContr[hd]==TO_FREE) return(-1);
    TContr[hd]=TO_FREE;
    return(TGRPModule::DelM(hd));
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

TController *TControllerS::at( unsigned id_hd)
{ 
    if(id_hd >= hd.size() || hd[id_hd] < 0 ) 
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


