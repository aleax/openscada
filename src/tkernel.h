
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
class    TParamS;
class    TModSchedul;

class TKernel
{

/** Public methods: */
public:
    TKernel( );
    ~TKernel(  );

    TGUIS        &GUI()    { return (*gui); }
    TArhiveS     &Arhive() { return (*arhive); } 
    TBDS         &BD()     { return (*bd); }
    TControllerS &Controller() { return (*controller); }
    TProtocolS   &Protocol()   { return (*protocol); }
    TTransportS  &Transport()  { return (*transport); }
    TSpecialS    &Special()    { return (*special); }
    TParamS      &Param()      { return (*param); }
    TModSchedul  &ModSchedul() { return (*modschedul); }
    
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
    TGUIS        *gui;
    TArhiveS     *arhive;
    TBDS         *bd;
    TControllerS *controller;
    TProtocolS   *protocol;
    TTransportS  *transport;
    TSpecialS    *special;
    TParamS      *param;
    TModSchedul  *modschedul;
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
