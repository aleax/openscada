#ifndef TEST_PROTOCOL_H
#define TEST_PROTOCOL_H

#include "../../tmodule.h"

class TProtocolTest: public TModule
{
    public:
	TProtocolTest(char *name);
	~TProtocolTest();
	
	void info( const string & name, string & info );
       	void init( void *param );
	
	void CheckCommandLine(  );
    public:

    private:
	void pr_opt_descr( FILE * stream );
    private:
	static SExpFunc ExpFuncLc[];	
};

#endif //TEST_PROTOCOL_H

