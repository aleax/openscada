
#include "tbd.h"
#include "tmessage.h"
#include "tcontrollers.h"
#include "tcontroller.h"
#include "ttipcontroller.h"
#include "tparamcontr.h"

TParamContr::TParamContr(TController *contr, TConfigElem *cfgelem ) : 
		controller(contr), TConfig(cfgelem)
{
    t_sync=time(NULL);
    UpdateVAL();
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
/*
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
}*/

TParamContr & TParamContr::operator=( TParamContr & PrmCntr )
{
    TConfig::operator=(PrmCntr);
    t_sync=PrmCntr.t_sync;

    return(*this);
}

int TParamContr::UpdateVAL()
{
    string type; 
    vector<string> types;
    TValueElem *valel;
    
    GetVal("TYPE",type);

    valel =  Controller()->TipController()->at_val(type);
    if(valel != NULL) SetValType( valel );

    return(0);
}                    


