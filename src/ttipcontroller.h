#ifndef TTIPCONTROLLER_H
#define TTIPCONTROLLER_H

#include <string>
using std::string;
#include <vector>
using std::vector;

#include "tconfig.h"

class TController;

class TTipController
{
    /** Public methods: */
public:
    TTipController();
    ~TTipController();

    int LoadElCtr( SElem *elements, int numb );
    int LoadElParm( int n_prm, SElem *elements, int numb );
    
    int LoadCtrCfg( int id );
    int SaveCtrCfg( int id );

    int LoadParmCfg( int id_ctr, int id_prm );
    int SaveParmCfg( int id_ctr, int id_prm );    
    
    /** Public atributes: */
public:
    int                   idmod;   // ID module into TGRPModule
    vector<TController *> contr;   // List controller's params
    TConfigElem           conf_el; // Structure configs of controller
    vector<TConfigElem *> param;   // List type parameter and Structure configs of parameter.
};

#endif // TTIPCONTROLLER_H

