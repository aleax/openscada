
#ifndef TGRPMODULE_H
#define TGRPMODULE_H

#include <string>
#include <vector>

class TModule;
class TModSchedul;

struct SModul
{
    TModule * modul;
    void    * hd;
    int       resource;
    int       access;
    string  * path;
    time_t    modif;
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

    string List(  );

    /**
      * Registring function.
      * @param addr
      *        Addres for save adress registring function.
      */
    int RegFunc( string NameFunc, void * addr, string SrcModName, string NameMod );

    virtual int PutCom(char * NameMod, string command ); // = 0;
    virtual int PutCom(int  idMod, string command ); // = 0;

    virtual bool AddM(char *name);

// Convert Name moduls to id into vector!
    int name_to_id(string & name);

/**Attributes: */
public:
//    SNameUser * users;

/** Protected methods: */
protected:
//    virtual void AddM(TModule & module);

protected:
    vector<SModul *> Moduls;
    char *DirPath;
//    vector<TModule *> Moduls;
/** Private methods: */
private:
    void ScanDir( const string & Paths, string & Mods );
    bool CheckFile(char * name);
private:
    char *NameType;
};

#endif // TGRPMODULE_H
