
#include "tbds.h"
#include "tmessage.h"
#include "tcontrollers.h"
#include "tcontroller.h"
#include "ttipcontroller.h"
#include "tparamcontr.h"

const char *TParamContr::o_name = "TParamContr";

TParamContr::TParamContr(TController *contr, TTipParam *tpprm ) : 
		owner(contr), TConfig(tpprm), tipparm(tpprm)
{
    t_sync=time(NULL);
}

TParamContr::~TParamContr( )
{

}

string TParamContr::Name()
{
    return(cf_Get_S("SHIFR"));
}

TParamContr & TParamContr::operator=( TParamContr & PrmCntr )
{
    TConfig::operator=(PrmCntr);
    t_sync=PrmCntr.t_sync;

    return(*this);
}

void TParamContr::UpdateVAL()
{    
    vl_SetType( owner->owner->at_val(cf_Get_S("TYPE")) );
}                    


void TParamContr::Enable()
{
    for(unsigned i_val = 0; i_val < vl_Elem()->vle_Size(); i_val++)
	vl_Valid(i_val,true);
}

void TParamContr::Disable()
{
    for(unsigned i_val = 0; i_val < vl_Elem()->vle_Size(); i_val++)
	vl_Valid(i_val,false);
}

