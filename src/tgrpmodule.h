
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
    int	      id_hd;
};

//For a multi moduls declaration into once a shared lib
//No made while!
struct SHD
{
    void    * hd;
    int       use;
    time_t    modif;
    string    path;
};

class TGRPModule
{

/** Public methods: */
public:
     TGRPModule( char * NameT );

     ~TGRPModule(  );

     friend class TModSchedul;
    /**
      * Search and loading all external modul (plugin) 
      * @param NameMod
      *        Modul's name!
      */
    int LoadAll(const string & Paths);

    /**
      * Init all moduls.
      * @param ModObject
      *        A Object's adres for the modul's tip.
      */
    virtual int InitAll( );

    int List( string & moduls );

    int Size() { return(Moduls.size()); }
    /**
      * Registring function.
      * @param addr
      *        Addres for save adress registring function.
      */
//    int RegFunc( string NameFunc, void * addr, string SrcModName, string NameMod );

//    virtual int PutCom(char * NameMod, string command ); // = 0;
//    virtual int PutCom(int  idMod, string command ); // = 0;

// Convert Name moduls to id into vector!
    int name_to_id(string & name);

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
    vector<SHD> SchHD;
    char *DirPath;
//    vector<TModule *> Moduls;
/** Private methods: */
private:
    int  AddShLib( char *name );
    int  RegHDShLb(const void* hd, char *path, time_t modif );
    int  FreeHDshLb(int id);	
    void ScanDir( const string & Paths, string & Mods );
    bool CheckFile(char * name);
    
private:
    char *NameType;
};

#endif // TGRPMODULE_H
