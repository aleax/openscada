#ifndef TPARAM_H
#define TPARAM_H

#include <string>
using std::string;
#include <vector>
using std::vector;

class TParamContr;

class TParam 
{

/** Public methods: */
public:
    TParam( TParamContr *PrmCntr );

    ~TParam(  );

    string Name() { return(name); }
    int Reg(TParamContr *PrmCntr);
/*
    int LoadBD(  );

    int SetVal( int val );

    int GetVal(  );

    int ReloadBD(  );

    int SaveBD(  );

    int GetParamList( string NameContr, string & list );

    int OperParam( string NameParam, string NameContr, string NameContrTip, string NameSecond, string operation );

    int GetValArhiv( string NameParm, long BegTime, long EndTime, int Period, char * buf );
*/
private:
    string name;
    vector< TParamContr *> ParamC; 
};

#endif // TPARAM_H
