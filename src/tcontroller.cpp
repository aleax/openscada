
#include "tcontroller.h"

 TController::TController( string name_c, string bd_c ) : config(NULL), name(name_c), bd(bd_c) 
{
    
}


 TController::~TController(  )
{
    if(config != NULL) delete config;
}


