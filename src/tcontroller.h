
#ifndef TCONTROLLER_H
#define TCONTROLLER_H

#include <time.h>
#include <string>
using std::string;

#include "tkernel.h"
#include "tparams.h"
#include "tconfig.h"

#define TCNTR_DISABLE 0   //disabled
#define TCNTR_ENABLE  1   //enabled
#define TCNTR_RUN     2   //run
#define TCNTR_ERR     3   //error

class TParamContr;
class TTipController;

class TController : public TConfig
{

/** Public methods: */
public:
    TController(TTipController *tcntr, string name_c, string _t_bd, string _n_bd, string _n_tb, TConfigElem *cfgelem);

    virtual ~TController(  );

    virtual void Load(  );
    virtual void Save(  );
    virtual void Free(  );
    virtual void Start(  );
    virtual void Stop(  );
    virtual void Enable(  );
    virtual void Disable(  );

    virtual TParamContr *ParamAttach(int type);
    /*
     * Add parameter with type Name_P and <name> to position <pos> (<0 - to end) 
     */
    unsigned  Add( string Name_TP, string name, int pos );
    /*
     * Delete parameter with type Name_P and <name>
     */    
    void  Del( string name );
    /*
     * Rotated parameter with type Name_P between name1 and name2
     */
    void  Rotate( string name1, string name2);


    string Name() { return(name); }
    char   Stat() { return(stat); }
    /*
     * Registering parameter(s)
     */
    void RegParamS();
    void RegParam( unsigned id_hd );
    /*
     * UnRegistering parameter(s)
     */
    void UnRegParamS();
    void UnRegParam( unsigned id_hd );

    void List( vector<string> & List );
    /*
     * Convert Name parameter to hd (hd - individual number of parameter for fast calling to parameter )
     */
    unsigned NameToHd( string Name );
    /*
     * Get Parameter throw hd (individual parameter number)
     */
    TParamContr &at( unsigned id_hd );
    TParamContr &operator[]( unsigned id_hd ){ return( at(id_hd) ); }
    
    TTipController &Owner() { return( *owner ); }

/**Attributes: */
public:
/** Private methods: */
private:
    /*
     * Hd operations
     */
    unsigned HdIns( unsigned id );
    void HdFree( unsigned id );
    void HdChange( unsigned id1, unsigned id2 );

    void LoadParmCfg(  );
    void SaveParmCfg(  );
    void FreeParmCfg(  );
/**Attributes: */
private:
    
    string  name;
    char    stat;
    string  t_bd;                             // type bd
    string  n_bd;                             // name bd
    string  n_tb;                             // name table    
    vector< int > hd;                         //header of parameter 
    
    TTipController *owner;    

    static const char *o_name;
protected:    
    vector< TParamContr * > cntr_prm; //config parameter
};


#endif // TCONTROLLER_H
