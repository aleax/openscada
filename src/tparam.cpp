
#include "tapplication.h"
#include "tparamcontr.h"
#include "tparam.h"

TParam::TParam( TParamContr *PrmCntr ) : work(0)
{
    ParamC.push_back(PrmCntr);
    name=PrmCntr->Name();
}

TParam::~TParam(  )
{
    while(Size())
	ParamC.erase(ParamC.begin());
}

int TParam::Reg(TParamContr *PrmCntr)
{
    if(PrmCntr == NULL) return(-1);
    for(unsigned i=0; i < Size(); i++)
	if(ParamC[i] == PrmCntr) return(0);
    ParamC.push_back(PrmCntr);

    return(0);
} 

int TParam::UnReg(TParamContr *PrmCntr)
{
    if(PrmCntr == NULL) return(-1);
    for(unsigned i=0; i < Size(); i++)
	if(ParamC[i] == PrmCntr) 
	    ParamC.erase(ParamC.begin()+i);

    return(0);	    
}

