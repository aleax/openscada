
#ifndef TARHIVE_H
#define TARHIVE_H

#include <string>
using std::string;
#include "tgrpmodule.h"

/**
  * class TArhive
  * Object of arhiv, protokol, and other  tasks protocoling.
  */

class TArhive : public TGRPModule
{

/** Public methods: */
public:
     TArhive(  );

     ~TArhive(  );

    void Init(  );

    void Start(  );

    string GetListArhivTip(  );

    int GetVal( string NameArhiv, string NameParm, string BegTime, int EndTime, int pereodic, char * buf );
    
    void CheckCommandLine(  );
/** Privateds: */
private:
    void pr_opt_descr( FILE * stream );
    
/** Private atributes: */
private:
};

#endif // TARHIVE_H
