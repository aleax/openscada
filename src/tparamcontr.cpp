
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
    SetValType( Controller()->TipController()->at_val(Get_S("TYPE")) );
}                    


