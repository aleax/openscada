#ifndef TEST_TCONTR_H
#define TEST_TCONTR_H

#include "../gener/tmodule.h"

class TVirtual: public TModule
{
    public:
	TVirtual(char *name);
	virtual ~TVirtual();
	
	virtual	int info( const string & name, string & info );
       	virtual int init( );
	
	void CheckCommandLine(  );
    public:

    private:
	void pr_opt_descr( FILE * stream );
    private:
	static SExpFunc ExpFuncLc[];	
};

#endif //TEST_TCONTR_H

