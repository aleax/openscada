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
    /*
     * Registering controller's param
     */
    int Reg(TParamContr *PrmCntr);
    /*
     * Unregistering controller's param
     */
    int UnReg(TParamContr *PrmCntr);
    /*
     * ammount controller of owners
     */
    unsigned Size() { return(ParamC.size()); }

    TParamContr &at() { return(*ParamC[work]); }
    
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
    int    work;
};

#endif // TPARAM_H
