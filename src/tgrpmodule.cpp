
#include <unistd.h>

#include "tsys.h"
#include "tkernel.h"
#include "tmessage.h"
#include "tmodule.h"
#include "tgrpmodule.h"

const char *TGRPModule::o_name = "TGRPModule";

TGRPModule::TGRPModule( TKernel *app, char *NameT ) : NameType(NameT), DirPath(""), owner(app)
{

}

TGRPModule::~TGRPModule(  )
{
    try{ for(unsigned i_m = 0; i_m < gmd_Size(); i_m++) gmd_DelM(i_m); }catch(...){ };
}

void TGRPModule::gmd_List( vector<string> & moduls ) const
{
    moduls.clear();
    for(unsigned i=0;i < gmd_Size();i++) 
	if(Moduls[i] != TO_FREE) 
	    moduls.push_back(Moduls[i]->mod_Name());
}

// Add modul 

int TGRPModule::gmd_AddM( TModule *modul )
{
    string NameMod, NameTMod;
    
    //---  Check names and version ---

    NameTMod = modul->mod_info("Type");
    NameMod  = modul->mod_info("Modul");
    for(unsigned i=0;i < Moduls.size(); i++)
    {
	if( Moduls[i] == TO_FREE ) continue;
	if( Moduls[i]->mod_Name() == NameMod )
	{
	    delete Moduls[i];
	    Moduls[i] = modul;
	    Moduls[i]->owner = this;
#if OSC_DEBUG 
	    Mess->put(0, "Update/Reload modul is ok!");
#endif	
	    return(i);	    
	}
    }

    unsigned i;
    for( i=0 ;i < Moduls.size(); i++)
	if(Moduls[i] == TO_FREE ) break;
    if(i == Moduls.size()) Moduls.push_back(modul);
    else                   Moduls[i] = modul;
    Moduls[i]->mod_connect(this);
#if OSC_DEBUG 
    Mess->put(1, "-------------------------------------");
    vector<string> list;
    (*this)[i].mod_info( list );
    for( unsigned i_opt = 0; i_opt < list.size(); i_opt++)
    	Mess->put(1, "| %s: %s",list[i_opt].c_str(),(*this)[i].mod_info(list[i_opt]).c_str());
    Mess->put(1, "-------------------------------------");
#endif	
    return(i);
}

void TGRPModule::gmd_DelM( unsigned hd )
{
#if OSC_DEBUG 
    Mess->put(0, "%s: disconnect modul <%s>!",o_name,Moduls[hd]->mod_Name().c_str() );
#endif	
    if(hd >= Moduls.size() || Moduls[hd] == TO_FREE ) 
	throw TError("%s: Module header %d error!",o_name,hd);
    delete Moduls[hd];
    Moduls[hd] = TO_FREE;
}

unsigned TGRPModule::gmd_NameToId(string name) const
{
    for(unsigned i=0; i<gmd_Size(); i++)
    {            
	if( Moduls[i] == TO_FREE )      continue;
	if( Moduls[i]->mod_Name() == name ) return(i);
    }
    throw TError("%s: no avoid modul %s!",o_name, name.c_str());
}

TModule &TGRPModule::gmd_at(unsigned int id) const 
{ 
    if(Moduls[id] != TO_FREE) return(*Moduls[id]); 
    throw TError("%s: module id error!",o_name); 
}

bool TGRPModule::gmd_MChk(unsigned int id)
{
    if(id >= gmd_Size() || Moduls[id] == TO_FREE ) return(true); 
    return(false);
}

TModule *TGRPModule::gmd_FUse(unsigned int id, char * func, void (TModule::**offptr)())
{
    if(id >= gmd_Size() || Moduls[id] == TO_FREE ) throw TError("%s: no id module!",o_name);
    Moduls[id]->mod_GetFunc(func, offptr);
    return(Moduls[id]);
}

void TGRPModule::gmd_FFree(unsigned int id, char * func)
{
    if(id >= gmd_Size() || Moduls[id] == TO_FREE ) throw TError("%s: no id module!",o_name);
    Moduls[id]->mod_FreeFunc(func);
}

void TGRPModule::gmd_CheckCommandLineMods()
{
    for(unsigned i_m=0; i_m < gmd_Size(); i_m++)
	if( Moduls[i_m] != TO_FREE )
	    Moduls[i_m]->mod_CheckCommandLine( );
}

void TGRPModule::gmd_UpdateOptMods()
{
    for(unsigned i_m=0; i_m < gmd_Size(); i_m++)
	if( Moduls[i_m] != TO_FREE )
	    Moduls[i_m]->mod_UpdateOpt();	    
}

