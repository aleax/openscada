
#ifndef TAPPLICATION_H
#define TAPPLICATION_H

#include <string>
using std::string;
#include <stdio.h>

class    TGUI;
class    TArhive;
class    TBD;
class    TControllerS;
class    TProcRequest;
class    TProtocol;
class    TSpecial;
class    TMessage;
class    TParamS;
class    TModSchedul;

class TApplication
{

/** Public methods: */
public:
    TApplication( int argi, char **argb, char **env );
    ~TApplication(  );

    /**
      * Run server
      */
    int run(  );

    string UserName() { return(User); }
    
    /**
      * Print comand line options! 
      */
    void pr_opt_descr( FILE * stream );

    string IOCharSet() { return(IO_Char_Set); }
    string CfgFile()   { return(Conf_File); }
/**Attributes: */
public:
    TGUI         *GUI;
    TArhive      *Arhive;
    TBD          *BD;
    TControllerS *Controller;
    TProcRequest *ProcRequest;
    TProtocol    *Protocol;
    TSpecial     *Special;
    TMessage     *Mess;
    TParamS      *Param;
    TModSchedul  *ModSchedul;
    /*
     * Debug level!
     */
    int d_level;
    /*
     * A comand line seting counter.
     */
    const int argc;
    /*
     * A comand line seting buffer.
     */
    const char **argv;
    /*
     * A system environment.
     */ 
    const char **envp;
    /*
     * Direction a logs and other informations!
     */
    int log_dir;
    /*
     * Direct config acces mode;
     */
    bool dir_cfg;

    string ModPath;
    /*
     * Get optio from generic config file.
     */
    void UpdateOpt(); 
    string GetOpt(string section, string opt);
/** Private methods: */
private:
    void CheckCommandLine(bool mode = false );
//    void CheckCommandLine(bool mode );
private:
    /**
      * A owner user name!
      */
    string User;
    string IO_Char_Set;
    string Conf_File;

    static const char *n_opt;
};

extern TApplication *App;

#endif // TAPPLICATION_H
