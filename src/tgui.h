
#ifndef TGUI_H
#define TGUI_H

#include <string>
using std::string;

#include "tgrpmodule.h"

class TGUI : public TGRPModule
{

/** Public methods: */
public:
     TGUI(  );

    void Start(  );

    void CheckCommandLine(  );
    void UpdateOpt();

/** Private methods: */
private:
    void pr_opt_descr( FILE * stream );

/** Private atributes: */
private:
};

#endif // TGUI_H
