#ifndef TEST_SPECIAL_H
#define TEST_SPECIAL_H

#include "../../tmodule.h"

class TSpecialTest: public TModule
{
    public:
	TSpecialTest(char *name);
	~TSpecialTest();
	
	void info( const string & name, string & info );
       	void init( void *param );
	void CheckCommandLine( char **argv, int argc );
    public:

    private:
	void pr_opt_descr( FILE * stream );
    private:
	static SExpFunc ExpFuncLc[];	
};

#endif //TEST_SPECIAL_H
