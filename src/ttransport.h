
#ifndef TTRANSPORT_H
#define TTRANSPORT_H

#include <string>
using std::string;

#include "tgrpmodule.h"

class TTransport : public TGRPModule
{

/** Public methods: */
public:
     TTransport(  );

    void Start(  );

    void CheckCommandLine(  );
    void UpdateOpt();

/** Private methods: */
private:
    void pr_opt_descr( FILE * stream );

/** Private atributes: */
private:
};

#endif // TTRANSPORT_H
