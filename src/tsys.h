#ifndef TSYS_H
#define TSYS_H

#define TO_FREE         NULL // Object free

#include <semaphore.h>

#include <string>
using std::string;
#include <vector>
using std::vector;

class TSYS
{
    /** Public methods: */
    public:
	TSYS(  );
	~TSYS(  );
    
	/** Semaphores/Resources **/
	unsigned ResCreate( unsigned val = 1 );
	void ResDelete( unsigned res );
    
	void ResRequest( unsigned res, long tm = 0 );
        void ResRelease( unsigned res );
	
    /** Private atributes: */
    private:
	/** Semaphores/Resources **/
	vector<sem_t *>  sems;

	static const char *o_name;    
};

extern TSYS *SYS;

#endif // TSYS_H
