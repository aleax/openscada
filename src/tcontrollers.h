
#ifndef TCONTROLLERS_H
#define TCONTROLLERS_H

#include <string>
using std::string;

#include "tconfig.h"
#include "tgrpmodule.h"

struct SContr            //Contain data from GENERIC BD
{
    bool     use;        // 
    unsigned id_mod;     // Modul's ID (-1 - noavoid)
    unsigned id_contr;   // Controller's ID into module    
};

class TController;
class TTipController;

class TControllerS : public TGRPModule, public TConfig 
{

/** Public methods: */
public:
    TControllerS( TKernel *app );
    ~TControllerS(  );

    /*
     * Init All controller's modules
     */    
    void gmd_InitAll( );
    /*
     * Init and start all configured controllers.
     */ 
    virtual int gmd_StartAll(  );                                         
    /*
     * Stop and deinit all configured controllers.
     */ 
    virtual int gmd_StopAll(  );

    void gmd_CheckCommandLine( );
    void gmd_UpdateOpt();
    
    /*
     * List controllers from bd 
     */
    void ContrList( vector<string> & List );
    /*
     * Add Controller for type controllers <tip> with BD <bd>
     */    
    unsigned AddContr( string name, string tip, string t_bd, string n_bd, string n_tb );
    /*
     * Delete Controller
     */    
    void DelContr( unsigned id );
    /*
     * Load/Reload all BD and update internal controllers structure!
     */
    void LoadBD( );
    /*
     * Update all BD from current to external BD.
     */
    void UpdateBD( );
    /*
     * Convert Name parameter to id (id - individual number of parameter for fast calling to parameter )
     */
    unsigned NameCntrToId( string Name );
    TController &at( unsigned id_hd);
    
    TTipController &at_tp( unsigned id ){ return( (TTipController &)gmd_at(id) ); }
    TTipController &operator[]( unsigned id ){ return( at_tp(id) ); }
    
    string TypeGenBD() { return(t_bd); }
    string NameGenBD() { return(n_bd); }
    string NameGenTB() { return(n_tb); }
    
/** Public atributes: */
public:
/** Private atributes: */
private:
    string t_bd;
    string n_bd;
    string n_tb;
    
    vector< SContr >          Contr;   //Controllers list from BD
    
    static SCfgFld            gen_elem[];  //Generic BD elements
    //TConfigElem               gen_ecfg;   
    
    static const char 	      *o_name;
    static const char 	      *n_opt;
/** Private methods: */
private:
    /**
     * Print desription of command line options.
     */
    void pr_opt_descr( FILE * stream );

    int HdIns( int id );
    int HdFree( int id );
    int HdChange( int id1, int id2 );
};

#endif // TCONTROLLERS_H
