
#ifndef TMODSCHEDUL_H
#define TMODSCHEDUL_H

#include <pthread.h>

class TGRPModule;

class TModSchedul 
{

/** Public methods: */
public:
    TModSchedul(  );

    ~TModSchedul(  );

    void Start(  );
    /*
     * Register group moduls
     */
    int RegGroupM( TGRPModule *gmod );
    /*
     * Unregister group moduls
     */ 
    int UnRegGroupM( TGRPModule *gmod );
    /* 
     * Find new moduls 
     */ 
    bool FindNewMod( string & Mods  );
    
    void UpdateMod( string & Mods );

/** Private methods: */
private:
    static void *SchedTask(void *param);
private:
    vector<TGRPModule *> grpmod; 
    pthread_t pthr_tsk;
    bool work;
};

#endif // TMODSCHEDUL_H

