
#ifndef TPARAM_H
#define TPARAM_H

#include <string>
#include <vector>

#include "tconfig.h"

class TConfig;

class TParam 
{

/** Public methods: */
public:
    TParam(  );

    ~TParam(  );

    int LoadBD(  );

    int SetVal( int val );

    int GetVal(  );

    int ReloadBD(  );

    int SaveBD(  );

    int GetParamList( string NameContr, string & list );

    int OperParam( string NameParam, string NameContr, string NameContrTip, string NameSecond, string operation );

    int GetValArhiv( string NameParm, long BegTime, long EndTime, int Period, char * buf );
private:
    vector<TConfig *> config;
};

#endif // TPARAM_H
