
#include "tbd.h"
#include "tmessage.h"
#include "tcontrollers.h"
#include "ttipcontroller.h"
#include "tparamcontr.h"
#include "tcontroller.h"


//==== TController ====
 TController::TController( TTipController *tcntr, string name_c, string bd_c, TConfigElem *cfgelem) : 
		name(name_c), bd(bd_c), TContr(tcntr), TConfig(cfgelem)
{
    for( unsigned i_prm=0; i_prm < TContr->paramt.size(); i_prm++ )
	if( i_prm == prm_cfg.size()) prm_cfg.push_back();

    SetVal("NAME",name_c);    
}

 TController::~TController(  )
{
    while(prm_cfg.size())
    {
	while(prm_cfg[0].size())
	{
	    delete prm_cfg[0][0];
	    prm_cfg[0].erase(prm_cfg[0].begin());
	}
	prm_cfg.erase(prm_cfg.begin());
    }
}

int TController::LoadParmCfg( string name_t_prm )
{
    unsigned i_t = TContr->NameElTpToId(name_t_prm);
    if(i_t < 0) return(-1);

    return( LoadParmCfg(i_t) );	    
}

int TController::LoadParmCfg( unsigned i_t )
{
    int         b_hd;
    string      parm_bd;
    TParamContr *PrmCntr;

    if( i_t >= prm_cfg.size()) return(-1);

    if(GetVal(TContr->paramt[i_t]->bd,parm_bd) < 0) return(-2);
    b_hd = App->BD->OpenBD(parm_bd);
    if(b_hd < 0) return(-3);

    time_t tm = time(NULL);
    for(int i=0; i < App->BD->NLines(b_hd); i++)
    {
	//Load param config fromBD
        PrmCntr = new TParamContr(this ,&TContr->paramt[i_t]->confs);   
	PrmCntr->LoadRecValBD(i,b_hd); PrmCntr->t_sync=tm;
	//!!! Want request resource
        //Find already loading param
        unsigned i_prm;
        for(i_prm=0; i_prm < prm_cfg[i_t].size(); i_prm++)
	    if(*PrmCntr == *prm_cfg[i_t][i_prm]) break;
	if( i_prm == prm_cfg[i_t].size())
	{
	    prm_cfg[i_t].push_back(PrmCntr);
	    HdIns(i_t,i_prm);
	}
	else
	{
	    *prm_cfg[i_t][i_prm] = *PrmCntr;
	    delete PrmCntr;
	}
	//Check freeing param
        for(i_prm=0; i_prm < prm_cfg[i_t].size(); i_prm++)
	    if( tm != prm_cfg[i_t][i_prm]->t_sync )
            {
    		App->Param->Del(prm_cfg[i_t][i_prm]);
		HdFree(i_t,i_prm);		
 		delete prm_cfg[i_t][i_prm];
		prm_cfg[i_t].erase(prm_cfg[i_t].begin()+i_prm);
		i_prm--;
	    }
	//!!! Want free resource
	
    }
    App->BD->CloseBD(b_hd);

    return(0);
}

int TController::SaveParmCfg( string name_t_prm )
{
    unsigned i_t = TContr->NameElTpToId(name_t_prm);
    if(i_t < 0) return(-1);

    return( SaveParmCfg(i_t) );
}

int TController::SaveParmCfg( unsigned i_t )
{
    string parm_bd;
    int    b_hd;

    if( i_t >= prm_cfg.size()) return(-1);
    if(GetVal(TContr->paramt[i_t]->bd,parm_bd) < 0) return(-2);
    
    //Update BD (resize, change atributes ..
    TContr->paramt[i_t]->confs.UpdateBDAtr( parm_bd );
    
    b_hd = App->BD->OpenBD(parm_bd);
    if(b_hd < 0) return(-3);
    //Clear BD
    while(App->BD->NLines(b_hd)) App->BD->DelLine(b_hd,0);
    time_t tm = time(NULL);
    for(int i_ln=0; i_ln < prm_cfg[i_t].size(); i_ln++)
    {
    	App->BD->AddLine(b_hd,i_ln);
	prm_cfg[i_t][i_ln]->SaveRecValBD(i_ln,b_hd);
        prm_cfg[i_t][i_ln]->t_sync=tm;
    }
    App->BD->SaveBD(b_hd);
    App->BD->CloseBD(b_hd);

    return(0);
}

int TController::FreeParmCfg( string name_t_prm )
{
    unsigned i_t = TContr->NameElTpToId(name_t_prm);
    if(i_t < 0) return(-1);

    return( FreeParmCfg(i_t) );	   
}

int TController::FreeParmCfg( unsigned i_t )
{
    if( i_t >= prm_cfg.size())  return(-1);

    //!!! Want request resource
    while(prm_cfg[i_t].size())
    {
	App->Param->Del(prm_cfg[i_t][0]);
	HdFree(i_t,0);		
	delete prm_cfg[i_t][0];
	prm_cfg[i_t].erase(prm_cfg[i_t].begin());
    }
    //!!! Want free resource

    return(0);    
}

int TController::RegParamS()
{
    for(int i_prm_t = 0; i_prm_t < prm_cfg.size(); i_prm_t++)
	for(int i_prm = 0; i_prm < prm_cfg[i_prm_t].size(); i_prm++)
	    App->Param->Add(prm_cfg[i_prm_t][i_prm]);

    return(0);
}

int TController::UnRegParamS()
{
    for(int i_prm_t = 0; i_prm_t < prm_cfg.size(); i_prm_t++)
	for(int i_prm = 0; i_prm < prm_cfg[i_prm_t].size(); i_prm++)
    	    App->Param->Del(prm_cfg[i_prm_t][i_prm]);

    return(0);
}

void TController::List( string Name_TP, vector<string> & List)
{
    List.clear();
    if(stat == TCNTR_DISABLE || stat == TCNTR_ERR ) return;

    int i_el = TContr->NameElTpToId(Name_TP);
    if(i_el < 0) return;
    for(unsigned i_prmc=0; i_prmc < prm_cfg[i_el].size(); i_prmc++)
	List.push_back(prm_cfg[i_el][i_prmc]->Name());
}


int TController::Add( string Name_TP, string name, int pos )
{
    unsigned i_prmc;
    
    TParamContr *PrmCntr;
    if(stat == TCNTR_DISABLE || stat == TCNTR_ERR ) return(-1);
    
    int id_el = TContr->NameElTpToId(Name_TP);
    if(id_el < 0) return(-2);
    //!!! Want request resource
    //Find param name
    for(unsigned i_el=0; i_el < prm_cfg.size(); i_el++)
	for(i_prmc=0; i_prmc < prm_cfg[i_el].size(); i_prmc++)
	    if(prm_cfg[i_el][i_prmc]->Name() == name ) 
	    {
		//!!! Want free resource    		
		return(-3);    
	    }
    
    PrmCntr = new TParamContr(this ,&TContr->paramt[id_el]->confs);
    PrmCntr->SetVal("SHIFR",name);  PrmCntr->t_sync = time(NULL);
    if(pos < 0 || pos >= prm_cfg[id_el].size() ) pos = prm_cfg[id_el].size();
    prm_cfg[id_el].insert(prm_cfg[id_el].begin() + pos,PrmCntr);
    HdIns(id_el,pos);
    App->Param->Add(prm_cfg[id_el][pos]);
    //!!! Want free resource    

    return(0);
}

int TController::Del( string Name_TP, string name )
{
    if(stat == TCNTR_DISABLE || stat == TCNTR_ERR ) return(-1);

    int i_el = TContr->NameElTpToId(Name_TP);
    if(i_el < 0) return(-2); 
    //!!! Want request resource
    for(unsigned i_prmc=0; i_prmc < prm_cfg[i_el].size(); i_prmc++)
	if(prm_cfg[i_el][i_prmc]->Name() == name )
	{    
	    App->Param->Del(prm_cfg[i_el][i_prmc]);
	    delete prm_cfg[i_el][i_prmc];
	    prm_cfg[i_el].erase(prm_cfg[i_el].begin() + i_prmc);
	    HdFree(i_el,i_prmc);		
	    //!!! Want free resource    	    
	    return(0);
	}
    //!!! Want free resource    	    

    return(-1);
}
    
int TController::Rotate( string Name_P, string name1, string name2)
{
    int id1= -1,id2= -1;
	
    if(stat == TCNTR_DISABLE || stat == TCNTR_ERR ) return(-1);

    int i_el = TContr->NameElTpToId(Name_P);
    if(i_el < 0) return(-2); 
    //!!! Want request resource
    for(unsigned i_prmc=0; i_prmc < prm_cfg[i_el].size(); i_prmc++)
	if(prm_cfg[i_el][i_prmc]->Name() == name1 ) 
	{ id1 = i_prmc; break; }
    for(unsigned i_prmc=0; i_prmc < prm_cfg[i_el].size(); i_prmc++)
	if(prm_cfg[i_el][i_prmc]->Name() == name2 ) 
	{ id2 = i_prmc; break; }
    if(id1 < 0 || id2 < 0 || id1 == id2) 
    {
	//!!! Want free resource
	return(-2);
    }
    TParamContr *PrmCntr = prm_cfg[i_el][id1];    
    prm_cfg[i_el][id1]   = prm_cfg[i_el][id2];    
    prm_cfg[i_el][id2]   = PrmCntr;
    HdChange(i_el,id1,id2);
    //!!! Want free resource
}

int TController::HdIns(int id_P, int id)
{
    unsigned i_hd;
    for( i_hd=0; i_hd < hd.size(); i_hd++)
	if(hd[i_hd].tprm == id_P && hd[i_hd].prm >= id ) hd[i_hd].prm++;
    for( i_hd=0; i_hd < hd.size(); i_hd++)
	if(hd[i_hd].tprm < 0 ) break;
    if( i_hd == hd.size() ) hd.push_back();
    hd[i_hd].tprm = id_P; 
    hd[i_hd].prm = id; 

    return(i_hd);    
}

int TController::HdFree(int id_P, int id)
{
    for(unsigned i_hd=0; i_hd < hd.size(); i_hd++)
	if(hd[i_hd].tprm == id_P && hd[i_hd].prm == id ) 
	{ hd[i_hd].tprm = -1; break; } 
    for(unsigned i_hd=0; i_hd < hd.size(); i_hd++)
	if( hd[i_hd].tprm == id_P && hd[i_hd].prm > id ) hd[i_hd].prm--;

    return(0);    
} 

int TController::HdChange( int id_P, int id1, int id2 )
{
    for(unsigned i_hd = 0; i_hd < hd.size(); i_hd++)
	if(hd[i_hd].tprm == id_P && hd[i_hd].prm == id1 )      { hd[i_hd].prm = id2; continue; }
	else if(hd[i_hd].tprm == id_P && hd[i_hd].prm == id2 ) { hd[i_hd].prm = id1; continue; }

    return(0);
}


int TController::NameToHd( string Name )
{
    for(unsigned i_hd = 0; i_hd < hd.size(); i_hd++)
	if(hd[i_hd].tprm >= 0 && prm_cfg[hd[i_hd].tprm][hd[i_hd].prm]->Name() == Name ) return(i_hd);

    return(-1);
}

TParamContr *TController::at(int id_hd)    
{
    if(id_hd < 0 || hd[id_hd].tprm < 0) return(NULL);
    return(prm_cfg[hd[id_hd].tprm][hd[id_hd].prm]);
}


