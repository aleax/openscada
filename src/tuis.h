
#ifndef TUIS_H
#define TUIS_H

#define  VER_UI 1    //UIS type modules version

#include <string>

#include "tmodule.h"
#include "tgrpmodule.h"

using std::string;
//================================================================
//================== TUI =========================================
//================================================================

class TUI : public TModule
{
/** Public methods: */
    public:
     	TUI( );

	virtual void start( ){ }
	virtual void stop( ){ }	
    protected:
	//================== Controll functions ========================
	void ctr_fill_info( XMLNode *inf );
	void ctr_din_get_( string a_path, XMLNode *opt );
	void ctr_din_set_( string a_path, XMLNode *opt );
    protected:
	bool  run_st; 
    /** Private atributes: */
    private:
	static const char *i_cntr;
	static const char *o_name;
};

//================================================================
//================== TUIS ========================================
//================================================================

class TUIS : public TGRPModule
{
    /** Public methods: */
    public:
	TUIS( TKernel *app );

	int gmd_Ver( ) { return(VER_UI); }

	TUI &gmd_at(unsigned hd)     { return( (TUI &)TGRPModule::gmd_at(hd) ); }
	TUI &operator[](unsigned hd) { return( gmd_at(hd) ); }

	void gmd_Start( );
	void gmd_Stop( );

	void gmd_CheckCommandLine( );
	void gmd_UpdateOpt();

    /** Private methods: */
    private:
	string opt_descr( );	

	//================== Controll functions ========================
	void ctr_fill_info( XMLNode *inf );
	void ctr_din_get_( string a_path, XMLNode *opt );
    /** Private atributes: */
    private:
	static const char *i_cntr;
	static const char *o_name;
};

#endif // TUIS_H
