
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
//    for(unsigned i_m = 0; i_m < Size(); i_m++) DelM(i_m);    
}

void TGRPModule::gmd_InitAll( )
{
    for(unsigned i=0;i<Moduls.size();i++) 
	if(Moduls[i] != TO_FREE) Moduls[i]->mod_init(NULL);
}

void TGRPModule::gmd_DeinitAll( )
{
    for(unsigned i=0;i<Moduls.size();i++) 
	if(Moduls[i] != TO_FREE) Moduls[i]->mod_deinit();
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

    modul->mod_info("NameType",NameTMod);
    modul->mod_info("NameModul",NameMod);
    for(unsigned i=0;i < Moduls.size(); i++)
    {
	if( Moduls[i] == TO_FREE ) continue;
	if( Moduls[i]->mod_Name() == NameMod )
	{
	    int major, major1, minor, minor1;
	    modul->mod_Version(major,minor);
    	    Moduls[i]->mod_Version(major1,minor1);

	    if(major>major1 || (major==major1 && minor > minor1))
	    {
		delete Moduls[i];
		Moduls[i] = modul;
#if OSC_DEBUG 
		Mess->put(0, "Update modul is ok!");
#endif	
		return(i);
	    }
	}
    }

    unsigned i;
    for( i=0 ;i < Moduls.size(); i++)
	if(Moduls[i] == TO_FREE ) break;
    if(i == Moduls.size()) Moduls.push_back( );
    Moduls[i]    = modul;
#if OSC_DEBUG 
    Mess->put(0, "Add modul <%s> is ok! Type <%s> .",NameMod.c_str(),NameTMod.c_str());
#endif	
    return(i);
}

void TGRPModule::gmd_DelM( unsigned hd )
{
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

TModule *TGRPModule::gmd_at(unsigned int id) const 
{ 
    if(Moduls[id] != TO_FREE) return(Moduls[id]); 
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

