
#ifndef TPARAMCONTR_H
#define TPARAMCONTR_H

#include <time.h>
#include <string>
using std::string;

#include "tapplication.h"
#include "tparams.h"
#include "tconfig.h"
#include "tvalue.h"

//==============================================================================================
//Use for register into TControllers and make direct access to controller function for parameter
//and direct acces to parameter from controller
//==============================================================================================
class TController;

class TParamContr : public TConfig, public TValue
{
/** Public methods: */
public:
    TParamContr(TController *contr, TConfigElem *cfgelem); 
    ~TParamContr();
    /*
     * Param name
     */
    string Name();
    /*
     * Add value
     */
//    int AddVal(int id_val, SBlock *block);
    /*
     * Compare object
     */
    bool operator==( TParamContr & PrmCntr )
    { if( Name() == PrmCntr.Name() ) return(true); return(false); };
    /*
     * Equaly config 
     */
    TParamContr & operator=( TParamContr & PrmCntr );

    TController *Controller() { return(controller); }

    int UpdateVAL();
/**Attributes: */
public:
    TController *controller;
    time_t      t_sync;  // time synchronized
/**Attributes: */
private:
    short       owner;   // id from password
    short       group;   // id from group
};

#endif // TPARAMCONTR_H
