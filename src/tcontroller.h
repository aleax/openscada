
#ifndef TCONTROLLER_H
#define TCONTROLLER_H

#include <string>
using std::string;
#include "tconfig.h"

class TTipController;
class TTipParam;

class TController
{

/** Public methods: */
public:
     TController( string name_c, string bd_c );

     ~TController(  );

/**Attributes: */
public:
    string  bd;
    string  name;
    TConfig *config;  //config controller
    vector <TConfig *> prm_cfg; //config parameter
/**Attributes: */
private:
//    TController * Controller;
//    TTipParam * TipParam;
    
};

#endif // TCONTROLLER_H
