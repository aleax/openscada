
#ifndef TBD_H
#define TBD_H

#include <string>
#include <vector>

#include "tgrpmodule.h"


class TBD : public TGRPModule
{

/** Public methods: */
public:
     TBD(  );

     ~TBD(  );

    void Init(  );

    int OpenBD( string name );

    int CloseBD( int hd );

    void Start(  );

    string GetListBDTip(  );

    void CheckCommandLine(  );
    
    virtual bool AddM(char *name);
/** Public atributes:: */
public:

/** Protected methods: */
protected:
    
    
/** Private methods: */
private:
    void pr_opt_descr( FILE * stream );

/** Private atributes: */
private:
    vector< vector<int> > hdBD;
};

#endif // TBD_H
