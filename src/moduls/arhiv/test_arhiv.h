#ifndef TEST_ARHIV_H
#define TEST_ARHIV_H

#include "../gener/tmodule.h"

class TArhivTest: public TModule
{
    public:
	TArhivTest(char *name);
	virtual ~TArhivTest();
	
	virtual	int info( const string & name, string & info );
       	virtual int init( );
	
	void CheckCommandLine(  );
    public:

    private:
	void pr_opt_descr( FILE * stream );
    private:
	static SExpFunc ExpFuncLc[];
};

#endif //TEST_ARHIV_H

