
#ifndef TSPECIALS_H
#define TSPECIALS_H

#include <string>
using std::string;

#include "tmodule.h"
#include "tgrpmodule.h"

//================================================================
//=========== TSpecial ===========================================
//================================================================
class TSpecialS;

class TSpecial: public TModule
{
    /** Public methods: */
    public:
	TSpecial( );
	virtual ~TSpecial();

    public:
	TSpecialS *owner;
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

    TSpecial *at_tp( unsigned id ) { return( (TSpecial *)gmd_at(id) ); }
    TSpecial *operator[]( unsigned id ) { return( at_tp(id) ); }

    void gmd_CheckCommandLine( );
    void gmd_UpdateOpt();
/** Private methods: */
private:
    void pr_opt_descr( FILE * stream );
    virtual int gmd_AddM( TModule *modul );

/** Private atributes: */
private:
    static const char *o_name;
};

#endif // TSPECIALS_H
