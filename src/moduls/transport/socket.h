#ifndef SOCKET_H
#define SOCKET_H

#include "../../tmodule.h"

class TTransSock: public TModule
{
    public:
	TTransSock(char *name);
	~TTransSock();
	
	void info( const string & name, string & info );
       	void init( void *param );
	void CheckCommandLine( char **argv, int argc );
    public:

    private:
	void pr_opt_descr( FILE * stream );
    private:
	static SExpFunc ExpFuncLc[];	
};

#endif //SOCKET_H

