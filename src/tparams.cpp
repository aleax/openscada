#include "tcontroller.h"
#include "tparam.h"
#include "tparams.h"


TParamS::TParamS(  )
{

}

TParamS::~TParamS(  )
{

}

int TParamS::Add(TParamContr *PrmCntr)
{
    int cnt=0;
    
    for(int i_prm=0; i_prm < param.size(); i_prm++)
	if(PrmCntr->Name() == param[i_prm]->Name() && !cnt) 
	{ 
	    param[i_prm]->Reg(PrmCntr);
	    cnt++;
	}
    if(!cnt)
	param.push_back( new TParam(PrmCntr) );

    return(0);	
}


