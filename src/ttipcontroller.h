#ifndef TTIPCONTROLLER_H
#define TTIPCONTROLLER_H

#include <string>
using std::string;
#include <vector>
using std::vector;

#include "tconfig.h"

class TController;
class TModule;

class TTipController
{
    /** Public methods: */
public:
    TTipController( TModule *mod );
    ~TTipController();

    int LoadElCtr( SElem *elements, int numb );
    int LoadElParm( int n_prm, SElem *elements, int numb );
    
    int LoadCtrCfg( int id );
    int SaveCtrCfg( int id );

    int LoadParmCfg( int id_ctr, int id_prm );
    int SaveParmCfg( int id_ctr, int id_prm );

    int Size() { return(contr.size()); }
    TController *at(unsigned int id)
    { if(id >= Size()) return(NULL); else return(contr[id]); }
    
    /** Public atributes: */
public:
    int                   idmod;   // ID module into TGRPModule
    TConfigElem           conf_el; // Structure configs of controller
    vector<TController *> contr;   // List controller      !! move to private
    vector<TConfigElem *> param;   // List type parameter and Structure configs of parameter.
    TModule               *module; // Controller's modul
    /** Private atributes: */
private:    
   /** Private methods: */
private:
//    SElem  BLock[];
//    RecStr RStr[];
};

#endif // TTIPCONTROLLER_H

