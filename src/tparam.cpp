
#include "tcontroller.h"
#include "tparam.h"

TParam::TParam( TParamContr *PrmCntr )
{
    ParamC.push_back(PrmCntr);
    name=PrmCntr->Name();
}

TParam::~TParam(  )
{
    while(ParamC.size())
    {
	delete(	ParamC[0] );
	ParamC.erase(ParamC.begin());
    }
}

int TParam::Reg(TParamContr *PrmCntr)
{
    ParamC.push_back(PrmCntr);

    return(0);
} 

