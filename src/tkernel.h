
#ifndef TKERNEL_H
#define TKERNEL_H

#include <stdio.h>

#include <string>
using std::string;
#include <vector>
using std::vector;

class    TGUIS;
class    TArhiveS;
class    TBDS;
class    TControllerS;
class    TProtocolS;
class    TTransportS;
class    TSpecialS;
//class    TMessage;
class    TParamS;
class    TModSchedul;

class TKernel
{

/** Public methods: */
public:
    TKernel( );
    ~TKernel(  );
    /**
      * Run server
      */
    int run(  );
    /*
     * Get optio from generic config file.
     */
    void UpdateOpt();
    /*
     * Update comand line option
     */
    void CheckCommandLine(bool mode = false );
    /*
     * Print comand line options! 
     */
    void pr_opt_descr( FILE * stream );
/**Attributes: */
public:
    TGUIS        *GUI;
    TArhiveS     *Arhive;
    TBDS         *BD;
    TControllerS *Controller;
    TProtocolS   *Protocol;
    TTransportS  *Transport;
    TSpecialS    *Special;
    TParamS      *Param;
    TModSchedul  *ModSchedul;
    /*
     * Direct config acces mode;
     */
    bool dir_cfg;

    string ModPath;
    string DefBDType;
    string DefBDName;

    vector<string> allow_m_list;
    vector<string> deny_m_list;
/** Private methods: */
private:
//    void CheckCommandLine(bool mode );
private:    

    static const char *n_opt;
};

#endif // TKERNEL_H
