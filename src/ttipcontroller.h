
#ifndef TTIPCONTROLLER_H
#define TTIPCONTROLLER_H

#include <string>

#include "tgrpmodule.h"

class TTipController : public TGRPModule
{

/** Public methods: */
public:
     TTipController(  );

    int (*MGetParamVal)(  );

    int (*MSetParamVal)(  );

    /**
      * 
      * @param NameContrTip
      *        Controller's name.
      */
    string GetContrList( string NameContrTip );

    void Init(  );

    /**
      * Init one (Name) controller's task.
      */
    int InitTask( string NameBDTask );

    void Start(  );

    int SaveBD(  );

    /**
      * Get the stat of controller's task. 
      * @param NameTipCtr
      *        Name of controller.
      */
    int GetContrInfo( string NameTipCtr, string NameCtr );

    int SendContrCom( string NameCtr, string Command );

    string GetParamTipList( string NameCtr, string NameTask );

    int ReloadBDCtr( string Command );

    int PutCom( string type );

    string GetContrTipList(  );

    int (*MGetConfigCadr)( string & cadr );

    int (*MPutConfigCadr)( string cadr );

    int OperContr( string NameTip, int NameCtr, string NameSecond, string opper );

    void CheckCommandLine(  );

/** Private methods: */
private:
    /**
      * It's gate to function of modul.   Load modul's (self) parameters from phisic BD
      * to local BD.
      */
    int (*MLoadSelfBD)( string NameCtr, int hdBD, string mode );

    /**
      * It's gate to function of modul.   Save modul's (self) parameters from local BD to
      * phisic BD.
      */
    int (*MSaveSelfBD)( string NameCtr, int hdBD, string mode );

    /**
      * It's gate to function of modul.   Get controller's (self) status.
      */
    int (*MGetCtrInfo)( string NameContr, string & info );

    void pr_opt_descr( FILE * stream );

/** Private atributes: */
private:
};

#endif // TTIPCONTROLLER_H
