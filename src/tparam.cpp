
#include "tsys.h"
#include "tkernel.h"
#include "tparamcontr.h"
#include "tparam.h"

const char *TParam::o_name = "TParam";

TParam::TParam( SCntrS cntr, string name, TParamS *prms ) : work(0), owner(prms)
{    
    TParam::name = name;
    hd_res = SYS->ResCreate();
    Reg( cntr, name );
}

TParam::~TParam(  )
{
    SYS->WResRequest(hd_res);
    while(PrmC.size())
    {
	Owner().Owner().Controller().at(PrmC[0].c_hd).det(PrmC[0].p_hd);
	Owner().Owner().Controller().det(PrmC[0].c_hd);
	PrmC.erase(PrmC.begin());	
    }
    SYS->WResRelease(hd_res);
    
    SYS->ResDelete(hd_res);
}

int TParam::Reg( SCntrS cntr, string name )
{
    SYS->WResRequest(hd_res);
    //Check already registry parameters
    for(unsigned i_pr = 0; i_pr < PrmC.size(); i_pr++)
 	if( Owner().Owner().Controller().at(PrmC[i_pr].c_hd).Name() == cntr.obj &&
	    Owner().Owner().Controller().at(PrmC[i_pr].c_hd).Owner().mod_Name() == cntr.tp &&
	    Owner().Owner().Controller().at(PrmC[i_pr].c_hd).at(PrmC[i_pr].p_hd).Name() == name)
	{
	    SYS->WResRelease(hd_res);
	    return( PrmC.size() );
	}
    //Registry parameter
    SParam prm;
    prm.c_hd = Owner().Owner().Controller().att(cntr);
    try{ prm.p_hd = Owner().Owner().Controller().at(prm.c_hd).att(name); }
    catch(...)
    {
	Owner().Owner().Controller().det(prm.c_hd);
	SYS->WResRelease(hd_res);
	throw;
    }
    PrmC.push_back(prm);
    SYS->WResRelease(hd_res);

    return( PrmC.size() );
} 

int TParam::UnReg( SCntrS cntr, string name )
{
    SYS->WResRequest(hd_res);
    //Check registry parameters
    for(unsigned i_pr = 0; i_pr < PrmC.size(); i_pr++)
 	if( Owner().Owner().Controller().at(PrmC[i_pr].c_hd).Name() == cntr.obj &&
	    Owner().Owner().Controller().at(PrmC[i_pr].c_hd).Owner().mod_Name() == cntr.tp &&
	    Owner().Owner().Controller().at(PrmC[i_pr].c_hd).at(PrmC[i_pr].p_hd).Name() == name)
	{
	    Owner().Owner().Controller().at(PrmC[i_pr].c_hd).det(PrmC[i_pr].p_hd);
	    Owner().Owner().Controller().det(PrmC[i_pr].c_hd);
	    PrmC.erase(PrmC.begin()+i_pr);
	    break;	    	    
	}
    SYS->WResRelease(hd_res);
    return( PrmC.size() );	    
}

TParamContr &TParam::at()
{    
    SYS->RResRequest(hd_res);
    TParamContr &c_prm = Owner().Owner().Controller().at(PrmC[work].c_hd).at(PrmC[work].p_hd);
    SYS->RResRelease(hd_res);
    return( c_prm );      
}

