/* Test Modul
** ==============================================================
*/

#ifndef TEST_BD_H
#define TEST_BD_H

#include <string>
using std::string;

#include "../../tmodule.h"

class TBDtest: public TModule
{
    public:
	TBDtest(char *name);
	~TBDtest();
	
	void info( const string & name, string & info );
       	void init( void *param );

	void CheckCommandLine(  );
	int OpenBD( string name );
	int CloseBD( int hd );
    public:
    private:
	void pr_opt_descr( FILE * stream );
    private:
	static SExpFunc ExpFuncLc[];
};

#endif // TEST_BD_H

