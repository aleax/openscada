
#ifndef TMODULE_H
#define TMODULE_H

#include <string>
using std::string;

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

class TModule
{

/** Public methods: */
public:
     TModule( );

    virtual ~TModule(  );
    /**
     * Init external modul (plugin) 
     *  (Load self data)     
    */
    virtual void init( void *param );
    /**
     * Deinit external modul (plugin)
     *  (Save self data)
    */
    virtual void deinit(  );
    /**
     * Get info about modul (plugin):
     *  NameFile, NameModul, NameType, Version, Autors, Description, ListCommand,
     *  ListExpFunc, ModStat, ProtExpFunc 
    */
    virtual void info( const string & name, string & info );

    virtual void CheckCommandLine( )  { };

    virtual void UpdateOpt(){ };    
    /**
      * Get address exporting function and registre of use function.
      */
    void GetFunc( string NameFunc, void (TModule::**offptr)() );
    /**
      * Unregistre function
      */
    void FreeFunc( string NameFunc );
 
    void Version( int & mayor, int & minor );

    string Name() { return(NameModul); }
    char   Stat() { return(stat); }
/**Attributes: */

public:
//    SNameUser * users;
    char stat;           // Modul stat

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
    static const char *o_name;
};



#endif // TMODULE_H
