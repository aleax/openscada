#ifndef TEST_TRANSPORT_H
#define TEST_TRANSPORT_H

#include "../../tmodule.h"

class TTransportTest: public TModule
{
    public:
	TTransportTest(char *name);
	~TTransportTest();
	
	void mod_info( const string & name, string & info );
       	void mod_init( void *param );
	void mod_CheckCommandLine(  );
    public:

    private:
	void pr_opt_descr( FILE * stream );
    private:
	static SExpFunc ExpFuncLc[];	
};

#endif //TEST_TRANSPORT_H

