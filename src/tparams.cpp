#include "xml.h"
#include "tmessage.h"
#include "tparamcontr.h"
#include "tparam.h"
#include "tparams.h"

const char *TParamS::o_name = "TParamS";
const char *TParamS::s_name = "Parameters";
const char *TParamS::i_cntr = 
	"<oscada_cntr>"
	"</oscada_cntr>"; 
 

SFld TParamS::gen_elem[] =
{
    {"NAME","Arhive name.",T_STRING,"","20"}
};

TParamS::TParamS( TKernel *app ) : 
	TConfig(NULL), owner(app), m_hd(o_name)
{
    for(unsigned i = 0; i < sizeof(gen_elem)/sizeof(SFld); i++) 
	cfConfElem().elAdd(&gen_elem[i]);
}

TParamS::~TParamS(  )
{

}

string TParamS::Name()
{ 
    return(Mess->I18N((char *)s_name));
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
    try
    {
	TParam *prm = (TParam *)m_hd.obj_del( param, 5 );
	if( prm->UnReg( cntr, param ) ) m_hd.obj_add( prm, &prm->Name() );
	else delete prm;
    }
    catch(TError err)
    { 
	throw TError("%s: Unregistrated parameter %s buzy! %s",s_name,param.c_str(),err.what().c_str()); 
    }
}

//==============================================================
//================== Controll functions ========================
//==============================================================
void TParamS::ctr_fill_info( XMLNode *inf )
{
    inf->load_xml( i_cntr );
    inf->set_text(Name());
} 

