#ifndef TEST_KERNEL_H
#define TEST_KERNEL_H

#include "../../tspecials.h"

namespace KernelTest
{
    class TTest: public TSpecial
    {
	public:
	    TTest(char *name);
	    ~TTest();

	    void Start( );

	    void mod_CheckCommandLine( );
	    void mod_UpdateOpt();
	private:
	    void pr_opt_descr( FILE * stream );	
	    string mod_info( const string name );
	    void   mod_info( vector<string> &list );
	private:
	    static SExpFunc ExpFuncLc[];	
    };    
}

#endif //TEST_KERNEL_H
