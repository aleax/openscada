
#ifndef TGUIS_H
#define TGUIS_H

#include <string>
using std::string;

#include "tmodule.h"
#include "tgrpmodule.h"

//================================================================
//================== TGUI ========================================
//================================================================

class TGUI : public TModule
{
/** Public methods: */
    public:
     	TGUI( ){ }

	virtual void Start( ){ }
	virtual void Stop( ){ }

	/** Private atributes: */
    private:
	static const char *o_name;
};

//================================================================
//================== TGUIS =======================================
//================================================================

class TGUIS : public TGRPModule
{
    /** Public methods: */
    public:
	TGUIS( TKernel *app );

	TGUI &gmd_at(unsigned int id) const
	{ return( (TGUI &)TGRPModule::gmd_at(id) ); }
	TGUI &operator[](unsigned int id) const
	{ return( gmd_at(id) ); }

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

#endif // TGUIS_H
