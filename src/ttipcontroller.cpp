
#include <stdio.h>

#include "./moduls/gener/tmodule.h"
#include "tbd.h"
#include "tcontrollers.h"
#include "tcontroller.h"
#include "ttipcontroller.h"


TTipController::TTipController( TModule *mod ) : module(mod)
{

};

TTipController::~TTipController( )
{
    while(contr.size())
    {
	delete contr[0];
	contr.erase(contr.begin());
    }
    while(param.size())
    {
	delete param[0];
	param.erase(param.begin());
    }
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
    contr[id]->SetVal("NAME",contr[id]->name);
    contr[id]->LoadRecValBD("NAME",contr[id]->bd);
    return(0);
}

int TTipController::SaveCtrCfg( int id )
{
    if(id >= contr.size()) return(-1);
//    contr[id]->SetVal("NAME",contr[id]->name);
    contr[id]->SaveRecValBD("NAME",contr[id]->bd);	
    return(0);
}

int TTipController::LoadParmCfg( int id_ctr, int id_prm )
{
    char   str[10];
    string parm_bd;
    int    b_hd;
    
    if(id_ctr >= contr.size()) return(-1);
    if(id_prm >= param.size()) return(-2);
    for( int i_prm=0; i_prm < param.size(); i_prm++ )
    {
	if( i_prm == contr[id_ctr]->prm_cfg.size())
    	    contr[id_ctr]->prm_cfg.push_back();
    }
    sprintf(str,"PRM_BD%d",id_prm+1);
    if(contr[id_ctr]->GetVal(0,str,parm_bd) < 0) return(-3);
    b_hd = App->BD->OpenBD(parm_bd);
    if(b_hd < 0) return(-4);
    for(int i=0; i < App->BD->NLines(b_hd); i++)
    {
//Want check to free and new param !!!
	if( i == contr[id_ctr]->prm_cfg[id_prm].size())
	    contr[id_ctr]->prm_cfg[id_prm].push_back(new TParamContr(contr[id_ctr],param[id_prm],&App->Controller->ValElem));
	contr[id_ctr]->prm_cfg[id_prm][i]->LoadRecValBD(i,b_hd);
    }
    App->BD->CloseBD(b_hd);
//    if(contr[id_ctr]->prm_cfg[id_prm]->LoadValBD(parm_bd) < 0) return(-4);	    
    return(0);	
}
    
int TTipController::SaveParmCfg( int id_ctr, int id_prm )
{
    char   str[10];
    string parm_bd;
    int    b_hd;

    if(id_ctr >= contr.size()) return(-1);
    if(id_prm >= param.size() || id_prm >= contr[id_ctr]->prm_cfg.size()) return(-2);
    sprintf(str,"PRM_BD%d",id_prm+1);
    if(contr[id_ctr]->GetVal(0,str,parm_bd) < 0)               return(-3);
    b_hd = App->BD->OpenBD(parm_bd);
    if(b_hd < 0)
    {
	b_hd = App->BD->NewBD(parm_bd);
	if(b_hd < 0) return(-4);
    }
    //Clear BD
    while(App->BD->NLines(b_hd)) App->BD->DelLine(b_hd,0);
    for(int i_ln=0; i_ln < contr[id_ctr]->prm_cfg[id_prm].size(); i_ln++)
    {
	App->BD->AddLine(b_hd,i_ln);
	contr[id_ctr]->prm_cfg[id_prm][i_ln]->SaveRecValBD(i_ln,b_hd);	
    }
    App->BD->SaveBD(b_hd);
    App->BD->CloseBD(b_hd);
//    if(contr[id_ctr]->prm_cfg[id_prm]->SaveValBD(parm_bd) < 0) return(-4);	    
    return(0);	
}

