#include <stdio.h>

#include "tmodule.h"
#include "tapplication.h"
#include "tmessage.h"
#include "tbd.h"
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

TTipController::TTipController( TModule *mod ) : module(mod)
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

int TTipController::Add( string name, string bd)
{
    TController * (TModule::*ContrAttach)(string name, string bd);
    char *n_f = "ContrAttach";	    
    
    try{ NameToHd(name); }
    catch(...)
    {
    	int i_cnt = Size();

    	module->GetFunc(n_f, (void (TModule::**)()) &ContrAttach);
	contr.push_back((module->*ContrAttach)(name,bd));
	//contr.push_back(new TController( this, name, bd, &conf_el ) );
	HdIns(i_cnt);
	//Fill BD of default values
	for(unsigned i_tprm=0; i_tprm < paramt.size(); i_tprm++)
	    at(i_cnt)->Set_S( paramt[i_tprm]->bd,module->Name()+'_'+name+'_'+paramt[i_tprm]->name);
	//at(i_cnt)->Enable();
	module->FreeFunc(n_f);

	return(i_cnt);
    }
}

void TTipController::Del( string name )
{
    int i_hd = NameToHd(name);
    at(name)->Disable( );
    delete contr[hd[i_hd]];
    contr.erase(contr.begin()+hd[i_hd]);
    HdFree(i_hd);
}

void TTipController::LoadElCtr( SCfgFld *elements, int numb )
{
    for(int i = 0; i < numb; i++) conf_el.Add(&elements[i]);
}

int TTipController::AddTpParm(string name_t, string n_fld_bd, string descr)
{
    int i_t;
    //search type
    try
    {
	i_t = NameElTpToId(name_t);
    }
    catch(TError err)
    {
	//add type
	i_t = paramt.size();
	paramt.push_back(new SParamT);
	paramt[i_t]->name  = name_t;
	paramt[i_t]->descr = descr;
	paramt[i_t]->bd    = n_fld_bd;
	LoadElParm(name_t, Elem_TPrm,sizeof(Elem_TPrm)/sizeof(SCfgFld));
    }

    return(i_t);
}

int TTipController::NameElTpToId(string name_t)
{
    for(unsigned i_t=0; i_t < paramt.size(); i_t++)
	if(paramt[i_t]->name == name_t) return(i_t);

    throw TError("%s: %s parameter's type no avoid!",o_name,name_t.c_str());
}

int TTipController::LoadElParm(string name_t_prm, SCfgFld *elements, int numb )
{
    int i_t = NameElTpToId(name_t_prm);
    for(int i = 0; i < numb; i++) paramt[i_t]->confs.Add(&elements[i]);

    return(i_t);
}

void TTipController::AddValType(string name, SVAL *vl_el, int number)
{
    unsigned id_elem = val_el.size();
    val_el.push_back( new TValueElem( name ));
    for(unsigned i_elem=0; i_elem < number; i_elem++)
	val_el[id_elem]->Add(&vl_el[i_elem]);
}

void TTipController::List( vector<string> & List )
{
    List.clear();
    for(unsigned i_cntr=0; i_cntr < Size(); i_cntr++)
	List.push_back(contr[i_cntr]->Name());    
}

void TTipController::ListTpPrm( vector<string> & List )
{
    List.clear();
    for(unsigned i_prmt=0; i_prmt < paramt.size(); i_prmt++)
	List.push_back(paramt[i_prmt]->name);
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
	List.push_back(val_el[i_val]->Name());
}

TValueElem *TTipController::at_val( string name)
{
    for(unsigned i_val=0; i_val < val_el.size(); i_val++)
	if(val_el[i_val]->Name() == name) return(val_el[i_val]); 
    throw TError("%s: value %s no avoid into controller!",o_name,name.c_str());
}


