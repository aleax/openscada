
#ifndef TCONTROLLERS_H
#define TCONTROLLERS_H

#include <string>
using std::string;

#include "tconfig.h"
#include "tgrpmodule.h"

#define TO_FREE         NULL // Object free

struct SContr            //Contain data from GENERIC BD
{
    string name;	 //Name controller
    string modul;	 //Name controller's module
    int    id_mod;	 //Modul's ID (-1 - noavoid)
    int    id_contr;     //Controller's ID into module    
    string bd;           //Controller's BD  
};

class TController;
class TTipController;

class TControllerS : public TGRPModule
{

/** Public methods: */
public:
    TControllerS(  );
    ~TControllerS(  );

    /*
     * List controllers from bd 
     */
    void ContrList( vector<string> & List );
    /*
     * Init All controller's modules
     */    
    virtual int InitAll( );
    /*
     * Deinit moduls controller's types. 
     */ 
    void DeInit(  );                                        //?!?!
    /*
     * Init and start all configured controllers.
     */ 
    virtual int StartAll(  );                                         
    /*
     * Stop and deinit all configured controllers.
     */ 
    virtual int StopAll(  );
    /*
     * Add Controller for type controllers <tip> with BD <bd>
     */    
    int AddContr( string name, string tip, string bd );
    /*
     * Delete Controller
     */    
    int DelContr( string name );
    /**
      * Load/Reload all BD and update internal controllers structure!
      */
    void LoadBD( );
    /*
     * Update all BD from current to external BD.
     */
    int UpdateBD( );
    /*
     * Controller amount
     */
    unsigned Size(){ return(Contr.size()); }
    /*
     * Convert Name parameter to hd (hd - individual number of parameter for fast calling to parameter )
     */
    int NameToHd( string Name );
    
    TController *at( int id_hd);
    TController *at( string name )
    { return(at(NameToHd(name))); }
    
    TTipController *at_tp( string name );

    void CheckCommandLine(  );
/** Public atributes: */
public:

/** Private atributes: */
private:
    string gener_bd;
    
    vector< SContr >          Contr;   //Controllers list from BD
    vector< int >             hd;      //Headers for short access to controllers
    vector< TTipController *> TContr;  //Tip controllers list from TGRPModule

/** Private methods: */
private:
    /**
     * Print desription of command line options.
     */
    void pr_opt_descr( FILE * stream );
    /*
     * virtual function adding module into TGRPModule
     */ 
    virtual int AddM( TModule *modul );
    /*
     * virtual function deleting module into TGRPModule
     */ 
    virtual int DelM( int hd );
    /**
     * Create general BD.
     */
    int CreateGenerBD( string type_bd );

    int HdIns( int id );
    int HdFree( int id );
    int HdChange( int id1, int id2 );
};

#endif // TCONTROLLERS_H
