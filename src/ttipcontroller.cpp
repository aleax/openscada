#include <stdio.h>

#include "tmodule.h"
#include "tkernel.h"
#include "tmessage.h"
#include "tbds.h"
#include "tcontrollers.h"
#include "ttiparam.h"
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
    LoadCfg( Elem_Ctr, sizeof(Elem_Ctr)/sizeof(SCfgFld) );
}

TTipController::~TTipController( )
{
    vector<string> c_list;

    m_hd_cntr.lock();
    list(c_list);
    for( unsigned i_ls = 0; i_ls < c_list.size(); i_ls++)
    	del(c_list[i_ls]);
	
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

void TTipController::add( string name, SBDS &bd )
{   
    TController *cntr = ContrAttach( name, bd.tp, bd.bd, bd.tbl );
    try
    { 
	//Fill BD of default values
	for(unsigned i_tprm=0; i_tprm < paramt.size(); i_tprm++)
	    cntr->cf_Set_S( paramt[i_tprm]->BD(),mod_Name()+'_'+name+'_'+paramt[i_tprm]->Name());	
	    
	m_hd_cntr.hd_obj_add( cntr, &cntr->Name() ); 
    }
    catch(TError err) { delete cntr; }
}

void TTipController::LoadCfg( SCfgFld *elements, int numb )
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
	LoadTpParmCfg(name_t, Elem_TPrm,sizeof(Elem_TPrm)/sizeof(SCfgFld));
    }

    return(i_t);
}

unsigned TTipController::NameTpPrmToId(string name_t)
{
    for(unsigned i_t=0; i_t < paramt.size(); i_t++)
	if(paramt[i_t]->Name() == name_t) return(i_t);
    throw TError("%s: %s parameter's type no avoid!",o_name,name_t.c_str());
}

int TTipController::LoadTpParmCfg(string name_t_prm, SCfgFld *elements, int numb )
{
    int i_t = NameTpPrmToId(name_t_prm);
    for(int i = 0; i < numb; i++) paramt[i_t]->cfe_Add(&elements[i]);

    return(i_t);
}

void TTipController::AddTpVal(string name, SVAL *vl_el, int number)
{
    unsigned id_elem, i_elem;
    
    for( id_elem = 0; id_elem < val_el.size(); id_elem++)
	if(val_el[id_elem]->vle_Name() == name) break;
    if( id_elem == val_el.size()) val_el.push_back( new TValueElem( name ));

    for( i_elem=0; i_elem < (unsigned)number; i_elem++)
	val_el[id_elem]->vle_Add(&vl_el[i_elem]);
}

void TTipController::ListTpVal( vector<string> & List )
{
    for(unsigned i_val=0; i_val < val_el.size(); i_val++)
	List.push_back(val_el[i_val]->vle_Name());
}

TValueElem &TTipController::at_TpVal( string name)
{
    for(unsigned i_val=0; i_val < val_el.size(); i_val++)
	if(val_el[i_val]->vle_Name() == name) return(*val_el[i_val]); 
    throw TError("%s: value %s no avoid into controller!",o_name,name.c_str());
}

