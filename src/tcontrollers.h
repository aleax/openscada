
#ifndef TCONTROLLERS_H
#define TCONTROLLERS_H

#define  VER_CNTR 1    //ControllerS type modules version

#include <string>

#include "tbds.h"
#include "tconfig.h"
#include "tgrpmodule.h"
#include "ttipcontroller.h"

using std::string;

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

class TControllerS : public TGRPModule, public TConfigElem
{
    /** Public methods: */
    public:
	TControllerS( TKernel *app );
	~TControllerS(  );

	int gmd_Ver( ) { return(VER_CNTR); }
	// Init All controller's modules    
	void gmd_Init( );
	// Init and start all configured controllers. 
	virtual void gmd_Start(  );                                         
	// Stop and deinit all configured controllers. 
	virtual void gmd_Stop(  );

	void gmd_CheckCommandLine( );
	void gmd_UpdateOpt();
    
	TTipController &gmd_at( unsigned hd ){ return( (TTipController &)TGRPModule::gmd_at(hd) ); }
	TTipController &operator[]( unsigned hd ){ return( gmd_at(hd) ); }

	// Avoid controllers list
	void list( vector<SCntrS> &list );
	// Add controller
	void add( SCntrS cntr, SBDS bd );
	// Del controller
	void del( SCntrS cntr );
	/*
	* Attach to controller
	* Return controller header
	*/
	SHDCntr att( SCntrS cntr, string how = "" );
	// Detach from controller
	void det( SHDCntr &hd );
	// Get attached controller
	TController &at( SHDCntr &hd ) { return( gmd_at( hd.h_tp ).at( hd.h_obj ) ); }
	
	// Load/Reload all BD and update internal controllers structure!
	void LoadBD( );
	// Update all BD from current to external BD.
	void UpdateBD( );    
    
	SBDS &BD() { return(m_bd); }
    /** Public atributes: */
    public:
	
    /** Private methods: */
    private:
	string opt_descr( );

	void gmd_del( string name );
	//================== Controll functions ========================
	void ctr_fill_info( XMLNode *inf );
	void ctr_din_get_( string a_path, XMLNode *opt );
	void ctr_din_set_( string a_path, XMLNode *opt );
	void ctr_cmd_go_( string a_path, XMLNode *fld, XMLNode *rez );
    /** Private atributes: */
    private:
	SBDS     m_bd;
    
	static SCfgFld            gen_elem[];  //Generic BD elements
    
	static const char 	      *o_name;
	static const char *i_cntr;
};

#endif // TCONTROLLERS_H
