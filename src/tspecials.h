
#ifndef TSPECIALS_H
#define TSPECIALS_H

#include <string>
using std::string;

#include "tgrpmodule.h"

class TSpecialS : public TGRPModule
{

/** Public methods: */
public:
     TSpecialS( TKernel *app );

    void Start(  );

    void CheckCommandLine( char **argv, int argc );
    void UpdateOpt();
/** Private methods: */
private:
    void pr_opt_descr( FILE * stream );

/** Private atributes: */
private:
};

#endif // TSPECIALS_H
