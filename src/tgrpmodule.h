
#ifndef TGRPMODULE_H
#define TGRPMODULE_H

#include <string>
using std::string;
#include <vector>
using std::vector;

#include "terror.h"

class TModule;
class TModSchedul;
class TKernel;

class TGRPModule
{

/** Public methods: */
public:
    TGRPModule( TKernel *app, char * NameT );

    virtual ~TGRPModule(  );

    friend class TModSchedul;
    
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
    TModule *at(unsigned int id) const;

    virtual void CheckCommandLine( ) = 0;
    virtual void UpdateOpt() = 0;

    void CheckCommandLineMods();
    void UpdateOptMods();
    
    string NameTMod() { return(NameType); }
    string ModPath()  { return(DirPath); }
/**Public Attributes: */
public:
//    SNameUser * users;
    TKernel *owner;

/** Protected methods: */
protected:
    unsigned int Size() const { return(Moduls.size()); } 
    /*
     * Check module 
     */    
    bool MChk(unsigned int id);
    /*
     * Register how user of function
     */
    TModule *FUse(unsigned int id, char * func, void (TModule::**offptr)());
    /*
     * Register how user of function
     */    
    void FFree(unsigned int id, char * func);

    virtual int AddM(TModule *modul );
    virtual void DelM( unsigned hd );

/** Protected Attributes: */
protected:
    vector<TModule *> Moduls;
    string            DirPath;
/** Private methods: */
private:
    
/** Private Attributes: */
private:
    char              *NameType;
    static const char *o_name;
};

#endif // TGRPMODULE_H
