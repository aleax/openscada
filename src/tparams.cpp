#include "tparamcontr.h"
#include "tparam.h"
#include "tparams.h"

const char *TParamS::o_name = "TParamS";

TParamS::TParamS( TKernel *app ) : App(app)
{

}

TParamS::~TParamS(  )
{

}

void TParamS::List( vector<string> & List)
{
    List.clear();
    for(unsigned i_prm = 0; i_prm < param.size(); i_prm++ )
    	List.push_back(param[i_prm]->Name()); 
}

int TParamS::Add(TParamContr *PrmCntr)
{
    int cnt=0;
    //!!! Want request resource    
    for(unsigned i_prm=0; i_prm < Size(); i_prm++)
	if(PrmCntr->Name() == param[i_prm]->Name() && !cnt) 
	{ 
	    param[i_prm]->Reg(PrmCntr);
	    cnt++;
	}
    if(!cnt)
    {
	param.push_back( new TParam(PrmCntr) );
	HdIns(param.size()-1);
    }
    //!!! Want free resource

    return(0);	
}

int TParamS::Del(TParamContr *PrmCntr)
{
    //!!! Want request resource    
    for(unsigned i_prm=0; i_prm < Size(); i_prm++)
	if( PrmCntr->Name() == param[i_prm]->Name() ) 
	{ 
	    param[i_prm]->UnReg(PrmCntr);
	    if(param[i_prm]->Size() == 0)
	    {
		delete param[i_prm];
		param.erase(param.begin()+i_prm);
		HdFree(i_prm);
	    }		
	}
    //!!! Want free resource
    return(0);	
}

int TParamS::NameToHd( string Name )
{
    for(unsigned i_hd = 0; i_hd < hd.size(); i_hd++)
	if(hd[i_hd] >= 0 && param[hd[i_hd]]->Name() == Name ) return(i_hd);

    throw TError("%s: %s param no avoid!",o_name,Name.c_str()); 
}

int TParamS::HdIns(int id)
{
    unsigned i_hd;
    for( i_hd=0; i_hd < hd.size(); i_hd++)
	if(hd[i_hd] >= id ) hd[i_hd]++;
    for( i_hd=0; i_hd < hd.size(); i_hd++)
	if(hd[i_hd] < 0 ) break;
    if( i_hd == hd.size() ) hd.push_back(id);
    else                    hd[i_hd] = id;

    return(i_hd);
}

int TParamS::HdFree(int id)
{
    for(unsigned i_hd=0; i_hd < hd.size(); i_hd++)
	if( hd[i_hd] == id ) { hd[i_hd] = -1; break; }
    for(unsigned i_hd=0; i_hd < hd.size(); i_hd++)
	if( hd[i_hd] > id ) hd[i_hd]--;

    return(0);
}

int TParamS::HdChange( int id1, int id2 )
{
    for(unsigned i_hd = 0; i_hd < hd.size(); i_hd++)
	if( hd[i_hd] == id1 )      { hd[i_hd] = id2; continue; }
	else if( hd[i_hd] == id2 ) { hd[i_hd] = id1; continue; }

    return(0);
}

