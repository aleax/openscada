
#ifndef TMODULE_H
#define TMODULE_H

#include <string>
using std::string;
#include <vector>
using std::vector;

#include "tkernel.h"
class TModule;

//====== Structura for Exportin function =======
struct SExpFunc
{
    char *NameFunc;
    void (TModule::*ptr)();
    char *prototip;
    char *descript;
    int  resource;
    int  access;
};

class TGRPModule;

class TModule
{
    friend class TGRPModule;
/** Public methods: */
public:
    TModule( );     

    virtual ~TModule(  );
    
    virtual string mod_info( const string name );
    virtual void   mod_info( vector<string> &list );
    
    virtual void mod_CheckCommandLine( )  { };

    virtual void mod_UpdateOpt(){ };    
    /**
      * Get address exporting function and registre of use function.
      */
    void mod_GetFunc( string NameFunc, void (TModule::**offptr)() );
    /**
      * Unregistre function
      */
    void mod_FreeFunc( string NameFunc );
 
    string mod_Name() { return(NameModul); }

    TGRPModule &Owner() { return( *owner ); }
    
/** Public Attributes: */
public:

protected:
    virtual void mod_connect(  ); 
/** Protected Attributes: */
protected:
    char *FileName;     // Sharelib file of module
    char *NameModul;    // Name module
    char *NameType;     // Name type module
    char *Vers;         // Version module
    char *Autors;       // Autors module
    char *DescrMod;     // Describe module
    char *License;      // License module 

    SExpFunc *ExpFunc;  // List of export function
    int  NExpFunc;      // Number export function

private:
    void mod_connect( TGRPModule *owner ); 

private:
    TGRPModule        *owner;
    static const char *l_info[];    // list info options
    
    static const char *o_name;
};



#endif // TMODULE_H
