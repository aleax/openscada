
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
    virtual void gmd_InitAll( );
    virtual void gmd_DeinitAll( );
    /*
     * Start all modules
     */    
    virtual int gmd_StartAll( ){ return(0); }
    virtual int gmd_StopAll( ){ return(0); }
    /*
     * List moduls
     */
    void gmd_List( vector<string> & moduls ) const;
    /*
     * Convert Name moduls to id into vector!
     */
    unsigned gmd_NameToId(string name) const;
    TModule *gmd_at(unsigned int id) const;
    TModule *operator[](unsigned int id) const
    { return(gmd_at(id)); }

    virtual void gmd_CheckCommandLine( ) = 0;
    virtual void gmd_UpdateOpt() = 0;

    void gmd_CheckCommandLineMods();
    void gmd_UpdateOptMods();
    
    string gmd_NameTMod() { return(NameType); }
    string gmd_ModPath()  { return(DirPath); }
/**Public Attributes: */
public:
//    SNameUser * users;
    TKernel *owner;

/** Protected methods: */
protected:
    unsigned int gmd_Size() const { return(Moduls.size()); } 
    /*
     * Check module 
     */    
    bool gmd_MChk(unsigned int id);
    /*
     * Register how user of function
     */
    TModule *gmd_FUse(unsigned int id, char * func, void (TModule::**offptr)());
    /*
     * Register how user of function
     */    
    void gmd_FFree(unsigned int id, char * func);

    virtual int  gmd_AddM(TModule *modul );
    virtual void gmd_DelM( unsigned hd );

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
