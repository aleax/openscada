#include <stdio.h>

#include "tmodule.h"
#include "tkernel.h"
#include "tmessage.h"
#include "tbds.h"
#include "tcontrollers.h"
#include "tcontroller.h"
#include "ttipcontroller.h"

SCfgFld TTipController::Elem_Ctr[] =
{
    {"NAME" ,"Short name of controller." ,CFG_T_STRING,"","","","20","","%s"},
    {"LNAME","Description of controller.",CFG_T_STRING,"","","","50","","%s"}
};

SCfgFld TTipController::Elem_TPrm[] =
{
    {"SHIFR","Short name of parameter (TAGG).",CFG_T_STRING,"","","","20","","%s"},
    {"NAME" ,"Description of parameter"       ,CFG_T_STRING,"","","","50","","%s"}
};	

const char *TTipController::o_name = "TTipController";

TTipController::TTipController( ) 
{
    LoadElCtr( Elem_Ctr, sizeof(Elem_Ctr)/sizeof(SCfgFld) );
}

TTipController::~TTipController( )
{
    while(Size())
    {
	delete contr[0];
	contr.erase(contr.begin());
    }
    while(paramt.size())
    {
	delete paramt[0];
	paramt.erase(paramt.begin());	
    }
    while(val_el.size())
    {
	delete val_el[0];
	val_el.erase(val_el.begin());
    }
};

unsigned TTipController::Add( string name, string t_bd, string n_bd, string n_tb)
{
//    TController * (TModule::*ContrAttach)(string name, string t_bd, string n_bd, string n_tb);
//    char *n_f = "ContrAttach";	    
    unsigned i_cnt;
    
    try{ i_cnt = NameToHd(name); }
    catch(...)
    {
    	i_cnt = Size();

//    	GetFunc(n_f, (void (TModule::**)()) &ContrAttach);
//	contr.push_back((this->*ContrAttach)(name,t_bd,n_bd,n_tb));
	contr.push_back(ContrAttach(name,t_bd,n_bd,n_tb));
	
	i_cnt = HdIns(i_cnt);
	//Fill BD of default values
	for(unsigned i_tprm=0; i_tprm < paramt.size(); i_tprm++)
	    at(i_cnt)->cf_Set_S( paramt[i_tprm]->BD(),mod_Name()+'_'+name+'_'+paramt[i_tprm]->Name());
	//at(i_cnt)->Enable();
//	FreeFunc(n_f);
    }
    return(i_cnt);
}

void TTipController::Del( unsigned id )
{
    if( id >= hd.size() || hd[id] < 0 ) 
	throw TError("%s: Controller header %d error!",o_name,id);
    at(id)->Disable( );
    delete contr[hd[id]];
    contr.erase(contr.begin()+hd[id]);
    HdFree(hd[id]);
}

void TTipController::LoadElCtr( SCfgFld *elements, int numb )
{
    for(int i = 0; i < numb; i++) cfe_Add(&elements[i]);
}

int TTipController::AddTpParm(string name_t, string n_fld_bd, string descr)
{
    int i_t;
    //search type
    try
    {
	i_t = NameTpPrmToId(name_t);
    }
    catch(TError err)
    {
	//add type
	i_t = paramt.size();
	paramt.push_back(new TTipParam(name_t, descr, n_fld_bd) );
	LoadElParm(name_t, Elem_TPrm,sizeof(Elem_TPrm)/sizeof(SCfgFld));
    }

    return(i_t);
}

unsigned TTipController::NameTpPrmToId(string name_t)
{
    for(unsigned i_t=0; i_t < paramt.size(); i_t++)
	if(paramt[i_t]->Name() == name_t) return(i_t);
    throw TError("%s: %s parameter's type no avoid!",o_name,name_t.c_str());
}

int TTipController::LoadElParm(string name_t_prm, SCfgFld *elements, int numb )
{
    int i_t = NameTpPrmToId(name_t_prm);
    for(int i = 0; i < numb; i++) paramt[i_t]->cfe_Add(&elements[i]);

    return(i_t);
}

void TTipController::AddValType(string name, SVAL *vl_el, int number)
{
    unsigned id_elem, i_elem;
    
    for( id_elem = 0; id_elem < val_el.size(); id_elem++)
	if(val_el[id_elem]->vle_Name() == name) break;
    if( id_elem == val_el.size()) val_el.push_back( new TValueElem( name ));

    for( i_elem=0; i_elem < (unsigned)number; i_elem++)
	val_el[id_elem]->vle_Add(&vl_el[i_elem]);
}

void TTipController::List( vector<string> & List )
{
    List.clear();
    for(unsigned i_cntr=0; i_cntr < Size(); i_cntr++)
	List.push_back(contr[i_cntr]->Name());    
}

int TTipController::HdIns(int id)
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

void TTipController::HdFree(int id)
{
    for(unsigned i_hd=0; i_hd < hd.size(); i_hd++)
	if( hd[i_hd] == id ) { hd[i_hd] = -1; break; }
    for(unsigned i_hd=0; i_hd < hd.size(); i_hd++)
	if( hd[i_hd] > id ) hd[i_hd]--;
}

void TTipController::HdChange( int id1, int id2 )
{
    for(unsigned i_hd = 0; i_hd < hd.size(); i_hd++)
	if( hd[i_hd] == id1 )      { hd[i_hd] = id2; continue; }
    	else if( hd[i_hd] == id2 ) { hd[i_hd] = id1; continue; }
}

int TTipController::NameToHd( string Name )
{
    for(unsigned i_hd = 0; i_hd < hd.size(); i_hd++)
	if(hd[i_hd] >= 0 && contr[hd[i_hd]]->Name() == Name ) return(i_hd);
    throw TError("%s: type controller %s no avoid!",o_name,Name.c_str());
}

void TTipController::ListTpVal( vector<string> & List )
{
    for(unsigned i_val=0; i_val < val_el.size(); i_val++)
	List.push_back(val_el[i_val]->vle_Name());
}

TValueElem *TTipController::at_val( string name)
{
    for(unsigned i_val=0; i_val < val_el.size(); i_val++)
	if(val_el[i_val]->vle_Name() == name) return(val_el[i_val]); 
    throw TError("%s: value %s no avoid into controller!",o_name,name.c_str());
}

TController *TTipController::at(unsigned int id_hd )  
{ 
    if(id_hd >= hd.size() || hd[id_hd] < 0 ) 
	throw TError("%s: Controller header %d error!",o_name,id_hd); 
    return(contr[hd[id_hd]]); 
}

