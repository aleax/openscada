
#ifndef TMODULE_H
#define TMODULE_H

#include <string>
using std::string;

#include "tapplication.h"

//==== Moduls errors ====
#define MOD_NO_ERR     0  //No function errror
#define MOD_NO_PATH    -1  
#define MOD_NO_FILE    -2
#define MOD_NO_FILEDIN -3
#define MOD_NO_PERMIT  -4
#define MOD_ERR        -5  //Function error
#define MOD_NO_FUNC    -6
#define MOD_NO_RES     -7  //Resource empty

//==== Moduls stats ====
#define SMOD_PRESENT   0  //Modul present but no init 
#define SMOD_READY     1  //Modul ready and may proced request
#define SMOD_TEST      2  //Modul work in mode test
#define SMOD_RUN       3  //Modul run cyclic or self pthread tasks
#define SMOD_ERROR     4  //Modul in stat error

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
    virtual int init( void *param );
    /**
     * Deinit external modul (plugin)
     *  (Save self data)
    */
    virtual int deinit(  );
    /**
     * Get info about modul (plugin):
     *  NameFile, NameModul, NameType, Version, Autors, Description, ListCommand,
     *  ListExpFunc, ModStat, ProtExpFunc 
    */
    virtual int info( const string & name, string & info );
    /**
      * Get address exporting function and registre of use function.
      */
    virtual int GetFunc( string NameFunc, void (TModule::**offptr)() );
    /**
      * Unregistre function
      */
    virtual int FreeFunc( string NameFunc );
 
    virtual void Version( int & mayor, int & minor );

    string Name() { return(NameModul); }
/**Attributes: */

public:
//    SNameUser * users;
    int stat;           // Modul stat

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
};



#endif // TMODULE_H
