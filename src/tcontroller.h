
#ifndef TCONTROLLER_H
#define TCONTROLLER_H

#include <string>
using std::string;

#include "tconfig.h"
#include "tvalue.h"

class TTipController;
class TParamContr;

class TController : public TConfig
{

/** Public methods: */
public:
     TController(TTipController *tcntr, string name_c, string bd_c, TConfigElem *cfgelem);

     ~TController(  );

     int RegParam();

/**Attributes: */
public:
    string  bd;
    string  name;
//    TConfig *config;            //config controller
    vector< vector <TParamContr *> > prm_cfg; //config parameter

    TTipController *TContr;
/**Attributes: */
private:
    
};

//==============================================================================================
//Use for register into TControllers and make direct access to controller function for parameter
//and direct acces to parameter from controller
//==============================================================================================

class TParamContr : public TConfig
{
/** Public methods: */
public:
    TParamContr(TController *contr, TConfigElem *cfgelem, TConfigElem *cfgval); 
    ~TParamContr();
    /*
     * Param name
     */
    string Name();
    /*
     * Add value
     */
    int AddVal(int id_val, SBlock *block);
/**Attributes: */
public:
    TController *controller;
/**Attributes: */
private:
    int         owner;   // id from password
    int         group;   // id from group

    TValue      val;    // value buffer
};

#endif // TCONTROLLER_H
