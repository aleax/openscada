
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
    int       resource;
    int       access;
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
    int InitAll( );

    int List( string & moduls );

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
     * Register modul using 
     */    
    int MUse(unsigned int id);
    int MUse(unsigned int id, char * func, void (TModule::**offptr)());
    /*
     * Unregister modul using 
     */    
    int MFree(unsigned int id);

    virtual int AddM(TModule *modul );
    virtual int DelM( int hd );

protected:
    vector<SModul *> Moduls;
    vector<SHD *> SchHD;
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
