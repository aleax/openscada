
#ifndef TMODSCHEDUL_H
#define TMODSCHEDUL_H

#include <pthread.h>

class TModSchedul 
{

/** Public methods: */
public:
    TModSchedul(  );

    ~TModSchedul(  );

    void Start(  );

    bool FindNewMod(string & Mods );
    
    void UpdateMod(string & Mods );

/** Private methods: */
private:
    static void *SchedTask(void *param);
private:
    pthread_t pthr_tsk;
    bool work;
};

#endif // TMODSCHEDUL_H

