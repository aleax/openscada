#ifndef TTIPCONTROLLER_H
#define TTIPCONTROLLER_H

#include <string>
using std::string;
#include <vector>
using std::vector;

#include "./moduls/gener/tmodule.h"
#include "tconfig.h"

class TController;
class TModule;

struct SParamT
{
    string      name;
    string      descr;
    string      bd;
    TConfigElem confs;   // Structure configs of parameter.
};

class TTipController
{
    /** Public methods: */
public:
    friend class TController;
    
    TTipController( TModule *mod );
    ~TTipController();
    
    int Add( string & name, string & bd );
    int Del( string & name );
    int LoadContr( string & name );
    int SaveContr( string & name );
    int FreeContr( string & name );
    int Start( string & name );
    int Stop( string & name );
    int Enable( string & name );
    int Disable( string & name );
    /*
     * Free unused controllers from BD and parametes bd unused controllers
     */
    void CleanBD();                //?!?!

    int NameElTpToId(string name_t);    
    /*
     * Size buffer (elements may by free)
     */    	
    unsigned Size() { return(contr.size()); }

    int NameToHd( string Name );
    
    TController *at(string name) { return( at(NameToHd(name)) ); }
    TController *at(int id_hd )  
    { if(id_hd >= hd.size() || id_hd < 0 || hd[id_hd] < 0 ) return(NULL); else return(contr[hd[id_hd]]); }
    /*
     * List controllers 
     */
    void List( vector<string> & List );
    /*
     * List type of param
     */
    void ListTpPrm( vector<string> & List );

    string Name() {return(module->Name());}
    
    int AddTpParm(string name_t, string n_fld_bd, string descr);
    int LoadElCtr( SElem *elements, int numb );

    int LoadElParm(string name_t_prm, SElem *elements, int numb );    
    /** Public atributes: */
public:
    int                   idmod;   // ID module into TGRPModule

    /** Protected methods: */
protected: 
    vector<SParamT *>     paramt;  // List type parameter and Structure configs of parameter.
    /** Private atributes: */
private:    
    TConfigElem           conf_el; // Structure configs of controller
    vector<TController *> contr;   // List controller      !! move to private
    vector< int >         hd;      // Headers for short access to controllers
    TModule               *module; // Controller's modul 
    /** Private methods: */
private:
    int HdIns( int id );
    int HdFree( int id );
    int HdChange( int id1, int id2 );
};

#endif // TTIPCONTROLLER_H

