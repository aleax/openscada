
#ifndef TPARAMS_H
#define TPARAMS_H

#include <string>
using std::string;
#include <vector>
using std::vector;

class TParam;

class TParams
{

/** Public methods: */
public:
     TParams(  );

     ~TParams(  );

//Start scheduler of params
    int StartSched(  );
     
    int LoadBD(  );

    int SetVal( int val );

    int GetVal(  );

    int ReloadBD(  );

    int SaveBD(  );

    int GetParamList( string NameContr, string NameTipParam, string & list );

    int OperParam( string NameCtrTip, string NameCtr, string NamePrmTip, string NameParm, string NameSecond, string operat );

    int GetValArhiv( string NamePrm, long BegTime, long EndTime, long period, char * buf );



/**Attributes: */

private:
    vector < vector<TParam *> > param;
    vector <string> tip;
};

#endif // TPARAMS_H
