//#include <sys/types.h>
//#include <sys/wait.h>

//#include <syslog.h>
//#include <stdarg.h>
//#include <stdio.h>
//#include <unistd.h>
//#include <signal.h>
//#include <iconv.h>

#include "terror.h"
#include "tsys.h"

const char *TSYS::o_name = "TSYS";

TSYS::TSYS(  )
{
    
}

TSYS::~TSYS(  )
{
    
}

unsigned TSYS::ResCreate( unsigned val )
{
    unsigned i_sem;
    
    for(i_sem = 0; i_sem < sems.size(); i_sem++)
	if(sems[i_sem] == TO_FREE) break;
    if( i_sem == sems.size() ) sems.push_back();
    sems[i_sem] = new sem_t;
    if( sem_init(sems[i_sem],0,val) != 0 )
    {
	delete sems[i_sem]; sems[i_sem] = TO_FREE;
	throw TError("%s: error open semaphor!", o_name);
    }

    return(i_sem);
}

void TSYS::ResDelete( unsigned res )
{
    if(res >= sems.size() || sems[res] == TO_FREE)
	throw TError("%s: error semaphor id = %d!", o_name, res);
    sem_destroy(sems[res]);
    delete sems[res]; 
    sems[res] = TO_FREE;
}

void TSYS::ResRequest( unsigned res, long tm )
{
    if(res >= sems.size() || sems[res] == TO_FREE)
	throw TError("%s: error semaphor id = %d!", o_name, res);
    sem_wait(sems[res]);
}

void TSYS::ResRelease( unsigned res )
{
    if(res >= sems.size() || sems[res] == TO_FREE)
	throw TError("%s: error semaphor id = %d!", o_name, res);
    sem_post(sems[res]);
}



