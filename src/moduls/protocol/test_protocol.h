#ifndef TEST_PROTOCOL_H
#define TEST_PROTOCOL_H

#include "../../tmodule.h"

class TProtocolTest: public TModule
{
    public:
	TProtocolTest(char *name);
	~TProtocolTest();
	
	void mod_info( const string & name, string & info );
       	void mod_init( void *param );
	void mod_CheckCommandLine( );
    public:

    private:
	void pr_opt_descr( FILE * stream );
    private:
	static SExpFunc ExpFuncLc[];	
};

#endif //TEST_PROTOCOL_H

