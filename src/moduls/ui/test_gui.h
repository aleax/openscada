#ifndef TEST_GUI_H
#define TEST_GUI_H

#include "../../tmodule.h"

class TGUITest: public TModule
{
    public:
	TGUITest(char *name);
	virtual ~TGUITest();
	
	virtual	int info( const string & name, string & info );
       	virtual int init( void *param );
	
	void CheckCommandLine(  );
    public:

    private:
	void pr_opt_descr( FILE * stream );
    private:
	static SExpFunc ExpFuncLc[];	
};

#endif //TEST_GUI_H

