
#ifndef TGUIS_H
#define TGUIS_H

#include <string>
using std::string;

#include "tgrpmodule.h"

class TGUIS : public TGRPModule
{

/** Public methods: */
public:
     TGUIS( TKernel *app );

    void Start(  );

    void CheckCommandLine( char **argv, int argc );
    void UpdateOpt();

/** Private methods: */
private:
    void pr_opt_descr( FILE * stream );

/** Private atributes: */
private:
};

#endif // TGUIS_H
