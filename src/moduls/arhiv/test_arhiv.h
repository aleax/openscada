#ifndef TEST_ARHIV_H
#define TEST_ARHIV_H

#include "../../tmodule.h"

class TArhivTest: public TModule
{
    public:
	TArhivTest(char *name);
	~TArhivTest();
	
	void info( const string & name, string & info );
       	void init( void *param );

	void CheckCommandLine( char **argv, int argc );
    public:

    private:
	void pr_opt_descr( FILE * stream );
    private:
	static SExpFunc ExpFuncLc[];
};

#endif //TEST_ARHIV_H

