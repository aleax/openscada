
#include <stdio.h>

#include "./moduls/gener/tmodule.h"
#include "tapplication.h"
#include "tmessage.h"
#include "tbd.h"
#include "tcontrollers.h"
#include "tcontroller.h"
#include "ttipcontroller.h"


TTipController::TTipController( TModule *mod ) : module(mod)
{

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

int TTipController::Add( string & name, string & bd)
{
    if(NameToHd(name) >= 0) return(-1);
    int i_cnt = Size();
    contr.push_back(new TController( this, name, bd, &conf_el ) );
    HdIns(i_cnt);
    //Fill BD of default values
    for(unsigned i_tprm=0; i_tprm < paramt.size(); i_tprm++)
	at(i_cnt)->SetVal( paramt[i_tprm]->bd,module->Name()+'_'+name+'_'+paramt[i_tprm]->name);
    at(i_cnt)->stat = TCNTR_ENABLE;

    return(i_cnt);
}

int TTipController::Del( string & name )
{
    int i_hd = NameToHd(name);
    if(i_hd < 0) return(-1);
    
    Disable(name);
    delete contr[hd[i_hd]];
    contr.erase(contr.begin()+hd[i_hd]);
    HdFree(i_hd);

    return(0);
}

int TTipController::LoadContr( string & name )
{
    int (TModule::*LoadContr)( unsigned id );
    char *n_f = "LoadContr"; 
    
    int i_hd = NameToHd(name);
    if(i_hd >= 0 && (at(hd[i_hd])->stat == TCNTR_ENABLE || at(hd[i_hd])->stat == TCNTR_RUN) )
    {
	at(hd[i_hd])->SetVal("NAME",contr[hd[i_hd]]->name);
        at(hd[i_hd])->LoadRecValBD("NAME",contr[hd[i_hd]]->bd);
 	for(unsigned i_tctr=0; i_tctr < paramt.size(); i_tctr++ )
	    at(hd[i_hd])->LoadParmCfg(i_tctr); 
	
	if(module->GetFunc(n_f, (void (TModule::**)()) &LoadContr) == MOD_ERR) return(-2);
	int kz = (module->*LoadContr)(hd[i_hd]);
	module->FreeFunc(n_f);
	return(kz);  
    }
    return(-1);  
}

int TTipController::SaveContr( string & name )
{
    int (TModule::*SaveContr)( unsigned id );
    char *n_f = "SaveContr"; 
    
    int i_hd = NameToHd(name);
    if(i_hd >= 0 && (at(hd[i_hd])->stat == TCNTR_ENABLE || at(hd[i_hd])->stat == TCNTR_RUN) )
    {
	for(unsigned i_tctr=0; i_tctr < paramt.size(); i_tctr++ )
	    at(hd[i_hd])->SaveParmCfg(i_tctr);
	
        conf_el.UpdateBDAtr( contr[hd[i_hd]]->bd );
	at(hd[i_hd])->SaveRecValBD("NAME",contr[hd[i_hd]]->bd);

	if(module->GetFunc(n_f, (void (TModule::**)()) &SaveContr) == MOD_ERR) return(-2);
	int kz = (module->*SaveContr)(hd[i_hd]);
	module->FreeFunc(n_f);
	return(kz);   
    }
    return(-1);
}

int TTipController::FreeContr( string & name )
{
    int (TModule::*FreeContr)( unsigned id );
    char *n_f = "FreeContr"; 
    
    int i_hd = NameToHd(name);
    if(i_hd >= 0 && (at(hd[i_hd])->stat == TCNTR_ENABLE || at(hd[i_hd])->stat == TCNTR_RUN) )
    {
	for(unsigned i_tctr=0; i_tctr < paramt.size(); i_tctr++ )
	    at(hd[i_hd])->FreeParmCfg(i_tctr);
	
	if(module->GetFunc(n_f, (void (TModule::**)()) &FreeContr) == MOD_ERR) return(-2);
	int kz = (module->*FreeContr)(hd[i_hd]);
	module->FreeFunc(n_f);
	return(kz);   
    }
    return(-1);
} 

int TTipController::Start( string & name )
{
    int (TModule::*StartContr)( unsigned id );
    char *n_f = "StartContr"; 
    
    int i_hd = NameToHd(name);
    if(i_hd >= 0 && at(hd[i_hd])->stat == TCNTR_ENABLE )
    {
	if(module->GetFunc(n_f, (void (TModule::**)()) &StartContr) == MOD_ERR) return(-2);
	int kz = (module->*StartContr)(hd[i_hd]);
	module->FreeFunc(n_f);
        if(kz == 0) at(hd[i_hd])->stat = TCNTR_RUN;
	return(kz);   
    }
    return(-1); 
}

int TTipController::Stop( string & name )
{
    int (TModule::*StopContr)( unsigned id );
    char *n_f = "StopContr"; 
    
    int i_hd = NameToHd(name);
    if(i_hd >= 0 && at(hd[i_hd])->stat == TCNTR_RUN )
    {
	if(module->GetFunc(n_f, (void (TModule::**)()) &StopContr) == MOD_ERR) return(-2);
	int kz = (module->*StopContr)(hd[i_hd]);
	module->FreeFunc(n_f);
        if(kz == 0) at(hd[i_hd])->stat = TCNTR_ENABLE;
	return(kz);   
    }
    return(-1); 
}

int TTipController::Enable( string & name )
{
    int i_hd = NameToHd(name);
    if(i_hd >= 0 && at(hd[i_hd])->stat == TCNTR_DISABLE )
    {
        at(hd[i_hd])->stat = TCNTR_ENABLE;
	LoadContr(name);

	return(0);   
    }
    return(-1);  
}

int TTipController::Disable( string & name )
{
    int i_hd = NameToHd(name);
    if(i_hd >= 0 && at(hd[i_hd])->stat == TCNTR_ENABLE )
    {
	if( at(hd[i_hd])->stat == TCNTR_RUN ) Stop(name);
	FreeContr(name);
        at(hd[i_hd])->stat = TCNTR_DISABLE;

	return(0);   
    }
    return(-1);   
}

void TTipController::CleanBD()
{

}

int TTipController::LoadElCtr( SElem *elements, int numb )
{
    int i_start = conf_el.Size();
    for(int i = 0; i < numb; i++) conf_el.Add(i_start+i,&elements[i]);
    return(0);
}

int TTipController::AddTpParm(string name_t, string n_fld_bd, string descr)
{
    //search type
    int i_t = NameElTpToId(name_t);
    
    if(i_t >= 0 ) return(i_t);
    //add type
    i_t = paramt.size();
    paramt.push_back(new SParamT);
    paramt[i_t]->name  = name_t;
    paramt[i_t]->descr = descr;
    paramt[i_t]->bd    = n_fld_bd;

    return(i_t);
}

int TTipController::NameElTpToId(string name_t)
{
    for(unsigned i_t=0; i_t < paramt.size(); i_t++)
	if(paramt[i_t]->name == name_t) return(i_t);

    return(-1);   
}

int TTipController::LoadElParm(string name_t_prm, SElem *elements, int numb )
{
    int i_t = NameElTpToId(name_t_prm);
    if(i_t < 0) return(-1);

    int i_start = paramt[i_t]->confs.Size();
    for(int i = 0; i < numb; i++) paramt[i_t]->confs.Add(i_start+i,&elements[i]);

    return(i_t);
}

int TTipController::AddValType(string name, SVAL *vl_el, int number)
{
    unsigned id_elem = val_el.size();
    val_el.push_back( new TValueElem( name ));
    for(unsigned i_elem=0; i_elem < number; i_elem++)
	val_el[id_elem]->Add(-1,&vl_el[i_elem]);

    return(0);
}

void TTipController::List( vector<string> & List )
{
    List.clear();
    for(unsigned i_cntr=0; i_cntr < Size(); i_cntr++)
	List.push_back(contr[i_cntr]->name);    
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

int TTipController::HdFree(int id)
{
    for(unsigned i_hd=0; i_hd < hd.size(); i_hd++)
	if( hd[i_hd] == id ) { hd[i_hd] = -1; break; }
    for(unsigned i_hd=0; i_hd < hd.size(); i_hd++)
	if( hd[i_hd] > id ) hd[i_hd]--;

    return(0);
}

int TTipController::HdChange( int id1, int id2 )
{
    for(unsigned i_hd = 0; i_hd < hd.size(); i_hd++)
	if( hd[i_hd] == id1 )      { hd[i_hd] = id2; continue; }
    else if( hd[i_hd] == id2 ) { hd[i_hd] = id1; continue; }

    return(0);
}

int TTipController::NameToHd( string Name )
{
    for(unsigned i_hd = 0; i_hd < hd.size(); i_hd++)
	if(hd[i_hd] >= 0 && contr[hd[i_hd]]->name == Name ) return(i_hd);

    return(-1);
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

    return(NULL);
}


