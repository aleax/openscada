
#include "tapplication.h"
#include "tparams.h"
#include "tvalue.h"
#include "tcontroller.h"


//==== TController ====
 TController::TController( TTipController *tcntr, string name_c, string bd_c, TConfigElem *cfgelem) : 
		name(name_c), bd(bd_c), TContr(tcntr), TConfig(cfgelem)
{

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

int TController::RegParam()
{
    for(int i_prm_t = 0; i_prm_t < prm_cfg.size(); i_prm_t++)
	for(int i_prm = 0; i_prm < prm_cfg[i_prm_t].size(); i_prm++)
	    App->Param->Add(prm_cfg[i_prm_t][i_prm]);

    return(0);
}


//==== TParamContr ====
TParamContr::TParamContr(TController *contr, TConfigElem *cfgelem, TConfigElem *cfgval) : 
		controller(contr), TConfig(cfgelem), val(cfgval)
{

}

TParamContr::~TParamContr( )
{

}

string TParamContr::Name()
{
    string val;
    
    GetVal("SHIFR",val);

    return(val);
}

int TParamContr::AddVal(int id_val, SBlock *block)
{
    double val_t;

    if(val.AddVal(id_val,block) < 0) return(-1);
    if(block->data&VAL_D_BD)
    {
	if(!block->access)
	    if(GetVal("ACCESS",val_t) == 0) val.SetVal(id_val,"ACCESS", val_t);		
	if(!block->min && !block->max)
	{
	    if(GetVal("MIN",val_t) == 0) val.SetVal(id_val,"MIN",val_t);
	    if(GetVal("MAX",val_t) == 0) val.SetVal(id_val,"MAX",val_t);
	}
    }
    if(block->data&VAL_D_VBD)
	if(GetVal(block->name,val_t) == 0) val.write(val_t);
    
    return(0);
}

