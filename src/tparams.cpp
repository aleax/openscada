#include "tparamcontr.h"
#include "tparam.h"
#include "tparams.h"

const char *TParamS::o_name = "TParamS";

SCfgFld TParamS::gen_elem[] =
{
    {"NAME"    ,"Arhive name."         ,CFG_T_STRING              ,"","",""           ,"20",""          ,"%s"}
};

TParamS::TParamS( TKernel *app ) : TConfig(NULL), owner(app), m_hd(o_name)
{
    for(unsigned i = 0; i < sizeof(gen_elem)/sizeof(SCfgFld); i++) 
	cf_ConfElem()->cfe_Add(&gen_elem[i]);
}

TParamS::~TParamS(  )
{

}

void TParamS::add( SCntrS cntr, string param )
{
    try
    {
	int hd = att(param);
	at(hd).Reg( cntr, param );
	det(hd);
    }
    catch(...)
    {
        TParam *Parm = new TParam( cntr, param, this );
    	m_hd.obj_add( Parm, &Parm->Name() );
    }    
}

void TParamS::del( SCntrS cntr, string param )
{
    int hd = att(param);
    if( !at(hd).UnReg( cntr, param ) )
    {
	det(hd);
	delete (TParam *)m_hd.obj_del( param );
	return;
    }
    det(hd);
}

/*
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
*/

