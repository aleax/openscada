
#ifndef TTRANSPORTS_H
#define TTRANSPORTS_H

#include <string>
using std::string;

#include "tgrpmodule.h"

class TTransportS : public TGRPModule
{

/** Public methods: */
public:
     TTransportS( TKernel *app );

    void Start(  );

    void CheckCommandLine( char **argv, int argc );
    void UpdateOpt();

/** Private methods: */
private:
    void pr_opt_descr( FILE * stream );

/** Private atributes: */
private:
};

#endif // TTRANSPORTS_H
