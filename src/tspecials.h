
#ifndef TSPECIALS_H
#define TSPECIALS_H

#include <string>
using std::string;

#include "tmodule.h"
#include "tgrpmodule.h"

//================================================================
//=========== TSpecialS ==========================================
//================================================================

class TSpecialS : public TGRPModule
{

/** Public methods: */
public:
    TSpecialS( TKernel *app );

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
