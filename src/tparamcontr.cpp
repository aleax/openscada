
#include "tbds.h"
#include "tmessage.h"
#include "tcontrollers.h"
#include "tcontroller.h"
#include "ttipcontroller.h"
#include "tparamcontr.h"

const char *TParamContr::o_name = "TParamContr";

TParamContr::TParamContr(TController *contr, TConfigElem *cfgelem ) : 
		owner(contr), TConfig(cfgelem)
{
    t_sync=time(NULL);
}

TParamContr::~TParamContr( )
{

}

inline string TParamContr::Name()
{
    return(Get_S("SHIFR"));
}

TParamContr & TParamContr::operator=( TParamContr & PrmCntr )
{
    TConfig::operator=(PrmCntr);
    t_sync=PrmCntr.t_sync;

    return(*this);
}

void TParamContr::UpdateVAL()
{
    SetValType( owner->owner->at_val(Get_S("TYPE")) );
}                    


void TParamContr::Enable()
{
    for(unsigned i_val = 0; i_val < Elem()->Size(); i_val++)
	Valid(i_val,true);
}

void TParamContr::Disable()
{
    for(unsigned i_val = 0; i_val < Elem()->Size(); i_val++)
	Valid(i_val,false);
}

