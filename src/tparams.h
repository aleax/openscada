
#ifndef TPARAMS_H
#define TPARAMS_H

#include <string>
using std::string;
#include <vector>
using std::vector;

#include "terror.h"

class TParam;
class TParamContr;

class TParamS
{

/** Public methods: */
public:
     TParamS( TKernel *app );

     ~TParamS(  );

    void List( vector<string> & List);     
    /*
     * Add param 
     */     
    int Add(TParamContr *PrmCntr);
    /*
     * Delete param
     */
    int Del(TParamContr *PrmCntr);
    /*
     * numbers param
     */
    unsigned Size() { return(param.size()); }
    /*
     * Convert Name parameter to hd (hd - individual number of parameter for fast calling to parameter )
     */
    int NameToHd( string Name );

    TParam *at( unsigned int id_hd ) 
    { if(id_hd >= hd.size() || hd[id_hd] < 0 ) throw TError("%s: error parameter hd!",o_name); else return(param[hd[id_hd]]); }
    TParam *at( string name )
    { return(at(NameToHd(name))); }
//Start scheduler of params
public:
    const TKernel *App;
/** Private methods: */
private:
    /*
     * Hd operations
     */
    int HdIns( int id );
    int HdFree( int id );
    int HdChange( int id1, int id2 );
    
/**Attributes: */
private:
    vector< int >    hd;                         //header of parameter
    vector<TParam *> param;

    static const char      *o_name;
};

#endif // TPARAMS_H
