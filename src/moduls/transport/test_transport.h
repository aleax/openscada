#ifndef TEST_TRANSPORT_H
#define TEST_TRANSPORT_H

#include "../../tmodule.h"

class TTransportTest: public TModule
{
    public:
	TTransportTest(char *name);
	~TTransportTest();
	
	void info( const string & name, string & info );
       	void init( void *param );
	void CheckCommandLine(  );
    public:

    private:
	void pr_opt_descr( FILE * stream );
    private:
	static SExpFunc ExpFuncLc[];	
};

#endif //TEST_TRANSPORT_H

