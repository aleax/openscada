#ifndef TEST_PROTOCOL_H
#define TEST_PROTOCOL_H

#include "../gener/tmodule.h"

class TProtocolTest: public TModule
{
    public:
	TProtocolTest(char *name);
	virtual ~TProtocolTest();
	
	virtual	int info( const string & name, string & info );
       	virtual int init( void *param );
	
	void CheckCommandLine(  );
    public:

    private:
	void pr_opt_descr( FILE * stream );
    private:
	static SExpFunc ExpFuncLc[];	
};

#endif //TEST_PROTOCOL_H

