#include "tparamcontr.h"
#include "tparam.h"
#include "tparams.h"

const char *TParamS::o_name = "TParamS";
const char *TParamS::i_cntr = 
	"<obj> Parameter subsystem"
	" <configs> Base parameters:"
	" </configs>"
	"</obj>"; 
 

SCfgFld TParamS::gen_elem[] =
{
    {"NAME"    ,"Arhive name."         ,CFG_T_STRING              ,"","",""           ,"20",""          ,"%s"}
};

TParamS::TParamS( TKernel *app ) : 
	TConfig(NULL), owner(app), m_hd(o_name), TContr( i_cntr )
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

//==============================================================
//================== Controll functions ========================
//==============================================================
void TParamS::ctr_fill_info( XMLNode &inf )
{

}

void TParamS::ctr_opt_apply( XMLNode &opt )
{

}
 

