#ifndef TTIPCONTROLLER_H
#define TTIPCONTROLLER_H

#include <string>
using std::string;
#include <vector>
using std::vector;

#include "tmodule.h"
#include "tvalue.h"
#include "tconfig.h"

class TController;
class TModule;

struct SParamT
{
    string      name;
    string      descr;
    string      bd;      // Name field into controller's BD for it type
    TConfigElem confs;   // Structure configs of parameter.
};

class TTipController : public TModule
{
    /** Public methods: */
public:
    friend class TController;
    
    TTipController( );
    virtual ~TTipController();
    
    unsigned Add( string name, string t_bd, string n_bd, string n_tb);
    void Del( string name );

    /*
     * Free unused controllers from BD and parametes bd unused controllers
     */
    //void CleanBD();                //?!?!

    int NameElTpToId(string name_t);    
    /*
     * Size buffer (elements may by free)
     */    	
    unsigned Size() { return(contr.size()); }

    int NameToHd( string Name );
    
    TController *at(string name) { return( at(NameToHd(name)) ); }
    TController *at(unsigned int id_hd )  
    { if(id_hd >= hd.size() || hd[id_hd] < 0 ) return(NULL); else return(contr[hd[id_hd]]); }
    /*
     * List controllers 
     */
    void List( vector<string> & List );
    /*
     * List type of param
     */
    void ListTpPrm( vector<string> & List );
    TConfigElem *at_TpPrmCfg( unsigned id )
    { if(id >= paramt.size()) throw TError("%s: id of param type error!",o_name); return(&paramt[id]->confs); }

    void LoadElCtr( SCfgFld *elements, int numb );
    int AddTpParm(string name_t, string n_fld_bd, string descr);
    int LoadElParm(string name_t_prm, SCfgFld *elements, int numb );
    void AddValType(string name, SVAL *vl_el, int number);

    void ListTpVal( vector<string> & List );
    TValueElem *at_val( string name);

    TConfigElem *ConfigElem() { return(&conf_el); }
    
    /** Public atributes: */
public:
    int                   idmod;   // ID module into TGRPModule
    TControllerS          *owner;  // 
    /** Protected methods: */
protected: 
    virtual TController *ContrAttach(string name, string t_bd, string n_bd, string n_tb)
    { throw TError("%s: Error controller %s attach!",o_name,name.c_str()); }
    /** Protected atributes: */
protected: 
    vector<SParamT *>     paramt;  // List type parameter and Structure configs of parameter.
    /** Private atributes: */    
private:    
    TConfigElem           conf_el; // Structure configs of controller
    vector<TValueElem *>  val_el;  // Value types for value of parameter            
    vector<TController *> contr;   // List controller      !! move to private
    vector< int >         hd;      // Headers for short access to controllers

    static SCfgFld        Elem_Ctr[];
    static SCfgFld        Elem_TPrm[];

    static const char     *o_name;
    /** Private methods: */
private:
    int HdIns( int id );
    void HdFree( int id );
    void HdChange( int id1, int id2 );
};

#endif // TTIPCONTROLLER_H

