
#ifndef TCONTROLLERS_H
#define TCONTROLLERS_H

#include <string>
using std::string;

#include "tbds.h"
#include "tconfig.h"
#include "tgrpmodule.h"
#include "ttipcontroller.h"

class SCntrS
{
    public:
	SCntrS( string m_tp, string m_obj ) : tp(m_tp), obj(m_obj) { }
	string tp;
	string obj;
};

struct SHDCntr
{
    unsigned h_tp;
    unsigned h_obj;
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
    void gmd_Init( );
    /*
     * Init and start all configured controllers.
     */ 
    virtual void gmd_Start(  );                                         
    /*
     * Stop and deinit all configured controllers.
     */ 
    virtual void gmd_Stop(  );

    void gmd_CheckCommandLine( );
    void gmd_UpdateOpt();
    
    TTipController &gmd_at( unsigned hd ){ return( (TTipController &)TGRPModule::gmd_at(hd) ); }
    TTipController &operator[]( unsigned hd ){ return( gmd_at(hd) ); }

    /*
     * Avoid controllers list
     */
    void list( vector<SCntrS> &list );
    /*
     * Add controller
     */
    void add( SCntrS cntr, SBDS bd );
    /*
     * Del controller
     */
    void del( SCntrS cntr );
    /*
     * Attach to controller
     * Return controller header
     */
    SHDCntr att( SCntrS cntr );
    /*
     * Detach from controller
     */
    void det( SHDCntr &hd );
    /*
     * Get attached controller
     */
    TController &at( SHDCntr &hd ) { return( gmd_at( hd.h_tp ).at( hd.h_obj ) ); }
    
    /*
     * Load/Reload all BD and update internal controllers structure!
     */
    void LoadBD( );
    /*
     * Update all BD from current to external BD.
     */
    void UpdateBD( );
    
    
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
    
    static SCfgFld            gen_elem[];  //Generic BD elements
    
    static const char 	      *o_name;
/** Private methods: */
private:
    /**
     * Print desription of command line options.
     */
    void pr_opt_descr( FILE * stream );

    void gmd_del( string name );
};

#endif // TCONTROLLERS_H
