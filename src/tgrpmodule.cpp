
#include <unistd.h>

#include "tsys.h"
#include "tkernel.h"
#include "tmessage.h"
#include "tgrpmodule.h"

const char *TGRPModule::o_name = "TGRPModule";
const char *TGRPModule::i_cntr = 
	"<obj>"
	" <configs> Base parameters:"
	" </configs>"
	" <branchs mode=\"att\" dscr=\"Subsystem modules:\">"
        " </branchs>"  
	"</obj>"; 
 

TGRPModule::TGRPModule( TKernel *app, char *NameT ) : 
	NameType(NameT), DirPath(""), owner(app), m_hd(o_name), TContr( i_cntr )
{

}

TGRPModule::~TGRPModule(  )
{
    vector<string> list;
    m_hd.lock();
    gmd_list(list);
    for(unsigned i_m = 0; i_m < list.size(); i_m++) 
    {
	try{ gmd_del(list[i_m]); } catch(...){ }
    }
}

void TGRPModule::gmd_add( TModule *modul )
{
    m_hd.obj_add( modul, &modul->mod_Name() );
    modul->mod_connect(this);
#if OSC_DEBUG 
    Mess->put("DEBUG",MESS_DEBUG,"-------------------------------------");
    vector<string> list;
    modul->mod_info( list );
    for( unsigned i_opt = 0; i_opt < list.size(); i_opt++)
    	Mess->put("DEBUG",MESS_DEBUG,"| %s: %s",list[i_opt].c_str(),modul->mod_info(list[i_opt]).c_str());
    Mess->put("DEBUG",MESS_DEBUG,"-------------------------------------");
#endif
}

void TGRPModule::gmd_del( string name )
{
#if OSC_DEBUG 
    Mess->put("DEBUG",MESS_INFO,"%s: Disconnect modul <%s>!",o_name,name.c_str() );
#endif

    delete (TModule *)m_hd.obj_del( name );
    
#if OSC_DEBUG 
    Mess->put("DEBUG",MESS_DEBUG,"%s: Disconnect modul <%s> ok!",o_name,name.c_str() );
#endif
}

TModule *TGRPModule::gmd_FUse(unsigned int hd, char * func, void (TModule::**offptr)())
{
    TModule &mod = gmd_at(hd);
    mod.mod_GetFunc(func, offptr);
    return(&mod);
}

void TGRPModule::gmd_CheckCommandLineMods()
{
    vector<string> list;
    gmd_list(list);
    for(unsigned i_m=0; i_m < list.size(); i_m++)
    {
	unsigned hd = gmd_att(list[i_m]);
	gmd_at(hd).mod_CheckCommandLine( );
	gmd_det(hd);
    }
}

void TGRPModule::gmd_UpdateOptMods()
{
    vector<string> list;
    gmd_list(list);
    for(unsigned i_m=0; i_m < list.size(); i_m++)
    {
	unsigned hd = gmd_att(list[i_m]);
       	gmd_at(hd).mod_UpdateOpt();	    
	gmd_det(hd);
    }	
}

XMLNode *TGRPModule::gmd_XMLCfgNode()
{
    int i_k = 0;
    while(true)
    {
	XMLNode *t_n = Owner().XMLCfgNode()->get_child("section",i_k++);
	if( t_n->get_attr("id") == gmd_Name() ) return( t_n );
    }
}

void TGRPModule::gmd_CheckCommandLine( )
{
#if OSC_DEBUG
    Mess->put("DEBUG",MESS_INFO,"%s: Read commandline options!",NameType.c_str() );
#endif
}

void TGRPModule::gmd_UpdateOpt()
{
#if OSC_DEBUG
    Mess->put("DEBUG",MESS_INFO,"%s: Read config options!",NameType.c_str());
#endif
}

//==============================================================
//================== Controll functions ========================
//==============================================================
void TGRPModule::ctr_fill_info( XMLNode &inf )
{
    inf.set_text(string(gmd_Name()+" subsistem"));    
    vector<string> list;
    gmd_list(list);
    ctr_br_putlist(inf, list);
}

void TGRPModule::ctr_opt_apply( XMLNode &opt )
{

}


