#ifndef TEST_TCONTR_H
#define TEST_TCONTR_H

#include "../../tmodule.h"

class TTContrTest: public TModule
{
    public:
	TTContrTest(char *name);
	~TTContrTest();
	
	void info( const string & name, string & info );
       	void init( void *param );
	void CheckCommandLine( char **argv, int argc );	
    public:

    private:
	void pr_opt_descr( FILE * stream );
    private:
	static SExpFunc ExpFuncLc[];	
};

#endif //TEST_TCONTR_H

