#ifndef TEST_SPECIAL_H
#define TEST_SPECIAL_H

#include "../gener/tmodule.h"

class TSpecialTest: public TModule
{
    public:
	TSpecialTest(char *name);
	virtual ~TSpecialTest();
	
	virtual	int info( const string & name, string & info );
       	virtual int init( void *param );
	
	void CheckCommandLine(  );
    public:

    private:
	void pr_opt_descr( FILE * stream );
    private:
	static SExpFunc ExpFuncLc[];	
};

#endif //TEST_SPECIAL_H
