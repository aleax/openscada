
#ifndef TMODULE_H
#define TMODULE_H

#include <string>

#include "../../tapplication.h"

//==== Moduls errors ====
#define MOD_NO_ERR     0
#define MOD_NO_PATH    1
#define MOD_NO_FILE    2
#define MOD_NO_FILEDIN 3
#define MOD_NO_PERMIT  4
#define MOD_ERR        5
#define MOD_NO_FUNC    6

//==== Moduls stats ====
#define SMOD_PRESENT   0
#define SMOD_READY     1
#define SMOD_TEST      2
#define SMOD_RUN       3
#define SMOD_ERROR     4

//====== Structura for Exportin function =======

class TModule;

struct SExpFunc
{
    char *NameFunc;
    void (TModule::*ptr)();
    char *prototip;
    char *descript;
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
    virtual int init(  );
    /**
     * Deinit external modul (plugin)
     *  (Save self data)
    */
    virtual int deinit(  );
    /**
     * Put command to external modul (plugin)
     *  (Start, Stop ....))
    */
    virtual int PutCommand( string & command ); 
    /**
     * Get info about modul (plugin):
     *  NameFile, NameModul, NameType, Version, Autors, Description, ListCommand,
     *  ListExpFunc, ModStat, ProtExpFunc 
    */
    virtual int info( const string & name, string & info );
    /**
      * Get address exporting function.
      */
    virtual int GetFunc( string NameFunc, void (TModule::**offptr)() );
 
    virtual void Version( int & mayor, int & minor );

/**Attributes: */

public:
//    SNameUser * users;

protected:
    char *FileName;
    char *NameModul;
    char *NameType;
    char *Vers;
    char *Autors;
    char *DescrMod;

    SExpFunc *ExpFunc;
    int  NExpFunc;
private:

private:
    int stat;
};



#endif // TMODULE_H
