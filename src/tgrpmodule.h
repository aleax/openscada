
#ifndef TGRPMODULE_H
#define TGRPMODULE_H

#include <string>
using std::string;
#include <vector>
using std::vector;

#include "terror.h"
#include "xml.h"

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
      * Init group modules.
      */
    virtual void gmd_Init( ) { }
    /*
     * Start group modules
     */    
    virtual void gmd_Start( ) { }
    virtual void gmd_Stop( ) { }
    /*
     * List moduls
     */
    void gmd_List( vector<string> & moduls ) const;
    /*
     * Convert Name moduls to id into vector!
     */
    unsigned gmd_NameToId(string name) const;
    TModule &gmd_at(unsigned int id) const;
    TModule &operator[](unsigned int id) const
    { return(gmd_at(id)); }

    virtual void gmd_CheckCommandLine( );
    virtual void gmd_UpdateOpt();

    void gmd_CheckCommandLineMods();
    void gmd_UpdateOptMods();
    /*
     * Get XML section node
     */
    XMLNode *gmd_XMLCfgNode();
    
    string gmd_Name()    { return(NameType); }
    string gmd_ModPath() { return(DirPath); }
    
    TKernel &Owner() { return(*owner); }
/**Public Attributes: */
public:
//    SNameUser * users;

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
    TKernel           *owner;
    
    char              *NameType;
    static const char *o_name;
};

#endif // TGRPMODULE_H
