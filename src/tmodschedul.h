
#ifndef TMODSCHEDUL_H
#define TMODSCHEDUL_H

#include <pthread.h>

//For a multi moduls declaration into once a shared lib
struct SUse
{
    int id_tmod;
    int id_mod;
};

struct SHD
{
    void         *hd;
    vector<SUse> use;
    time_t       modif;
    string       path;
};

class TGRPModule;

class TModSchedul 
{

/** Public methods: */
public:
    TModSchedul( TKernel *app );

    ~TModSchedul(  );
    /*
     * Check command line all TGRPModules
     */ 
    void CheckCommandLine(  );
    void CheckCommandLineMod(  );
    /*
     * Update options from generic config file
     */
    void UpdateOpt();
    void UpdateOptMod();
    /*
     * Load all share libs and registry moduls into TGRPModule
     */
    void LoadAll(  );
    /*
     * Load share libs for <dest> from <path>
     */
    void Load( string path, int dest);

    void InitAll(  );
    
    void DeinitAll(  );

    void StartAll(  );
    
    void StartSched(  );
    /*
     * Register group moduls
     */
    int RegGroupM( TGRPModule *gmod );
    /*
     * Unregister group moduls
     */ 
    int UnRegGroupM( TGRPModule *gmod );

    TKernel &Owner() { return(*owner); }

public:
/** Private methods: */
private:
    static void *SchedTask(void *param);
    void ScanDir( const string & Paths, string & Mods );
    bool CheckFile(char * name, bool new_f);
    int  AddShLib( char *name, int dest );

    int  RegMod_ShLb(const void* hd, char *path, time_t modif, int id_tmod, int id_mod );
    int  UnRegMod_ShLb(int id_tmod, int id_mod);    
private:
    TKernel  		 *owner;
    
    vector<TGRPModule *> grpmod; 
    vector<SHD>          SchHD;
    pthread_t            pthr_tsk;
    bool                 work;
};

#endif // TMODSCHEDUL_H

