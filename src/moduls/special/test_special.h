#ifndef TEST_SPECIAL_H
#define TEST_SPECIAL_H

#include "../../tmodule.h"

class TSpecialTest: public TModule
{
    public:
	TSpecialTest(char *name);
	~TSpecialTest();
	
	void mod_info( const string & name, string & info );
       	void mod_init( void *param );
	void mod_CheckCommandLine( );
    public:

    private:
	void pr_opt_descr( FILE * stream );
    private:
	static SExpFunc ExpFuncLc[];	
};

#endif //TEST_SPECIAL_H
