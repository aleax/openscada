
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

struct SC_HD
{
    int tprm;
    int prm;
};

class TParamContr;
class TTipController;

class TController : public TConfig
{

/** Public methods: */
public:
    TController(TTipController *tcntr, string name_c, string _t_bd, string _n_bd, string _n_tb, TConfigElem *cfgelem);

    virtual ~TController(  );

    virtual int Load(  );
    virtual int Save(  );
    virtual int Free(  );
    virtual int Start(  );
    virtual int Stop(  );
    virtual int Enable(  );
    virtual int Disable(  );

    virtual TParamContr *ParamAttach(int type);
    /*
     * Add parameter with type Name_P and <name> to position <pos> (<0 - to end) 
     */
    int  Add( string Name_TP, string name, int pos );
    /*
     * Delete parameter with type Name_P and <name>
     */    
    int  Del( string Name_TP, string name );
    /*
     * Rotated parameter with type Name_P between name1 and name2
     */
    int  Rotate( string Name_P, string name1, string name2);


    string Name() { return(name); }
    char   Stat() { return(stat); }
    /*
     * Registering parameter(s)
     */
    int RegParamS();
    int RegParam( int id_hd );
    /*
     * UnRegistering parameter(s)
     */
    int UnRegParamS();
    int UnRegParam( int id_hd );

    void List( string Name_TP, vector<string> & List);
    /*
     * Convert Name parameter to hd (hd - individual number of parameter for fast calling to parameter )
     */
    int NameToHd( string Name );
    /*
     * Get Parameter throw hd (individual parameter number)
     */
    TParamContr *at(int id_hd);
    TParamContr *at(string name) { return(at(NameToHd(name))); }

    TTipController *owner;
/**Attributes: */
public:
    
/** Private methods: */
private:
    /*
     * Hd operations
     */
    int HdIns( int id_P, int id );
    int HdFree( int id_P, int id );
    int HdChange( int id_P, int id1, int id2 );

    int LoadParmCfg( string name_t_prm );
    int LoadParmCfg( unsigned i_t );
    int SaveParmCfg( string name_t_prm );
    int SaveParmCfg( unsigned i_t );
    int FreeParmCfg( string name_t_prm );
    int FreeParmCfg( unsigned i_t );
/**Attributes: */
private:
    string  name;
    char    stat;
    string  t_bd;                             // type bd
    string  n_bd;                             // name bd
    string  n_tb;                             // name table    
    vector< SC_HD > hd;                       //header of parameter 

    static const char *o_name;
protected:    
    vector< vector <TParamContr *> > prm_cfg; //config parameter
};


#endif // TCONTROLLER_H
