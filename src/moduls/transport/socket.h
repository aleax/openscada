#ifndef SOCKET_H
#define SOCKET_H

#include "../../tmodule.h"
#include "../../ttransports.h"


class TSocketIn: public TTransportIn
{
    public:
    	TSocketIn(string name, string address) 
	    : TTransportIn(name,address) { };
	//~TSocketIn();
};

class TSocketOut: public TTransportOut
{
    public:
    	TSocketOut(string name, string address) 
	    : TTransportOut(name,address) { };
	//~TSocketOut();
};

class TTransSock: public TTipTransport
{
    public:
	TTransSock(char *name);
	~TTransSock();
        
	TTransportIn  *In(string name, string address );
	TTransportOut *Out(string name, string address );	    
	
	void CheckCommandLine( char **argv, int argc );
	void UpdateOpt();	
    public:

    private:
	
	void pr_opt_descr( FILE * stream );
    private:
	static SExpFunc ExpFuncLc[];	
};

#endif //SOCKET_H

