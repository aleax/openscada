
#ifndef TGRPMODULE_H
#define TGRPMODULE_H

#define GRM_ST_FREE  0  //modul cell of free  
#define GRM_ST_OCCUP 1  //modul cell of occupied   

#include <string>
using std::string;
#include <vector>
using std::vector;

#include "terror.h"

class TModule;
class TModSchedul;
class TKernel;

struct SModul
{
    char      stat;
    string    name;
    TModule * modul;
};

class TGRPModule
{

/** Public methods: */
public:
    TGRPModule( TKernel *app, char * NameT );

    virtual ~TGRPModule(  );

    friend class TModSchedul;
    
    virtual void ConnectAll( ){ }
    /**
      * Init all moduls.
      * @param ModObject
      *        A Object's adres for the modul's tip.
      */
    virtual void InitAll( );
    virtual void DeinitAll( );
    /*
     * Start all modules
     */    
    virtual int StartAll( ){ return(0); }
    virtual int StopAll( ){ return(0); }
    /*
     * List moduls
     */
    void List( vector<string> & moduls ) const;
    /*
     * Convert Name moduls to id into vector!
     */
    unsigned NameToId(string name) const;
    TModule *at(unsigned int id) const 
    { if(Moduls[id].stat == GRM_ST_OCCUP) return(Moduls[id].modul); else throw TError("%s: module id error!",o_name); }

    virtual void CheckCommandLine( char **argv, int argc ) = 0;
    virtual void UpdateOpt() = 0;

    void CheckCommandLineMods();
    void UpdateOptMods();
    
    string NameTMod() { return(NameType); }
    string ModPath()  { return(DirPath); }
/**Attributes: */
public:
//    SNameUser * users;
    TKernel *owner;

/** Protected methods: */
protected:
    unsigned int Size() const { return(Moduls.size()); } 
    /*
     * Check module 
     */    
    bool MChk(unsigned int id)
    { if(id >= Size() || Moduls[id].stat != GRM_ST_OCCUP ) return(true); else return(false); }
    /*
     * Register how user of function
     */
    TModule *FUse(unsigned int id, char * func, void (TModule::**offptr)());
    /*
     * Register how user of function
     */    
    void FFree(unsigned int id, char * func);

    virtual int AddM(TModule *modul );
    virtual int DelM( unsigned hd );

protected:
    vector<SModul> Moduls;
    string         DirPath;
//    vector<TModule *> Moduls;
/** Private methods: */
private:
    
private:
    char              *NameType;
    static const char *o_name;
};

#endif // TGRPMODULE_H
