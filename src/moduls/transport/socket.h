#ifndef SOCKET_H
#define SOCKET_H

#include "../../tmodule.h"
#include "../../ttransports.h"

/*
class TSocket: public 
{
    public:
    	TSocket();
	~TSocket();
    
};
*/

class TTransSock: public TTipTransport
{
    public:
	TTransSock(char *name);
	~TTransSock();
	
	void CheckCommandLine( char **argv, int argc );
	void UpdateOpt();	
    public:

    private:
	void pr_opt_descr( FILE * stream );
    private:
	static SExpFunc ExpFuncLc[];	
};

#endif //SOCKET_H

