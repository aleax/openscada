
#ifndef TCONTROLLERS_H
#define TCONTROLLERS_H

#include <string>
using std::string;

#include "tconfig.h"
#include "tgrpmodule.h"

#define TCNTR_FREE    0   //Cell free 
#define TCNTR_DISABLE 1   //Controller present and disable 
#define TCNTR_ENABLE  2   //Controller present and enabled
#define TCNTR_ERR     3   //Controller present, disabled and error

struct SContr            //Contain data from GENERIC BD
{
    int    stat;         //Stat controller
    string name;	 //Name controller
    string modul;	 //Name controller's module
    int    id_mod;	 //Modul's ID 
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
     * List controllers for <NameContrTip> type's
     */
    void ContrList( const string NameContrTip, string & List );
    /*
     * Init All controller's modules
     */    
    virtual int InitAll( );
    /*
     * Init moduls controller's types. 
     */
    void Init(  );
    /*
     * Deinit moduls controller's types. 
     */ 
    void DeInit(  );                                        //?!?!
    /*
     * Init and start all configured controllers.
     */ 
    void Start(  );                                         //?!?!
    /*
     * Stop and deinit all configured controllers.
     */ 
    void Stop(  );                                          //?!?!
    /*
     * Add Controller for type controllers <tip> with BD <bd>
     */    
    int AddContr( string name, string tip, string bd );     //?!?!
    /*
     * Delete Controller
     */    
    int DelContr( string name );                            //?!?!
    /**
      * Load/Reload all BD and update internal controllers structure!
      */
    void LoadBD( );
    /*
     * Update all BD from current to external BD.
     */
    int UpdateBD( );
    /**
     * Create general BD.
     */
    int CreateGenerBD( string type_bd );

    /*
     * Put command to controller: 
     * DISABLE (GLOBAL) - disable;
     * ENABLE (GLOBAL) - enable;
     * INIT (LOCAL) - init controller; 
     * DEINIT (LOCAL) - deinit controller;
     * START (LOCAL) - start controller;
     * STOP (LOCAL) - stop controller;
     * ADD (LOCAL) - add controller;
     * DELETE (LOCAL) - delete controller;
     */
    int PutCntrComm( string comm, int id_ctr );

    void CheckCommandLine(  );
/** Public atributes: */
public:
    TConfigElem               ValElem; //Value elements for use into Params

/** Private methods: */
private:
    /**
     * Print desription of command line options.
     */
    void pr_opt_descr( FILE * stream );

    /**
     * Test
     */
    int test();
/** Private atributes: */
private:
    string gener_bd;
    
    vector< SContr *>         Contr;   //Controllers list from BD
    vector< TTipController *> TContr;  //Tip controllers list from TGRPModule

/** Private methods: */
private:
    /*
     * virtual function adding module into TGRPModule
     */ 
    virtual int AddM( TModule *modul );
    /*
     * virtual function deleting module into TGRPModule
     */ 
    virtual int DelM( int hd );
};

#endif // TCONTROLLERS_H
