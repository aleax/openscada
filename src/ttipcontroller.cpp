
#include <stdio.h>

#include "tcontroller.h"
#include "ttipcontroller.h"

TTipController::TTipController( )
{

};

TTipController::~TTipController( )
{

};

int TTipController::LoadElCtr( SElem *elements, int numb )
{
    int i_start = conf_el.Size();
    for(int i = 0; i < numb; i++) conf_el.Add(i_start+i,&elements[i]);
    return(0);
}

int TTipController::LoadElParm(int n_prm, SElem *elements, int numb )
{
    if( n_prm >= param.size() )
    {	
	n_prm = param.size();
	param.push_back(new TConfigElem);
    }
    int i_start = param[n_prm]->Size();
    for(int i = 0; i < numb; i++) param[n_prm]->Add(i_start+i,&elements[i]);
    return(n_prm);
}

int TTipController::LoadCtrCfg( int id )
{
    if(id >= contr.size()) return(-1);
    if(contr[id]->config == NULL)
	contr[id]->config = new TConfig(&conf_el);
    contr[id]->config->InitRecord(0);
    contr[id]->config->SetVal(0,"NAME",contr[id]->name);
    contr[id]->config->LoadRecValBD(0,"NAME",contr[id]->bd);
    return(0);
}

int TTipController::SaveCtrCfg( int id )
{
    if(id >= contr.size()) return(-1);
    contr[id]->config->SetVal(0,"NAME",contr[id]->name);
    contr[id]->config->SaveRecValBD(0,"NAME",contr[id]->bd);	
    return(0);
}

int TTipController::LoadParmCfg( int id_ctr, int id_prm )
{
    char   str[10];
    string parm_bd;
    
    if(id_ctr >= contr.size()) return(-1);
    if(id_prm >= param.size()) return(-2);
    for( int i_prm=0; i_prm < param.size(); i_prm++ )
    {
	if( i_prm == contr[id_ctr]->prm_cfg.size())
    	    contr[id_ctr]->prm_cfg.push_back(new TConfig(param[i_prm]));
    }
    sprintf(str,"PRM_BD%d",id_prm+1);
    if(contr[id_ctr]->config->GetVal(0,str,parm_bd) < 0) return(-3);
    if(contr[id_ctr]->prm_cfg[id_prm]->LoadValBD(parm_bd) < 0) return(-4);	    
    return(0);	
}
    
int TTipController::SaveParmCfg( int id_ctr, int id_prm )
{
    char   str[10];
    string parm_bd;

    if(id_ctr >= contr.size()) return(-1);
    if(id_prm >= param.size() || id_prm >= contr[id_ctr]->prm_cfg.size()) return(-2);
    sprintf(str,"PRM_BD%d",id_prm+1);
    if(contr[id_ctr]->config->GetVal(0,str,parm_bd) < 0) return(-3);
    if(contr[id_ctr]->prm_cfg[id_prm]->SaveValBD(parm_bd) < 0) return(-4);	    
    return(0);	
}

