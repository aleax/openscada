
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
    int OpenBD( string nametype, string name );
    int OpenBD( int idtype, string name );
    
    int CloseBD( int hd );
    int CloseBD( string nametype, int hd );
    int CloseBD( int idtype, int hd );

    void Start(  );

    string GetListBDTip(  );

    void CheckCommandLine(  );
    
    virtual bool AddM(char *name);

    bool test(int idtype);
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
