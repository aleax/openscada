
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

void TGRPModule::InitAll( )
{
    for(unsigned i=0;i<Moduls.size();i++) 
	if(Moduls[i] != TO_FREE) Moduls[i]->init(NULL);
}

void TGRPModule::DeinitAll( )
{
    for(unsigned i=0;i<Moduls.size();i++) 
	if(Moduls[i] != TO_FREE) Moduls[i]->deinit();
}

void TGRPModule::List( vector<string> & moduls ) const
{
    moduls.clear();
    for(unsigned i=0;i < Size();i++) 
	if(Moduls[i] != TO_FREE) 
	    moduls.push_back(Moduls[i]->Name());
}

// Add modul 

int TGRPModule::AddM( TModule *modul )
{
    string NameMod, NameTMod;
    
    //---  Check names and version ---

    modul->info("NameType",NameTMod);
    modul->info("NameModul",NameMod);
    for(unsigned i=0;i < Moduls.size(); i++)
    {
	if( Moduls[i] == TO_FREE ) continue;
	if( Moduls[i]->Name() == NameMod )
	{
	    int major, major1, minor, minor1;
	    modul->Version(major,minor);
    	    Moduls[i]->Version(major1,minor1);

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

void TGRPModule::DelM( unsigned hd )
{
    if(hd >= Moduls.size() || Moduls[hd] == TO_FREE ) 
	throw TError("%s: Module header %d error!",o_name,hd);
    delete Moduls[hd];
    Moduls[hd] = TO_FREE;
}

unsigned TGRPModule::NameToId(string name) const
{
    for(unsigned i=0; i<Size(); i++)
    {            
	if( Moduls[i] == TO_FREE )      continue;
	if( Moduls[i]->Name() == name ) return(i);
    }
    throw TError("%s: no avoid modul %s!",o_name, name.c_str());
}

TModule *TGRPModule::at(unsigned int id) const 
{ 
    if(Moduls[id] != TO_FREE) return(Moduls[id]); 
    throw TError("%s: module id error!",o_name); 
}

bool TGRPModule::MChk(unsigned int id)
{
    if(id >= Size() || Moduls[id] == TO_FREE ) return(true); 
    return(false);
}

TModule *TGRPModule::FUse(unsigned int id, char * func, void (TModule::**offptr)())
{
    if(id >= Size() || Moduls[id] == TO_FREE ) throw TError("%s: no id module!",o_name);
    Moduls[id]->GetFunc(func, offptr);
    return(Moduls[id]);
}

void TGRPModule::FFree(unsigned int id, char * func)
{
    if(id >= Size() || Moduls[id] == TO_FREE ) throw TError("%s: no id module!",o_name);
    Moduls[id]->FreeFunc(func);
}

void TGRPModule::CheckCommandLineMods()
{
    for(unsigned i_m=0; i_m < Size(); i_m++)
	if( Moduls[i_m] != TO_FREE )
	    Moduls[i_m]->CheckCommandLine( );
}

void TGRPModule::UpdateOptMods()
{
    for(unsigned i_m=0; i_m < Size(); i_m++)
	if( Moduls[i_m] != TO_FREE )
	    Moduls[i_m]->UpdateOpt();	    
}

