#ifndef TEST_TCONTR_H
#define TEST_TCONTR_H

#include "../../tmodule.h"

class TTContrTest: public TModule
{
    public:
	TTContrTest(char *name);
	~TTContrTest();
	
	void mod_info( const string & name, string & info );
       	void mod_init( void *param );
	void mod_CheckCommandLine( );	
    public:

    private:
	void pr_opt_descr( FILE * stream );
    private:
	static SExpFunc ExpFuncLc[];	
};

#endif //TEST_TCONTR_H
