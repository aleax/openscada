
#ifndef TMODULE_H
#define TMODULE_H

#include <string>
using std::string;
#include <vector>
using std::vector;

#include "tkernel.h"
//==== Moduls stats ====
#define SMOD_PRESENT   0  //Modul present but no init 
#define SMOD_READY     1  //Modul ready and may proced request

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
    /**
     * Init external modul (plugin) 
     *  (Load self data)     
    */
    virtual void mod_init( void *param );
    /**
     * Deinit external modul (plugin)
     *  (Save self data)
    */
    virtual void mod_deinit(  );
    /**
     * Get info about modul (plugin):
     *  NameFile, NameModul, NameType, Version, Autors, Description, ListCommand,
     *  ListExpFunc, ModStat, ProtExpFunc 
    */
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
    char   mod_Stat() { return(stat); }

    TGRPModule &Owner() { return( *owner ); }
    
/** Public Attributes: */
public:
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

private:
    char stat;           // Modul stat
    TGRPModule        *owner;
    static const char *l_info[];    // list info options
    
    static const char *o_name;
};



#endif // TMODULE_H
