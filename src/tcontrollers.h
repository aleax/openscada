
#ifndef TCONTROLLERS_H
#define TCONTROLLERS_H

#include <string>
using std::string;

#include "tconfig.h"
#include "tgrpmodule.h"

struct SContr            //Contain data from GENERIC BD
{
    string  name;	 // Name controller
    TConfig *config;      // Generic config 
//    string  modul;	 // Name controller's module
    int     id_mod;	 // Modul's ID (-1 - noavoid)
    int     id_contr;     // Controller's ID into module    
//    string  bd;           // Controller's BD
};

class TController;
class TTipController;

class TControllerS : public TGRPModule
{

/** Public methods: */
public:
    TControllerS( TKernel *app );
    ~TControllerS(  );

    /*
     * List controllers from bd 
     */
    void ContrList( vector<string> & List );
    /*
     * Init All controller's modules
     */    
    void InitAll( );
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
    int AddContr( string name, string tip, string t_bd, string n_bd, string n_tb );
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
    
    TController *at( unsigned id_hd);
    TController *at( string name )
    { return(at(NameToHd(name))); }
    
    TTipController *at_tp( string name ){ return(TContr[NameToId(name)]); }

    void CheckCommandLine( char **argv, int argc );
    void UpdateOpt();
    
    string TypeGenBD() { return(t_bd); }
    string NameGenBD() { return(n_bd); }
    string NameGenTB() { return(n_tb); }
    
/** Public atributes: */
public:
    TConfigElem               gener_ecfg;  //Public for external access 
/** Private atributes: */
private:
    string t_bd;
    string n_bd;
    string n_tb;
    //string gener_bd;
    
    
    vector< SContr >          Contr;   //Controllers list from BD
    vector< int >             hd;      //Headers for short access to controllers
    vector< TTipController *> TContr;  //Tip controllers list from TGRPModule
    
    static SCfgFld            gen_elem[]; //Generic BD elements
    
    static const char 	      *o_name;
    static const char 	      *n_opt;
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

    int HdIns( int id );
    int HdFree( int id );
    int HdChange( int id1, int id2 );
};

#endif // TCONTROLLERS_H
