
#ifndef TPROTOCOL_H
#define TPROTOCOL_H

#include <string>

#include "tgrpmodule.h"

class TProtocol : public TGRPModule
{

/** Public methods: */
public:
     TProtocol(  );

    void Init(  );

    void Start(  );

    void CheckCommandLine(  );

/** Private methods: */
private:
    void pr_opt_descr( FILE * stream );

/** Private atributes: */
private:
};

#endif // TPROTOCOL_H
