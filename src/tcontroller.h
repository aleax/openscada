
#ifndef TCONTROLLER_H
#define TCONTROLLER_H

#include <string>
using std::string;
#include "tconfig.h"

class TTipController;
class TTipParam;

class TController : public TConfig
{

/** Public methods: */
public:
     TController( TTipController & Controller );

     ~TController(  );

    string GetList(  );

    int LoadBD(  );

    int SetParam(  );

    int ReloadBD(  );

    int SaveBD(  );

    int GetContrInfo( string NameContr );

    int SendContrCom( string NameCtr, string Command );

    void GetParamTipList( string NameTask );

    int OperContr( string NameCtr, string NameSecond, string opper );



/**Attributes: */

private:
    TController * Controller;
    TTipParam * TipParam;
};

#endif // TCONTROLLER_H
