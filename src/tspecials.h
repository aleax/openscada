
#ifndef TSPECIALS_H
#define TSPECIALS_H

#include <string>
using std::string;

#include "tmodule.h"
#include "tgrpmodule.h"

//================================================================
//=========== TSpecial ===========================================
//================================================================

class TSpecial : public TModule
{
    /** Public methods: */
    public:
	TSpecial( ){ }

	virtual void Start( ){ }
	virtual void Stop( ){ }
    
    /** Private atributes: */
    private:
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

	TSpecial &gmd_at(unsigned hd)     { return( (TSpecial &)TGRPModule::gmd_at(hd) ); } 
	TSpecial &operator[](unsigned hd) { return( gmd_at(hd) ); } 

	void gmd_Start( );
	void gmd_Stop( );

	void gmd_CheckCommandLine( );
	void gmd_UpdateOpt();
    /** Private methods: */
    private:
	void pr_opt_descr( FILE * stream );

    /** Private atributes: */
    private:
	static const char *o_name;
};

#endif // TSPECIALS_H
