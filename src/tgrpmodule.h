
#ifndef TGRPMODULE_H
#define TGRPMODULE_H

#define GRM_ST_FREE  0  //modul cell of free  
#define GRM_ST_OCCUP 1  //modul cell of occupied   

#include <string>
using std::string;
#include <vector>
using std::vector;

class TModule;
class TModSchedul;

struct SModul
{
    int       stat;
    string    name;
    TModule * modul;
//    int	      id_hd;
};

class TGRPModule
{

/** Public methods: */
public:
     TGRPModule( char * NameT );

     ~TGRPModule(  );

     friend class TModSchedul;
    /**
      * Init all moduls.
      * @param ModObject
      *        A Object's adres for the modul's tip.
      */
    virtual int InitAll( );
    /*
     * Start all modules
     */    
    virtual int StartAll( ){ };

    virtual int StopAll( ){ };
    /*
     * List moduls
     */
    void List( vector<string> & moduls );

    int Size() { return(Moduls.size()); }
    /*
     * Convert Name moduls to id into vector!
     */
    int name_to_id(string & name);

    virtual void CheckCommandLine() = 0;

    string NameTMod() { return(NameType); }
    char *ModPath()   { return(DirPath); }
/**Attributes: */
public:
//    SNameUser * users;

/** Protected methods: */
protected:
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
    int FFree(unsigned int id, char * func);

    virtual int AddM(TModule *modul );
    virtual int DelM( int hd );

protected:
    vector<SModul> Moduls;
    char           *DirPath;
//    vector<TModule *> Moduls;
/** Private methods: */
private:
    
private:
    char *NameType;
};

#endif // TGRPMODULE_H
