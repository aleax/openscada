
#ifndef TPARAMS_H
#define TPARAMS_H

#include <string>
#include <vector>

#include "thd.h"
#include "tconfig.h"
#include "terror.h"
//#include "tcontrollers.h"

using std::string;
using std::vector;

class SCntrS;
class TParam;
class TParamContr;

class TParamS : public TConfig
{

/** Public methods: */
public:
    TParamS( TKernel *app );

    ~TParamS(  );

    /*
     * Avoid parameters list
     */
    void list( vector<string> &list )
    { m_hd.obj_list( list ); }
    /*
     * Add parameter
     */
    void add( SCntrS cntr, string param );
    /*
     * Del parameter
     */
    void del( SCntrS cntr, string param );
    /*
     * Attach to parameter
     * Return parameter header
     */
    unsigned att( string name )
    { return( m_hd.hd_att( name ) ); }
    /*
     * Detach from parameter
     */
    void det( unsigned hd )
    { m_hd.hd_det( hd ); }
    /*
     * Get attached parameter
     */
    TParam &at( unsigned hd )
    { return( *(TParam *)m_hd.hd_at( hd ) ); }
    TParam &operator[]( unsigned hd ){ return( at(hd) ); }

    TKernel &Owner() { return( *owner ); }
    
    //void List( vector<string> & List);     
    /*
     * Add param 
     */     
    //int Add(TParamContr *PrmCntr);
    /*
     * Delete param
     */
    //int Del(TParamContr *PrmCntr);
    /*
     * numbers param
     */
    //unsigned Size() { return(param.size()); }
    /*
     * Convert Name parameter to hd (hd - individual number of parameter for fast calling to parameter )
     */
    //int NameToHd( string Name );

    //TParam &at( unsigned int id_hd ) 
    //{ if(id_hd >= hd.size() || hd[id_hd] < 0 ) throw TError("%s: error parameter hd!",o_name); else return(*param[hd[id_hd]]); }
    //TParam &operator[]( unsigned int id_hd ) 
    //{ return( at(id_hd) ); }
    
//Start scheduler of params
public:
/** Private methods: */
private:
    /*
     * Hd operations
     */
    //int HdIns( int id );
    //int HdFree( int id );
    //int HdChange( int id1, int id2 );
    
/**Attributes: */
private:
    //vector< int >    hd;                         //header of parameter
    //vector<TParam *> param;
    THD               m_hd;
    
    TKernel           *owner;

    static SCfgFld    gen_elem[];  
    static const char *o_name;
};

#endif // TPARAMS_H
