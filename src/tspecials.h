
#ifndef TSPECIALS_H
#define TSPECIALS_H

#define  VER_SPC 1    //SpecialS type modules version

#include <string>

#include "tmodule.h"
#include "tgrpmodule.h"

using std::string;

//================================================================
//=========== TSpecial ===========================================
//================================================================

class TSpecial : public TModule
{
    /** Public methods: */
    public:
	TSpecial( );

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
//=========== TSpecialS ==========================================
//================================================================

class TSpecialS : public TGRPModule
{

    /** Public methods: */
    public:
	TSpecialS( TKernel *app );

	int gmd_Ver( ) { return(VER_SPC); }

	TSpecial &gmd_at(unsigned hd)     { return( (TSpecial &)TGRPModule::gmd_at(hd) ); } 
	TSpecial &operator[](unsigned hd) { return( gmd_at(hd) ); } 

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

#endif // TSPECIALS_H
