#ifndef TEST_GUI_H
#define TEST_GUI_H

#include "../../tmodule.h"

class TGUITest: public TModule
{
    public:
	TGUITest(char *name);
	~TGUITest();
	
	void mod_info( const string & name, string & info );
       	void mod_init( void *param );
	void mod_CheckCommandLine( );
    public:

    private:
	void pr_opt_descr( FILE * stream );
    private:
	//static SExpFunc ExpFuncLc[];	
};

#endif //TEST_GUI_H

