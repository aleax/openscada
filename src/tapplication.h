
#ifndef TAPPLICATION_H
#define TAPPLICATION_H

#include <string>
#include <stdio.h>

class    TArhive;
class    TBD;
class    TTipController;
class    TProcRequest;
class    TProtocol;
class    TSpecial;
class    TMessage;
class    TTipParam;
class    TModSchedul;

class TApplication
{

/** Public methods: */
public:
    TApplication( int argi, char **argb );
    ~TApplication(  );

    int RegFunc( char * NameTipM, char * NameMod, char * NameFunc, void *addr, string NameSrcMod );

    int ReloadBD( string mode );

    int SaveBD(  );

    /**
      * Run server
      */
    int run(  );

    /**
      * Print comand line options! 
      */
    void pr_opt_descr( FILE * stream );



/** Private methods: */
private:
    void CheckCommandLine(bool mode = false );


/**Attributes: */

public:
    TArhive * Arhive;
    TBD * BD;
    TTipController * TipController;
    TProcRequest * ProcRequest;
    TProtocol * Protocol;
    TSpecial * Special;
    TMessage * Mess;
    TTipParam * Param;
    TModSchedul * ModSchedul;
    /**
      * Debug level!
      */
    int d_level;
    /**
      * A comand line seting counter.
      */
    const int argc;
    /**
      * A comand line seting buffer.
      */
    const char **argv;
    /**
      * Direction a logs and other informations!
      */
    int log_dir;

    char *ModPath;
private:
    /**
      * A owner user name!
      */
    char * UserName;
};


extern TApplication *App;

#endif // TAPPLICATION_H
