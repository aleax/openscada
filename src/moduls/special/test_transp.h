#ifndef TEST_TRANSP_H
#define TEST_TRANSP_H

#include "../../tspecials.h"

namespace TranspTest
{
    class TTest: public TSpecial
    {
	public:
	    TTest( string name );
	    ~TTest();

	    void Start();

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

#endif //TEST_TRANSP_H
